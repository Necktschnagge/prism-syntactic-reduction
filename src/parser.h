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
#include <string_view>
#include <functional>

#define def_standard_clone() std::shared_ptr<token> clone() const override { return std::make_shared<std::remove_const<std::remove_reference<decltype(*this)>::type>::type>(*this); }

namespace regular_extensions {
	template <class ... _Tokens>
	class alternative;

}

class unique_consts {
public:
	std::set<std::string> keywords;

	const std::string* get_unique_keyword(const std::string& key) {
		auto [iter, inserted] = keywords.insert(key);
		return &(*iter);
	}
};

class token_annotation {
public:
	std::shared_ptr<std::string> _original_file;
	std::string::const_iterator _begin;
	std::string::const_iterator _end;

	token_annotation() {}

	token_annotation(
		std::shared_ptr<std::string> original_file,
		std::string::const_iterator begin,
		std::string::const_iterator end
	) : _original_file(original_file),
		_begin(begin),
		_end(end)
	{}

};

class token {
public:
	using string_const_iterator = std::string::const_iterator;
	using regex_iterator = boost::regex_iterator<std::string::const_iterator>;
	//using const_token_ptr_vector = std::vector<const token*>;
	using const_token_ptr_vector = std::vector<const token*>;
	using token_ptr_vector = std::vector<token*>;

protected:
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

	token_annotation annotations;

public:

	virtual bool operator==(const token& another) const = 0; // reviewed

	virtual std::string to_string() const = 0; // reviewed

	virtual std::string type_info() const = 0; // reviewed

	static std::string static_type_info() {
		return "TOKEN_BASE_CLASS";
	}

	virtual const_token_ptr_vector children() const = 0; // reviewed

	token() : annotations() {}

	token(
		std::shared_ptr<std::string> original_file,
		std::string::const_iterator begin,
		std::string::const_iterator end
	) : annotations(original_file, begin, end) {}

	inline void annotate(std::shared_ptr<std::string> original_file, std::string::const_iterator begin, std::string::const_iterator end) {
		annotations = token_annotation(original_file, begin, end);
	}

};

template <class _Token>
class error_token : public _Token {
public:
	using type = error_token<_Token>;
private:

	//std::string _string;

	//type(const std::string& string) : _string(string) {}

	std::string _exception_message;


public:

	template< class ... Args>
	error_token(const std::string& exception_message, Args&& ... parent_class_arguments) : _Token(std::forward<Args>(parent_class_arguments) ...), _exception_message(exception_message) {}

	virtual bool operator==(const token& another) const override {
		return false;
	}

	virtual std::string to_string() const override {
		return "";
	}

	virtual std::string type_info() const override {
		return static_type_info();
	}

	static std::string static_type_info() {
		return std::string("ERROR_TOKEN<") + _Token::static_type_info() + ">";
	}

	virtual token::const_token_ptr_vector children() const override {
		return token::const_token_ptr_vector();
	}

	inline std::string get_exception() { return _exception_message; }
};

template<class _Token>
bool check_match(const std::string& test_string) {

	try {
		_Token test = _Token::parse_string(test_string.cbegin(), test_string.cend(), std::shared_ptr<std::string>());
		return true;
	}
	catch (const parse_error& e) {
		return false;
	}

}

template<class _Token>
bool check_match(const std::string::const_iterator& test_string_begin, const std::string::const_iterator& test_string_end) {

	try {
		_Token test = _Token::parse_string(test_string_begin, test_string_end, std::shared_ptr<std::string>());
		return true;
	}
	catch (const parse_error&) {
		return false;
	}

}

class general_string_token : public token {
public:
	using type = general_string_token;

private:
	std::string _string;

protected:
	general_string_token(const std::string& string) : _string(string) {}

public:
	static type parse_string(string_const_iterator begin, string_const_iterator end, const std::string& pattern, std::shared_ptr<std::string> file_content) {

		// parse it completely, recursive
		// if any exception, rethrow it here. cannot_parse_error, ambiguous_parse_error
		string_const_iterator iter = pattern.cbegin();
		string_const_iterator jter = begin;

		while (jter != end || iter != pattern.cend()) {

			const auto n_more = [](string_const_iterator begin, string_const_iterator iter, string_const_iterator end, const std::iterator_traits<string_const_iterator>::difference_type& n = 10) {
				if (std::distance(iter, end) <= n)
					return std::string(begin, end);
				return std::string(begin, std::next(iter, n)) + "...";
			};

			if (jter == end) {
				std::string details_message_1;
				details_message_1 += "Error when parsing general_string_token:\n";
				details_message_1 += "Gotten input already ended but pattern still provides remaining characters:\n";
				details_message_1 += "input:   " + std::string(begin, end) + "\n";
				details_message_1 += "         " + std::string(std::distance(begin, end), ' ') + "^\n";
				details_message_1 += "pattern: " + n_more(pattern.cbegin(), iter, pattern.cend()) + "\n";
				details_message_1 += "         " + std::string(std::distance(begin, end), ' ') + "^\n";
				throw not_matching(details_message_1, file_content, jter);
			}
			if (iter == pattern.cend()) {
				std::string details_message_2;
				details_message_2 += "Error when parsing general_string_token:\n";
				details_message_2 += "Pattern already ended but gotten input still provides remaining characters:\n";
				details_message_2 += "input:   " + n_more(begin, jter, end) + "\n";
				details_message_2 += "         " + std::string(pattern.size(), ' ') + "^\n";
				details_message_2 += "pattern: " + pattern + "\n";
				details_message_2 += "         " + std::string(pattern.size(), ' ') + "^\n";

				throw not_matching(details_message_2, file_content, jter);
			}

			if (*jter != *iter) {
				std::string details_message_3;
				details_message_3 += "Error when parsing general_string_token:\n";
				details_message_3 += "Pattern and input do not match at certain position:\n";
				details_message_3 += "input:   " + n_more(begin, jter, end) + "\n";
				details_message_3 += "         " + std::string(std::distance(begin, jter), ' ') + "^\n";
				details_message_3 += "pattern: " + n_more(pattern.cbegin(), iter, pattern.cend()) + "\n";
				details_message_3 += "         " + std::string(std::distance(begin, iter), ' ') + "^\n";

				throw not_matching(details_message_3, file_content, jter);
			}
			++jter;
			++iter;
		}

		return type(pattern);
	}

	static std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> find_all_candidates(std::string::const_iterator begin, std::string::const_iterator end, const std::string& pattern) {
		// list all subsection where an x_token might be parsed., might be empty.

		const auto sv = std::string(begin, end); // does it really use existing range?

		if constexpr (false) { // C++ 20 not yet supported, use one day:
			//const auto sv = std::string_view(begin, end); // does it really use existing range?
		}

		std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> results;

		std::string::size_type start{ 0 };
		while (true) {
			const auto pos = sv.find(pattern, start);

			if (pos == sv.npos) break;

			results.push_back(
				std::make_pair(
					std::next(begin, pos),
					std::next(begin, pos + pattern.size())
				)
			);

			start = pos + 1;
		}

		return results;

	}

	/**
		@brief Returns [true, iter] if [begin,iter] matches pattern, otherwise [false, end] if input ended before pattern was read, [false, iter] in any other case where at position iter the match failed for the first time.
	*/
	static std::pair<bool, std::string::const_iterator> has_front_candidate(std::string::const_iterator begin, std::string::const_iterator end, const std::string& pattern) {
		std::string::const_iterator iter = begin;
		std::string::const_iterator jter = pattern.cbegin();
		while (jter != pattern.cend()) {
			if (iter == end) return std::make_pair(false, end); // input ended
			if (*iter != *jter) return std::make_pair(false, iter); // missmatch
			++iter;
			++jter;
		}
		return std::make_pair(true, iter);
	}

public:
	virtual bool operator==(const token& another) const override {
		try {
			const type& down_casted = dynamic_cast<const type&>(another);
			return _string == down_casted._string;
		}
		catch (const std::bad_cast&) {
			return false;
		}
	}

	virtual std::string to_string() const override {
		return _string;
	}

	virtual std::string type_info() const override {
		return std::string("GENERAL_STRING_TOKEN(\"") + _string + "\")";
	}

	static std::string static_type_info() {
		return "GENERAL_STRING_TOKEN()";
	}

	virtual const_token_ptr_vector children() const override {
		return const_token_ptr_vector();
	}

};

template <const std::string* _String_Ptr>
class string_token : public general_string_token {
	static_assert(_String_Ptr != nullptr, "Not allowed: string_token<nullptr>");

public:
	using type = string_token<_String_Ptr>;

	constexpr static const std::string* _string_ptr{ _String_Ptr };

private:

	string_token() : general_string_token(*_string_ptr) {}
	template <class ... _Tokens>
	friend class regular_extensions::alternative;
public:

	static type parse_string(string_const_iterator begin, string_const_iterator end, std::shared_ptr<std::string> file_content) {

		const std::string& pattern{ *_string_ptr };
		try {
			general_string_token test = general_string_token::parse_string(begin, end, pattern, file_content);
		}
		catch (const parse_error& e) {
			std::string error_message;
			error_message += "Error when parsing string_token<" + *_string_ptr + ">:\nCaused here:\n";
			error_message += e.what();
			throw not_matching(error_message, file_content, begin);
		}

		return type();
	}

