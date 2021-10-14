#include <catch2/catch.hpp>
#include <glsl_assembler/module.h>
#include <glsl_assembler/module_loader.h>
#include <glsl_assembler/simple_module_loader.h>
#include <glsl_assembler/module_graph.h>
#include <cmrc/cmrc.hpp>

CMRC_DECLARE(GLSLAssemblerTests);

static std::string loadFile(const std::string &name) {
    static cmrc::embedded_filesystem fs = cmrc::GLSLAssemblerTests::get_filesystem();
    cmrc::file resource = fs.open(name);
    return std::string(resource.begin(), resource.end());
}

class CMRCModuleLoader : public SimpleModuleLoader {
public:
    std::string load(const std::string &path) override {
        return StringUtils::replaceAll(loadFile(path), "\r\n", "\n");
    }
};

SCENARIO("ModuleGraph simple", "[module_graph_test.cpp]") {
    CMRCModuleLoader loader;
    ModuleGraph moduleGraph;
    moduleGraph.setModuleLoader(&loader);
    moduleGraph.setIncludeDir("resources/shaders/simple");
    moduleGraph.loadModule("resources/shaders/simple/main.glsl");
    REQUIRE(moduleGraph.getAssembledSource() == loader.load("resources/shaders/simple/assembled.glsl"));

    REQUIRE(moduleGraph.getModuleCount() == 3);
    Module *aModule = moduleGraph.getSortedModule(0);
    Module *bModule = moduleGraph.getSortedModule(1);
    Module *mainModule = moduleGraph.getSortedModule(2);
    REQUIRE(aModule->getId() == "resources/shaders/simple/a.glsl");
    REQUIRE(bModule->getId() == "resources/shaders/simple/b.glsl");
    REQUIRE(mainModule->getId() == "resources/shaders/simple/main.glsl");

    REQUIRE(aModule->getDependencyCount() == 0);
    REQUIRE(bModule->getDependencyCount() == 0);
    REQUIRE(mainModule->getDependencyCount() == 2);
    REQUIRE(mainModule->getDependency(0).module == aModule);
    REQUIRE(mainModule->getDependency(1).module == bModule);

    REQUIRE(moduleGraph.getSourceBlocksCount() == 3);
    REQUIRE(moduleGraph.getSourceBlock(0).assembledRange.begin == 1);
    REQUIRE(moduleGraph.getSourceBlock(0).assembledRange.end == 5);
    REQUIRE(moduleGraph.getSourceBlock(1).assembledRange.begin == 8);
    REQUIRE(moduleGraph.getSourceBlock(1).assembledRange.end == 12);
    REQUIRE(moduleGraph.getSourceBlock(2).assembledRange.begin == 15);
    REQUIRE(moduleGraph.getSourceBlock(2).assembledRange.end == 21);

    REQUIRE(StringUtils::startsWith(mainModule->getSourceLine(mainModule->getDependency(0).includeLine), "//"));
    REQUIRE(StringUtils::startsWith(mainModule->getSourceLine(mainModule->getDependency(1).includeLine), "//"));

    int moduleLine;
    REQUIRE(moduleGraph.mapLine(1, moduleLine) == aModule);
    REQUIRE(moduleLine == 0);

    REQUIRE(moduleGraph.mapLine(11, moduleLine) == bModule);
    REQUIRE(moduleLine == 3);

    REQUIRE(moduleGraph.mapLine(19, moduleLine) == mainModule);
    REQUIRE(moduleLine == 4);
}

SCENARIO("ModuleGraph cycle", "[module_graph_test.cpp]") {
    CMRCModuleLoader loader;
    ModuleGraph moduleGraph;
    moduleGraph.setModuleLoader(&loader);
    moduleGraph.setIncludeDir("resources/shaders/cycle");
    REQUIRE_THROWS_WITH(
        moduleGraph.loadModule("resources/shaders/cycle/main.glsl"),
        "Dependency cycle: resources/shaders/cycle/main.glsl --> resources/shaders/cycle/a.glsl --> resources/shaders/cycle/b.glsl --> resources/shaders/cycle/main.glsl"
    );
}

SCENARIO("ModuleGraph comment", "[module_graph_test.cpp]") {
    CMRCModuleLoader loader;
    ModuleGraph moduleGraph;
    moduleGraph.setModuleLoader(&loader);
    moduleGraph.setIncludeDir("resources/shaders/comment");
    moduleGraph.loadModule("resources/shaders/comment/main.glsl");
    REQUIRE(moduleGraph.getAssembledSource() == loader.load("resources/shaders/comment/assembled.glsl"));
    REQUIRE(moduleGraph.getModuleCount() == 1);
}

SCENARIO("ModuleGraph relative", "[module_graph_test.cpp]") {
    CMRCModuleLoader loader;
    ModuleGraph moduleGraph;
    moduleGraph.setModuleLoader(&loader);
    moduleGraph.setIncludeDir("resources/shaders/relative");
    moduleGraph.loadModule("resources/shaders/relative/main.glsl");
    REQUIRE(moduleGraph.getAssembledSource() == loader.load("resources/shaders/relative/assembled.glsl"));
}

SCENARIO("ModuleGraph diamond", "[module_graph_test.cpp]") {
    CMRCModuleLoader loader;
    ModuleGraph moduleGraph;
    moduleGraph.setModuleLoader(&loader);
    moduleGraph.setIncludeDir("resources/shaders/diamond");
    moduleGraph.loadModule("resources/shaders/diamond/main.glsl");
    REQUIRE(moduleGraph.getAssembledSource() == loader.load("resources/shaders/diamond/assembled.glsl"));
}

SCENARIO("ModuleGraph hoisting", "[module_graph_test.cpp]") {
    CMRCModuleLoader loader;
    ModuleGraph moduleGraph;
    moduleGraph.setModuleLoader(&loader);
    moduleGraph.setIncludeDir("resources/shaders/hoisting");
    moduleGraph.loadModule("resources/shaders/hoisting/main.glsl");
    REQUIRE(moduleGraph.getAssembledSource() == loader.load("resources/shaders/hoisting/assembled.glsl"));
}
