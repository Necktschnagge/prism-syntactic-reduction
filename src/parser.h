#pragma once

#include "const_regexes.h"
#include "internal_error.h"
#include "parse_error.h"


#include <algorithm>
#include <list>
#include <memory>
#include <numeric> // std::accumulate
#include <string>

class token {
public:
	using iterator = std::string::const_iterator;
	using regex_iterator = boost::regex_iterator<std::string::const_iterator>;
	using token_list = std::list<std::shared_ptr<token>>;

	template <class _MatchResults>
	static iterator match_begin(const _MatchResults& m) {
		return m.prefix().end();
	}

	template <class _MatchResults>
	static iterator match_end(const _MatchResults& m) {
		return m.suffix().begin();
	}

protected:
	token_list children;

	std::shared_ptr<const std::string> _file_content;
	iterator _begin;
	iterator _end;

public:

	token(std::shared_ptr<const std::string> file_content, iterator begin, iterator end) : _file_content(file_content), _begin(begin), _end(end) {}
	token(const token& parent_token, iterator begin, iterator end) : token(parent_token._file_content, begin, end) {}
	token(const token* parent_token, iterator begin, iterator end) : token(parent_token->_file_content, begin, end) {}

	virtual bool is_primitive() const = 0;
	virtual bool is_sound() const = 0;

	bool is_sound_recursive() const {
		return is_sound() && std::accumulate(children.cbegin(), children.cend(), true, [](auto acc, auto& child) { return acc && child->is_sound_recursive(); });
	}

private:

	virtual void parse_non_primitive() = 0;

public:

	void parse() {
		if (!is_primitive()) {
			parse_non_primitive();
		}
		for (auto& child : children) {
			if (child) child->parse();
		}
	}

	iterator cbegin() const { return _begin; }

	iterator cend() const { return _end; }

	std::string str() const { return std::string(cbegin(), cend()); }

};

class primitive_regex_token : public token {

	virtual boost::regex primitive_regex() const = 0;

public:

	using token::token;

	virtual void parse_non_primitive() final override {}

	virtual bool is_primitive() const final override { return true; }

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), primitive_regex());
	}

};

class const_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::const_keyword;
	}
};

class global_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::global_keyword;
	}
};

class module_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::module_keyword;
	}
};

class endmodule_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::endmodule_keyword;
	}
};
//### try to do this as a template? using pointer?

class type_specifier_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::type_specifier;
	}
};


class left_square_brace_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::left_square_brace;
	}
};

class left_brace_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::left_brace;
	}
};

class right_square_brace_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::right_square_brace;
	}
};

class right_brace_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::right_brace;
	}
};

class natural_number_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::natural_number;
	}
};

class two_dots_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::dot_dot;
	}
};

class space_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::spaces;
	}

};

class spaces_plus_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::spaces_plus;
	}

};

class formula_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::formula;
	}

};

class identifier_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::identifier;
	}

};

class equals_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::equals;
	}

};

class semicolon_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::semicolon;
	}

};

class colon_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::colon;
	}

};

class ascii_arrow_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::ascii_arrow;
	}

};

class plus_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::plus;
	}

};

class or_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives:: or ;
	}

};

class float_token : public token {
public:

	using token::token;

	virtual void parse_non_primitive() final override {}

	virtual bool is_primitive() const final override { return true; }

	virtual bool is_sound() const final override {
		try {
			float x = std::stod(std::string(cbegin(), cend()));
		}
		catch (...) {
			return false;
		}
		return true;
	}

};

class expression_token : public token {

public:

	using token::token;

	std::shared_ptr<token> _root_operator;
	std::shared_ptr<expression_token> _left_expression;
	std::shared_ptr<expression_token> _right_expression;
	std::shared_ptr<spaces_plus_token> _ignored_spaces;
	std::shared_ptr<identifier_token> _identifier;
	std::shared_ptr<expression_token> _child_expression;

	virtual void parse_non_primitive() final override {
		iterator rest_begin{ cbegin() };
		iterator rest_end{ cend() };

		auto search_equals_operator = regex_iterator(rest_begin, rest_end, const_regexes::primitives::equals);
		if (search_equals_operator != regex_iterator()) {
			_root_operator = std::make_shared<equals_token>(this, search_equals_operator->prefix().end(), search_equals_operator->suffix().begin());
			_left_expression = std::make_shared<expression_token>(this, search_equals_operator->prefix().begin(), search_equals_operator->prefix().end());
			_right_expression = std::make_shared<expression_token>(this, search_equals_operator->suffix().begin(), search_equals_operator->suffix().end());
			children.push_back(_left_expression);
			children.push_back(_root_operator);
			children.push_back(_right_expression);
			return;
		}

		auto search_leading_spaces = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces_plus);
		if (search_leading_spaces != regex_iterator() && search_leading_spaces->prefix().end() == rest_begin) {
			_ignored_spaces = std::make_shared<spaces_plus_token>(this, rest_begin, search_leading_spaces->suffix().begin());
			_child_expression = std::make_shared<expression_token>(this, search_leading_spaces->suffix().begin(), rest_end);
			children.push_back(_ignored_spaces);
			children.push_back(_child_expression);
			return;
		}
		auto search_trailing_spaces = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces_plus);
		if (search_leading_spaces != regex_iterator()) {
			// search for last match:
			while (true) {
				auto next = search_trailing_spaces;
				++next;
				if (next == regex_iterator()) break;
				search_trailing_spaces = next;
			}
			if (search_trailing_spaces->suffix().begin() == rest_end) {
				_child_expression = std::make_shared<expression_token>(this, rest_begin, search_trailing_spaces->prefix().end());
				_ignored_spaces = std::make_shared<spaces_plus_token>(this, search_trailing_spaces->prefix().end(), search_trailing_spaces->suffix().begin());
				children.push_back(_child_expression);
				children.push_back(_ignored_spaces);
				return;
			}
		}
		_identifier = std::make_shared<identifier_token>(this, rest_begin, rest_end);
		children.push_back(_identifier);
	}

	virtual bool is_primitive() const final override { return false; }

	virtual bool is_sound() const final override {
		return true;
	}

};

class condition_token : public token {

public:

	enum class type {
		OR, AND, SUB_CONDITION, EQUATION
	};

	using token::token;

	std::shared_ptr<token> _root_operator;
	std::shared_ptr<expression_token> _left_expression;
	std::shared_ptr<expression_token> _right_expression;
	std::shared_ptr<spaces_plus_token> _ignored_spaces;
	std::shared_ptr<identifier_token> _identifier;
	std::shared_ptr<expression_token> _child_expression;

	type _type;
	std::vector<std::shared_ptr<condition_token>> _sub_conditions;
	std::vector<std::shared_ptr<token>> _separators;

