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

    char *command[3] = {"/bin/cat", "/bin/grep", "/usr/bin/wc"};
    char *grep_argv[] = {"grep", "what", NULL};
    char *cat_argv[] = {"/bin/cat", NULL};
    char *wc_argv[] = {"wc", NULL};

    for (int i = 0; i < 3; i++) {
        posix_spawn_file_actions_init(&actions);
        if (i == 0) {
            
            // get file path
            char file_path[2000000] = "hello.txt";
            FILE *file;
            if ((file = fopen(file_path, "r")) == NULL) {
                fprintf(stderr, "No such file or directory\n");
                return 0;
            };
            int fd_in = fileno(file);
            posix_spawn_file_actions_adddup2(&actions, fd_in, 0);

            for (int j = 1; j < 2; j++) {
                posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[0][0]);
                posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[j][0]);
                posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[j][1]);
            }
            posix_spawn_file_actions_adddup2(&actions, pipe_file_descriptors[0][1], 1);
            posix_spawn(&pid, command[i], &actions, NULL, cat_argv, environ);
            close(pipe_file_descriptors[0][1]);
        } else if (i == 2) {
            

            // get file path
            char file_path1[2000000] = "whatisthisss.txt";
            FILE *file1;
            if ((file1 = fopen(file_path1, "w")) == NULL) {
                fprintf(stderr, "No such file or directory\n");
                return 0;
            };
            int fd_out = fileno(file1);
            posix_spawn_file_actions_adddup2(&actions, fd_out, 1);

            for (int k = 0; k < 1; k++) {
                posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[0][1]);
                posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[k][0]);
                posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[k][1]);
            }
            posix_spawn_file_actions_adddup2(&actions, pipe_file_descriptors[1][0], 0);
            posix_spawn(&pid, command[i], &actions, NULL, wc_argv, environ);
            fclose(file1);
            close(pipe_file_descriptors[1][0]);
        } else {
            for (int v = 0; v < 2; v++) {
                if (v != i - 1) {
                    posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[v][0]);
                }
                if (v != i) {
                    posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[v][1]);
                }
            }
            posix_spawn_file_actions_adddup2(&actions, pipe_file_descriptors[i - 1][0], 0);
            posix_spawn_file_actions_adddup2(&actions, pipe_file_descriptors[i][1], 1);
            posix_spawn(&pid, command[i], &actions, NULL, grep_argv, environ);
            close(pipe_file_descriptors[i - 1][0]);
            close(pipe_file_descriptors[i][1]);
        }   
    }

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

