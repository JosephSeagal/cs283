#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop() {
    char *cmd_buff;
    int rc = 0;
    cmd_buff_t cmd;
    bool interactive = isatty(STDIN_FILENO);
    if (interactive) {
        printf("%s", SH_PROMPT);
    }
    // TODO IMPLEMENT MAIN LOOP
    while (1) {
        cmd_buff = (char *)malloc(SH_CMD_MAX);
        if (!cmd_buff) {
            fprintf(stderr, "Memory allocation error\n");
            break;
        }
        if (!fgets(cmd_buff, SH_CMD_MAX, stdin)) {
            printf("%s\n", SH_PROMPT);
            free(cmd_buff);
            break;
        }
        cmd_buff[strcspn(cmd_buff, "\r\n")] = '\0';
        // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff
        if (strlen(cmd_buff) == 0) {
            printf("No command entered\n");
            printf("%s", SH_PROMPT);
            free(cmd_buff);
            continue;
        }
        if (build_cmd_buff(cmd_buff, &cmd) != OK) {
            fprintf(stderr, "Error parsing command\n");
            printf("%s", SH_PROMPT);
            free(cmd_buff);
            continue;
        }
        // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
        // the cd command should chdir to the provided directory; if no directory is provided, do nothing
        if (exec_built_in_cmd(&cmd) == BI_EXECUTED) {
            printf("%s", SH_PROMPT);
            free(cmd_buff);
            continue;
        }
        // TODO IMPLEMENT if not built-in command, fork/exec as an external command
        // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"
        if (fork() == 0) {
            execvp(cmd.argv[0], cmd.argv);
            perror("execvp");
            exit(ERR_EXEC_CMD);
        }
        wait(NULL);
        printf("%s", SH_PROMPT);
        free(cmd_buff);
    }
    return OK;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (cmd->argc == 0) {
        return WARN_NO_CMDS;
    }
    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);
    if (cmd_type == BI_CMD_EXIT) {
        exit(0);
    }
    if (cmd_type == BI_CMD_CD) {
        if (cmd->argc < 2) {
            return BI_EXECUTED;
        }
        if (chdir(cmd->argv[1]) != 0) {
            perror("cd");
            return ERR_EXEC_CMD;
        }
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, "exit") == 0) {
        return BI_CMD_EXIT;
    }
    if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    }
    return BI_NOT_BI;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    if (!cmd_line || !cmd_buff) {
        return ERR_MEMORY;
    }
    memset(cmd_buff, 0, sizeof(*cmd_buff));
    cmd_buff->_cmd_buffer = strdup(cmd_line);
    if (!cmd_buff->_cmd_buffer) {
        return ERR_MEMORY;
    }
    char *token = cmd_buff->_cmd_buffer;
    char *start;
    int argc = 0;
    while (*token) {
        while (isspace((unsigned char)*token)) {
            token++;
        }
        if (*token == '\0') {
            break;
        }
        if (*token == '"') {
            token++;
            start = token;
            while (*token && *token != '"') {
                token++;
            }
            if (*token == '"') {
                *token++ = '\0';
            }
        } else {
            start = token;
            while (*token && !isspace((unsigned char)*token)) {
                token++;
            }
            if (*token) {
                *token++ = '\0';
            }
        }
        if (argc >= CMD_ARGV_MAX - 1) {
            fprintf(stderr, "error: too many arguments\n");
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        cmd_buff->argv[argc++] = start;
    }
    cmd_buff->argv[argc] = NULL;
    cmd_buff->argc = argc;
    return (argc > 0) ? OK : WARN_NO_CMDS;
}