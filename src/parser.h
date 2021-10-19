#pragma once

#include "const_regexes.h"
#include "internal_error.h"
#include "parse_error.h"


#include <algorithm>
#include <list>
#include <memory>
#include <numeric> // std::accumulate
#include <string>
#include <type_traits> // std::remove_pointer

#define def_standard_clone() std::shared_ptr<token> clone() const override { return std::make_shared<std::remove_const<std::remove_reference<decltype(*this)>::type>::type>(*this); }

template <class T>
std::shared_ptr<T> copy_shared_ptr(const std::shared_ptr<T>& ptr) {
	if (ptr)
		return std::make_shared<T>(*ptr.get());
	return std::shared_ptr<T>();
}

class token;

class file_token;

class token {
public:
	using string_const_iterator = std::string::const_iterator;
	using regex_iterator = boost::regex_iterator<std::string::const_iterator>;
	using token_list = std::list<std::shared_ptr<token>>; //###remove

	struct utils {
		template <class _MatchResults>
		static string_const_iterator match_begin(const _MatchResults& m) {
			return m.prefix().end();
		}

		template <class _MatchResults>
		static string_const_iterator match_end(const _MatchResults& m) {
			return m.suffix().begin();
		}
	};

protected:

	std::shared_ptr<const std::string> _file_content; //###remove
	string_const_iterator _begin; //###remove
	string_const_iterator _end; //###remove
	const token* _parent;  //###remove

public:

	virtual bool operator==(const token& another) const = 0; // reviewed

	virtual std::string to_string() const = 0; // reviewed

	virtual token_list children() const = 0;

	virtual std::shared_ptr<token> clone() const = 0;

	token(std::shared_ptr<const std::string> file_content, string_const_iterator begin, string_const_iterator end) : _file_content(file_content), _begin(begin), _end(end), _parent(this) {}
	token(const token& parent_token, string_const_iterator begin, string_const_iterator end) : _file_content(parent_token._file_content), _begin(begin), _end(end), _parent(&parent_token) {}
	token(const token* parent_token, string_const_iterator begin, string_const_iterator end) : _file_content(parent_token->_file_content), _begin(begin), _end(end), _parent(parent_token) {}

	token(const token& another) {
		_file_content = another._file_content;
		_begin = another._begin;
		_end = another._end;
		_parent = this;
	}

	virtual bool is_primitive() const = 0;
	virtual bool is_sound() const = 0;

	bool is_sound_recursive() const {
		auto got_children = children();
		auto result = is_sound() && std::accumulate(got_children.cbegin(), got_children.cend(), true, [](auto acc, auto& child) { return acc && child->is_sound_recursive(); });
		if (!result) {
			standard_logger().error("here");
		}
		return result;
	}

private:

	virtual void parse_non_primitive() = 0;

public:

	void parse() {
		if (!is_primitive()) {
			parse_non_primitive();
		}
		auto got_children = children();
		for (auto& child : got_children) {
			if (child) child->parse();
		}
	}

	string_const_iterator cbegin() const { return _begin; }

	string_const_iterator cend() const { return _end; }

	std::string str() const { return std::string(cbegin(), cend()); }

};



template<class ... _Tokens>
class compound_token : public token {
	using _tuple = tuple<_Tokens...>;
	_tuple sub_tokens;

	static compound_token<_Tokens...> parse_string(std::string::const_iterator begin, std::string::const_iterator end) {
		if constexpr (sizeof ...(_Tokens) == 0) {
			if (begin != end) {
				// throw cannot parse
			}
			return compound_token<_Tokens...>();
		}
		else {
			std::vector<std::pair<std::string::const_iterator, std::string::const_iterator>> candidates_for_first_sub_token =
				std::tuple_element<0, _tuple>::type::find_all_candidates(begin, end, START_AT_FRONT);
			// for each check if extension yields a correct parsed compound token, also check the first one if candidate is real existence
			// -> =1 should yield a correctly parsed token -> then return this one
		}

		// find separations by finding all candidates for sub tokens -> might be more possibilities
		// try parse every separation candidate, =1 should be successful, otherwise cannot parse error or ambiguous parse error
		// 
		// parse it completely, recursive
		// if any exception, rethrow it here. cannot_parse_error, ambiguous_parse_error
		return x_token();
	}

	static std::vector<std::pair<std::string::const_iterator, std::string::const_iterator>> find_all_candidates(std::string::const_iterator begin, std::string::const_iterator end) {
		// add options: anywhere, start_at_begin, end_at_end
		// 
		// 
		// use calls for sub tokens, calc all combinations....
		// list all subsection where an x_token might be parsed., might be empty.
	}

};

class primitive_string_token : public token {
public:
	using type = primitive_string_token;
private:
	type(const std::string& string) : _string(string) {}

public:


	static type parse_string(string_const_iterator begin, string_const_iterator end, const std::string& pattern) {

		// parse it completely, recursive
		// if any exception, rethrow it here. cannot_parse_error, ambiguous_parse_error
		string_const_iterator iter = pattern.cbegin();
		string_const_iterator jter = begin;

		while (jter != end && iter != pattern.cend()) {

			const auto n_more = [](string_const_iterator begin, string_const_iterator iter, string_const_iterator end, const std::iterator_traits<string_const_iterator>::difference_type& n) {
				if (std::distance(iter, end) <= n)
					return std::string(begin, end);
				return std::string(begin, std::next(iter, n)) + "...";
			};

			if (jter == end) {
				std::string details_message_1{ "Gotten input already ended but pattern still provides remaining characters:\n" };
				details_message_1 += "input:   " + std::string(begin, end) + "\n";
				details_message_1 += "         " + std::string(std::distance(begin, end), ' ') + "^\n";
				details_message_1 += "pattern: " + n_more(pattern.cbegin(), iter, pattern.cend(), 10) + "\n";
				details_message_1 += "         " + std::string(std::distance(begin, end), ' ') + "^\n";
				throw not_matching(details_message_1);
			}
			if (iter == pattern.cend()) {
				std::string details_message_2{ "Pattern already ended but gotten input still provides remaining characters:\n" };
				details_message_2 += "input:   " + n_more(begin, jter, end, 10) + "\n";
				details_message_2 += "         " + std::string(pattern.size(), ' ') + "^\n";
				details_message_2 += "pattern: " + pattern + "\n";
				details_message_2 += "         " + std::string(pattern.size(), ' ') + "^\n";

				throw not_matching(details_message_2);
			}

			if (*jter != *iter) {
				std::string details_message_3{ "Pattern and input do not match at certain position:\n" };
				details_message_3 += "input:   " + n_more(begin, jter, end, 10) + "\n";
				details_message_3 += "         " + std::string(std::distance(begin, jter), ' ') + "^\n";
				details_message_3 += "pattern: " + n_more(pattern.cbegin(), iter, pattern.cend(), 10) + "\n";
				details_message_3 += "         " + std::string(std::distance(begin, iter), ' ') + "^\n";

				throw not_matching(details_message_3);
			}
			++jter;
			++iter;
		}

		return type(pattern);
	}

	static std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> find_all_candidates(std::string::const_iterator begin, std::string::const_iterator end) {
		// list all subsection where an x_token might be parsed., might be empty.
	}

private:
	std::string _string;


public:
	virtual bool operator==(const token& another) const override {
		try {
			const type& down_casted = dynamic_cast<const type&>(another);
			return to_string() == another.to_string();
		}
		catch (const std::bad_cast& e) {
			return false;
		}
	}

	virtual std::string to_string() const override {
		return _string;
	}

};



inline std::shared_ptr<token> clone_shared_ptr(const std::shared_ptr<token>& ptr) {
	if (ptr)
		return ptr->clone();
	return std::shared_ptr<token>();
}


class primitive_regex_token : public token {

	virtual boost::regex primitive_regex() const = 0;

public:

	primitive_regex_token(const primitive_regex_token& another) :
		token(another)
	{}

	virtual token_list children() const override {
		return token_list();
	}

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

	std::shared_ptr<token> clone() const override {
		return std::make_shared<const_token>(*this);
	}

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::const_keyword;
	}
};

class global_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	std::shared_ptr<token> clone() const override {
		return std::make_shared<global_token>(*this);
	}

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::global_keyword;
	}
};