	virtual void parse_non_primitive() final override {
		iterator rest_begin{ cbegin() };
		iterator rest_end{ cend() };

		// search all braces
		std::vector<std::pair<iterator, int64_t>> open_braces;
		int64_t count{ 0 };
		if (rest_begin == rest_end) throw parse_error("Empty condition.");
		if (*rest_begin != '(') open_braces.push_back({ rest_begin,0 });
		for (iterator it = rest_begin; it != rest_end; ++it) {
			if (*it == '(') {
				++count;
				open_braces.push_back({ it,count });
			}
			if (*it == ')') {
				--count;
				open_braces.push_back({ it,count });
				if (count < 0) throw parse_error("More closing braces than opening braces in condition.");
			}
		}
		if (count != 0) throw parse_error("More opening braces than closing braces in condition.");
		open_braces.push_back({ rest_end, 0 });;

		const bool first_brace_matches_last_brace = [&] {
			uint_fast8_t count{ 0 };
			bool has_0_open_braces{ true };
			for (auto e : open_braces) {
				if (e.second > 1 && has_0_open_braces) {
					has_0_open_braces = false;
					++count;
				}
				if (e.second == 0) has_0_open_braces = true;
			}
			if (count > 1) return false;
			return true;
		}();

		//search outer ()
		if (boost::regex_match(rest_begin, rest_end, const_regexes::clauses::subcondition)
			&& first_brace_matches_last_brace) {
			_type = type::SUB_CONDITION;
			const auto IN_BRACES{ R"((?<=\()[\s\S]*(?=\)))" };
			auto search_sub_condition = regex_iterator(rest_begin, rest_end, boost::regex(IN_BRACES));
			parse_error::assert_true(search_sub_condition != regex_iterator(), R"(Could find sub-condition where expected.)");
			auto sub = std::make_shared<condition_token>(this, search_sub_condition->prefix().end(), search_sub_condition->suffix().begin());
			auto search_leading_spaces = regex_iterator(rest_begin, search_sub_condition->prefix().end(), const_regexes::primitives::spaces);
			parse_error::assert_true(search_leading_spaces != regex_iterator(), R"(Could find leading spaces before sub-condition.)");
			parse_error::assert_true(search_leading_spaces->prefix().end() == rest_begin, R"(Could not find space separator immediately at beginning.)");
			auto leading_spaces = std::make_shared<space_token>(this, search_leading_spaces->prefix().end(), search_leading_spaces->suffix().begin());
			auto left_brace = std::make_shared<left_brace_token>(this, search_leading_spaces->suffix().begin(), search_sub_condition->prefix().end());
			auto search_right_brace = regex_iterator(search_sub_condition->suffix().begin(), rest_end, const_regexes::primitives::right_brace);
			parse_error::assert_true(search_right_brace != regex_iterator(), R"x(Could find ")" after sub-condition.)x");
			parse_error::assert_true(search_right_brace->prefix().end() == search_sub_condition->suffix().begin(), R"x(Could find ")" immediately after sub-condition.)x");
			auto right_brace = std::make_shared<right_brace_token>(this, search_right_brace->prefix().end(), search_right_brace->suffix().begin());
			auto trailing_spaces = std::make_shared<space_token>(this, search_right_brace->suffix().begin(), rest_end);

			_separators.push_back(leading_spaces);
			_separators.push_back(left_brace);
			_sub_conditions.push_back(sub);
			_separators.push_back(right_brace);
			_separators.push_back(trailing_spaces);

			children.push_back(leading_spaces);
			children.push_back(left_brace);
			children.push_back(sub);
			children.push_back(right_brace);
			children.push_back(trailing_spaces);

			return;
		}

		auto search_separators_outside_braces = [&](const boost::regex& r_separator) {
			std::vector<std::pair<iterator, iterator>> separator_positions;
			auto tracer = open_braces.cbegin();
			for (auto search_separator = regex_iterator(rest_begin, rest_end, r_separator); search_separator != regex_iterator(); ++search_separator) {
				auto pos = search_separator->prefix().end();
				while (std::next(tracer)->first <= pos && /* just for memory safety, should be an implication of the left -> */ tracer != open_braces.cend()) ++tracer;
				// for checking if at zero brace depth
				if (tracer->second == 0) separator_positions.push_back({ pos, search_separator->suffix().begin() });
			}
			return separator_positions;
		};
		//search | not inside braces
		std::vector<std::pair<iterator, iterator>> or_positions = search_separators_outside_braces(const_regexes::primitives::or);
		if (!or_positions.empty()) { // it is a disjunction
			_type = type::OR;

			for (const auto& or_pos : or_positions) {
				auto sub = std::make_shared<condition_token>(this, rest_begin, or_pos.first);
				auto or = std::make_shared<or_token>(this, or_pos.first, or_pos.second);
				rest_begin = or_pos.second;
				_sub_conditions.push_back(sub);
				children.push_back(sub);
				_separators.push_back(or );
				children.push_back(or );
			}
			auto last_sub = std::make_shared<condition_token>(this, rest_begin, rest_end);
			_sub_conditions.push_back(last_sub);
			children.push_back(last_sub);
			return;
		}
		std::vector<std::pair<iterator, iterator>> and_positions = search_separators_outside_braces(const_regexes::primitives::and_sign);



		//search &
		//search =


		auto search_equals_operator = regex_iterator(rest_begin, rest_end, const_regexes::primitives::equals);
		if (search_equals_operator != regex_iterator()) {
			_root_operator = std::make_shared<equals_token>(this, search_equals_operator->prefix().end(), search_equals_operator->suffix().begin());
			_left_expression = std::make_shared<expression_token>(this, search_equals_operator->prefix().begin(), search_equals_operator->prefix().end());
			_right_expression = std::make_shared<expression_token>(this, search_equals_operator->suffix().begin(), search_equals_operator->suffix().end());
			children.push_back(_left_expression);
			children.push_back(_root_operator);
			children.push_back(_right_expression);
			return;
		}

		auto search_leading_spaces = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces_plus);
		if (search_leading_spaces != regex_iterator() && search_leading_spaces->prefix().end() == rest_begin) {
			_ignored_spaces = std::make_shared<spaces_plus_token>(this, rest_begin, search_leading_spaces->suffix().begin());
			_child_expression = std::make_shared<expression_token>(this, search_leading_spaces->suffix().begin(), rest_end);
			children.push_back(_ignored_spaces);
			children.push_back(_child_expression);
			return;
		}
		auto search_trailing_spaces = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces_plus);
		if (search_leading_spaces != regex_iterator()) {
			// search for last match:
			while (true) {
				auto next = search_trailing_spaces;
				++next;
				if (next == regex_iterator()) break;
				search_trailing_spaces = next;
			}
			if (search_trailing_spaces->suffix().begin() == rest_end) {
				_child_expression = std::make_shared<expression_token>(this, rest_begin, search_trailing_spaces->prefix().end());
				_ignored_spaces = std::make_shared<spaces_plus_token>(this, search_trailing_spaces->prefix().end(), search_trailing_spaces->suffix().begin());
				children.push_back(_child_expression);
				children.push_back(_ignored_spaces);
				return;
			}
		}
		_identifier = std::make_shared<identifier_token>(this, rest_begin, rest_end);
		children.push_back(_identifier);
	}

	virtual bool is_primitive() const final override { return false; }

	virtual bool is_sound() const final override {
		return true;
	}

};

class formula_definition_token : public token {
public:

	std::shared_ptr<formula_token> _formula_token;
	std::shared_ptr<spaces_plus_token> _formula_keyword_separator;
	std::shared_ptr<identifier_token> _formula_identifier;
	std::shared_ptr<space_token> _identifier_separator;
	std::shared_ptr<equals_token> _equals_token;
	std::shared_ptr<space_token> _equals_separator;
	std::shared_ptr<expression_token> _expression;
	std::shared_ptr<semicolon_token> _semicolon;

