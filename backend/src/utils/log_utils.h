#ifndef LOG_UTILS_H
#define LOG_UTILS_H

// Original logging function
void log_to_file(const char *log_content, const char *timestamp);

// Enhanced logging functions for demo
void log_request(const char *request_type, const char *user_info, const char *details);
void log_response(const char *request_type, const char *status, const char *details);
void log_info(const char *message);
void log_error(const char *message);

#endif // LOG_UTILS_H