	static std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> find_all_candidates(std::string::const_iterator begin, std::string::const_iterator end) {
		const std::string& pattern{ *_string_ptr };

		return general_string_token::find_all_candidates(begin, end, pattern);
	}

	/**
	@brief Returns [true, iter] if [begin,iter] matches pattern, otherwise [false, end] if input ended before pattern was read, [false, iter] in any other case where at position iter the match failed for the first time.
	*/
	static std::pair<bool, std::string::const_iterator> has_front_candidate(std::string::const_iterator begin, std::string::const_iterator end) {
		const std::string& pattern{ *_string_ptr };
		return general_string_token::has_front_candidate(begin, end, pattern);
	}

	virtual std::string type_info() const override {
		return type::static_type_info();
	}

	static std::string static_type_info() {
		return std::string("STRING_TOKEN<\"") + *_string_ptr + "\">";
	}
};

class general_regex_token : public token {
public:
	using type = general_regex_token;
private:

	std::string _content;
	std::string _regex;

protected:
	general_regex_token(const std::string& content, const std::string& regex) : _content(content), _regex(regex) {}

public:

	static type parse_string(string_const_iterator begin, string_const_iterator end, const std::string& regex, std::shared_ptr<std::string> file_content) {

		// parse it completely, recursive
		// if any exception, rethrow it here. cannot_parse_error, ambiguous_parse_error

		const bool matching = boost::regex_match(begin, end, boost::regex(regex));
		if (!matching) {
			std::string error_message;
			error_message += "Input does not match regex:\n";
			error_message += "regex: " + regex + "\n";
			error_message += "begin of input sequence:\n";
			error_message += not_matching::get_position_description(file_content, begin);
			error_message += not_matching::show_position(file_content, begin);
			error_message += " end  of input sequence:\n";
			error_message += not_matching::get_position_description(file_content, end);
			error_message += not_matching::show_position(file_content, end);
			throw not_matching(error_message, file_content, begin);
		}

		return type(std::string(begin, end), regex);
	}

	static std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> find_all_candidates(std::string::const_iterator begin, std::string::const_iterator end, const std::string& regex) {
		// list all subsection where an x_token might be parsed., might be empty.

		std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> results;

		for (auto it = regex_iterator(begin, end, boost::regex(regex)); it != regex_iterator(); ++it) {
			results.emplace_back(utils::match_begin(*it), utils::match_end(*it));
		}

		return results;
	}

	/**
	@brief Returns [true, iter] if [begin,iter] matches pattern, otherwise [false, iter] if found a match at later position, [false, end] if no match at all.
	*/
	static std::pair<bool, std::string::const_iterator> has_front_candidate(std::string::const_iterator begin, std::string::const_iterator end, const std::string& regex) {

		auto it = regex_iterator(begin, end, boost::regex(regex));

		if (it == regex_iterator()) return std::make_pair(false, end);

		if (utils::match_begin(*it) != begin) return std::make_pair(false, utils::match_begin(*it));

		return std::make_pair(true, utils::match_end(*it));

	}

public:
	virtual bool operator==(const token& another) const override {
		try {
			const type& down_casted = dynamic_cast<const type&>(another);
			return _regex == down_casted._regex && _content == down_casted._content;
		}
		catch (const std::bad_cast&) {
			return false;
		}
	}

	virtual std::string to_string() const override {
		return _content;
	}

	virtual std::string type_info() const override {
		return std::string("GENERAL_REGEX_TOKEN(\"") + _regex + "\")";
	}

	static std::string static_type_info() {
		return "GENERAL_REGEX_TOKEN()";
	}

	virtual const_token_ptr_vector children() const override {
		return const_token_ptr_vector();
	}

};

template <const std::string* _Regex_String_Ptr>
class regex_token : public general_regex_token {
	static_assert(_Regex_String_Ptr != nullptr, "Not allowed: regex_token<nullptr>");
public:
	using type = regex_token<_Regex_String_Ptr>;

	constexpr static const std::string* _regex_string_ptr{ _Regex_String_Ptr };

private:


public:
	regex_token(const std::string& content) : general_regex_token(content, *_regex_string_ptr) {}

	static type parse_string(string_const_iterator begin, string_const_iterator end, std::shared_ptr<std::string> file_content) {

		try {
			general_regex_token test = general_regex_token::parse_string(begin, end, *_regex_string_ptr, file_content);
		}
		catch (const parse_error& e) {
			std::string error_message;
			error_message += "Error when parsing regex_token<" + *_regex_string_ptr + ">:\nCaused here:\n";
			error_message += e.what();
			throw not_matching(error_message, file_content, begin);
		}

		return type(std::string(begin, end));
	}

	static std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> find_all_candidates(std::string::const_iterator begin, std::string::const_iterator end) {
		return general_regex_token::find_all_candidates(begin, end, *_regex_string_ptr);
	}

	/**
		@brief Returns [true, iter] if [begin,iter] matches pattern, otherwise [false, iter] if found a match at later position, [false, end] if no match at all.
	*/
	static std::pair<bool, std::string::const_iterator> has_front_candidate(std::string::const_iterator begin, std::string::const_iterator end) {

		return general_regex_token::has_front_candidate(begin, end, *_regex_string_ptr);

	}

	virtual std::string type_info() const override {
		return type::static_type_info();
	}

	static std::string static_type_info() {
		return std::string("REGEX_TOKEN<\"") + *_regex_string_ptr + "\">";
	}

};

namespace keyword_tokens {

	using const_token = string_token<&const_regexes::strings::keywords::CONST_>;
	using dtmc_token = string_token<&const_regexes::strings::keywords::DTMC>;
	using endinit_token = string_token<&const_regexes::strings::keywords::ENDINIT>;
	using endmodule_token = string_token<&const_regexes::strings::keywords::ENDMODULE>;
	using endrewards_token = string_token<&const_regexes::strings::keywords::ENDREWARDS>;
	using false_token = string_token<&const_regexes::strings::keywords::FALSE_>;
	using formula_token = string_token<&const_regexes::strings::keywords::FORMULA>;
	using global_token = string_token<&const_regexes::strings::keywords::GLOBAL>;
	using init_token = string_token<&const_regexes::strings::keywords::INIT>;
	using int_token = string_token<&const_regexes::strings::keywords::INT>;
	using module_token = string_token<&const_regexes::strings::keywords::MODULE>;
	using rewards_token = string_token<&const_regexes::strings::keywords::REWARDS>;
	using true_token = string_token<&const_regexes::strings::keywords::TRUE_>;

};

namespace delimiter_tokens {

	using ampersand_token = string_token<&const_regexes::strings::delimiter::ampersand>;
	using ascii_arrow_token = string_token<&const_regexes::strings::delimiter::ascii_arrow>;
	using colon_token = string_token<&const_regexes::strings::delimiter::colon>;
	using dot_dot_token = string_token<&const_regexes::strings::delimiter::dot_dot>;
	using double_quote_token = string_token<&const_regexes::strings::delimiter::double_quote>;
	using double_slash_token = string_token<&const_regexes::strings::delimiter::double_slash>;
	using equals_token = string_token<&const_regexes::strings::delimiter::equals_sign>;
	using greater_or_equal_token = string_token<&const_regexes::strings::delimiter::greater_or_equal>;
	using greater_token = string_token<&const_regexes::strings::delimiter::greater>;
	using left_parenthesis_token = string_token<&const_regexes::strings::delimiter::left_parenthesis>;
	using left_square_bracket_token = string_token<&const_regexes::strings::delimiter::left_square_bracket>;
	using less_or_equal_token = string_token<&const_regexes::strings::delimiter::less_or_equal>;
	using less_token = string_token<&const_regexes::strings::delimiter::less>;
	using not_equals_token = string_token<&const_regexes::strings::delimiter::not_equals>;
	using plus_token = string_token<&const_regexes::strings::delimiter::plus>;
	using right_parenthesis_token = string_token<&const_regexes::strings::delimiter::right_parenthesis>;
	using right_square_bracket_token = string_token<&const_regexes::strings::delimiter::right_square_bracket>;
	using semicolon_token = string_token<&const_regexes::strings::delimiter::semicolon>;
	using vertical_bar_token = string_token<&const_regexes::strings::delimiter::vertical_bar>;

};

namespace regular_tokens {

	using anything_but_newline_token = regex_token<&const_regexes::strings::regulars::anything_but_newline>; //## do it as regular_extensions::alternative<...>
	//using comparison_operator_token = regex_token<&const_regexes::strings::regulars::comparison_operator>; //## do it as regular_extensions::alternative<...>
	using float_number_token = regex_token<&const_regexes::strings::regulars::float_number>;
	using identifier_token = regex_token<&const_regexes::strings::regulars::identifier>;
	using line_feed_token = regex_token<&const_regexes::strings::regulars::line_feed>;
	using natural_number_token = regex_token<&const_regexes::strings::regulars::natural_number>;
	using single_space_token = regex_token<&const_regexes::strings::regulars::single_space>;
	using type_specifier_token = regex_token<&const_regexes::strings::regulars::type_specifier>;

};


namespace primitive_clauses {

};

namespace regular_extensions {

	template<class _Token>
	class kleene_star : public token {
	public:
		using type = kleene_star<_Token>;
	private:

