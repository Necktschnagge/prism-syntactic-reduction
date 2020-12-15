
#include "../src/not_implemented.h"

#include "gtest/gtest.h"


TEST(not_implemented, assert_true) {
	try {
		not_implemented::assert_true(true, "some message");
	}
	catch (...) {
		FAIL() << "throwing with condition true";
	}
	try {
		not_implemented::assert_true(false, "some message");
	}
	catch (const not_implemented&) {
		return;
	}
	catch (...) {
		
	}
	FAIL() << "Not throwing correctly with condition false";
}
	