class module_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	std::shared_ptr<token> clone() const override {
		return std::make_shared<module_token>(*this);
	}

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::module_keyword;
	}
};

class rewards_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	std::shared_ptr<token> clone() const override {
		return std::make_shared<rewards_token>(*this);
	}

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::rewards_keyword;
	}
};

class endrewards_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	std::shared_ptr<token> clone() const override {
		return std::make_shared<endrewards_token>(*this);
	}

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::endrewards_keyword;
	}
};

class endmodule_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::endmodule_keyword;
	}
};
//### try to do this as a template? using pointer?

class type_specifier_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;
	def_standard_clone()
		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::type_specifier;
	}
};


class left_square_brace_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()
		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::left_square_brace;
	}
};

class left_brace_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()
		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::left_brace;
	}
};

class right_square_brace_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::right_square_brace;
	}
};

class right_brace_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::right_brace;
	}
};

class natural_number_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		long long get_ll() {
		return std::stoll(str());
	}

	void modify(std::string natural_number) {
		const auto sptr = std::make_shared<std::string>(natural_number);
		_file_content = sptr;
		_begin = sptr->begin();
		_end = sptr->end();
	}

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::natural_number;
	}
};

class two_dots_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::dot_dot;
	}
};

class space_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::spaces;
	}

};

class spaces_plus_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()
		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::spaces_plus;
	}

};

template <const boost::regex* _Regex>
class primitive_regex_token_template : public primitive_regex_token {
	using primitive_regex_token::primitive_regex_token;

	static_assert(_Regex != nullptr, "Cannot use primitive regex token for unspecified regex!");

	std::shared_ptr<token> clone() const override {
		return std::make_shared<primitive_regex_token_template<_Regex>>(*this);
	}

	virtual boost::regex primitive_regex() const final override {
		return *_Regex;
	}
};

using double_quote_token = primitive_regex_token_template<&const_regexes::primitives::double_quote>;
using init_token = primitive_regex_token_template<&const_regexes::primitives::init_keyword>;



class formula_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::formula;
	}

};

class identifier_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::identifier;
	}

	void modify_string(const std::string& new_name) {
		const auto sptr = std::make_shared<std::string>(new_name);
		_file_content = sptr;
		_begin = sptr->begin();
		_end = sptr->end();
	}

	std::shared_ptr<int> int_value(const std::map<std::string, int>& const_table) {
		auto iter = const_table.find(str());
		if (iter != const_table.cend()) {
			return std::make_shared<int>(iter->second);
		}
		else {
			return nullptr;
		}

	}
};

class equals_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	std::shared_ptr<token> clone() const override {
		return std::make_shared<equals_token>(*this);
	}

	virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::equals;
	}

};

class semicolon_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::semicolon;
	}

};

class colon_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::colon;
	}

};

class ascii_arrow_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::ascii_arrow;
	}

};

class plus_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::plus;
	}

};

class or_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::or_sign;
	}

};

class and_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::and_sign;
	}

};

class comparison_operator_token : public primitive_regex_token {
public:

	using primitive_regex_token::primitive_regex_token;

	def_standard_clone()

		virtual boost::regex primitive_regex() const final override {
		return const_regexes::primitives::comparison_operator;
	}

};

class float_token : public token {
public:

	using token::token;

	float_token(const float_token& another) : token(another) {}

	std::shared_ptr<token> clone() const override {
		return std::make_shared<float_token>(*this);
	}


	virtual void parse_non_primitive() final override {}

	virtual token_list children() const override {
		return token_list();
	}

	virtual bool is_primitive() const final override { return true; }

	virtual bool is_sound() const final override {
		try {
			double x = std::stod(std::string(cbegin(), cend()));
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

	std::shared_ptr<expression_token> _left_expression;
	std::shared_ptr<token> _root_operator;
	std::shared_ptr<expression_token> _right_expression;

	std::shared_ptr<spaces_plus_token> _ignored_spaces;
	std::shared_ptr<expression_token> _child_expression;
	std::shared_ptr<spaces_plus_token> _trailing_ignored_spaces;

	std::shared_ptr<identifier_token> _identifier;

	expression_token(const expression_token& another) :
		token(another),
		_left_expression(copy_shared_ptr(another._left_expression)),
		_root_operator(clone_shared_ptr(another._root_operator)),
		_right_expression(copy_shared_ptr(another._right_expression)),
		_ignored_spaces(copy_shared_ptr(another._ignored_spaces)),
		_child_expression(copy_shared_ptr(another._child_expression)),
		_trailing_ignored_spaces(copy_shared_ptr(another._trailing_ignored_spaces)),
		_identifier(copy_shared_ptr(another._identifier))
	{}

	std::shared_ptr<token> clone() const override {
		return std::make_shared<expression_token>(*this);
	}

	virtual void parse_non_primitive() final override {
		string_const_iterator rest_begin{ cbegin() };
		string_const_iterator rest_end{ cend() };

		auto search_equals_operator = regex_iterator(rest_begin, rest_end, const_regexes::primitives::equals);
		if (search_equals_operator != regex_iterator()) {
			_root_operator = std::make_shared<equals_token>(this, search_equals_operator->prefix().end(), search_equals_operator->suffix().begin());
			_left_expression = std::make_shared<expression_token>(this, search_equals_operator->prefix().begin(), search_equals_operator->prefix().end());
			_right_expression = std::make_shared<expression_token>(this, search_equals_operator->suffix().begin(), search_equals_operator->suffix().end());
			return;
		}

		auto search_leading_spaces = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces_plus);
		if (search_leading_spaces != regex_iterator() && search_leading_spaces->prefix().end() == rest_begin) {
			_ignored_spaces = std::make_shared<spaces_plus_token>(this, rest_begin, search_leading_spaces->suffix().begin());
			_child_expression = std::make_shared<expression_token>(this, search_leading_spaces->suffix().begin(), rest_end);
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
				_trailing_ignored_spaces = std::make_shared<spaces_plus_token>(this, search_trailing_spaces->prefix().end(), search_trailing_spaces->suffix().begin());
				return;
			}
		}
		_identifier = std::make_shared<identifier_token>(this, rest_begin, rest_end);
	}

	virtual token_list children() const override {
		std::vector<std::shared_ptr<token>> possible_tokens{
			_left_expression, _root_operator, _right_expression, _ignored_spaces, _child_expression, _trailing_ignored_spaces, _identifier
		};
		token_list result;
		std::copy_if(
			possible_tokens.cbegin(),
			possible_tokens.cend(),
			std::back_inserter(result),
			[](const std::shared_ptr<token>& ptr) {
				return ptr.operator bool();
			}
		);
		return result;
	}

	virtual bool is_primitive() const final override { return false; }

	virtual bool is_sound() const final override {
		return true;
	}

};

class number_token : public token {

public:

	using token::token;

	number_token(const number_token& another) : token(another) {}

	std::shared_ptr<token> clone() const override {
		return std::make_shared<number_token>(*this);
	}


	virtual void parse_non_primitive() final override {
	}

	virtual token_list children() const override {
		return token_list();
	}

	virtual bool is_primitive() const final override { return true; }

	virtual bool is_sound() const final override {
		try {
			long double x = std::stold(std::string(cbegin(), cend()));
		}
		catch (...) {
			return false;
		}
		return true;
	}


	int int_value() {
		return std::stoi(str());
	}
};

class identifier_or_number : public token {

public:


	std::shared_ptr<identifier_token> _identifier;
	std::shared_ptr<number_token> _number;

	using token::token;

	identifier_or_number(const identifier_or_number& another) :
		token(another),
		_identifier(copy_shared_ptr(another._identifier)),
		_number(copy_shared_ptr(another._number))
	{}

	std::shared_ptr<token> clone() const override {
		return std::make_shared<identifier_or_number>(*this);
	}

	virtual void parse_non_primitive() final override {
		parse_error::assert_true(cbegin() != cend(), "Empty identifier_or_number token");
		if (boost::regex_match(cbegin(), std::next(cbegin()), const_regexes::primitives::digit)) {
			_number = std::make_shared<number_token>(this, cbegin(), cend());
		}
		else {
			_identifier = std::make_shared<identifier_token>(this, cbegin(), cend());
		}
	}

