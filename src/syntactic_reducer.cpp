
#include "logger.h"

#include "parser.h"
#include "debug_001.h"

#include <memory>


int cli(int argc, char** argv) {
	standard_logger().info("Start Parsing example...");

	auto ftoken = file_token(std::make_shared<std::string>(example_family()));

	ftoken.parse();

	bool check = ftoken.is_sound_recursive();
	
	// live range analysis:
	// calculate program graph:
	std::string var_name{"cf"};
	std::string var_name_next{ var_name + "'" };

	using value_type = int;

	std::vector<std::tuple<value_type, value_type, std::shared_ptr<transition_token>>> program_graph;

	auto module_defs = ftoken._dtmc_body_component->module_definitions();
	// assert correct size.

	for (const auto& s : module_defs.front()->_transitions) {
		const auto& transition = s.first;

		auto contains_var = transition->_pre_condition->contains_variable(var_name);
		auto contains_var_next = transition->_pre_condition->contains_variable(var_name_next); // should be an error

		std::vector<std::pair<bool,bool>> relevant_post_conditions;

		for (const auto& posts : transition->_post_conditions) {
			const auto& post_condition = std::get<3>(posts);
			relevant_post_conditions.push_back({
					post_condition->contains_variable(var_name),
					post_condition->contains_variable(var_name_next)
				});
		}

		if (contains_var) {
			std::vector<int> pre_values = transition->_pre_condition.get_values(var_name);
		}

	}



	return 0;
}

int main(int argc, char** argv)
{
	init_logger();

	return cli(argc, argv);

}
