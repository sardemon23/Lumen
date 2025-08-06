
#ifndef SYS_LOG_H
#define SYS_LOG_H

#include <stdbool.h>
typedef enum {
    LOG_INFO,
    LOG_ERROR,
    LOG_CREATED,
    LOG_DELETED,
    LOG_DEBUG
} LogLevel;

#define LOG_BUFFER_SIZE 4096
#define LOG_MSG_MAX_LEN 128

void log_write(LogLevel level,const char* msg);
void log_dump(void);

#endif
