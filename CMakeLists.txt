cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME Syntactic-Reducer)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

project(${PROJECT_NAME} VERSION 1.0)

set( CMAKE_VERBOSE_MAKEFILE on )

############### Language and Flags ###############

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)
#message(${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Ob0 /Od /RTC1")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") # due to linkage error on cloud linux build.

############### Libraries ###############

add_subdirectory(extern/googletest)


#Boost
if (MSVC)
    set(Boost_USE_STATIC_LIBS ON)
else()
	set(Boost_USE_STATIC_LIBS OFF)
endif (MSVC)
FIND_PACKAGE(Boost 1.54.0 COMPONENTS regex REQUIRED )
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIR} )


include_directories(SYSTEM extern/json/include)

include_directories(SYSTEM extern/spdlog/include)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


############### Sources ###############

add_subdirectory(src)

add_subdirectory(test)

add_subdirectory(tools)

############### Testing ###############

enable_testing()


############### Linkage ###############




############### Visual Studio ###############

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})#Set Visualo Studio start-up project, so that one can directly run the debugger.


################################
# Unit Tests
################################
# Add test cpp file
#add_executable(runUnitTests test/example_test.cpp test/test_main.cpp)
# Link test executable against gtest & gtest_main
#target_link_libraries(runUnitTests gtest gtest_main)
#add_test( runUnitTests runUnitTests )