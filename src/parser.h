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
		template <class _MatchResults> //###remove???
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
	catch (const parse_error& e) {
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
	using endmodule_token = string_token<&const_regexes::strings::keywords::ENDMODULE>;
	using endrewards_token = string_token<&const_regexes::strings::keywords::ENDREWARDS>;
	using formula_token = string_token<&const_regexes::strings::keywords::FORMULA>;
	using global_token = string_token<&const_regexes::strings::keywords::GLOBAL>;
	using init_token = string_token<&const_regexes::strings::keywords::INIT>;
	using int_token = string_token<&const_regexes::strings::keywords::INT>;
	using module_token = string_token<&const_regexes::strings::keywords::MODULE>;
	using rewards_token = string_token<&const_regexes::strings::keywords::REWARDS>;

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
	using comparison_operator_token = regex_token<&const_regexes::strings::regulars::comparison_operator>; //## do it as regular_extensions::alternative<...>
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

					try {
						_Token test = _Token::parse_string(iter, end_of_match_candidate, std::shared_ptr<std::string>());
					}
					catch (const parse_error& e) {
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

			bool parsed_successfully;
			std::unique_ptr<_Token> _Token_if_successfully;
			std::string error_message_if_not_successfully;

			sub_parse_struct() = default;
			sub_parse_struct(sub_parse_struct&&) = default;

			inline bool operator == (const sub_parse_struct<_Token>& another) const {
				return parsed_successfully == another.parsed_successfully &&
					(!_Token_if_successfully && !another._Token_if_successfully || _Token_if_successfully && another._Token_if_successfully && *_Token_if_successfully == *another._Token_if_successfully) &&
					error_message_if_not_successfully == another.error_message_if_not_successfully;
			}
		};

		std::tuple<sub_parse_struct<_Tokens>...> _sub_tokens;

		alternative(std::tuple<sub_parse_struct<_Tokens>...>&& sub_tokens) : _sub_tokens(std::move(sub_tokens)) {} //##error copying

		template <class _Token>
		struct helper {
			template <class _Function >
			//static parse_wrapper(_Token(*parse_function)(string_const_iterator, string_const_iterator, std::shared_ptr<std::string>), string_const_iterator begin, string_const_iterator end, std::shared_ptr<std::string> file_content) {
			static sub_parse_struct<_Token> parse_wrapper(_Function parse_function, string_const_iterator begin, string_const_iterator end, std::shared_ptr<std::string> file_content) {
				sub_parse_struct<_Token> result;
				try {
					result._Token_if_successfully.reset(new _Token(parse_function(begin, end, file_content)));
					//return std::pair<bool, std::unique_ptr<_Token>>(true, std::move(std::unique_ptr()));
					//return std::make_pair(true, std::make_unique<_Token>()); //###
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
			std::apply([&result](auto&& ... args) { (((args.parsed_successfully) ? result += args._Token_if_successfully->to_string() : result), ...); }, _sub_tokens);
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
			std::apply([&result](auto&& ... element) { ((element.parsed_successfully ? result.push_back(element._Token_if_successfully.get()) : false), ...); }, _sub_tokens);
			/*for (std::size_t i{ 0 }; i < _sub_tokens; ++i) { //### erg�nzen
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
					correctly_parsed_collected.emplace_back(c.front().first, c.back().second());
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
					correctly_parsed_collected.emplace_back(c.back().second());
				}
				catch (const parse_error&) {
					//ignore, do not kep th collected candidate.
				}
			}

			if (correctly_parsed_collected.empty())
				return std::make_pair(false, begin);
			else
				return std::make_pair(true, *std::max_element(correctly_parsed_collected.cbegin(), correctly_parsed_collected.cend()))
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
	using comment_line_ending = regular_extensions::compound<delimiter_tokens::double_slash_token, regular_extensions::kleene_star<regular_tokens::anything_but_newline_token>>;
	//using any_ignorable_line_ending = regular_extensions::compound< regular_extensions::kleene_star<>, delimiter_tokens::double_slash_token, regular_extensions::kleene_star<regular_tokens::anything_but_newline_token>>;

};

struct higher_clauses {

	using relaxed_comment_section = regular_extensions::kleene_star<
		regular_extensions::alternative<
		simple_derived::comment_line_ending,
		regular_tokens::single_space_token
		>
	>;

	using const_definition = regular_extensions::compound<
		keyword_tokens::const_token,
		regular_extensions::kleene_plus<regular_tokens::single_space_token>,
		keyword_tokens::int_token,
		regular_extensions::kleene_plus<regular_tokens::single_space_token>,
		regular_tokens::identifier_token,
		regular_extensions::kleene_star<regular_tokens::single_space_token>,
		delimiter_tokens::equals_token,
		regular_extensions::kleene_star<regular_tokens::single_space_token>,
		regular_tokens::natural_number_token,
		regular_extensions::kleene_star<regular_tokens::single_space_token>,
		delimiter_tokens::semicolon_token
	>;

	using global_var_definition = delimiter_tokens::semicolon_token; //####expand
	using module_section = delimiter_tokens::semicolon_token; //####expand
	using formula_definition = delimiter_tokens::semicolon_token; //####expand
	using init_section = delimiter_tokens::semicolon_token; //####expand
	using rewards_section = delimiter_tokens::semicolon_token; //####expand

	using dtmc_file_body = regular_extensions::kleene_star<regular_extensions::alternative<relaxed_comment_section, const_definition, global_var_definition, module_section, formula_definition, init_section, rewards_section>>;

	using dtmc_file = regular_extensions::compound<relaxed_comment_section, keyword_tokens::dtmc_token, dtmc_file_body>;

};


////////////////////////////////////////////////////////////////////////////////////////////////////////   OLD STUFF
////////////////////////////////////////////////////////////////////////////////////////////////////////   OLD STUFF
////////////////////////////////////////////////////////////////////////////////////////////////////////   OLD STUFF
////////////////////////////////////////////////////////////////////////////////////////////////////////   OLD STUFF
////////////////////////////////////////////////////////////////////////////////////////////////////////   OLD STUFF
////////////////////////////////////////////////////////////////////////////////////////////////////////   OLD STUFF
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

#endif



#if false


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

#endif