	using token::token;

	virtual void parse_non_primitive() override {
		iterator rest_begin{ cbegin() };
		iterator rest_end{ cend() };

		auto search_keyword = regex_iterator(rest_begin, rest_end, const_regexes::primitives::formula);
		parse_error::assert_true(search_keyword != regex_iterator(), R"(Could not find keyword "formula" in formula definition.)");
		parse_error::assert_true(search_keyword->prefix().end() == rest_begin, R"(Formula definition does not start with "formula".)");
		_formula_token = std::make_shared<formula_token>(this, rest_begin, search_keyword->suffix().begin());
		rest_begin = search_keyword->suffix().begin();

		auto search_space_separator_after_keyword = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces_plus);
		parse_error::assert_true(search_space_separator_after_keyword != regex_iterator(), R"(Could not find space separator after keyword "formula" in formula definition.)");
		parse_error::assert_true(search_space_separator_after_keyword->prefix().end() == rest_begin, R"(Could not find space separator immediately after keyword "formula" in formula definition.)");
		_formula_keyword_separator = std::make_shared<spaces_plus_token>(this, rest_begin, search_space_separator_after_keyword->suffix().begin());
		rest_begin = search_space_separator_after_keyword->suffix().begin();

		auto search_identifier = regex_iterator(rest_begin, rest_end, const_regexes::primitives::identifier);
		parse_error::assert_true(search_identifier != regex_iterator(), R"(Could not find an identifier after keyword "formula" in formula definition.)");
		parse_error::assert_true(search_identifier->prefix().end() == rest_begin, R"(Could not find an identifier immediately after keyword "formula" in formula definition.)");
		_formula_identifier = std::make_shared<identifier_token>(this, rest_begin, search_identifier->suffix().begin());
		rest_begin = search_identifier->suffix().begin();

		auto search_space_separator_after_identifier = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_space_separator_after_identifier != regex_iterator(), R"(Could not find space separators after identifier in formula definition.)");
		parse_error::assert_true(search_space_separator_after_identifier->prefix().end() == rest_begin, R"(Could not find space separator immediately after identifier in formula definition.)");
		_identifier_separator = std::make_shared<space_token>(this, rest_begin, search_space_separator_after_identifier->suffix().begin());
		rest_begin = search_space_separator_after_identifier->suffix().begin();

		auto search_equals_symbol = regex_iterator(rest_begin, rest_end, const_regexes::primitives::equals);
		parse_error::assert_true(search_equals_symbol != regex_iterator(), R"(Could not find "=" after identifier in formula definition.)");
		parse_error::assert_true(search_equals_symbol->prefix().end() == rest_begin, R"(Could not find "=" immediately after identifier in formula definition.)");
		_equals_token = std::make_shared<equals_token>(this, rest_begin, search_equals_symbol->suffix().begin());
		rest_begin = search_equals_symbol->suffix().begin();

		auto search_space_separator_after_equals_symbol = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_space_separator_after_equals_symbol != regex_iterator(), R"(Could not find space separators after "=" in formula definition.)");
		parse_error::assert_true(search_space_separator_after_equals_symbol->prefix().end() == rest_begin, R"(Could not find space separator immediately after "=" in formula definition.)");
		_equals_separator = std::make_shared<space_token>(this, rest_begin, search_space_separator_after_equals_symbol->suffix().begin());
		rest_begin = search_space_separator_after_equals_symbol->suffix().begin();

		auto search_semicolon = regex_iterator(rest_begin, rest_end, const_regexes::primitives::semicolon);
		parse_error::assert_true(search_semicolon != regex_iterator(), R"(Could not find semicolon at the end of formula definition.)");
		parse_error::assert_true(search_semicolon->suffix().begin() == rest_end, R"(Unexpected semicolon in formula definition.)");
		_semicolon = std::make_shared<semicolon_token>(this, search_semicolon->prefix().end(), search_semicolon->suffix().begin());
		rest_end = search_semicolon->prefix().end();

		_expression = std::make_shared<expression_token>(this, rest_begin, rest_end); // just assume rest to be an expression

		children.push_back(_formula_token);
		children.push_back(_formula_keyword_separator);
		children.push_back(_formula_identifier);
		children.push_back(_identifier_separator);
		children.push_back(_equals_token);
		children.push_back(_equals_separator);
		children.push_back(_expression);
		children.push_back(_semicolon);
	}

	virtual bool is_primitive() const { return false; }

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), const_regexes::clauses::formula_definition);
	}

};

class const_definition_token : public token {//##
public:

	using token::token;

	std::shared_ptr<const_token> _const_token;
	std::shared_ptr<spaces_plus_token> _const_separator;
	std::shared_ptr<type_specifier_token> _type_specifier_token;
	std::shared_ptr<spaces_plus_token> _type_separator;
	std::shared_ptr<identifier_token> _constant_identifier;
	std::shared_ptr<space_token> _identifier_separator;
	std::shared_ptr<equals_token> _equals_token;
	std::shared_ptr<space_token> _equals_separator;
	std::shared_ptr<expression_token> _expression;
	std::shared_ptr<semicolon_token> _semicolon;

	virtual void parse_non_primitive() override {
		iterator rest_begin{ cbegin() };
		iterator rest_end{ cend() };

		auto search_keyword = regex_iterator(rest_begin, rest_end, const_regexes::primitives::const_keyword);
		parse_error::assert_true(search_keyword != regex_iterator(), R"(Could not find keyword "const" in const definition.)");
		parse_error::assert_true(search_keyword->prefix().end() == rest_begin, R"(Const definition does not start with "const".)");
		_const_token = std::make_shared<const_token>(this, rest_begin, search_keyword->suffix().begin());
		rest_begin = search_keyword->suffix().begin();

		auto search_space_separator_after_keyword = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces_plus);
		parse_error::assert_true(search_space_separator_after_keyword != regex_iterator(), R"(Could not find space separator after keyword "const" in const definition.)");
		parse_error::assert_true(search_space_separator_after_keyword->prefix().end() == rest_begin, R"(Could not find space separator immediately after keyword "const" in const definition.)");
		_const_separator = std::make_shared<spaces_plus_token>(this, rest_begin, search_space_separator_after_keyword->suffix().begin());
		rest_begin = search_space_separator_after_keyword->suffix().begin();

		auto search_type = regex_iterator(rest_begin, rest_end, const_regexes::primitives::type_specifier);
		parse_error::assert_true(search_type != regex_iterator(), R"(Could not find a type specifier after keyword "const" in const definition.)");
		parse_error::assert_true(search_type->prefix().end() == rest_begin, R"(Could not find a type specifie immediately after keyword "const" in const definition.)");
		_type_specifier_token = std::make_shared<type_specifier_token>(this, rest_begin, search_type->suffix().begin());
		rest_begin = search_type->suffix().begin();

