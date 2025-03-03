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
    char cmd_buff[SH_CMD_MAX];
    command_list_t cmd_list;
    while (1) {
        if (!fgets(cmd_buff, sizeof(cmd_buff), stdin)) {
            break;
        }
        cmd_buff[strcspn(cmd_buff, "\r\n")] = '\0';
        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            break;
        }
        if (build_cmd_list(cmd_buff, &cmd_list) == OK) {
            execute_pipeline(&cmd_list);
        }
        printf("%s", SH_PROMPT);
        printf("%s", SH_PROMPT);
        break;
    }
    return OK;
}

void handle_redirection(cmd_buff_t *cmd) {
    if (cmd->infile) {
        int fd_in = open(cmd->infile, O_RDONLY);
        if (fd_in < 0) {
            perror("open infile");
            _exit(ERR_EXEC_CMD);
        }
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    }
    if (cmd->outfile) {
        int fd_out = open(cmd->outfile, O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC), 0644);
        if (fd_out < 0) {
            perror("open outfile");
            _exit(ERR_EXEC_CMD);
        }
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
    }
}

int execute_pipeline(command_list_t *clist) {
    if (clist->num < 1) {
        return WARN_NO_CMDS;
    }
    if (clist->num == 1) {  
        cmd_buff_t *cmd = &clist->commands[0];
        if (match_command(cmd->argv[0]) != BI_NOT_BI) {
            exec_built_in_cmd(cmd);
            return OK;
        }
        pid_t pid = fork();
        if (pid == 0) {  
            handle_redirection(cmd);
            execvp(cmd->argv[0], cmd->argv);
            perror("execvp");
            _exit(ERR_EXEC_CMD);
        }
        waitpid(pid, NULL, 0);
        return OK;
    }
    int pipes[clist->num - 1][2];
    pid_t pids[clist->num];
    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return ERR_MEMORY;
        }
    }
    for (int i = 0; i < clist->num; i++) {
        cmd_buff_t *cmd = &clist->commands[i];
        pids[i] = fork();
        if (pids[i] == 0) {
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i < clist->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            handle_redirection(cmd);
            execvp(cmd->argv[0], cmd->argv);
            perror("execvp");
            _exit(ERR_EXEC_CMD);
        }
    }
    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    for (int i = 0; i < clist->num; i++) {
        waitpid(pids[i], NULL, 0);
    }
    return OK;
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

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);
    if (cmd_type == BI_CMD_EXIT) {
        return BI_EXECUTED;
    }
    if (cmd_type == BI_CMD_CD) {
        if (cmd->argc < 2) {
            fprintf(stderr, "cd: missing operand\n");
        } else if (chdir(cmd->argv[1]) != 0) {
            perror("cd failed");
        }
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    memset(clist, 0, sizeof(*clist));
    int count = 0;
    char *saveptr = NULL;
    char *token = strtok_r(cmd_line, PIPE_STRING, &saveptr);
    while (token) {
        while (isspace((unsigned char)*token)) {
            token++;
        }
        if (count >= CMD_MAX) {
            fprintf(stderr, CMD_ERR_PIPE_LIMIT, CMD_MAX);
            return ERR_TOO_MANY_COMMANDS;
        }
        clist->commands[count]._cmd_buffer = strdup(token);
        build_cmd_buff(clist->commands[count]._cmd_buffer, &clist->commands[count]);
        count++;
        token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }
    clist->num = count;
    if (count == 0) {
        return WARN_NO_CMDS;
    }
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    memset(cmd_buff, 0, sizeof(*cmd_buff));
    cmd_buff->_cmd_buffer = strdup(cmd_line);
    char *token = strtok(cmd_buff->_cmd_buffer, " ");
    int argc = 0;
    while (token) {
        if (strcmp(token, "<") == 0) {
            if ((token = strtok(NULL, " "))) {
                cmd_buff->infile = token;
            }
            token = strtok(NULL, " ");
            continue;
        }
        if (strcmp(token, ">") == 0 || strcmp(token, ">>") == 0) {
            cmd_buff->append = (strcmp(token, ">>") == 0);
            if ((token = strtok(NULL, " "))) {
                cmd_buff->outfile = token;
            }
            token = strtok(NULL, " ");
            continue;
        }
        if (argc >= CMD_ARGV_MAX - 1) {
            fprintf(stderr, "error: too many arguments\n");
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        cmd_buff->argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    cmd_buff->argv[argc] = NULL;
    cmd_buff->argc = argc;
    return OK;
}