#pragma once
#include <glsl_assembler/conf.h>
#include <string>
#include <vector>

/**
 * ModuleLoader is used by {@link ModuleGraph} to resolve paths and to actually load modules.
 * <p>Different implementations can load, for example, from disk or from memory.
 * <p>A basic (partial) implementation is provided by {@link SimpleModuleLoader}.
 */
class ModuleLoader {
public:
    ModuleLoader() = default;
    virtual ~ModuleLoader() = default;

    /**
     * Loads the GLSL file located at path.
     * Note: the returned string <strong>MUST</strong> use \\n as newline character.
     *
     * @param path the path of the resource to load.
     * @return the GLSL file contents.
     * @throws std::runtime_error if the resource cannot be loaded.
     */
    virtual std::string load(const std::string &path) = 0;

    /**
     * @param pathString the path string to check
     * @return true if the string is only a path, without filename
     */
    virtual bool isPath(const std::string &pathString) = 0;

    /**
     * Given a path name (path + filename), extracts the path part
     * @param pathName a path along with a filename (e.g. /my/path/file.glsl)
     * @return the path part
     */
    virtual std::string extractPath(const std::string &pathName) = 0;

    /**
     * Joins a path with a path name, which can be relative.
     * <p>A relative path can use ".." or ".", for example
     * <ul>
     * <li>../my/folder/file.glsl</li>
     * <li>../.././myfile.glsl</li>
     * </ul>
     * @param path the base path (never includes a filename)
     * @param pathName a path name, which can include special ".." and "." symbols
     * @return the joined path
     */
    virtual std::string join(const std::string &path, const std::string &pathName) = 0;
};