		auto search_space_separator_after_type = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces_plus);
		parse_error::assert_true(search_space_separator_after_type != regex_iterator(), R"(Could not find space separator after type specifier in const definition.)");
		parse_error::assert_true(search_space_separator_after_type->prefix().end() == rest_begin, R"(Could not find space separator immediately after type specifier in const definition.)");
		_type_separator = std::make_shared<spaces_plus_token>(this, rest_begin, search_space_separator_after_type->suffix().begin());
		rest_begin = search_space_separator_after_type->suffix().begin();

		auto search_identifier = regex_iterator(rest_begin, rest_end, const_regexes::primitives::identifier);
		parse_error::assert_true(search_identifier != regex_iterator(), R"(Could not find an identifier after type specifier in const definition.)");
		parse_error::assert_true(search_identifier->prefix().end() == rest_begin, R"(Could not find an identifier immediately after type specifier in const definition.)");
		_constant_identifier = std::make_shared<identifier_token>(this, rest_begin, search_identifier->suffix().begin());
		rest_begin = search_identifier->suffix().begin();

		auto search_space_separator_after_identifier = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_space_separator_after_identifier != regex_iterator(), R"(Could not find space separators after identifier in const definition.)");
		parse_error::assert_true(search_space_separator_after_identifier->prefix().end() == rest_begin, R"(Could not find space separator immediately after identifier in const definition.)");
		_identifier_separator = std::make_shared<space_token>(this, rest_begin, search_space_separator_after_identifier->suffix().begin());
		rest_begin = search_space_separator_after_identifier->suffix().begin();

		auto search_equals_symbol = regex_iterator(rest_begin, rest_end, const_regexes::primitives::equals);
		parse_error::assert_true(search_equals_symbol != regex_iterator(), R"(Could not find "=" after identifier in const definition.)");
		parse_error::assert_true(search_equals_symbol->prefix().end() == rest_begin, R"(Could not find "=" immediately after identifier in const definition.)");
		_equals_token = std::make_shared<equals_token>(this, rest_begin, search_equals_symbol->suffix().begin());
		rest_begin = search_equals_symbol->suffix().begin();

		auto search_space_separator_after_equals_symbol = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_space_separator_after_equals_symbol != regex_iterator(), R"(Could not find space separators after "=" in const definition.)");
		parse_error::assert_true(search_space_separator_after_equals_symbol->prefix().end() == rest_begin, R"(Could not find space separator immediately after "=" in const definition.)");
		_equals_separator = std::make_shared<space_token>(this, rest_begin, search_space_separator_after_equals_symbol->suffix().begin());
		rest_begin = search_space_separator_after_equals_symbol->suffix().begin();

		auto search_semicolon = regex_iterator(rest_begin, rest_end, const_regexes::primitives::semicolon);
		parse_error::assert_true(search_semicolon != regex_iterator(), R"(Could not find semicolon at the end of const definition.)");
		parse_error::assert_true(search_semicolon->suffix().begin() == rest_end, R"(Unexpected semicolon in const definition.)");
		_semicolon = std::make_shared<semicolon_token>(this, search_semicolon->prefix().end(), search_semicolon->suffix().begin());
		rest_end = search_semicolon->prefix().end();

		_expression = std::make_shared<expression_token>(this, rest_begin, rest_end); // just assume rest to be an expression

		children.push_back(_const_token);
		children.push_back(_const_separator);
		children.push_back(_type_specifier_token);
		children.push_back(_type_separator);
		children.push_back(_constant_identifier);
		children.push_back(_identifier_separator);
		children.push_back(_equals_token);
		children.push_back(_equals_separator);
		children.push_back(_expression);
		children.push_back(_semicolon);
	}

	virtual bool is_primitive() const { return false; }

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), const_regexes::clauses::const_definition);
	}
};

class global_definition_token : public token {//##
public:

	using token::token;

	std::shared_ptr<global_token> _global_token;
	std::shared_ptr<spaces_plus_token> _global_separator;
	std::shared_ptr<identifier_token> _global_identifier;
	std::shared_ptr<space_token> _identifier_separator;
	std::shared_ptr<colon_token> _colon_token;
	std::shared_ptr<space_token> _colon_separator;
	std::shared_ptr<left_square_brace_token> _left_brace;
	std::shared_ptr<space_token> _left_brace_separator;
	std::shared_ptr<natural_number_token> _lower_bound;
	std::shared_ptr<space_token> _lower_bound_separator;
	std::shared_ptr<two_dots_token> _dots;
	std::shared_ptr<space_token> _dots_separator;
	std::shared_ptr<natural_number_token> _upper_bound;
	std::shared_ptr<space_token> _upper_bound_separator;
	std::shared_ptr<right_square_brace_token> _right_brace;
	std::shared_ptr<space_token> _right_brace_separator;
	std::shared_ptr<semicolon_token> _semicolon;


	virtual void parse_non_primitive() override {
		/* global cf : [0 .. 142]; */
		iterator rest_begin{ cbegin() };
		iterator rest_end{ cend() };

		auto search_keyword = regex_iterator(rest_begin, rest_end, const_regexes::primitives::global_keyword);
		parse_error::assert_true(search_keyword != regex_iterator(), R"(Could not find keyword "global" in global definition.)");
		parse_error::assert_true(search_keyword->prefix().end() == rest_begin, R"(Global definition does not start with "global".)");
		_global_token = std::make_shared<global_token>(this, rest_begin, search_keyword->suffix().begin());
		rest_begin = search_keyword->suffix().begin();

		auto search_space_separator_after_keyword = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces_plus);
		parse_error::assert_true(search_space_separator_after_keyword != regex_iterator(), R"(Could not find space separator after keyword "global" in global definition.)");
		parse_error::assert_true(search_space_separator_after_keyword->prefix().end() == rest_begin, R"(Could not find space separator immediately after keyword "global" in global definition.)");
		_global_separator = std::make_shared<spaces_plus_token>(this, rest_begin, search_space_separator_after_keyword->suffix().begin());
		rest_begin = search_space_separator_after_keyword->suffix().begin();

		auto search_identifier = regex_iterator(rest_begin, rest_end, const_regexes::primitives::identifier);
		parse_error::assert_true(search_identifier != regex_iterator(), R"(Could not find an identifier after keyword "global" in global definition.)");
		parse_error::assert_true(search_identifier->prefix().end() == rest_begin, R"(Could not find an identifier immediately after keyword "global" in global definition.)");
		_global_identifier = std::make_shared<identifier_token>(this, rest_begin, search_identifier->suffix().begin());
		rest_begin = search_identifier->suffix().begin();

