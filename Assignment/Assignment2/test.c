#include <stdio.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

int main(void) {
    // create a pipe
    int pipe_file_descriptors[2];
    pipe(pipe_file_descriptors);
    
    pid_t pid;
    posix_spawn_file_actions_t actions;
    extern char **environ;
    
    // create a list of file actions to be carried out on spawned process
    /*
    posix_spawn_file_actions_init(&actions);

    posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[0]);
    posix_spawn_file_actions_adddup2(&actions, pipe_file_descriptors[1], 1);

    char *date_argv[] = {"/bin/cat", "hello.txt", NULL};
    posix_spawn(&pid, "/bin/cat", &actions, NULL, date_argv, environ);

    close(pipe_file_descriptors[1]);*/
    

    posix_spawn_file_actions_init(&actions);

    posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[1]);
    posix_spawn_file_actions_adddup2(&actions, pipe_file_descriptors[0], 0);

    char *grep_argv[] = {"sort", NULL};
    posix_spawn(&pid, "/usr/bin/sort", &actions, NULL, grep_argv, environ);

    // close unused read end of pipe
    close(pipe_file_descriptors[0]);

    // create a stdio stream from write-end of pipe
    FILE *f = fdopen(pipe_file_descriptors[1], "w");
    if (f == NULL) {
        perror("fdopen");
        return 1;
    }

    // send some input to the /usr/bin/sort process
    //sort with will print the lines to stdout in sorted order
    fprintf(f, "that sort\n here words\nplease that\nthese\n");

    fclose(f);

    int exit_status;
    if (waitpid(pid, &exit_status, 0) == -1) {
        perror("waitpid");
        return 1;
    }
    printf("exit status was %d\n", exit_status);

    // free the list of file actions
    posix_spawn_file_actions_destroy(&actions);

    return 0;
}