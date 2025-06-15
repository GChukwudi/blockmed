#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "utils.h"
#include "transaction.h"

// Define block structure
typedef struct block {
    int index;
    char timestamp[20];
    medical_transaction_t transaction;
    unsigned long nonce;
    char previous_hash[HASH_SIZE];
    char current_hash[HASH_SIZE];
    struct block *next;
} block_t;

// blockchain structure
typedef struct {
    block_t *head;
    block_t *tail;
    int length;
} blockchain_t;

// Function prototypes
blockchain_t* create_blockchain(void);
block_t* create_genesis_block(void);
block_t* create_block(int index, const medical_transaction_t *tx,
                      const char *prev_hash);
void calculate_block_hash(block_t *block);
int add_block_to_chain(blockchain_t *chain, block_t *block);
void print_blockchain(const blockchain_t *chain);
int validate_blockchain(const blockchain_t *chain);
void free_blockchain(blockchain_t *chain);

#endif
