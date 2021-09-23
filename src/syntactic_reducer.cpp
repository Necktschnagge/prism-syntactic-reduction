
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

	bool operator == (const collapse_node& another) { return this->id == another.id; }
	bool operator <(const collapse_node& another) { return this->id.to_string() < another.id.to_string(); }
};

std::vector<collapse_node> enlarge_sets(const collapse_node& the_unique, const std::vector<collapse_node>& the_array) {
	std::vector<collapse_node> newly_created_ones;
	for (auto iter = the_array.cbegin(); iter != the_array.cend(); ++iter) {
		if ((the_unique.forbidden_merges & iter->id).none() && (the_unique.id & iter->id).none()) {
			newly_created_ones.emplace_back(the_unique.id | iter->id, the_unique.forbidden_merges | iter->forbidden_merges);
		}
	}
	return newly_created_ones;
}


std::vector<collapse_node> all_nodes_size_9;

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

void helper_process_sub_colorings(
	std::shared_ptr<std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>> collapse_graph,
	const std::vector<std::string>& all_vars
) {
	std::ofstream save_max_sets("max_sets.txt", std::ios_base::app); //make parameter
	std::ofstream save_all_sets("all_sets.txt", std::ios_base::app); //make parameter
	std::mutex mutex_save_max_sets;


	//### check that not more than 128 variables!!!!
	if (!(all_vars.size() < collapse_node::N))
		throw 648358;

	std::list<std::vector<collapse_node>> all_sets;

	const auto get_id_index = [&](const std::string& s) -> std::size_t {
		std::size_t i{ 0 };
		for (; i < all_vars.size(); ++i) {
			if (all_vars[i] == s) break;
		}
		return i;
	};

	all_sets.emplace_back(); // size zero
	all_sets.emplace_back(); // size one

	// fill size one
	for (const auto& pair : *collapse_graph) {
		if (pair.first->size() != 1) throw 3;
		collapse_node::big_int id{ 0 };
		collapse_node::big_int forbidden_merges{ 0 };
		std::size_t i{ get_id_index(*pair.first->begin()) };
		id[i] = 1;
		for (const auto& element : pair.second) {
			std::size_t i{ get_id_index(*element->begin()) };
			forbidden_merges[i] = 1;
		}
		all_sets.back().emplace_back(id, forbidden_merges);
		std::stringstream ss;
		ss << "add node : " << id << " forbid: " << forbidden_merges;
		standard_logger().info(ss.str());
	}

	//all_sets.emplace_back(collapse_node::big_int(), collapse_node::big_int().set());

	all_sets.emplace_back(); // size two
	all_sets.emplace_back(); // size 3
	all_sets.emplace_back(); // size 4
	all_sets.emplace_back(); // size 5
	all_sets.emplace_back(); // size 6
	all_sets.emplace_back(); // size 7
	all_sets.emplace_back(); // size 8

	// load size nine from file
	all_sets.emplace_back(std::move(all_nodes_size_9));
	for (auto& elem : all_sets.back()) {

		std::vector<collapse_node>::iterator begin_single{ std::next(all_sets.begin(),1)->begin() };
		std::vector<collapse_node>::iterator end_single{ std::next(all_sets.begin(),1)->end() };

		for (auto iter = begin_single; iter != end_single; ++iter) {
			if ((iter->id & elem.id).any())
				elem.forbidden_merges |= iter->forbidden_merges;
		}
	}
	if (!std::is_sorted(all_sets.back().cbegin(), all_sets.back().cend(), COMP_BITSET)) {
		standard_logger().error("not sorted here");
		std::sort(all_sets.back().begin(), all_sets.back().end(), COMP_BITSET);
	}

	std::size_t next_free_index{ 10 };

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

		// double for last entry
		const auto write_all_sets_to_file = [&]() {

			std::size_t size{ 0 }, count{ 0 };
			if (!last_filled.empty()) size = last_filled.front().id.count();
			if (size == 9) return; ///#### debug code   ....  because of part which is already calculated
			count = last_filled.size();

			for (const auto& set : last_filled) {
				save_all_sets << set.id.to_string() << std::endl;
			}
			standard_logger().info(std::string("Wrote all sets to file:   size: ") + std::to_string(size) + ",   count: " + std::to_string(count));

		};

		all_sets.emplace_back();

		std::vector<collapse_node>& fill{ all_sets.back() };

		std::list<std::vector<collapse_node>> filtered_chain;
		std::mutex m_filtered_chain;


		// double for last entry
		const auto write_max_sets_to_file = [&]() {
			std::size_t size{ 0 };
			std::size_t count_max_sets{ 0 };
			if (!last_filled.empty()) size = last_filled.front().id.count();

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
				if (!std::is_sorted(created.cbegin(), created.cend(), COMP_BITSET)) {
					standard_logger().error("not sorted here");
					std::sort(created.begin(), created.end(), COMP_BITSET);
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
					std::merge(/*std::execution::parallel_policy(),*/ created.begin(), created.end(), another_to_merge_with.begin(), another_to_merge_with.end(), std::back_inserter(merged_vector));
					merged_vector.erase(std::unique(merged_vector.begin(), merged_vector.end()), merged_vector.end());
					standard_logger().info(std::string("Successfully merged two sets       thread - ID : ") + std::to_string(log_id));

					created.clear();
					created = std::move(merged_vector);
				}
			}
		};



		constexpr uint8_t count_produce_threads{ 16 };

		std::array<std::pair<std::vector<collapse_node>, std::mutex>, count_produce_threads> produced;

		std::array<std::thread, count_produce_threads> produce_threads;


		std::array<std::thread, count_produce_threads> produce_and_merge_threads;

		bool produce_process_done{ false };
		bool filter_process_done{ false };
		bool merge_process_done{ false };

		std::size_t filtered_chain_accumulated_size{ 0 };

		std::array<std::thread, count_produce_threads> filter_threads;

		std::array<std::thread, count_produce_threads> merge_threads;

		const auto produce = [&](int mod, int index, std::pair<std::vector<collapse_node>, std::mutex>& results) {
			int rounds = last_filled.size() - index;
			if (rounds < 0) return;
			rounds += mod - 1;
			rounds /= mod;
			if (rounds == 0) return;

			auto big = std::next(last_filled.begin(), index);

			for (int count = 1; true; ++count) {
				bool found_extension{ false };
				for (auto small = begin_single; small != end_single; ++small) {
					if ((big->id & small->id).any()) continue;
					if ((small->forbidden_merges & big->id).any()) continue;
					const auto new_id{ small->id | big->id };
					const auto new_forbidden{ small->forbidden_merges | big->forbidden_merges };
					//std::size_t wait{ 0 };
					{
						auto lock = std::unique_lock(results.second);
						results.first.emplace_back(new_id, new_forbidden);
						//wait = results.first.size();
					}
					found_extension = true;
					//if (wait > 900) std::this_thread::sleep_for(std::chrono::milliseconds(1000 * (wait - 900)));
				}
				if (!found_extension) {
					auto lock = std::unique_lock(mutex_save_max_sets);
					save_max_sets << big->id.to_string() << std::endl;
				}
				if (count == rounds) return;
				std::advance(big, mod);
			}
		};

		const auto filter = [&](int prod_index) {
			while (true) {
				std::vector<collapse_node> fetched;
				{
					auto lock = std::unique_lock(produced[prod_index].second);
					fetched = std::move(produced[prod_index].first);
					//standard_logger().info(std::string("                                                            filter_thread fetched    ID ") + std::to_string(prod_index) + "  size   " + std::to_string(fetched.size()));

					if (produce_process_done && fetched.empty()) {
						standard_logger().info(std::string("filter_thread done   ID ") + std::to_string(prod_index));
						return;
					}
				}
				if (fetched.empty()) std::this_thread::sleep_for(std::chrono::milliseconds(1000));


				std::sort(fetched.begin(), fetched.end(), COMP_BITSET);
				//standard_logger().info(std::string("filter_thread finished self-sort     ID ") + std::to_string(prod_index));

				fetched.erase(std::unique(fetched.begin(), fetched.end()), fetched.end());
				//standard_logger().info(std::string("filter_thread finished self-checking     ID ") + std::to_string(prod_index));


				{
					auto lock = std::unique_lock(m_filtered_chain);
					filtered_chain_accumulated_size += fetched.size();
					filtered_chain.push_back(std::move(fetched));
				}
			}
		};

		const auto merge = [&](int prod_index) {
			while (true) {
				std::vector<collapse_node> fetched1;
				std::vector<collapse_node> fetched2;
				std::vector<collapse_node> merged3;
				std::size_t before{ 0 };
				bool wait{ false };
				{
					auto lock = std::unique_lock(m_filtered_chain);
					if (filtered_chain.size() > 1) {
						fetched1 = std::move(filtered_chain.front());
						filtered_chain.pop_front();
						fetched2 = std::move(filtered_chain.front());
						filtered_chain.pop_front();
					}
					else {
						if (filter_process_done) {
							standard_logger().info(std::string("END of merge thread           ID ") + std::to_string(prod_index));
							return; // there are enough other threads performing merges.
						}
						wait = true;
					}
				}
				before = fetched1.size() + fetched2.size();
				if (wait) {
					std::this_thread::sleep_for(std::chrono::milliseconds(10000));
				}
				std::merge(/*std::execution::parallel_policy(),*/ fetched1.begin(), fetched1.end(), fetched2.begin(), fetched2.end(), std::back_inserter(merged3));
				merged3.erase(std::unique(merged3.begin(), merged3.end()), merged3.end());
				{
					auto lock = std::unique_lock(m_filtered_chain);
					filtered_chain_accumulated_size -= (before - merged3.size());
					filtered_chain.push_back(std::move(merged3));
				}
				//standard_logger().info(std::string("successful merge on thread           ID ") + std::to_string(prod_index));

			}
		};

		const auto progress_logger = [&]() {
			while (true) {
				{
					auto lock = std::unique_lock(m_filtered_chain);
					standard_logger().info(std::string("Current size of accumulated new sets:    ") + std::to_string(filtered_chain_accumulated_size));
				}
				for (unsigned long long i = 0; i < 60 * 60; ++i) {
					std::this_thread::sleep_for(std::chrono::seconds(1));
					if (merge_process_done) return;
				}
			}
		};

		/*
		for (uint8_t t = 0; t < count_produce_threads; ++t) {
			produce_threads[t] = std::thread(produce, count_produce_threads, t, std::ref(produced[t]));
		}

		for (uint8_t t = 0; t < count_produce_threads; ++t) {
			filter_threads[t] = std::thread(filter, t);
		}

		for (uint8_t t = 0; t < count_produce_threads; ++t) {
			merge_threads[t] = std::thread(merge, t);
		}
		auto progress_logger_thread = std::thread(progress_logger);

		// join all threads:
		for (uint8_t t = 0; t < count_produce_threads; ++t) {
			produce_threads[t].join();
		}
		produce_process_done = true;
		standard_logger().info("produce phase: done");

		for (uint8_t t = 0; t < count_produce_threads; ++t) {
			filter_threads[t].join();
		}
		filter_process_done = true;
		standard_logger().info("sort and filter phase: done");

		for (uint8_t t = 0; t < count_produce_threads; ++t) {
			merge_threads[t].join();
		}
		merge_process_done = true;
		standard_logger().info("merging: done");

		progress_logger_thread.join();
		*/

		std::thread write_max_sets_to_file_thread = std::thread(write_max_sets_to_file);
		std::thread write_all_sets_to_file_thread = std::thread(write_all_sets_to_file);

		for (uint8_t t = 0; t < count_produce_threads; ++t) {
			produce_and_merge_threads[t] = std::thread(produce_and_merge, t);
		}

		for (uint8_t t = 0; t < count_produce_threads; ++t) {
			produce_and_merge_threads[t].join();
		}

		write_max_sets_to_file_thread.join();
		write_all_sets_to_file_thread.join();

		fill = std::move(filtered_chain.front());

		auto filled_size{ fill.size() };

		standard_logger().info(std::string("successfully created sets with size  ") + std::to_string(next_free_index) + "  counting " + std::to_string(filled_size));

		if (filled_size == 0) {
			return;
		}

		++next_free_index;

	}

	return;

}

