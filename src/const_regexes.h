#pragma once

#include <boost/regex.hpp>

#include <string>

class const_regexes {
public:
	struct strings {

		class delimiter {
		public:
			inline static const auto dot_dot{ std::string(R"(..)") };
			inline static const auto double_quote{ std::string(R"(")") };
			inline static const auto left_parenthesis{ std::string(R"(()") };
			inline static const auto left_square_bracket{ std::string(R"([)") };
			inline static const auto right_parenthesis{ std::string(R"())") };
			inline static const auto right_square_bracket{ std::string(R"(])") };
		};

		class keywords {
		public:
			inline static const auto CONST_{ std::string(R"(const)") };
			inline static const auto ENDMODULE{ std::string(R"(endmodule)") };
			inline static const auto ENDREWARDS{ std::string(R"(endrewards)") };
			inline static const auto GLOBAL{ std::string(R"(global)") };
			inline static const auto INIT{ std::string(R"(init)") };
			inline static const auto MODULE{ std::string(R"(module)") };
			inline static const auto REWARDS{ std::string(R"(rewards)") };
		};


		class regulars {
		public:
			inline static const auto line_feed{ std::string(R"(\n)") };
			inline static const auto natural_number{ std::string(R"(([1-9][0-9]*)|0)") };
			inline static const auto single_space{ std::string(R"(\s)") };
			inline static const auto type_specifier{ std::string(R"(int)") };


			inline static const auto endinit_keyword{ std::string(R"(endinit)") };
			inline static const auto dtmc{ std::string(R"(dtmc)") };
			inline static const auto digit{ std::string(R"(\d)") };
			inline static const auto equals{ std::string(R"(=)") };
			inline static const auto formula{ std::string(R"(formula)") };
			inline static const auto identifier{ std::string(R"([_a-zA-Z][_a-zA-Z0-9']*)") };
			inline static const auto semicolon{ std::string(R"(;)") };
			inline static const auto right_square_brace{ std::string(R"(\])") };
			inline static const auto right_brace{ std::string(R"(\))") };
			inline static const auto colon{ std::string(R"(:)") };
			inline static const auto spaces{ std::string(R"(\s*)") }; //spaces, tabs, line breaks
			inline static const auto not_spaces{ std::string(R"(\S*)") }; //no spaces, tabs, line breaks
			inline static const auto spaces_plus{ std::string(R"(\s+)") }; //spaces, tabs, line breaks, at least one
			inline static const auto plus{ std::string(R"(\+)") };
			inline static const auto or_sign{ std::string(R"(\|)") };
			inline static const auto and_sign{ std::string(R"(\&)") };
			inline static const auto comparison_operator{ std::string(R"(!=|=||<=|>=|<|>)") };
			inline static const auto ascii_arrow{ std::string(R"(->)") };

		};


		class primitives {
		public:
			inline static const auto const_keyword{ std::string(R"(const)") };
			inline static const auto global_keyword{ std::string(R"(global)") };
			inline static const auto module_keyword{ std::string(R"(module)") };
			inline static const auto endmodule_keyword{ std::string(R"(endmodule)") };
			inline static const auto rewards_keyword{ std::string(R"(rewards)") };
			inline static const auto endrewards_keyword{ std::string(R"(endrewards)") };
			inline static const auto init_keyword{ std::string(R"(init)") };
			inline static const auto endinit_keyword{ std::string(R"(endinit)") };
			inline static const auto dtmc{ std::string(R"(dtmc)") };
			inline static const auto digit{ std::string(R"(\d)") };
			inline static const auto equals{ std::string(R"(=)") };
			inline static const auto formula{ std::string(R"(formula)") };
			inline static const auto identifier{ std::string(R"([_a-zA-Z][_a-zA-Z0-9']*)") };
			inline static const auto semicolon{ std::string(R"(;)") };
			inline static const auto left_square_brace{ std::string(R"(\[)") };
			inline static const auto left_brace{ std::string(R"(\()") };
			inline static const auto right_square_brace{ std::string(R"(\])") };
			inline static const auto right_brace{ std::string(R"(\))") };
			inline static const auto colon{ std::string(R"(:)") };
			inline static const auto double_quote{ std::string(R"(")") };
			inline static const auto spaces{ std::string(R"(\s*)") }; //spaces, tabs, line breaks
			inline static const auto not_spaces{ std::string(R"(\S*)") }; //no spaces, tabs, line breaks
			inline static const auto spaces_plus{ std::string(R"(\s+)") }; //spaces, tabs, line breaks, at least one
			inline static const auto plus{ std::string(R"(\+)") };
			inline static const auto or_sign{ std::string(R"(\|)") };
			inline static const auto and_sign{ std::string(R"(\&)") };
			inline static const auto comparison_operator{ std::string(R"(!=|=||<=|>=|<|>)") };
			inline static const auto type_specifier{ std::string(R"(int)") };
			inline static const auto dot_dot{ std::string(R"(\.\.)") };
			inline static const auto natural_number{ std::string(R"(([1-9][0-9]*)|0)") };
			inline static const auto ascii_arrow{ std::string(R"(->)") };

		};

