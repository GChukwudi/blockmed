#ifndef CLI_H
#define CLI_H

#include "blockchain.h"
#include "auth.h"
#include "storage.h"
#include "pow.h"
#include "log.h"

// function prototypes
void show_menu(user_role_t role);
void handle_add_record(blockchain_t *chain, const user_t *user);
void handle_mine_block(blockchain_t *chain, const user_t *user);
void handle_view_blockchain(const blockchain_t *chain, const user_t *user);
void handle_validate_chain(const blockchain_t *chain, const user_t *user);
void handle_user_login(user_t *user);
void handle_user_registration(void);
int run_cli(blockchain_t *chain);


#endif
