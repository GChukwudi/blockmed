#include "blockchain.h"

// functions to create blockchain
blockchain_t* create_blockchain(void) {
    blockchain_t *chain = malloc(sizeof(blockchain_t));
    if (!chain) return NULL;

    chain->head = NULL;
    chain->tail = NULL;
    chain->length = 0;

    // Create the genesis block
    block_t *genesis = create_genesis_block();
    if (genesis) {
        add_block_to_chain(chain, genesis);
    }
    return chain;
}

// Create the genesis block with initial transaction
block_t* create_genesis_block(void) {
    block_t *genesis = malloc(sizeof(block_t));
    if (!genesis) return NULL;

    genesis->index = 0;
    get_timestamp(genesis->timestamp);

    // Create a dummy transaction for the genesis block
    create_transaction(&genesis->transaction, "GENESIS", "system@alueducation.com",
                       "Genesis Block", "No Prescription", "Initial Block in the chain");

    // Set nonce and previous hash for the genesis block
    genesis->nonce = 0;
    strcpy(genesis->previous_hash, "0000000000000000000000000000000000000000000000000000000000000000");
    genesis->next = NULL;

    // Calculate the hash for the genesis block
    calculate_block_hash(genesis);
    return genesis;
}

// Create a new block with the given transaction and previous hash
block_t* create_block(int index, const medical_transaction_t *tx, const char *prev_hash) {
    if (!tx || !prev_hash) return NULL;

    // Allocate memory for the new block
    block_t *block = malloc(sizeof(block_t));
    if (!block) return NULL;

    // Initialize the block fields
    block->index = index;
    get_timestamp(block->timestamp);
    block->transaction = *tx;
    block->nonce = 0;
    // Copy the previous hash and initialize the current hash
    strncpy(block->previous_hash, prev_hash, HASH_SIZE - 1);
    block->previous_hash[HASH_SIZE - 1] = '\0';
    block->next = NULL;

    return block;
}

// Calculate the hash for the block
void calculate_block_hash(block_t *block) {
    if (!block) return;

    // Prepare the string representation of the block
    char tx_string[2048];
    transaction_to_string(&block->transaction, tx_string);

    // Ensure the transaction string is null-terminated
    char block_data[4096];
    snprintf(block_data, sizeof(block_data), "%d%s%s%lu%s",
             block->index, block->timestamp, tx_string, block->nonce, block->previous_hash);

    // Calculate the SHA-256 hash of the block data
    sha256_hash(block_data, block->current_hash);
}

// Add a block to the blockchain
int add_block_to_chain(blockchain_t *chain, block_t *block) {
    if (!chain || !block) return 0;

    // Validate the block's index and previous hash
    if (chain->length == 0) {
        // Adding the first block (genesis block)
        chain->head = block;
        chain->tail = block;
    } else {
        // Adding subsequent blocks
        chain->tail->next = block;
        chain->tail = block;
    }
    chain->length++;
    return 1;
}

// Print the entire blockchain
void print_blockchain(const blockchain_t *chain) {
    if (!chain) return;

    printf("\n===== BLOCKCHAIN (Length: %d) =====\n", chain->length);
    block_t *current = chain->head;

    while (current) {
        printf("\n-- Block %d --\n", current->index);
        printf("Timestamp: %s\n", current->timestamp);
        printf("Previous Hash: %s\n", current->previous_hash);
        printf("Current Hash: %s\n", current->current_hash);
        printf("Nonce: %lu\n", current->nonce);
        printf("Transaction:\n");
        print_transaction(&current->transaction);

        current = current->next;
    }
}

// Validate the blockchain by checking the hashes and links
int validate_blockchain(const blockchain_t *chain) {
    if (!chain || !chain->head) return 0;

    block_t *current = chain->head;

    while (current->next)  {
        block_t *next_block = current->next;

        // Check if the current block's hash matches the next block's previous hash
        if (strcmp(current->current_hash, next_block->previous_hash) != 0) {
            printf("Chain integrity compromised at block %d!\n", next_block->index);
            return 0;
        }

        // verify the current block's hash
        char temp_hash[HASH_SIZE];
        strcpy(temp_hash, current->current_hash);
        calculate_block_hash(current);

        // Check if the calculated hash matches the stored hash
        if (strcmp(temp_hash, current->current_hash) != 0) {
            printf("Block %d has been tampered with!\n", current->index);
            return 0;
        }

        current = current->next;
    }
    return 1;
}

// Free the entire blockchain
void free_blockchain(blockchain_t *chain) {
    if (!chain) return;

    block_t *current = chain->head;
    while (current) {
        block_t *next = current->next;
        free(current);
        current = next;
    }
    free(chain);
}
