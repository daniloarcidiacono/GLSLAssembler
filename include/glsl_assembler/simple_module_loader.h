#pragma once
#include <glsl_assembler/conf.h>
#include <glsl_assembler/module_loader.h>
#include <glsl_assembler/string_utils.h>

/**
 * Basic implementation of path utility methods.
 * This class should be further extended to implement the loadModule method.
 */
class SimpleModuleLoader : public ModuleLoader {
public:
    virtual ~SimpleModuleLoader() = default;

    bool isPath(const std::string &pathString) override {
        return true;
    }

    std::string extractPath(const std::string &pathName) override {
        const int ch = pathName.find_last_of('/');
        if (ch != -1) {
            return pathName.substr(0, ch);
        }

        return "";
    }

    std::string join(const std::string &path, const std::string &pathName) override {
        std::vector<std::string> pathSegments = StringUtils::split(path, "/");
        const std::vector<std::string> pathNameSegments = StringUtils::split(pathName, "/");
        for (const std::string &segment : pathNameSegments) {
            if (segment == "" || segment == ".") {
                continue;
            } else if (segment == "..") {
                if (pathSegments.empty()) {
                    throw std::runtime_error("Unknown path");
                }

                pathSegments.pop_back();
            } else {
                pathSegments.push_back(segment);
            }
        }

        return StringUtils::join(pathSegments, "/");
    }
};
