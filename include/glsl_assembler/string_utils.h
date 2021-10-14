#pragma once
#include <glsl_assembler/conf.h>
#include <string>
#include <vector>

/**
 * String utilities.
 */
namespace StringUtils {
    /**
     * Returns a copy of a string without whitespace at the start.
     * @param s the string to left-trim
     * @return the left-trimmed string
     */
    GLSLASSEMBLER_API std::string ltrim_copy(std::string s);

    /**
     * Returns a copy of a string without whitespace at the end.
     * @param s the string to right-trim
     * @return the right-trimmed string
     */
    GLSLASSEMBLER_API std::string rtrim_copy(std::string s);

    /**
     * Returns a copy of a string without whitespace at the start nor at the end.
     * @param s the string to trim
     * @return the trimmed string
     */
    GLSLASSEMBLER_API std::string trim_copy(std::string s);

    /**
     * Joints a list of strings with a specific separator.
     * @param list the strings to join
     * @param separator the separator to use between each string
     * @return the joined string
     */
    GLSLASSEMBLER_API std::string join(const std::vector<std::string> &list, const std::string &separator = "");

    /**
     * Splits the lines of a string
     * @param str the string to split into lines.
     * @return the list of string lines.
     */
    GLSLASSEMBLER_API std::vector<std::string> splitLines(const std::string &str);

    /**
     * Splits a string according to a delimiter.
     * @param str the string to split
     * @param delimiter the string to use as a delimiter
     * @return the string parts
     */
    GLSLASSEMBLER_API std::vector<std::string> split(const std::string &str, const std::string &delimiter);

    /**
     * @param fullString the string to check for
     * @param starting the wanted starting string
     * @return true if fullString starts with the specified string, false otherwise
     */
    GLSLASSEMBLER_API bool startsWith(const std::string &fullString, const std::string &starting);

    /**
     * @param fullString the string to check for
     * @param ending the wanted ending string
     * @return true if fullString ends with the specified string, false otherwise
     */
    GLSLASSEMBLER_API bool endsWith(const std::string &fullString, const std::string &ending);

    /**
     * Replaces all the instances of a substring with another.
     * @param fullString the whole string
     * @param from the substring to replace
     * @param to the substring to insert
     * @return the whole string with the replacement done.
     */
    GLSLASSEMBLER_API std::string replaceAll(const std::string &fullString, const std::string &from, const std::string &to);
}
