#include "daemonRunner.h"
#include "daemonProcess.h"
#include <unistd.h>
#include "thirdparty/log/log.h"
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/reboot.h>
#include <signal.h>
#include <assert.h>
#include <sys/wait.h>

static SubProcess subProcess[2];
static int isRunning = 0;

static int crushTime = 0;

static void sigHandler(int sig){
    assert(sig == SIGTERM);
    isRunning = 0;
}

void daemonRun(){
    if(daemon(0,1)<0){
        log_error("daemon failed");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    open("/dev/null", O_RDWR);
    open(LOG_FILE, O_RDWR | O_CREAT, 0644);
    open(LOG_FILE, O_RDWR | O_CREAT, 0644);

    signal(SIGTERM, sigHandler);

    subProcessInit(&subProcess[0], "app");
    subProcessStart(&subProcess[0]);
    subProcessInit(&subProcess[1], "ota");
    subProcessStart(&subProcess[1]);
    
    while (isRunning)
    {
        int rc = waitpid(-1, NULL, WNOHANG);
        if(rc < 0){
            log_error("waitpid failed");
            continue;
        }
        if(rc == 0){
            // 等待100ms后继续
            usleep(100000);
            continue;
        }
        crushTime++;
        if(crushTime > 10){
            log_error("the version is unstable, crush");
            int fd = open(ERROR_LOG_FILE, O_RDWR | O_CREAT, 0644);
            close(fd);
        }

        for(int i=0; i<2; i++){
            if(subProcess[i].pid == rc){
                log_warn("sub process %s is dead, restart it", subProcess[i].name);
                subProcessStart(&subProcess[i]);
            }
        }
    }
    for(int i=0; i<2; i++){
        subProcessStop(&subProcess[i]);
    }
}