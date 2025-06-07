
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#ifdef _WIN32
#include <windows.h>
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

#define MAX_PATH 4096
#define MAX_FILES 1000
#define MAX_LIBS 100

typedef struct {
  char path[MAX_PATH];
  time_t mtime;
} SourceFile;

typedef struct {
  bool is_cpp;
  char libs[MAX_LIBS][256];
  int lib_count;
  SourceFile sources[MAX_FILES];
  int source_count;
} ProjectConfig;

// Function prototypes
void create_directory(const char *path);
void write_file(const char *path, const char *content);
void init_project();
void compile_project(int argc, char *argv[]);
void scan_sources(ProjectConfig *config);
void update_compile_commands(ProjectConfig *config);
time_t get_file_mtime(const char *path);
bool needs_recompilation(const char *source, const char *object);
void compile_file(const char *source, const char *object,
                  ProjectConfig *config);
void link_executable(ProjectConfig *config);
void parse_args(int argc, char *argv[], ProjectConfig *config);

// Template files content
const char *c_main_template = "#include <stdio.h>\n"
                              "#include <stdlib.h>\n"
                              "\n"
                              "int main(int argc, char *argv[]) {\n"
                              "    printf(\"Hello, World!\\n\");\n"
                              "    return 0;\n"
                              "}\n";

const char *cpp_main_template =
    "#include <iostream>\n"
    "#include <vector>\n"
    "#include <string>\n"
    "\n"
    "int main(int argc, char *argv[]) {\n"
    "    std::cout << \"Hello, World!\" << std::endl;\n"
    "    return 0;\n"
    "}\n";

const char *clangd_template = "CompileFlags:\n"
                              "  Add:\n"
                              "    - -std=c23\n"
                              "    - -Wall\n"
                              "    - -Wextra\n"
                              "    - -Wpedantic\n"
                              "    - -Werror\n"
                              "    - -O2\n"
                              "    - -I../include\n"
                              "  Remove:\n"
                              "    - -W*\n"
                              "\n"
                              "Diagnostics:\n"
                              "  ClangTidy:\n"
                              "    Add:\n"
                              "      - modernize-*\n"
                              "      - bugprone-*\n"
                              "      - performance-*\n"
                              "      - readability-*\n"
                              "    Remove:\n"
                              "      - modernize-use-trailing-return-type\n"
                              "      - readability-magic-numbers\n"
                              "\n"
                              "Index:\n"
                              "  Background: Build\n"
                              "\n"
                              "Style:\n"
                              "  FullyQualifiedNamespaces: No\n";

const char *clangd_cpp_template = "CompileFlags:\n"
                                  "  Add:\n"
                                  "    - -std=c++23\n"
                                  "    - -Wall\n"
                                  "    - -Wextra\n"
                                  "    - -Wpedantic\n"
                                  "    - -Werror\n"
                                  "    - -O2\n"
                                  "    - -I../include\n"
                                  "  Remove:\n"
                                  "    - -W*\n"
                                  "\n"
                                  "Diagnostics:\n"
                                  "  ClangTidy:\n"
                                  "    Add:\n"
                                  "      - modernize-*\n"
                                  "      - bugprone-*\n"
                                  "      - performance-*\n"
                                  "      - readability-*\n"
                                  "    Remove:\n"
                                  "      - modernize-use-trailing-return-type\n"
                                  "      - readability-magic-numbers\n"
                                  "\n"
                                  "Index:\n"
                                  "  Background: Build\n"
                                  "\n"
                                  "Style:\n"
                                  "  FullyQualifiedNamespaces: No\n";

const char *clang_format_template =
    "BasedOnStyle: LLVM\n"
    "IndentWidth: 4\n"
    "TabWidth: 4\n"
    "UseTab: Never\n"
    "BreakBeforeBraces: Attach\n"
    "AllowShortIfStatementsOnASingleLine: false\n"
    "AllowShortLoopsOnASingleLine: false\n"
    "AllowShortFunctionsOnASingleLine: None\n"
    "AllowShortBlocksOnASingleLine: false\n"
    "IndentCaseLabels: true\n"
    "ColumnLimit: 100\n"
    "PointerAlignment: Right\n"
    "SpaceAfterCStyleCast: false\n"
    "SpacesInParentheses: false\n"
    "SpacesInSquareBrackets: false\n"
    "SpaceBeforeAssignmentOperators: true\n"
    "ContinuationIndentWidth: 4\n"
    "AlignTrailingComments: true\n"
    "AlignConsecutiveAssignments: false\n"
    "AlignConsecutiveDeclarations: false\n"
    "SortIncludes: true\n"
    "IncludeBlocks: Regroup\n"
    "IncludeCategories:\n"
    "  - Regex: '^<.*\\.h>'\n"
    "    Priority: 1\n"
    "  - Regex: '^<.*>'\n"
    "    Priority: 2\n"
    "  - Regex: '.*'\n"
    "    Priority: 3\n";

