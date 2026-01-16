#include <iostream>
#include <cctype>
#include <fstream>
#include <string>
#include <cstdlib>
#include <map>
#include <vector>

struct Config {
    std::map<std::string, std::map<std::string, std::string>> sections;
};

std::string trim(const std::string& str) {
    size_t start{str.find_first_not_of(" \t\r\n")};
    size_t end{str.find_last_not_of(" \t\r\n")};

    if (start == std::string::npos) return "";
    return str.substr(start, end - start + 1);
}

bool isBlankLine(const std::string& line) {
    return line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos;
}

bool isComment(const std::string& line) {
    if (line.empty()) return false;
    size_t first = line.find_first_not_of(" \t");
    return first != std::string::npos && (line[first] == ';' || line[first] == '#');
}

bool isSection(const std::string& line, std::string& sectionName, std::string& error) {
    std::string trimmed{trim(line)};

    if (trimmed.empty() || trimmed[0] != '[') return false;

    if (trimmed.back() != ']') {
        error = "Section missing closing bracket ']'";
        return false;
    }

    sectionName = trimmed.substr(1, trimmed.length() - 2);
    sectionName = trim(sectionName);

    if (sectionName.empty()) {
        error = "Section name cannot be empty";
        return false;
    }

    for (char c : sectionName) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            error = "Section name '" + sectionName + "' contains invalid character '" + std::string(1, c) + "'";
            return false;
        }
    }

    return true;
}

bool isKeyValue(const std::string& line, std::string& key, std::string& value, std::string& error) {
    size_t equalPos{line.find('=')};

    if (equalPos == std::string::npos) return false;

    key = trim(line.substr(0, equalPos));
    value = trim(line.substr(equalPos + 1));

    if (key.empty()) {
        error = "Key cannot be empty before '='";
        return true;
    }

    if (key.find(' ') != std::string::npos) {
        error = "Key '" + key + "' contains spaces. Keys cannot have spaces";
        return true;
    }

    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            error = "Key '" + key + "' contains invalid character '" + std::string(1, c) + "'";
            return true;
        }
    }

    if (value.empty()) {
        error = "Value cannot be empty for key '" + key + "'";
        return true;
    }

    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <config-file>\n";
        return EXIT_FAILURE;
    }

    const std::string filename{argv[1]};
    std::ifstream file{filename};

    if (!file.is_open()) {
        std::cout << "Error: Could not open file '" << filename << "'\n";
        return EXIT_FAILURE;
    }

    Config config{};
    std::string line{};
    std::string currentSection{};
    int lineNum{0};
    int errorCount{0};
    bool hasErrors{false};

    std::cout << "Validating " << filename << "...\n\n";

    while (std::getline(file, line)) {
        ++lineNum;

        std::string sectionName{};
        std::string key{};
        std::string value{};
        std::string error{};

        if (isBlankLine(line) || isComment(line)) {
            continue;
        } else if (isSection(line, sectionName, error)) {
            if (!error.empty()) {
                std::cout << "Line " << lineNum << ": " << error << "\n";
                ++errorCount;
                hasErrors = true;
            } else {
                currentSection = sectionName;

                if (config.sections.find(sectionName) != config.sections.end()) {
                    std::cout << "Line " << lineNum << ": Warning - Section [" << sectionName << "] already exists (will merge)\n";
                } else {
                    config.sections[sectionName] = {};
                }

                std::cout << "Line " << lineNum << ": Valid section [" << sectionName << "]\n";
            }
        } else if (isKeyValue(line, key, value, error)) {
            if (!error.empty()) {
                std::cout << "Line " << lineNum << ": " << error << "\n";
                ++errorCount;
                hasErrors = true;
            } else {
                if (currentSection.empty()) {
                    std::cout << "Line " << lineNum << ": Key-value pair '" << key << "=" << value << "' found outside of any section\n";
                    ++errorCount;
                    hasErrors = true;
                } else {
                    if (config.sections[currentSection].find(key) != config.sections[currentSection].end()) {
                        std::cout << "Line " << lineNum << ": Warning - Key '" << key << "' already exists in [" << currentSection << "] (will overwrite)\n";
                    }

                    config.sections[currentSection][key] = value;
                    std::cout << "Line " << lineNum << ": Valid key-value: " << key << " = " << value << "\n";
                }
            }
        } else {
            std::cout << "Line " << lineNum << ": Invalid syntax: " << line << '\n';
            ++errorCount;
            hasErrors = true;
        }
    }

    std::cout << "\n" << (hasErrors ? "Validation Failed" : "Validation Passed") << "\n";
    std::cout << "Total lines: " << lineNum << "\n";
    std::cout << "Sections found: " << config.sections.size() << "\n";

    if (!hasErrors) {
        std::cout << "\nParsed Configuration:\n";
        for (const auto& [section, keys] : config.sections) {
            std::cout << "[" << section << "]\n";
            for (const auto& [key, val] : keys) {
                std::cout << "  " << key << " = " << val << "\n";
            }
        }
    }

    if (errorCount > 0) {
        std::cout << "Errors found: " << errorCount << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}