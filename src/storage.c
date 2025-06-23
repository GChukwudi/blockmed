#include "storage.h"
#include <errno.h>


int save_blockchain(const blockchain_t *chain, const char *filename) {
    if (!chain || !filename) {
        printf("Error: Invalid parameters for save_blockchain\n");
        return 0;
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Could not open file '%s' for writing: %s\n", filename, strerror(errno));
        return 0;
    }

    printf("Saving blockchain with %d blocks to '%s'\n", chain->length, filename);

    if (fwrite(&chain->length, sizeof(int), 1, file) != 1) {
        printf("Error: Failed to write blockchain length\n");
        fclose(file);
        return 0;
    }

    block_t *current = chain->head;
    int blocks_written = 0;

    while (current) {
        // Write fields individually
        fwrite(&current->index, sizeof(int), 1, file);
        fwrite(current->timestamp, sizeof(current->timestamp), 1, file);
        fwrite(&current->transaction, sizeof(medical_transaction_t), 1, file);
        fwrite(&current->nonce, sizeof(unsigned long), 1, file);
        fwrite(current->previous_hash, HASH_SIZE, 1, file);
        fwrite(current->current_hash, HASH_SIZE, 1, file);

        blocks_written++;
        current = current->next;
    }

    printf("Successfully saved %d blocks\n", blocks_written);
    fclose(file);
    return 1;
}

blockchain_t *load_blockchain(const char *filename) {
    if (!filename) {
        printf("Error: Filename is NULL\n");
        return NULL;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Could not open file '%s' for reading: %s\n", filename, strerror(errno));
        return NULL;
    }

    blockchain_t *chain = malloc(sizeof(blockchain_t));
    if (!chain) {
        printf("Error: Memory allocation failed for blockchain\n");
        fclose(file);
        return NULL;
    }

    chain->head = NULL;
    chain->tail = NULL;
    chain->length = 0;

    int saved_length;
    if (fread(&saved_length, sizeof(int), 1, file) != 1) {
        printf("Error: Failed to read blockchain length from file\n");
        free(chain);
        fclose(file);
        return NULL;
    }

    printf("Loading blockchain with %d blocks from '%s'\n", saved_length, filename);

    if (saved_length < 0 || saved_length > 100000) {
        printf("Error: Invalid blockchain length: %d\n", saved_length);
        free(chain);
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < saved_length; i++) {
        block_t *block = malloc(sizeof(block_t));
        if (!block) {
            printf("Error: Memory allocation failed for block %d\n", i);
            free_blockchain(chain);
            fclose(file);
            return NULL;
        }

        // Read fields individually
        if (
            fread(&block->index, sizeof(int), 1, file) != 1 ||
            fread(block->timestamp, sizeof(block->timestamp), 1, file) != 1 ||
            fread(&block->transaction, sizeof(medical_transaction_t), 1, file) != 1 ||
            fread(&block->nonce, sizeof(unsigned long), 1, file) != 1 ||
            fread(block->previous_hash, HASH_SIZE, 1, file) != 1 ||
            fread(block->current_hash, HASH_SIZE, 1, file) != 1
        ) {
            printf("Error: Failed to read block %d from file\n", i);
            free_blockchain(chain);
            free(block);
            fclose(file);
            return NULL;
        }

        block->next = NULL;

        if (add_block_to_chain(chain, block) != 1) {
            printf("Error: Failed to add block %d to chain\n", i);
            free_blockchain(chain);
            free(block);
            fclose(file);
            return NULL;
        }

        printf("Loaded block %d successfully\n", i + 1);
    }

    fclose(file);
    printf("Successfully loaded blockchain with %d blocks\n", chain->length);
    return chain;
}

int calculate_file_hash(const char *filename, char *hash) {
    if (!filename || !hash) {
        printf("Error: Invalid parameters for calculate_file_hash\n");
        return 0;
    }

    // Open the file for reading in binary mode
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Could not open file '%s' for hashing: %s\n", filename, strerror(errno));
        return 0;
    }

    // Initialize SHA256 context
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    unsigned char buffer[1024];
    size_t bytes_read;
    size_t total_bytes = 0;

    // Read the file in chunks and update the SHA256 context
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        SHA256_Update(&sha256, buffer, bytes_read);
        total_bytes += bytes_read;
    }

    // Check for read errors
    if (ferror(file)) {
        printf("Error: Failed to read file for hashing\n");
        fclose(file);
        return 0;
    }

    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_Final(digest, &sha256);

    // Convert to hex string
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hash + (i * 2), "%02x", digest[i]);
    }
    hash[64] = '\0'; // Null-terminate the hash string

    printf("Calculated hash for file '%s' (%zu bytes): %.16s...\n", filename, total_bytes, hash);
    fclose(file);
    return 1; // Success
}

int verify_file_integrity(const char *filename, const char *expected_hash) {
    if (!filename || !expected_hash) {
        printf("Error: Invalid parameters for verify_file_integrity\n");
        return 0;
    }

    char computed_hash[HASH_SIZE];
    if (!calculate_file_hash(filename, computed_hash)) {
        printf("Error: Failed to compute hash for verification\n");
        return 0;
    }

    int result = strcmp(computed_hash, expected_hash) == 0;
    printf("File integrity check: %s\n", result ? "PASSED" : "FAILED");
    if (!result) {
        printf("Expected: %.16s...\n", expected_hash);
        printf("Computed: %.16s...\n", computed_hash);
    }
    
    return result;
}