		std::vector<_Token> _sub_tokens;

		kleene_star(std::vector<_Token>&& sub_tokens) : _sub_tokens(std::forward<std::vector<_Token>>(sub_tokens)) {}

	public:

		static type parse_string(string_const_iterator begin, string_const_iterator end, std::shared_ptr<std::string> file_content) {

			// parse it completely, recursive
			// if any exception, rethrow it here. cannot_parse_error, ambiguous_parse_error

			std::vector<_Token> sub_tokens;

			string_const_iterator iter = begin;
			while (iter != end) {
				std::pair<bool, std::string::const_iterator> pair = _Token::has_front_candidate(iter, end);
				const bool& has_front_match{ pair.first };
				const std::string::const_iterator& end_of_match_candidate{ pair.second };

				if (!has_front_match) {
					std::string message;
					message += "Input cannot be parsed as Kleene star component:\n";
					message += "Cannot get a next submatch from here:\n";
					message += not_matching::get_position_description(file_content, iter);
					message += not_matching::show_position(file_content, iter);
					message += "End of input sequence:\n";
					message += not_matching::get_position_description(file_content, end);
					message += not_matching::show_position(file_content, end);

					throw not_matching(message, file_content, begin);
				}

				try {
					sub_tokens.push_back(_Token::parse_string(iter, end_of_match_candidate, file_content));
				}
				catch (const parse_error& e) {
					std::string message;
					message += "Error when parsing a match candidate as submatch of Kleene star component:\n";
					message += "Cannot parse next submatch from here:\n";
					message += not_matching::get_position_description(file_content, iter);
					message += not_matching::show_position(file_content, iter);
					message += "End of match candidate:\n";
					message += not_matching::get_position_description(file_content, end_of_match_candidate);
					message += not_matching::show_position(file_content, end_of_match_candidate);
					message += "Caused here:\n";
					message += e.what();
					throw not_matching(message, file_content, begin);
				}
				iter = end_of_match_candidate;
			}

			return type(std::move(sub_tokens));

		}

		static std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> find_all_candidates(std::string::const_iterator begin, std::string::const_iterator end) {
			// list all subsection where an x_token might be parsed., might be empty. (maximal kleene expansions from every starting position)

			std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> results;

			string_const_iterator begin_match = begin;
		outer_again:
			string_const_iterator iter = begin_match;
			while (begin_match != end) {
				while (iter != end) {
					std::pair<bool, std::string::const_iterator> pair = _Token::has_front_candidate(iter, end);
					const bool& has_front_match{ pair.first };
					const std::string::const_iterator& end_of_match_candidate{ pair.second };

					if (!has_front_match) { // on front only the empty match, try starting from next position
						results.emplace_back(begin_match, iter);
						++begin_match;
						goto outer_again;
					}

					if (!check_match<_Token>(iter, end_of_match_candidate)) {
						results.emplace_back(begin_match, iter);
						++begin_match;
						goto outer_again;
					}
					iter = end_of_match_candidate;
				}
				results.emplace_back(begin_match, iter);
				++begin_match;
				goto outer_again;
			}
			results.emplace_back(begin_match, begin_match);
			return results;
		}


		/**
		@brief Returns [true, iter] where [begin, iter] matches pattern and is the longest possible match, might be [true, begin] if found no front match but the empty match.
		*/
		static std::pair<bool, std::string::const_iterator> has_front_candidate(std::string::const_iterator begin, std::string::const_iterator end) {

			if (begin == end) return std::make_pair(true, begin);

			string_const_iterator iter = begin;
			while (iter != end) {
				std::pair<bool, std::string::const_iterator> pair = _Token::has_front_candidate(iter, end);
				const bool& has_front_match{ pair.first };
				const std::string::const_iterator& end_of_match_candidate{ pair.second };

				if (!has_front_match) { // on front only the empty match, try starting from next position
					return std::make_pair(true, iter);
				}

				try {
					_Token test = _Token::parse_string(iter, end_of_match_candidate, std::shared_ptr<std::string>());
				}
				catch (const parse_error& e) {
					return std::make_pair(true, iter);
				}
				iter = end_of_match_candidate;
			}
			return std::make_pair(true, iter);


		}


	public:
		virtual bool operator==(const token& another) const override {
			try {
				const type& down_casted = dynamic_cast<const type&>(another);
				return _sub_tokens == down_casted._sub_tokens;
			}
			catch (const std::bad_cast&) {
				return false;
			}
		}

		virtual std::string to_string() const override {
			std::string result;
			for (const auto& sub : _sub_tokens)
				result += sub.to_string();
			return result;
		}

		virtual std::string type_info() const override {
			return type::static_type_info();
		}

		static std::string static_type_info() {
			return std::string("KLEENE_STAR<") + _Token::static_type_info() + ">";
		}

		virtual const_token_ptr_vector children() const override {
			auto result = const_token_ptr_vector();
			for (std::size_t i{ 0 }; i < _sub_tokens.size(); ++i) {
				result.push_back(&_sub_tokens[i]);
			}
			return result;
		}

		typename std::vector<_Token>::size_type size() const {
			return _sub_tokens.size();
		}

		typename std::vector<_Token>::reference operator[](typename std::vector<_Token>::size_type index) {
			return _sub_tokens[index];
		}
	};

	template<class _Token>
	class kleene_plus : public token {
	public:
		using type = kleene_plus<_Token>;
	private:

		std::vector<_Token> _sub_tokens;

		kleene_plus(std::vector<_Token>&& sub_tokens) : _sub_tokens(std::forward<std::vector<_Token>>(sub_tokens)) {}

	public:

		static type parse_string(string_const_iterator begin, string_const_iterator end, std::shared_ptr<std::string> file_content) {

			// parse it completely, recursive
			// if any exception, rethrow it here. cannot_parse_error, ambiguous_parse_error

			std::vector<_Token> sub_tokens;

			if (begin == end) {
				try {
					sub_tokens.push_back(_Token::parse_string(begin, end, file_content));
				}
				catch (const parse_error& e) {
					std::string message;
					message += "Empty input cannot be parsed as Kleene plus component where sub component does not match empty input:\n";
					message += "Caused here:\n";
					message += e.what();
					throw not_matching(message, file_content, begin);
				}
				return type(std::move(sub_tokens));
			}

			string_const_iterator iter = begin;
			while (iter != end) {
				std::pair<bool, std::string::const_iterator> pair = _Token::has_front_candidate(iter, end);
				const bool& has_front_match{ pair.first };
				const std::string::const_iterator& end_of_match_candidate{ pair.second };

				if (!has_front_match) {
					std::string message;
					message += "Input cannot be parsed as Kleene plus component:\n";
					message += "Cannot get a next submatch from here:\n";
					message += not_matching::get_position_description(file_content, iter);
					message += not_matching::show_position(file_content, iter);
					message += "End of input sequence:\n";
					message += not_matching::get_position_description(file_content, end);
					message += not_matching::show_position(file_content, end);

					throw not_matching(message, file_content, begin);
				}

				try {
					sub_tokens.push_back(_Token::parse_string(iter, end_of_match_candidate, file_content));
				}
				catch (const parse_error& e) {
					std::string message;
					message += "Error when parsing a match candidate as submatch of Kleene plus component:\n";
					message += "Cannot parse next submatch from here:\n";
					message += not_matching::get_position_description(file_content, iter);
					message += not_matching::show_position(file_content, iter);
					message += "End of match candidate:\n";
					message += not_matching::get_position_description(file_content, end_of_match_candidate);
					message += not_matching::show_position(file_content, end_of_match_candidate);
					message += "Caused here:\n";
					message += e.what();
					throw not_matching(message, file_content, begin);
				}
				iter = end_of_match_candidate;
			}

			return type(std::move(sub_tokens));

		}

		static std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> find_all_candidates(std::string::const_iterator begin, std::string::const_iterator end) {
			// list all subsection where an x_token might be parsed., might be empty. (maximal kleene expansions from every starting position)

			std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> results;

			string_const_iterator begin_match = begin;
		outer_again:
			string_const_iterator iter = begin_match;
			while (begin_match != end) {
				while (iter != end) {
					std::pair<bool, std::string::const_iterator> pair = _Token::has_front_candidate(iter, end);
					const bool& has_front_match{ pair.first };
					const std::string::const_iterator& end_of_match_candidate{ pair.second };

					if (!has_front_match) { // no further match from iter on
						if (begin_match != iter || check_match<_Token>(begin_match, iter)) { // if the matched string is not "" OR the sub token can be ""
							results.emplace_back(begin_match, iter);
						}
						++begin_match;
						goto outer_again;

					}

					try {
						_Token test = _Token::parse_string(iter, end_of_match_candidate, std::shared_ptr<std::string>());
					}
					catch (const parse_error& e) {
						if (begin_match != iter || check_match<_Token>(begin_match, iter)) { // if the matched string is not "" OR the sub token can be ""
							results.emplace_back(begin_match, iter);
						}
						++begin_match;
						goto outer_again;
					}
					iter = end_of_match_candidate;
				}
				if (begin_match != iter || check_match<_Token>(begin_match, iter)) { // if the matched string is not "" OR the sub token can be ""
					results.emplace_back(begin_match, iter);
				}
				++begin_match;
				goto outer_again;
			}
			if (check_match<_Token>(begin_match, begin_match)) { // if the sub token can be ""
				results.emplace_back(begin_match, begin_match);
			}
			return results;
		}


