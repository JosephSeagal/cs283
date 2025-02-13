#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist) {
    if (cmd_line == NULL || strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }
    memset(clist, 0, sizeof(command_list_t));
    char *current_cmd = strtok(cmd_line, "|");
    while (current_cmd != NULL) {
        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }
        while (*current_cmd == ' ') {
            current_cmd++;
        }
        char *arg_start = strchr(current_cmd, ' ');
        if (arg_start) {
            *arg_start = '\0';
            arg_start++;
            while (*arg_start == ' ') {
                arg_start++;
            }
            strncpy(clist->commands[clist->num].args, arg_start, ARG_MAX - 1);
            int arg_length = strlen(clist->commands[clist->num].args);
            while (arg_length > 0 && clist->commands[clist->num].args[arg_length - 1] == ' ') {
                clist->commands[clist->num].args[arg_length - 1] = '\0';
                arg_length--;
            }
        } else {
            clist->commands[clist->num].args[0] = '\0';
        }
        strncpy(clist->commands[clist->num].exe, current_cmd, EXE_MAX - 1);
        clist->num++;
        current_cmd = strtok(NULL, "|");
    }
    return OK;
}