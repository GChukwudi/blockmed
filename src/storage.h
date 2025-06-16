#ifndef STORAGE_H
#define STORAGE_H

#include "blockchain.h"

// function prototypes

int save_blockchain(const blockchain_t *chain, const char *filename);
blockchain_t *load_blockchain(const char *filename);
int calculate_file_hash(const char *filename, char *hash);
int verify_file_integrity(const char *filename, const char *expected_hash);

#endif
