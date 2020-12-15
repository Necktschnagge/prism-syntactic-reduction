#pragma once

#include <string>
#include <exception>

class not_implemented : public std::exception {
	std::string message;

	inline static const std::string INTERNAL_ERROR{ "Missing Implementation of ceratin feature:  " };
	inline static const std::string PLEASE_REPORT{ "  Please report this to the developers: Open an issue at [...] and copy-paste the error message. If possible describe the scenario which led to this error." };
public:
	static void assert_true(bool condition, const not_implemented& exception) {
		if (!condition) throw exception;
	}

	not_implemented(const std::string& message) : message(INTERNAL_ERROR + message + PLEASE_REPORT) {}
	not_implemented(const char* message) : not_implemented(std::string(message)) {}
	not_implemented(const not_implemented&) = default;

	const char* what() const noexcept override {
		return message.c_str();
	}
};