	virtual token_list children() const override {
		std::vector<std::shared_ptr<token>> possible_tokens{
			_identifier, _number
		};
		token_list result;
		std::copy_if(
			possible_tokens.cbegin(),
			possible_tokens.cend(),
			std::back_inserter(result),
			[](const std::shared_ptr<token>& ptr) {
				return ptr.operator bool();
			}
		);
		return result;
	}

	virtual bool is_primitive() const final override { return false; }

	virtual bool is_sound() const final override {
		return true;
	}

	bool contains_variable(const std::string& var_name) const {
		if (_identifier) return _identifier->str() == var_name;
		return false;
	}

	/*!
		@brief Returns the integer value of number or const symbol. Returns nullptr if it is undefined.
	*/
	std::shared_ptr<int> get_value(const std::map<std::string, int>& const_table) {
		if (_number) {
			return std::make_shared<int>(_number->int_value());
		}
		return _identifier->int_value(const_table);
	}

	std::vector<std::string> all_variables(const std::map<std::string, int>& const_table) const {
		auto result = std::vector<std::string>();
		if (!_identifier) return result;
		if (const_table.find(_identifier->str()) == const_table.cend())
			result.push_back(_identifier->str());
		return result;
	}
};

class equation_token : public token {

public:


	std::shared_ptr<space_token> _leading_separator;
	std::shared_ptr<identifier_or_number> _left_expression;
	std::shared_ptr<space_token> _left_expression_separator;
	std::shared_ptr<comparison_operator_token> _root_operator;
	std::shared_ptr<space_token> _operator_separator;
	std::shared_ptr<identifier_or_number> _right_expression;
	std::shared_ptr<space_token> _trailing_separator;

	using token::token;

	equation_token(const equation_token& another) :
		token(another),
		_leading_separator(copy_shared_ptr(another._leading_separator)),
		_left_expression(copy_shared_ptr(another._left_expression)),
		_left_expression_separator(copy_shared_ptr(another._left_expression_separator)),
		_root_operator(copy_shared_ptr(another._root_operator)),
		_operator_separator(copy_shared_ptr(another._operator_separator)),
		_right_expression(copy_shared_ptr(another._right_expression)),
		_trailing_separator(copy_shared_ptr(another._trailing_separator))
	{}

	std::shared_ptr<token> clone() const override {
		return std::make_shared<equation_token>(*this);
	}

	virtual void parse_non_primitive() final override {
		string_const_iterator rest_begin{ cbegin() };
		string_const_iterator rest_end{ cend() };

		std::vector<std::string> comparison_operators{ "!=", "<=", ">=", "=", "<", ">" };

		for (const auto& comparator : comparison_operators) {
			auto search_comparator = regex_iterator(rest_begin, rest_end, boost::regex(comparator));
			if (search_comparator == regex_iterator()) continue;
			// found comparator here:
			_root_operator = std::make_shared<comparison_operator_token>(this, search_comparator->prefix().end(), search_comparator->suffix().begin());
			auto process_expression = [this](std::shared_ptr<space_token>& _leading_separator, std::shared_ptr<identifier_or_number>& _expression, std::shared_ptr<space_token> _trailing_separator, string_const_iterator rest_begin, string_const_iterator rest_end) {
				auto search_leading_spaces = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
				parse_error::assert_true(search_leading_spaces != regex_iterator(), R"(Could not find leading spaces in equation token.)");
				parse_error::assert_true(search_leading_spaces->prefix().end() == rest_begin, R"(Could not find leading spaces in equation token but elsewhere.)");
				_leading_separator = std::make_shared<space_token>(this, rest_begin, search_leading_spaces->suffix().begin());
				rest_begin = search_leading_spaces->suffix().begin();
				auto search_expression = regex_iterator(rest_begin, rest_end, const_regexes::primitives::not_spaces);
				parse_error::assert_true(search_expression != regex_iterator(), R"(Could not find expression in equation token.)");
				parse_error::assert_true(search_expression->prefix().end() == rest_begin, R"(Could not find expression in equation token at expected position.)");
				_expression = std::make_shared<identifier_or_number>(this, rest_begin, search_expression->suffix().begin());
				_trailing_separator = std::make_shared<space_token>(this, search_expression->suffix().begin(), rest_end);
			};
			process_expression(_leading_separator, _left_expression, _left_expression_separator, rest_begin, search_comparator->prefix().end());
			process_expression(_operator_separator, _right_expression, _trailing_separator, search_comparator->suffix().begin(), rest_end);
			return;
		}
		throw parse_error("Could not find any comparator in equation_token");
	}

	virtual token_list children() const override {
		std::vector<std::shared_ptr<token>> possible_tokens{
			_leading_separator, _left_expression, _left_expression_separator, _root_operator, _operator_separator, _right_expression, _trailing_separator
		};
		token_list result;
		std::copy_if(
			possible_tokens.cbegin(),
			possible_tokens.cend(),
			std::back_inserter(result),
			[](const std::shared_ptr<token>& ptr) {
				return ptr.operator bool();
			}
		);
		return result;
	}

	virtual bool is_primitive() const final override { return false; }

	virtual bool is_sound() const final override {
		return true;
	}

	inline std::shared_ptr<std::vector<int>> __get_values_helper(const std::shared_ptr<identifier_or_number>& expression, const std::string& var_name, const std::map<std::string, int>& const_table) const {
		auto int_ptr = expression->get_value(const_table);
		if (int_ptr) {
			auto result = std::make_shared<std::vector<int>>();
			result->push_back(*int_ptr);
			return result;
		}
		else return nullptr;
	}

public:
	bool contains_variable(const std::string& var_name) const {
		return _left_expression->contains_variable(var_name) || _right_expression->contains_variable(var_name);
	}

	/*!
		@brief Returns the integer values var_name can take such that the equation evaluates true. Returns nullptr if it is undefined.
		Error if var_name != CONST is contained.
	*/
	std::shared_ptr<std::vector<int>> get_values(const std::string& var_name, const std::map<std::string, int>& const_table) const {
		if (_left_expression->contains_variable(var_name)) {
			return __get_values_helper(_right_expression, var_name, const_table);
		}
		if (_right_expression->contains_variable(var_name)) {
			return __get_values_helper(_left_expression, var_name, const_table);
		}
		return nullptr;
	}

	std::vector<std::string> all_variables(const std::map<std::string, int>& const_table) const {
		std::vector<std::string> result = _left_expression->all_variables(const_table);
		std::vector<std::string> second = _right_expression->all_variables(const_table);
		result.insert(result.end(), second.begin(), second.end());
		return result;
	}
};

class condition_token : public token {

public:

	enum class type {
		OR, AND, SUB_CONDITION, EQUATION
	};

	type _type;

	std::vector<std::shared_ptr<token>> _leading_tokens;
	std::vector<
		std::pair<
		std::shared_ptr<condition_token>,
		std::shared_ptr<token> /* separator */
		>
	> _sub_conditions;
	std::vector<std::shared_ptr<token>> _trailing_tokens;
	std::shared_ptr<equation_token> _equation;

	using token::token;

	/** Needs manually initialisation */
	condition_token(type _the_type, std::shared_ptr<std::string> dummy) : token(dummy, dummy->begin(), dummy->end()), _type(_the_type)
	{}

	condition_token(const condition_token& another) :
		token(another),
		_type(another._type),
		_equation(copy_shared_ptr(another._equation))
	{
		for (const auto& ltoken : another._leading_tokens) {
			_leading_tokens.push_back(clone_shared_ptr(ltoken));
		}
		for (const auto& pair : another._sub_conditions) {
			_sub_conditions.push_back(std::make_pair(copy_shared_ptr(pair.first), clone_shared_ptr(pair.second)));
		}
		for (const auto& ttoken : another._trailing_tokens) {
			_trailing_tokens.push_back(clone_shared_ptr(ttoken));
		}
	}

	std::shared_ptr<token> clone() const override {
		return std::make_shared<condition_token>(*this);
	}

