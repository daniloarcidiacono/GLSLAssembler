#include <glsl_assembler/string_utils.h>
#include <algorithm>
#include <cctype>
#include <locale>
#include <sstream>

namespace StringUtils {
    // https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
    std::string ltrim_copy(std::string s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
        return s;
    }

    std::string rtrim_copy(std::string s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
        return s;
    }

    std::string trim_copy(std::string s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
        return s;
    }

    std::string join(const std::vector<std::string> &list, const std::string &separator) {
        std::string result;
        for (int i = 0; i < list.size(); i++) {
            result += i + 1 < list.size() ? (list[i] + separator) : list[i];
        }

        return result;
    }

    std::vector<std::string> splitLines(const std::string &str) {
        std::vector<std::string> result;
        std::stringstream ss(str);

        std::string line;
        while (std::getline(ss, line)) {
            // Note: std::getline removes the EOL terminator
            result.push_back(line);
        }

        return result;
    }

    // https://stackoverflow.com/questions/13172158/c-split-string-by-line/13172579
    std::vector<std::string> split(const std::string &str, const std::string &delimiter) {
        std::vector<std::string> strings;

        std::string::size_type pos = 0;
        std::string::size_type prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos) {
            strings.push_back(str.substr(prev, pos - prev));
            prev = pos + delimiter.size();
        }

        // To get the last substring (or only, if delimiter is not found)
        strings.push_back(str.substr(prev));

        return strings;
    }

    bool startsWith(const std::string &fullString, const std::string &starting) {
        // https://stackoverflow.com/questions/1878001/how-do-i-check-if-a-c-stdstring-starts-with-a-certain-string-and-convert-a
        return fullString.rfind(starting, 0) == 0;
    }

    bool endsWith(const std::string &fullString, const std::string &ending) {
        // https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }

    // https://stackoverflow.com/questions/5343190/how-do-i-replace-all-instances-of-a-string-with-another-string
    std::string replaceAll(const std::string &fullString, const std::string &from, const std::string &to) {
        if (from.empty()) {
            return fullString;
        }

        std::string wsRet;
        wsRet.reserve(fullString.length());
        size_t start_pos = 0, pos;
        while ((pos = fullString.find(from, start_pos)) != std::string::npos) {
            wsRet += fullString.substr(start_pos, pos - start_pos);
            wsRet += to;
            pos += from.length();
            start_pos = pos;
        }

        wsRet += fullString.substr(start_pos);
        return wsRet;
    }
}
