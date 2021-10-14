#include <glsl_assembler/module.h>
#include <glsl_assembler/string_utils.h>
#include <regex>

Module::Module() {
}

Module *Module::fromSource(const std::string &id, const std::string &source) {
    Module *module = new Module();
    module->sourceLines = StringUtils::splitLines(source);
    module->id = id;
    module->analyzeDependencies();

    return module;
}

void Module::commentLine(int index) {
    sourceLines[index] = "// " + sourceLines[index];
}

void Module::hoistLine(int index) {
    HoistedLine hoistedLine;
    hoistedLine.line = sourceLines[index];
    hoistedLine.index = index;
    hoistLines.push_back(hoistedLine);

    commentLine(index);
}

void Module::analyzeDependencies() {
    dependencies.clear();

    const std::regex relativeRegex(R"(^#include \"((\\.|[^\"])*)\"$)");
    const std::regex absoluteRegex(R"(^#include <((\\.|[^\"])*)>$)");
    for (int i = 0; i < sourceLines.size(); i++) {
        const std::string line = StringUtils::trim_copy(sourceLines[i]);

        // Skip commented lines (block comments are not supported)
        if (StringUtils::startsWith(line, "//")) {
            continue;
        }

        // Handle includes
        auto relativeIncludesBegin = std::sregex_iterator(line.begin(), line.end(), relativeRegex);
        auto relativeIncludesEnd = std::sregex_iterator();
        if (relativeIncludesBegin != relativeIncludesEnd) {
            for (std::sregex_iterator j = relativeIncludesBegin; j != relativeIncludesEnd; j++) {
                const std::string relativeInclude = j->str(1);
                if (!relativeInclude.empty()) {
                    dependencies.emplace_back(Dependency::relative(relativeInclude, i));
                    commentLine(i);
                    break;
                } else {
                    throw std::runtime_error("Error '" + id + "'(" + std::to_string(i + 1) + "): invalid #include syntax.");
                }
            }
        } else {
            auto absoluteIncludesBegin = std::sregex_iterator(line.begin(), line.end(), absoluteRegex);
            auto absoluteIncludesEnd = std::sregex_iterator();
            if (absoluteIncludesBegin != absoluteIncludesEnd) {
                for (std::sregex_iterator j = absoluteIncludesBegin; j != absoluteIncludesEnd; j++) {
                    const std::string absoluteInclude = j->str(1);
                    if (!absoluteInclude.empty()) {
                        dependencies.emplace_back(Dependency::absolute(absoluteInclude, i));
                        commentLine(i);
                        break;
                    } else {
                        throw std::runtime_error("Error '" + id + "'(" + std::to_string(i + 1) + "): invalid #include syntax.");
                    }
                }
            }
        }

        // Handle hoisting
        if (StringUtils::startsWith(line, "#version") || StringUtils::startsWith(line, "precision")) {
            hoistLine(i);
        }
    }
}

void Module::inject(std::vector<std::string> &lines) const {
    lines.push_back("// MODULE BEGIN: " + id);
    lines.insert(lines.end(), sourceLines.begin(), sourceLines.end());
    lines.emplace_back("");
}
