#include "storage.h"

int save_blockchain(const blockchain_t *chain, const char *filename) {
    if (!chain || !filename) return 0;

    // Open the file for writing in binary mode
    FILE *file = fopen(filename, "wb");
    if (!file) return 0;

    // Write the blockchain length
    fwrite(&chain->length, sizeof(int), 1, file);

    // Write each block in the blockchain
    block_t *current = chain->head;
    while (current) {
        fwrite(current, sizeof(block_t), 1, file);
        current = current->next;
    }

    fclose(file);
    return 1; // Success
}

blockchain_t *load_blockchain(const char *filename) {
    if (!filename) return NULL;

    // Open the file for reading in binary mode
    FILE *file = fopen(filename, "rb");
    if (!file) return NULL;

    // Allocate memory for the blockchain structure
    blockchain_t *chain = malloc(sizeof(blockchain_t));
    if (!chain) {
        fclose(file);
        return NULL; // Memory allocation failed
    }

    chain->head = NULL;
    chain->tail = NULL;
    chain->length = 0;

    int saved_length;
    if (fread(&saved_length, sizeof(int), 1, file) != 1) {
        free(chain);
        fclose(file);
        return NULL; // Failed to read length
    }

    for (int i = 0; i < saved_length; i++) {
        block_t *block = malloc(sizeof(block_t));
        if (!block || fread(block, sizeof(block_t), 1, file) != 1) {
            free_blockchain(chain);
            if (block) free(block);
            return NULL;
        }

        block->next = NULL;
        add_block_to_chain(chain, block);
    }
    fclose(file);
    return chain; // Successfully loaded blockchain
}

int calculate_file_hash(const char *filename, char *hash) {
    if (!filename || !hash) return 0;

    // Open the file for reading in binary mode
    FILE *file = fopen(filename, "rb");
    if (!file) return 0;

    // Initialize SHA256 context
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    unsigned char buffer[1024];
    size_t bytes_read;

    // Read the file in chunks and update the SHA256 context
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        SHA256_Update(&sha256, buffer, bytes_read);
    }

    // Check for read errors
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_Final(digest, &sha256);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hash + (i * 2), "%02x", digest[i]);
    }
    hash[64] = '\0'; // Null-terminate the hash string

    fclose(file);
    return 1; // Success
}

int verify_file_integrity(const char *filename, const char *expected_hash) {
    if (!filename || !expected_hash) return 0;

    char computed_hash[HASH_SIZE];
    if (!calculate_file_hash(filename, computed_hash)) return 0;

    return strcmp(computed_hash, expected_hash) == 0;
}
