#ifndef __READCMD_H
#define __READCMD_H

/* Reads a command line from standard input.
 * Note:
 * If sufficient memory cannot be allocated for the result,
 * an error message is displayed and exit() is called.
 */

struct cmdline *readcmd(void);

/* Structure returned by readcmd()
 * Useful notes:
 * - readcmd() may return null if the process waiting on readcmd receives a signal.
 * - In successive calls, readcmd() reuses the memory allocated for the cmdline structure it returns.
 */
struct cmdline {
    char *err;          /* If not null: error message to display. 
                         * In this case, other fields are null.
                         */
    char *in;           /* If not null: name of the file to redirect input. */
    char *out;          /* If not null: name of the file to redirect output. */
    char *backgrounded; /* If not null: command running in the background. */
    char ***seq;        /* A command line is a sequence of commands connected by pipes.
                         * Hence,
                         * - A command is an array of strings (char **).
                         *   Each element of this array is indeed a string (char *),
                         *   corresponding to an argument of the command.
                         *   The last pointer of the array is followed by a null pointer,
                         *   which marks the end of the array.
                         * - seq is an array of commands (char ***), each command in this
                         *   array corresponding to an elementary command in the pipeline.
                         *   The last element (char **) of the array is also followed by a null pointer.
                         * - In the special case where an empty line is entered, the pointer referenced
                         *   by seq directly points to NULL (since the array has no elements, the end-of-array
                         *   marker is referenced by seq, which can be written as (*seq == NULL)).
                         */
};

#endif
