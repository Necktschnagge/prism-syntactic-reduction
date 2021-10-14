
#include "logger.h"

#include "parser.h"
#include "debug_001.h"

#include <nlohmann/json.hpp>

#include <boost/multiprecision/cpp_int.hpp>


#include <memory>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <limits>
#include <future>
#include <sstream>
#include <bitset>
#include <algorithm>
#include <execution>
#include <locale>
#include <codecvt>

using grouping_enemies_table = std::vector<std::set<std::size_t>>; /// [var_id_x] |-> { var_id_y | var_id_x and var_id_y cannot be merged }

bool __compare_helper__(std::map<std::string, int>& c1, std::map<std::string, int>& c2) {
	std::map<int, int> homomorphism;
	for (auto& pair : c1) {
		auto right_value = c2[pair.first];
		auto [iter, insert_took_place] = homomorphism.insert(std::make_pair(pair.second, right_value));
		if (!insert_took_place) {
			if (homomorphism[pair.second] != right_value) {
				return false;
			}
		}
	}
	return true;
}

bool compare_colorings(std::map<std::string, int>& c1, std::map<std::string, int>& c2) {

	return __compare_helper__(c1, c2) && __compare_helper__(c2, c1);
}

void print_model_to_stream(const file_token& reduced_file, std::ostream& ofile) {
	token::token_list to_be_printed = reduced_file.children();
	while (!to_be_printed.empty()) {
		const auto element = to_be_printed.front();
		to_be_printed.pop_front();
		if (element->is_primitive()) {
			ofile << element->str();
		}
		else {
			const auto got_children = element->children();
			to_be_printed.insert(to_be_printed.begin(), got_children.begin(), got_children.end());
		}
	}
}


// state of cf variables |-> (must be recalculated, variables that are live there (current state), changes since last calculation)
using liveness_tuple = std::tuple<bool, std::vector<std::string>, std::vector<std::string>>;
using live_var_map = std::map<int, liveness_tuple>;
const auto flag_of_liveness_tuple = [](liveness_tuple& tup) -> bool& { return std::get<0>(tup); };
const auto current_of_liveness_tuple = [](liveness_tuple& tup) -> std::vector<std::string>&{ return std::get<1>(tup); };
const auto changes_of_liveness_tuple = [](liveness_tuple& tup) -> std::vector<std::string>&{ return std::get<2>(tup); };


const auto is_active = [](std::tuple<bool, int, std::set<std::string>, int>& t) -> bool& { return std::get<0>(t); };
const auto count_active_neighbours = [](std::tuple<bool, int, std::set<std::string>, int>& t) -> int& { return std::get<1>(t); };
const auto neighbours = [](std::tuple<bool, int, std::set<std::string>, int>& t) -> std::set<std::string>&{ return std::get<2>(t); };
//const auto color = [](std::tuple<bool, int, std::set<std::string>, int>& t) -> int& { return std::get<3>(t); };
auto color(std::tuple<bool, int, std::set<std::string>, int>& t) -> int& {
	return std::get<3>(t);
}
auto color(const std::tuple<bool, int, std::set<std::string>, int>& t) -> int {
	return std::get<3>(t);
}

void apply_coloring_to_file_token(file_token& reduced_file, const std::string& var_name, const std::map<std::string, int>& const_table, live_var_map& live_vars, const std::map<std::string, std::tuple<bool, int, std::set<std::string>, int>>& graph) {

	// transform the init condition:
	std::list<std::shared_ptr<init_definition_token>> list_of_init_defs = reduced_file._dtmc_body_component->init_definitions();
	// should be size 1
	std::shared_ptr<init_definition_token> the_init_def = list_of_init_defs.front();

	std::shared_ptr<std::vector<int>> all_cf_init_states = the_init_def->_start_condition->get_values(var_name, const_table);

	std::vector<int> all_init_cf_values = *all_cf_init_states;

	std::vector<std::pair<std::shared_ptr<condition_token>, int>> new_conditions;

	for (const auto& value : all_init_cf_values) {
		new_conditions.push_back(std::make_pair(copy_shared_ptr(the_init_def->_start_condition), value));
	}

	for (auto& pair : new_conditions) {
		// f |-> (cf=XXX) & f 
		std::shared_ptr<condition_token> embrace_condition = std::make_shared<condition_token>(condition_token::type::SUB_CONDITION, std::make_shared<std::string>());
		const auto brace_l = std::make_shared<std::string>("(");
		const auto brace_r = std::make_shared<std::string>(")");
		const auto space = std::make_shared<std::string>(" ");
		embrace_condition->_leading_tokens.push_back(std::make_shared<space_token>(space, space->begin(), space->end()));
		embrace_condition->_leading_tokens.push_back(std::make_shared<left_brace_token>(brace_l, brace_l->begin(), brace_l->end()));
		embrace_condition->_sub_conditions.push_back(std::make_pair(pair.first, nullptr));
		embrace_condition->_trailing_tokens.push_back(std::make_shared<right_brace_token>(brace_r, brace_r->begin(), brace_r->end()));
		const auto equation_text = std::make_shared<std::string>(std::string(" (") + var_name + "=" + std::to_string(pair.second) + ") ");
		std::shared_ptr<condition_token> cf_state = std::make_shared<condition_token>(equation_text, equation_text->begin(), equation_text->end());
		cf_state->parse();
		std::shared_ptr<condition_token> appended_state = std::make_shared<condition_token>(condition_token::type::AND, std::make_shared<std::string>());
		const auto and_symbol = std::make_shared<std::string>("&");
		appended_state->_sub_conditions.push_back(std::make_pair(cf_state, std::make_shared<and_token>(and_symbol, and_symbol->begin(), and_symbol->end())));
		appended_state->_sub_conditions.push_back(std::make_pair(embrace_condition, nullptr));
		// remove all not live conditions:
		const auto& vector_of_live_vars = current_of_liveness_tuple(live_vars[pair.second]);
		const auto recursively_remove_deads = [&](std::shared_ptr<condition_token> token) -> std::list<std::shared_ptr<condition_token>> {
			std::list<std::shared_ptr<condition_token>> remaining;
			if (token->_type == condition_token::type::EQUATION) {
				std::vector<std::string> vars = token->_equation->all_variables(const_table);
				if (vars.size() > 0) { //ignored case of more than  variable
					auto search = std::find(vector_of_live_vars.cbegin(), vector_of_live_vars.cend(), vars[0]);
					if (search == vector_of_live_vars.cend()) {
						std::shared_ptr<std::string> true_equation = std::make_shared<std::string>("1=1");
						token->_equation = std::make_shared<equation_token>(true_equation, true_equation->begin(), true_equation->end());
						token->_equation->parse();
					}
				}
			}
			else {
				for (const auto& sub_pair : token->_sub_conditions) {
					remaining.push_back(sub_pair.first);
				}
			}
			return remaining;
		};

		std::list<std::shared_ptr<condition_token>> remaining;
		remaining.push_back(appended_state);
		while (!remaining.empty()) {
			const auto subs = recursively_remove_deads(remaining.front());
			remaining.pop_front();
			remaining.insert(remaining.end(), subs.begin(), subs.end());
		}

		pair.first = appended_state;
	}
	if (new_conditions.size() == 1) {
		list_of_init_defs.front()->_start_condition = new_conditions.front().first;
	}
	else {
		std::shared_ptr<condition_token> new_condition = std::make_shared<condition_token>(condition_token::type::OR, std::make_shared<std::string>("dummy"));
		for (auto& pair : new_conditions) {
			const auto or_symbol = std::make_shared<std::string>("|");
			new_condition->_sub_conditions.push_back(std::make_pair(pair.first, std::make_shared<or_token>(or_symbol, or_symbol->begin(), or_symbol->end())));
		}
		new_condition->_sub_conditions.back().second = nullptr; // remove trailing |
		list_of_init_defs.front()->_start_condition = new_condition;
	}

	//#if false
	// replace colored things:

	standard_logger().info("apply coloring to var names");

	const auto top_level_children_72954 = reduced_file.children();

	const std::function<void(token::token_list)> apply_coloring = [&](token::token_list children_list) {
		while (!children_list.empty()) {
			const std::shared_ptr<token> current_child = children_list.front();
			children_list.pop_front();

			if (current_child->is_primitive()) {
				if (dynamic_cast<identifier_token*>(current_child.get())) {
					auto tokenptr = dynamic_cast<identifier_token*>(current_child.get());
					const auto entry = graph.find(current_child->str());
					if (entry != graph.end()) {
						std::string new_name = std::string("colored_") + std::to_string(color(entry->second));
						tokenptr->modify_string(new_name);
					}
					else {
						const auto entry = graph.find(current_child->str().substr(0, current_child->str().length() - 1));
						if (entry != graph.end()) {
							std::string new_name = std::string("colored_") + std::to_string(color(entry->second)) + "'";
							tokenptr->modify_string(new_name);
						}
					}
				}
			}
			else {
				auto got_children = current_child->children();
				children_list.insert(children_list.end(), got_children.begin(), got_children.end());
			}

		}
	};
	apply_coloring(top_level_children_72954);

	// remove multiple declarations:
	auto& got_children = reduced_file._dtmc_body_component->local_children;

	std::list<token::token_list::iterator> global_definition_tokens;
	for (auto iter = got_children.begin(); iter != got_children.end(); ++iter) {
		if (std::dynamic_pointer_cast<global_definition_token>(*iter)) {
			global_definition_tokens.push_back(iter);
		}
	}
	for (
		auto i_iterator = global_definition_tokens.begin();
		i_iterator != global_definition_tokens.end() /*&& std::next(i_iterator) != global_definition_tokens.end()*/;
		++i_iterator
		) {
		std::shared_ptr<global_definition_token> gdef = std::dynamic_pointer_cast<global_definition_token>(**i_iterator);
		std::string var_name = gdef->_global_identifier->str();
		for (auto jter = std::next(i_iterator); jter != global_definition_tokens.end();) {
			std::shared_ptr<global_definition_token> compare = std::dynamic_pointer_cast<global_definition_token>(**jter);
			if (compare->_global_identifier->str() == var_name) {
				if (compare->_lower_bound->get_ll() < gdef->_lower_bound->get_ll()) {
					gdef->_lower_bound->modify(compare->_lower_bound->str());
				}
				if (compare->_upper_bound->get_ll() > gdef->_upper_bound->get_ll()) {
					gdef->_upper_bound->modify(compare->_upper_bound->str());
				}
				got_children.erase(*jter); // remove the token from the parse tree
				auto del = jter;
				++jter;
				global_definition_tokens.erase(del); // remove entry from our todo list
			}
			else {
				++jter;
			}
		}
	}
	//#endif

}