		/**
		@brief Returns [true, iter] where [begin, iter] matches pattern and is the longest possible match, might be [true, begin] if found no front match but the empty match, but returns [false, X] if no such match is found.
		*/
		static std::pair<bool, std::string::const_iterator> has_front_candidate(std::string::const_iterator begin, std::string::const_iterator end) {

			if (begin == end) {
				if (check_match<_Token>(begin, end)) { // if the sub token can be ""
					return std::make_pair(true, begin);
				}
				else {
					return std::make_pair(false, begin);
				}
			}

			string_const_iterator iter = begin;
			while (iter != end) {
				std::pair<bool, std::string::const_iterator> pair = _Token::has_front_candidate(iter, end);
				const bool& has_front_match{ pair.first };
				const std::string::const_iterator& end_of_match_candidate{ pair.second };

				if (!has_front_match) { // no match on front (starting from iter position)
					if (begin != iter || check_match<_Token>(begin, iter)) { // if the matched string is not "" OR the sub token can be ""
						return std::make_pair(true, iter);
					}
					else {
						return std::make_pair(false, iter); // here: iter == begin 
					}
				}

				try {
					_Token test = _Token::parse_string(iter, end_of_match_candidate, std::shared_ptr<std::string>());
				}
				catch (const parse_error& e) {
					if (begin != iter || check_match<_Token>(begin, iter)) { // if the matched string is not "" OR the sub token can be ""
						return std::make_pair(true, iter);
					}
					else {
						return std::make_pair(false, iter); // here: iter == begin 
					}
				}
				iter = end_of_match_candidate;
			}
			// here: iter != begin; ( -> skipping the if clause )
			return std::make_pair(true, iter);
		}


	public:
		virtual bool operator==(const token& another) const override {
			try {
				const type& down_casted = dynamic_cast<const type&>(another);
				return _sub_tokens == down_casted._sub_tokens;
			}
			catch (const std::bad_cast&) {
				return false;
			}
		}

		virtual std::string to_string() const override {
			std::string result;
			for (const auto& sub : _sub_tokens)
				result += sub.to_string();
			return result;
		}

		virtual std::string type_info() const override {
			return type::static_type_info();
		}

		static std::string static_type_info() {
			return std::string("KLEENE_PLUS<") + _Token::static_type_info() + ">";
		}

		virtual const_token_ptr_vector children() const override {
			auto result = const_token_ptr_vector();
			for (std::size_t i{ 0 }; i < _sub_tokens.size(); ++i) {
				result.push_back(&_sub_tokens[i]);
			}
			return result;
		}

		typename std::vector<_Token>::size_type size() const {
			return _sub_tokens.size();
		}

		typename std::vector<_Token>::reference operator[](typename std::vector<_Token>::size_type index) {
			return _sub_tokens[index];
		}
	};

	template<class ... _Tokens>
	class alternative : public token {
		static_assert(sizeof...(_Tokens) != 0, "Forbidden to use empty alternative.");
	public:
		using type = alternative<_Tokens...>;
	private:

		template<class _Token>
		struct sub_parse_struct {
			using value_type = _Token;

			bool parsed_successfully; // remove it. this flag should be contained in the optional!!! #####
			std::optional<_Token> _Token_if_successfully;
			std::string error_message_if_not_successfully;

			sub_parse_struct() : parsed_successfully(false) {};
			sub_parse_struct(sub_parse_struct&&) = default;
			sub_parse_struct(const sub_parse_struct&) = default;

			inline bool operator == (const sub_parse_struct<_Token>& another) const {
				return parsed_successfully == another.parsed_successfully &&
					(!_Token_if_successfully && !another._Token_if_successfully || _Token_if_successfully.has_value() && another._Token_if_successfully.has_value() && _Token_if_successfully.value() == another._Token_if_successfully.value()) &&
					error_message_if_not_successfully == another.error_message_if_not_successfully;
			}
		};

		std::tuple<sub_parse_struct<_Tokens>...> _sub_tokens;

		alternative(std::tuple<sub_parse_struct<_Tokens>...>&& sub_tokens) : _sub_tokens(std::move(sub_tokens)) {}

		template <class _Token>
		struct helper {
			template <class _Function >
			//static parse_wrapper(_Token(*parse_function)(string_const_iterator, string_const_iterator, std::shared_ptr<std::string>), string_const_iterator begin, string_const_iterator end, std::shared_ptr<std::string> file_content) {
			static sub_parse_struct<_Token> parse_wrapper(_Function parse_function, string_const_iterator begin, string_const_iterator end, std::shared_ptr<std::string> file_content) {
				sub_parse_struct<_Token> result;
				try {
					result._Token_if_successfully.emplace(std::move(parse_function(begin, end, file_content)));
					result.parsed_successfully = true;
				}
				catch (const parse_error& e) {
					result.parsed_successfully = false;
					result.error_message_if_not_successfully = e.what();
				}
				return result;
			}
		};

	public:

		alternative(const alternative&) = default;
		alternative(alternative&&) = default;

		static type parse_string(string_const_iterator begin, string_const_iterator end, std::shared_ptr<std::string> file_content) {

			// parse it completely, recursive
			// if any exception, rethrow it here. cannot_parse_error, ambiguous_parse_error

			std::tuple<sub_parse_struct<_Tokens>...> parsed{ helper<_Tokens>::parse_wrapper(_Tokens::parse_string, begin, end, file_content)... };

			std::size_t count_matches{ 0 };
			std::apply([&count_matches](auto&& ... sub_parse) {(((sub_parse.parsed_successfully) ? ++count_matches : count_matches), ...); }, parsed);

			if (count_matches == 0) {
				std::string message;
				message += "Input cannot be parsed as Alternative component:\n";
				message += "The alternatives tested for match are:\n";

				std::apply([&message](auto&& ... pairs) {
					((
						message +=
						"candidate type: " +
						std::remove_reference_t<decltype(pairs)>::value_type::static_type_info() +
						"\nError for possible alternative caused here:\n" +
						pairs.error_message_if_not_successfully
						), ...);
					}, parsed);
				throw not_matching(message, file_content, begin);
			}

			if (count_matches > 1) {
				std::string message;
				message += "Input cannot be parsed as Alternative component unambiguously:\n";
				message += "There are " + std::to_string(count_matches) + " alternatives matching:\n";

				std::apply([&](auto&& ... pairs) {
					((
						(pairs.parsed_successfully) ?
						message +=
						"candidate type: " +
						std::remove_reference_t<decltype(pairs)>::value_type::static_type_info()
						: message
						), ...);
					}, parsed);
				throw ambiguous_matches(message, file_content, begin);
			}
			return type(std::move(parsed));
		}

		static std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> find_all_candidates(std::string::const_iterator begin, std::string::const_iterator end) {
			// list all subsection where an x_token might be parsed., might be empty. (maximal kleene expansions from every starting position)

			std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> results;

			std::list<std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>>> splitted_results;

			((splitted_results.push_back(_Tokens::find_all_candidates(begin, end))), ...);

			while (!splitted_results.empty()) {
				results.insert(results.cend(), splitted_results.front().cbegin(), splitted_results.front().cend());
			}

			std::sort(
				results.cbegin(),
				results.cend(),
				[](const std::pair<token::string_const_iterator, std::string::const_iterator>& left, const std::pair<token::string_const_iterator, std::string::const_iterator>& right) {
					return left.first == right.first ? left.second > right.second : left.first < right.first;
				});

			return results;

		}


		/**
		@brief Returns [true, iter] where [begin, iter] matches pattern and is the longest possible match, might be [true, begin] if found no front match but the empty match, but returns [false, X] if no such match is found.
		*/

		static std::pair<bool, std::string::const_iterator> has_front_candidate(std::string::const_iterator begin, std::string::const_iterator end) {

			std::vector<std::pair<bool, std::string::const_iterator>> collect;

			//std::list<std::vector<std::pair<bool, std::string::const_iterator>>> splitted_results;

			((collect.push_back(_Tokens::has_front_candidate(begin, end))), ...);

			std::pair<bool, std::string::const_iterator> result{ false, begin };

			for (const auto& item : collect) {
				if (item.first) {
					result.first = item.first;
					if (item.second > result.second)
						result.second = item.second;
				}
			}

			return result;
		}


	public:
		virtual bool operator==(const token& another) const override {
			try {
				const type& down_casted = dynamic_cast<const type&>(another);
				return _sub_tokens == down_casted._sub_tokens;
			}
			catch (const std::bad_cast&) {
				return false;
			}
		}

		virtual std::string to_string() const override {
			std::string result;
			std::apply([&result](auto&& ... args) { (((args.parsed_successfully) ? result += args._Token_if_successfully.value().to_string() : result), ...); }, _sub_tokens);
			return result;
		}

		virtual std::string type_info() const override {
			return type::static_type_info();
		}

		static std::string static_type_info() {
			std::string result;
			result += std::string("ALTERNATIVE<");
			((result += _Tokens::static_type_info() + ", "), ...);
			result.erase(result.size() - 2);
			result += ">";
			return result;
		}