	virtual void parse_non_primitive() final override {
		string_const_iterator rest_begin{ cbegin() };
		string_const_iterator rest_end{ cend() };

		// search all braces
		std::vector<std::pair<string_const_iterator, int64_t>> open_braces;
		int64_t count{ 0 };
		if (rest_begin == rest_end) throw parse_error("Empty condition.");
		if (*rest_begin != '(') open_braces.push_back({ rest_begin,0 });
		for (string_const_iterator it = rest_begin; it != rest_end; ++it) {
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
			unsigned int count_here{ 0 };
			bool has_0_open_braces{ true };
			for (const auto& e : open_braces) {
				if (e.second > 0 && has_0_open_braces) {
					has_0_open_braces = false;
					++count_here;
				}
				if (e.second == 0) has_0_open_braces = true;
			}
			if (count_here > 1) return false;
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

			_leading_tokens.push_back(leading_spaces);
			_leading_tokens.push_back(left_brace);
			_sub_conditions.push_back(std::make_pair(sub, nullptr));
			_trailing_tokens.push_back(right_brace);
			_trailing_tokens.push_back(trailing_spaces);

			return;
		}

		auto search_separators_outside_braces = [&](const boost::regex& r_separator) {
			std::vector<std::pair<string_const_iterator, string_const_iterator>> separator_positions;
			auto tracer = open_braces.cbegin();
			for (auto search_separator = regex_iterator(rest_begin, rest_end, r_separator); search_separator != regex_iterator(); ++search_separator) {
				auto pos = search_separator->prefix().end();
				while (std::next(tracer)->first <= pos && /* just for memory safety, should be an implication of the left -> */ tracer != open_braces.cend()) ++tracer;
				// for checking if at zero brace depth
				if (tracer->second == 0) separator_positions.push_back({ pos, search_separator->suffix().begin() });
			}
			return separator_positions;
		};

		auto split_using_separators = [&](const auto& splitters, auto the_type_ptr) -> bool {
			if (!splitters.empty()) {
				for (const auto& split : splitters) {
					auto sub = std::make_shared<condition_token>(this, rest_begin, split.first);
					auto splitter = std::make_shared<typename std::remove_pointer<decltype(the_type_ptr)>::type>(this, split.first, split.second);
					rest_begin = split.second;
					_sub_conditions.push_back(std::make_pair(sub, splitter));
				}
				auto last_sub = std::make_shared<condition_token>(this, rest_begin, rest_end);
				_sub_conditions.push_back(std::make_pair(last_sub, nullptr));
				return true; // successfully splitted
			}
			return false; // no splitters
		};
		//search | not inside braces
		std::vector<std::pair<string_const_iterator, string_const_iterator>> or_positions = search_separators_outside_braces(const_regexes::primitives::or_sign);
		if (split_using_separators(or_positions, static_cast<or_token*>(nullptr))) {
			_type = type::OR;
			return;
		}
		//search & not inside braces
		std::vector<std::pair<string_const_iterator, string_const_iterator>> and_positions = search_separators_outside_braces(const_regexes::primitives::and_sign);
		if (split_using_separators(and_positions, (and_token*)nullptr)) {
			_type = type::AND;
			return;
		}

		_type = type::EQUATION;
		_equation = std::make_shared<equation_token>(this, rest_begin, rest_end);
	}

	virtual token_list children() const override {
		token_list result;
		for (const std::shared_ptr<token>& pre_token : _leading_tokens) {
			if (pre_token.operator bool()) result.push_back(pre_token);
		}
		for (const
			std::pair<
			std::shared_ptr<condition_token>,
			std::shared_ptr<token> /* separator */
			>& cond_pair : _sub_conditions)
		{
			if (cond_pair.first.operator bool()) result.push_back(cond_pair.first);
			if (cond_pair.second.operator bool()) result.push_back(cond_pair.second);
		}
		for (const std::shared_ptr<token>& post_token : _trailing_tokens) {
			if (post_token.operator bool()) result.push_back(post_token);
		}
		if (_equation.operator bool()) result.push_back(_equation);
		return result;
	}


	virtual bool is_primitive() const final override { return false; }

	virtual bool is_sound() const final override {
		return true;
	}

	bool contains_variable(const std::string& var_name) const {
		bool accumulator = std::accumulate(_sub_conditions.cbegin(), _sub_conditions.cend(),
			false, [&](const auto& acc, const auto& element) { return acc || element.first->contains_variable(var_name); });
		if (_equation) {
			accumulator |= _equation->contains_variable(var_name);
		}
		return accumulator;
	}

	/*!
		@brief Returns set of all values var_name can have when this condition is satisfied.
	*/
	std::shared_ptr<std::vector<int>> get_values(const std::string& var_name, const std::map<std::string, int>& const_table) const {
		if (_type == type::EQUATION) {
			return _equation->get_values(var_name, const_table);
		}
		if (_type == type::SUB_CONDITION) {
			return _sub_conditions[0].first->get_values(var_name, const_table);
		}
		// and - or
		std::vector<std::shared_ptr<std::vector<int>>> sub_values;
		std::transform(
			_sub_conditions.cbegin(),
			_sub_conditions.cend(),
			std::back_inserter(sub_values),
			[&](const auto& x) { return x.first->get_values(var_name, const_table); });
		if (_type == type::OR) {
			return std::accumulate(
				sub_values.cbegin(),
				sub_values.cend(),
				std::make_shared<std::vector<int>>(), // empty vector
				[](auto s_ptr, auto s_ptr2) {
					if (!s_ptr) return s_ptr;
					if (!s_ptr2) return s_ptr2; // nullptr = all values possible.
					s_ptr->insert(s_ptr->end(), s_ptr2->begin(), s_ptr2->end());
					return s_ptr;
				}
			);
		}
		if (_type == type::AND) {
			return std::accumulate(
				sub_values.cbegin(),
				sub_values.cend(),
				std::shared_ptr<std::vector<int>>(), // nullptr
				[](std::shared_ptr<std::vector<int>> s_ptr, std::shared_ptr<std::vector<int>> s_ptr2) {
					if (!s_ptr) return s_ptr2;
					if (!s_ptr2) return s_ptr;
					auto new_end = std::remove_if(
						s_ptr->begin(),
						s_ptr->end(),
						[&](int y) {
							return std::find(s_ptr2->cbegin(), s_ptr2->cend(), y) == s_ptr2->cend();
						}
					);
					s_ptr->erase(new_end, s_ptr->cend());
					return s_ptr;
				}
			);
		}
		throw std::bad_exception();
	}

	std::vector<std::string> all_variables(const std::map<std::string, int>& const_table) const {
		if (_type == type::OR || _type == type::AND || _type == type::SUB_CONDITION) {
			std::vector<std::string> result;
			for (auto& sub : _sub_conditions) {
				const auto sub_var = sub.first->all_variables(const_table);
				result.insert(result.end(), sub_var.begin(), sub_var.end());
			}
			return result;
		}
		// _type == type::EQUATION
		return _equation->all_variables(const_table);
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
	std::shared_ptr<expression_token> _expression; // use identifier_or_number_token #####
	std::shared_ptr<semicolon_token> _semicolon;

	using token::token;

	formula_definition_token(const formula_definition_token& another) :
		token(another),
		_formula_token(copy_shared_ptr(another._formula_token)),
		_formula_keyword_separator(copy_shared_ptr(another._formula_keyword_separator)),
		_formula_identifier(copy_shared_ptr(another._formula_identifier)),
		_identifier_separator(copy_shared_ptr(another._identifier_separator)),
		_equals_token(copy_shared_ptr(another._equals_token)),
		_equals_separator(copy_shared_ptr(another._equals_separator)),
		_expression(copy_shared_ptr(another._expression)),
		_semicolon(copy_shared_ptr(another._semicolon))
	{}

	std::shared_ptr<token> clone() const override {
		return std::make_shared<formula_definition_token>(*this);
	}

	virtual void parse_non_primitive() override {
		string_const_iterator rest_begin{ cbegin() };
		string_const_iterator rest_end{ cend() };

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
	}

	virtual token_list children() const override {
		std::vector<std::shared_ptr<token>> possible_tokens{
			_formula_token, _formula_keyword_separator, _formula_identifier, _identifier_separator, _equals_token, _equals_separator, _expression, _semicolon
		};
		token_list result;
		std::copy_if(
			possible_tokens.cbegin(),
			possible_tokens.cend(),
			std::back_inserter(result),
			[](const std::shared_ptr<token>& ptr) {
				return ptr.operator bool();
			}
		);
		return result;
	}

	virtual bool is_primitive() const override { return false; }

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), const_regexes::clauses::formula_definition);
	}

};

