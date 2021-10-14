# CMake search for this GLSLAssembler.cmake file when another package uses
# find_package(GLSLAssembler). GLSLAssembler.cmake includes GLSLAssemblerTargets.cmake file which contains
# the exported information of targets GLSLAssembler, this is how the targets and their properties
# are imported into another project.
include(CMakeFindDependencyMacro)
include("${CMAKE_CURRENT_LIST_DIR}/GLSLAssemblerTargets.cmake")
