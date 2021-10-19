#pragma once

#include <string>
#include <exception>

class parse_error : public std::exception {
	std::string message;

public:
	static void assert_true(bool condition, const parse_error& exception) {
		if (!condition) throw exception;
	}

	parse_error(const std::string& message) : message(message) {}
	parse_error(const char* message) : parse_error(std::string(message)) {}
	parse_error(const parse_error&) = default;

	const char* what() const noexcept override {
		return message.c_str();
	}
};

class parse_primitive_error : public std::exception {
	std::string message;

public:
	static void assert_true(bool condition, const parse_primitive_error& exception) {
		if (!condition) throw exception;
	}

	parse_primitive_error(const std::string& message) : message(message) {}
	parse_primitive_error(const char* message) : parse_primitive_error(std::string(message)) {}
	parse_primitive_error(const parse_primitive_error&) = default;

	const char* what() const noexcept override {
		return message.c_str();
	}
};

class not_matching : public parse_error {
	std::string _actual;
	std::string _expected_description;
	std::string _details;

	std::shared_ptr<std::string> _file_content;
	std::string::const_iterator _error_position;

	static std::string get_position_description(std::shared_ptr<std::string> file_content, std::string::const_iterator error_position) {
		if (!file_content) return "[ Fatal: Could not get information on error location! ]\n";
		std::string::const_iterator iter{ file_content->cbegin() };
		std::size_t line{ 1 };
		std::size_t column{ 1 };
		while (iter != error_position) {

			if (*iter == '\n') {
				++line;
				column = 0;
			}
			++column;
		}
		std::string position_message;
		position_message += "On line " + std::to_string(line);
		position_message += ", column " + std::to_string(column);
		position_message += ", position " + std::to_string(std::size_t(1) + std::distance(file_content->cbegin(), error_position)) + ":\n";
		return position_message;
	}

public:

	not_matching(const std::string& actual, const std::string& expected_description) :
		parse_error(std::string("Got:   ") + actual + "\nExpected:   " + expected_description),
		_actual(actual),
		_expected_description(expected_description)
	{
	}

	not_matching(std::string::const_iterator actual_begin, std::string::const_iterator actual_end, const std::string& expected_description, const std::string& details) :
		parse_error(std::string("Got:   ") + std::string(actual_begin, actual_end) + "\nExpected:   " + expected_description + "\n\nDetails:\n" + details),
		_actual(actual_begin, actual_end),
		_expected_description(expected_description),
		_details(details)
	{	}

	not_matching(const std::string& message, std::shared_ptr<std::string> file_content, std::string::const_iterator error_position) :
		parse_error(get_position_description(file_content, _error_position) + message),
		_file_content(file_content),
		_error_position(error_position)
	{	}
	/*
	[[deprecated]] not_matching(const std::string& message) :
		parse_error(message)
	{	}
	*/
};
