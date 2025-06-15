#ifndef UTILS_H
#define UTILS_H

// utils.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

// Constants
#define MAX_INPUT_SIZE 1024
#define HASH_SIZE 65
#define MAX_EMAIL_SIZE 100
#define MAX_PASSWORD_SIZE 256
#define MAX_DIAGNOSIS_SIZE 500
#define MAX_PRESCRIPTION_SIZE 500
#define MAX_NOTES_SIZE 1000

// Function prototypes
void get_timestamp(char *timestamp);
void sha256_hash(const char *input, char *output);
void sanitize_input(char *input);
int is_valid_email(const char *email);
void secure_input(char *buffer, size_t size);

#endif
