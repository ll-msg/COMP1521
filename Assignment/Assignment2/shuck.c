////////////////////////////////////////////////////////////////////////
// COMP1521 21t2 -- Assignment 2 -- shuck, A Simple Shell
// <https://www.cse.unsw.edu.au/~cs1521/21T2/assignments/ass2/index.html>
//
// Written by YANRAN WANG (z5295965) on 30/07/2021.
//
// 2021-07-12    v1.0    Team COMP1521 <cs1521@cse.unsw.edu.au>
// 2021-07-21    v1.1    Team COMP1521 <cs1521@cse.unsw.edu.au>
//     * Adjust qualifiers and attributes in provided code,
//       to make `dcc -Werror' happy.
//

#include <sys/types.h>

#include <sys/stat.h>
#include <sys/wait.h>

#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// [[ TODO: put any extra `#include's here ]]
#include <spawn.h>
#include <glob.h>
#include <malloc.h>
// [[ TODO: put any `#define's here ]]
#define FILE_CHECK 5
#define RE_OUTPUT_WR 1
#define RE_OUTPUT_APP 2
#define RE_INPUT 3
#define BOTH_WR 4
#define BOTH_APP 5

//
// Interactive prompt:
//     The default prompt displayed in `interactive' mode --- when both
//     standard input and standard output are connected to a TTY device.
//
static const char *const INTERACTIVE_PROMPT = "shuck& ";

//
// Default path:
//     If no `$PATH' variable is set in Shuck's environment, we fall
//     back to these directories as the `$PATH'.
//
static const char *const DEFAULT_PATH = "/bin:/usr/bin";

//
// Default history shown:
//     The number of history items shown by default; overridden by the
//     first argument to the `history' builtin command.
//     Remove the `unused' marker once you have implemented history.
//
static const int DEFAULT_HISTORY_SHOWN __attribute__((unused)) = 10;

//
// Input line length:
//     The length of the longest line of input we can read.
//
static const size_t MAX_LINE_CHARS = 1024;

//
// Special characters:
//     Characters that `tokenize' will return as words by themselves.
//
static const char *const SPECIAL_CHARS = "!><|";

//
// Word separators:
//     Characters that `tokenize' will use to delimit words.
//
static const char *const WORD_SEPARATORS = " \t\r\n";

// [[ TODO: put any extra constants here ]]


// [[ TODO: put any type definitions (i.e., `typedef', `struct', etc.) here ]]


static void execute_command(char **words, char **path, char **environment);
static void do_exit(char **words);
static int is_executable(char *pathname);
static char **tokenize(char *s, char *separators, char *special_chars);
static void free_tokens(char **tokens);

// [[ TODO: put any extra function prototypes here ]]
static void save_history(char **words);
static void exe_history(char **words, char **path, char **environment);
static char** file_expansion(char **words, char **path, char **environment);
static void redirection(char **words);
static int check_num_arguments(char **words);
static int redirection_check(char **words);
static int redirection_invalid(char **words, int check_exist);
static int redirection_file_mod(char **words, char *input, 
                                                char *output, int check_exist);
static int redirection_builtin(char **words, int check_exist);
static void pipe_exe(char **words, char **path, char **environment);

int main (void)
{
    // Ensure `stdout' is line-buffered for autotesting.
    setlinebuf(stdout);

    // Environment variables are pointed to by `environ', an array of
    // strings terminated by a NULL value -- something like:
    //     { "VAR1=value", "VAR2=value", NULL }
    extern char **environ;

    // Grab the `PATH' environment variable for our path.
    // If it isn't set, use the default path defined above.
    char *pathp;
    if ((pathp = getenv("PATH")) == NULL) {
        pathp = (char *) DEFAULT_PATH;
    }
    char **path = tokenize(pathp, ":", "");

    // Should this shell be interactive?
    bool interactive = isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);

    // Main loop: print prompt, read line, execute command
    while (1) {
        // If `stdout' is a terminal (i.e., we're an interactive shell),
        // print a prompt before reading a line of input.
        if (interactive) {
            fputs(INTERACTIVE_PROMPT, stdout);
            fflush(stdout);
        }

        char line[MAX_LINE_CHARS];
        if (fgets(line, MAX_LINE_CHARS, stdin) == NULL)
            break;

        // Tokenise and execute the input line.
        char **command_words =
            tokenize(line, (char *) WORD_SEPARATORS, (char *) SPECIAL_CHARS);
        execute_command(command_words, path, environ);
        save_history(command_words);
        exe_history(command_words, path, environ);
        free_tokens(command_words);
    }

    free_tokens(path);
    return 0;
}


