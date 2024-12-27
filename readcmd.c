#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // For SIZE_MAX
#include <errno.h>
#include "readcmd.h"

static void memory_error(void) {
    errno = ENOMEM;
    perror("Memory allocation error");
    exit(EXIT_FAILURE);
}

static void *xmalloc(size_t size) {
    void *p = malloc(size);
    if (!p) memory_error();
    return p;
}

static void *xrealloc(void *ptr, size_t size) {
    void *p = realloc(ptr, size);
    if (!p) memory_error();
    return p;
}

/* Read a line from standard input */
static char *readline(void) {
    size_t buf_len = 16;
    char *buf = xmalloc(buf_len * sizeof(char));

    if (fgets(buf, buf_len, stdin) == NULL) {
        free(buf);
        return NULL;
    }

    do {
        size_t l = strlen(buf);
        if (l > 0 && buf[l - 1] == '\n') {
            buf[l - 1] = '\0';
            return buf;
        }
        if (buf_len >= (SIZE_MAX / 2)) memory_error();
        buf_len *= 2;
        buf = xrealloc(buf, buf_len * sizeof(char));
        if (fgets(buf + l, buf_len - l, stdin) == NULL) return buf;
    } while (1);
}

/* Split the input line into words */
static char **split_in_words(char *line) {
    char *cur = line;
    char **tab = NULL;
    size_t l = 0;
    char c;

    while ((c = *cur) != 0) {
        char *w = NULL;
        char *start;
        switch (c) {
        case ' ':
        case '\t':
            cur++;
            break;
        case '<':
        case '>':
        case '|':
        case '&':
            w = xmalloc(2 * sizeof(char));
            w[0] = c;
            w[1] = '\0';
            cur++;
            break;
        default:
            start = cur;
            while (*cur && !strchr(" \t<>|&", *cur)) cur++;
            w = xmalloc((cur - start + 1) * sizeof(char));
            strncpy(w, start, cur - start);
            w[cur - start] = '\0';
        }
        if (w) {
            tab = xrealloc(tab, (l + 1) * sizeof(char *));
            tab[l++] = w;
        }
    }
    tab = xrealloc(tab, (l + 1) * sizeof(char *));
    tab[l] = NULL;
    return tab;
}

static void freeseq(char ***seq) {
    for (int i = 0; seq[i] != NULL; i++) {
        char **cmd = seq[i];
        for (int j = 0; cmd[j] != NULL; j++) free(cmd[j]);
        free(cmd);
    }
    free(seq);
}

static void freecmd(struct cmdline *s) {
    if (s->in) free(s->in);
    if (s->out) free(s->out);
    if (s->seq) freeseq(s->seq);
}

struct cmdline *readcmd(void) {
    static struct cmdline *static_cmdline = NULL;
    struct cmdline *s = static_cmdline;
    char *line;
    char **words;
    char ***seq = NULL;
    size_t seq_len = 0;

    line = readline();
    if (line == NULL) {
        if (s) {
            freecmd(s);
            free(s);
        }
        return static_cmdline = NULL;
    }

    words = split_in_words(line);
    free(line);

    if (!s)
        static_cmdline = s = xmalloc(sizeof(struct cmdline));
    else
        freecmd(s);

    s->err = NULL;
    s->in = NULL;
    s->out = NULL;
    s->backgrounded = NULL;
    s->seq = NULL;

    size_t cmd_len = 0;
    char **cmd = xmalloc(sizeof(char *));
    cmd[0] = NULL;

    for (int i = 0; words[i] != NULL; i++) {
        char *w = words[i];

        switch (w[0]) {
        case '&':
            if (s->backgrounded) {
                s->err = "Multiple '&' not allowed";
                goto error;
            }
            s->backgrounded = "&";
            break;
        case '<':
            if (s->in) {
                s->err = "Multiple input redirections not allowed";
                goto error;
            }
            s->in = strdup(words[++i]);
            break;
        case '>':
            if (s->out) {
                s->err = "Multiple output redirections not allowed";
                goto error;
            }
            s->out = strdup(words[++i]);
            break;
        case '|':
            if (cmd_len == 0) {
                s->err = "Pipe with no command";
                goto error;
            }
            seq = xrealloc(seq, (seq_len + 2) * sizeof(char **));
            seq[seq_len++] = cmd;
            seq[seq_len] = NULL;

            cmd = xmalloc(sizeof(char *));
            cmd[0] = NULL;
            cmd_len = 0;
            break;
        default:
            cmd = xrealloc(cmd, (cmd_len + 2) * sizeof(char *));
            cmd[cmd_len++] = strdup(w);
            cmd[cmd_len] = NULL;
        }
    }

    if (cmd_len > 0) {
        seq = xrealloc(seq, (seq_len + 2) * sizeof(char **));
        seq[seq_len++] = cmd;
        seq[seq_len] = NULL;
    } else {
        free(cmd);
    }

    free(words);
    s->seq = seq;
    return s;

error:
    free(words);
    for (int i = 0; cmd[i] != NULL; i++) free(cmd[i]);
    free(cmd);
    freecmd(s);
    return NULL;
}