		class clauses {
		public:

			inline static const auto const_definition{ std::string(R"((const\s[^;]*;))") };
			inline static const auto dtmc{ std::string("(dtmc)") };
			inline static const auto formula_definition{ std::string(R"((formula\s[^;]*;))") };
			inline static const auto global_definition{ std::string(R"((global\s[^;]*;))") };
			inline static const auto init_definition{ std::string(R"((init\s[\s\S]*?endinit))") };
			inline static const auto module_definition{ std::string(R"((module\s[\s\S]*?endmodule))") };
			inline static const auto reward_definition{ std::string(R"((rewards\s[\s\S]*?endrewards))") };
			inline static const auto transition{ std::string(R"((\[([_a-zA-Z][_a-zA-Z0-9]*)\][\s\S]*?;))") };
			inline static const auto subcondition{ std::string(R"(^\s*\([\s\S]*\)\s*$)") };

		};
	};

	struct primitives {
		inline static const auto const_keyword{ boost::regex(strings::primitives::const_keyword) };
		inline static const auto global_keyword{ boost::regex(strings::primitives::global_keyword) };
		inline static const auto module_keyword{ boost::regex(strings::primitives::module_keyword) };
		inline static const auto endmodule_keyword{ boost::regex(strings::primitives::endmodule_keyword) };
		inline static const auto rewards_keyword{ boost::regex(strings::primitives::rewards_keyword) };
		inline static const auto init_keyword{ boost::regex(strings::primitives::init_keyword) };
		inline static const auto endinit_keyword{ boost::regex(strings::primitives::endinit_keyword) };
		inline static const auto endrewards_keyword{ boost::regex(strings::primitives::endrewards_keyword) };
		inline static const auto dtmc{ boost::regex(strings::primitives::dtmc) };
		inline static const auto digit{ boost::regex(strings::primitives::digit) };
		inline static const auto double_quote{ boost::regex(strings::primitives::double_quote) };
		inline static const auto equals{ boost::regex(strings::primitives::equals) };
		inline static const auto formula{ boost::regex(strings::primitives::formula) };
		inline static const auto identifier{ boost::regex(strings::primitives::identifier) };
		inline static const auto semicolon{ boost::regex(strings::primitives::semicolon) };
		inline static const auto left_square_brace{ boost::regex(strings::primitives::left_square_brace) };
		inline static const auto left_brace{ boost::regex(strings::primitives::left_brace) };
		inline static const auto right_square_brace{ boost::regex(strings::primitives::right_square_brace) };
		inline static const auto right_brace{ boost::regex(strings::primitives::right_brace) };
		inline static const auto colon{ boost::regex(strings::primitives::colon) };
		inline static const auto spaces{ boost::regex(strings::primitives::spaces) };
		inline static const auto not_spaces{ boost::regex(strings::primitives::not_spaces) };
		inline static const auto spaces_plus{ boost::regex(strings::primitives::spaces_plus) };
		inline static const auto plus{ boost::regex(strings::primitives::plus) };
		inline static const auto or_sign{ boost::regex(strings::primitives::or_sign) };
		inline static const auto and_sign{ boost::regex(strings::primitives::and_sign) };
		inline static const auto comparison_operator{ boost::regex(strings::primitives::comparison_operator) };
		inline static const auto type_specifier{ boost::regex(strings::primitives::type_specifier) };
		inline static const auto dot_dot{ boost::regex(strings::primitives::dot_dot) };
		inline static const auto natural_number{ boost::regex(strings::primitives::natural_number) };
		inline static const auto ascii_arrow{ boost::regex(strings::primitives::ascii_arrow) };
	};

	struct clauses {
		inline static const auto dtmc{ boost::regex(strings::clauses::dtmc) };
		inline static const auto formula_definition{ boost::regex(strings::clauses::formula_definition) };
		inline static const auto const_definition{ boost::regex(strings::clauses::const_definition) };
		inline static const auto global_definition{ boost::regex(strings::clauses::global_definition) };
		inline static const auto module_definition{ boost::regex(strings::clauses::module_definition) };
		inline static const auto reward_definition{ boost::regex(strings::clauses::reward_definition) };
		inline static const auto transition{ boost::regex(strings::clauses::transition) };
		inline static const auto init_definition{ boost::regex(strings::clauses::init_definition) };
		inline static const auto subcondition{ boost::regex(strings::clauses::subcondition) };
	};
};