		virtual const_token_ptr_vector children() const override {
			auto result = const_token_ptr_vector();
			std::apply([&result](auto&& ... element) { ((element.parsed_successfully ? result.push_back(&element._Token_if_successfully.value()) : void()), ...); }, _sub_tokens);
			/*for (std::size_t i{ 0 }; i < _sub_tokens; ++i) { //### ergänzen
				result.push_back(&_sub_tokens[i]);
			}*/
			return result;
		}


	};

	template<class ... _Tokens>
	class compound : public token {
		static_assert(sizeof...(_Tokens) != 0, "Forbidden to use empty compound.");
	public:
		using type = compound<_Tokens...>;
		using tuple = std::tuple<_Tokens...>;
	private:

		std::tuple<_Tokens...> _sub_tokens;

		compound(std::tuple<_Tokens...>&& sub_tokens) : _sub_tokens(std::move(sub_tokens)) {} //##error copying

		using candidate_vector = std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>>;

		// Convert vector into a tuple
		template<class tuple_type>
		struct to_tuple_helper {
			template<std::size_t... I>
			static auto get_tuple_from_vector(const candidate_vector& all_component_splittings, std::index_sequence<I...>, std::shared_ptr<std::string> file_content)
			{
				//regular_tokens::single_space_token::parse_string
				return std::make_tuple(std::tuple_element<I, tuple_type>::type::parse_string(all_component_splittings[I].first, all_component_splittings[I].second, file_content) ...);
			}
		};

	public:
		compound(const compound& another) = default;
		compound(compound&& another) = default;

		static type parse_string(string_const_iterator begin, string_const_iterator end, std::shared_ptr<std::string> file_content) {

			// parse it completely, recursive
			// if any exception, rethrow it here. cannot_parse_error, ambiguous_parse_error


			std::vector<candidate_vector> sub_candidates{ _Tokens::find_all_candidates(begin, end)... }; // { {comp1_occ1, comp1_occ2, ... }, {comp2_occ1, comp2_occ2, ... }, ...}

			std::vector<candidate_vector> collect; //{  "variant1" { (comp1_begin, comp1_end), (comp2_begin, comp2_end), ... }, "variant2" { (comp1_begin, comp1_end), (comp2_begin, comp2_end), ... }, ... }


			for (auto& vec : sub_candidates)
				std::sort(
					vec.begin(),
					vec.end(),
					[](const std::pair<token::string_const_iterator, std::string::const_iterator>& left, const std::pair<token::string_const_iterator, std::string::const_iterator>& right) {
						return left.first == right.first ? left.second > right.second : left.first < right.first;
					});

			string_const_iterator string_rest_begin{ begin };
			std::vector<candidate_vector::const_iterator> current_in_ith_component(sub_candidates.size());
			std::size_t i{ 0 }; // current component
			current_in_ith_component[0] = sub_candidates[0].cbegin();

			while (!(current_in_ith_component[0] == sub_candidates[0].cend())) { // reached end of first component's occurrences
				if (i == sub_candidates.size()) { // if at end
					if (string_rest_begin == end) { // accept result if matched whole input
						candidate_vector new_candidate; // { (comp1_begin, comp1_end), (comp2_begin, comp2_end), ... }
						for (auto iter = 0; iter < sub_candidates.size(); ++iter) {
							new_candidate.push_back(*current_in_ith_component[iter]);
						}
						collect.push_back(std::move(new_candidate));
					}
				}
				else {
					// try expand, and if it is possible, continue loop from begin to not forget some match:
					if (current_in_ith_component[i] != sub_candidates[i].cend()) { // otherwise current component fully expanded.
						// expand further...
						if (current_in_ith_component[i]->first == string_rest_begin) { // go into next component
							string_rest_begin = current_in_ith_component[i]->second;
							++i;
							if (i != current_in_ith_component.size()) current_in_ith_component[i] = sub_candidates[i].cbegin();
						}
						else { // go to next item in current component
							++current_in_ith_component[i];
						}
						continue; // to do not go back before collect.push_back(...); i.e. do not skip a result
					}
				}

				// go back in current component to the left if fully expanded sub component
				while (i == sub_candidates.size() || current_in_ith_component[i] == sub_candidates[i].cend()) {
					--i;
					string_rest_begin = current_in_ith_component[i]->first;
					++current_in_ith_component[i];
				}
			}
			// all collected.

			std::vector<std::tuple<_Tokens...>> correctly_parsed_collected;

			for (const auto& c : collect) {
				try {
					correctly_parsed_collected.emplace_back(to_tuple_helper<tuple>::get_tuple_from_vector(c, std::make_index_sequence<sizeof ... (_Tokens)>{}, file_content));
				}
				catch (const parse_error&) {
					//ignore, do not kep th collected candidate.
				}
			}

			if (correctly_parsed_collected.size() == 0) {
				std::string message;
				message += "Input cannot be parsed as compound token:\n";
				message += "The compound type is: " + type::static_type_info() + "\n";
				message += "Input end to match is here:\n";
				message += not_matching::get_position_description(file_content, end);
				message += not_matching::show_position(file_content, end);
				throw not_matching(message, file_content, begin);
			}

			if (correctly_parsed_collected.size() > 1) {
				std::string message;
				message += "Input cannot be parsed as compound token unambiguously:\n";
				message += "The compound type is: " + type::static_type_info() + "\n";
				message += "There are " + std::to_string(collect.size()) + " compound matches:\n";
				message += "[omitting displaying all of them]\n";
				throw ambiguous_matches(message, file_content, begin);
			}

			return type(std::move(correctly_parsed_collected[0]));
		}

		static std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> find_all_candidates(std::string::const_iterator begin, std::string::const_iterator end) {
			// list all subsection where an x_token might be parsed., might be empty. (maximal kleene expansions from every starting position)

			std::vector<candidate_vector> sub_candidates{ _Tokens::find_all_candidates(begin, end)... }; // { {comp1_occ1, comp1_occ2, ... }, {comp2_occ1, comp2_occ2, ... }, ...}

			std::vector<candidate_vector> collect; //{  "variant1" { (comp1_begin, comp1_end), (comp2_begin, comp2_end), ... }, "variant2" { (comp1_begin, comp1_end), (comp2_begin, comp2_end), ... }, ... }


			for (auto& vec : sub_candidates)
				std::sort(
					vec.begin(),
					vec.end(),
					[](const std::pair<token::string_const_iterator, std::string::const_iterator>& left, const std::pair<token::string_const_iterator, std::string::const_iterator>& right) {
						return left.first == right.first ? left.second > right.second : left.first < right.first;
					});

			string_const_iterator string_rest_begin{ begin };
			std::vector<candidate_vector::const_iterator> current_in_ith_component(sub_candidates.size());
			std::size_t i{ 0 }; // current component
			current_in_ith_component[0] = sub_candidates[0].cbegin();

			while (!(current_in_ith_component[0] == sub_candidates[0].cend())) { // reached end of first component's occurrences
				if (i == sub_candidates.size()) { // if at end
					candidate_vector new_candidate; // { (comp1_begin, comp1_end), (comp2_begin, comp2_end), ... }
					for (auto iter = 0; iter < sub_candidates.size(); ++iter) {
						new_candidate.push_back(*current_in_ith_component[iter]);
					}
					collect.push_back(std::move(new_candidate));
				}
				else {
					// try expand, and if it is possible, continue loop from begin to not forget some match:
					if (current_in_ith_component[i] != sub_candidates[i].cend()) { // otherwise current component fully expanded.
						// expand further...
						if (current_in_ith_component[i]->first == string_rest_begin || i == 0) { // go into next component if (this component extends the previous one) || (this component is the very first component)
							string_rest_begin = current_in_ith_component[i]->second;
							++i;
							if (i != current_in_ith_component.size()) current_in_ith_component[i] = sub_candidates[i].cbegin();
						}
						else { // go to next item in current component
							++current_in_ith_component[i];
						}
						continue; // to do not go back before collect.push_back(...); i.e. do not skip a result
					}
				}

				// go back in current component to the left if fully expanded sub component
				while (i == sub_candidates.size() || current_in_ith_component[i] == sub_candidates[i].cend()) {
					--i;
					string_rest_begin = current_in_ith_component[i]->first;
					++current_in_ith_component[i];
				}
			}
			// all collected.

			std::vector<std::pair<string_const_iterator, string_const_iterator>> correctly_parsed_collected;

			for (const auto& c : collect) {
				try {
					auto test = to_tuple_helper<tuple>::get_tuple_from_vector(c, std::make_index_sequence<sizeof ... (_Tokens)>{}, std::shared_ptr<std::string>());
					correctly_parsed_collected.emplace_back(c.front().first, c.back().second);
				}
				catch (const parse_error&) {
					//ignore, do not kep th collected candidate.
				}
			}
			return correctly_parsed_collected;
		}


		/**
		@brief Returns [true, iter] where [begin, iter] matches pattern and is the longest possible match, might be [true, begin] if found no front match but the empty match, but returns [false, X] if no such match is found.
		*/