class const_definition_token : public token {
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
	std::shared_ptr<identifier_or_number> _expression;
	std::shared_ptr<semicolon_token> _semicolon;

	const_definition_token(const const_definition_token& another) :
		token(another),
		_const_token(copy_shared_ptr(another._const_token)),
		_const_separator(copy_shared_ptr(another._const_separator)),
		_type_specifier_token(copy_shared_ptr(another._type_specifier_token)),
		_type_separator(copy_shared_ptr(another._type_separator)),
		_constant_identifier(copy_shared_ptr(another._constant_identifier)),
		_identifier_separator(copy_shared_ptr(another._identifier_separator)),
		_equals_token(copy_shared_ptr(another._equals_token)),
		_equals_separator(copy_shared_ptr(another._equals_separator)),
		_expression(copy_shared_ptr(another._expression)),
		_semicolon(copy_shared_ptr(another._semicolon))
	{
	}

	std::shared_ptr<token> clone() const override {
		return std::make_shared<const_definition_token>(*this);
	}

	virtual void parse_non_primitive() override {
		string_const_iterator rest_begin{ cbegin() };
		string_const_iterator rest_end{ cend() };

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

		_expression = std::make_shared<identifier_or_number>(this, rest_begin, rest_end); // just assume rest to be an expression

	}

	virtual token_list children() const override {
		std::vector<std::shared_ptr<token>> possible_tokens{
		_const_token, _const_separator, _type_specifier_token, _type_separator, _constant_identifier, _identifier_separator, _equals_token, _equals_separator, _expression, _semicolon
		};
		token_list result;
		std::copy_if(
			possible_tokens.cbegin(),
			possible_tokens.cend(),
			std::back_inserter(result),
			[](const std::shared_ptr<token>& ptr) {
				return ptr.operator bool();
			}
		);
		return result;
	}


	virtual bool is_primitive() const override { return false; }

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), const_regexes::clauses::const_definition);
	}
};

class global_definition_token : public token {
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
	std::optional<std::tuple<
		std::shared_ptr<init_token>,
		std::shared_ptr<spaces_plus_token>,
		std::shared_ptr<natural_number_token>,
		std::shared_ptr<space_token>
		>> _init_clause;
	std::shared_ptr<semicolon_token> _semicolon;

	global_definition_token(const global_definition_token& another) :
		token(another),
		_global_token(copy_shared_ptr(another._global_token)),
		_global_separator(copy_shared_ptr(another._global_separator)),
		_global_identifier(copy_shared_ptr(another._global_identifier)),
		_identifier_separator(copy_shared_ptr(another._identifier_separator)),
		_colon_token(copy_shared_ptr(another._colon_token)),
		_colon_separator(copy_shared_ptr(another._colon_separator)),
		_left_brace(copy_shared_ptr(another._left_brace)),
		_left_brace_separator(copy_shared_ptr(another._left_brace_separator)),
		_lower_bound(copy_shared_ptr(another._lower_bound)),
		_lower_bound_separator(copy_shared_ptr(another._lower_bound_separator)),
		_dots(copy_shared_ptr(another._dots)),
		_dots_separator(copy_shared_ptr(another._dots_separator)),
		_upper_bound(copy_shared_ptr(another._upper_bound)),
		_upper_bound_separator(copy_shared_ptr(another._upper_bound_separator)),
		_right_brace(copy_shared_ptr(another._right_brace)),
		_right_brace_separator(copy_shared_ptr(another._right_brace_separator)),
		_semicolon(copy_shared_ptr(another._semicolon))
	{
		if (another._init_clause)
			_init_clause = std::make_optional(
				std::make_tuple(
					copy_shared_ptr(std::get<0>(another._init_clause.value())),
					copy_shared_ptr(std::get<1>(another._init_clause.value())),
					copy_shared_ptr(std::get<2>(another._init_clause.value())),
					copy_shared_ptr(std::get<3>(another._init_clause.value()))
				)
			);
	}

	std::shared_ptr<token> clone() const override {
		return std::make_shared<global_definition_token>(*this);
	}

	virtual void parse_non_primitive() override {
		/* global cf : [0 .. 142]; */
		/* global cf : [0 .. 142] init 2; */
		string_const_iterator rest_begin{ cbegin() };
		string_const_iterator rest_end{ cend() };

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
		parse_error::assert_true(search_lower_bound != regex_iterator(), R"(Could not find lower bound in global definition.)");
		parse_error::assert_true(search_lower_bound->prefix().end() == rest_begin, R"(lower bound at unexpected position.)");
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
		if (search_semicolon->prefix().end() != rest_begin) { // there is some init clause
			auto init_begin = rest_begin;
			auto init_end = search_semicolon->prefix().end();
			/*init spaces + natural_number + spaces* */

			auto search_init_token = regex_iterator(init_begin, init_end, const_regexes::primitives::init_keyword);
			parse_error::assert_true(search_init_token != regex_iterator(), R"(Could not find init keyword after ] in global definition.)");
			auto debugxx = search_init_token->prefix().end();
			parse_error::assert_true(search_init_token->prefix().end() == init_begin, R"(Could not find init keyword immediately after ] in global definition.)");
			auto _init_token = std::make_shared<init_token>(this, init_begin, search_init_token->suffix().begin());
			init_begin = search_init_token->suffix().begin();

			auto search_init_separator = regex_iterator(init_begin, init_end, const_regexes::primitives::spaces_plus);
			parse_error::assert_true(search_init_separator != regex_iterator(), R"(Could not find space separator after keyword "init" in global definition.)");
			parse_error::assert_true(search_init_separator->prefix().end() == init_begin, R"(Could not find space separator immediately after keyword "init" in global definition.)");
			auto _init_separator = std::make_shared<spaces_plus_token>(this, init_begin, search_init_separator->suffix().begin());
			init_begin = search_init_separator->suffix().begin();

			auto search_init_value = regex_iterator(init_begin, init_end, const_regexes::primitives::natural_number);
			parse_error::assert_true(search_init_value != regex_iterator(), R"(Could not find init value in init clause.)");
			parse_error::assert_true(search_init_value->prefix().end() == init_begin, R"(Init value not immediately after init keyword.)");
			auto _init_value = std::make_shared<natural_number_token>(this, init_begin, search_init_value->suffix().begin());
			init_begin = search_init_value->suffix().begin();

			auto search_value_separator = regex_iterator(init_begin, init_end, const_regexes::primitives::spaces);
			parse_error::assert_true(search_value_separator != regex_iterator(), R"(Could not find space separator after value in init clause.)");
			parse_error::assert_true(
				search_value_separator->prefix().end() == init_begin && search_value_separator->suffix().begin() == init_end,
				R"(Unexpected characters in init clause.)");
			auto _value_separator = std::make_shared<space_token>(this, init_begin, init_end);
			_init_clause = std::make_tuple(_init_token, _init_separator, _init_value, _value_separator);
		}
		_semicolon = std::make_shared<semicolon_token>(this, search_semicolon->prefix().end(), search_semicolon->suffix().begin());
		rest_end = search_semicolon->prefix().end();

	}

	virtual token_list children() const override {
		std::vector<std::shared_ptr<token>> possible_tokens{
		_global_token, _global_separator, _global_identifier, _identifier_separator, _colon_token, _colon_separator, _left_brace, _left_brace_separator, _lower_bound, _lower_bound_separator, _dots, _dots_separator, _upper_bound, _upper_bound_separator, _right_brace, _right_brace_separator
		};
		if (_init_clause) {
			possible_tokens.push_back(std::get<0>(_init_clause.value()));
			possible_tokens.push_back(std::get<1>(_init_clause.value()));
			possible_tokens.push_back(std::get<2>(_init_clause.value()));
			possible_tokens.push_back(std::get<3>(_init_clause.value()));
		}
		possible_tokens.push_back(_semicolon);

		token_list result;
		std::copy_if(
			possible_tokens.cbegin(),
			possible_tokens.cend(),
			std::back_inserter(result),
			[](const std::shared_ptr<token>& ptr) {
				return ptr.operator bool();
			}
		);
		return result;
	}