		auto search_space_separator_after_identifier = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_space_separator_after_identifier != regex_iterator(), R"(Could not find space separators after identifier in global definition.)");
		parse_error::assert_true(search_space_separator_after_identifier->prefix().end() == rest_begin, R"(Could not find space separator immediately after identifier in global definition.)");
		_identifier_separator = std::make_shared<space_token>(this, rest_begin, search_space_separator_after_identifier->suffix().begin());
		rest_begin = search_space_separator_after_identifier->suffix().begin();

		auto search_colon = regex_iterator(rest_begin, rest_end, const_regexes::primitives::colon);
		parse_error::assert_true(search_colon != regex_iterator(), R"(Could not find ":" after identifier in global definition.)");
		parse_error::assert_true(search_colon->prefix().end() == rest_begin, R"(Could not find ":" immediately after identifier in global definition.)");
		_colon_token = std::make_shared<colon_token>(this, rest_begin, search_colon->suffix().begin());
		rest_begin = search_colon->suffix().begin();

		auto search_space_separator_after_colon = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_space_separator_after_colon != regex_iterator(), R"(Could not find space separators after ":" in global definition.)");
		parse_error::assert_true(search_space_separator_after_colon->prefix().end() == rest_begin, R"(Could not find space separator immediately after ":" in global definition.)");
		_colon_separator = std::make_shared<space_token>(this, rest_begin, search_space_separator_after_colon->suffix().begin());
		rest_begin = search_space_separator_after_colon->suffix().begin();

		auto search_left_square_brace = regex_iterator(rest_begin, rest_end, const_regexes::primitives::left_square_brace);
		parse_error::assert_true(search_left_square_brace != regex_iterator(), R"(Could not find left square brace after colon in global definition.)");
		parse_error::assert_true(search_left_square_brace->prefix().end() == rest_begin, R"(left square brace at unexpected position.)");
		_left_brace = std::make_shared<left_square_brace_token>(this, rest_begin, search_left_square_brace->suffix().begin());
		rest_begin = search_left_square_brace->suffix().begin();

		auto search_space_separator_after_left_brace = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_space_separator_after_left_brace != regex_iterator(), R"(Could not find space separators after "[" in global definition.)");
		parse_error::assert_true(search_space_separator_after_left_brace->prefix().end() == rest_begin, R"(Could not find space separator immediately after "[" in global definition.)");
		_left_brace_separator = std::make_shared<space_token>(this, rest_begin, search_space_separator_after_left_brace->suffix().begin());
		rest_begin = search_space_separator_after_left_brace->suffix().begin();

		auto search_lower_bound = regex_iterator(rest_begin, rest_end, const_regexes::primitives::natural_number);
		parse_error::assert_true(search_left_square_brace != regex_iterator(), R"(Could not find lower bound in global definition.)");
		parse_error::assert_true(search_left_square_brace->prefix().end() == rest_begin, R"(lower bound at unexpected position.)");
		_lower_bound = std::make_shared<natural_number_token>(this, rest_begin, search_lower_bound->suffix().begin());
		rest_begin = search_lower_bound->suffix().begin();

		auto search_lower_bound_separator = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_lower_bound_separator != regex_iterator(), R"(Could not find space separators after lower bound in global definition.)");
		parse_error::assert_true(search_lower_bound_separator->prefix().end() == rest_begin, R"(Could not find space separator immediately after lower bound in global definition.)");
		_lower_bound_separator = std::make_shared<space_token>(this, rest_begin, search_lower_bound_separator->suffix().begin());
		rest_begin = search_lower_bound_separator->suffix().begin();

		auto search_dots = regex_iterator(rest_begin, rest_end, const_regexes::primitives::dot_dot);
		parse_error::assert_true(search_dots != regex_iterator(), R"(Could not find dots after lower bound in global definition.)");
		parse_error::assert_true(search_dots->prefix().end() == rest_begin, R"(Could not find dots immediately after lower bound in global definition.)");
		_dots = std::make_shared<two_dots_token>(this, rest_begin, search_dots->suffix().begin());
		rest_begin = search_dots->suffix().begin();

		auto search_dots_separator = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_dots_separator != regex_iterator(), R"(Could not find space separators after dots in global definition.)");
		parse_error::assert_true(search_dots_separator->prefix().end() == rest_begin, R"(Could not find space separator immediately after dots in global definition.)");
		_dots_separator = std::make_shared<space_token>(this, rest_begin, search_dots_separator->suffix().begin());
		rest_begin = search_dots_separator->suffix().begin();

		auto search_upper_bound = regex_iterator(rest_begin, rest_end, const_regexes::primitives::natural_number);
		parse_error::assert_true(search_upper_bound != regex_iterator(), R"(Could not find upper bound in global definition.)");
		parse_error::assert_true(search_upper_bound->prefix().end() == rest_begin, R"(upper bound at unexpected position.)");
		_upper_bound = std::make_shared<natural_number_token>(this, rest_begin, search_upper_bound->suffix().begin());
		rest_begin = search_upper_bound->suffix().begin();

		auto search_upper_bound_separator = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_upper_bound_separator != regex_iterator(), R"(Could not find space separators after upper bound in global definition.)");
		parse_error::assert_true(search_upper_bound_separator->prefix().end() == rest_begin, R"(Could not find space separator immediately after upper bound in global definition.)");
		_upper_bound_separator = std::make_shared<space_token>(this, rest_begin, search_upper_bound_separator->suffix().begin());
		rest_begin = search_upper_bound_separator->suffix().begin();

		auto search_right_square_brace = regex_iterator(rest_begin, rest_end, const_regexes::primitives::right_square_brace);
		parse_error::assert_true(search_right_square_brace != regex_iterator(), R"(Could not find right square brace after upper bound in global definition.)");
		parse_error::assert_true(search_right_square_brace->prefix().end() == rest_begin, R"(Right square brace at unexpected position.)");
		_right_brace = std::make_shared<right_square_brace_token>(this, rest_begin, search_right_square_brace->suffix().begin());
		rest_begin = search_right_square_brace->suffix().begin();

		auto search_right_brace_separator = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_right_brace_separator != regex_iterator(), R"(Could not find space separators after upper bound in global definition.)");
		parse_error::assert_true(search_right_brace_separator->prefix().end() == rest_begin, R"(Could not find space separator immediately after upper bound in global definition.)");
		_right_brace_separator = std::make_shared<space_token>(this, rest_begin, search_right_brace_separator->suffix().begin());
		rest_begin = search_right_brace_separator->suffix().begin();

		auto search_semicolon = regex_iterator(rest_begin, rest_end, const_regexes::primitives::semicolon);
		parse_error::assert_true(search_semicolon != regex_iterator(), R"(Could not find semicolon at the end of global definition.)");
		parse_error::assert_true(search_semicolon->suffix().begin() == rest_end, R"(Semicolon not at the end of global definition.)");
		parse_error::assert_true(search_semicolon->prefix().end() == rest_begin, R"(Unexpected characters before semicolon at the end of global definition.)");
		_semicolon = std::make_shared<semicolon_token>(this, search_semicolon->prefix().end(), search_semicolon->suffix().begin());
		rest_end = search_semicolon->prefix().end();

		children.push_back(_global_token);
		children.push_back(_global_separator);
		children.push_back(_global_identifier);
		children.push_back(_identifier_separator);
		children.push_back(_colon_token);
		children.push_back(_colon_separator);
		children.push_back(_left_brace);
		children.push_back(_left_brace_separator);
		children.push_back(_lower_bound);
		children.push_back(_lower_bound_separator);
		children.push_back(_dots);
		children.push_back(_dots_separator);
		children.push_back(_upper_bound);
		children.push_back(_upper_bound_separator);
		children.push_back(_right_brace);
		children.push_back(_right_brace_separator);
		children.push_back(_semicolon);
	}

	virtual bool is_primitive() const { return false; }

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), const_regexes::clauses::global_definition);
	}
};

class transition_token : public token {
	using token::token;

