#include "log_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Helper function to get current timestamp
static void get_timestamp(char *buffer, size_t size)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

// Original logging function (keep for backward compatibility)
void log_to_file(const char *log_content, const char *timestamp)
{
    FILE *file = fopen("log.txt", "a");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open log file\n");
        return;
    }

    fprintf(file, "%s - %s\n", timestamp, log_content);
    fclose(file);
}

// Enhanced logging function for requests
void log_request(const char *request_type, const char *user_info, const char *details)
{
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));

    FILE *file = fopen("log.txt", "a");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open log file\n");
        return;
    }

    // Print to console for demo
    printf("\n[%s] ===== REQUEST: %s =====\n", timestamp, request_type);
    if (user_info && strlen(user_info) > 0)
    {
        printf("  User: %s\n", user_info);
    }
    if (details && strlen(details) > 0)
    {
        printf("  Details: %s\n", details);
    }

    // Write to file
    fprintf(file, "\n[%s] REQUEST: %s\n", timestamp, request_type);
    if (user_info && strlen(user_info) > 0)
    {
        fprintf(file, "  User: %s\n", user_info);
    }
    if (details && strlen(details) > 0)
    {
        fprintf(file, "  Details: %s\n", details);
    }

    fclose(file);
}

// Enhanced logging function for responses
void log_response(const char *request_type, const char *status, const char *details)
{
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));

    FILE *file = fopen("log.txt", "a");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open log file\n");
        return;
    }

    // Print to console for demo
    printf("  Result: %s\n", status);
    if (details && strlen(details) > 0)
    {
        printf("  %s\n", details);
    }
    printf("========================================\n\n");

    // Write to file
    fprintf(file, "  Result: %s\n", status);
    if (details && strlen(details) > 0)
    {
        fprintf(file, "  %s\n", details);
    }
    fprintf(file, "========================================\n");

    fclose(file);
}

// Info logging
void log_info(const char *message)
{
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));

    FILE *file = fopen("log.txt", "a");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open log file\n");
        return;
    }

    printf("[%s] INFO: %s\n", timestamp, message);
    fprintf(file, "[%s] INFO: %s\n", timestamp, message);

    fclose(file);
}

// Error logging
void log_error(const char *message)
{
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));

    FILE *file = fopen("log.txt", "a");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open log file\n");
        return;
    }

    printf("[%s] ERROR: %s\n", timestamp, message);
    fprintf(file, "[%s] ERROR: %s\n", timestamp, message);

    fclose(file);
}