void create_directory(const char *path) {
#ifdef _WIN32
  CreateDirectory(path, NULL);
#else
  mkdir(path, 0755);
#endif
}

void write_file(const char *path, const char *content) {
  FILE *f = fopen(path, "w");
  if (f) {
    fprintf(f, "%s", content);
    fclose(f);
  } else {
    fprintf(stderr, "Error: Could not create file %s\n", path);
  }
}

void init_project() {
  char choice[10];
  bool is_cpp = false;

  printf("Do you want to create a C or C++ project? (c/cpp): ");
  if (fgets(choice, sizeof(choice), stdin) == NULL) {
    fprintf(stderr, "Error reading input\n");
    return;
  }

  choice[strcspn(choice, "\n")] = 0;

  if (strcmp(choice, "cpp") == 0 || strcmp(choice, "c++") == 0) {
    is_cpp = true;
  } else if (strcmp(choice, "c") != 0) {
    printf("Invalid choice. Defaulting to C project.\n");
  }

  // Create directories
  create_directory("src");
  create_directory("include");
  create_directory("bin");
  create_directory("bin" PATH_SEP "obj");

  // Create main file
  if (is_cpp) {
    write_file("src" PATH_SEP "main.cpp", cpp_main_template);
    write_file(".clangd", clangd_cpp_template);
  } else {
    write_file("src" PATH_SEP "main.c", c_main_template);
    write_file(".clangd", clangd_template);
  }

  // Create config files
  write_file(".clang-format", clang_format_template);

  // Create initial compile_commands.json
  write_file("compile_commands.json", "[]");

  // Create .ccc_config
  FILE *config = fopen(".ccc_config", "w");
  if (config) {
    fprintf(config, "project_type=%s\n", is_cpp ? "cpp" : "c");
    fclose(config);
  }

  printf("Project initialized successfully!\n");
  printf("Structure created:\n");
  printf("  src/       - Source files\n");
  printf("  include/   - Header files\n");
  printf("  bin/       - Binary output\n");
  printf("  bin/obj/   - Object files\n");
  printf("\nConfiguration files created:\n");
  printf("  .clangd\n");
  printf("  .clang-format\n");
  printf("  compile_commands.json\n");
}

time_t get_file_mtime(const char *path) {
  struct stat st;
  if (stat(path, &st) == 0) {
    return st.st_mtime;
  }
  return 0;
}

bool needs_recompilation(const char *source, const char *object) {
  time_t source_time = get_file_mtime(source);
  time_t object_time = get_file_mtime(object);

  return source_time > object_time;
}

void scan_sources(ProjectConfig *config) {
  DIR *dir;
  struct dirent *entry;
  char path[MAX_PATH];

  config->source_count = 0;

  // Scan src directory
  dir = opendir("src");
  if (dir) {
    while ((entry = readdir(dir)) != NULL) {
      const char *ext = strrchr(entry->d_name, '.');
      if (ext) {
        bool is_source = false;
        if (config->is_cpp &&
            (strcmp(ext, ".cpp") == 0 || strcmp(ext, ".cc") == 0 ||
             strcmp(ext, ".cxx") == 0 || strcmp(ext, ".c") == 0)) {
          is_source = true;
        } else if (!config->is_cpp && strcmp(ext, ".c") == 0) {
          is_source = true;
        }

        if (is_source && config->source_count < MAX_FILES) {
          snprintf(config->sources[config->source_count].path, MAX_PATH,
                   "src" PATH_SEP "%s", entry->d_name);
          config->sources[config->source_count].mtime =
              get_file_mtime(config->sources[config->source_count].path);
          config->source_count++;
        }
      }
    }
    closedir(dir);
  }
}