		static std::pair<bool, std::string::const_iterator> has_front_candidate(std::string::const_iterator begin, std::string::const_iterator end) {


			std::vector<candidate_vector> sub_candidates{ _Tokens::find_all_candidates(begin, end)... }; // { {comp1_occ1, comp1_occ2, ... }, {comp2_occ1, comp2_occ2, ... }, ...}

			std::vector<candidate_vector> collect; //{  "variant1" { (comp1_begin, comp1_end), (comp2_begin, comp2_end), ... }, "variant2" { (comp1_begin, comp1_end), (comp2_begin, comp2_end), ... }, ... }


			for (auto& vec : sub_candidates)
				std::sort(
					vec.begin(),
					vec.end(),
					[](const std::pair<token::string_const_iterator, std::string::const_iterator>& left, const std::pair<token::string_const_iterator, std::string::const_iterator>& right) {
						return left.first == right.first ? left.second > right.second : left.first < right.first;
					});

			string_const_iterator string_rest_begin{ begin };
			std::vector<candidate_vector::const_iterator> current_in_ith_component(sub_candidates.size());
			std::size_t i{ 0 }; // current component
			current_in_ith_component[0] = sub_candidates[0].cbegin();

			while (!(current_in_ith_component[0] == sub_candidates[0].cend())) { // reached end of first component's occurrences
				if (i == sub_candidates.size()) { // if at end
					candidate_vector new_candidate; // { (comp1_begin, comp1_end), (comp2_begin, comp2_end), ... }
					for (auto iter = 0; iter < sub_candidates.size(); ++iter) {
						new_candidate.push_back(*current_in_ith_component[iter]);
					}
					collect.push_back(std::move(new_candidate));
				}
				else {
					// try expand, and if it is possible, continue loop from begin to not forget some match:
					if (current_in_ith_component[i] != sub_candidates[i].cend()) { // otherwise current component fully expanded.
						// expand further...
						if (current_in_ith_component[i]->first == string_rest_begin) { // go into next component if (this component extends the previous one) || (this component is the very first component)
							string_rest_begin = current_in_ith_component[i]->second;
							++i;
							if (i != current_in_ith_component.size()) current_in_ith_component[i] = sub_candidates[i].cbegin();
						}
						else { // go to next item in current component
							++current_in_ith_component[i];
						}
						continue; // to do not go back before collect.push_back(...); i.e. do not skip a result
					}
				}

				// go back in current component to the left if fully expanded sub component
				while (i == sub_candidates.size() || current_in_ith_component[i] == sub_candidates[i].cend()) {
					--i;
					string_rest_begin = current_in_ith_component[i]->first;
					++current_in_ith_component[i];
				}
			}
			// all collected.

			std::vector<string_const_iterator> correctly_parsed_collected;

			for (const auto& c : collect) {
				try {
					auto test = to_tuple_helper<tuple>::get_tuple_from_vector(c, std::make_index_sequence<sizeof ... (_Tokens)>{}, std::shared_ptr<std::string>());
					correctly_parsed_collected.emplace_back(c.back().second);
				}
				catch (const parse_error&) {
					//ignore, do not kep th collected candidate.
				}
			}

			if (correctly_parsed_collected.empty())
				return std::make_pair(false, begin);
			else
				return std::make_pair(true, *std::max_element(correctly_parsed_collected.cbegin(), correctly_parsed_collected.cend()));
		}


	public:
		virtual bool operator==(const token& another) const override {
			try {
				const type& down_casted = dynamic_cast<const type&>(another);
				return _sub_tokens == down_casted._sub_tokens;
			}
			catch (const std::bad_cast&) {
				return false;
			}
		}

		virtual std::string to_string() const override {
			std::string result;
			std::apply([&result](auto&& ... args) { ((result += args.to_string()), ...); }, _sub_tokens);
			return result;
		}

		virtual std::string type_info() const override {
			return type::static_type_info();
		}

		static std::string static_type_info() {
			std::string result;
			result += std::string("COMPOUND<");
			((result += _Tokens::static_type_info() + ", "), ...);
			result.erase(result.size() - 2);
			result += ">";
			return result;
		}

		virtual const_token_ptr_vector children() const override {
			auto result = const_token_ptr_vector();
			std::apply([&result](auto&& ... element) { ((result.push_back(&element)), ...); }, _sub_tokens);
			return result;
		}


	};

	template<class _Token>
	class optional : public token {
	public:
		using type = optional<_Token>;
	private:

		std::optional<_Token> _sub_token;

		optional(std::optional<_Token>&& sub_token) : _sub_token(std::forward<std::optional<_Token>>(sub_token)) {}

	public:

		static type parse_string(string_const_iterator begin, string_const_iterator end, std::shared_ptr<std::string> file_content) {

			// parse it completely, recursive
			// if any exception, rethrow it here. cannot_parse_error, ambiguous_parse_error

			if (begin == end)
				return type(std::optional<_Token>());

			std::optional<_Token> sub_token;

			try {
				sub_token = std::make_optional<_Token>(_Token::parse(begin, end, file_content));
			}
			catch (const parse_error& e) {
				std::string message;
				message += "Nonempty input cannot be parsed as optional component:\n";
				message += e.what();
				throw not_matching(message, file_content, begin);
			}
			return type(sub_token);
		}

		static std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> find_all_candidates(std::string::const_iterator begin, std::string::const_iterator end) {
			// list all subsection where an x_token might be parsed., might be empty. (maximal kleene expansions from every starting position)

			std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> results;

			for (auto iter = begin; iter != end; ++iter)
				results.emplace_back(iter, iter);

			std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> rest{ _Token::find_all_candidates(begin, end) };

			results.insert(results.cend(), rest.cbegin(), rest.cend());
			return results;
		}


		/**
		@brief Returns [true, iter] where [begin, iter] matches pattern and is the longest possible match, might be [true, begin] if found no front match but the empty match.
		*/
		static std::pair<bool, std::string::const_iterator> has_front_candidate(std::string::const_iterator begin, std::string::const_iterator end) {

			if (begin == end) return std::make_pair(true, begin);

			std::pair<bool, std::string::const_iterator> pair = _Token::has_front_candidate(begin, end);

			if (pair.first) return pair;

			return std::make_pair(true, begin);
		}


	public:
		virtual bool operator==(const token& another) const override {
			try {
				const type& down_casted = dynamic_cast<const type&>(another);
				return _sub_token == down_casted._sub_token;
			}
			catch (const std::bad_cast&) {
				return false;
			}
		}

		virtual std::string to_string() const override {
			std::string result;
			if (_sub_token.has_value()) result += _sub_token.value().to_string();
			return result;
		}

		virtual std::string type_info() const override {
			return type::static_type_info();
		}

		static std::string static_type_info() {
			return std::string("OPTIONAL<") + _Token::static_type_info() + ">";
		}

		virtual const_token_ptr_vector children() const override {
			auto result = const_token_ptr_vector();
			if (_sub_token.has_value()) result.push_back(&_sub_token.value());
			return result;
		}
	};

	/*
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
	*/
};

struct simple_derived {

	using maybe_spaces_token = regular_extensions::kleene_star<regular_tokens::single_space_token>;
	using spaces_token = regular_extensions::kleene_plus<regular_tokens::single_space_token>;

	using comparison_operator_token = regular_extensions::alternative<
		delimiter_tokens::equals_token,
		delimiter_tokens::not_equals_token,
		delimiter_tokens::less_token,
		delimiter_tokens::less_or_equal_token,
		delimiter_tokens::greater_token,
		delimiter_tokens::greater_or_equal_token
	>;


	static token::string_const_iterator find_largest_single_space_range_at(token::string_const_iterator begin, token::string_const_iterator end, token::string_const_iterator search_from_here) {
		if (search_from_here == begin)
		{ // search forwards
			while (search_from_here != end) {
				const auto last_position{ search_from_here++ };
				if (!check_match<regular_tokens::single_space_token>(last_position, search_from_here))
					return last_position;
			}
			return end;
		}
		else
			if (search_from_here == end)
			{ // search backwards
				do {
					const auto last_position{ search_from_here-- };
					if (!check_match<regular_tokens::single_space_token>(search_from_here, last_position))
						return last_position;
				} while (search_from_here != begin);
				return begin;
			}
			else
			{
				throw std::invalid_argument("searching position must equal begin or end");
			}
	}

	using comment_line_ending_token = regular_extensions::compound<
		delimiter_tokens::double_slash_token,
		regular_extensions::kleene_star<regular_tokens::anything_but_newline_token>
	>;
	using natural_number_or_identifier_token = regular_extensions::alternative<
		regular_tokens::identifier_token,
		regular_tokens::natural_number_token
	>;

	using init_value_clause_token = regular_extensions::compound<
		keyword_tokens::init_token,
		simple_derived::spaces_token,
		natural_number_or_identifier_token,
		simple_derived::maybe_spaces_token
	>;
	//using any_ignorable_line_ending = regular_extensions::compound< regular_extensions::kleene_star<>, delimiter_tokens::double_slash_token, regular_extensions::kleene_star<regular_tokens::anything_but_newline_token>>;

};

struct higher_clauses {

	using relaxed_comment_section = regular_extensions::kleene_star<
		regular_extensions::alternative<
		regular_extensions::compound<simple_derived::comment_line_ending_token, regular_tokens::line_feed_token>,
		regular_tokens::single_space_token
		>
	>;

