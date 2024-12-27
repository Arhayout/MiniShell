#ifndef JOB_H
#define JOB_H

#include <sys/types.h>

typedef enum { BG, FG, SUSPENDED } state_t;

typedef struct {
    int id;
    pid_t pid;
    state_t state;
    char *cmd_line;
} job_t;

void add_job(pid_t pid, const char *cmd_line, state_t state);
void delete_job(pid_t pid);
void list_jobs();
void update_job_state(pid_t pid, state_t state);
void stop_job(int id);
void bg_job(int id);
pid_t fg_job(int id);

#endif
