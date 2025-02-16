# ai2fs

Command-line tool that converts AI-generated code into a file system structure. Automatically creates directories and files from code blocks marked with file paths, making it easy to implement code suggestions from any AI provider.

## Features

- Creates complete directory structures from path markers in text
- Places all generated files in an isolated `generated-code` directory
- Supports any file extension
- Preserves file content formatting and whitespace
- Handles both Unix and Windows-style paths
- Simple and lightweight with no external dependencies

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

Example:
```bash
ai2fs paste.txt
```

This will:
1. Create a `generated-code` directory in the current folder
2. Parse the input file for path markers
3. Create all necessary subdirectories
4. Generate files with their corresponding content

### Input Format

The input file should contain path markers followed by the file path. Everything between two path markers becomes the content of the file. Supported markers include:

- `//` (C-style comments)
- `#` (Shell, Python, Ruby comments)
- `-->` or `->` (Arrow notation)
- `>` (Markdown-style)
- `=>` (Alternative arrow)
- `[` (Bracket notation)
- `-` (Dash/hyphen)
- `***` or `---` (Markdown separators)
- `##` (Alternative hash)

Examples of valid path markers:

```
// app/src/main.js
console.log('Hello World');

# app/src/utils.js
function helper() {
    return true;
}

--> app/styles/main.css
body {
    margin: 0;
    padding: 0;
}

[ app/components/Button.tsx ]
const Button = () => {
    return <button>Click me</button>;
}
```

### Output Structure

Given the above input, ai2fs will create:

```
generated-code/
├── app/
│   ├── src/
│   │   ├── main.js
│   │   └── utils.js
│   └── styles/
│       └── main.css
```

## Examples

### Basic Example
Input file `example.txt`:
```
// project/src/index.js
const express = require('express');
const app = express();

// project/src/routes/users.js
router.get('/users', (req, res) => {
    res.send('Users route');
});
```

Run the command:
```bash
ai2fs example.txt
```

This creates:
```
generated-code/
└── project/
    └── src/
        ├── index.js
        └── routes/
            └── users.js
```

### Working with AI Providers

1. Copy the code blocks from your AI chat/completion
2. Paste them into a text file
3. Run ai2fs on that file
4. Find your generated code in the `generated-code` directory

## Contributing

Contributions are welcome! Here are some ways you can contribute:

1. Report bugs
2. Suggest new features
3. Submit pull requests
4. Improve documentation

Please feel free to create issues for bugs or feature requests.

### Development

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by the need to efficiently implement AI-generated code structures
- Thanks to all contributors and users of the project

## Support

If you encounter any problems or have questions:

1. Check the [Issues](https://github.com/izaccavalheiro/ai2fs/issues) page
2. Create a new issue if your problem isn't already listed
3. Include sample input and expected output in bug reports
