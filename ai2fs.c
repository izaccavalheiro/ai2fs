/*
 * ai2fs - AI output to filesystem parser
 * Version: 1.0.0
 * 
 * Description:
 * A command-line utility that processes text output from AI assistants and generates
 * a file system structure based on path markers in the text. The program recognizes
 * various marker styles commonly used in programming and documentation to indicate
 * file paths, and creates the corresponding files and directory structure.
 * 
 * Supported path markers:
 * - // (C-style comments)   Examples: // main.js, // src/app.js
 * - #  (Shell/Python comments)  Examples: # config.py, # lib/utils.py
 * - --> or -> (Arrow notation)  Examples: --> index.tsx, --> components/Button.tsx
 * - >  (Markdown-style)  Examples: > README.md, > docs/api.md
 * - => (Alternative arrow)  Examples: => style.css, => css/main.css
 * - [  (Bracket notation)  Examples: [ types.d.ts ], [ src/interfaces.ts ]
 * - -  (Dash/hyphen)   Examples: - package.json, - config/dev.json
 * - *** or --- (MD separators)   Examples: *** .env, *** config/.env.local
 * - ## (Alternative hash)  Examples: ## tsconfig.json, ## src/routes.ts
 * 
 * Path Structure:
 * - Can create files in root directory (e.g., main.js, README.md)
 * - Can create files in subdirectories (e.g., src/app.js, lib/utils/helper.js)
 * - All files must have an extension
 * 
 * Features:
 * - Flexible path marker recognition
 * - Creates complete directory structures
 * - Supports any file extension
 * - Places all generated files in a 'generated-code' root folder
 * - Preserves file content formatting and whitespace
 * - Handles both Unix and Windows-style paths
 * - No external dependencies
 * 
 * Usage: ai2fs <input_file>
 * 
 * Output Structure:
 * generated-code/
 * ├── [files in root]
 * └── [subdirectory structure]
 * 
 * Example input format:
 * // main.js
 * console.log('Root file');
 * 
 * # src/utils.py
 * def helper():
 *   return True
 * 
 * Author: Izac Cavalheiro
 * License: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>

/* Constants */
#define PROGRAM_NAME "ai2fs"
#define ROOT_FOLDER "generated-code"
#define MAX_LINE_LENGTH 4096
#define MAX_PATH_LENGTH 256

/* Global variables */
static const char *PATH_MARKERS[] = {
  "// ", "#  ", "-->", "->", "=> ", "> ", "[ ", "- ", "***", "---", "## ",
  NULL
};

/* Function declarations */
void trim(char *str);
int is_path_line(const char *line);
void extract_path(const char *line, char *path);
void create_directories(const char *path);

/* Function implementations */
void trim(char *str) {
  if (!str) return;
  
  char *end;
  
  // Trim leading space
  while (isspace((unsigned char)*str)) {
    str++;
  }
  
  if (*str == 0) return;
  
  // Trim trailing space
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end)) {
    end--;
  }
  
  end[1] = '\0';
}

int is_path_line(const char *line) {
  if (!line) return 0;
  
  char trimmed[MAX_LINE_LENGTH];
  strncpy(trimmed, line, MAX_LINE_LENGTH - 1);
  trimmed[MAX_LINE_LENGTH - 1] = '\0';
  trim(trimmed);
  
  // Must not be empty
  if (trimmed[0] == '\0') return 0;

  // Skip directory structure lines
  if (strstr(trimmed, "├") || 
    strstr(trimmed, "└") || 
    strstr(trimmed, "│") ||
    strstr(trimmed, "|--")) {
    return 0;
  }

  // Check if line starts with any of our markers
  const char **marker;
  const char *path_start = NULL;
  
  for (marker = PATH_MARKERS; *marker != NULL; marker++) {
    size_t marker_len = strlen(*marker);
    if (strncmp(trimmed, *marker, marker_len - 1) == 0) {
      path_start = trimmed + (strchr(*marker, ' ') ? marker_len - 1 : marker_len);
      break;
    }
  }
  
  if (!path_start) return 0;

  // Skip any remaining spaces after marker
  while (isspace((unsigned char)*path_start)) {
    path_start++;
  }
  
  // Must have content after marker
  if (*path_start == '\0') return 0;
  
  // Must have a file extension
  const char *last_dot = strrchr(path_start, '.');
  return (last_dot && *(last_dot + 1) != '\0');
}