	using const_definition = regular_extensions::compound<
		keyword_tokens::const_token,
		simple_derived::spaces_token,
		keyword_tokens::int_token,
		simple_derived::spaces_token,
		regular_tokens::identifier_token,
		simple_derived::maybe_spaces_token,
		delimiter_tokens::equals_token,
		simple_derived::maybe_spaces_token,
		regular_tokens::natural_number_token,
		simple_derived::maybe_spaces_token,
		delimiter_tokens::semicolon_token
	>;

	using var_definition = regular_extensions::compound<
		regular_tokens::identifier_token,
		simple_derived::maybe_spaces_token,
		delimiter_tokens::colon_token,
		simple_derived::maybe_spaces_token,
		delimiter_tokens::left_square_bracket_token,
		simple_derived::maybe_spaces_token,
		simple_derived::natural_number_or_identifier_token,
		simple_derived::maybe_spaces_token,
		delimiter_tokens::dot_dot_token,
		simple_derived::maybe_spaces_token,
		simple_derived::natural_number_or_identifier_token,
		simple_derived::maybe_spaces_token,
		delimiter_tokens::right_square_bracket_token,
		simple_derived::maybe_spaces_token,
		regular_extensions::optional<simple_derived::init_value_clause_token>,
		delimiter_tokens::semicolon_token
	>;

	using global_var_definition = regular_extensions::compound<
		keyword_tokens::global_token,
		simple_derived::spaces_token,
		higher_clauses::var_definition
	>;

	using term_token = regular_extensions::compound<
		simple_derived::maybe_spaces_token,
		simple_derived::natural_number_or_identifier_token,
		simple_derived::maybe_spaces_token
	>;

	using comparison_token = //regular_tokens::single_space_token; ///#####
		regular_extensions::compound<
		//term_token,
		//simple_derived::comparison_operator_token//,
		//term_token
		regular_tokens::single_space_token, regular_tokens::single_space_token, regular_tokens::single_space_token
		//,term_token
		>;

	using boolean_operator = regular_extensions::alternative<
		delimiter_tokens::vertical_bar_token,
		delimiter_tokens::ampersand_token
		//,delimiter_tokens::not_...
	>;



	class condition_token : public token {
	public:
		using type = condition_token;
	private:

		std::optional<simple_derived::spaces_token> _leading_spaces;
		std::optional<delimiter_tokens::left_parenthesis_token> _leading_left_parenthesis;
		std::vector</*std::unique_ptr<*/higher_clauses::condition_token/*>*/> _sub_conditions;
		std::vector</*std::unique_ptr<*/higher_clauses::boolean_operator/*>*/> _outer_boolean_operators;
		std::optional<keyword_tokens::false_token> _false_keyword;
		std::optional<keyword_tokens::true_token> _true_keyword;
		std::optional<higher_clauses::comparison_token> _comparison;
		std::optional<delimiter_tokens::right_parenthesis_token> _trailing_right_parenthesis;
		std::optional<simple_derived::spaces_token> _trailing_spaces;

		condition_token() {}

		static std::pair<std::vector<string_const_iterator>, std::vector<string_const_iterator>> find_all_parenthesis(string_const_iterator begin, string_const_iterator end) {
			std::pair<std::vector<string_const_iterator>, std::vector<string_const_iterator>> result;
			for (auto iter = begin; iter != end; ++iter) {
				if (*iter == '(')
					result.first.push_back(iter);
				if (*iter == ')')
					result.second.push_back(iter);
			}
			return result;
		}

		static bool well_bracketed(
			string_const_iterator begin,
			string_const_iterator end,
			const std::optional< std::pair<std::vector<string_const_iterator>, std::vector<string_const_iterator>>>& pre_analyzed_all_paranthesis = std::optional< std::pair<std::vector<string_const_iterator>, std::vector<string_const_iterator>>>()
		) {
			std::pair<std::vector<string_const_iterator>, std::vector<string_const_iterator>> all_parenthesis;
			if (pre_analyzed_all_paranthesis.has_value()) {
				std::copy_if(
					pre_analyzed_all_paranthesis.value().first.begin(),
					pre_analyzed_all_paranthesis.value().first.end(),
					std::back_inserter(all_parenthesis.first),
					[&](const string_const_iterator& it) {
						return !(it < begin) && (it < end);
					}
				);
				std::copy_if(
					pre_analyzed_all_paranthesis.value().second.begin(),
					pre_analyzed_all_paranthesis.value().second.end(),
					std::back_inserter(all_parenthesis.second),
					[&](const string_const_iterator& it) {
						return !(it < begin) && (it < end);
					}
				);
			}
			else
				all_parenthesis = find_all_parenthesis(begin, end);
			if (all_parenthesis.first.size() != all_parenthesis.second.size())
				return false;
			for (std::size_t i = 0; i < all_parenthesis.first.size(); ++i) {
				if (all_parenthesis.first[i] > all_parenthesis.second[i])
					return false; // with n = i + 1, the n-th '(' appears after already seen n times ')'
			}
			return true;
		}

	public:
		condition_token(condition_token&&) = default;
		/*
remove outer spaces
remove outer (...) // if they are matching, just try if the middle string is legally bracketed
split at highest |
split at highes &
look for primitive:
   true
   false
   comparison
*/
		static type parse_string(string_const_iterator begin, string_const_iterator end, std::shared_ptr<std::string> file_content) {


			// parse it completely, recursive
			// if any exception, rethrow it here. cannot_parse_error, ambiguous_parse_error

			// find all cadidates for spaces.
			// determine öargest space chunk at front and back.
			// remove eliminated space chunks

			type result;

			string_const_iterator front_spaces = simple_derived::find_largest_single_space_range_at(begin, end, begin);
			string_const_iterator back_spaces = simple_derived::find_largest_single_space_range_at(begin, end, end);

			if (front_spaces > back_spaces) {
				std::string message;
				message += "Tried to parse a string as condition_token, but the whole string consists of spaces.\n";
				throw not_matching(message, file_content, begin);
			}

			if (front_spaces != begin) {
				result._leading_spaces = std::move(simple_derived::spaces_token::parse_string(begin, front_spaces, file_content));
				begin = front_spaces;
			}
			if (back_spaces != end) {
				result._trailing_spaces = std::move(simple_derived::spaces_token::parse_string(back_spaces, end, file_content));
				end = back_spaces;
			}
			if (begin == end) {
				std::string message;
				message += "Tried to parse an empty string as condition_token.\n";
				throw not_matching(message, file_content, begin);
			}
			// try match front / back ()    (if they are matching, just try if the middle string is legally bracketed)
			if (
				check_match<delimiter_tokens::left_parenthesis_token>(begin, std::next(begin)) &&
				check_match<delimiter_tokens::right_parenthesis_token>(std::prev(end), end) &&
				well_bracketed(std::next(begin), std::prev(end))
				) {
				result._leading_left_parenthesis = std::move(delimiter_tokens::left_parenthesis_token::parse_string(begin, std::next(begin), file_content));
				result._trailing_right_parenthesis = std::move(delimiter_tokens::right_parenthesis_token::parse_string(std::prev(end), end, file_content));
				++begin;
				--end;
				// forward further parsing to sub condition if detected outer parenthesis
				result._sub_conditions.push_back(std::move(condition_token::parse_string(begin, end, file_content)));
				return result;
			}

			const auto all_parenthesis{ find_all_parenthesis(begin, end) };
			const auto count_parenthesis_depth = [&](const string_const_iterator& pos) {
				std::size_t count_left{ 0 };
				for (const auto& lp : all_parenthesis.first) {
					if (lp < pos) ++count_left;
				}
				std::size_t count_right{ 0 };
				for (const auto& rp : all_parenthesis.second) {
					if (rp < pos) ++count_right;
				}
				return count_left - count_right;
			};

			// search some outer "|"
			{
				std::vector<string_const_iterator> pos_of_or_delimiters;
				for (auto iter = begin; iter != end; ++iter) {
					if (check_match<delimiter_tokens::vertical_bar_token>(iter, std::next(iter))) {
						if (count_parenthesis_depth(iter) == 0)
							pos_of_or_delimiters.push_back(iter);
					}
				}

				if (!pos_of_or_delimiters.empty()) { // it is an or - condition
					for (const auto& pos : pos_of_or_delimiters) {
						result._sub_conditions.emplace_back(std::move(condition_token::parse_string(begin, pos, file_content)));
						result._outer_boolean_operators.emplace_back(std::move(higher_clauses::boolean_operator::parse_string(pos, std::next(pos), file_content)));
						begin = std::next(pos);
					}
					result._sub_conditions.emplace_back(std::move(condition_token::parse_string(begin, end, file_content)));
					return result;
				}
			}
			// search some outer "&"
			{
				std::vector<string_const_iterator> pos_of_or_delimiters;
				for (auto iter = begin; iter != end; ++iter) {
					if (check_match<delimiter_tokens::ampersand_token>(iter, std::next(iter))) {
						if (count_parenthesis_depth(iter) == 0)
							pos_of_or_delimiters.push_back(iter);
					}
				}

				if (!pos_of_or_delimiters.empty()) { // it is an and - condition
					for (const auto& pos : pos_of_or_delimiters) {
						result._sub_conditions.emplace_back(std::move(condition_token::parse_string(begin, pos, file_content)));
						result._outer_boolean_operators.emplace_back(std::move(higher_clauses::boolean_operator::parse_string(pos, std::next(pos), file_content)));
						begin = std::next(pos);
					}
					result._sub_conditions.emplace_back(std::move(condition_token::parse_string(begin, end, file_content)));
					return result;
				}
			}
			if (check_match<keyword_tokens::true_token>(begin, end)) { // true
				result._true_keyword = std::move(keyword_tokens::true_token::parse_string(begin, end, file_content));
				return result;
			}
			if (check_match<keyword_tokens::false_token>(begin, end)) { // false
				result._false_keyword = std::move(keyword_tokens::false_token::parse_string(begin, end, file_content));
				return result;
			}

			// when here the condition must consist of an equation. Leading and trailing spaces have already been removed.

			higher_clauses::comparison_token&& xxx = std::move(higher_clauses::comparison_token::parse_string(begin, end, file_content));
			result._comparison.emplace(std::move(xxx));

			return result;
		}

