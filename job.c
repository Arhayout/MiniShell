#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

/** Color Definitions for Output **/
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

/** Process State Definitions **/
typedef enum {
    BG, // Background
    FG, // Foreground
    SUSPENDED // Suspended
} state_t;

/** Job Structure **/
typedef struct {
    int id;          // Job ID
    pid_t pid;       // Process ID
    state_t state;   // Process State
    char *cmd_line;  // Command Line
} job_t;

/** Job List Structure **/
typedef struct {
    int count;       // Number of active/suspended jobs
    int max_id;      // Maximum job ID
    job_t jobs[100]; // Job list (size can be adjusted)
} job_list_t;

/** Global Variables **/
job_list_t job_list = {0, 0, {}}; // Initialize job list

/** Utility Functions **/
int find_job_index(pid_t pid) {
    for (int i = 0; i < job_list.count; i++) {
        if (job_list.jobs[i].pid == pid) {
            return i;
        }
    }
    return -1;
}

int find_pid_by_id(int id) {
    for (int i = 0; i < job_list.count; i++) {
        if (job_list.jobs[i].id == id) {
            return job_list.jobs[i].pid;
        }
    }
    return -1;
}

void update_job_state(pid_t pid, state_t new_state) {
    int index = find_job_index(pid);
    if (index >= 0) {
        job_list.jobs[index].state = new_state;
    } else {
        printf(ANSI_COLOR_RED "Error: No such process found.\n" ANSI_COLOR_RESET);
    }
}

/** Job Management Functions **/
void add_job(pid_t pid, const char *cmd_line, state_t state) {
    if (job_list.count < 100) {
        job_t *job = &job_list.jobs[job_list.count];
        job->id = ++job_list.max_id;
        job->pid = pid;
        job->state = state;
        job->cmd_line = strdup(cmd_line);
        job_list.count++;
    } else {
        printf(ANSI_COLOR_RED "Error: Job list is full.\n" ANSI_COLOR_RESET);
    }
}

void delete_job(pid_t pid) {
    int index = find_job_index(pid);
    if (index >= 0) {
        free(job_list.jobs[index].cmd_line);
        for (int i = index; i < job_list.count - 1; i++) {
            job_list.jobs[i] = job_list.jobs[i + 1];
        }
        job_list.count--;
    } else {
        printf(ANSI_COLOR_RED "Error: No such process found.\n" ANSI_COLOR_RESET);
    }
}

void list_jobs() {
    printf("ID\tPID\tState\t\tCommand\n");
    for (int i = 0; i < job_list.count; i++) {
        job_t *job = &job_list.jobs[i];
        const char *state_str = (job->state == BG) ? "Background" : (job->state == FG) ? "Foreground" : "Suspended";
        printf("[%d]\t%d\t%s\t%s\n", job->id, job->pid, state_str, job->cmd_line);
    }
}

/** Command Functions **/
void stop_job(int id) {
    int pid = find_pid_by_id(id);
    if (pid > 0) {
        kill(pid, SIGSTOP);
        update_job_state(pid, SUSPENDED);
    } else {
        printf(ANSI_COLOR_RED "Error: No such job ID.\n" ANSI_COLOR_RESET);
    }
}

void bg_job(int id) {
    int pid = find_pid_by_id(id);
    if (pid > 0) {
        kill(pid, SIGCONT);
        update_job_state(pid, BG);
    } else {
        printf(ANSI_COLOR_RED "Error: No such job ID.\n" ANSI_COLOR_RESET);
    }
}

pid_t fg_job(int id) {
    int pid = find_pid_by_id(id);
    if (pid > 0) {
        update_job_state(pid, FG);
        kill(pid, SIGCONT);
        return pid;
    } else {
        printf(ANSI_COLOR_RED "Error: No such job ID.\n" ANSI_COLOR_RESET);
    }
    return -1;
}
