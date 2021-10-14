set(MODULE_TARGET_TESTS GLSLAssemblerTests)
set(
    MODULE_TEST_SRCS
        src/main.cpp
        src/module_graph_test.cpp
        src/simple_module_loader_test.cpp
        src/string_utils_test.cpp
)

set(
    MODULE_TEST_INCLUDES
)

set(
    MODULE_TEST_RESOURCES
        resources/shaders/comment/a.glsl
        resources/shaders/comment/main.glsl
        resources/shaders/comment/assembled.glsl

        resources/shaders/cycle/a.glsl
        resources/shaders/cycle/b.glsl
        resources/shaders/cycle/main.glsl

        resources/shaders/diamond/a.glsl
        resources/shaders/diamond/b.glsl
        resources/shaders/diamond/c.glsl
        resources/shaders/diamond/main.glsl
        resources/shaders/diamond/assembled.glsl

        resources/shaders/hoisting/a.glsl
        resources/shaders/hoisting/b.glsl
        resources/shaders/hoisting/main.glsl
        resources/shaders/hoisting/assembled.glsl

        resources/shaders/relative/utils/nested/a.glsl
        resources/shaders/relative/utils/b.glsl
        resources/shaders/relative/main.glsl
        resources/shaders/relative/assembled.glsl

        resources/shaders/simple/a.glsl
        resources/shaders/simple/b.glsl
        resources/shaders/simple/main.glsl
        resources/shaders/simple/assembled.glsl
)

# Bundle resources
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CMakeRC.cmake")
cmrc_add_resource_library(
    GLSLAssemblerTestsRC
    ALIAS GLSLAssemblerTests::rc
    NAMESPACE GLSLAssemblerTests
    ${MODULE_TEST_RESOURCES}
)

add_executable(${MODULE_TARGET_TESTS} ${MODULE_TEST_SRCS} ${MODULE_TEST_INCLUDES})

# Catch2
target_link_libraries(${MODULE_TARGET_TESTS} Catch2::Catch2)

# GLSLAssembler
target_link_libraries(${MODULE_TARGET_TESTS} GLSLAssembler)
if (GLSLASSEMBLER_BUILD_SHARED_LIB)
    add_custom_command(
        TARGET ${MODULE_TARGET_TESTS} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:GLSLAssembler>
        $<TARGET_FILE_DIR:${MODULE_TARGET_TESTS}>
    )
endif()

# Test resources
target_link_libraries(${MODULE_TARGET_TESTS} GLSLAssemblerTests::rc)

target_compile_features(
    ${MODULE_TARGET_TESTS}
    PUBLIC
        cxx_std_11
)

target_include_directories(
    ${MODULE_TARGET_TESTS}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set_target_properties(
    ${MODULE_TARGET_TESTS}
    PROPERTIES
        RELEASE_POSTFIX "_${PROJECT_VERSION}"
        RELWITHDEBINFO_POSTFIX "_${PROJECT_VERSION}"
        DEBUG_POSTFIX "_${PROJECT_VERSION}d"
)

catch_discover_tests(${MODULE_TARGET_TESTS})
