
#include "logger.h"

#include "parser.h"
#include "debug_001.h"

#include <memory>


void test_something() {

}

int cli(int argc, char** argv) {
	test_something();
	standard_logger().info("Start Parsing example...");

	auto ftoken = file_token(std::make_shared<std::string>(example_family()));

	ftoken.parse();

	bool check = ftoken.is_sound_recursive();



	const std::map<std::string, int> const_table{ [&] {
		std::map<std::string, int> const_table;
		for (const auto& const_def : ftoken._dtmc_body_component->const_definitions()) {
			const_table[const_def->_constant_identifier->str()] = *const_def->_expression->get_value(const_table); // check nullptr?
		}
		return const_table;
	}() };




	// live range analysis:
	// calculate program graph:
	std::string var_name{ "cf" };
	std::string var_name_next{ var_name + "'" };

	using value_type = int;

	std::vector<std::tuple<value_type, value_type, std::shared_ptr<transition_token>, std::shared_ptr<condition_token>>> program_graph;

	auto module_defs = ftoken._dtmc_body_component->module_definitions();
	// assert correct size.
	auto& the_module{ module_defs.front() };

	for (const auto& s : the_module->_transitions) {
		const auto& transition = s.first; // ignore separating space

		auto contains_var = transition->_pre_condition->contains_variable(var_name);
		auto contains_var_next = transition->_pre_condition->contains_variable(var_name_next); // should be an error

		std::vector<std::pair<bool, bool>> relevant_post_conditions; // check if variable is contained.

		for (const auto& posts : transition->_post_conditions) {
			const auto& post_condition = std::get<3>(posts); // ignore probability
			relevant_post_conditions.push_back({
					post_condition->contains_variable(var_name),
					post_condition->contains_variable(var_name_next)
				});
		}

		if (contains_var) {
			std::shared_ptr<std::vector<int>> pre_values = transition->_pre_condition->get_values(var_name, const_table);
			for (int post_index = 0; post_index < relevant_post_conditions.size(); ++post_index) {
				const auto& post_condition = std::get<3>(transition->_post_conditions[post_index]);
				std::shared_ptr<std::vector<int>> post_values = post_condition->get_values(var_name_next, const_table);
				// check for no /more values here! ###
				program_graph.push_back(std::make_tuple((*pre_values)[0], (*post_values)[0], transition, post_condition));
			}
		}

	}


	std::map<std::pair<std::shared_ptr<transition_token>, std::shared_ptr<condition_token>>, std::vector<std::string>> gen_sets;
	std::map<std::pair<std::shared_ptr<transition_token>, std::shared_ptr<condition_token>>, std::vector<std::string>> kill_sets;

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

	std::map<int, std::vector<std::string>> live_vars;
	for (const auto& p : program_graph) {
		live_vars[std::get<0>(p)] = std::vector<std::string>();
		live_vars[std::get<1>(p)] = std::vector<std::string>();
	}

	const auto int_vector_union = [](const auto& l, const auto& r) {
		std::vector<std::string> copy = l;
		for (const auto& val : r) {
			if (std::find(copy.cbegin(), copy.cend(), val) == copy.cend()) {
				copy.push_back(val);
			}
		}
		return copy;
	};

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

	// all values for cf variable:
	std::map<int, bool> recalculate;
	for (const auto& edge : program_graph) {
		recalculate[std::get<0>(edge)] = true;
		recalculate[std::get<1>(edge)] = true;
	}


	while (true) {
		int x{ -1 };
		auto iter = recalculate.begin();
		for(; iter!= recalculate.cend(); ++iter) {
			if (iter->second == true) {
				x = iter->first;
				iter->second = false;
				break;
			}
		}
		if (iter == recalculate.cend()) break;
		// step:
		// predecessors:
		std::vector<int> successors;
		for (const auto& edge : program_graph) {
			if (std::get<0>(edge) == x) {
				successors.push_back(std::get<1>(edge));
			}
		}
		std::vector<std::string> new_lives = std::accumulate(successors.cbegin(), successors.cend(), std::vector<std::string>(), [&](const auto& acc, const auto& el) {
			std::pair<std::shared_ptr<transition_token>, std::shared_ptr<condition_token>> tr;
			for (const auto edge : program_graph) {
				if (std::get<0>(edge) == x && std::get<1>(edge) == el) tr = std::make_pair(std::get<2>(edge), std::get<3>(edge));
			}
			auto killed = int_vector_set_minus(live_vars[el], kill_sets[tr]);
			auto vec = int_vector_union(killed, gen_sets[tr]);

			return int_vector_union(acc, vec);
			});
		live_vars[x] = new_lives;
		// check if somethign changed, if so add to recalculate
	}

	return 0;
}

int main(int argc, char** argv)
{
	init_logger();

	return cli(argc, argv);

}
