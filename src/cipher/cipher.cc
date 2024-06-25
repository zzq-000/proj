#include "cipher.h"

int Cipher::AES_Encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
        unsigned char *iv, unsigned char *ciphertext, unsigned char *tag) {
    return ::AES_Encrypt(ctx_, plaintext, plaintext_len, key, iv, ciphertext, tag);
}

int Cipher::AES_Decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *tag, unsigned char *plaintext) {
    return ::AES_Decrypt(ctx_, ciphertext, ciphertext_len, key, iv, tag, plaintext);
}

int Cipher::ChaCha20_Encrypt(unsigned char *plaintext, int plaintext_len,
            unsigned char *key, unsigned char *iv, unsigned char *ciphertext) {
    return ::ChaCha20_Encrypt(ctx_, plaintext, plaintext_len, key, iv, ciphertext);

}
int Cipher::ChaCha20_Decrypt(unsigned char *ciphertext, int ciphertext_len,
            unsigned char *key, unsigned char *iv, unsigned char *plaintext) {
    return ::ChaCha20_Decrypt(ctx_, ciphertext, ciphertext_len, key, iv, plaintext);
}