// sys_log.c

#include "sys_log.h"
#include "../lib/string.h"
#include "../lib/stdio.h"
#include "stddef.h"
#include "types.h"


static char log_buffer[LOG_BUFFER_SIZE];
static size_t log_start = 0; // start of the buffer (first message)
static size_t log_end = 0;   // end of the buffer (last message)

static size_t log_len = 0;   // bytes used 


static size_t advance_index(size_t idx, size_t amount) {
    idx += amount;
    if (idx >= LOG_BUFFER_SIZE) idx -= LOG_BUFFER_SIZE;
    return idx;
}
const char* log_level_to_str(LogLevel level){
    switch (level) {
        case LOG_INFO: return "[INFO] ";
        case LOG_ERROR: return "[ERROR] ";
        case LOG_CREATED: return "[CREATED] ";
        case LOG_DELETED: return "[DELETED] ";
        case LOG_DEBUG: return "[DEBUG] ";
        default: return "[UNKNOWN] ";
    }
}
void log_write(LogLevel level, const char* msg) {
    const char* prefix = log_level_to_str(level);
    size_t prefix_len = strnlen(prefix, LOG_MSG_MAX_LEN);
    size_t msg_len = strnlen(msg, LOG_MSG_MAX_LEN - prefix_len);

    if (prefix_len + msg_len > LOG_MSG_MAX_LEN) {
        msg_len = LOG_MSG_MAX_LEN - prefix_len;
    }

    size_t total_len = prefix_len + msg_len;

    // Clean up space if needed
    while (log_len + total_len + 1 > LOG_BUFFER_SIZE) {
        while (log_buffer[log_start] != '\0') {
            log_start = advance_index(log_start, 1);
            log_len--;
        }
        log_start = advance_index(log_start, 1);
        log_len--;
    }

    // Write prefix
    for (size_t i = 0; i < prefix_len; i++) {
        log_buffer[log_end] = prefix[i];
        log_end = advance_index(log_end, 1);
    }

    // Write message
    for (size_t i = 0; i < msg_len; i++) {
        log_buffer[log_end] = msg[i];
        log_end = advance_index(log_end, 1);
    }

    // Null terminator
    log_buffer[log_end] = '\0';
    log_end = advance_index(log_end, 1);

    log_len += total_len + 1;
}


void log_dump(void) {
    size_t idx = log_start;
    while (idx != log_end) {
        if (log_buffer[idx] == '\0') {
            printf("\n");
        } else {
            char buf[2] = { log_buffer[idx], '\0' };
            printf("%s", buf);
        }
        idx = advance_index(idx, 1);
    }
}
