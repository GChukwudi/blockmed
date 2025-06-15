#include "log.h"

static FILE *log_file = NULL;

// function to initialize logging
void init_logging(void) {
    log_file = fopen("data/access.log", "a");
    if (!log_file) {
        fprintf(stderr, "Error opening log file\n");
    }
}

// function to log operational events
void log_operation(log_level_t level, const char *user, const char *operation) {
    if (!log_file) return;

    char timestamp[20];
    // Get the current timestamp
    get_timestamp(timestamp);

    const char *level_str[] = {
        "INFO",
        "WARNING",
        "ERROR",
        "SECURITY"
    };

    // Log the operation with timestamp, level, user, and operation details
    fprintf(log_file, "[%s] [%s] User: %s, Operation: %s\n", 
            timestamp, level_str[level], user ? user : "Unknown", operation);
    fflush(log_file);
}

// function to log security events
void log_security_event(const char *user, const char *event) {
    log_operation(LOG_DEBUG, user, event);
}
