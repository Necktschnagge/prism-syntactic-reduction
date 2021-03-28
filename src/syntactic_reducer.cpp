
#include "logger.h"

#include "parser.h"
#include "debug_001.h"

#include <memory>
#include <unordered_set>
#include <iostream>
#include <fstream>



int cli(int argc, char** argv) {

	std::shared_ptr<std::string> model_string_ptr;

	if (argc >= 2) {
		std::ifstream model_ifstream;
		model_ifstream.open(argv[1]);
		model_string_ptr = std::make_shared<std::string>(std::istreambuf_iterator<char>(model_ifstream), std::istreambuf_iterator<char>());
	}

	if (argc >= 3) {
		std::ifstream prop_ifstream;
		prop_ifstream.open(argv[2]);
		auto property_list = std::make_shared<std::string>(std::istreambuf_iterator<char>(prop_ifstream), std::istreambuf_iterator<char>());
	}

	if (!model_string_ptr) {
		model_string_ptr = std::make_shared<std::string>(example_family());
	}


	// when here then all live set were computed.
	std::vector<std::string> excluded_vars{ "y_Integrator_44480461" };

	standard_logger().info("Start Parsing example...");
	auto ftoken = file_token(model_string_ptr);

	ftoken.parse();

	bool check = ftoken.is_sound_recursive();


	// values of const symbols:
	const std::map<std::string, int> const_table{ [&] {
		std::map<std::string, int> const_table;
		auto const_def_container = ftoken._dtmc_body_component->const_definitions();
		for (const auto& const_def : const_def_container) {
			const_table[const_def->_constant_identifier->str()] = *const_def->_expression->get_value(const_table); // check nullptr?
		}
		return const_table;
	}() };




	// live range analysis:
	// calculate program graph:
	std::string var_name{ "cf" };
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

	// fil gen and kill sets
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

	// state of cf variables |-> (must be recalculated, variables that are live there (current state), changes since last calculation)
	using liveness_tuple = std::tuple<bool, std::vector<std::string>, std::vector<std::string>>;
	using live_var_map = std::map<int, liveness_tuple>;
	live_var_map live_vars;

	const auto flag_of_liveness_tuple = [](liveness_tuple& tup) -> bool& { return std::get<0>(tup); };
	const auto current_of_liveness_tuple = [](liveness_tuple& tup) -> std::vector<std::string>&{ return std::get<1>(tup); };
	const auto changes_of_liveness_tuple = [](liveness_tuple& tup) { return std::get<2>(tup); };

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
				std::vector<std::string> real_additives = int_vector_set_minus(propagate, changes_of_liveness_tuple(from_state_liveness));
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
	std::map<std::string, std::tuple<bool, int, std::set<std::string> /*, std::set<std::string>*/, int>> graph; // node |-> (!removed, count neighbours, active and inactive neighbours, color)

	const auto is_active = [](std::tuple<bool, int, std::set<std::string> /*, std::set<std::string>*/, int>& t) -> bool& { return std::get<0>(t); };
	const auto count_active_neighbours = [](std::tuple<bool, int, std::set<std::string> /*, std::set<std::string>*/, int>& t) -> int& { return std::get<1>(t); };
	const auto neighbours = [](std::tuple<bool, int, std::set<std::string> /*, std::set<std::string>*/, int>& t) -> std::set<std::string>&{ return std::get<2>(t); };
	const auto color = [](std::tuple<bool, int, std::set<std::string> /*, std::set<std::string>*/, int>& t) -> int& { return std::get<3>(t); };

	for (auto& live_pair : live_vars) {
		const std::vector<std::string>& vector_of_incident_var_names{ current_of_liveness_tuple(live_pair.second) };
		for (auto iter = vector_of_incident_var_names.cbegin(); iter != vector_of_incident_var_names.cend(); ++iter) {
			neighbours(graph[*iter]).insert(vector_of_incident_var_names.cbegin(), iter);
			neighbours(graph[*iter]).insert(std::next(iter), vector_of_incident_var_names.cend());
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

	// output result:
	std::cout << "\n\n::::: color map :::::\n\n";
	for (auto& graph_pair : graph) {
		std::cout << "Color " << color(graph_pair.second) << " for var " << graph_pair.first << std::endl;
	}
	for (int i{ 0 }; i < std::numeric_limits<int>::max(); ++i) {
		std::vector<std::string> var_names;
		for (auto& graph_pair : graph) {
			if (color(graph_pair.second) == i) var_names.push_back(graph_pair.first);
		}
		if (var_names.empty()) break;
		std::cout << "Variables for color " << i << " :\n\t";
		for (const auto& s : var_names) std::cout << s << ", ";
		std::cout << "\n";
	}

	//Output in prism Format:
	standard_logger().info("prism format output\n");

	auto& top_level_children = ftoken.get_children();

	const std::function<void(token::token_list&)> print_model = [&](token::token_list& children_list) {
		for (auto& child : children_list) {
			if (child->is_primitive()) {
				if (dynamic_cast<identifier_token*>(child.get())) {
					const auto entry = graph.find(child->str());
					if (entry != graph.end())
						std::cout << "colored_" << color(entry->second);
					else {
						const auto entry = graph.find(child->str().substr(0, child->str().length() - 1));
						if (entry != graph.end())
							std::cout << "colored_" << color(entry->second) << "'";
						else std::cout << child->str();
					}
				}
				else std::cout << child->str();
			}
			else {
				print_model(child->get_children());
			}
		}
							};

	print_model(top_level_children);

	return 0;
}

int main(int argc, char** argv)
{
	init_logger();

	return cli(argc, argv);

}
