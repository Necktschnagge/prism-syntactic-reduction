
#include "logger.h"

#include "parser.h"
#include "debug_001.h"

#include <nlohmann/json.hpp>

#include <memory>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <filesystem>

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

void process_sub_colorings(
	std::list<std::map<std::string, int>>& all_colorings,
	std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>& collapse_graph,
	bool skip_output
) {
	using collapse_graph_t = std::remove_reference<decltype(collapse_graph)>::type;

	struct temporal_prohibiting_edge {
		std::shared_ptr<std::set<std::string>> key_set_ptr1;
		std::shared_ptr<std::set<std::string>> key_set_ptr2;
		bool can_be_joined{ false };
	};

	struct temporal_join {
		std::shared_ptr<std::set<std::string>> deleted_set;
		std::set<std::shared_ptr<std::set<std::string>>> deleted_set_neighbours;
		std::set<std::shared_ptr<std::set<std::string>>> neighbours_of_set1_before;
		std::list<std::shared_ptr<std::set<std::string>>> relink_to_set2_on_rollback_and_delete_set1_link;
		std::list<std::shared_ptr<std::set<std::string>>> relink_to_set2_on_rollback_and_keep_set1_link;
	};

	struct activation_record {
		bool skip_output;
		temporal_prohibiting_edge temp_edge;
		temporal_join temp_join;
		int phase; /* 0 -> start at beginning
					  1 -> rollback of temporal edge
				   */

		activation_record(bool skip_output, int phase = 0) : skip_output(skip_output), phase(phase) {}
	};
	/* do - undo - chain*/
	std::vector<activation_record> chain;

	chain.emplace_back(skip_output);

	while (!chain.empty()) {

		activation_record& ar{ chain.back() };

/*		if (chain.size() > 3000) {
			standard_logger().info("here");
		}
*/
		standard_logger().info(std::string("rec-depth: ") + std::to_string(chain.size()) + "\t\tphase: " + std::to_string(ar.phase)+"\t\t#colorings: " + std::to_string(all_colorings.size()));

		if (ar.phase == 0) {
			// output the current coloring if !skip_output
			if (!ar.skip_output) {
				int next_free_color{ 0 };
				std::map<std::string, int> the_extracted_coloring;
				for (auto iter = collapse_graph.cbegin(); iter != collapse_graph.cend(); ++iter) {
					for (auto jter = iter->first->cbegin(); jter != iter->first->cend(); ++jter) {// all variables within one color equivalence class
						the_extracted_coloring[*jter] = next_free_color;
					}
					++next_free_color;
				}
				all_colorings.push_back(the_extracted_coloring);
			}
			std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>::iterator set1;
			std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>::iterator set2;
			ar.temp_edge.can_be_joined = false;
			// search for the first two sets that can be collapsed
			for (auto iter = collapse_graph.begin(); iter != collapse_graph.end(); ++iter) {
				for (auto jter = std::next(iter); jter != collapse_graph.end(); ++jter) {
					// iterate over all possible collapse pairs here.
					auto search_prohibition_edge = iter->second.find(jter->first);
					ar.temp_edge.can_be_joined = (iter->second.end() == search_prohibition_edge);
					if (ar.temp_edge.can_be_joined) {
						set1 = iter;
						set2 = jter;
						goto found_merging_9876542834;
					}
				}
			}
		found_merging_9876542834:
			// case 0: no possible collapse: just return.
			if (!ar.temp_edge.can_be_joined) {
				chain.pop_back();
				continue;
			};

			standard_logger().info(std::string("Excluding possible merge found:\n") + to_string(*set1->first) + "\n" + to_string(*set2->first));

			//case 1
			// mark the joinable as not joinable and run subprocedure with skip = true
			set1->second.insert(set2->first);
			set2->second.insert(set1->first);
			ar.temp_edge.key_set_ptr1 = set1->first;
			ar.temp_edge.key_set_ptr2 = set2->first;

			ar.phase = 1; // rollback of temp edge
			chain.emplace_back(true);
			continue;
		}
		//process_sub_colorings(all_colorings, collapse_graph, /*var_mapped_to_color_class, */ true);
		if (ar.phase == 1) {
			std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>::iterator set1{collapse_graph.find(ar.temp_edge.key_set_ptr1) };
			std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>>::iterator set2{ collapse_graph.find(ar.temp_edge.key_set_ptr2) };
			standard_logger().info(std::string("Forcing possible merge found:\n") + to_string(*set1->first) + "\n" + to_string(*set2->first));

			//rollback
			set1->second.erase(set2->first);
			set2->second.erase(set1->first);

			// case 2: collapse the two sets and propagate into collapse_graph and var_mapped_to_color_class,
			// run sub_procedure with skip = false. 
			ar.temp_join.deleted_set = set2->first;
			ar.temp_join.deleted_set_neighbours = set2->second;
			ar.temp_join.neighbours_of_set1_before = set1->second;


			set1->first->insert(ar.temp_join.deleted_set->cbegin(), ar.temp_join.deleted_set->cend()); //join sets itself %01
			set1->second.insert(ar.temp_join.deleted_set_neighbours.cbegin(), ar.temp_join.deleted_set_neighbours.cend()); //link neighbours of deleted set2 to set1 %02


			for (auto iter = collapse_graph.begin(); iter != collapse_graph.end(); ++iter) { // replace shared_ptr to deleted set2 by shared_ptr to set1 where it can be found. %03
				auto find_del_set_link = iter->second.find(ar.temp_join.deleted_set);
				if (find_del_set_link != iter->second.end()) {
					iter->second.erase(find_del_set_link);
					auto [ignored_iterator, insert_took_place] = iter->second.insert(set1->first);
					if (insert_took_place)
						ar.temp_join.relink_to_set2_on_rollback_and_delete_set1_link.push_back(iter->first);
					else
						ar.temp_join.relink_to_set2_on_rollback_and_keep_set1_link.push_back(iter->first);
				}
			}
			collapse_graph.erase(set2); // delete entry %04

			ar.phase = 2;
			chain.emplace_back(false);
			continue;
			//process_sub_colorings(all_colorings, collapse_graph, false);
		}
		if (ar.phase == 2) {

			collapse_graph[ar.temp_join.deleted_set] = ar.temp_join.deleted_set_neighbours; // %04

			// %03
			for (const auto key : ar.temp_join.relink_to_set2_on_rollback_and_delete_set1_link) {
				collapse_graph[key].erase(ar.temp_edge.key_set_ptr1);
				collapse_graph[key].insert(ar.temp_join.deleted_set);
			}
			for (const auto key : ar.temp_join.relink_to_set2_on_rollback_and_keep_set1_link) {
				collapse_graph[key].insert(ar.temp_edge.key_set_ptr1);
			}

			collapse_graph[ar.temp_edge.key_set_ptr1] = std::move(ar.temp_join.neighbours_of_set1_before); // %02

			for (const std::string& var_name : *ar.temp_join.deleted_set) ar.temp_edge.key_set_ptr1->erase(var_name); // %01

			standard_logger().info(std::string("unwind merge:\n") + to_string(*ar.temp_edge.key_set_ptr1) + "\n" + to_string(*ar.temp_edge.key_set_ptr2));

			chain.pop_back();
			continue;
		}
	}
};

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

	std::map<std::shared_ptr<std::set<std::string>>, std::set<std::shared_ptr<std::set<std::string>>>> collapse_graph;
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
		auto local_neighbours = collapse_graph[single_node_set];
		for (auto jter = neighbours(iter->second).cbegin(); jter != neighbours(iter->second).cend(); ++jter) {
			local_neighbours.insert(var_mapped_to_color_class[*jter]);
		}
	}

	std::list<std::map<std::string, int>> all_colorings;


	process_sub_colorings(all_colorings, collapse_graph, false);

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

	return cli(argc, argv);

}
