#pragma once
#include <glsl_assembler/conf.h>
#include <vector>
#include <string>

// Forward declarations
class Module;
class ModuleLoader;

/**
 * <p>ModuleGraph is the main interface of GLSLAssembler. It encapsulates all the {@link Module} along with their dependencies.
 * <p>There are 3 steps to use this class:
 * <ol>
 *  <li>Set the module loader with {@link #setModuleLoader()}</li>
 *  <li>Set the include base path {@link #setIncludeDir()}</li>
 *  <li>Load a module with {@link #loadModule()}</li>
 * </ol>
 *
 * <p>If the module is loaded successfully, the assembled source can be retrieved with {@link #getAssembledSource()}.
 * <p>The same instance can be reused to load multiple (unrelated) modules.
 * <p>This class is <strong>NOT</strong> threadsafe.
 */
class GLSLASSEMBLER_API ModuleGraph {
public:
    /**
     * A continguous line index range.
     */
    struct GLSLASSEMBLER_API LineRange {
        /**
         * The start of the range (zero-based)
         */
        int begin = -1;

        /**
         * The end of the range (zero-based)
         */
        int end = - 1;

        /**
         * Checks whether a line index is contained inside the range.
         *
         * @param line the index of the line (zero-based)
         * @return true if the range contains the line, false otherwise
         */
        bool contains(const int line) const {
            return line >= begin && line <= end;
        }
    };

    /**
     * A source block contains the mapping between the assembled (global) and module (local) line ranges.
     */
    struct GLSLASSEMBLER_API SourceBlock {
        /**
         * The line range in the assembled source.
         */
        LineRange assembledRange;

        /**
         * The line range in the module source.
         */
        LineRange moduleRange;

        /**
         * Reference to the module (not owned by this class).
         */
        Module *module = nullptr;

        /**
         * Checks whether a global line index is contained inside the block.
         * @param assembledLine the global (i.e. relative to the assembled source) line index (zero-based)
         * @return true if the source block contains the line, false otherwise
         */
        bool contains(const int assembledLine) const {
            return assembledRange.contains(assembledLine);
        }

        /**
         * Maps a global line index into the local module index.
         * @param assembledLine the zero-based global line index. It must be contained into the source block.
         * @return the local line index (zero-based).
         */
        int mapLine(const int assembledLine) const {
            return assembledLine - assembledRange.begin;
        }
    };

private:
    /**
     * Modules are the node of the graph (owned by this class).
     */
    std::vector<Module *> modules;

    /**
     * Topological sort of modules (references). Populated by {@link #buildTopologicalSort()}.
     */
    std::vector<Module *> toposort;

    /**
     * Include directory (only one is supported). This is used for #include<...> directives
     */
    std::string includeDir;

    /**
     * Loader for modules (not owned by this class).
     */
    ModuleLoader *moduleLoader;

    /**
     * Fully assembled source, with include directives resolved. Populated by {@link #assembleSource()}.
     */
    std::string assembledSource;

    /**
     * Source blocks composing the assembled sources, used for line mapping. Populated by {@link #assembleSource()}.
     */
    std::vector<SourceBlock> assembledSourceBlocks;

    /**
     * Frees all the allocated memory
     */
    void destroy();

    /**
     * Builds the topological sort of the modules. An exception is thrown if a cycle is found.
     */
    void buildTopologicalSort();

    /**
     * Helper recursive method for topological sort.
     * @param module the current module to examine
     * @param stack the stack of module ids (used to report dependency cycles)
     * @throws std::runtime_error if a cycle is detected.
     */
    void buildTopologicalSort(Module *module, std::vector<std::string> &stack);

    /**
     * Assembles the source and computes the source blocks.
     */
    void assembleSource();

public:
    ModuleGraph();
    ~ModuleGraph();

    /**
     * @return Begin iterator for topological sort of modules.
     */
    std::vector<Module *>::iterator begin() { return toposort.begin(); }

    /**
     * @return End iterator for topological sort of modules.
     */
    std::vector<Module *>::iterator end() { return toposort.end(); }

    /**
     * Builds the module graph starting from a single module.
     * A module loader must be set before invoking this method.
     * @param modulePath the pathname of the first module to load.
     * @return the assembled source
     */
    const std::string &loadModule(const std::string &modulePath);

    /**
     * @param id the id of the module
     * @return the module having the specified id, or null if it does not exist.
     */
    Module *findModule(const std::string &id);

    /**
     * Given a line in the assembled source, returns the corresponding local line index and module.
     * @param assembledLine the line index in the assembled source (zero-based).
     * @param moduleLine will contain the local line index (zero-based).
     * @return the module if a mapping is found, nullptr otherwise.
     */
    Module *mapLine(const int assembledLine, int &moduleLine) const;

    /**
     * @return the number of modules.
     */
    int getModuleCount() const { return modules.size(); }

    /**
     * @return the module loader.
     */
    ModuleLoader *getModuleLoader() const { return moduleLoader; }

    /**
     * @param index of the module relative to the topological sort
     * @return the module at position index in the topological sort.
     */
    Module *getSortedModule(const int index) const { return toposort.at(index); }

    /**
     * @return the include base path.
     */
    const std::string &getIncludeDir() const { return includeDir; }

    /**
     * @return the assembled source.
     */
    const std::string &getAssembledSource() const { return assembledSource; }

    /**
     * @return the number of source blocks into the assembled sources.
     */
    int getSourceBlocksCount() const { return assembledSourceBlocks.size(); }

    /**
     * @param index the source block index.
     * @return the source block
     */
    const SourceBlock &getSourceBlock(const int index) const { return assembledSourceBlocks.at(index); }

    /**
     * Sets the module loader. This is mandatory before using {@link #loadModule()}.
     * @param moduleLoader the module loader instance (cannot be nullptr).
     */
    void setModuleLoader(ModuleLoader *moduleLoader) { this->moduleLoader = moduleLoader; }

    /**
     * Sets the include base path, which is used to resolve #include <...> directives
     * @param includeDir the base path (cannot contain a filename)
     */
    void setIncludeDir(const std::string &includeDir);
};