//
// Execute a command, and wait until it finishes.
//
//  * `words': a NULL-terminated array of words from the input command line
//  * `path': a NULL-terminated array of directories to search in;
//  * `environment': a NULL-terminated array of environment variables.
//
static void execute_command(char **words, char **path, char **environment)
{
    assert(words != NULL);
    assert(path != NULL);
    assert(environment != NULL);

    char *program = words[0];

    if (program == NULL) {
        // nothing to do
        return;
    }

    if (strcmp(program, "exit") == 0) {
        do_exit(words);
        // `do_exit' will only return if there was an error.
        return;
    }

    //////////////////// subset 5 ////////////////////
    int pipe_check = 0;
    for (int i = 0; words[i] != NULL; i++) {
        if (strrchr(words[i], '|')) {
            pipe_check = 1;
        }
    }
    if (pipe_check == 1) {
        pipe_exe(words, path, environment);
        return;
    }

    // check how many arguments in the words
    int check = check_num_arguments(words);
    char **expand_command = file_expansion(words, path, environment);

    // if execute file expansions
    // get the number of updated arguments
    check = check_num_arguments(expand_command);
    // subset 4 check commands type
    int check_exist = redirection_check(expand_command);

    // subset4 invalid error checking
    int find_erorr = redirection_invalid(expand_command, check_exist);
    if (find_erorr == 1) {
        return;
    }

    // subset 4 builtin command error checking
    if (!strcmp(program, "pwd") || !strcmp(program, "cd") ||
        !strcmp(program, "history") || !strcmp(program, "!")) {
        for (int i = 0; expand_command[i] != NULL; i++) {
            if (!strcmp(expand_command[i], "<") 
                                        || !strcmp(expand_command[i], ">")) {
                fprintf(stderr, 
        "%s: I/O redirection not permitted for builtin commands\n", program);
                return;
            }
        }
    }

    //////////////////// subset 0 ////////////////////
    char *cd_path = NULL;
    // when the command is "cd" - change directory
    if (check_exist == 0) {
        if (!strcmp(program, "cd")) {
            // if there are too many arguments
            if (check > 2) {
                fprintf(stderr, "cd: too many arguments\n");
                return;
            }
            cd_path = expand_command[1];
            // sent directory command is NULL
            if (cd_path == NULL) {
                cd_path = getenv("HOME");
            }
            // the directory doesn't exist
            if (chdir(cd_path) != 0) {
                fprintf(stderr, "cd: %s: No such file or directory\n", cd_path);
                return;
            }
        }
        // when the command is "pwd" - print working directory
        if (!strcmp(program, "pwd")) {
            if (check > 1) {
                fprintf(stderr, "pwd: too many arguments\n");
                return;
            }
            char *print_path;
            print_path = getcwd(NULL, 0);
            printf("current directory is '%s'\n", print_path);
            free(print_path);
        }
    }

    //////////////////// subset 1 ////////////////////
    if (strcmp(program, "pwd") && strcmp(program, "cd") && 
        strcmp(program, "history") && strcmp(program, "!")) {
        pid_t pid;
        int count = 0;
        
        // find path of commands used in posix spawn
        // get the max length of path
        char *pathp;
        if ((pathp = getenv("PATH")) == NULL) {
            pathp = (char *) DEFAULT_PATH;
        }
        int size = strlen(pathp) + strlen(program) + strlen("/") + 1;
        char exe_path[size];
        char use_path[size];
        // if command does not has "/", find the actual path 
        if (strrchr(program, '/') == NULL) {
            for (int i = 0; path[i] != NULL && i < strlen(*path); i++) {
                strcpy(exe_path, path[i]);
                strcat(exe_path, "/");
                // if there is only ">" or ">>"
                if (check_exist == RE_OUTPUT_WR || check_exist == RE_OUTPUT_APP 
                                                        || check_exist == 0) {
                    strcat(exe_path, program);
                    // if there are also "<"
                } else if (check_exist == RE_INPUT || check_exist == BOTH_WR 
                                                || check_exist == BOTH_APP) {
                    strcat(exe_path, expand_command[2]);
                }

                if (is_executable(exe_path)) {
                    strcpy(use_path, exe_path);
                    count++;
                }
            }
            // if command has "/", use it as path directly
        } else {
            if (is_executable(program)) {
                strcpy(use_path, program);
                count++;
            }
        }
        
        // there is no such executable command
        if (count == 0 && check_exist == 0) {
            fprintf(stderr, "%s: command not found\n", program);
            return;
        }

        //////////////////// subset 4 ////////////////////
        // check if the command has ">" or "<" or "<<"
        if (check_exist != 0) {
            // find the file
            char in_path[MAX_LINE_CHARS];
            char out_path[MAX_LINE_CHARS];

            int check_out = 0;
            FILE *re_output = 0;
            FILE *double_out = 0;

            // get the file path
            if (check_exist == RE_INPUT) {
                sprintf(in_path, "%s", expand_command[1]);
            } else if (check_exist ==  RE_OUTPUT_WR || 
                        check_exist == RE_OUTPUT_APP) {
                sprintf(out_path, "%s", expand_command[check - 1]);
            } else {
                // input file path
                sprintf(in_path, "%s", expand_command[1]);
                // output file path
                sprintf(out_path, "%s", expand_command[check - 1]);
            }
            // check if the file exists
            if (check_exist == BOTH_WR || check_exist == BOTH_APP) {
                // if output file exists, check_out will be 5
                if ((double_out = fopen(out_path, "r")) != NULL) {
                    check_out += FILE_CHECK;
                }
                if (check_out == 0) {
                    // if output file not exists, create one
                    double_out = fopen(out_path, "w");
                }
            }

            if (check_exist == RE_OUTPUT_WR || check_exist == BOTH_WR) {
                if (check_exist == BOTH_WR) {
                    // input file
                    re_output = fopen(in_path, "r");
                    // output file
                    double_out = fopen(out_path, "w");
                } else {
                    // if file exists, overwritten the file - ">"
                    re_output = fopen(out_path, "w");
                }
            } else if (check_exist == RE_OUTPUT_APP 
                        || check_exist == BOTH_APP) {
                if (check_exist == BOTH_APP) {
                    // input file
                    re_output = fopen(in_path, "r");
                    // output file
                    double_out = fopen(out_path, "a");
                } else {
                    // if file exists, append the file - ">>"
                    re_output = fopen(out_path, "a");
                }
            }
            // subset 4 builtin command error checking
            int check_builtin = 
                            redirection_builtin(expand_command, check_exist);
            if (check_builtin == 1) {
                return;
            }
            // subset 4 file permissions error checking
            int check_mod = redirection_file_mod(expand_command, in_path, 
                                                    out_path, check_exist);
            if (check_mod == 0) {
                return;
            }

            // redirect output into specified file
            posix_spawn_file_actions_t actions;
            // initilize actions
            posix_spawn_file_actions_init(&actions);
            // get the file descriptor
            int fd_in = 0;
            int fd_out = 0;
            
            FILE *command_file = 0;
            if (check_exist == RE_INPUT) {
                // find the path of the file
                command_file = fopen(in_path, "r");
                fd_in = fileno(command_file);
                posix_spawn_file_actions_adddup2(&actions, fd_in, STDIN_FILENO);
            } else if (check_exist == RE_OUTPUT_WR || 
                        check_exist == RE_OUTPUT_APP) {
                fd_out = fileno(re_output);
            posix_spawn_file_actions_adddup2(&actions, fd_out, STDOUT_FILENO);
            } else if (check_exist == BOTH_WR || check_exist == BOTH_APP) {
                // input file to read
                fd_in = fileno(re_output);
                posix_spawn_file_actions_adddup2(&actions, 
                                                        fd_in, STDIN_FILENO);
                // output file to write
                fd_out = fileno(double_out);
                posix_spawn_file_actions_adddup2(&actions, 
                                                        fd_out, STDOUT_FILENO);
            }

            // get the right part of command words
            char original[MAX_LINE_CHARS];
            sprintf(original, "%s", expand_command[0]);
            // if the command is "<"
            if (check_exist == RE_OUTPUT_WR) {
                for (int i = 1; i < check - 2; i++) {
                    strcat(original, " ");
                    strcat(original, expand_command[i]);
                }
                // if the command is "<<"
            } else if (check_exist == RE_OUTPUT_APP) {
                for (int i = 1; i < check - 3; i++) {
                    strcat(original, " ");
                    strcat(original, expand_command[i]);
                }
                // if the command has "<" and ">>" or ">" at the same time
            } else if (check_exist == RE_INPUT || check_exist == BOTH_WR 
                                            || check_exist == BOTH_APP) {
                strcpy(original, expand_command[2]);
                for (int i = 3; i < check && strcmp(expand_command[i], ">"); i++) {
                    strcat(original, " ");
                    strcat(original, expand_command[i]);
                }
            }
            // find arguments
            char **command_words = tokenize(original, 
                            (char *) WORD_SEPARATORS, (char *) SPECIAL_CHARS);
            // subset 4 executing
            if (posix_spawn(&pid, use_path, &actions, NULL, command_words, 
                                                        environment) != 0) {
                posix_spawn_file_actions_destroy(&actions);
                perror("spawn");
                exit(1);
            }
            free_tokens(command_words);
            posix_spawn_file_actions_destroy(&actions);
            if (check_exist == 3) {
                fclose(command_file);
            } else if (check_exist == 1 || check_exist == 2) {
                fclose(re_output);
            } else {
                fclose(double_out);
                fclose(re_output);
            }
        } else {
            // subset 1 executing
            if (posix_spawn(&pid, use_path, NULL, NULL, expand_command, 
                                        environment) != 0) {
                perror("spawn");
                exit(1);
            }
        }
        // get exit status and print out
        int exit_status;
        if (waitpid(pid, &exit_status, 0) == -1) {
            perror("waitpid");
            exit(1);
        }
        printf("%s exit status = %d\n", use_path, (exit_status / 256));
    }
    free_tokens(expand_command);
}