void process_sub_colorings(
	std::list<std::map<std::string, int>>& all_colorings,
	std::shared_ptr<std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>> collapse_graph,
	bool skip_output,
	std::mutex& mutex_all_colorings,
	const std::size_t max_threads,
	std::list<std::tuple<std::future<void>, std::shared_ptr<std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>>>>& futures,
	std::mutex& m_futures,
	std::vector<std::string> all_vars
) {



	/* do - undo - chain*/
	//std::list<activation_record> chain;

	//chain.emplace_back(skip_output);

	std::stringstream ss;
	ss << "initial collapse graph:\n\n";
	for (const auto& pair : *collapse_graph) {
		ss << "set  { ";
		for (const auto& name : *pair.first) {
			ss << name << ", ";
		}
		ss << " }\n";
		ss << "   forbidden merge partners:\n";
		for (const auto& sptr : pair.second) {
			ss << "      [" << *sptr->begin() << "]\n";
		}
	}
	standard_logger().info(ss.str());

	std::size_t ivar1 = 0;
	std::size_t ivar2 = 1;

	if (all_vars.size() < 2)
		throw 2;

	helper_process_sub_colorings(collapse_graph, all_vars);

}

void live_range_analysis(const file_token& ftoken, const std::map<std::string, int>& const_table, const std::string& var_name, std::vector<std::string>& excluded_vars, std::map<std::string, std::tuple<bool, int, std::set<std::string>, int>>& graph, live_var_map& live_vars) {
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


unsigned long long count_variables_of_coloring(const std::map<std::string, int>& coloring) {
	std::set<int> values;
	for (const auto& pair : coloring) values.insert(pair.second);
	return values.size();
}



void filter_colorings(std::list<std::pair<std::map<std::string, int>, unsigned long long>>& useful_colorings, std::list<std::map<std::string, int>>& all_colorings, std::mutex& m_all_colorings, bool& continue_running) {
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

int cli(int argc, char** argv) {

	standard_logger().info(std::string("Running: ") + argv[0]);

	std::shared_ptr<std::string> model_string_ptr;
	std::filesystem::path artifacts_path;

	if (argc >= 2) {
		std::ifstream model_ifstream;
		model_ifstream.open(argv[1]);
		model_string_ptr = std::make_shared<std::string>(std::istreambuf_iterator<char>(model_ifstream), std::istreambuf_iterator<char>());
	}

	if (argc >= 3) {
		artifacts_path = argv[2];
	}

	if (!model_string_ptr) {
		model_string_ptr = std::make_shared<std::string>(example_family());
	}

	// debug code:
	//std::ifstream model_ifstream;
	//model_ifstream.open(R"(..\..\Examples\bsp.prism)");
	//model_string_ptr = std::make_shared<std::string>(std::istreambuf_iterator<char>(model_ifstream), std::istreambuf_iterator<char>());


	// when here then all live set were computed.
	std::vector<std::string> excluded_vars{ "y_Integrator_44480461" };

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

	std::mutex mutex_all_colorings;
	std::list<std::map<std::string, int>> all_colorings;
	std::list<std::pair<std::map<std::string, int>, unsigned long long>> useful_colorings;
	bool continue_running{ true };

	//auto filter_thread = std::thread(filter_colorings, std::ref(useful_colorings), std::ref(all_colorings), std::ref(mutex_all_colorings), std::ref(continue_running));

	const std::size_t max_threads{ 8 };
	std::list<std::tuple<std::future<void>, std::shared_ptr<std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>>>> futures;
	std::mutex m_futures;

	std::vector<std::string> all_var_names;
	std::transform(graph.begin(), graph.end(), std::back_inserter(all_var_names), [](auto& pair) {return pair.first; });
	std::sort(all_var_names.begin(), all_var_names.end());

	process_sub_colorings(all_colorings, collapse_graph, false, mutex_all_colorings, max_threads, futures, m_futures, all_var_names);

	// wait for futures here
	while (!futures.empty()) {
		standard_logger().info("waiting for futures...");
		std::list<std::tuple<std::future<void>, std::shared_ptr<std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>>>>::iterator iter;
		{
			auto lock = std::unique_lock(m_futures);
			iter = futures.begin();
		}
		std::get<0>(*iter).wait();
		{
			auto lock = std::unique_lock(m_futures);
			futures.erase(iter);
		}
	}
	standard_logger().info("all futures finished!");
	continue_running = false;
	//filter_thread.join();
	all_colorings.clear();
	std::transform(useful_colorings.begin(), useful_colorings.end(), std::back_inserter(all_colorings), [](const std::pair<std::map<std::string, int>, unsigned long long>& x) { return x.first; });

	//### all colorings still unused here.
	/*+++++++++++++++++++++++++++++*/
	unsigned long long i{ 0 };
	for (const std::map<std::string, int>& coloring : all_colorings) {
		std::string reduced_model_file_name{ std::string("reduced_model") + std::to_string(i) + ".prism" };

		// for convenience: apply coloring into graph:
		for (const auto& pair : coloring) {
			color(graph[pair.first]) = pair.second;
		}
		std::ofstream output_model((artifacts_path / reduced_model_file_name).string());
		file_token reduced_model_parse_tree(ftoken);
		apply_coloring_to_file_token(reduced_model_parse_tree, var_name, const_table, live_vars, graph);
		print_model_to_stream(reduced_model_parse_tree, output_model);
		print_coloring_from_graph_with_color_annotations(graph, output_model);
		++i;
	}
	nlohmann::json meta_results;
	meta_results["count_models"] = i;
	auto json_file = std::ofstream((artifacts_path / "meta.json").string());
	json_file << meta_results.dump(3);


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
	return 0;
}



int main(int argc, char** argv)
{
	init_logger();

	{
		/* read and analyse all_sets file. */

		std::ifstream read_all_sets("all_sets.txt");

		std::string line;

		std::map<int, int> count;
		for (int x = 0; x < 20; ++x) {
			count[x] = 0;
		}

		while (std::getline(read_all_sets, line)) {
			++count[std::count(line.cbegin(), line.cend(), '1')];

			if (std::count(line.cbegin(), line.cend(), '1') == 9) {
				all_nodes_size_9.emplace_back(collapse_node::big_int(line), collapse_node::big_int());
			}
		}

		for (int x = 0; x < 20; ++x) {
			standard_logger().info(std::to_string(x) + "  :  " + std::to_string(count[x]));
		}
	}

	//return 0;
	return cli(argc, argv);

}