	virtual bool is_primitive() const override { return false; }

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), const_regexes::clauses::global_definition);
	}
};

class transition_token : public token {

public:
	std::shared_ptr<left_square_brace_token> _start_label;
	std::shared_ptr<space_token> _start_label_separator;
	std::shared_ptr<identifier_token> _label;
	std::shared_ptr<space_token> _label_separator;
	std::shared_ptr<right_square_brace_token> _end_label;
	std::shared_ptr<space_token> _right_brace_separator;
	std::shared_ptr<condition_token> _pre_condition;
	std::shared_ptr<ascii_arrow_token> _arrow;
	std::shared_ptr<space_token> _arrow_separator;
	using probability_clause = std::optional<
		std::tuple<
		std::shared_ptr<float_token>,
		std::shared_ptr<space_token>,
		std::shared_ptr<colon_token>
		>
	>;
	using plus_clause = std::optional<
		std::tuple<
		std::shared_ptr<plus_token>,
		std::shared_ptr<space_token>
		>
	>;
	std::vector<
		std::tuple<
		probability_clause,
		std::shared_ptr<condition_token>,
		plus_clause
		>
	> _regular_post_conditions;
	std::shared_ptr<semicolon_token> _semicolon;

	using token::token;

	transition_token(const transition_token& another) :
		token(another),
		_start_label(copy_shared_ptr(another._start_label)),
		_start_label_separator(copy_shared_ptr(another._start_label_separator)),
		_label(copy_shared_ptr(another._label)),
		_label_separator(copy_shared_ptr(another._label_separator)),
		_end_label(copy_shared_ptr(another._end_label)),
		_right_brace_separator(copy_shared_ptr(another._right_brace_separator)),
		_pre_condition(copy_shared_ptr(another._pre_condition)),
		_arrow(copy_shared_ptr(another._arrow)),
		_arrow_separator(copy_shared_ptr(another._arrow_separator)),
		_semicolon(copy_shared_ptr(another._semicolon))
	{
		for (const auto& tuple : another._regular_post_conditions) {
			probability_clause first;
			if (std::get<0>(tuple)) {
				first = std::make_optional(
					std::make_tuple(
						copy_shared_ptr(std::get<0>(std::get<0>(tuple).value())),
						copy_shared_ptr(std::get<1>(std::get<0>(tuple).value())),
						copy_shared_ptr(std::get<2>(std::get<0>(tuple).value()))
					)
				);
			}
			plus_clause third;
			if (std::get<2>(tuple)) {
				third = std::make_optional(
					std::make_tuple(
						copy_shared_ptr(std::get<0>(std::get<2>(tuple).value())),
						copy_shared_ptr(std::get<1>(std::get<2>(tuple).value()))
					)
				);
			}
			_regular_post_conditions.push_back(
				std::make_tuple(
					first,
					copy_shared_ptr(std::get<1>(tuple)),
					third
				)
			);
		}
	}

	virtual std::shared_ptr<token> clone() const override {
		return std::make_shared<transition_token>(*this);
	}

	virtual void parse_non_primitive() override {

		string_const_iterator rest_begin{ cbegin() };
		string_const_iterator rest_end{ cend() };

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
			if (search_colon == regex_iterator()) {
				_regular_post_conditions.push_back({
					std::optional<std::tuple<std::shared_ptr<float_token>,std::shared_ptr<space_token>,std::shared_ptr<colon_token>>>(),
					std::make_shared< condition_token>(this, rest_begin, rest_end),
					std::optional<std::tuple<std::shared_ptr<plus_token>,std::shared_ptr<space_token>>>()
					});
				break;
			}
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
			rest_begin = found_last_condition ? _condition_end : search_plus->suffix().begin();

			if (!found_last_condition) {
				auto _plus{ std::make_shared<plus_token>(this, search_plus->prefix().end(), search_plus->suffix().begin()) };

				auto search_plus_separator = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
				parse_error::assert_true(search_plus_separator != regex_iterator(), R"(Could not find space separators after "+" in transition.)");
				parse_error::assert_true(search_plus_separator->prefix().end() == rest_begin, R"(Could not find space separators immediately after "+" in transition.)");
				auto _plus_separator = std::make_shared<space_token>(this, rest_begin, search_plus_separator->suffix().begin());
				rest_begin = search_plus_separator->suffix().begin();

				_regular_post_conditions.push_back({
					std::make_tuple(_probability, _probability_separator, _colon_token),
					_condition,
					std::make_tuple(_plus, _plus_separator)
					});
			}
			else {
				_regular_post_conditions.push_back({
					std::make_tuple(_probability, _probability_separator, _colon_token),
					_condition,
					std::tuple_element<2, typename decltype(_regular_post_conditions)::value_type>::type()
					});
			}
		}
	}

	virtual token_list children() const override {
		std::vector<std::shared_ptr<token>> possible_tokens;

		possible_tokens.push_back(_start_label);
		possible_tokens.push_back(_start_label_separator);
		possible_tokens.push_back(_label);
		possible_tokens.push_back(_label_separator);
		possible_tokens.push_back(_end_label);
		possible_tokens.push_back(_right_brace_separator);
		possible_tokens.push_back(_pre_condition);
		possible_tokens.push_back(_arrow);
		possible_tokens.push_back(_arrow_separator);

		for (const auto& post_condition_tuple : _regular_post_conditions) {
			if (std::get<0>(post_condition_tuple).has_value()) {
				possible_tokens.push_back(std::get<0>(*std::get<0>(post_condition_tuple)));
				possible_tokens.push_back(std::get<1>(*std::get<0>(post_condition_tuple)));
				possible_tokens.push_back(std::get<2>(*std::get<0>(post_condition_tuple)));
			}
			possible_tokens.push_back(std::get<1>(post_condition_tuple));
			if (std::get<2>(post_condition_tuple).has_value()) {
				possible_tokens.push_back(std::get<0>(*std::get<2>(post_condition_tuple)));
				possible_tokens.push_back(std::get<1>(*std::get<2>(post_condition_tuple)));
			}
		}
		possible_tokens.push_back(_semicolon);
		token_list result;
		std::copy_if(
			possible_tokens.cbegin(),
			possible_tokens.cend(),
			std::back_inserter(result),
			[](const std::shared_ptr<token>& ptr) {
				return ptr.operator bool();
			}
		);
		return result;
	}

	virtual bool is_primitive() const override { return false; }

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

	module_definition_token(const module_definition_token& another) :
		token(another),
		_module_token(copy_shared_ptr(another._module_token)),
		_module_separator(copy_shared_ptr(another._module_separator)),
		_module_identifier(copy_shared_ptr(another._module_identifier)),
		_identifier_separator(copy_shared_ptr(another._identifier_separator)),
		_endmodule_token(copy_shared_ptr(another._endmodule_token))
	{
		for (const auto& pair : another._transitions) {
			_transitions.push_back(std::make_pair(
				copy_shared_ptr(pair.first),
				copy_shared_ptr(pair.second)
			));
		}
	}

	std::shared_ptr<token> clone() const override {
		return std::make_shared<module_definition_token>(*this);
	}

	virtual void parse_non_primitive() override {

		string_const_iterator rest_begin{ cbegin() };
		string_const_iterator rest_end{ cend() };

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
		_endmodule_token = std::make_shared<endmodule_token>(this, search_endmodule->prefix().end(), search_endmodule->suffix().begin());
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
	}

	virtual token_list children() const override {
		std::vector<std::shared_ptr<token>> possible_tokens{
			_module_token, _module_separator, _module_identifier, _identifier_separator
		};
		for (const auto& transition_pair : _transitions) {
			possible_tokens.push_back(transition_pair.first);
			possible_tokens.push_back(transition_pair.second);
		}
		possible_tokens.push_back(_endmodule_token);
		token_list result;
		std::copy_if(
			possible_tokens.cbegin(),
			possible_tokens.cend(),
			std::back_inserter(result),
			[](const std::shared_ptr<token>& ptr) {
				return ptr.operator bool();
			}
		);
		return result;
	}

	virtual bool is_primitive() const override { return false; }

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), const_regexes::clauses::module_definition);
	}
};

