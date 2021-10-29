#include "char_helper.h"

#include <codecvt>

/*
template <class T>
auto path_to_string_template(const std::filesystem::path& path) -> std::string {
	auto f = [&]() {
		if constexpr (std::is_same<std::filesystem::path::value_type, wchar_t>::value) {
			return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(path.c_str());
		}
		else {
			return std::string(path.c_str());
		}
	};
	return f();
};

namespace {

	void foo() {
		path_to_string_template<void>;
	}
}

*/

auto path_to_string(const std::filesystem::path& path) -> std::string {
#ifdef _MSC_VER
	return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(path.c_str());
#else
	return std::string(path.c_str());
#endif	
};