void print_coloring_from_graph_with_color_annotations(const std::map<std::string, std::tuple<bool, int, std::set<std::string>, int>>& graph, std::ostream& out) {
	// output result:
	out << "// ::::: color map :::::\n//\n//  ** var -> color **\n//\n";
	for (auto& graph_pair : graph) {
		out << "// Color " << color(graph_pair.second) << " for var " << graph_pair.first << std::endl;
	}
	out << "//\n//  ** color -> set of variables **\n//\n";
	for (int i{ 0 }; i < std::numeric_limits<int>::max(); ++i) {
		std::vector<std::string> var_names;
		for (auto& graph_pair : graph) {
			if (color(graph_pair.second) == i) var_names.push_back(graph_pair.first);
		}
		if (var_names.empty()) break;
		out << "// Variables for color " << i << " :\n//\t";
		for (const auto& s : var_names) out << s << ", ";
		out << "\n";
	}

}


void starke_coloring(std::map<std::string, std::tuple<bool, int, std::set<std::string>, int>>& graph) {
	// find a coloring
	std::vector<std::string/*std::pair<std::string, std::set<std::string>>*/ > removed_nodes;

	while (true) {
		// while there are nodes, select one with min incidence:
		auto selected{ graph.end() };
		int min_seen{ std::numeric_limits<int>::max() };
		for (auto iter{ graph.begin() }; iter != graph.end(); ++iter) {
			if (is_active(iter->second)) {
				int current_incidence{ count_active_neighbours(iter->second) };
				if (current_incidence < min_seen) {
					selected = iter;
					min_seen = current_incidence;
				}
			}
		}
		if (selected == graph.end()) break; // all removed
		// remove selected node:
		const std::string& node_name{ selected->first };
		// remove backward edges:
		for (const auto& incident_node_name : neighbours(selected->second)) {
			if (is_active(graph[incident_node_name])) {
				--count_active_neighbours(graph[incident_node_name]);
			}
		}
		// remove the node itself
		removed_nodes.push_back(node_name);
		is_active(graph[node_name]) = false;
	}

	// iterate list backwards and insert the smallest color not used by active neighbours:
	for (auto ri{ removed_nodes.rbegin() }; ri != removed_nodes.rend(); ++ri) {
		// select color
		std::unordered_set<int> excluded;
		auto& current_tuple{ graph[*ri] };
		for (const auto& incident_node_name : neighbours(current_tuple)) {
			if (is_active(graph[incident_node_name]))
				excluded.insert(color(graph[incident_node_name]));
		}
		int c{ 0 };
		for (; c < std::numeric_limits<int>::max(); ++c) {
			if (excluded.find(c) == excluded.end())
				break;
		}
		color(current_tuple) = c;
		// reactivate
		is_active(graph[*ri]) = true;
	}

}

std::string to_string(const std::set<std::string>& s) {
	std::string result;
	for (const auto& el : s) result += std::string("  ") + el;
	return result;
}


using collapse_graph_t = std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>;

struct temporal_prohibiting_edge {
	std::shared_ptr<std::set<std::string>> key_set_ptr1;
	//std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>::iterator iter1;
	std::shared_ptr<std::set<std::string>> key_set_ptr2;
	//std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>::iterator iter2;
	bool can_be_joined{ false };
};

struct temporal_join {
	std::shared_ptr<std::set<std::string>> deleted_set;
	std::set<std::shared_ptr<std::set<std::string>>> deleted_set_neighbours;
	std::set<std::shared_ptr<std::set<std::string>>> neighbours_of_set1_before;
	std::list<std::shared_ptr<std::set<std::string>>> relink_to_set2_on_rollback_and_delete_set1_link;
	std::list<std::shared_ptr<std::set<std::string>>> relink_to_set2_on_rollback_and_keep_set1_link;


	//std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>> collapse_graph_debug_copy;
};

enum class PHASE {
	SEARCH_MERGING = 0,
	FORBID_MERGING = 1,
	UNDO_FORBID_MERGING = 2,
	FORCE_MERGING = 3,
	UNDO_MERGING = 4,
	FINISHED = 5

};

const auto next_phase = [](const PHASE& p) -> PHASE {
	switch (p)
	{
	case PHASE::SEARCH_MERGING: return PHASE::FORCE_MERGING;
	case PHASE::FORBID_MERGING: return PHASE::UNDO_FORBID_MERGING;
	case PHASE::FORCE_MERGING: return PHASE::UNDO_MERGING;
	case PHASE::UNDO_MERGING: return PHASE::FORBID_MERGING;
	default:
		return PHASE::FINISHED;
	}
};

struct activation_record {
	bool skip_output;

	std::string searched_set1_representor;
	std::string searched_set2_representor;

	temporal_prohibiting_edge temp_edge;
	temporal_join temp_join;
	PHASE phase;
	bool kill_on_end;
	activation_record(bool skip_output, PHASE phase = PHASE::SEARCH_MERGING) : skip_output(skip_output), phase(phase), kill_on_end(false) {}
};

auto deep_copy_collapse_graph(const std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>& collapse_graph)
-> std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>> {
	/*std::map<std::string, std::tuple<std::set<std::string>, std::set<std::string>>> without_shared_pointers;
	for (const auto& pair : collapse_graph) {
		std::get<0>(without_shared_pointers[*pair.first->begin()]) = std::set<std::string>(*pair.first);
		for (const auto& s_ptr : pair.second) {
			std::get<1>(without_shared_pointers[*pair.first->begin()]).insert(*s_ptr->begin());
		}
	}
	*/

	std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>> new_collapse_graph;

	std::map< std::shared_ptr<std::set<std::string>>, std::shared_ptr<std::set<std::string>>> bijection;
	for (const auto& pair : collapse_graph) {
		bijection[pair.first] = std::make_shared<std::set<std::string>>(*pair.first);
	}
	// rebuild a collapse_graph;
	for (const auto& pair : collapse_graph) {
		new_collapse_graph[bijection[pair.first]] = [&]() {
			std::set<std::shared_ptr<std::set<std::string>> > y;
			for (const auto& neighbour : pair.second) y.insert(bijection[neighbour]);
			return y;
		}();
	}

	return new_collapse_graph;
}