class reward_definition_token : public token {
public:

	std::shared_ptr<rewards_token> _rewards_token;
	std::shared_ptr<spaces_plus_token> _rewards_separator;
	std::shared_ptr<double_quote_token> _open_quote;
	std::shared_ptr<identifier_token> _reward_identifier;
	std::shared_ptr<double_quote_token> _close_quote;
	std::shared_ptr<space_token> _identifier_separator;
	std::vector<
		std::tuple<
		std::shared_ptr<condition_token>,
		std::shared_ptr<colon_token>,
		std::shared_ptr<space_token>,
		std::shared_ptr<float_token>,
		std::shared_ptr<semicolon_token>,
		std::shared_ptr<space_token>
		>
	> _reward_triggers;
	std::shared_ptr<endrewards_token> _endrewards_token;

	using token::token;

	reward_definition_token(const reward_definition_token& another) :
		token(another),
		_rewards_token(copy_shared_ptr(another._rewards_token)),
		_open_quote(copy_shared_ptr(another._open_quote)),
		_reward_identifier(copy_shared_ptr(another._reward_identifier)),
		_close_quote(copy_shared_ptr(another._close_quote)),
		_identifier_separator(copy_shared_ptr(another._identifier_separator)),
		_endrewards_token(copy_shared_ptr(another._endrewards_token))
	{
		for (const auto& tuple : another._reward_triggers) {
			_reward_triggers.push_back(std::make_tuple(
				copy_shared_ptr(std::get<0>(tuple)),
				copy_shared_ptr(std::get<1>(tuple)),
				copy_shared_ptr(std::get<2>(tuple)),
				copy_shared_ptr(std::get<3>(tuple)),
				copy_shared_ptr(std::get<4>(tuple)),
				copy_shared_ptr(std::get<5>(tuple))
			));
		}
	}

	std::shared_ptr<token> clone() const override {
		return std::make_shared<reward_definition_token>(*this);
	}

	virtual void parse_non_primitive() override {
		string_const_iterator rest_begin{ cbegin() };
		string_const_iterator rest_end{ cend() };

		auto search_keyword = regex_iterator(rest_begin, rest_end, const_regexes::primitives::rewards_keyword);
		parse_error::assert_true(search_keyword != regex_iterator(), R"(Could not find keyword "rewards" in reward definition.)");
		parse_error::assert_true(search_keyword->prefix().end() == rest_begin, R"(Reward definition does not start with "rewards".)");
		_rewards_token = std::make_shared<rewards_token>(this, rest_begin, search_keyword->suffix().begin());
		rest_begin = search_keyword->suffix().begin();

		auto search_space_separator_after_keyword = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces_plus);
		parse_error::assert_true(search_space_separator_after_keyword != regex_iterator(), R"(Could not find space separator after keyword "rewards" in reward definition.)");
		parse_error::assert_true(search_space_separator_after_keyword->prefix().end() == rest_begin, R"(Could not find space separator immediately after keyword "rewards" in reward definition.)");
		_rewards_separator = std::make_shared<spaces_plus_token>(this, rest_begin, search_space_separator_after_keyword->suffix().begin());
		rest_begin = search_space_separator_after_keyword->suffix().begin();

		auto search_quote = regex_iterator(rest_begin, rest_end, const_regexes::primitives::double_quote);
		parse_error::assert_true(search_quote != regex_iterator(), R"(Could not find an " in reward definition.)");
		parse_error::assert_true(search_quote->prefix().end() == rest_begin, R"(Could not find an " immediately after keyword "rewards" in reward definition.)");
		_open_quote = std::make_shared<double_quote_token>(this, rest_begin, search_quote->suffix().begin());
		rest_begin = search_quote->suffix().begin();

		auto search_quote_2 = regex_iterator(rest_begin, rest_end, const_regexes::primitives::double_quote);
		parse_error::assert_true(search_quote_2 != regex_iterator(), R"(Could not find a second " in reward definition.)");
		_reward_identifier = std::make_shared<identifier_token>(this, search_quote->suffix().begin(), search_quote_2->prefix().end());
		_close_quote = std::make_shared<double_quote_token>(this, search_quote_2->prefix().end(), search_quote_2->suffix().begin());
		rest_begin = search_quote_2->suffix().begin();

		auto search_space_separator_after_identifier = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
		parse_error::assert_true(search_space_separator_after_identifier != regex_iterator(), R"(Could not find space separators after identifier in reward definition.)");
		parse_error::assert_true(search_space_separator_after_identifier->prefix().end() == rest_begin, R"(Could not find space separator immediately after identifier in reward definition.)");
		_identifier_separator = std::make_shared<space_token>(this, rest_begin, search_space_separator_after_identifier->suffix().begin());
		rest_begin = search_space_separator_after_identifier->suffix().begin();

		auto search_end_keyword = regex_iterator(rest_begin, rest_end, const_regexes::primitives::endrewards_keyword);
		parse_error::assert_true(search_end_keyword != regex_iterator(), R"(Could not find "endrewards" in reward definition.)");
		parse_error::assert_true(search_end_keyword->suffix().begin() == rest_end, R"(Could not find "endrewards" at the end of reward definition.)");
		_endrewards_token = std::make_shared<endrewards_token>(this, search_end_keyword->prefix().end(), search_end_keyword->suffix().begin());
		rest_end = search_end_keyword->prefix().end();

		while (rest_begin != rest_end) {
			auto search_semicolon = regex_iterator(rest_begin, rest_end, const_regexes::primitives::semicolon);
			parse_error::assert_true(search_semicolon != regex_iterator(), R"(Could not find ";" in reward definition.)");
			auto _semicolon = std::make_shared<semicolon_token>(this, search_semicolon->prefix().end(), search_semicolon->suffix().begin());
			auto inner_begin = rest_begin;
			auto inner_end = search_semicolon->prefix().end();
			rest_begin = search_semicolon->suffix().begin();

			auto search_space_separator_after_semicolon = regex_iterator(rest_begin, rest_end, const_regexes::primitives::spaces);
			parse_error::assert_true(search_space_separator_after_semicolon != regex_iterator(), R"(Could not find space separator after ";" in reward definition.)");
			parse_error::assert_true(search_space_separator_after_semicolon->prefix().end() == rest_begin, R"(Could not find space separator immediately after ";" in reward definition.)");
			auto _semicolon_separator = std::make_shared<space_token>(this, rest_begin, search_space_separator_after_semicolon->suffix().begin());
			rest_begin = search_space_separator_after_semicolon->suffix().begin();

			auto search_colon = regex_iterator(inner_begin, inner_end, const_regexes::primitives::colon);
			parse_error::assert_true(search_colon != regex_iterator(), R"(Could not find ":" in reward definition.)");
			auto _colon = std::make_shared<colon_token>(this, search_colon->prefix().end(), search_colon->suffix().begin());
			auto _condition = std::make_shared<condition_token>(this, inner_begin, search_colon->prefix().end());
			inner_begin = search_colon->suffix().begin();

			auto search_colon_separator = regex_iterator(inner_begin, inner_end, const_regexes::primitives::spaces);
			parse_error::assert_true(search_colon_separator != regex_iterator(), R"(Could not find space separator after ":" in reward definition.)");
			parse_error::assert_true(search_colon_separator->prefix().end() == inner_begin, R"(Could not find space separator immediately after ":" in reward definition.)");
			auto _colon_separator = std::make_shared<space_token>(this, inner_begin, search_colon_separator->suffix().begin());
			inner_begin = search_colon_separator->suffix().begin();

			auto _accumulator = std::make_shared<float_token>(this, inner_begin, inner_end); //### need some expression token in case of 1.0 ; (additional space)

			_reward_triggers.push_back({ _condition, _colon, _colon_separator, _accumulator, _semicolon, _semicolon_separator });
		}


	}

	virtual token_list children() const override {
		std::vector<std::shared_ptr<token>> possible_tokens{
			_rewards_token, _rewards_separator, _open_quote, _reward_identifier, _close_quote, _identifier_separator
		};
		for (const auto& entry : _reward_triggers) {
			possible_tokens.push_back(std::get<0>(entry));
			possible_tokens.push_back(std::get<1>(entry));
			possible_tokens.push_back(std::get<2>(entry));
			possible_tokens.push_back(std::get<3>(entry));
			possible_tokens.push_back(std::get<4>(entry));
			possible_tokens.push_back(std::get<5>(entry));
		}
		possible_tokens.push_back(_endrewards_token);
		token_list result;
		std::copy_if(
			possible_tokens.cbegin(),
			possible_tokens.cend(),
			std::back_inserter(result),
			[](const std::shared_ptr<token>& ptr) {
				return ptr.operator bool();
			}
		);
		return result;
	}

	virtual bool is_primitive() const override { return false; }

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), const_regexes::clauses::reward_definition);
	}
};

