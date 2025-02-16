# ai2fs

Command-line tool that converts AI-generated code into a file system structure. Automatically creates directories and files from code blocks marked with file paths, making it easy to implement code suggestions from any AI provider.

## Features

### Smart Path Detection
- Supports 11 different path marker styles:
  ```
  // src/app.js              (C-style comments)
  # lib/helpers.py           (Shell/Python comments)
  --> components/Button.tsx  (Arrow notation)
  -> styles/main.css        (Short arrow)
  => utils/helpers.js       (Alternative arrow)
  > docs/README.md          (Markdown-style)
  [ config/settings.json ]  (Bracket notation)
  - schema/types.graphql    (Dash/hyphen)
  *** config/.env           (Asterisk separator)
  --- docker-compose.yml    (Dash separator)
  ## tests/test.spec.ts     (Double hash)
  ```

### Intelligent Structure Handling
- Creates full directory structures automatically
- Supports files in root level and nested directories
- Creates all parent directories as needed
- Places all generated files in an isolated `generated-code` folder
- Preserves original directory structure

### Preview Detection
- Automatically skips common directory previews:
  ```
  src/
  ├── components/
  │   └── Button.tsx
  └── utils/
      └── helpers.js
  ```
- Ignores tree-view lines containing:
  - ├── (T-shaped branch)
  - └── (L-shaped branch)
  - │   (Vertical line)
  - |-- (ASCII-style branch)

### Content Management
- Preserves exact file content formatting
- Maintains proper whitespace and indentation
- Supports any file extension
- Handles large files through dynamic buffer allocation
- Protects against memory issues with proper allocation checks

### Error Handling
- Provides clear error messages for:
  - File creation issues
  - Permission problems
  - Memory allocation failures
  - Invalid input
- Safe directory creation with proper permissions
- Memory leak prevention through proper cleanup

### Cross-Platform Support
- Works on both Unix and Windows systems
- Handles both forward slashes (/) and backslashes (\\)
- Creates directories with appropriate system permissions
- Uses platform-specific directory creation commands

## Installation

### Prerequisites
- GCC or any C compiler
- Make (optional, for easier building)

### Building from source
```bash
# Clone the repository
git clone https://github.com/yourusername/ai2fs.git
cd ai2fs

# Compile
gcc -o ai2fs ai2fs.c

# Optional: Install system-wide
sudo cp ai2fs /usr/local/bin/
```

## Usage

Basic usage:
```bash
ai2fs <input_file>
```

### Input File Format

Your input file can contain any combination of supported path markers followed by the file content. Here's an example:

```
// First component
// src/components/Button.tsx
import React from 'react';

export const Button = () => {
  return <button>Click me</button>;
};

# Python utility
# utils/helpers.py
def get_config():
    return {
        "debug": True
    }

[ Configuration file ]
[ config/settings.json ]
{
  "name": "my-app",
  "version": "1.0.0"
}
```

### Output Structure

The program creates all files under a `generated-code` directory:
```
generated-code/
├── src/
│   └── components/
│       └── Button.tsx
├── utils/
│   └── helpers.py
└── config/
    └── settings.json
```

## Examples

### Basic File Creation
```
// config.json
{
  "name": "my-app"
}
```

### Nested Directories
```
# src/utils/helpers.py
def helper():
    pass
```

### Multiple Styles
```
// Component file
// src/components/Button.tsx
import React from 'react';

# Python file
# utils/helpers.py
def helper():
    pass

[ Config file ]
[ config.json ]
{
  "name": "app"
}
```

## Contributing

Contributions are welcome! Here are some ways you can contribute:

1. Report bugs
2. Suggest new features
3. Add support for new path markers
4. Improve documentation
5. Add more test cases

## Development

### Adding New Path Markers
Path markers are defined in the `PATH_MARKERS` array. To add a new marker:

1. Add it to the array in `ai2fs.c`:
```c
static const char *PATH_MARKERS[] = {
    "// ", "#  ", "-->", ...
    "new-marker",  // Add your new marker here
    NULL
};
```

2. Test it with various inputs to ensure proper handling

### Testing
Create a test file with various path markers and run:
```bash
./ai2fs test-input.txt
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

If you encounter any problems or have questions:

1. Check the [Issues](https://github.com/yourusername/ai2fs/issues) page
2. Create a new issue if your problem isn't already listed
3. Include sample input and expected output in bug reports