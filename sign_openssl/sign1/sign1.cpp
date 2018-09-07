// sign.cpp : Defines the entry point for the console application.

#include <stdio.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <iostream>
  
 
/*
PKCS7Sign.cpp
功能：调用OpenSSL实现数字签名功能
环境：OpenSSL 1.0.2g
*/
 
void InitOpenSSL()
{
	ERR_load_crypto_strings();
}
 
unsigned char * GetSign(char* keyFile,char* plainText,unsigned char* cipherText,unsigned int *cipherTextLen)
{	
	FILE* fp = fopen(keyFile, "r");
	if (fp == NULL) 
		return NULL;

	/* Read private key */
	EVP_PKEY* pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
	fclose (fp);
 
	if (pkey == NULL) { 
		ERR_print_errors_fp (stderr);
		return NULL;
	}
 
	/* Do the signature */
	EVP_MD_CTX     md_ctx;
	EVP_SignInit   (&md_ctx, EVP_sha1());
	EVP_SignUpdate (&md_ctx, plainText, strlen(plainText));
	int err = EVP_SignFinal (&md_ctx, cipherText, cipherTextLen, pkey);
 
	if (err != 1) {
		ERR_print_errors_fp(stderr);
		return NULL;
	}
 
	EVP_PKEY_free(pkey);
 
	return cipherText;
}
 
bool VerifySign(char* certFile,unsigned char* cipherText,unsigned int cipherTextLen,char* plainText)
{
	/* Get X509 */
	FILE* fp = fopen (certFile, "r");
	if (fp == NULL) 
		return false;
	X509* x509 = PEM_read_X509(fp, NULL, NULL, NULL);
	fclose (fp);
 
	if (x509 == NULL) {
		ERR_print_errors_fp (stderr);
		return false;
	}
 
	/* Get public key - eay */
	EVP_PKEY *pkey=X509_get_pubkey(x509);
	if (pkey == NULL) {
		ERR_print_errors_fp (stderr);
		return false;
	}
 
	/* Verify the signature */
	EVP_MD_CTX md_ctx;
	EVP_VerifyInit   (&md_ctx, EVP_sha1());
	EVP_VerifyUpdate (&md_ctx, plainText, strlen((char*)plainText));
	int err = EVP_VerifyFinal (&md_ctx, cipherText, cipherTextLen, pkey);
	EVP_PKEY_free (pkey);
 
	if (err != 1) {
		ERR_print_errors_fp (stderr);
		return false;
	}
	return true;
}
 
int main(int argc, char* argv[])
{
	char certFile[] = "cacert.pem";//含共匙
	char keyFile[]  = "key.pem";//含私匙
 
	char plainText[]     = "I owe you...";//待签名的明文
	unsigned char cipherText[1024*4];
	unsigned int cipherTextLen;
 
	InitOpenSSL();
 
	memset(cipherText,0,sizeof(cipherText));
	if(NULL==GetSign(keyFile,plainText,cipherText,&cipherTextLen))
	{
		printf("签名失败！\n");
		return -1;
	}
	std::cout << cipherText << std::endl;
 
	if(false==VerifySign(certFile,cipherText,cipherTextLen,plainText))
	{
		printf("验证签名失败！\n");
		return -2;
	}
 
 
	printf ("Signature Verified Ok.\n");
	return 0;
}