#include "cipher_impl.h"
void handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}

int AES_Encrypt(EVP_CIPHER_CTX *ctx, unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext, unsigned char *tag) {
    ;
    int len;
    int ciphertext_len;

    // 初始化加密操作
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
        handleErrors();

    // 设置密钥和IV
    if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv)) handleErrors();

    // 提供待加密的数据
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    // 完成加密
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;

    // 获取标签
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag))
        handleErrors();

    return ciphertext_len;
}

int AES_Decrypt(EVP_CIPHER_CTX *ctx, unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *tag, unsigned char *plaintext) {
    int len;
    int plaintext_len;
    int ret;

    // 初始化解密操作
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
        handleErrors();

    // 设置密钥和IV
    if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)) handleErrors();

    // 提供待解密的数据
    if (!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    // 设置标签
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag))
        handleErrors();

    // 完成解密
    ret = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);

    if (ret > 0) {
        plaintext_len += len;
        return plaintext_len;
    } else {
        return -1;
    }
}


int ChaCha20_Encrypt(EVP_CIPHER_CTX *ctx, unsigned char *plaintext, int plaintext_len,
            unsigned char *key,
            unsigned char *iv,
            unsigned char *ciphertext) {
    int len;
    int ciphertext_len;


    if (1 != EVP_EncryptInit_ex(ctx, EVP_chacha20(), NULL, key, iv))
        handleErrors();

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;

    return ciphertext_len;
}

int ChaCha20_Decrypt(EVP_CIPHER_CTX *ctx, unsigned char *ciphertext, int ciphertext_len,
            unsigned char *key,
            unsigned char *iv,
            unsigned char *plaintext) {
    
    int len;
    int plaintext_len;

    if (1 != EVP_DecryptInit_ex(ctx, EVP_chacha20(), NULL, key, iv))
        handleErrors();

    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
    plaintext_len += len;

    return plaintext_len;
}
