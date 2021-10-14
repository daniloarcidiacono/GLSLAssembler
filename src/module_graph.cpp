#include <glsl_assembler/module_graph.h>
#include <glsl_assembler/module.h>
#include <glsl_assembler/module_loader.h>
#include <glsl_assembler/string_utils.h>
#include <queue>
#include <stdexcept>
#include <iostream>

ModuleGraph::ModuleGraph() {
}

ModuleGraph::~ModuleGraph() {
    destroy();
}

void ModuleGraph::destroy() {
    for (const Module *module : modules) {
        delete module;
    }

    modules.clear();
    toposort.clear();
    assembledSource = "";
    assembledSourceBlocks.clear();
}

Module *ModuleGraph::findModule(const std::string &id) {
    for (Module *module : modules) {
        if (module->getId() == id) {
            return module;
        }
    }

    return nullptr;
}

Module *ModuleGraph::mapLine(const int assembledLine, int &moduleLine) const {
    for (const SourceBlock &sourceBlock : assembledSourceBlocks) {
        if (sourceBlock.contains(assembledLine)) {
            moduleLine = sourceBlock.mapLine(assembledLine);
            return sourceBlock.module;
        }
    }

    moduleLine = -1;
    return nullptr;
}

const std::string &ModuleGraph::loadModule(const std::string &modulePath) {
    if (!moduleLoader) {
        throw std::runtime_error("No module loader specified!");
    }

    // Destroy old data (if present)
    destroy();

    // Load the root module and register it
    Module *rootModule;
    try {
        rootModule = Module::fromSource(modulePath,moduleLoader->load(modulePath));
        modules.push_back(rootModule);
    } catch (...) {
        std::cerr << "Could not load module " << modulePath << std::endl;
        throw;
    }

    // Expand to other modules
    std::queue<Module *> queue;
    queue.push(rootModule);
    while (!queue.empty()) {
        // Extract a module
        Module *module = queue.front();
        queue.pop();

        // Handle its dependencies
        for (Module::Dependency &dependency : *module) {
            // Build the full path
            if (dependency.type == Module::Dependency::Type::ABSOLUTE) {
                dependency.moduleId = moduleLoader->join(includeDir, dependency.moduleId);
            } else {
                dependency.moduleId = moduleLoader->join(moduleLoader->extractPath(module->getId()), dependency.moduleId);
            }

            // Reuse an existing module
            dependency.module = findModule(dependency.moduleId);

            // If it's a new module
            if (!dependency.module) {
                // Load and store it
                try {
                    dependency.module = Module::fromSource(dependency.moduleId, moduleLoader->load(dependency.moduleId));
                    modules.push_back(dependency.module);
                    queue.push(dependency.module);
                } catch (...) {
                    std::cerr << module->getId() << " line " << (dependency.includeLine + 1) << ": Could not load module " << dependency.moduleId << std::endl;
                    throw;
                }
            }
        }
    }

    // Build the topological sort
    buildTopologicalSort();

    // Assemble the source
    assembleSource();

    return assembledSource;
}

void ModuleGraph::buildTopologicalSort() {
    // Compute topological sort (Tarjan)
    std::queue<Module *> queue;
    for (Module *module : modules) {
        module->setMark(Module::Mark::UNMARKED);
        queue.push(module);
    }

    std::vector<std::string> stack;
    toposort.clear();
    while (!queue.empty()) {
        Module *curNode = queue.front();
        queue.pop();

        stack.push_back(curNode->getId());
        buildTopologicalSort(curNode, stack);
    }
}

void ModuleGraph::buildTopologicalSort(Module *module, std::vector<std::string> &stack) {
    if (module->getMark() == Module::Mark::PERMANENT) {
        return;
    } else if (module->getMark() == Module::Mark::TEMPORARY) {
        throw std::runtime_error("Dependency cycle: " + StringUtils::join(stack, " --> "));
    }
    module->setMark(Module::Mark::TEMPORARY);

    for (Module::Dependency &dependency : *module) {
        stack.push_back(dependency.moduleId);
        buildTopologicalSort(dependency.module, stack);
        stack.pop_back();
    }

    module->setMark(Module::Mark::PERMANENT);
    toposort.push_back(module);
}

void ModuleGraph::assembleSource() {
    std::vector<std::string> assembledSourceLines;

    // First hoisted lines
    for (Module *module : toposort) {
        for (int i = 0; i < module->getHoistedLinesCount(); i++) {
            const Module::HoistedLine &hoistedLine = module->getHoistedLine(i);
            assembledSourceLines.push_back(hoistedLine.line);

            // Build the source block mapping for the hoisted line
            SourceBlock block;
            block.module = module;
            block.moduleRange.begin = block.moduleRange.end = hoistedLine.index;
            block.assembledRange.begin = block.assembledRange.end = assembledSourceLines.size() - 1;
            assembledSourceBlocks.push_back(block);
        }
    }

    // Then modules
    for (Module *module : toposort) {
        // Skip empty modules
        if (!module->isEmpty()) {
            const int begin = assembledSourceLines.size();
            module->inject(assembledSourceLines);
            const int end = assembledSourceLines.size() - 1;

            // Build the source block mapping for the module
            SourceBlock block;
            block.module = module;
            block.moduleRange.begin = 0;
            block.moduleRange.end = module->getSourceLinesCount() - 1;
            block.assembledRange.begin = begin + 1; // skip the MODULE BEGIN comment
            block.assembledRange.end = end - 1; // skip the empty line module separator
            assembledSourceBlocks.push_back(block);
        }
    }

    assembledSource = StringUtils::join(assembledSourceLines, "\n");
}

void ModuleGraph::setIncludeDir(const std::string &includeDir) {
    if (!moduleLoader) {
        throw std::runtime_error("No module loader specified!");
    }

    if (!moduleLoader->isPath(includeDir)) {
        throw std::runtime_error("Include dir is not a path!");
    }

    this->includeDir = includeDir;
}