		static std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> find_all_candidates(std::string::const_iterator begin, std::string::const_iterator end) {
			// list all subsection where an x_token might be parsed., might be empty.
			const auto all_parenthesis{ std::make_optional(find_all_parenthesis(begin, end)) };

			std::vector<std::pair<token::string_const_iterator, std::string::const_iterator>> results;
			// brute force soltion: just return everything:
			for (auto iter = begin; iter != end; ++iter)
				for (auto jter = std::next(iter); jter != end; ++jter)
					if (well_bracketed(iter, jter, all_parenthesis))
						results.emplace_back(iter, jter);
			return results;
		}

		/**
		@brief Returns [true, iter] if [begin,iter] matches pattern, otherwise [false, iter] if found a match at later position, [false, end] if no match at all.
		*/
		static std::pair<bool, std::string::const_iterator> has_front_candidate(std::string::const_iterator begin, std::string::const_iterator end) {

			const auto all_parenthesis{ std::make_optional(find_all_parenthesis(begin, end)) };

			std::vector<std::string::const_iterator> results;
			// brute force soltion: just return everything:
			for (auto jter = begin; jter != end; ++jter) {
				if (begin == jter) continue;
				if (well_bracketed(begin, jter, all_parenthesis))
					results.emplace_back(jter);
			}
			if (results.empty())
				return std::make_pair(false, begin);
			else
				return std::make_pair(true, results.back());
		}

	public:
		virtual bool operator==(const token& another) const override {
			try {
				const type& down_casted = dynamic_cast<const type&>(another);
				return
					_leading_spaces == down_casted._leading_spaces &&
					_leading_left_parenthesis == down_casted._leading_left_parenthesis &&
					_sub_conditions == down_casted._sub_conditions &&
					_outer_boolean_operators == down_casted._outer_boolean_operators &&
					_false_keyword == down_casted._false_keyword &&
					_true_keyword == down_casted._true_keyword &&
					_comparison == down_casted._comparison &&
					_trailing_right_parenthesis == down_casted._trailing_right_parenthesis &&
					_trailing_spaces == down_casted._trailing_spaces
					;
			}
			catch (const std::bad_cast&) {
				return false;
			}
		}

		virtual std::string to_string() const override {
			std::string result;
			if (_leading_spaces.has_value()) result += _leading_spaces.value().to_string();
			if (_leading_left_parenthesis.has_value()) result += _leading_left_parenthesis.value().to_string();

			auto iter = _sub_conditions.cbegin();
			auto jter = _outer_boolean_operators.cbegin();
			while (true) {
				if (iter != _sub_conditions.cend()) {
					result += iter->to_string();
					++iter;
				}
				if (jter != _outer_boolean_operators.cend()) {
					result += jter->to_string();
					++jter;
				}
				if (iter == _sub_conditions.cend() && jter == _outer_boolean_operators.cend())
					break;
			}
			if (_false_keyword.has_value()) result += _false_keyword.value().to_string();
			if (_true_keyword.has_value()) result += _true_keyword.value().to_string();
			if (_comparison.has_value()) result += _comparison.value().to_string();

			if (_trailing_right_parenthesis.has_value()) result += _trailing_right_parenthesis.value().to_string();
			if (_trailing_spaces.has_value()) result += _trailing_spaces.value().to_string();
			return result;
		}

		virtual std::string type_info() const override {
			return static_type_info();
		}

		static std::string static_type_info() {
			return "CONDITION_TOKEN";
		}

		virtual const_token_ptr_vector children() const override {
			const_token_ptr_vector result;
			if (_leading_spaces.has_value()) result.push_back(&_leading_spaces.value());
			if (_leading_left_parenthesis.has_value()) result.push_back(&_leading_left_parenthesis.value());

			auto iter = _sub_conditions.cbegin();
			auto jter = _outer_boolean_operators.cbegin();
			while (true) {
				if (iter != _sub_conditions.cend()) {
					result.push_back(&(*iter));
					++iter;
				}
				if (jter != _outer_boolean_operators.cend()) {
					result.push_back(&(*jter));
					++jter;
				}
				if (iter == _sub_conditions.cend() && jter == _outer_boolean_operators.cend())
					break;
			}
			if (_false_keyword.has_value()) result.push_back(&_false_keyword.value());
			if (_true_keyword.has_value()) result.push_back(&_true_keyword.value());
			if (_comparison.has_value()) result.push_back(&_comparison.value());

			if (_trailing_right_parenthesis.has_value()) result.push_back(&_trailing_right_parenthesis.value());
			if (_trailing_spaces.has_value()) result.push_back(&_trailing_spaces.value());
			return result;
		}

	};


	using module_transition_post_condition_probability_distribution_case_token = regular_extensions::compound<
		simple_derived::maybe_spaces_token,
		regular_tokens::float_number_token, // probability
		simple_derived::maybe_spaces_token,
		delimiter_tokens::colon_token,
		higher_clauses::condition_token // post-condition

	>;

	using module_transition_post_condition_probability_distribution_token = regular_extensions::compound<
		module_transition_post_condition_probability_distribution_case_token,
		regular_extensions::kleene_star<
		regular_extensions::compound<
		delimiter_tokens::plus_token,
		module_transition_post_condition_probability_distribution_case_token
		>
		>
	>;

	using module_transition_post_conditions_token = regular_extensions::alternative<
		higher_clauses::condition_token,
		module_transition_post_condition_probability_distribution_token
	>;

	using module_transition_token = regular_extensions::compound<
		delimiter_tokens::left_square_bracket_token, // [
		simple_derived::maybe_spaces_token,
		regular_tokens::identifier_token, // transition label
		simple_derived::maybe_spaces_token,
		delimiter_tokens::right_square_bracket_token, // ]
		//simple_derived::maybe_spaces_token,
		condition_token, // precondition
		delimiter_tokens::ascii_arrow_token, // ->
		module_transition_post_conditions_token,
		delimiter_tokens::semicolon_token
	>;

	using module_section = regular_extensions::compound <
		keyword_tokens::module_token,
		simple_derived::spaces_token,
		regular_tokens::identifier_token, // module name
		simple_derived::spaces_token,
		regular_extensions::kleene_star<
		regular_extensions::alternative<
		module_transition_token,
		var_definition
		>
		>,
		keyword_tokens::endmodule_token,
		simple_derived::spaces_token
	>;

	using formula_definition = regular_extensions::compound<
		keyword_tokens::formula_token,
		simple_derived::spaces_token,
		regular_tokens::identifier_token,
		simple_derived::maybe_spaces_token,
		delimiter_tokens::equals_token,
		//simple_derived::maybe_spaces_token,
		condition_token,
		//simple_derived::maybe_spaces_token,
		delimiter_tokens::semicolon_token
	>;

	using init_section = regular_extensions::compound <
		keyword_tokens::init_token,
		higher_clauses::condition_token, // initial condition on variables
		keyword_tokens::endinit_token
	>;

	using reward_trigger_token = regular_extensions::compound <
		higher_clauses::condition_token,
		delimiter_tokens::colon_token,
		simple_derived::maybe_spaces_token,
		regular_tokens::float_number_token,
		simple_derived::maybe_spaces_token,
		delimiter_tokens::semicolon_token
	>;

	using rewards_section = regular_extensions::compound<
		keyword_tokens::rewards_token,
		simple_derived::spaces_token,
		delimiter_tokens::double_quote_token,
		regular_tokens::identifier_token,
		delimiter_tokens::double_quote_token,
		regular_extensions::kleene_star<reward_trigger_token>,
		simple_derived::maybe_spaces_token,
		keyword_tokens::endrewards_token
	>;

	using dtmc_file_body = regular_extensions::kleene_star<regular_extensions::alternative<
		relaxed_comment_section,
		const_definition,
		global_var_definition,
		module_section,
		formula_definition,
		init_section,
		rewards_section
		>>;

	using dtmc_file = regular_extensions::compound<
		relaxed_comment_section,
		keyword_tokens::dtmc_token,
		dtmc_file_body
	>;

};


////////////////////////////////////////////////////////////////////////////////////////////////////////   OLD STUFF
////////////////////////////////////////////////////////////////////////////////////////////////////////   OLD STUFF
////////////////////////////////////////////////////////////////////////////////////////////////////////   OLD STUFF


#if false
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
#endif



#if false

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

#endif



#if false
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
#endif
