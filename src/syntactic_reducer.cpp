
#include "logger.h"

#include "parser.h"
#include "debug_001.h"

#include <memory>


int cli(int argc, char** argv) {
	standard_logger().info("Start Parsing example...");

	auto ftoken = file_token(std::make_shared<std::string>(example_family()));

	//std::list<std::unique_ptr<token>> all_tokens{};
	//all_tokens.emplace_front(ftoken);

	//prism_dtmc::parse_it(all_tokens);
	ftoken.parse();

	bool check = ftoken.is_sound();
	
	return 0;
}

int main(int argc, char** argv)
{
	init_logger();

	return cli(argc, argv);

}