struct collapse_node {
	static constexpr std::size_t N{ 80 };
	using big_int = std::bitset<N>; //boost::multiprecision::int128_t;
	big_int id;
	big_int forbidden_merges;

	collapse_node(const big_int& id, const big_int& forbidden_merges) : id(id), forbidden_merges(forbidden_merges) {}

	bool operator == (const collapse_node& another) const { return this->id == another.id; }
	bool operator <(const collapse_node& another) const { return this->id.to_string() < another.id.to_string(); }
};

std::vector<collapse_node> enlarge_sets(const collapse_node& the_unique, const std::vector<collapse_node>& the_array) {
	constexpr std::size_t Cthreads = 16;
	std::array<std::vector<collapse_node>, Cthreads> newly_created_ones;
	std::list<std::thread> threads;

	const auto linear_enlarge = [](const collapse_node& the_unique, std::vector<collapse_node>::const_iterator begin, std::vector<collapse_node>::const_iterator end, std::vector<collapse_node>& destination) {
		for (auto iter = begin; iter != end; ++iter) {
			if ((the_unique.forbidden_merges & iter->id).none() && (the_unique.id & iter->id).none()) {
				destination.emplace_back(the_unique.id | iter->id, the_unique.forbidden_merges | iter->forbidden_merges);
			}
		}
	};

	for (int i = 0; i < Cthreads; ++i) {
		threads.emplace_back(linear_enlarge, std::ref(the_unique), the_array.cbegin() + (i * the_array.size()) / Cthreads, the_array.cbegin() + ((i + 1) * the_array.size()) / Cthreads, std::ref(newly_created_ones[i]));
	};
	if (!(the_array.cbegin() + (((Cthreads - 1) + 1) * the_array.size()) / Cthreads == the_array.cend()))
	{
		throw 845923735;
	}
	while (!threads.empty()) {
		threads.front().join();
		threads.pop_front();
	}
	for (int i = 1; i < Cthreads; ++i) {
		newly_created_ones[0].insert(newly_created_ones[0].end(), newly_created_ones[i].cbegin(), newly_created_ones[i].cend());
	}
	return newly_created_ones[0];
}


std::vector<collapse_node> all_nodes_size_10;

template <class T>
class comp_bitset { // comp collapse node would be the correct name
public:
	template <class U>
	auto operator ()(const U& l, const U& r) const -> bool {
		return l.id.to_string() < r.id.to_string();
	}
	comp_bitset() {}
};

const auto COMP_BITSET = comp_bitset<collapse_node::big_int>();

collapse_node create_collapse_node_from_id(collapse_node::big_int id, const std::vector<collapse_node>& primitives) {
	auto result = collapse_node(id, collapse_node::big_int());
	for (const auto& elem : primitives) {
		if ((elem.id & id).any())
			result.forbidden_merges |= elem.forbidden_merges;

	}
	return result;
}


template< class _Container, class _Comp>
void multimerge(std::size_t count_threads, _Container& c1, _Container& c2, _Container& destination, const _Comp& comp) {

	if (c1.empty()) {
		destination = std::move(c2);
		return;
	}
	if (c2.empty()) {
		destination = std::move(c1);
		return;
	}

	_Container pivots;
	for (std::size_t i{ 1 }; i < count_threads; ++i) {
		pivots.push_back(c1[c1.size() * i / count_threads]);
		pivots.push_back(c2[c2.size() * i / count_threads]);
	}
	std::sort(pivots.begin(), pivots.end(), comp);

	std::list<_Container> splitted1;
	std::list<_Container> splitted2;
	std::list<_Container> merged3;
	for (std::size_t i{ 1 }; i < count_threads; ++i) {
		std::size_t index = 2 * i - 1;
		auto p1 = std::lower_bound(c1.begin(), c1.end(), pivots[index], comp);
		auto p2 = std::lower_bound(c2.begin(), c2.end(), pivots[index], comp);
		splitted1.emplace_back(c1.begin(), p1);
		splitted2.emplace_back(c2.begin(), p2);
		c1.erase(c1.begin(), p1);
		c2.erase(c2.begin(), p2);
	}
	c1.shrink_to_fit();
	c2.shrink_to_fit();
	splitted1.push_back(std::move(c1));
	splitted2.push_back(std::move(c2));
	std::list<std::thread> threads;

	const auto mm = [](_Container& left, _Container& right, _Container& destination) {
		std::merge(
			left.cbegin(),
			left.cend(),
			right.cbegin(),
			right.cend(),
			std::back_inserter(destination)
		);
		left.clear();
		right.clear();
		left.shrink_to_fit();
		right.shrink_to_fit();
		destination.erase(std::unique(destination.begin(), destination.end()), destination.end());
	};
	for (std::size_t i{ 0 }; i < count_threads; ++i) {
		//auto& left = *std::next(splitted1.cbegin(), i);
		//auto& right = *std::next(splitted2.cbegin(), i);
		merged3.emplace_back();
		threads.push_back(std::thread(mm, std::ref(*std::next(splitted1.begin(), i)), std::ref(*std::next(splitted2.begin(), i)), std::ref(merged3.back())));
	}
	while (!threads.empty()) {
		threads.front().join();
		threads.pop_front();
	}
	for (auto& vec : merged3) {
		destination.insert(destination.cend(), vec.cbegin(), vec.cend());
		vec.clear();
		vec.shrink_to_fit();
	}
}

