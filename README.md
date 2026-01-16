# INI Validator

A command-line tool to validate INI configuration files with syntax checking, required field validation, and type checking.

## Features

- Validates INI file syntax (sections, key-value pairs, comments)
- Checks for invalid characters in section and key names
- Detects duplicate sections and keys
- Ensures key-value pairs are inside sections
- Validates required sections and keys
- Type checking (ports must be numbers)
- Clear, detailed error messages

## Installation

### Via APT (Debian/Ubuntu)
```bash
echo "deb [trusted=yes] https://tmehta0405.github.io/INI-validator stable main" | sudo tee /etc/apt/sources.list.d/ini-validator.list
sudo apt update
sudo apt install ini-validator
```

### Build from Source
```bash
git clone https://github.com/tmehta0405/INI-validator.git
cd INI-validator
g++ -std=c++17 -Wall -Wextra -O2 validator.cpp -o ini-validator
./ini-validator test.ini
```

## Usage
```bash
ini-validator <config-file>
```

Example:
```bash
ini-validator /etc/myapp/config.ini
```

## Valid INI Format
```ini
[database]
host=localhost
port=5432

[server]
port=8080
debug=true
```

## Validation Rules

- Section names must only contain alphanumeric characters, underscore, and dash
- Key names must only contain alphanumeric characters, underscore, and dash
- Keys cannot have spaces
- Values cannot be empty
- Keys must be inside a section
- Required sections: database, server
- Required keys: host and port in database, port in server
- Port values must be numeric

## License

MIT