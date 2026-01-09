#ifndef ADMIN_SERVICE_H
#define ADMIN_SERVICE_H

// User approval
char *get_pending_users();
int approve_user(int user_id);
int reject_user(int user_id);

// User management
char *get_all_users();
int admin_add_user(const char *email, const char *password, const char *name, const char *role);
int admin_update_user(int user_id, const char *email, const char *name, const char *role, const char *status);
int admin_delete_user(int user_id);

#endif // ADMIN_SERVICE_H

