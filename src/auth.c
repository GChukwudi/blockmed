#include "auth.h"

user_role_t get_role_from_email(const char *email) {
    if (!email) return ROLE_INVALID;

    if (strstr(email, "@alueducation.com") != NULL) {
        return ROLE_STAFF;
    } else if (strstr(email, "@si.alueducation.com") != NULL) {
        return ROLE_INTERN;
    } else if (strstr(email, "@alustudent.com") != NULL) {
        return ROLE_STUDENT;
    }

    return ROLE_INVALID;
}

void hash_password(const char *password, char *hash) {
    if (!password || !hash) return;

    // Simple password hashing for demonstration purposes
    char salted_password[MAX_PASSWORD_SIZE + 20];
    snprintf(salted_password, sizeof(salted_password), "ALU_SALT_%s_2024", password);

    sha256_hash(salted_password, hash);
}

int authenticate_user(const char *email, const char *password, user_t *user) {
    if (!email || !password || !user) return 0;

    FILE *file = fopen("data/users.csv", "r");
    if (!file) {
        // create the file if it doesn't exist
        file = fopen("data/users.csv", "w");
        if (file) {
            char admin_hash[HASH_SIZE];
            hash_password("admin123", admin_hash);
            fprintf(file, "admin@alueducation.com,%s,STAFF\n", admin_hash);
            fclose(file);
        }
        file = fopen("data/users.csv", "r");
        if (!file) return 0;
    }

    char line[512];
    char stored_email[MAX_EMAIL_SIZE];
    char stored_hash[HASH_SIZE];
    char role_str[20];

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%99[^,],%64[^,],%19s", stored_email, stored_hash, role_str) == 3) {
            if (strcmp(email, stored_email) == 0) {
                char input_hash[HASH_SIZE];
                hash_password(password, input_hash);

                if (strcmp(input_hash, stored_hash) == 0) {
                    strcpy(user->email, email);
                    strcpy(user->password_hash, stored_hash);
                    user->role = get_role_from_email(email);
                    fclose(file);
                    return 1; // Authentication successful
                }
            }
        }
    }
    fclose(file);
    return 0; // Authentication failed
}

int register_user(const char *email, const char *password) {
    if (!email || !password || !is_valid_email(email)) return 0;

    user_role_t role = get_role_from_email(email);
    if (role == ROLE_INVALID) {
        printf("Invalid email domain. Must be @alustudent.com, @si.alueducation.com, or @alueducation.com.\n");
        return 0;
    }

    FILE *file = fopen("data/users.csv", "a");
    if (!file) return 0;

    char hash[HASH_SIZE];
    hash_password(password, hash);

    fprintf(file, "%s,%s,%s\n", email, hash, role_to_string(role));
    fclose(file);

    return 1; // Registration successful
}

int has_write_permission(user_role_t role) {
    return (role == ROLE_INTERN || role == ROLE_STAFF);
}

int has_full_permission(user_role_t role) {
    return (role == ROLE_STAFF);
}

const char* role_to_string(user_role_t role) {
    switch (role) {
        case ROLE_STUDENT: return "STUDENT";
        case ROLE_INTERN: return "INTERN";
        case ROLE_STAFF: return "STAFF";
        default: return "INVALID";
    }
}