	std::shared_ptr<left_square_brace_token> _start_label;
	std::shared_ptr<space_token> _start_label_separator;
	std::shared_ptr<identifier_token> _label;
	std::shared_ptr<space_token> _label_separator;
	std::shared_ptr<right_square_brace_token> _end_label;
	std::shared_ptr<space_token> _right_brace_separator;
	std::shared_ptr<condition_token> _pre_condition;
	//std::shared_ptr<space_token> _pre_condition_separator;
	std::shared_ptr<ascii_arrow_token> _arrow;
	std::shared_ptr<space_token> _arrow_separator;
	std::vector<
		std::tuple<
		std::shared_ptr<float_token>,
		std::shared_ptr<space_token>,
		std::shared_ptr<colon_token>,
		//std::shared_ptr<space_token>,
		std::shared_ptr<condition_token>,
		//std::shared_ptr<space_token>,
		std::optional<
		std::tuple<
		std::shared_ptr<plus_token>,
		std::shared_ptr<space_token>
		>
		>
		>
	> _post_conditions;
	std::shared_ptr<semicolon_token> _semicolon;

	virtual void parse_non_primitive() override {

		iterator rest_begin{ cbegin() };
		iterator rest_end{ cend() };

		auto search_left_square_brace = regex_iterator(rest_begin, rest_end, const_regexes::primitives::left_square_brace);
		parse_error::assert_true(search_left_square_brace != regex_iterator(), R"(Could not find left square brace in transition.)");
		parse_error::assert_true(search_left_square_brace->prefix().end() == rest_begin, R"(left square brace at unexpected position.)");
		_start_label = std::make_shared<left_square_brace_token>(this, rest_begin, search_left_square_brace->suffix().begin());
		rest_begin = search_left_square_brace->suffix().begin();

		auto search_start_label_separator = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_start_label_separator != regex_iterator(), R"(Could not find space separators after "[" in transition.)");
		parse_error::assert_true(search_start_label_separator->prefix().end() == rest_begin, R"(Could not find space separators immediately after "[" in transition.)");
		_start_label_separator = std::make_shared<space_token>(this, rest_begin, search_start_label_separator->suffix().begin());
		rest_begin = search_start_label_separator->suffix().begin();

		auto search_identifier = regex_iterator(rest_begin, rest_end, const_regexes::primitives::identifier);
		parse_error::assert_true(search_identifier != regex_iterator(), R"(Could not find an identifier after "[" in transition.)");
		parse_error::assert_true(search_identifier->prefix().end() == rest_begin, R"(Could not find an identifier immediately after "[" in transition.)");
		_label = std::make_shared<identifier_token>(this, rest_begin, search_identifier->suffix().begin());
		rest_begin = search_identifier->suffix().begin();

		auto search_space_separator_after_identifier = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_space_separator_after_identifier != regex_iterator(), R"(Could not find space separators after identifier in transition.)");
		parse_error::assert_true(search_space_separator_after_identifier->prefix().end() == rest_begin, R"(Could not find space separator immediately after identifier transition.)");
		_label_separator = std::make_shared<space_token>(this, rest_begin, search_space_separator_after_identifier->suffix().begin());
		rest_begin = search_space_separator_after_identifier->suffix().begin();

		auto search_right_square_brace = regex_iterator(rest_begin, rest_end, const_regexes::primitives::right_square_brace);
		parse_error::assert_true(search_right_square_brace != regex_iterator(), R"(Could not find right square brace after identifier in transition.)");
		parse_error::assert_true(search_right_square_brace->prefix().end() == rest_begin, R"(Right square brace at unexpected position.)");
		_end_label = std::make_shared<right_square_brace_token>(this, rest_begin, search_right_square_brace->suffix().begin());
		rest_begin = search_right_square_brace->suffix().begin();

		auto search_right_square_brace_separator = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_right_square_brace_separator != regex_iterator(), R"(Could not find space separators after "]" in transition.)");
		parse_error::assert_true(search_right_square_brace_separator->prefix().end() == rest_begin, R"(Could not find space separator immediately after "]" transition.)");
		_right_brace_separator = std::make_shared<space_token>(this, rest_begin, search_right_square_brace_separator->suffix().begin());
		rest_begin = search_right_square_brace_separator->suffix().begin();

		auto search_arrow = regex_iterator(rest_begin, rest_end, const_regexes::primitives::ascii_arrow);
		parse_error::assert_true(search_arrow != regex_iterator(), R"(Could not find "->" in transition.)");
		_arrow = std::make_shared<ascii_arrow_token>(this, search_arrow->prefix().end(), search_arrow->suffix().begin());
		_pre_condition = std::make_shared<condition_token>(this, rest_begin, search_arrow->prefix().end());
		rest_begin = search_arrow->suffix().begin();

		auto search_arrow_separator = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_arrow_separator != regex_iterator(), R"(Could not find space separators after "->" in transition.)");
		parse_error::assert_true(search_arrow_separator->prefix().end() == rest_begin, R"(Could not find space separators immediately after "->" in transition.)");
		_arrow_separator = std::make_shared<space_token>(this, rest_begin, search_arrow_separator->suffix().begin());
		rest_begin = search_arrow_separator->suffix().begin();

		auto search_semicolon = regex_iterator(rest_begin, rest_end, const_regexes::primitives::semicolon);
		parse_error::assert_true(search_semicolon != regex_iterator(), R"(Could not find semicolon in transition.)");
		parse_error::assert_true(search_semicolon->suffix().begin() == rest_end, R"(Unexpected semicolon in formula definition.)");
		_semicolon = std::make_shared<semicolon_token>(this, search_semicolon->prefix().end(), search_semicolon->suffix().begin());
		rest_end = search_semicolon->prefix().end();

		while (rest_begin != rest_end) {

			auto search_colon = regex_iterator(rest_begin, rest_end, const_regexes::primitives::colon);
			parse_error::assert_true(search_colon != regex_iterator(), R"(Could not find a ":" somewhere after "->" in transition.)");
			auto _colon_token{ std::make_shared<colon_token>(this, search_colon->prefix().end(), search_colon->suffix().begin()) };

			auto search_probability = regex_iterator(rest_begin, search_colon->prefix().end(), const_regexes::primitives::not_spaces);
			parse_error::assert_true(search_probability != regex_iterator(), R"(Could not find a space-free part somewhere after "->" in transition.)");
			parse_error::assert_true(search_probability->prefix().end() == rest_begin, R"(Could not find a space-free part immediately after "->" in transition.)");

			auto _probability{ std::make_shared<float_token>(this, search_probability->prefix().end(), search_probability->suffix().begin()) };
			auto _probability_separator{ std::make_shared<space_token>(this, search_probability->suffix().begin(), search_probability->suffix().end()) };
			rest_begin = search_colon->suffix().begin();

			auto search_plus = regex_iterator(rest_begin, rest_end, const_regexes::primitives::plus);
			const bool found_last_condition{ search_plus == regex_iterator() };
			auto _condition_end{ !found_last_condition ? search_plus->prefix().end() : rest_end };
			auto _condition{ std::make_shared<condition_token>(this, rest_begin, _condition_end) };
			rest_begin = search_plus->suffix().begin();

			if (!found_last_condition) {
				auto _plus{ std::make_shared<plus_token>(this, search_plus->prefix().end(), search_plus->suffix().begin()) };

				auto search_plus_separator = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
				parse_error::assert_true(search_plus_separator != regex_iterator(), R"(Could not find space separators after "+" in transition.)");
				parse_error::assert_true(search_plus_separator->prefix().end() == rest_begin, R"(Could not find space separators immediately after "+" in transition.)");
				auto _plus_separator = std::make_shared<space_token>(this, rest_begin, search_plus_separator->suffix().begin());
				rest_begin = search_plus_separator->suffix().begin();

				_post_conditions.push_back({ _probability, _probability_separator, _colon_token, _condition, std::make_tuple(_plus, _plus_separator) });
			}
			else {
				_post_conditions.push_back({ _probability, _probability_separator, _colon_token, _condition, std::tuple_element<4, typename decltype(_post_conditions)::value_type>::type() });
			}
		}



		children.push_back(_start_label);
		children.push_back(_start_label_separator);
		children.push_back(_label);
		children.push_back(_label_separator);
		children.push_back(_end_label);
		children.push_back(_right_brace_separator);
		children.push_back(_pre_condition);
		children.push_back(_arrow);
		children.push_back(_arrow_separator);

		for (const auto& post_condition_tuple : _post_conditions) {
			children.push_back(std::get<0>(post_condition_tuple));
			children.push_back(std::get<1>(post_condition_tuple));
			children.push_back(std::get<2>(post_condition_tuple));
			children.push_back(std::get<3>(post_condition_tuple));
			if (std::get<4>(post_condition_tuple).has_value()) {
				std::get<0>(*std::get<4>(post_condition_tuple));
				std::get<1>(*std::get<4>(post_condition_tuple));
			}
		}
		children.push_back(_semicolon);
	}

	virtual bool is_primitive() const { return false; }

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), const_regexes::clauses::transition);
	}
};