void update_compile_commands(ProjectConfig *config) {
  FILE *f = fopen("compile_commands.json", "w");
  if (!f)
    return;

  fprintf(f, "[\n");

  for (int i = 0; i < config->source_count; i++) {
    if (i > 0)
      fprintf(f, ",\n");

    fprintf(f, "  {\n");
    fprintf(f, "    \"directory\": \"%s\",\n", getcwd(NULL, 0));
    fprintf(f,
            "    \"command\": \"%s -c %s -o bin/obj/%s.o -Iinclude -std=%s "
            "-Wall -Wextra -O2",
            config->is_cpp ? "g++" : "gcc", config->sources[i].path,
            strrchr(config->sources[i].path, PATH_SEP[0]) + 1,
            config->is_cpp ? "c++23" : "c23");

    // Add libraries
    for (int j = 0; j < config->lib_count; j++) {
      fprintf(f, " -l%s", config->libs[j]);
    }

    fprintf(f, "\",\n");
    fprintf(f, "    \"file\": \"%s\"\n", config->sources[i].path);
    fprintf(f, "  }");
  }

  fprintf(f, "\n]\n");
  fclose(f);
}

void compile_file(const char *source, const char *object,
                  ProjectConfig *config) {
  char cmd[MAX_PATH * 2];

  snprintf(cmd, sizeof(cmd),
           "%s -c %s -o %s -Iinclude -std=%s -Wall -Wextra -Wpedantic -O2",
           config->is_cpp ? "g++" : "gcc", source, object,
           config->is_cpp ? "c++23" : "c23");

  printf("Compiling: %s\n", source);

  int result = system(cmd);
  if (result != 0) {
    fprintf(stderr, "Compilation failed for %s\n", source);
    exit(1);
  }
}

void link_executable(ProjectConfig *config) {
  char cmd[MAX_PATH * 10];
  char objects[MAX_PATH * 5] = "";

  // Collect all object files
  for (int i = 0; i < config->source_count; i++) {
    char obj_name[MAX_PATH];
    const char *base = strrchr(config->sources[i].path, PATH_SEP[0]);
    if (!base)
      base = config->sources[i].path;
    else
      base++;

    snprintf(obj_name, sizeof(obj_name), " bin" PATH_SEP "obj" PATH_SEP "%s.o",
             base);
    strcat(objects, obj_name);
  }

  // Build link command
  snprintf(cmd, sizeof(cmd), "%s%s -o bin" PATH_SEP "main",
           config->is_cpp ? "g++" : "gcc", objects);

  // Add libraries
  for (int i = 0; i < config->lib_count; i++) {
    strcat(cmd, " -l");
    strcat(cmd, config->libs[i]);
  }

  printf("Linking executable...\n");

  int result = system(cmd);
  if (result != 0) {
    fprintf(stderr, "Linking failed\n");
    exit(1);
  }

  printf("Build successful! Executable: bin" PATH_SEP "main\n");
}

void parse_args(int argc, char *argv[], ProjectConfig *config) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
      // Parse library list
      char *libs = argv[++i];
      char *token = strtok(libs, " ,");
      while (token && config->lib_count < MAX_LIBS) {
        strcpy(config->libs[config->lib_count++], token);
        token = strtok(NULL, " ,");
      }
    }
  }
}

void compile_project(int argc, char *argv[]) {
  ProjectConfig config = {0};

  // Read project type
  FILE *f = fopen(".ccc_config", "r");
  if (f) {
    char line[256];
    while (fgets(line, sizeof(line), f)) {
      if (strncmp(line, "project_type=cpp", 16) == 0) {
        config.is_cpp = true;
      }
    }
    fclose(f);
  } else {
    fprintf(stderr, "Error: No project found. Run 'ccc init' first.\n");
    exit(1);
  }

  // Parse command line arguments
  parse_args(argc, argv, &config);

  // Scan for source files
  scan_sources(&config);

  if (config.source_count == 0) {
    fprintf(stderr, "Error: No source files found in src/\n");
    exit(1);
  }

  // Update compile_commands.json
  update_compile_commands(&config);

  // Compile changed files
  int compiled = 0;
  for (int i = 0; i < config.source_count; i++) {
    char obj_name[MAX_PATH];
    const char *base = strrchr(config.sources[i].path, PATH_SEP[0]);
    if (!base)
      base = config.sources[i].path;
    else
      base++;

    snprintf(obj_name, sizeof(obj_name), "bin" PATH_SEP "obj" PATH_SEP "%s.o",
             base);

    if (needs_recompilation(config.sources[i].path, obj_name)) {
      compile_file(config.sources[i].path, obj_name, &config);
      compiled++;
    }
  }

  if (compiled == 0) {
    printf("All files up to date.\n");
  }

  // link (in case libraries changed)
  link_executable(&config);
}

int main(int argc, char *argv[]) {
  if (argc > 1 && strcmp(argv[1], "init") == 0) {
    init_project();
  } else {
    compile_project(argc, argv);
  }

  return 0;
}
