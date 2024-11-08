#include "hill_cipher.h"


char characters[38]={' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9','\0'};
int key[2][2] = {{27, 35}, {34, 23}};

/**
 * @brief Function to calculate the greatest common divisor of two numbers
 * @param a
 * @param b
 */
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}


/**
 * @brief Function to calculate the determinant of a 2x2 matrix
 * @param key
 */
int determinant(int key[2][2]) {
    return (key[0][0] * key[1][1] - key[0][1] * key[1][0]) % 37;
}


/**
 * @brief Function to check if the key matrix is valid
 * @param key
 */
int is_valid_key(int key[2][2]) {
    int det = determinant(key);
    return (gcd(det + 37, 37) == 1); // Check if det is coprime to 36
}


/**
 * @brief Function to generate a random key matrix
 * @param key
 */
void generate_random_key(int key[2][2]) {
    do {
        // Generate random values for the key matrix
        key[0][0] = rand() % 37;
        key[0][1] = rand() % 37;
        key[1][0] = rand() % 37;
        key[1][1] = rand() % 37;
    } while (!is_valid_key(key));
}


/**
 * @brief Function to encrypt the text using the Hill Cipher
 * @param str
 */
void encryption(char *str) {
    //generate_random_key(key);
    for (int i = 0; i < strlen(str); i++){
        str[i] = tolower(str[i]);
    }

    int length = strlen(str);

    // Ensure even length by padding with 'x' if necessary
    if (length % 2 != 0) {
        str[length] = ' ';
        str[length + 1] = '\0';
        length++;
    }

    for (int i = 0; i < length; i += 2) {
        int index1 = strchr(characters, str[i]) - characters;
        int index2 = strchr(characters, str[i + 1]) - characters;

        int new_index1 = (key[0][0] * index1 + key[0][1] * index2) % 37;
        int new_index2 = (key[1][0] * index1 + key[1][1] * index2) % 37;

        str[i] = characters[new_index1];
        str[i + 1] = characters[new_index2];
    }
}

/**
 * @brief Function to calculate the modular inverse of a number
 * @param a
 * @param m
 */
int mod_inverse(int a, int m) {
    for (int x = 1; x < m; x++) {
        if ((a * x) % m == 1) {
            return x;
        }
    }
    return -1;
}


/**
 * @brief Function to decrypt the text using the Hill Cipher
 * @param str
 */
void decryption(char *str) {
    int determinant = (key[0][0] * key[1][1] - key[0][1] * key[1][0]) % 37;
    if (determinant < 0) determinant += 37;

    int inverse_determinant = mod_inverse(determinant, 37);
    if (inverse_determinant == -1) {
        printf("No modular inverse exists for the key matrix.\n");
        return;
    }

    // Calculate inverse key matrix using modular inverse
    int inv_key[2][2] = {
        {(key[1][1] * inverse_determinant) % 37, (-key[0][1] * inverse_determinant + 37) % 37},
        {(-key[1][0] * inverse_determinant + 37) % 37, (key[0][0] * inverse_determinant) % 37}
    };

    int length = strlen(str);

    for (int i = 0; i < length; i += 2) {
        int index1 = strchr(characters, str[i]) - characters;
        int index2 = strchr(characters, str[i + 1]) - characters;

        int new_index1 = (inv_key[0][0] * index1 + inv_key[0][1] * index2) % 37;
        int new_index2 = (inv_key[1][0] * index1 + inv_key[1][1] * index2) % 37;

        if (new_index1 < 0) new_index1 += 37;
        if (new_index2 < 0) new_index2 += 37;

        str[i] = characters[new_index1];
        str[i + 1] = characters[new_index2];
    }
}

