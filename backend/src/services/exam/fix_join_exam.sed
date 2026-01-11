393s|// Already joined, return existing submission_id|// Already has a submission, check status|
395a\        const char *status = row[1];\
\
        // If already submitted, cannot rejoin\
        if (strcmp(status, "submitted") == 0) {\
            mysql_free_result(res);\
            return 0; // Cannot join - already submitted\
        }\
\
        // If still in_progress, allow rejoin
396,397d
