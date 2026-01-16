#include <iostream>
#include <cctype>
#include <fstream>
#include <string>
#include <cstdlib>

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

bool isKeyValue(const std::string& line) {
    return line.find('=') != std::string::npos;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config-file>\n";
        return EXIT_FAILURE;
    }

    const std::string filename{argv[1]};
    std::ifstream file{filename};

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        return EXIT_FAILURE;
    }

    std::string line{};
    int lineNum{0};
    int errorCount{0};
    bool hasErrors{false};

    std::cout << "Validating " << filename << "...\n\n";

    while (std::getline(file, line)) {
        ++lineNum;

        std::string sectionName{};
        std::string error{};

        if (isBlankLine(line) || isComment(line)) {
            continue;
        } else if (isSection(line, sectionName, error)) {
            if (!error.empty()) {
                std::cerr << "Line " << lineNum << ": " << error << "\n";
                ++errorCount;
                hasErrors = true;
            } else {
                std::cout << "Line " << lineNum << ": Valid section [" << sectionName << "]\n";
            }
        } else if (isKeyValue(line)) {
            std::cout << "Line " << lineNum << ": Valid key-value pair\n";
        } else {
            std::cerr << "Line " << lineNum << ": Invalid syntax: " << line << '\n';
            ++errorCount;
            hasErrors = true;
        }
    }

    std::cout << "\n" << (hasErrors ? "Validation Failed" : "Validation Passed") << "\n";
    std::cout << "Total lines: " << lineNum << "\n";
    if (errorCount > 0) {
        std::cout << "Errors found: " << errorCount << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}