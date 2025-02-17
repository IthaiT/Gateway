#ifndef __DAEMON_RUNNER_H__
#define __DAEMON_RUNNER_H__

#define LOG_FILE "/var/log/gateway.log"
#define ERROR_LOG_FILE "/var/log/gateway_error.log"

/**
 * @brief Runs the daemon process.
 */
void daemonRun();

#endif