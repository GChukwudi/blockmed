#ifndef AUTH_H
#define AUTH_H

#include "utils.h"

// enum for user roles
typedef enum {
    ROLE_STUDENT,   // @alustudent.com - View only
    ROLE_INTERN,    // @si.alueducation.com - Limited write access
    ROLE_STAFF,    //@alueducation.com - Full access
    ROLE_INVALID
} user_role_t;

// struct for user information
typedef struct {
    char email[MAX_EMAIL_SIZE];
    char password_hash[HASH_SIZE];
    user_role_t role;
} user_t;

// Function prototypes
user_role_t get_role_from_email(const char *email);
int authenticate_user(const char *email, const char *password, user_t *user);
int register_user(const char *email, const char *password);
void hash_password(const char *password, char *hash);
int has_write_permission(user_role_t role);
int has_full_permission(user_role_t role);
const char* role_to_string(user_role_t role);


#endif
