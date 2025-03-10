#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/varlena.h"  /* Required for VARSIZE_ANY_EXHDR, SET_VARSIZE, VARDATA */
#include <openssl/evp.h>
#include <string.h>
#include "varatt.h"


PG_MODULE_MAGIC;

/* Use a 16-byte key and IV for AES-128 (for demonstration only) */
#define KEY "0123456789abcdef"    /* 16 bytes */
#define IV  "abcdef9876543210"     /* 16 bytes */

PG_FUNCTION_INFO_V1(tde_encrypt);
PG_FUNCTION_INFO_V1(tde_decrypt);

/*
 * Function: tde_encrypt
 * Purpose: Encrypts a given text using AES-128-CBC.
 * Returns: Encrypted data as a bytea.
 */
Datum
tde_encrypt(PG_FUNCTION_ARGS)
{
    text *plaintext = PG_GETARG_TEXT_PP(0);
    int plaintext_len = VARSIZE_ANY_EXHDR(plaintext);
    char *plaintext_str = text_to_cstring(plaintext);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        ereport(ERROR, (errmsg("Failed to create cipher context")));

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, (unsigned char *)KEY, (unsigned char *)IV))
    {
        EVP_CIPHER_CTX_free(ctx);
        ereport(ERROR, (errmsg("Failed to initialize encryption")));
    }

    int block_size = EVP_CIPHER_block_size(EVP_aes_128_cbc());
    int ciphertext_len = plaintext_len + block_size;
    unsigned char *ciphertext = palloc(ciphertext_len);
    int len = 0, total_len = 0;

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, (unsigned char *)plaintext_str, plaintext_len))
    {
        EVP_CIPHER_CTX_free(ctx);
        ereport(ERROR, (errmsg("Encryption update failed")));
    }
    total_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
    {
        EVP_CIPHER_CTX_free(ctx);
        ereport(ERROR, (errmsg("Encryption finalization failed")));
    }
    total_len += len;
    EVP_CIPHER_CTX_free(ctx);

    bytea *result = (bytea *) palloc(total_len + VARHDRSZ);
    SET_VARSIZE(result, total_len + VARHDRSZ);
    memcpy(VARDATA(result), ciphertext, total_len);
    pfree(ciphertext);

    PG_RETURN_BYTEA_P(result);
}

/*
 * Function: tde_decrypt
 * Purpose: Decrypts data encrypted with AES-128-CBC.
 * Returns: Decrypted text.
 */
Datum
tde_decrypt(PG_FUNCTION_ARGS)
{
    bytea *ciphertext = PG_GETARG_BYTEA_P(0);
    int ciphertext_len = VARSIZE_ANY_EXHDR(ciphertext);
    unsigned char *ciphertext_data = (unsigned char *) VARDATA(ciphertext);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        ereport(ERROR, (errmsg("Failed to create cipher context")));

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, (unsigned char *)KEY, (unsigned char *)IV))
    {
        EVP_CIPHER_CTX_free(ctx);
        ereport(ERROR, (errmsg("Failed to initialize decryption")));
    }

    int plaintext_len = ciphertext_len;  /* Maximum possible length */
    unsigned char *plaintext = palloc(plaintext_len);
    int len = 0, total_len = 0;

    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext_data, ciphertext_len))
    {
        EVP_CIPHER_CTX_free(ctx);
        ereport(ERROR, (errmsg("Decryption update failed")));
    }
    total_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
    {
        EVP_CIPHER_CTX_free(ctx);
        ereport(ERROR, (errmsg("Decryption finalization failed")));
    }
    total_len += len;
    EVP_CIPHER_CTX_free(ctx);

    text *result = cstring_to_text_with_len((char *)plaintext, total_len);
    pfree(plaintext);

    PG_RETURN_TEXT_P(result);
}