//
// Implement the `exit' shell built-in, which exits the shell.
//
// Synopsis: exit [exit-status]
// Examples:
//     % exit
//     % exit 1
//
static void do_exit(char **words)
{
    assert(words != NULL);
    assert(strcmp(words[0], "exit") == 0);

    int exit_status = 0;

    if (words[1] != NULL && words[2] != NULL) {
        // { "exit", "word", "word", ... }
        fprintf(stderr, "exit: too many arguments\n");

    } else if (words[1] != NULL) {
        // { "exit", something, NULL }
        char *endptr;
        exit_status = (int) strtol(words[1], &endptr, 10);
        if (*endptr != '\0') {
            fprintf(stderr, "exit: %s: numeric argument required\n", words[1]);
        }
    }

    exit(exit_status);
}


//
// Check whether this process can execute a file.  This function will be
// useful while searching through the list of directories in the path to
// find an executable file.
//
static int is_executable(char *pathname)
{
    struct stat s;
    return
        // does the file exist?
        stat(pathname, &s) == 0 &&
        // is the file a regular file?
        S_ISREG(s.st_mode) &&
        // can we execute it?
        faccessat(AT_FDCWD, pathname, X_OK, AT_EACCESS) == 0;
}


//
// Split a string 's' into pieces by any one of a set of separators.
//
// Returns an array of strings, with the last element being `NULL'.
// The array itself, and the strings, are allocated with `malloc(3)';
// the provided `free_token' function can deallocate this.
//
static char **tokenize(char *s, char *separators, char *special_chars)
{
    size_t n_tokens = 0;

    // Allocate space for tokens.  We don't know how many tokens there
    // are yet --- pessimistically assume that every single character
    // will turn into a token.  (We fix this later.)
    char **tokens = calloc((strlen(s) + 1), sizeof *tokens);
    assert(tokens != NULL);

    while (*s != '\0') {
        // We are pointing at zero or more of any of the separators.
        // Skip all leading instances of the separators.
        s += strspn(s, separators);

        // Trailing separators after the last token mean that, at this
        // point, we are looking at the end of the string, so:
        if (*s == '\0') {
            break;
        }

        // Now, `s' points at one or more characters we want to keep.
        // The number of non-separator characters is the token length.
        size_t length = strcspn(s, separators);
        size_t length_without_specials = strcspn(s, special_chars);
        if (length_without_specials == 0) {
            length_without_specials = 1;
        }
        if (length_without_specials < length) {
            length = length_without_specials;
        }

        // Allocate a copy of the token.
        char *token = strndup(s, length);
        assert(token != NULL);
        s += length;

        // Add this token.
        tokens[n_tokens] = token;
        n_tokens++;
    }

    // Add the final `NULL'.
    tokens[n_tokens] = NULL;

    // Finally, shrink our array back down to the correct size.
    tokens = realloc(tokens, (n_tokens + 1) * sizeof *tokens);
    assert(tokens != NULL);

    return tokens;
}


