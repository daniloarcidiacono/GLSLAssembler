#include <catch2/catch.hpp>
#include <glsl_assembler/simple_module_loader.h>

class TestModuleLoader : public SimpleModuleLoader {
public:
    std::string load(const std::string &path) override {
        // Not used in test
        return "";
    }
};

SCENARIO("SimpleModuleLoader works", "[simple_module_loader_test.cpp]") {
    TestModuleLoader loader;
    REQUIRE(loader.isPath(""));
    REQUIRE(loader.isPath("my_path"));
//    REQUIRE(!loader.isPath("my_path/file.txt"));
    REQUIRE(loader.extractPath("my_path/file.txt") == "my_path");
    REQUIRE(loader.join("my_path", "/file.txt") == "my_path/file.txt");
    REQUIRE(loader.join("my_path", "file.txt") == "my_path/file.txt");
    REQUIRE(loader.join("my_path/nested", "../file.txt") == "my_path/file.txt");
    REQUIRE(loader.join("my_path", "../file.txt") == "file.txt");
    REQUIRE_THROWS(loader.join("my_path", "../../file.txt"));
}
