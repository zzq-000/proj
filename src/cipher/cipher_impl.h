#pragma once
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include "cipher_type.pb.h"
void handleErrors();

// key: 32bytes, iv: 12bytes
// tag: 16bytes

int AES_Encrypt(EVP_CIPHER_CTX *ctx, unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext, unsigned char *tag);

int AES_Decrypt(EVP_CIPHER_CTX *ctx, unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *tag, unsigned char *plaintext);

int ChaCha20_Encrypt(EVP_CIPHER_CTX *ctx, unsigned char *plaintext, int plaintext_len,
            unsigned char *key, unsigned char *iv, unsigned char *ciphertext);
int ChaCha20_Decrypt(EVP_CIPHER_CTX *ctx, unsigned char *ciphertext, int ciphertext_len,
            unsigned char *key, unsigned char *iv, unsigned char *plaintext);