//
// Free an array of strings as returned by `tokenize'.
//
static void free_tokens(char **tokens)
{
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

//
// subset2 - save commands to history
//
static void save_history(char **words) {

    // open the file shuck_history, if doesn't exist then create
    FILE *history;
    // find the path of the target file
    char *his_name = "/.shuck_history";
    int size = strlen(getenv("HOME")) + strlen(his_name) + 1;
    char filepath[size];
    strcpy(filepath, getenv("HOME"));
    strcat(filepath, his_name);
    
    // if fail creating, exit
    if ((history = fopen(filepath, "a+")) == NULL) {
        fprintf(stderr, "No such file or directory\n");
        return;
    }

    if (!strcmp(words[0], "!")) {
        fclose(history);
        return;
    }
    // append the command into history file
    int i = 0;
    while (words[i] != NULL) {
        fputs(words[i], history);
        fputs(" ", history);
        i++;
    }
    fputs("\n", history);
    fclose(history);
}
//
// subset2 - execute commands from history
//
static void exe_history(char **words, char **path, char **environment) {
    // check if the command has redirection command
    int check_exist = redirection_check(words);
    int check = check_num_arguments(words);
    // if no redirection command, execute
    if (check_exist == 0) {
        // find the path of the file history
        FILE *history;
        char *his_name = "/.shuck_history";
        int size = strlen(getenv("HOME")) + strlen(his_name) + 1;
        char filepath[size];

        // open the file history
        strcpy(filepath, getenv("HOME"));
        strcat(filepath, his_name);
        
        if ((history = fopen(filepath, "r")) == NULL) {
            fprintf(stderr, "No such file or directory\n");
            return;
        }
        int total = 0;
        
        if (!strcmp(words[0], "history")) {
            // check if the number of arguments is right
            if (check > 2) {
                fprintf(stderr, "history: too many arguments\n");
                return;
            }
            // if n is not specified, print the last 10 commands
            char prt_command[MAX_LINE_CHARS];
            int prt_index = 0;
            int check_history = 1;
            int target_index = 0;
            // calculate the total number of commands in history
            while (fgets(prt_command, MAX_LINE_CHARS, history) != NULL) {
                total++;
            }
            // set pointer back to the beginning of the file
            fseek(history, 0, SEEK_SET);
            // check if the latest command is history, not true check will be 0
            char *str = "history";
            if (!strcmp(str, prt_command)) {
                check_history = 0;
            }
            // print out history
            while (fgets(prt_command, MAX_LINE_CHARS, history) != NULL) {
                if (words[1] != NULL) {
                    // check if the argument is not a number
                    char *endptr;
                    target_index = strtol(words[1], &endptr, 10);
                    if (endptr ==  words[1] || *endptr != '\0') {
                        fprintf(stderr, "%s: %s: numeric argument required\n", 
                                                            words[0], words[1]);
                        return;
                    }
                    if (target_index < 0) {
                        fprintf(stderr, "%s: %s: numeric argument required\n", 
                                                            words[0], words[1]);
                        return;
                    }
                }
                
                // if the last command is history
                // print commands except the last "history"
                if (check_history != 1) {
                    // if no specific line commands
                    // print last 10 commands
                    if (words[1] == NULL) {
                        if (prt_index < total && 
                                                    prt_index >= (total - 10)) {
                            printf("%d: %s", prt_index, prt_command);
                        }
                        // if specifices line commands
                        // print last n commands
                    } else {
                        if (prt_index < total && 
                                        prt_index >= (total - target_index)) {
                            printf("%d: %s", prt_index, prt_command);
                        }
                    }
                    // if the last command is not history
                } else {
                    // if no specific line commands
                    // print last 10 commands
                    if (words[1] == NULL) {
                        if (prt_index < (total - 1) && 
                                                    prt_index >= (total - 11)) {
                            printf("%d: %s", prt_index, prt_command);
                        }
                        // if specifices line commands
                        // print last n commands
                    } else {
                        if (prt_index < (total - 1) && 
                                    prt_index >= (total - target_index - 1)) {
                            printf("%d: %s", prt_index, prt_command);
                        }
                    }
                }
                prt_index++;
            }
        }

        // execute history
        if (!strcmp(words[0], "!")) {
            // if more than 1 arguments
            if (check > 2) {
                fprintf(stderr, "!: too many arguments\n");
                return;
            }

            char exe_command[MAX_LINE_CHARS];
            total = 0;
            // calculate the total number of commands in history
            while (fgets(exe_command, MAX_LINE_CHARS, history) != NULL) {
                total++;
            }
            // set pointer back to the beginning of the file
            fseek(history, 0, SEEK_SET);
            // if no specific line commands
            // execute the last command
            if (words[1] == NULL) {
                while (fgets(exe_command, MAX_LINE_CHARS, history) != NULL) {
                    fgets(exe_command, MAX_LINE_CHARS, history);
                }
                printf("%s", exe_command);
                char **command_words = tokenize(exe_command, 
                            (char *) WORD_SEPARATORS, (char *) SPECIAL_CHARS);
                execute_command(command_words, path, environment);
                save_history(command_words);
                free_tokens(command_words);
                // if specifices line commands
                // execute the nth command
            } else {
                char *endptr;
                int exe_index = strtol(words[1], &endptr, 10);
                // check if the given argument is not a number
                if (endptr ==  words[1] || *endptr != '\0') {
                    fprintf(stderr, "%s: %s: numeric argument required\n", 
                                                        words[0], words[1]);
                    return;
                }
                if (exe_index < 0) {
                    fprintf(stderr, "%s: %s: numeric argument required\n", 
                                                        words[0], words[1]);
                    return;
                }
                // if the index out of bound
                if (exe_index > total) {
                    fprintf(stderr, "!: invalid history reference\n");
                    return;
                }
                int find_index = 0;
                while (fgets(exe_command, MAX_LINE_CHARS, history) != NULL) {
                    if (exe_index == find_index) {
                        printf("%s", exe_command);
                        char **command_words = tokenize(exe_command, 
                            (char *) WORD_SEPARATORS, (char *) SPECIAL_CHARS);
                        execute_command(command_words, path, environment);
                        save_history(command_words);
                        free_tokens(command_words);
                    }
                    find_index++;
                }
            }
        }
        fclose(history);
    }
}

//
// subset3 - filename expansion
//
static char** file_expansion(char **words, char **path, char **environment) {
    int check = check_num_arguments(words);   
    glob_t matches;
    char *save_commands = malloc(MAX_LINE_CHARS * sizeof(char));

    int result = 0;
    char *expand_commands = realloc(save_commands, 
                                        (strlen(words[0]) + 2) * sizeof(char));
    strcpy(expand_commands, words[0]);
    int size = strlen(words[0]);
    // hold the pattern expansion
    for (int i = 1; i < check; i++) {
        result = glob(words[i], GLOB_NOCHECK|GLOB_TILDE, NULL, &matches);
        if (result != 0) {
            size += (strlen(words[i]) + 2) * sizeof(char);
            expand_commands = realloc(expand_commands, size);
            strcat(expand_commands, " ");
            strcat(expand_commands, words[i]);
        } else {
            for (int j = 0; j < matches.gl_pathc; j++) {
                size += (strlen(matches.gl_pathv[j]) + 2) * sizeof(char);
                expand_commands = realloc(expand_commands, size);
                strcat(expand_commands, " ");
                strcat(expand_commands, matches.gl_pathv[j]);
            }
        }
        globfree(&matches);
    }
    char **command_words = tokenize(expand_commands, 
                            (char *) WORD_SEPARATORS, (char *) SPECIAL_CHARS);
    free(expand_commands);
    return command_words;
}

// helper functions //

//
// check how many arguments are in the words
//
static int check_num_arguments(char **words) {
    int check = 0;
    for (int i = 0; words[i] != NULL; i++) {
        check += 1;
    }
    return check; 
}

//
// check the type of commands in subset4
//
static int redirection_check(char **words) {
    int check = check_num_arguments(words);
    int check_exist = 0;
    
    if (check >= 3) {
        // if the command is ">"
        if (!strcmp(words[check - 2], ">")) {
            // if the command is ">" and "<"
            if (!strcmp(words[0], "<")) {
                check_exist = BOTH_WR;
            } else {
                check_exist = RE_OUTPUT_WR;
            }
        }
        // if the command is ">>"
        if (!strcmp(words[check - 2], ">") && !strcmp(words[check - 3], ">")) {
            // if the command is ">>" and "<"
            if (!strcmp(words[0], "<")) {
                check_exist = BOTH_APP;
            } else {
                check_exist = RE_OUTPUT_APP;
            }
        }
        // if the command is "<"
        if (!strcmp(words[0], "<") && strcmp(words[check - 2], ">")) {
            check_exist = RE_INPUT;
        }
    }
    return check_exist;
}

//
// check if subset4 redirection command has invalid input
//
static int redirection_invalid(char **words, int check_exist) {
    int find_error = 0;
    int arg_num = check_num_arguments(words);
    
    // check if there are enough arguments
    if (!strcmp(words[0], "<") && arg_num < 3) {
        fprintf(stderr, "Invalid input redirection\n");
        find_error = 1;
        return find_error;
    } else if (!strcmp(words[arg_num - 1], ">") && arg_num < 3) {
        fprintf(stderr, "Invalid input redirection\n");
        find_error = 1;
        return find_error;
    }

    // check if the command is in the right order
    for (int i = 0; words[i] != NULL; i++) {
        if (!strcmp(words[i], "<") && (i != 0)) {
            fprintf(stderr, "Invalid input redirection\n");
            find_error = 1;
            return find_error;
        } else if (!strcmp(words[i], ">") && 
                                    (i != arg_num - 2 && i != arg_num - 3)) {
            fprintf(stderr, "Invalid input redirection\n");
            find_error = 1;
            return find_error;
        }
    }
    // check if the command has file and command
    if (check_exist == RE_OUTPUT_WR || check_exist == RE_OUTPUT_APP) {
        if (!strcmp(words[arg_num - 1], "<")) {
            fprintf(stderr, "Invalid input redirection\n");
            find_error = 1;
            return find_error;
        }
    }
    if(check_exist == RE_INPUT) {
        if (words[1] == NULL || words[2] == NULL) {
            fprintf(stderr, "Invalid input redirection\n");
            find_error = 1;
            return find_error;
        }
    }
    if(check_exist == BOTH_WR || check_exist == BOTH_APP) {
        if (words[1] == NULL || words[2] == NULL || 
                                            !strcmp(words[arg_num - 1], "<")) {
            fprintf(stderr, "Invalid input redirection\n");
            find_error = 1;
            return find_error;
        }
    }
    return find_error;
}

//
// check if subset4 redirection has perimission to the target files
//
static int redirection_file_mod(char **words, char *input, 
                                                char *output, int check_exist) {
    int access_check = 1;
    int arg_num = check_num_arguments(words);

    // if command is ">" or ">>" and no write permission to the file
    if (check_exist == RE_OUTPUT_WR || check_exist == RE_OUTPUT_APP) {
        if (access(output, W_OK) == -1) {
            fprintf(stderr, "%s: Permission denied\n", words[arg_num - 1]);
            access_check = 0;
            return access_check;
        }
        // if command is "<" and no such file exists
    } else if (check_exist == RE_INPUT) {
        if (access(input, F_OK) == -1) {
            fprintf(stderr, "%s: No such file or directory\n", words[1]);
            access_check = 0;
            return access_check;
            // if command is "<" and no read permission to the file
        } else if (access(input, R_OK) == -1) {
            fprintf(stderr, "%s: Permission denied\n", words[1]);
            access_check = 0;
            return access_check;
        }
        // if both command exist
    } else if (check_exist == BOTH_WR || check_exist == BOTH_APP) {
        if (access(input, F_OK) == -1) {
            fprintf(stderr, "%s: No such file or directory\n", words[1]);
            access_check = 0;
            return access_check;
        } else if (access(input, R_OK) == -1) {
            fprintf(stderr, "%s: Permission denied\n", words[1]);
            access_check = 0;
            return access_check;
        } else if (access(output, W_OK) == -1) {
            fprintf(stderr, "%s: Permission denied\n", words[arg_num - 1]);
            access_check = 0;
            return access_check;
        }
    }
    return access_check;
}

//
// check if subset4 redirection uses commands with builtin command
//
static int redirection_builtin(char **words, int check_exist) {
    int check = check_num_arguments(words);
    int check_builtin = 0;
    if (check_exist != 0) {
        // deal with cd command
        if (!strcmp(words[0], "cd") || (!strcmp(words[check - 1], "cd") 
                                            && strcmp(words[check - 2], ">"))) {
            fprintf(stderr, 
            "cd: I/O redirection not permitted for builtin commands\n");
            check_builtin = 1;
            return check_builtin;
            // deal with history command
        } else if ((!strcmp(words[check - 1], "pwd") && 
                strcmp(words[check - 2], ">")) || !strcmp(words[0], "pwd")) {
            fprintf(stderr, 
            "pwd: I/O redirection not permitted for builtin commands\n");
            check_builtin = 1;
            return check_builtin;
        } else if (!strcmp(words[0], "<") && !strcmp(words[2], "history")) {
             fprintf(stderr, 
            "history: I/O redirection not permitted for builtin commands\n");
            check_builtin = 1;
            return check_builtin;
        } else if (!strcmp(words[0], "history")) {
            for (int i = 0; words[i] != NULL; i++) {
                if (!strcmp(words[i], ">")) {
                    fprintf(stderr, 
            "history: I/O redirection not permitted for builtin commands\n");
                    check_builtin = 1;
                    return check_builtin;
                }
            }
        }
    }
    return check_builtin;
}

//
// subset 5 - Pipes
//
static void pipe_exe(char **words, char **path, char **environment) {
    int total = check_num_arguments(words) - 1;
    // find the correct part of commands
    char *pipe_commands[MAX_LINE_CHARS];
    char save_command_position[MAX_LINE_CHARS];
    int num_commands = 0;
    // create space for commands array
    for (int i = 0; words[i] != NULL && i < MAX_LINE_CHARS; i++) {
        pipe_commands[i] = (char *)malloc(sizeof(char *) * strlen(words[i]));
    }
    // if the command has "<"
    if (!strcmp(words[0], "<")) {
        strcpy(pipe_commands[0], "");
        save_command_position[0] = 2;
    } else {
        strcpy(pipe_commands[0], words[0]);
        save_command_position[0] = 0;
    }
    // if the command has "<"
    // move the index to the first command place
    int command_index = 0;
    if (strrchr(words[0], '<') != NULL) {
        command_index = 2;
    }
    // append each command into the commands array
    while (words[command_index] != NULL) {
        if (!strcmp(words[command_index], "|")) {
            command_index += 1;
            num_commands += 1;
            strcpy(pipe_commands[num_commands], words[command_index]);
            save_command_position[num_commands] = command_index;
        } else {
            if (command_index != 0) {
                // for each situation, stop the index at the right place
                if ((!strcmp(words[total - 1], ">") 
                && strcmp(words[total - 2], ">") && command_index <= total - 2) 
                                            || (strcmp(words[total - 1], ">")) 
            || (!strcmp(words[total - 2], ">") && command_index <= total - 3)) {
                    if (!strcmp(words[0], "<") && command_index == 2) {
                        strcat(pipe_commands[num_commands], 
                                                        words[command_index]);
                    } else {
                        strcat(pipe_commands[num_commands], " ");
                        strcat(pipe_commands[num_commands], 
                                                        words[command_index]);
                    }
                }
            }
        }
        command_index++;
    }

    // create space for paths array
    char *pipe_path[MAX_LINE_CHARS];
    for (int i = 0; i <= num_commands; i++) {
        pipe_path[i] = (char *)malloc(sizeof(char *) * MAX_LINE_CHARS);
    }

    char *pathp;
    if ((pathp = getenv("PATH")) == NULL) {
        pathp = (char *) DEFAULT_PATH;
    }
    
    for (int i = 0; i <= num_commands; i++) {
        
        int index = save_command_position[i];
        int size = strlen(pathp) + strlen(words[index]) + strlen("/") + 1;
        char exe_path[size];
        char use_path[size];
        
        // if there is no "/" in the given argument
        // find the right executable path
        if (strrchr(words[index], '/') == NULL) {
            for (int j = 0; path[j] != NULL && j < strlen(*path); j++) {
                sprintf(exe_path, "%s%s%s", path[j], "/", words[index]);
                if (is_executable(exe_path)) {
                    strcpy(use_path, exe_path);
                }
            }
            // if there is "/" in the given argument
            // use it directly to check
        } else {
            if (is_executable(words[index])) {
                strcpy(use_path, words[index]);
            }
        }
        sprintf(pipe_path[i], "%s", use_path);
    }
    // create pipes
    int pipe_file_descriptors[num_commands][2];
    for (int i = 0; i < num_commands; i++) {
        pipe(pipe_file_descriptors[i]);
    }

    pid_t pid;
    extern char **environ;
    posix_spawn_file_actions_t actions;

    // execute commands
    for (int i = 0; i <= num_commands; i++) {
        // initialize actions
        posix_spawn_file_actions_init(&actions);
        if (i == 0) {
            // close unused ends of all the pipes
            for (int j = 1; j < num_commands; j++) {
                posix_spawn_file_actions_addclose(&actions, 
                                                pipe_file_descriptors[0][0]);
                posix_spawn_file_actions_addclose(&actions, 
                                                pipe_file_descriptors[j][0]);
                posix_spawn_file_actions_addclose(&actions, 
                                                pipe_file_descriptors[j][1]);
            }
            // if need redirection input
            // redirection stdin to the target file
            FILE *input_file = 0;
            if (!strcmp(words[0], "<")) {
                // if the first command needs input redirection
                // find file path
                char *current = getcwd(NULL, 0);
                char in_path[MAX_LINE_CHARS];
                sprintf(in_path, "%s%s%s", current, "/", words[1]);
                if ((input_file = fopen(in_path, "r")) == NULL) {
                    fprintf(stderr, "input: No such file or directory\n");
                    return;
                }
                // get file descriptor
                int fd_input = fileno(input_file);
                // set the input place as the target file
                posix_spawn_file_actions_adddup2(&actions, fd_input, 
                                                                STDIN_FILENO);
            }
            // set the output place as write end of pipe
            posix_spawn_file_actions_adddup2(&actions, 
                                    pipe_file_descriptors[0][1], STDOUT_FILENO);
            // tokenize the commands
            char **pipe_use_command = tokenize(pipe_commands[i], 
                            (char *) WORD_SEPARATORS, (char *) SPECIAL_CHARS);
            posix_spawn(&pid, pipe_path[i], &actions, NULL, 
                                                    pipe_use_command, environ);
            free_tokens(pipe_use_command);
            if (!strcmp(words[0], "<")) {
                fclose(input_file);
            }
            close(pipe_file_descriptors[0][1]);
        } else if (i == num_commands) {
            // close unused ends of all the pipes
            for (int k = 0; k < num_commands - 1; k++) {
                posix_spawn_file_actions_addclose(&actions, 
                                                pipe_file_descriptors[0][1]);
                posix_spawn_file_actions_addclose(&actions, 
                                                pipe_file_descriptors[k][0]);
                posix_spawn_file_actions_addclose(&actions, 
                                                pipe_file_descriptors[k][1]);
            }
            // if need redirection output
            // redirection stdin to the target file
            FILE *output_file = 0;
            if (!strcmp(words[total - 1], ">")) {
                // if the last command needs output redirection
                // find file path
                char *current = getcwd(NULL, 0);
                char out_path[MAX_LINE_CHARS];
                sprintf(out_path, "%s%s%s", current, "/", words[total]);
                // if the command is ">>"
                // open the file to append
                if (!strcmp(words[total - 2], ">")) {
                    if ((output_file = fopen(out_path, "a")) == NULL) {
                        fprintf(stderr, 
                            "%s: No such file or directory\n", words[total]);
                        return;
                    }
                    // if the command is ">"
                    // open the file to write
                } else {
                    if ((output_file = fopen(out_path, "w")) == NULL) {
                        fprintf(stderr, 
                            "%s: No such file or directory\n", words[total]);
                        return;
                    }
                }
                // get file descriptor
                int fd_output = fileno(output_file);
                // set the output place as the target file
                posix_spawn_file_actions_adddup2(&actions, 
                                                    fd_output, STDOUT_FILENO);
            }
            // set the input place as write end of pipe
            posix_spawn_file_actions_adddup2(&actions, 
                    pipe_file_descriptors[num_commands - 1][0], STDIN_FILENO);
            // tokenize the commands
            char **pipe_use_command = tokenize(pipe_commands[i], 
                            (char *) WORD_SEPARATORS, (char *) SPECIAL_CHARS);
            posix_spawn(&pid, pipe_path[i], &actions, NULL, 
                                                    pipe_use_command, environ);
            if (!strcmp(words[total - 1], ">")) {
                fclose(output_file);
            }
            free_tokens(pipe_use_command);
            close(pipe_file_descriptors[num_commands - 1][0]);
        } else {
            // close unused ends of all the pipes
            for (int v = 0; v < num_commands; v++) {
                if (v != i - 1) {
                    posix_spawn_file_actions_addclose(&actions, 
                                                pipe_file_descriptors[v][0]);
                }
                if (v != i) {
                    posix_spawn_file_actions_addclose(&actions, 
                                                pipe_file_descriptors[v][1]);
                }
            }
            // set the output place as write end of the current pipe
            // set the input place as read end of the previous pipe
            posix_spawn_file_actions_adddup2(&actions, 
                                pipe_file_descriptors[i - 1][0], STDIN_FILENO);
            posix_spawn_file_actions_adddup2(&actions, 
                                    pipe_file_descriptors[i][1], STDOUT_FILENO);
            // tokenize the commands
            char **pipe_use_command = tokenize(pipe_commands[i], 
                            (char *) WORD_SEPARATORS, (char *) SPECIAL_CHARS);
            posix_spawn(&pid, pipe_path[i], &actions, NULL, 
                                                    pipe_use_command, environ);
            free_tokens(pipe_use_command);
            close(pipe_file_descriptors[i - 1][0]);
            close(pipe_file_descriptors[i][1]);
        }   
    }
    // get exit status and print out
    int exit_status;
    if (waitpid(pid, &exit_status, 0) == -1) {
        perror("waitpid");
        exit(1);
    }
    printf("%s exit status = %d\n", pipe_path[num_commands], exit_status / 256);

    // free the list of file actions
    posix_spawn_file_actions_destroy(&actions);
    // free the list of commands
    for (int i = 0; i < total + 1; i++) {
        free(pipe_commands[i]);
    }
    // free the list of paths
    for (int i = 0; i < num_commands + 1; i++) {
        free(pipe_path[i]);
    }
}
