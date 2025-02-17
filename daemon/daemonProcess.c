#include "daemonProcess.h"
#include "thirdparty/log/log.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int subProcessInit(SubProcess *subProcess, char *name){
    subProcess->name = malloc(sizeof(char) * (strlen(name) + 1));
    if(subProcess->name == NULL){
        log_error("Memory allocation failed for subProcess->name");
        return -1;
    }
    subProcess->args = malloc(sizeof(char*) * 3);
    if(subProcess->args == NULL){
        log_error("Memory allocation failed for subProcess->args");
        free(subProcess->name);
        return -1;
    }

    strcpy(subProcess->name, name);
    subProcess->pid = -1;

    subProcess->args[0] = PROGRAM_NAME;
    subProcess->args[1] = subProcess->name;
    subProcess->args[2] = NULL;
    return 0;
}

int subProcessStart(SubProcess *subProcess){
    int subpid = fork();
    if(subpid < 0){
        log_error("Fork failed for subProcess %s", subProcess->name);
        return -1;
    }
    if(subpid == 0){
        if (execve(PROGRAM_NAME, subProcess->args, __environ)<0){
            log_error("Execve failed for subProcess %s", subProcess->name);
            exit(EXIT_FAILURE);
        }
        log_info("Execve failed for subProcess %s", subProcess->name);
    }
    return 0;
}

int subProcessStop(SubProcess *subProcess){
    if(kill(subProcess->pid, SIGTERM) < 0){
        log_error("Failed to send SIGTERM to subProcess %s", subProcess->name);
        return -1;
    }
    if(waitpid(subProcess->pid, NULL, 0) < 0){
        log_error("Failed to wait for subProcess %s", subProcess->name);
        return -1;
    }
    log_error("SubProcess %s stopped", subProcess->name);
    return 0;
}