class module_definition_token : public token {
public:

	using token::token;

	std::shared_ptr<module_token> _module_token;
	std::shared_ptr<spaces_plus_token> _module_separator;
	std::shared_ptr<identifier_token> _module_identifier;
	std::shared_ptr<spaces_plus_token> _identifier_separator;
	std::vector<
		std::pair<
		std::shared_ptr<transition_token>,
		std::shared_ptr<space_token>
		>
	> _transitions;
	std::shared_ptr<endmodule_token> _endmodule_token;

	virtual void parse_non_primitive() override {

		iterator rest_begin{ cbegin() };
		iterator rest_end{ cend() };

		auto search_keyword = regex_iterator(rest_begin, rest_end, const_regexes::primitives::module_keyword);
		parse_error::assert_true(search_keyword != regex_iterator(), R"(Could not find keyword "module" in module definition.)");
		parse_error::assert_true(search_keyword->prefix().end() == rest_begin, R"(Module definition does not start with "module".)");
		_module_token = std::make_shared<module_token>(this, rest_begin, search_keyword->suffix().begin());
		rest_begin = search_keyword->suffix().begin();

		auto search_space_separator_after_keyword = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces_plus);
		parse_error::assert_true(search_space_separator_after_keyword != regex_iterator(), R"(Could not find space separator after keyword "module" in module definition.)");
		parse_error::assert_true(search_space_separator_after_keyword->prefix().end() == rest_begin, R"(Could not find space separator immediately after keyword "module" in module definition.)");
		_module_separator = std::make_shared<spaces_plus_token>(this, rest_begin, search_space_separator_after_keyword->suffix().begin());
		rest_begin = search_space_separator_after_keyword->suffix().begin();

		auto search_identifier = regex_iterator(rest_begin, rest_end, const_regexes::primitives::identifier);
		parse_error::assert_true(search_identifier != regex_iterator(), R"(Could not find an identifier after keyword "module" in module definition.)");
		parse_error::assert_true(search_identifier->prefix().end() == rest_begin, R"(Could not find an identifier immediately after keyword "module" in module definition.)");
		_module_identifier = std::make_shared<identifier_token>(this, rest_begin, search_identifier->suffix().begin());
		rest_begin = search_identifier->suffix().begin();

		auto search_space_separator_after_identifier = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces_plus);
		parse_error::assert_true(search_space_separator_after_identifier != regex_iterator(), R"(Could not find space separators after identifier in module definition.)");
		parse_error::assert_true(search_space_separator_after_identifier->prefix().end() == rest_begin, R"(Could not find space separator immediately after identifier in module definition.)");
		_identifier_separator = std::make_shared<spaces_plus_token>(this, rest_begin, search_space_separator_after_identifier->suffix().begin());
		rest_begin = search_space_separator_after_identifier->suffix().begin();

		auto search_endmodule = regex_iterator(rest_begin, rest_end, const_regexes::primitives::endmodule_keyword);
		parse_error::assert_true(search_endmodule != regex_iterator(), R"(Could not find endmodule in module definition.)");
		parse_error::assert_true(search_endmodule->suffix().begin() == rest_end, R"(Keyword "endmodule" not at the end of global definition.)");
		_endmodule_token = std::make_shared<semicolon_token>(this, search_endmodule->prefix().end(), search_endmodule->suffix().begin());
		rest_end = search_endmodule->prefix().end();

		while (rest_begin != rest_end) {

			auto search_transition = regex_iterator(rest_begin, rest_end, const_regexes::clauses::transition);
			parse_error::assert_true(search_transition != regex_iterator(), R"(Could not find a transition in module definition.)");
			parse_error::assert_true(search_transition->prefix().end() == rest_begin, R"(Could not find a transition immediately at the beginning of the remaining body of module definition.)");
			auto my_transition{ std::make_shared<transition_token>(this, rest_begin, search_transition->suffix().begin()) };
			rest_begin = search_transition->suffix().begin();

			auto search_space_separator_after_transition = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
			parse_error::assert_true(search_space_separator_after_transition != regex_iterator(), R"(Could not find space separators after identifier in module definition.)");
			parse_error::assert_true(search_space_separator_after_transition->prefix().end() == rest_begin, R"(Could not find space separator immediately after identifier in module definition.)");
			auto my_separator{ std::make_shared<space_token>(this, rest_begin, search_space_separator_after_transition->suffix().begin()) };
			rest_begin = search_space_separator_after_transition->suffix().begin();

			_transitions.push_back({ my_transition, my_separator });
		}



		children.push_back(_module_token);
		children.push_back(_module_separator);
		children.push_back(_module_identifier);
		children.push_back(_identifier_separator);
		for (const auto& transition_pair : _transitions) {
			children.push_back(transition_pair.first);
			children.push_back(transition_pair.second);
		}
		children.push_back(_endmodule_token);
	}

	virtual bool is_primitive() const { return false; }//##

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), const_regexes::clauses::module_definition);
	}
};

class reward_definition_token : public token {
public:

	using token::token;

	virtual void parse_non_primitive() override {
		throw 0;
	}

	virtual bool is_primitive() const { return false; }//##

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), const_regexes::clauses::reward_definition);
	}
};

class init_definition_token : public token {
public:

	using token::token;

	virtual void parse_non_primitive() override { throw 0; }

	virtual bool is_primitive() const { return false; }//##

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), const_regexes::clauses::init_definition);
	}
};

