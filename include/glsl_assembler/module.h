#pragma once
#include <glsl_assembler/conf.h>
#include <string>
#include <vector>

/**
 * A module represents a single GLSL file. Modules are linked to each other through one ore more {@link Dependency}.
 */
class GLSLASSEMBLER_API Module {
public:
    /**
     * Mark used for topological sort algorithm.
     */
    enum class Mark : int {
        /**
         * Unmarked marker. Every node has this mark when starting the topological sort.
         */
        UNMARKED,

        /**
         * Temporary marker. If a node has already this mark, a cycle has been found.
         */
        TEMPORARY,

        /**
         * Permanent marker. A node which has been fully explored ends up with this mark.
         */
        PERMANENT
    };

    /**
     * A dependency to another module.
     */
    struct GLSLASSEMBLER_API Dependency {
        enum class Type {
            /**
             * The dependency's moduleId is relative to the includeDir of {@link ModuleGraph}.
             */
            ABSOLUTE,

            /**
             * The dependency's moduleId is relative to the moduleId
             */
            RELATIVE
        };

        /**
         * Reference to the module. This field is populated by {@link ModuleGraph}.
         */
        Module *module = nullptr;

        /**
         * The unique identifier of the module, i.e. its full pathname.
         */
        std::string moduleId;

        /**
         * Line (zero-based) in the module source which contains the include directive.
         */
        int includeLine = -1;

        /**
         * Type of dependency. Include directives with angular brackets are ABSOLUTE, while quotes are RELATIVE.
         */
        Type type;

        Dependency() {
        }

        /**
         * Constructs an absolute Dependency.
         * @param moduleId the id of the module
         * @param includeLine the include line
         * @return the built Dependency object
         */
        static Dependency absolute(const std::string &moduleId, const int includeLine) {
            Dependency dependency;
            dependency.moduleId = moduleId;
            dependency.includeLine = includeLine;
            dependency.type = Type::ABSOLUTE;

            return dependency;
        }

        /**
         * Constructs a relative Dependency.
         * @param moduleId the id of the module
         * @param includeLine the include line
         * @return the built Dependency object
         */
        static Dependency relative(const std::string &moduleId, const int includeLine) {
            Dependency dependency;
            dependency.moduleId = moduleId;
            dependency.includeLine = includeLine;
            dependency.type = Type::RELATIVE;

            return dependency;
        }
    };

    /**
     * Some GLSL directives (like version and precision) must appear at the start of the file.
     * These lines are "hoisted" during the import process.
     */
    struct GLSLASSEMBLER_API HoistedLine {
        /**
         * Original line (hoisted lines become commented)
         */
        std::string line;

        /**
         * Line index (zero-based)
         */
        int index;
    };

private:
    /**
     * The unique identifier of the module, i.e. its full pathname.
     */
    std::string id;

    /**
     * Lines of the module that need to be hoisted.
     */
    std::vector<HoistedLine> hoistLines;

    /**
     * Source lines of the module.
     */
    std::vector<std::string> sourceLines;

    /**
     * Dependencies of the module, initially populated by {@link #analyzeDependencies()}, and further refined
     * by {@link ModuleGraph}.
     */
    std::vector<Dependency> dependencies;

    /**
     * Mark used for topological sort.
     */
    Mark mark = Mark::UNMARKED;

    /**
     * Populates the dependencies vector (without instancing other Modules)
     */
    void analyzeDependencies();

    /**
     * A module must be instantiated through {@link #fromSource()} method.
     */
    Module();

    /**
     * Comments a module line.
     * Includes and hoisted lines are commented instead of removed, to simplify the mapping between the
     * assembled source and the individual modules.
     * @param index the line index to comment (zero-based)
     */
    void commentLine(int index);

    /**
     * Some GLSL directives must appear at the top of the file, so they must be "hoisted".
     * Hoisted lines are also commented.
     * @param index the line index to comment (zero-based)
     */
    void hoistLine(int index);

public:
    /**
     * Creates a module with the given id and source.
     * Include directives and hoist directives are processed.
     * @param id the unique id of the module
     * @param source the source code of the module
     * @return the built Module instance
     */
    static Module *fromSource(const std::string &id, const std::string &source);

    /**
     * Checks whether the module has no source lines
     * @return true if the modules has no source lines.
     */
    bool isEmpty() const {
        return sourceLines.empty();
    }

    /**
     * Injects the module's source code into a vector. A comment preamble followed by the module source followed
     * by an empty line is injected.
     * @param lines the vector where to inject the module.
     */
    void inject(std::vector<std::string> &lines) const;

    /**
     * Begin iterator over the dependencies.
     * @return the iterator begin
     */
    std::vector<Dependency>::iterator begin() { return dependencies.begin(); }

    /**
     * End iterator over the dependencies.
     * @return the iterator begin
     */
    std::vector<Dependency>::iterator end() { return dependencies.end(); }

    /**
     * Sets a mark for the node. Used by {@link ModuleGraph} for topological sorting.
     * @param mark the mark to set
     */
    void setMark(Mark mark) { this->mark = mark; }

    /**
     * @return the module id, i.e. its full path.
     */
    const std::string &getId() const { return id; }

    /**
     * @param index the index of hoisted line
     * @return the index-th hoisted line
     */
    const HoistedLine &getHoistedLine(int index) const { return hoistLines.at(index); }

    /**
     * @return the number of hoisted lines.
     */
    int getHoistedLinesCount() const { return hoistLines.size(); }

    /**
     * @return the number of dependencies of the module.
     */
    int getDependencyCount() const { return dependencies.size(); }

    /**
     * @param index the index of the dependency
     * @return the index-th dependency
     */
    Dependency &getDependency(const int index) { return dependencies.at(index); }

    /**
     * @param index the index of the dependency
     * @return the index-th dependency
     */
    const Dependency &getDependency(const int index) const { return dependencies.at(index); }

    /**
     * @return the number of source lines
     */
    int getSourceLinesCount() const { return sourceLines.size(); }

    /**
     * @param index the source line (zero-based)
     * @return the index-th source line
     */
    const std::string &getSourceLine(const int index) const { return sourceLines.at(index); }

    /**
     * @return the node mark (used by {@link ModuleGraph})
     */
    Mark getMark() const { return mark; }
};
