#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

#include "user.h"
#include "hill_cipher.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct transactions {
    char transaction_type[20];
    char transaction_id[20];
    char user_id[20];
    int amount;
    struct transactions *next_transaction;
    struct transactions *prev_transaction;
} transactions;

extern transactions *trans_head;

void initialize_transaction(transactions **trans_head, struct users *users_list);
void deposit_funds(transactions **list, struct users *users_list);
void withdraw_funds(transactions **list, struct users *users_list);
void transfer_funds(transactions **list, struct users *users_list);
void deposit_history(const char *filename, struct users *users_list);

#endif
