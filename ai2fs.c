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
 * - // (C-style comments)     Example: // path/to/file.js
 * - #  (Shell/Python comments)  Example: # path/to/file.py
 * - --> or -> (Arrow notation)  Example: --> path/to/file.tsx
 * - >  (Markdown-style)      Example: > path/to/file.md
 * - => (Alternative arrow)    Example: => path/to/file.html
 * - [  (Bracket notation)    Example: [ path/to/file.css ]
 * - -  (Dash/hyphen)       Example: - path/to/file.json
 * - *** or --- (MD separators)   Example: *** path/to/file.yml
 * - ## (Alternative hash)    Example: ## path/to/file.config
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
 * └── [maintains original path structure]
 * 
 * Example input format:
 * // path/to/file.ext
 * Content for file.ext
 * More content...
 * 
 * # another/path/file.txt
 * Content for another file
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

#define PROGRAM_NAME "ai2fs"
#define ROOT_FOLDER "generated-code"
#define MAX_LINE_LENGTH 4096
#define MAX_PATH_LENGTH 256

// Function to trim whitespace from both ends of a string
void trim(char *str) {
  char *end;
  
  // Trim leading space
  while(isspace((unsigned char)*str)) str++;
  
  if(*str == 0) return;
  
  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;
  
  end[1] = '\0';
}

// Define supported path markers
#define MAX_MARKER_LENGTH 4
static const char *PATH_MARKERS[] = {
  "//", "#", "-->", "->", ">", "=>", "[", "-", "***", "---", "##",
  NULL  // Terminator
};

// Function to check if a line starts with a supported marker and contains a valid path
int is_path_line(const char *line) {
  char trimmed[MAX_LINE_LENGTH];
  strncpy(trimmed, line, MAX_LINE_LENGTH - 1);
  trimmed[MAX_LINE_LENGTH - 1] = '\0';
  trim(trimmed);
  
  // Empty lines are not path lines
  if (trimmed[0] == '\0') return 0;
  
  // Check for any of our supported markers
  const char **marker = PATH_MARKERS;
  const char *path_start = NULL;
  
  while (*marker != NULL) {
    if (strncmp(trimmed, *marker, strlen(*marker)) == 0) {
      path_start = trimmed + strlen(*marker);
      break;
    }
    marker++;
  }
  
  // If no marker found, not a path line
  if (path_start == NULL) return 0;
  
  // Skip whitespace after marker
  while(isspace((unsigned char)*path_start)) path_start++;
  
  // Must have content after marker
  if (*path_start == '\0') return 0;
  
  // Must contain at least one / in the path
  if (strchr(path_start, '/') == NULL) return 0;
  
  // Must contain a file extension (any characters after a dot)
  const char *last_dot = strrchr(path_start, '.');
  if (last_dot != NULL && *(last_dot + 1) != '\0') {
    // Check that there are characters after the dot
    const char *ext = last_dot + 1;
    while (*ext != '\0') {
      if (!isspace((unsigned char)*ext)) {
        return 1;  // Found at least one non-space character after the dot
      }
      ext++;
    }
  }
  
  return 0;
}

// Function to extract the path from a marker line
void extract_path(const char *line, char *path) {
  char trimmed[MAX_LINE_LENGTH];
  strncpy(trimmed, line, MAX_LINE_LENGTH - 1);
  trimmed[MAX_LINE_LENGTH - 1] = '\0';
  trim(trimmed);
  
  // Find which marker was used
  const char **marker = PATH_MARKERS;
  const char *path_start = NULL;
  
  while (*marker != NULL) {
    if (strncmp(trimmed, *marker, strlen(*marker)) == 0) {
      path_start = trimmed + strlen(*marker);
      break;
    }
    marker++;
  }
  
  if (path_start == NULL) {
    path[0] = '\0';
    return;  // Should never happen as we checked in is_path_line
  }
  
  // Skip whitespace after marker
  while(isspace((unsigned char)*path_start)) path_start++;
  
  // Copy the path
  strncpy(path, path_start, MAX_PATH_LENGTH - 1);
  path[MAX_PATH_LENGTH - 1] = '\0';
  trim(path);
}

// Function to create directory structure for a file path
void create_directories(const char *path) {
  char temp[MAX_PATH_LENGTH];
  char *p = NULL;
  size_t len;
  
  // First create the root folder
  #ifdef _WIN32
    _mkdir(ROOT_FOLDER);
  #else
    mkdir(ROOT_FOLDER, S_IRWXU);
  #endif
  
  // Prepend root folder to path
  snprintf(temp, sizeof(temp), "%s/%s", ROOT_FOLDER, path);
  
  len = strlen(temp);
  if (temp[len - 1] == '/') {
    temp[len - 1] = 0;
  }
  
  for (p = temp + 1; *p; p++) {
    if (*p == '/') {
      *p = 0;
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
  
  printf("Root folder '%s' created.\n", ROOT_FOLDER);
  
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
  
  while (fgets(line, sizeof(line), input_file)) {
    // Check if this line is a path marker
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
      
      // Open new file with root folder prepended
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