void extract_path(const char *line, char *path) {
  if (!line || !path) return;
  
  char trimmed[MAX_LINE_LENGTH];
  strncpy(trimmed, line, MAX_LINE_LENGTH - 1);
  trimmed[MAX_LINE_LENGTH - 1] = '\0';
  trim(trimmed);
  
  // Find which marker was used
  const char **marker;
  const char *path_start = NULL;
  
  for (marker = PATH_MARKERS; *marker != NULL; marker++) {
    size_t marker_len = strlen(*marker);
    if (strncmp(trimmed, *marker, marker_len - 1) == 0) {
      path_start = trimmed + (strchr(*marker, ' ') ? marker_len - 1 : marker_len);
      break;
    }
  }
  
  if (!path_start) {
    path[0] = '\0';
    return;
  }
  
  // Skip any remaining spaces
  while (isspace((unsigned char)*path_start)) {
    path_start++;
  }
  
  // Copy the path
  strncpy(path, path_start, MAX_PATH_LENGTH - 1);
  path[MAX_PATH_LENGTH - 1] = '\0';
  trim(path);
  
  // Remove closing bracket if present
  size_t len = strlen(path);
  if (len > 0 && path[len - 1] == ']') {
    path[len - 1] = '\0';
    trim(path);
  }
}

void create_directories(const char *path) {
  if (!path) return;
  
  char temp[MAX_PATH_LENGTH];
  char *p;
  
  // First create the root folder
  #ifdef _WIN32
    _mkdir(ROOT_FOLDER);
  #else
    mkdir(ROOT_FOLDER, S_IRWXU);
  #endif
  
  // Prepend root folder to path
  snprintf(temp, sizeof(temp), "%s/%s", ROOT_FOLDER, path);
  
  for (p = temp + 1; *p; p++) {
    if (*p == '/') {
      *p = '\0';
      #ifdef _WIN32
        _mkdir(temp);
      #else
        mkdir(temp, S_IRWXU);
      #endif
      *p = '/';
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input_file>\n", PROGRAM_NAME);
    return 1;
  }
  
  FILE *input_file = fopen(argv[1], "r");
  if (!input_file) {
    perror("Error opening input file");
    return 1;
  }
  
  char line[MAX_LINE_LENGTH];
  char current_path[MAX_PATH_LENGTH] = {0};
  FILE *output_file = NULL;
  char *content_buffer = NULL;
  size_t content_size = 0;
  size_t content_capacity = 0;
  
  printf("Root folder '%s' created.\n", ROOT_FOLDER);
  
  while (fgets(line, sizeof(line), input_file)) {
    if (is_path_line(line)) {
      // If we were collecting content, save it
      if (output_file) {
        if (content_buffer) {
          fputs(content_buffer, output_file);
          free(content_buffer);
          content_buffer = NULL;
        }
        fclose(output_file);
        printf("Created file: %s\n", current_path);
      }
      
      // Extract and process new path
      extract_path(line, current_path);
      create_directories(current_path);
      
      // Open new file
      char full_path[MAX_PATH_LENGTH];
      snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_FOLDER, current_path);
      output_file = fopen(full_path, "w");
      if (!output_file) {
        fprintf(stderr, "Error creating file %s: %s\n", 
            full_path, strerror(errno));
        continue;
      }
      
      // Reset content buffer
      content_size = 0;
      content_capacity = MAX_LINE_LENGTH;
      content_buffer = malloc(content_capacity);
      if (!content_buffer) {
        perror("Memory allocation failed");
        fclose(output_file);
        return 1;
      }
      content_buffer[0] = '\0';
    } else if (output_file) {
      // Append to content buffer
      size_t line_len = strlen(line);
      while (content_size + line_len + 1 > content_capacity) {
        content_capacity *= 2;
        char *new_buffer = realloc(content_buffer, content_capacity);
        if (!new_buffer) {
          perror("Memory reallocation failed");
          free(content_buffer);
          fclose(output_file);
          return 1;
        }
        content_buffer = new_buffer;
      }
      strcat(content_buffer, line);
      content_size += line_len;
    }
  }
  
  // Save last file if we were collecting content
  if (output_file && content_buffer) {
    fputs(content_buffer, output_file);
    free(content_buffer);
    fclose(output_file);
    printf("Created file: %s\n", current_path);
  }
  
  fclose(input_file);
  return 0;
}
