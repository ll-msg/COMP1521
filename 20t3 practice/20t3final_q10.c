#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <spawn.h>
#include <signal.h>
#include <wait.h>
#include <assert.h>
#include <errno.h>

void pdie(char *);
void *srealloc(void *ptr, size_t size);
char *sstrdup(char *s);
void execute_line(int argc, char **args, char *line);

int main(int argc, char *argv[]) {
    if (argc == 1) {
        argc++;
        argv[1] = "echo";        // match xargs which  defaults to `echo`
    }

    char *line = NULL;
    size_t length = 0;
    while (getline(&line, &length, stdin) != -1) {
        execute_line(argc - 1, argv + 1, line);
    }
    free(line);

    return 0;
}


// process a lines of stdin  into command line args for the new process

void execute_line(int argc, char **command_line_args, char *line) {
    char *p = strchr(line, '\n');
    if (p) {
        *p = '\0';
    }
    char **args = srealloc(NULL, (argc + 1) * sizeof (char *));
    for (int a = 0; a < argc; a++) {
        args[a] = command_line_args[a];
    }

    int new_argc = argc  + 1;
    for (char *token = strtok(line, " "); token != NULL; token = strtok(NULL, " ")) {
        args[new_argc - 1] = sstrdup(token);
        new_argc++;
        args = srealloc(args, new_argc * sizeof (char *));
    }
    args[new_argc - 1] = NULL;

    // call the new process with it's processed command line args
    pid_t pid;
    if (posix_spawnp(&pid, args[0], NULL, NULL, args, NULL) != 0) {
        perror(args[0]);
    }

    if (waitpid(pid, NULL, 0) == -1) {
        pdie("waitpid");
    }

    for (int a = argc; a < new_argc - 1; a++) {
        free(args[a]);
    }
    free(args);
}

void *srealloc(void *ptr, size_t size) {
    char *n = realloc(ptr, size);
    if (n == NULL) {
        pdie("realloc");
    }
    return n;
}

char *sstrdup(char *s) {
    char *n = strdup(s);
    if (n == NULL) {
        pdie("strdup");
    }
    return n;
}

void pdie(char *message) {
    perror(message);
    exit(1);
}