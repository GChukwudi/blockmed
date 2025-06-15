#include "utils.h"

// function to extract the current timestamp
void get_timestamp(char *timestamp) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", tm_info);
}

// function to hash a string using SHA-256
void sha256_hash(const char *input, char *output) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input, strlen(input));
    SHA256_Final(hash, &sha256);

    // Convert the hash to a hexadecimal string
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }

    output[64] = '\0'; // Null-terminate the string
}

// function to sanitize input to prevent injection attacks
void sanitize_input(char *input) {
    if (!input)
        return;

    // Remove newline if present
    char *newline = strchr(input, '\n');
    if (newline) {
        *newline = '\0';
    }

    // Replace semicolons, slashes, and ampersands with underscores
    for (int i = 0; input[i]; i++) {
        if (input[i] == ';' || input[i] == '/' || input[i] == '&') {
            input[i] = '_';
        }
    }
}

// function to validate email format
int is_valid_email(const char *email) {
    if (!email || strlen(email) < 5) return 0;

    const char *at_sign = strchr(email, '@');
    const char *dot = strrchr(email, '.');

    // Basic structure check: has '@', has '.', and '.' comes after '@'
    if (!(at_sign && dot && at_sign < dot && at_sign > email)) {
        return 0;
    }

    // Domain-specific check for ALU
    if (strstr(at_sign, "@alueducation.com") ||
        strstr(at_sign, "@si.alueducation.com") ||
        strstr(at_sign, "@alustudent.com")) {
        return 1;
    }

    return 0;
}

// function to securely read input from the user
void secure_input(char *buffer, size_t size) {
    if (fgets(buffer, size, stdin))
    {
        sanitize_input(buffer);
    }
}
