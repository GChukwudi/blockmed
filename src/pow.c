#include "pow.h"

static int mining_difficulty = DEFAULT_DIFFICULTY;

// Set the mining difficulty
void set_mining_difficulty(int difficulty) {
    if (difficulty > 0 && difficulty <= 8) {
        mining_difficulty = difficulty;
    }
}

// Get the current mining difficulty
int get_mining_difficulty(void) {
    return mining_difficulty;
}

int is_valid_proof(const char *hash, int difficulty) {
    if (!hash || difficulty <= 0) return 0;

    // Check if the hash starts with the required number of zeros
    for (int i = 0; i < difficulty; i++) {
        if (hash[i] != '0') {
            return 0;
        }
    }
    return 1;
}

int mine_block(block_t *block, int difficulty) {
    if (!block) {
        return 0; // Invalid block
    }

    printf("Mining block %d with difficulty %d...\n", block->index, difficulty);

    block->nonce = 0;

    do {
        // Calculate the hash for the current nonce
        block->nonce++;
        calculate_block_hash(block);

        // show progress every 1000 iterations
        if (block->nonce % 1000 == 0) {
            printf("Nonce: %lu, Hash: %.16s...\n", block->nonce, block->current_hash);
        }
    } while (!is_valid_proof(block->current_hash, difficulty));

    printf("Block mined! Nonce: %lu, Hash: %s\n", block->nonce, block->current_hash);
    return 1;
}
