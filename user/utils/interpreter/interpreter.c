#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../../include/dcac.h"

#define cmd_add_uname "add_uname"
#define cmd_add "add"
#define cmd_drop "drop"
#define cmd_mask "mask"
#define cmd_lock "lock"
#define cmd_unlock "unlock"

#define cmd_set_def_rd "set_def_rd"
#define cmd_set_def_wr "set_def_wr"
#define cmd_set_def_ex "set_def_ex"
#define cmd_set_def_md "set_def_md"

#define cmd_set_file_rd "set_file_rd"
#define cmd_set_file_wr "set_file_wr"
#define cmd_set_file_ex "set_file_ex"
#define cmd_set_file_md "set_file_md"

#define cmd_set_file_rd_reset "set_file_rd_reset"
#define cmd_set_file_wr_reset "set_file_wr_reset"
#define cmd_set_file_ex_reset "set_file_ex_reset"
#define cmd_set_file_md_reset "set_file_md_reset"

#define cmd_fork "fork"


char cmd[32];
char spaces[32];
char operand[4096];
char operand2[4096];

#define MAX_FILES_TO_RESET 256

int reset_files_cnt = 0;
char *reset_files[MAX_FILES_TO_RESET];
void *reset_attrs[MAX_FILES_TO_RESET];
int reset_attr_lens[MAX_FILES_TO_RESET];

int save_file_attr(char *file) {
    if (reset_files_cnt >= MAX_FILES_TO_RESET)
        return -1;
    reset_files[reset_files_cnt] = malloc(strlen(file) + 1);
    if (reset_files[reset_files_cnt] == NULL)
        return -1;
    reset_attrs[reset_files_cnt] = malloc(4096);
    if (reset_attrs[reset_files_cnt] == NULL) {
        free(reset_files[reset_files_cnt]);
        return -1;
    }
    strcpy(reset_files[reset_files_cnt], file);
    reset_attr_lens[reset_files_cnt] = getxattr(file,
            "security.dcac.pm", reset_attrs[reset_files_cnt],
            4096);
    reset_files_cnt++;
    return 0;
}

int process_line(char *line, char *prog)
{
    int result = 0;
    cmd[0] = 0;
    operand[0] = 0;

    int len = strlen(line);
    if (line[len - 1] =='\n')
        line[len - 1] = 0;
    sscanf(line, "%[^ ]", cmd);
    sscanf(line, "%[^ \n]%[ ]%[^ \n]%[ ]%[^ \n]%[ \n]", cmd,
            spaces, operand, spaces, operand2, spaces);
    
    printf("CMD %d %s, OPND %d %s\n", (int)strlen(cmd), cmd,
            (int)strlen(operand), operand);
    if (strcmp(cmd_add_uname, cmd) == 0) {
        result = dcac_add_uname_attr(DCAC_ADDMOD);
    }
    else if (strcmp(cmd_add, cmd) == 0) {
        result = dcac_add_any_attr(operand, DCAC_ADDMOD);
    }
    else if (strcmp(cmd_drop, cmd) == 0) {
        close(dcac_get_attr_fd(operand));
    }
    else if (strcmp(cmd_mask, cmd) == 0) {
        int mask = strtol(operand, NULL, 8);
        dcac_set_mask(mask);
        printf("mask: %o\n", dcac_get_mask());
    }
    else if (strcmp(cmd_lock, cmd) == 0) {
        dcac_lockdown();
    }
    else if (strcmp(cmd_unlock, cmd) == 0) {
        dcac_unlock();
    }
    else if (strcmp(cmd_set_def_rd, cmd) == 0) {
        result = dcac_set_def_rdacl(operand);
    }
    else if (strcmp(cmd_set_def_wr, cmd) == 0) {
        result = dcac_set_def_wracl(operand);
    }
    else if (strcmp(cmd_set_def_ex, cmd) == 0) {
        result = dcac_set_def_exacl(operand);
    }
    else if (strcmp(cmd_set_def_md, cmd) == 0) {
        result = dcac_set_def_mdacl(operand);
    }
    else if (strcmp(cmd_set_file_rd, cmd) == 0) {
        result = dcac_set_file_rdacl(operand, operand2);
    }
    else if (strcmp(cmd_set_file_wr, cmd) == 0) {
        result = dcac_set_file_wracl(operand, operand2);
    }
    else if (strcmp(cmd_set_file_ex, cmd) == 0) {
        result = dcac_set_file_exacl(operand, operand2);
    }
    else if (strcmp(cmd_set_file_md, cmd) == 0) {
        result = dcac_set_file_mdacl(operand, operand2);
    }
    else if (strcmp(cmd_set_file_rd_reset, cmd) == 0) {
        result = save_file_attr(operand);
        if (result == 0)
            result = dcac_set_file_rdacl(operand, operand2);
    }
    else if (strcmp(cmd_set_file_wr_reset, cmd) == 0) {
        result = save_file_attr(operand);
        if (result == 0)
            result = dcac_set_file_wracl(operand, operand2);
    }
    else if (strcmp(cmd_set_file_ex_reset, cmd) == 0) {
        result = save_file_attr(operand);
        if (result == 0)
            result = dcac_set_file_exacl(operand, operand2);
    }
    else if (strcmp(cmd_set_file_md_reset, cmd) == 0) {
        result = save_file_attr(operand);
        if (result == 0)
            result = dcac_set_file_mdacl(operand, operand2);
    }
    else if (strcmp(cmd_fork, cmd) == 0) {
        return fork();
    }
    else return -1;

    printf("%s %s : - %s\n", cmd, operand, result >= 0 ? "success" : "failed");
    return -1;
}

int main(int argc, char *argv[]) {
    FILE *file;
    char *prog;
    char line[256];
    int pid, status;
    int i;

    if (argc < 2) {
        printf("ERROR: no script specified.\n");
        return -1;
    }

    file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("ERROR: cannot open %s.", argv[0]);
        return -1;
    }
    if (argc > 2)
        prog = argv[2];
    else
        prog = "/bin/bash";

    while (1) {
        fgets(line, sizeof(line), file);
        if (feof(file))
            break;
        pid = process_line(line, prog);
        if (pid > 0) {
            waitpid(pid, &status, 0);
            break;
        }
    }
    fclose(file);

    if (pid == 0) {
        system(prog);
        return 0;
    }
    if (pid < 0)
        system(prog);


    for (i = 0; i < reset_files_cnt; i++) {
        if (reset_attr_lens[i] > 0) {
            setxattr(reset_files[i], "security.dcac.pm", reset_attrs[i], reset_attr_lens[i], 0);
        } else {
            removexattr(reset_files[i], "security.dcac.pm");
        }
    }
}
