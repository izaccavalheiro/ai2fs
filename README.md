# ai2fs

Command-line tool that converts AI-generated code into a file system structure. Automatically creates directories and files from code blocks marked with file paths, making it easy to implement code suggestions from any AI provider.

## Features

### Path Marker Recognition
- Supports multiple marker styles:
  ```
  // path/to/file.js          (C-style comments)
  # path/to/file.py           (Shell/Python comments)
  --> path/to/file.tsx        (Arrow notation)
  -> path/to/file.html        (Short arrow)
  > path/to/file.md          (Markdown-style)
  => path/to/file.css        (Alternative arrow)
  [ path/to/file.json ]      (Bracket notation)
  - path/to/file.yml         (Dash/hyphen)
  *** path/to/file.config    (Markdown separator)
  ## path/to/file.ts         (Alternative hash)
  ```

### Smart Directory Structure Handling
- Creates files in both root and nested directories:
  ```
  // config.json              (Root file)
  // src/app.js              (Nested file)
  // src/components/Button.tsx (Deeply nested file)
  ```
- Automatically creates all necessary parent directories
- Places all generated files in an isolated `generated-code` folder
- Preserves original directory structure

### Intelligent Preview Detection
- Automatically skips directory structure previews:
  ```
  Project structure:
  ├── src/
  │   ├── components/
  │   └── utils/
  └── public/
  ```
- Ignores tree-view diagrams and file listings
- Detects comment blocks and documentation sections
- Automatically resumes file creation after preview sections

### File Content Handling
- Preserves exact content formatting and whitespace
- Supports any file extension
- Handles both Unix and Windows-style paths
- Maintains file content exactly as provided

### Implementation Details
- No external dependencies
- Lightweight and fast
- Cross-platform compatible
- Simple command-line interface

## Installation

### Prerequisites

- GCC or any C compiler
- Make (optional, for easier building)

### Building from source

1. Clone the repository:
```bash
git clone https://github.com/izaccavalheiro/ai2fs.git
cd ai2fs
```

2. Compile the program:
```bash
gcc -o ai2fs ai2fs.c
```

3. (Optional) Install system-wide:
```bash
sudo cp ai2fs /usr/local/bin/
```

## Usage

Basic usage:
```bash
ai2fs <input_file>
```

### Example Input File

```
// Project structure preview (automatically skipped)
src/
  ├── components/
  └── utils/

// First we'll create our configuration
// config.json
{
  "name": "my-app",
  "version": "1.0.0"
}

// Now let's create the main component
// src/components/Button.tsx
import React from 'react';

export const Button = () => {
  return <button>Click me</button>;
};

# Create a utility function
# src/utils/helpers.ts
export const formatDate = (date: Date) => {
  return date.toLocaleDateString();
};
```

### Generated Output

```
generated-code/
├── config.json
└── src/
    ├── components/
    │   └── Button.tsx
    └── utils/
        └── helpers.ts
```

## Advanced Features

### Comment Block Handling
The program intelligently handles various comment block styles:
```
/*
 * Project explanation
 */

"""
Python-style documentation
"""

<!--
HTML/XML comments
-->
```

### Directory Listing Detection
Automatically skips common directory listing formats:
```
drwxr-xr-x  src/
-rw-r--r--  package.json

Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
d-----         2/16/2024   3:24 PM                src
```

## Contributing

Contributions are welcome! Here are some ways you can contribute:

1. Report bugs
2. Suggest new features
3. Submit pull requests
4. Improve documentation

Please feel free to create issues for bugs or feature requests.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

If you encounter any problems or have questions:

1. Check the [Issues](https://github.com/izaccavalheiro/ai2fs/issues) page
2. Create a new issue if your problem isn't already listed
3. Include sample input and expected output in bug reports
