#include "action.h"


Action* Action_new(char mod, String* command) {
    Action* action = malloc(sizeof(Action));
    action->modifier = mod;
    action->command = command;
    return action;
}

void Action_free(Action* self) {
    String_free(self->command);
    free(self);
}

int Action_exec(Action* self) {
    printf("%s\n", self->command->str);
    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "FATAL: Failed to fork in Action_exec\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        // parent
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            fprintf(stderr, "waitpid failed in Action_exec\n");
            exit(EXIT_FAILURE);
        }
        if (WIFEXITED(status)) { // got an exit code from child
            int child_exit_code = WEXITSTATUS(status);
            return child_exit_code;
        } else { // did not get an exit code from child
            return EXIT_FAILURE;
        }
    } else if (pid == 0) {
        // child
        char* shell = getenv("SHELL");
        if (shell == NULL)
            shell = DEFAULT_SHELL;
        execl(shell, shell, "-c", self->command->str, (char*)NULL); // this replaces this child with the shell
        exit(EXIT_FAILURE); // if we get to here execl failed
    }
    return EXIT_FAILURE;
}
