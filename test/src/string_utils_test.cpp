#include <catch2/catch.hpp>
#include <glsl_assembler/string_utils.h>

using namespace StringUtils;

SCENARIO("StringUtils works", "[string_utils_test.cpp]") {
    const std::string s = "\t MYSTRING\t ";
    REQUIRE(trim_copy(s) == "MYSTRING");
    REQUIRE(ltrim_copy(s) == "MYSTRING\t ");
    REQUIRE(rtrim_copy(s) == "\t MYSTRING");
    REQUIRE(s == "\t MYSTRING\t ");

    const std::vector<std::string> list = { "first", "second", "third" };
    REQUIRE(join(list) == "firstsecondthird");
    REQUIRE(join(list, ", ") == "first, second, third");

    REQUIRE(splitLines("first\nsecond\n") == std::vector<std::string>{"first", "second"});

    REQUIRE(split("first,second,third", ",") == std::vector<std::string>{ "first", "second", "third" });

    REQUIRE(startsWith("myString", "my"));
    REQUIRE(!startsWith("MYString", "my"));
    REQUIRE(endsWith("myString", "String"));
    REQUIRE(!endsWith("mySTRING", "String"));

    REQUIRE(replaceAll("It's a fair bet that if it's fair tomorrow", "fair", "unfair") == "It's a unfair bet that if it's unfair tomorrow");
}
