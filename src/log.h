#ifndef LOG_H
#define LOG_H

// pull in necessary headers from utils.h
#include "utils.h"

// enum for logging levels
typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_DEBUG
} log_level_t;


// functions for logging operations and security events
void init_logging(void);
void log_operation(log_level_t level, const char *user,
                   const char *operation);
void log_security_event(const char *user, const char *event);

#endif
