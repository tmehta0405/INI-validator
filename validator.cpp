#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

bool isBlankLine(const std::string& line) {
    return line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos;
}

bool isComment(const std::string& line) {
    if (line.empty()) return false;
    size_t first = line.find_first_not_of(" \t");
    return first != std::string::npos && (line[first] == ';' || line[first] == '#');
}

bool isSection(const std::string& line) {
    if (line.empty()) return false;
    size_t start = line.find_first_not_of(" \t");
    size_t end = line.find_last_not_of(" \t");
    if (start == std::string::npos) return false;
    return line[start] == '[' && line[end] == ']';
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

        if (isBlankLine(line) || isComment(line)) {
            continue;
        } else if (isSection(line)) {
            std::cout << "Line " << lineNum << ": Valid section\n";
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