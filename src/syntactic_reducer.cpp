

#include "logger.h"





int cli(int argc, char** argv) {
	standard_logger().info("Hello World");
	return 0;
}

int main(int argc, char** argv)
{
	init_logger();

	return cli(argc, argv);

}
