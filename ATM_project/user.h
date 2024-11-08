#ifndef USER_H
#define USER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

#include "transaction.h"
#include "hill_cipher.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct users {
    char *ID;
    char *name;
    char *password;
    int current_balance;
    struct users *priv_user;
    struct users *next_user;
} users;

extern users *head;

void read_from_csv(const char *filename);
void authenticate_user(users *users_list);
void display_menu(users *users_list);
void balance_inquiry(users *users_list);

#endif
