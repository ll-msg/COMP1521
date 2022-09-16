#include <stdio.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

int main(void) {
    int pipe_file_descriptors[2][2];
    for (int i = 0; i < 2; i++) {
        pipe(pipe_file_descriptors[i]);
    }
    pid_t pid;
    extern char **environ;
    posix_spawn_file_actions_t actions;
    // first write - put cat result into pipe 0 write end
    posix_spawn_file_actions_init(&actions);

    posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[1][0]);
    posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[1][1]);
    posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[0][0]);
    
    posix_spawn_file_actions_adddup2(&actions, pipe_file_descriptors[0][1], 1);

    char *cat_argv[] = {"/bin/cat", "hello.txt", NULL};
    posix_spawn(&pid, "/bin/cat", &actions, NULL, cat_argv, environ);

    close(pipe_file_descriptors[0][1]);


    // first read - read cat result from pipe 0 read end
    posix_spawn_file_actions_init(&actions);

    posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[1][0]);
    posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[0][1]);

    posix_spawn_file_actions_adddup2(&actions, pipe_file_descriptors[0][0], 0);
    posix_spawn_file_actions_adddup2(&actions, pipe_file_descriptors[1][1], 1);

    char *grep_argv[] = {"grep", "that", NULL};
    posix_spawn(&pid, "/bin/grep", &actions, NULL, grep_argv, environ);

    close(pipe_file_descriptors[0][0]);
    close(pipe_file_descriptors[1][1]);

    // second read - write grep result into pipe 1 write end
    posix_spawn_file_actions_init(&actions);

    posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[0][0]);
    posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[0][1]);
    posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[1][1]);

    posix_spawn_file_actions_adddup2(&actions, pipe_file_descriptors[1][0], 0);

    char *wc_argv[] = {"wc", NULL};
    posix_spawn(&pid, "/usr/bin/wc", &actions, NULL, wc_argv, environ);

    close(pipe_file_descriptors[1][0]);


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