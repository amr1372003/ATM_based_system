#ifndef HILL_CIPHER_H
#define HILL_CIPHER_H

    #include<stdio.h>
    #include<string.h>
    #include <ctype.h>
    #include <time.h>
    #include <stdlib.h>

    extern char characters[38];
    extern int key[2][2];

    void encryption(char *str);
    void decryption(char *str);

#endif