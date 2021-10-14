cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
option(GLSLASSEMBLER_BUILD_SHARED_LIB "Build shared lib" OFF)
option(GLSLASSEMBLER_BUILD_DOCS "Build documentation" ON)
set(PACKAGE_VERSION 0.1)

# Define the root project
project(
    GLSLAssembler
        VERSION ${PACKAGE_VERSION}
        DESCRIPTION "Assembler for GLSL files"
        LANGUAGES CXX
)

# Let's nicely support folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Create main target
set(MODULE_TARGET GLSLAssembler)
set(
    MODULE_INCLUDES
        include/glsl_assembler/conf.h
        include/glsl_assembler/module.h
        include/glsl_assembler/module_graph.h
        include/glsl_assembler/module_loader.h
        include/glsl_assembler/simple_module_loader.h
        include/glsl_assembler/string_utils.h
)

set(
    MODULE_SRCS
        src/module.cpp
        src/module_graph.cpp
        src/string_utils.cpp
)

# Define the library
if (GLSLASSEMBLER_BUILD_SHARED_LIB)
    message(WARNING "Building GLSLAssembler as a shared library is not recommended, since it uses STL classes in its interfaces")
    add_library(${MODULE_TARGET} SHARED ${MODULE_SRCS} ${MODULE_INCLUDES})
    target_compile_definitions(
        ${MODULE_TARGET}
        PUBLIC
            GLSLASSEMBLER_SHARED=1
        PRIVATE
            GLSLASSEMBLER_BUILD=1
    )
else()
    add_library(${MODULE_TARGET} STATIC ${MODULE_SRCS} ${MODULE_INCLUDES})
    target_compile_definitions(
        ${MODULE_TARGET}
        PRIVATE
            GLSLASSEMBLER_BUILD=1
    )
endif()

# Allow includes from include/
target_include_directories(
    ${MODULE_TARGET}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(
    ${MODULE_TARGET}
    PROPERTIES
        RELEASE_POSTFIX "_${PROJECT_VERSION}"
        RELWITHDEBINFO_POSTFIX "_${PROJECT_VERSION}"
        DEBUG_POSTFIX "_${PROJECT_VERSION}d"
        PUBLIC_HEADER "${MODULE_INCLUDES}"
        LINKER_LANGUAGE CXX
)

target_compile_features(
    ${MODULE_TARGET}
    PUBLIC
        cxx_std_11
)

##################################################
# Tests
##################################################
include(CTest) # note: this adds a BUILD_TESTING which defaults to ON
if (BUILD_TESTING)
    # We use Catch2 as testing framework
    find_package(Catch2 REQUIRED)
    include(Catch)

    # Add the test folder
    add_subdirectory(test)
endif()

##################################################
# Documentation
##################################################
if (GLSLASSEMBLER_BUILD_DOCS)
    find_package(Doxygen REQUIRED)

    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/config-file.doxygen)
    set(DOCS_TARGET GLSLAssemblerDocs)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(
        ${DOCS_TARGET} ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

##################################################
# Install
##################################################
# The first install sets the installation locations for each target and puts the information
# out to GLSLAssemblerTargets.
install(
    TARGETS ${MODULE_TARGET}
    EXPORT GLSLAssemblerTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/glsl_assembler
)

# Writes out the information that was set to GLSLAssemblerTargets to GLSLAssemblerTargets.cmake file and
# copies the GLSLAssemblerTargets.cmake file to lib/cmake/GLSLAssembler directory
install(
    EXPORT GLSLAssemblerTargets
    FILE GLSLAssemblerTargets.cmake
    NAMESPACE GLSLAssembler::
    DESTINATION lib/cmake/GLSLAssembler
)

# It's usually a good idea to give CMake access to the version, so that find_package can have
# a version specified.
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/GLSLAssemblerConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/GLSLAssemblerConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/GLSLAssemblerConfigVersion.cmake
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/lib/cmake/GLSLAssembler
)

if (GLSLASSEMBLER_BUILD_DOCS)
    install(
        DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}/docs/html
        DESTINATION
            ${CMAKE_INSTALL_PREFIX}/share/doc
    )
endif()
