/* Main Entry Point for MiniShell Project */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include "readcmd.h"
#include "job.h"

/* ANSI color codes for terminal */
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"


/* Global Variables */
pid_t pid = 0;       // Current process
bool ctrl_Z = false; // Flag for Ctrl+Z
bool ctrl_c = false; // Flag for Ctrl+C
pid_t bg_pid = 0;    // Background process ID
bool bg_stop = false;// Background process stopped

/* Signal Handlers */
void handler_SIGCHLD() {
    int c_pid, wstatus;
    do {
        c_pid = waitpid(-1, &wstatus, WNOHANG | WUNTRACED | WCONTINUED);
        if (c_pid > 0) {
            if (WIFEXITED(wstatus) || WIFSIGNALED(wstatus)) {
                delete_job(c_pid);
            } else if (WIFSTOPPED(wstatus)) {
                update_job_state(c_pid, SUSPENDED);
            } else if (WIFCONTINUED(wstatus)) {
                update_job_state(c_pid, BG);
            }
        }
    } while (c_pid > 0);
}

void handler_SIGTSTP() {
    if (bg_pid != 0) {
        kill(bg_pid, SIGSTOP);
        update_job_state(bg_pid, SUSPENDED);
        bg_stop = true;
    }
}

void handler_SIGINT() {
    if (bg_pid != 0) {
        kill(bg_pid, SIGKILL);
        delete_job(bg_pid);
        bg_stop = true;
    }
}

/* Main Function */
int main(int argc, char *argv[]) {
    struct cmdline *cmdl;

    // Signal handling
    signal(SIGCHLD, handler_SIGCHLD);
    signal(SIGTSTP, handler_SIGTSTP);
    signal(SIGINT, handler_SIGINT);

    while (1) {
        ctrl_Z = false;
        ctrl_c = false;

        printf("arhayout@" ANSI_COLOR_RED "Minishell" ANSI_COLOR_RESET ":~$ ");
        fflush(stdout);

        cmdl = readcmd();
        if (cmdl == NULL || cmdl->seq == NULL) continue;

        // Internal commands
        if (strcmp(cmdl->seq[0][0], "exit") == 0) {
            exit(0);
        } else if (strcmp(cmdl->seq[0][0], "cd") == 0) {
            chdir(cmdl->seq[0][1] ? cmdl->seq[0][1] : getenv("HOME"));
        } else if (strcmp(cmdl->seq[0][0], "jobs") == 0) {
            list_jobs();
        } else if (strcmp(cmdl->seq[0][0], "fg") == 0) {
            fg_job(atoi(cmdl->seq[0][1]));
        } else if (strcmp(cmdl->seq[0][0], "bg") == 0) {
            bg_job(atoi(cmdl->seq[0][1]));
        } else {
            // External commands
            pid = fork();
            if (pid == 0) {
                // Redirections
                if (cmdl->in) {
                    int fd_in = open(cmdl->in, O_RDONLY);
                    dup2(fd_in, STDIN_FILENO);
                    close(fd_in);
                }
                if (cmdl->out) {
                    int fd_out = open(cmdl->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    dup2(fd_out, STDOUT_FILENO);
                    close(fd_out);
                }

                execvp(cmdl->seq[0][0], cmdl->seq[0]);
                perror("Command execution failed");
                exit(EXIT_FAILURE);
            } else {
                if (!cmdl->backgrounded) {
                    bg_pid = pid;
                    waitpid(pid, NULL, 0);
                } else {
                    add_job(pid, cmdl->seq[0][0], BG);
                }
            }
        }
    }
    return 0;
}