using init_token = primitive_regex_token_template<&const_regexes::primitives::init_keyword>;
using endinit_token = primitive_regex_token_template<&const_regexes::primitives::endinit_keyword>;

class init_definition_token : public token {
public:

	std::shared_ptr<init_token> _init_keyword;
	std::shared_ptr<condition_token> _start_condition;
	std::shared_ptr<endinit_token> _endinit_keyword;


	using token::token;

	init_definition_token(const init_definition_token& another) :
		token(another),
		_init_keyword(copy_shared_ptr(another._init_keyword)),
		_start_condition(copy_shared_ptr(another._start_condition)),
		_endinit_keyword(copy_shared_ptr(another._endinit_keyword))
	{}

	std::shared_ptr<token> clone() const override {
		return std::make_shared<init_definition_token>(*this);
	}

	virtual void parse_non_primitive() override {
		string_const_iterator rest_begin{ cbegin() };
		string_const_iterator rest_end{ cend() };

		auto search_keyword = regex_iterator(rest_begin, rest_end, const_regexes::primitives::init_keyword);
		parse_error::assert_true(search_keyword != regex_iterator(), R"(Could not find keyword "init" in init definition.)");
		parse_error::assert_true(search_keyword->prefix().end() == rest_begin, R"(Init definition does not start with "init".)");
		_init_keyword = std::make_shared<init_token>(this, rest_begin, search_keyword->suffix().begin());
		rest_begin = search_keyword->suffix().begin();

		auto search_end_keyword = regex_iterator(rest_begin, rest_end, const_regexes::primitives::endinit_keyword);
		parse_error::assert_true(search_end_keyword != regex_iterator(), R"(Could not find "endinit" in init definition.)");
		parse_error::assert_true(search_end_keyword->suffix().begin() == rest_end, R"(Could not find "endinit" at the end of init definition.)");
		_endinit_keyword = std::make_shared<endinit_token>(this, search_end_keyword->prefix().end(), search_end_keyword->suffix().begin());
		rest_end = search_end_keyword->prefix().end();

		_start_condition = std::make_shared<condition_token>(this, rest_begin, rest_end);
	}

	virtual token_list children() const override {
		std::vector<std::shared_ptr<token>> possible_tokens{
			_init_keyword, _start_condition, _endinit_keyword
		};
		token_list result;
		std::copy_if(
			possible_tokens.cbegin(),
			possible_tokens.cend(),
			std::back_inserter(result),
			[](const std::shared_ptr<token>& ptr) {
				return ptr.operator bool();
			}
		);
		return result;
	}

	virtual bool is_primitive() const override { return false; }

	virtual bool is_sound() const final override {
		return boost::regex_match(cbegin(), cend(), const_regexes::clauses::init_definition);
	}
};

class dtmc_body : public token {

public:
	using token::token;

	virtual bool is_primitive() const override { return false; }

	token_list local_children;

	dtmc_body(const dtmc_body& another) : token(another) {
		std::transform(another.local_children.cbegin(), another.local_children.cend(), std::back_inserter(local_children),
			[&](const std::shared_ptr<token>& original) { return
			clone_shared_ptr(original);
			});
	}

	def_standard_clone()

private:
	virtual void parse_non_primitive() override {
		token_list& result{ local_children };

		string_const_iterator rest_begin{ cbegin() };
		string_const_iterator rest_end{ cend() };

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
				result.push_back(std::make_shared<formula_definition_token>(this, match_begin(current_match_result), match_end(current_match_result)));
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

	virtual token_list children() const override {
		token_list result;
		std::copy_if(
			local_children.cbegin(),
			local_children.cend(),
			std::back_inserter(result),
			[](const std::shared_ptr<token>& ptr) {
				return ptr.operator bool();
			}
		);
		return result;
	}

	virtual bool is_sound() const final override {
		return true;
		//return std::accumulate(children.cbegin(), children.cend(), true, [](bool acc, const auto& element) { return acc && element->is_sound(); }); //check lowercase?
	}

public:
	template <class _TokenType>
	std::list<std::shared_ptr<_TokenType>> children_of_kind() {
		std::list<std::shared_ptr<token>> down_castable;
		const auto got_children = children();
		std::copy_if(got_children.cbegin(),
			got_children.cend(),
			std::back_inserter(down_castable),
			[](const std::shared_ptr<token>& ptr) { return std::dynamic_pointer_cast<_TokenType>(ptr); }
		);
		std::list<std::shared_ptr<_TokenType>> down_casted;
		std::transform(down_castable.begin(), down_castable.end(), std::back_inserter(down_casted), [](auto ptr) { return std::dynamic_pointer_cast<_TokenType>(ptr); });
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

using dtmc_token = primitive_regex_token_template<&const_regexes::primitives::dtmc>;


class file_token : public token {

public:

	std::shared_ptr<space_token> _leading_spaces;
	std::shared_ptr<dtmc_token> _dtmc_declaration;
	std::shared_ptr<dtmc_body> _dtmc_body_component;


	using token::token;
	file_token(std::shared_ptr<const std::string> file_content) : token(file_content, file_content->cbegin(), file_content->cend()) {}

	std::shared_ptr<token> clone() const override {
		auto the_clone = std::make_shared<file_token>(*this);
		return the_clone;
	}


	file_token(const file_token& another) :
		token(another),
		_leading_spaces(copy_shared_ptr(another._leading_spaces)),
		_dtmc_declaration(copy_shared_ptr(another._dtmc_declaration)),
		_dtmc_body_component(copy_shared_ptr(another._dtmc_body_component))
	{}

	virtual void parse_non_primitive() override {
		auto it_dtmc = regex_iterator(_begin, _end, const_regexes::clauses::dtmc);

		parse_error::assert_true(it_dtmc != regex_iterator(), R"(No "dtmc" found.)");
		//parse_error::assert_true(boost::regex_match(it_dtmc->prefix().begin(), it_dtmc->prefix().end(), const_regexes::primitives::spaces), R"(Found "dtmc", but input sequence does not start with "dtmc".)");


		_leading_spaces = std::make_shared<space_token>(this, it_dtmc->prefix().end(), it_dtmc->prefix().end()); // I just ignore here what comes in front of dtmc ######
		_dtmc_declaration = std::make_shared<dtmc_token>(*this, it_dtmc->prefix().end(), it_dtmc->suffix().begin());
		_dtmc_body_component = std::make_shared<dtmc_body>(*this, it_dtmc->suffix().begin(), it_dtmc->suffix().end());
	}

	virtual token_list children() const override {
		std::vector<std::shared_ptr<token>> possible_tokens{
			_leading_spaces, _dtmc_declaration, _dtmc_body_component
		};
		token_list result;
		std::copy_if(
			possible_tokens.cbegin(),
			possible_tokens.cend(),
			std::back_inserter(result),
			[](const std::shared_ptr<token>& ptr) {
				return ptr.operator bool();
			}
		);
		return result;
	}

	virtual bool is_primitive() const override { return false; }

	virtual bool is_sound() const final override {
		return true;
	}
private:
	std::shared_ptr<space_token> leading_spaces() { return _leading_spaces; }

	std::shared_ptr<dtmc_token> dtmc_declaration() { return _dtmc_declaration; }

	std::shared_ptr<dtmc_body> dtmc_body_component() { return _dtmc_body_component; }

};