void find_local_groupings(
	const grouping_enemies_table& enemies_table,
	const std::vector<std::string>& all_vars,
	std::vector<collapse_node>& max_groupings
) {

	static constexpr bool SANITY_CHECKS{ false };

	//### check that not more than 128 variables!!!!
	if (!(all_vars.size() < collapse_node::N))
		throw 648358;


	const auto get_id_index = [&](const std::string& s) -> std::size_t {
		std::size_t i{ 0 };
		for (; i < all_vars.size(); ++i) {
			if (all_vars[i] == s) break;
		}
		return i;
	};

	std::list<std::vector<collapse_node>> all_sets;

	all_sets.emplace_back(); // size zero
	all_sets.emplace_back(); // size one

	// fill size one -new variant
	for (std::size_t var_id{ 0 }; var_id < enemies_table.size(); ++var_id) {
		collapse_node::big_int var_id_bitstring{ 0 };
		collapse_node::big_int enemies_bitstring{ 0 };
		var_id_bitstring[var_id] = 1;
		for (const auto& enemie : enemies_table[var_id]) {
			enemies_bitstring[enemie] = 1;
		}
		all_sets.back().emplace_back(var_id_bitstring, enemies_bitstring);

		std::stringstream ss;
		ss << "add node : " << var_id_bitstring << " forbid: " << enemies_bitstring;
		standard_logger().info(ss.str());
	}

	if (!std::is_sorted(all_sets.back().begin(), all_sets.back().end(), COMP_BITSET)) {
		standard_logger().warn("Per algorithm design the sequence of initial size-one sets should already be ordered, but indeed is not.");
		std::sort(all_sets.back().begin(), all_sets.back().end(), COMP_BITSET);
	}

	// ### for accumulating: make a new vector where maximal size 1 sets will be excluded.

	std::vector<collapse_node>& primitives{ all_sets.back() };

	static constexpr bool READ_LAST_PROGRESS_AND_RESUME{ false };
	constexpr int size_to_skip{ 10 };

	if constexpr (READ_LAST_PROGRESS_AND_RESUME)
	{ // read last collection....
		std::ifstream read_all_sets("all_sets.txt"); //make parameter

		std::string line;

		while (std::getline(read_all_sets, line)) {
			//++count[std::count(line.cbegin(), line.cend(), '1')];

			if (std::count(line.cbegin(), line.cend(), '1') == 10) {
				all_nodes_size_10.emplace_back(create_collapse_node_from_id(collapse_node::big_int(line), primitives));
			}
		}
	}

	std::ofstream save_max_sets("max_sets.txt", std::ios_base::app); //make parameter
	std::ofstream save_all_sets("all_sets.txt", std::ios_base::app); //make parameter
	std::mutex mutex_save_max_sets;

	std::size_t next_free_index{ 2 };

	if constexpr (READ_LAST_PROGRESS_AND_RESUME)
	{
		all_sets.emplace_back(); // size 2
		all_sets.emplace_back(); // size 3
		all_sets.emplace_back(); // size 4
		all_sets.emplace_back(); // size 5
		all_sets.emplace_back(); // size 6
		all_sets.emplace_back(); // size 7
		all_sets.emplace_back(); // size 8
		all_sets.emplace_back(); // size 9
		all_sets.push_back(std::move(all_nodes_size_10));
#ifdef __clang__
		if (!std::is_sorted(all_sets.back().cbegin(), all_sets.back().cend(), COMP_BITSET)) {
#else
		if (!std::is_sorted(std::execution::par, all_sets.back().cbegin(), all_sets.back().cend(), COMP_BITSET)) {
#endif
			standard_logger().warn("Loaded data not yet sorted.");
			std::sort(all_sets.back().begin(), all_sets.back().end(), COMP_BITSET);
		}

		next_free_index = 11;
	}


	while (true) {
		//
		std::vector<collapse_node>::iterator begin_single{ std::next(all_sets.begin(),1)->begin() };
		std::vector<collapse_node>::iterator end_single{ std::next(all_sets.begin(),1)->end() };

		std::list<std::vector<collapse_node>::iterator> produce_todo_list;
		std::mutex mutex_produce_todo_list;
		for (auto iter = begin_single; iter != end_single; ++iter) {
			produce_todo_list.push_back(iter);
		}

		if (all_sets.size() > 3) {
			auto begin_erase = std::next(all_sets.begin(), 2);
			auto end_erase = std::prev(all_sets.end());
			for (auto iter = begin_erase; iter != end_erase; ++iter) {
				standard_logger().info(std::string("Erased ") + std::to_string(iter->size()) + " elements!");
				iter->clear();
				iter->shrink_to_fit();
			}
		}

		std::vector<collapse_node>& last_filled{ all_sets.back() };


		all_sets.emplace_back();

		std::vector<collapse_node>& fill{ all_sets.back() };

		std::list<std::vector<collapse_node>> filtered_chain;
		std::mutex m_filtered_chain;

		const auto write_all_sets_to_file = [&]() {

			std::size_t size{ 0 }, count{ 0 };
			if (!last_filled.empty()) size = last_filled.front().id.count();
			count = last_filled.size();
			if constexpr (READ_LAST_PROGRESS_AND_RESUME)
				if (size == size_to_skip) return;

			for (const auto& set : last_filled) {
				save_all_sets << set.id.to_string() << std::endl;
			}
			standard_logger().info(std::string("Wrote all sets to file:   size: ") + std::to_string(size) + ",   count: " + std::to_string(count));

		};

		const auto write_max_sets_to_file = [&]() {
			std::size_t size{ 0 };
			std::size_t count_max_sets{ 0 };
			if (!last_filled.empty()) size = last_filled.front().id.count();

			if constexpr (READ_LAST_PROGRESS_AND_RESUME)
				if (size == size_to_skip) return;

			for (const auto& elem : last_filled) {
				for (auto iter = begin_single; iter != end_single; ++iter) {
					if ((elem.id & iter->forbidden_merges).none() && (elem.id & iter->id).none()) {
						// not maximal
						goto next_iteration_0925727394;
					}
				}
				//set is maximal
				{
					auto lock = std::unique_lock(mutex_save_max_sets);
					max_groupings.push_back(elem);
					save_max_sets << elem.id.to_string() << std::endl;
					++count_max_sets;
				}
			next_iteration_0925727394:
				continue;
			}
			standard_logger().info(std::string("Wrote max sets to file:   size: ") + std::to_string(size) + ",   count: " + std::to_string(count_max_sets));
		};

		const auto produce_and_merge = [&](int log_id) {
			while (true) {
				/* get todo*/
				std::vector<collapse_node>::iterator todo;
				{
					auto lock = std::unique_lock(mutex_produce_todo_list);

					if (produce_todo_list.empty()) {
						standard_logger().info(std::string("END of thread     ID : ") + std::to_string(log_id));
						return; // END OF THREAD
					}

					todo = produce_todo_list.front(); // GET TODO
					produce_todo_list.pop_front();
				}

				/* produce todo */
				std::vector<collapse_node> created = enlarge_sets(*todo, last_filled);
				if constexpr (SANITY_CHECKS) {
					if (!std::is_sorted(created.cbegin(), created.cend(), COMP_BITSET)) {
						standard_logger().error("not sorted here");
						std::sort(created.begin(), created.end(), COMP_BITSET);
					}
				}

				standard_logger().info(std::string("Created a series of new sets       thread - ID : ") + std::to_string(log_id));
				/* merge into some existing as long as there exists something */
				while (true) {
					// fetch some other vector to merge with
					std::vector<collapse_node> another_to_merge_with;
					{
						auto lock = std::unique_lock(m_filtered_chain);


						if (filtered_chain.empty()) {
							filtered_chain.emplace_back(std::move(created));
							break;
						}
						else {
							/* add it to list of produced but unmerged */
							another_to_merge_with = std::move(filtered_chain.front());
							filtered_chain.pop_front();
						}
					}

					/* perform the merge */
					std::vector<collapse_node> merged_vector;
					//std::vector<collapse_node> merged_vector(another_to_merge_with.size() + created.size(),collapse_node(collapse_node::big_int(), collapse_node::big_int()));
					//auto new_end = std::merge(std::execution::par, created.begin(), created.end(), another_to_merge_with.begin(), another_to_merge_with.end(), merged_vector.begin());
					//merged_vector.erase(std::unique(merged_vector.begin(), new_end), merged_vector.end());

					multimerge(16, created, another_to_merge_with, merged_vector, COMP_BITSET);

					standard_logger().info(std::string("Successfully merged two sets       thread - ID : ") + std::to_string(log_id));

					created.clear();
					created = std::move(merged_vector);
				}
			}
		};

		constexpr uint8_t count_threads{ 1 };

		std::array<std::thread, count_threads> produce_and_merge_threads;

		/* start threads */
		std::thread write_max_sets_to_file_thread = std::thread(write_max_sets_to_file);
		std::thread write_all_sets_to_file_thread = std::thread(write_all_sets_to_file);

		for (uint8_t t = 0; t < count_threads; ++t) {
			produce_and_merge_threads[t] = std::thread(produce_and_merge, t);
		}

		/* join  threads */
		write_max_sets_to_file_thread.join();
		write_all_sets_to_file_thread.join();

		for (uint8_t t = 0; t < count_threads; ++t) {
			produce_and_merge_threads[t].join();
		}

		fill = std::move(filtered_chain.front());

		auto filled_size{ fill.size() };

		standard_logger().info(std::string("successfully created sets with size  ") + std::to_string(next_free_index) + "  counting " + std::to_string(filled_size));

		if (filled_size == 0) {
			return;
		}

		++next_free_index;

	}
}

enum class selected : char {
	YES, NO, UNDECIDED
};

struct consideration {
	std::vector<selected> sel;
	collapse_node::big_int already_covered;

	std::size_t count() const {
		return std::count_if(sel.cbegin(), sel.cend(), [](selected x) { return x == selected::YES; });
	}

	consideration(const std::vector<selected>& sel, const collapse_node::big_int& already_covered) : sel(sel), already_covered(already_covered) {}
};

void find_all_global_coverings_with_max_groupings(
	const std::vector<std::string>&all_vars,
	const std::vector<collapse_node>&max_groupings,
	std::vector<consideration>&combinations_of_max_groupings
) {

	std::size_t min_selection_size_so_far{ all_vars.size() };

	auto goal = collapse_node::big_int();

	for (std::size_t i = 0; i < all_vars.size(); ++i) {
		goal[i] = 1;
	}

	std::list<consideration> todo_chain;

	todo_chain.emplace_back(std::vector<selected>(max_groupings.size(), selected::UNDECIDED), collapse_node::big_int()); // nothing yet selected, already_covered zero variables

	{	// corner case speed up: If some variable needs a singleton group, we can select it at the beginning.
		consideration& initial_consideration{ todo_chain.back() };
		for (std::size_t i = 0; i < max_groupings.size(); ++i) {
			if (max_groupings[i].id.count() == 1) {
				initial_consideration.sel[i] = selected::YES;
				initial_consideration.already_covered |= max_groupings[i].id;
			}
		}
	}
	// mark all max single var sets as already selected here!!! #####

	const auto step = [&](const consideration& rec) {
		if (rec.already_covered == goal) { // reached goal :)
			const auto size_selection = rec.count();
			if (size_selection < min_selection_size_so_far) {
				min_selection_size_so_far = size_selection;
				combinations_of_max_groupings.clear(); // discard all previously found combinations that took more selected items.
			}
			combinations_of_max_groupings.push_back(rec);
			return;
		}
		if (rec.count() >= min_selection_size_so_far) // did not reached goal, but with the next selection we will exceed min_selection_size_so_far
			return;

		// construct sub cases:

		collapse_node::big_int rest_goal = goal & (~rec.already_covered); // the variables that are not yet covered by selected groups.

		// select the next group to add to consideration by looking for max rest_goal elimination:
		std::size_t sel_i = 0;
		std::size_t sel_rest_size = 0;
		for (std::size_t i = 0; i < max_groupings.size(); ++i) {
			const auto rest_usage_size = (max_groupings[i].id & rest_goal).count();
			if (rest_usage_size > sel_rest_size) {
				sel_rest_size = rest_usage_size;
				sel_i = i;
			}
		}
		if (sel_rest_size == 0)
			return; // cannot select any progress.
		if ((min_selection_size_so_far - rec.count()) * sel_rest_size < rest_goal.count())
			// with each step of selecting another group we may cover at most {sel_rest_size} rest variables.
			// We cannot cover all rest variables {rest_goal.size()} if we only have {(min_selection_size_so_far - rec.count())} choices left
			// without exceeding {min_selection_size_so_far} choices.
			return;

		// Abort criterium: If there is a specific variable that is not yet covered and all its groups are already excluded we cannot reach goal anymore.
		if constexpr /* USE THIS CRITERIUM? */ (true) {
			collapse_node::big_int reachable_goal;
			for (std::size_t i{ 0 }; i < max_groupings.size(); ++i) {
				if (rec.sel[i] != selected::NO) reachable_goal |= max_groupings[i].id;
				if (reachable_goal == goal) goto abort_criterium_proceed_4756928;
			}
			return; // here it holds that:   reachable_goal != goal
		}
	abort_criterium_proceed_4756928:

		consideration add_yes = rec;
		consideration add_no = rec;

		add_yes.sel[sel_i] = selected::YES;
		add_yes.already_covered = add_yes.already_covered | max_groupings[sel_i].id;

		add_no.sel[sel_i] = selected::NO;

		todo_chain.push_front(add_yes);
		todo_chain.push_back(add_no);
	};

	while (!todo_chain.empty()) {
		consideration first = todo_chain.front();
		todo_chain.pop_front();
		step(first);
	}
}

void find_all_coverings_with_non_overlapping_groups(
	std::vector<std::string> all_vars,
	const std::vector<collapse_node>&max_groupings,
	const std::vector<consideration>&combinations_of_max_groupings,
	std::vector<std::vector<collapse_node::big_int>>&coverings_with_non_overlapping_groups
) {
	if (combinations_of_max_groupings.empty()) return;
	coverings_with_non_overlapping_groups.clear();

	std::vector<std::vector<collapse_node::big_int>> coverings_with_overlapping_groups;

	const auto COUNT_VARS{ all_vars.size() };

	for (const auto& group_combination : combinations_of_max_groupings) {
		//std::size_t last_size = coverings_with_non_overlapping_groups.size();
		//std::vector<std::size_t> select_group_for_variable(COUNT_VARS, 0);

		std::vector<collapse_node::big_int> only_used_max_groups;
		for (std::size_t i{ 0 }; i < group_combination.sel.size(); ++i) {
			if (group_combination.sel[i] == selected::YES) {
				only_used_max_groups.push_back(max_groupings[i].id);
			}
		}

		coverings_with_overlapping_groups.push_back(only_used_max_groups);
	}

	// resolve overlappings:
	std::vector<std::vector<collapse_node::big_int>> next_coverings_with_overlapping_groups;
	while (!coverings_with_overlapping_groups.empty()) {
		for (const std::vector<collapse_node::big_int>& covering : coverings_with_overlapping_groups) {
			// for each var search for overlapping
			for (std::size_t var_id{ 0 }; var_id < COUNT_VARS; ++var_id) {
				// list all groups covering var_id:
				std::vector<std::size_t> indices_covering_var_id;
				for (std::size_t j{ 0 }; j < covering.size(); ++j) {
					if (covering[j][var_id])
						indices_covering_var_id.push_back(j);
				}
				// if var_id is overlapping in this covering:
				if (indices_covering_var_id.size() > 1) {
					auto sub_cases = std::vector< std::vector<collapse_node::big_int>>(indices_covering_var_id.size(), covering);
					for (std::size_t i{ 0 }; i < sub_cases.size(); ++i) {
						for (std::size_t j{ 0 }; j < indices_covering_var_id.size(); ++j) {
							if (i != j) sub_cases[i][indices_covering_var_id[j]][var_id] = 0;// at sub case i, take the jth group covering var_id, reset bit for var_id
						}
					}
					next_coverings_with_overlapping_groups.insert(next_coverings_with_overlapping_groups.cend(), sub_cases.cbegin(), sub_cases.cend());
					goto proceed_with_next_covering_763941346;
				}
				// var_id was not overlapping, lets check the others...
			}
			//no var_id is overlapping, so the whole covering is not overlapping:
			coverings_with_non_overlapping_groups.push_back(covering);
		proceed_with_next_covering_763941346:
			continue;
		}
		coverings_with_overlapping_groups = std::move(next_coverings_with_overlapping_groups);
	}

	// sort and eliminate duplicates:

	// inner sort
	for (std::vector<collapse_node::big_int>& element : coverings_with_non_overlapping_groups) {
		std::sort(
			element.begin(),
			element.end(),
			[](const collapse_node::big_int& left, const collapse_node::big_int& right) {
				return left.to_string() < right.to_string();
			}
		);
	}
	// outer sort
	std::sort(
		coverings_with_non_overlapping_groups.begin(),
		coverings_with_non_overlapping_groups.end(),
		[](const std::vector<collapse_node::big_int>& left, const std::vector<collapse_node::big_int>& right) {
			if (left.size() < right.size()) return true;
			for (std::size_t i{ 0 }; i < left.size(); ++i) {
				if (left[i].to_string() < right[i].to_string()) return true;
				if (left[i].to_string() > right[i].to_string()) return false;
			}
			return false;
		}
	);

	// remove duplicates
	coverings_with_non_overlapping_groups.erase(std::unique(coverings_with_non_overlapping_groups.begin(), coverings_with_non_overlapping_groups.end()), coverings_with_non_overlapping_groups.cend());

}

void print_enemies_table_to_log( //#?ready
	const grouping_enemies_table & enemies_table,
	std::vector<std::string> all_vars
) {
	std::stringstream ss;
	ss << std::endl;
	for (std::size_t i{ 0 }; i < enemies_table.size(); ++i) {
		ss << "Enemies of variable with id   " << i << "   (" << all_vars[i] << "):" << std::endl
			<< "   {";
		for (const auto& enemie : enemies_table[i]) {
			ss << "variable   " << enemie << "   (" << all_vars[enemie] << "),\n";
		}
		ss << "   }\n\n";
	}
	ss << "\n\n\n";

	standard_logger().info(ss.str());
}

void find_all_minimal_partitionings( //#?ready
	const grouping_enemies_table & enemies_table,
	const std::size_t max_threads,
	std::vector<std::string> all_vars,
	std::vector<collapse_node>&max_groupings,
	std::vector<std::vector<collapse_node::big_int>>&all_colorings_with_minimal_variables
) {
	standard_logger().info("#############################################################################################");
	standard_logger().info("##### Start calculating all possible partitionings with minimal number of partitions... #####");
	standard_logger().info("#############################################################################################");
	standard_logger().info("");

	standard_logger().info("The following enemies are forbidden to be merged into one partition:");
	standard_logger().info("");

	print_enemies_table_to_log(enemies_table, all_vars);

	standard_logger().info("Calculating all maximal local groupings...");
	find_local_groupings(enemies_table, all_vars, max_groupings);
	standard_logger().info("Calculating all maximal local groupings   ...DONE!");

	standard_logger().info("Calculating all coverings of the whole variable set by selecting minimal amounts of local groupings...");
	std::vector<consideration> combinations_of_max_groupings;
	find_all_global_coverings_with_max_groupings(all_vars, max_groupings, combinations_of_max_groupings);
	standard_logger().info("Calculating all coverings of the whole variable set by selecting minimal amounts of local groupings   ...DONE!");

	standard_logger().info("Calculating all partitionings of the whole variable set by eliminating overlapping maximal local groupings...");
	find_all_coverings_with_non_overlapping_groups(all_vars, max_groupings, combinations_of_max_groupings, all_colorings_with_minimal_variables);
	standard_logger().info("Calculating all partitionings of the whole variable set by eliminating overlapping maximal local groupings   ...DONE!");

	standard_logger().info("##############################################################################################");
	standard_logger().info("##### Finished calculating all possible partitionings with minimal number of partitions. #####");
	standard_logger().info("##############################################################################################");

}

void live_range_analysis(
	const file_token & ftoken,
	const std::map<std::string, int>&const_table,
	const std::string & var_name, std::vector<std::string>&excluded_vars,
	std::map<std::string, std::tuple<bool, int, std::set<std::string>, int>>&graph,
	live_var_map & live_vars
) {
	// live range analysis:
	// calculate program graph:
	std::string var_name_next{ var_name + "'" };

	using value_type = int;

	// vector of state transitions with associated transition and postcond token
	using program_graph_edge = std::tuple<value_type, value_type, std::shared_ptr<transition_token>, std::shared_ptr<condition_token>>;
	std::vector<program_graph_edge> program_graph;

	const auto concrete_transition_identifier_of_program_graph_item =
		[](const std::tuple<value_type, value_type, std::shared_ptr<transition_token>, std::shared_ptr<condition_token>>& edge) {
		return std::make_pair(std::get<2>(edge), std::get<3>(edge));
	};

	auto module_defs = ftoken._dtmc_body_component->module_definitions();
	// assert correct size.
	auto& the_module{ module_defs.front() };


	// fill program graph
	for (const auto& s : the_module->_transitions) {
		const auto& transition = s.first; // ignore separating space

		auto contains_var = transition->_pre_condition->contains_variable(var_name);
		auto contains_var_next = transition->_pre_condition->contains_variable(var_name_next); // should be an error

		if (contains_var) {

			std::vector<std::pair<bool, bool>> relevant_post_conditions; // check if variable is contained. (cf, cf')

			// fill relevant_post_conditions
			for (const auto& posts : transition->_regular_post_conditions) {
				const auto& post_condition = std::get<1>(posts); // ignore probability
				relevant_post_conditions.push_back({
						post_condition->contains_variable(var_name),
						post_condition->contains_variable(var_name_next)
					});
			}

			std::shared_ptr<std::vector<int>> pre_values = transition->_pre_condition->get_values(var_name, const_table);
			if (std::find(pre_values->begin(), pre_values->end(), 137) != pre_values->end()) {
				standard_logger().info("should eventually be reached");
			}
			for (int post_index = 0; post_index < relevant_post_conditions.size(); ++post_index) {
				const auto& post_condition = std::get<1>(transition->_regular_post_conditions[post_index]);
				std::shared_ptr<std::vector<int>> post_values = post_condition->get_values(var_name_next, const_table);
				// check for no /more values here! ###
				if (!(pre_values->size() && post_values->size())) {
					standard_logger().info("debug");
				}
				program_graph.push_back(std::make_tuple((*pre_values)[0], (*post_values)[0], transition, post_condition)); //## fix vector subscript error here!
			}
		}
		else {
			standard_logger().warn("check here");
		}

	}

	std::map<std::pair<std::shared_ptr<transition_token>, std::shared_ptr<condition_token>>, std::vector<std::string>> gen_sets;
	std::map<std::pair<std::shared_ptr<transition_token>, std::shared_ptr<condition_token>>, std::vector<std::string>> kill_sets;

	// fill gen and kill sets
	for (const auto& edge : program_graph) {
		const auto& tr = std::get<2>(edge);
		const auto& post_cond = std::get<3>(edge);

		const auto remove_quote = [](auto& vec) {
			for (auto& e : vec) {
				if (e.back() == '\'') {
					e = std::string(e.cbegin(), std::prev(e.cend()));
				}
			}
		};
		const auto the_pair = std::make_pair(tr, post_cond);

		if (gen_sets.find(the_pair) == gen_sets.end()) {
			// used variables in transition before written
			gen_sets[the_pair] = tr->_pre_condition->all_variables(const_table);
			// ariables that get an assignment
			kill_sets[the_pair] = post_cond->all_variables(const_table);
			remove_quote(kill_sets[the_pair]);
		}
	}


	const auto int_vector_union = [](const auto& l, const auto& r) { //### rename
		std::vector<std::string> copy = l;
		for (const auto& val : r) {
			if (std::find(copy.cbegin(), copy.cend(), val) == copy.cend()) {
				copy.push_back(val);
			}
		}
		return copy;
	};

	// init live var sets from union of incident gen sets
	for (const auto& p : program_graph) {
		auto key = std::get<0>(p);
		auto [iter, initialized] = live_vars.insert(
			std::make_pair(key, std::make_tuple(true, std::vector<std::string>(), std::vector<std::string>()))
		);
		auto& value{ std::get<1>(iter->second) };
		value = int_vector_union(value, gen_sets[concrete_transition_identifier_of_program_graph_item(p)]);
	}
	// copy change sets:
	for (auto& mapping : live_vars) {
		std::get<2>(mapping.second) = std::get<1>(mapping.second);
	}

	const auto int_vector_set_minus = [](const auto& l, const auto& r) {
		std::vector<std::string> copy = l;
		for (const auto& val : r) {
			while (true) {
				auto iter = std::find(copy.cbegin(), copy.cend(), val);
				if (iter != copy.cend()) {
					copy.erase(iter);
				}
				else {
					break;
				}
			}
		}
		return copy;
	};

	live_var_map::iterator state_iter{ live_vars.begin() };

	constexpr std::size_t HINT_SIZE{ 10 };
	std::vector<live_var_map::iterator>  recalculate_hints(HINT_SIZE, state_iter);
	std::size_t insert_hint = 1 % HINT_SIZE;
	std::size_t read_hint = 0;

	const auto edge_from = [](const program_graph_edge& edge) { return std::get<0>(edge); }; // make this real function, use two instance, const and non const maybe...
	const auto edge_to = [](const program_graph_edge& edge) { return std::get<1>(edge); };
again_while:
	while (true) {
		auto get_next_recalculatable_live_set = [&]() {
			// search through hints:
			auto check_found = [&](const live_var_map::iterator& it) {
				return flag_of_liveness_tuple(it->second) ? std::make_optional<live_var_map::iterator>(it) : std::optional<live_var_map::iterator>();
			};
			for (std::size_t i{ 0 }; i < HINT_SIZE; ++i) {
				auto candidate = check_found(recalculate_hints[read_hint++ % HINT_SIZE]);
				if (candidate) {
					read_hint %= HINT_SIZE;
					return candidate;
				}
			}
			// search through all map entries
			for (std::size_t i{ 0 }; i < live_vars.size(); ++i) {
				auto candidate = check_found(state_iter++);
				if (state_iter == live_vars.cend()) state_iter = live_vars.begin();
				if (candidate) return candidate;
			}
			return std::optional<live_var_map::iterator>();
		};

		std::optional<live_var_map::iterator> next{ get_next_recalculatable_live_set() };
		if (!next) break;
		auto& live_var_mapping{ next.value() };
		auto& key{ live_var_mapping->first };
		auto& value_tuple{ live_var_mapping->second };
		auto& recalculate_flag{ std::get<0>(value_tuple) };
		auto& changes{ std::get<2>(value_tuple) };
		recalculate_flag = false;

		for (const auto& edge : program_graph) {
			if (edge_to(edge) == key) {
				// propagate changes from #value_tuple to the predecessor on this #egde...
				auto& from_state_liveness{ live_vars[edge_from(edge)] };
				std::vector<std::string> propagate = int_vector_set_minus(changes, kill_sets[concrete_transition_identifier_of_program_graph_item(edge)]);
				auto minus = current_of_liveness_tuple(from_state_liveness); //### delete this local variable!!!
				std::vector<std::string> real_additives = int_vector_set_minus(propagate, minus);
				if (real_additives.empty()) continue;
				current_of_liveness_tuple(from_state_liveness) = int_vector_union(current_of_liveness_tuple(from_state_liveness), real_additives);
				changes_of_liveness_tuple(from_state_liveness) = int_vector_union(changes_of_liveness_tuple(from_state_liveness), real_additives);
				flag_of_liveness_tuple(from_state_liveness) = true;
				if (recalculate_flag == true) {
					goto again_while;
				}
			}
		}
		changes.clear();
	}

	// build graph for collapsing variables:
	// std::map<std::string, std::tuple<bool, int, std::set<std::string>, int>> graph;
	// node "var_name" |-> (!removed during coloring phase, count neighbours during coloring phrase, active and inactive neighbours, color)


	for (auto& live_pair : live_vars) {
		const std::vector<std::string>& vector_of_incident_var_names{ current_of_liveness_tuple(live_pair.second) };
		for (auto iter = vector_of_incident_var_names.cbegin(); iter != vector_of_incident_var_names.cend(); ++iter) {
			neighbours(graph[*iter]).insert(vector_of_incident_var_names.cbegin(), iter);
			neighbours(graph[*iter]).insert(std::next(iter), vector_of_incident_var_names.cend());
		}
	}
	// variables that are never generated to be live but are killed from liveness should be added:
	for (const auto& pair : kill_sets) {
		for (const auto& var_name : pair.second) {
			graph[var_name];
		}
	}
	// fill in all var names that should not be collapsed with others:
	for (const auto& excluded : excluded_vars) {
		if (graph.find(excluded) != graph.end()) {
			for (auto& graph_pair : graph) {
				if (graph_pair.first != excluded)
					neighbours(graph_pair.second).insert(excluded);
				else
					for (auto& inner_graph_pair : graph)
						if (inner_graph_pair.first != excluded)
							neighbours(graph_pair.second).insert(inner_graph_pair.first);
			}
		}
	}

	for (auto& graph_pair : graph) {
		is_active(graph_pair.second) = true;
		count_active_neighbours(graph_pair.second) = neighbours(graph_pair.second).size();
		color(graph_pair.second) = -1;
	}
}


unsigned long long count_variables_of_coloring(const std::map<std::string, int>&coloring) {
	std::set<int> values;
	for (const auto& pair : coloring) values.insert(pair.second);
	return values.size();
}



void filter_colorings(std::list<std::pair<std::map<std::string, int>, unsigned long long>>&useful_colorings, std::list<std::map<std::string, int>>&all_colorings, std::mutex & m_all_colorings, bool& continue_running) {
	unsigned long long TOLERANCE{ 0 };
	unsigned long long min_var_count{ std::numeric_limits<unsigned long long>::max() - TOLERANCE };
	unsigned long long i{ 0 };
	bool shrink{ false };
	while (continue_running) {
		++i;
		shrink = false;
		std::list<std::map<std::string, int>> fetched_colorings;
		std::this_thread::sleep_for(std::chrono::milliseconds(30000));
		{
			auto lock = std::unique_lock(m_all_colorings);
			fetched_colorings = std::move(all_colorings);
			all_colorings.clear();
		}
		standard_logger().info("fetched colorings size");
		standard_logger().info(fetched_colorings.size());
		for (auto& c : fetched_colorings) {
			auto size = count_variables_of_coloring(c);
			if (size <= min_var_count + TOLERANCE) {
				if (size < min_var_count) {
					min_var_count = size;
					shrink = true;
				}
				/*
				//standard_logger().info("search duplicates in colorings:");
				for (auto& uc : useful_colorings) {
					if (compare_colorings(c, uc.first)) {
						standard_logger().error("found duplicate!");
					}
				}
				*/
				useful_colorings.push_back(std::make_pair(std::move(c), size));
			}
		}
		if (shrink) {
			std::vector<std::list<std::pair<std::map<std::string, int>, unsigned long long>>::const_iterator> to_delete;
			for (auto iter = useful_colorings.cbegin(); iter != useful_colorings.cend(); ++iter) {
				if (!(iter->second <= min_var_count + TOLERANCE)) to_delete.push_back(iter);
			}
			for (const auto& iter : to_delete) useful_colorings.erase(iter);
		}
		standard_logger().info(std::string("filter colorings:  iteration: ") + std::to_string(i) + "   useful colorings found: " + std::to_string(useful_colorings.size()));
	}

}

void write_var_list_txt(const std::filesystem::path & directory, const std::vector<std::string>&all_vars) {
	std::filesystem::create_directories(directory);
	auto file = std::ofstream((directory / "var_list.txt").c_str());
	/*standard_logger().info(
		std::string(
			(directory / "var_list.txt").c_str()
		)
	);
	*/
	for (const auto& var : all_vars)
		file << var << std::endl;
}

void write_max_local_groupings(const std::filesystem::path & directory, const std::vector<collapse_node>&max_groupings) {
	std::filesystem::create_directories(directory);
	auto file = std::ofstream((directory / "max_groupings.txt").c_str());
	for (const auto& group : max_groupings)
		file << group.id.to_string() << std::endl;
}

void write_all_partitionings(const std::filesystem::path & directory, const std::vector<std::vector<collapse_node::big_int>>&all_partitionings_with_minimal_size) {
	std::filesystem::create_directories(directory);
	auto file = std::ofstream((directory / "all_partitionings.json").c_str());

	nlohmann::json json;
	auto& j_partitions{ json["partitionings"] };
	for (std::size_t i{ 0 }; i < all_partitionings_with_minimal_size.size(); ++i) {
		const auto& partitioning = all_partitionings_with_minimal_size[i];
		nlohmann::json this_partitioning;
		for (const auto& partition : partitioning)
			this_partitioning["partitions"].push_back(partition.to_string());
		j_partitions[std::to_string(i)] = this_partitioning;
	}

	file << json.dump(3);
}

void write_meta_json(const std::filesystem::path & directory, std::size_t number_of_partitionings) {
	std::filesystem::create_directories(directory);
	auto file = std::ofstream((directory / "meta.json").c_str());

	nlohmann::json json;
	json["count_partitionings"] = number_of_partitionings;

	file << json.dump(3);
}

void write_model_to_file(const std::filesystem::path & directory, const file_token & model) {
	std::filesystem::create_directories(directory);
	auto file = std::ofstream((directory / "reduced_model.prism").c_str());
	print_model_to_stream(model, file);
}

void write_partitioning_to_file(const std::filesystem::path & directory, const std::vector<collapse_node::big_int>&partitioning) {
	std::filesystem::create_directories(directory);
	auto file = std::ofstream((directory / "partitioning.json").c_str());

	nlohmann::json json;
	for (const auto& partition : partitioning) {
		json["partitions"].push_back(partition.to_string());
	}
	file << json.dump(3);
}

file_token construct_reduced_model(
	const file_token & original_model,
	const std::vector<collapse_node::big_int>&partitioning,
	const std::vector<std::string>&all_vars,
	std::string cf_var_name, const std::map<std::string, int>&const_table,
	live_var_map & live_vars,
	const std::map<std::string, std::tuple<bool, int, std::set<std::string>, int>>&graph
) {
	file_token reduced_file(original_model);

	std::map<std::string, std::tuple<bool, int, std::set<std::string>, int>> graph_with_colors{ graph };

	//### todo here apply partitioning to graph.... <-- DONE     .... but refactor!!!, use a more restricted structure instead of graph data type

	for (std::size_t color_i{ 0 }; color_i < partitioning.size(); ++color_i) {
		for (std::size_t var_id{ 0 }; var_id < all_vars.size(); ++var_id) {
			if (partitioning[color_i][var_id])
				color(graph_with_colors[all_vars[var_id]]) = color_i;
		}
	}

	apply_coloring_to_file_token(reduced_file, cf_var_name, const_table, live_vars, graph_with_colors);

	return reduced_file;
}


const auto path_to_string = [](auto path) {
	if constexpr (std::is_same<std::filesystem::path::value_type, wchar_t>::value) {
		return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(path.c_str());
	}
	else {
		return std::string(path.c_str());
	}
};

int cli(int argc, char** argv) {
	standard_logger().info("This is Syntactic Reducer 1.0\n\n");
	//standard_logger().info(std::string("Running config json: ") + argv[0]);
	const std::string CURRENT_PATH_CHAR_STRING{ path_to_string(std::filesystem::current_path()) };
	standard_logger().info(std::string("Current path:  ") + CURRENT_PATH_CHAR_STRING);
	//std::filesystem::path CURRENT_PATH{ CURRENT_PATH_CHAR_STRING };

	standard_logger().info("Loading config json...");
	/* Obtain config json path */
	std::filesystem::path config_json_path{ argv[1] };
	standard_logger().info(std::string("Given config json path:  ") + path_to_string(config_json_path));
	config_json_path = std::filesystem::canonical(config_json_path);
	const std::filesystem::path config_json_directory{ config_json_path.parent_path() };
	standard_logger().info(std::string("Canonical config json path:  ") + path_to_string(config_json_path));
	/* Load config json */
	nlohmann::json config;
	if (argc >= 2) {
		std::ifstream config_json_ifstream;
		config_json_ifstream.open(config_json_path);
		//auto config_json = std::string(std::istreambuf_iterator<char>(config_json_ifstream), std::istreambuf_iterator<char>());
		try {
			config = nlohmann::json::parse(config_json_ifstream);
		}
		catch (...) {
			standard_logger().error("Exception on parsing config json!");
			throw;
		}
	}
	else {
		standard_logger().error("Missing config json path!");
		return 1;
	}
	standard_logger().info(std::string("Successfully loaded config json:\n\n") + config.dump(3) + "\n");
	std::string prism_command = config["prism_command"];
	std::string transformed_prism_command{};
	for (const auto& c : prism_command) {
		if (c == '/') transformed_prism_command.push_back('\\');
		transformed_prism_command.push_back(c);
	}
	standard_logger().info("transformed json");
	standard_logger().info(transformed_prism_command);
	config["prism_command"] = transformed_prism_command;

	/* Load model */
	standard_logger().info("Loading model...");
	//standard_logger().debug(path_to_string(config_json_directory / std::string(config["model_path"])));
	std::filesystem::path model_path{
		std::filesystem::canonical(
			config_json_directory / std::string(config["model_path"])
		)
	};
	standard_logger().info(std::string("Canonical model path:  ") + path_to_string(model_path));
	std::ifstream model_ifstream;
	model_ifstream.open(model_path);
	auto model_string_ptr = std::make_shared<std::string>(std::istreambuf_iterator<char>(model_ifstream), std::istreambuf_iterator<char>());
	standard_logger().info("Loading model   ...DONE!");

	std::filesystem::path results_directory{ std::filesystem::weakly_canonical(config_json_directory / std::string(config["result_dir"])) };
	standard_logger().info(std::string("Results will be written to directory:  ") + path_to_string(results_directory));

	standard_logger().info("Loading list of excluded variables...");
	// when here then all live set were computed.
	auto excluded_vars = std::vector<std::string>(config["exclude_vars"].cbegin(), config["exclude_vars"].cend());
	//std::vector<std::string> excluded_vars{ "y_Integrator_44480461", "x_cfblk5_1_1174489129" };

	standard_logger().info("Start parsing...");
	auto ftoken = file_token(model_string_ptr);
	ftoken.parse();
	bool check = ftoken.is_sound_recursive();
	standard_logger().info("Finished parsing.");


	// values of const symbols:
	const std::map<std::string, int> const_table{ [&] {
		std::map<std::string, int> const_table;
		auto const_def_container = ftoken._dtmc_body_component->const_definitions();
		for (const auto& const_def : const_def_container) {
			const_table[const_def->_constant_identifier->str()] = *const_def->_expression->get_value(const_table); // check nullptr?
		}
		return const_table;
	}() };

	std::string var_name{ "cf" };

	// node "var_name" |-> (!removed during coloring phase, count neighbours during coloring phrase, active and inactive neighbours, color)
	std::map<std::string, std::tuple<bool, int, std::set<std::string>, int>> graph;
	live_var_map live_vars;


	live_range_analysis(ftoken, const_table, var_name, excluded_vars, graph, live_vars);

	/*+++++++++++++++++++++++++++++*/

	// brute force all colorings:

	std::shared_ptr<std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>> collapse_graph = std::make_shared< std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>>();
	// set of variables with the same coloring |-> pointers to all other sets that must not be unified with the set.

	std::map<std::string, std::shared_ptr<std::set<std::string>>> var_mapped_to_color_class;
	// create single sets for collapsing_graph:
	for (auto iter = graph.cbegin(); iter != graph.cend(); ++iter) {
		std::shared_ptr<std::set<std::string>> single_node = std::shared_ptr<std::set<std::string>>(new std::set<std::string>({ iter->first }));
		var_mapped_to_color_class[iter->first] = single_node;
	}
	// add all keys + values to collapse_graph:
	for (auto iter = graph.begin(); iter != graph.end(); ++iter) {
		std::shared_ptr<std::set<std::string>> single_node_set = var_mapped_to_color_class[iter->first];
		auto& local_neighbours = (*collapse_graph)[single_node_set];
		for (auto jter = neighbours(iter->second).cbegin(); jter != neighbours(iter->second).cend(); ++jter) {
			local_neighbours.insert(var_mapped_to_color_class[*jter]);
		}
	}

	const std::size_t max_threads{ 8 };

	/*#### create such a class where inverse search is added as member function!!!
	class var_names_collection {
		std::vector<std::string> all_var_names;

	};

	algor for inverse search here:
			auto [first, last] = std::equal_range(all_var_names.cbegin(), all_var_names.cend(), do_not_merge_with);
			if ((first == last) || (first + 1 != last))
				throw 935734839;
			std::size_t var_id_enemie{ first - all_var_names.cbegin() };
	*/

	std::vector<std::string> all_var_names;
	std::transform(graph.begin(), graph.end(), std::back_inserter(all_var_names), [](auto& pair) {return pair.first; });
	std::sort(all_var_names.begin(), all_var_names.end());
	//## sanity check: no duplicates in this vector.

	auto enemies_table = grouping_enemies_table(all_var_names.size());
	for (std::size_t var_id{ 0 }; var_id < all_var_names.size(); ++var_id) {
		if (!enemies_table[var_id].empty())
			throw 9134872798; //### just a sanity check, remove please!
		for (const std::string& do_not_merge_with : neighbours(graph[all_var_names[var_id]])) {
			auto [first, last] = std::equal_range(all_var_names.cbegin(), all_var_names.cend(), do_not_merge_with);
			if ((first == last) || (first + 1 != last))
				throw 935734839;
			std::size_t var_id_enemie{ static_cast<std::size_t>(first - all_var_names.cbegin()) };
			enemies_table[var_id].insert(var_id_enemie);
		}
	}

	std::vector<std::vector<collapse_node::big_int>> all_partitionings_with_minimal_size;
	std::vector<collapse_node> max_groupings;

	find_all_minimal_partitionings(enemies_table, max_threads, all_var_names, max_groupings, all_partitionings_with_minimal_size);  //  ####refactor maxthreads inner and outer threads config....

	// var_list.txt
	write_var_list_txt(results_directory, all_var_names);

	// max_groupings.txt
	write_max_local_groupings(results_directory, max_groupings);

	// all_partitions.json
	write_all_partitionings(results_directory, all_partitionings_with_minimal_size);
	
	// meta.json
	write_meta_json(results_directory, all_partitionings_with_minimal_size.size());

	for (std::size_t i{ 0 }; i < all_partitionings_with_minimal_size.size(); ++i) {
		file_token reduced_model = construct_reduced_model(ftoken, all_partitionings_with_minimal_size[i], all_var_names, var_name, const_table, live_vars, graph);

		// reduced_model.prism
		write_model_to_file(results_directory / std::to_string(i), reduced_model);

		// partitioning.json
		write_partitioning_to_file(results_directory / std::to_string(i), all_partitionings_with_minimal_size[i]);
	}

	//## create a meta json to be able to start the prism stuff in consequence...

	//
	//### all colorings still unused here.
/*+++++++++++++++++++++++++++++*/
	/*
		print_coloring_from_graph_with_color_annotations(graph, output_model);
		apply_coloring_to_file_token(reduced_model_parse_tree, var_name, const_table, live_vars, graph);

		*/

		/*
		starke_coloring(graph);

		print_coloring_from_graph_with_color_annotations(graph, std::cout);


		// copy the whole parse tree
		file_token reduced_file(ftoken);

		apply_coloring_to_file_token(reduced_file, var_name, const_table, live_vars, graph);

		// print reduced model:
		auto ofile = std::ofstream("reduced_model.prism");
		print_model_to_stream(reduced_file, ofile);
		*/

	std::ofstream config_json_ofstream;
	config_json_ofstream.open(config_json_path);
	config_json_ofstream << config.dump(3);
	
	return 0;
}



int main(int argc, char** argv)
{
	init_logger();
	return cli(argc, argv);

}
