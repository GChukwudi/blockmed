#ifndef POW_H
#define POW_H

#include "blockchain.h"

#define DEFAULT_DIFFICULTY 4

// Function prototypes
int mine_block(block_t *block, int difficulty);
int is_valid_proof(const char *hash, int difficulty);
void set_mining_difficulty(int difficulty);
int get_mining_difficulty(void);

#endif