class dtmc_body : public token {

public:
	using token::token;

	virtual bool is_primitive() const { return false; }


private:
	virtual void parse_non_primitive() override {
		token_list& result{ children };

		iterator rest_begin{ cbegin() };
		iterator rest_end{ cend() };

		using clause_searcher = std::pair<boost::regex, regex_iterator>;

		while (rest_begin != rest_end) {
			// check the casse of no match in the whole string, after loop step: recreate iterators in vector.
			std::vector<clause_searcher> searchers;
			auto append_searcher{ [&](const boost::regex& r) {
				searchers.emplace_back(r,regex_iterator(rest_begin,rest_end,r));
			} };
			append_searcher(const_regexes::primitives::spaces_plus);
			append_searcher(const_regexes::clauses::formula_definition);
			append_searcher(const_regexes::clauses::const_definition);
			append_searcher(const_regexes::clauses::global_definition);
			append_searcher(const_regexes::clauses::module_definition);
			append_searcher(const_regexes::clauses::reward_definition);
			append_searcher(const_regexes::clauses::init_definition);

			const auto COMP_SEARCHERS{ [&](const clause_searcher& l, const clause_searcher& r) {
				if (l.second == regex_iterator()) return false; // left did not find anything
				if (r.second == regex_iterator()) return true; // right did not find anything
				return l.second->prefix().end() < r.second->prefix().end();
			} };
			std::sort(searchers.begin(), searchers.end(), COMP_SEARCHERS);
			auto& current_match_result{ *searchers.front().second };
			//check for match at begin of remaining body:
			parse_error::assert_true(searchers.front().second->prefix().end() == rest_begin && (rest_begin == current_match_result.prefix().end()), "The first matching clause does not start with the beginning of the remaining body.");
			//### enhancement of error message: info where in the input did an error rise?
			//check for ambiguities:
			parse_error::assert_true(searchers.cbegin()->first != std::next(searchers.cbegin())->first, "Ambiguitiy found when parsing body.");
			// there is an unique clause found.
			if (searchers.front().first == const_regexes::primitives::spaces_plus) {
				result.push_back(std::make_unique<space_token>(this, match_begin(current_match_result), match_end(current_match_result)));
			}
			if (searchers.front().first == const_regexes::clauses::formula_definition) {
				children.push_back(std::make_shared<formula_definition_token>(this, match_begin(current_match_result), match_end(current_match_result)));
			}
			if (searchers.front().first == const_regexes::clauses::const_definition) {
				result.push_back(std::make_unique<const_definition_token>(this, match_begin(current_match_result), match_end(current_match_result)));
			}
			if (searchers.front().first == const_regexes::clauses::global_definition) {
				result.push_back(std::make_unique<global_definition_token>(this, match_begin(current_match_result), match_end(current_match_result)));
			}
			if (searchers.front().first == const_regexes::clauses::module_definition) {
				result.push_back(std::make_unique<module_definition_token>(this, match_begin(current_match_result), match_end(current_match_result)));
			}
			if (searchers.front().first == const_regexes::clauses::reward_definition) {
				result.push_back(std::make_unique<reward_definition_token>(this, match_begin(current_match_result), match_end(current_match_result)));
			}
			if (searchers.front().first == const_regexes::clauses::init_definition) {
				result.push_back(std::make_unique<init_definition_token>(this, match_begin(current_match_result), match_end(current_match_result)));
			}
			rest_begin = match_end(current_match_result);
		}
	}

	virtual bool is_sound() const final override {
		return std::accumulate(children.cbegin(), children.cend(), true, [](bool acc, const auto& element) { return acc && element->is_sound(); }); //check lowercase?
	}

	template <class _TokenType>
	std::list<std::shared_ptr<_TokenType>> children_of_kind() {
		std::list<std::shared_ptr<_TokenType>> down_casted;
		std::transform(children.cbegin(),
			children.cend(),
			std::back_inserter(down_casted),
			[](const std::shared_ptr<token>& ptr) { return std::dynamic_pointer_cast<_TokenType>(ptr); }
		);
		std::remove_if(down_casted.begin(), down_casted.end(), [](auto ptr) { return ptr.operator bool(); });
		return down_casted;
	}

	std::list<std::shared_ptr<const_definition_token>> const_definitions() { return children_of_kind<const_definition_token>(); }

	std::list<std::shared_ptr<formula_definition_token>> formula_definitions() { return children_of_kind<formula_definition_token>(); }

	std::list<std::shared_ptr<global_definition_token>> global_definitions() { return children_of_kind<global_definition_token>(); }

	std::list<std::shared_ptr<init_definition_token>> init_definitions() { return children_of_kind<init_definition_token>(); }

	std::list<std::shared_ptr<module_definition_token>> module_definitions() { return children_of_kind<module_definition_token>(); }

	std::list<std::shared_ptr<reward_definition_token>> reward_definitions() { return children_of_kind<reward_definition_token>(); }

	std::list<std::shared_ptr<space_token>> space_tokens() { return children_of_kind<space_token>(); }

};

class dtmc_token : public token {

public:
	using token::token;

	virtual bool is_primitive() const override {
		return true;
	}

private:
	virtual void parse_non_primitive() override {}

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), const_regexes::primitives::dtmc); //check lowercase?
	}

};

class file_token : public token {

public:

	std::shared_ptr<space_token> _leading_spaces;
	std::shared_ptr<dtmc_token> _dtmc_declaration;
	std::shared_ptr<dtmc_body> _dtmc_body_component;

	using token::token;
	file_token(std::shared_ptr<const std::string> file_content) : token(file_content, file_content->cbegin(), file_content->cend()) {}

	virtual void parse_non_primitive() override {
		auto it_dtmc = regex_iterator(_begin, _end, const_regexes::clauses::dtmc);

		parse_error::assert_true(it_dtmc != regex_iterator(), R"(No "dtmc" found.)");
		parse_error::assert_true(boost::regex_match(it_dtmc->prefix().begin(), it_dtmc->prefix().end(), const_regexes::primitives::spaces), R"(Found "dtmc", but input sequence does not start with "dtmc".)");

		_leading_spaces = std::make_unique<space_token>(this, it_dtmc->prefix().begin(), it_dtmc->prefix().end());
		_dtmc_declaration = std::make_unique<dtmc_token>(*this, it_dtmc->prefix().end(), it_dtmc->suffix().begin());
		_dtmc_body_component = std::make_unique<dtmc_body>(*this, it_dtmc->suffix().begin(), it_dtmc->suffix().end());
		children.push_back(_leading_spaces); // leading spaces
		children.push_back(_dtmc_declaration); // "dtmc"
		children.push_back(_dtmc_body_component); // body
	}

	virtual bool is_primitive() const { return false; }

	virtual bool is_sound() const final override {
		return std::accumulate(children.cbegin(), children.cend(), true, [](bool acc, const auto& element) { return acc && element->is_sound(); }); //check lowercase?
	}
private:
	std::shared_ptr<space_token> leading_spaces() { return _leading_spaces; }

	std::shared_ptr<dtmc_token> dtmc_declaration() { return _dtmc_declaration; }

	std::shared_ptr<dtmc_body> dtmc_body_component() { return _dtmc_body_component; }

};

