/*
 * @Descripttion : 
 * @version      : 
 * @Author       : Kevincoooool
 * @Date         : 2021-01-07 14:40:25
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-07 18:24:01
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\baidu\base64.c
 */
/*base64.c*/
#include "base64.h"

// Define the base64 encoding table
#define base64_table "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

char *base64_encode(uint8_t *str, uint32_t img_len)
{
	long len;
	long str_len;
	char *res = NULL;
	int i, j;
	// Calculate the length of the base64-encoded string
	str_len = img_len;
	if (str_len % 3 == 0)
		len = str_len / 3 * 4;
	else
		len = (str_len / 3 + 1) * 4;

	res = malloc(sizeof(char) * len + 1);
	res[len] = '\0';

	// Encode in groups of three 8-bit characters
	for (i = 0, j = 0; i < len - 2; j += 3, i += 4)
	{
		res[i] = base64_table[str[j] >> 2];										// Take the first 6 bits of the first character and find the corresponding result character
		res[i + 1] = base64_table[(str[j] & 0x3) << 4 | (str[j + 1] >> 4)];		// Combine the low bits of the first character with the high 4 bits of the second character
		res[i + 2] = base64_table[(str[j + 1] & 0xf) << 2 | (str[j + 2] >> 6)]; // Combine the low 4 bits of the second character with the high 2 bits of the third character
		res[i + 3] = base64_table[str[j + 2] & 0x3f];							// Take the low 6 bits of the third character and find the result character
	}

	switch (str_len % 3)
	{
	case 1:
		res[i - 2] = '=';
		res[i - 1] = '=';
		break;
	case 2:
		res[i - 1] = '=';
		break;
	}

	return res;
}

uint8_t *base64_decode(uint8_t *code)
{
	// Look up the corresponding decimal value for a character using the base64 table
	int table[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				   0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0,
				   63, 52, 53, 54, 55, 56, 57, 58,
				   59, 60, 61, 0, 0, 0, 0, 0, 0, 0, 0,
				   1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
				   13, 14, 15, 16, 17, 18, 19, 20, 21,
				   22, 23, 24, 25, 0, 0, 0, 0, 0, 0, 26,
				   27, 28, 29, 30, 31, 32, 33, 34, 35,
				   36, 37, 38, 39, 40, 41, 42, 43, 44,
				   45, 46, 47, 48, 49, 50, 51};
	long len;
	long str_len;
	uint8_t *res;
	int i, j;

	// Calculate the length of the decoded string
	len = strlen((const char *)code);
	// Check whether the encoded string ends with '='
	if (strstr((const char *)code, "=="))
		str_len = len / 4 * 3 - 2;
	else if (strstr((const char *)code, "="))
		str_len = len / 4 * 3 - 1;
	else
		str_len = len / 4 * 3;

	res = malloc(sizeof(uint8_t) * str_len + 1);
	res[str_len] = '\0';

	// Decode in groups of four characters
	for (i = 0, j = 0; i < len - 2; j += 3, i += 4)
	{
		res[j] = ((uint8_t)table[code[i]]) << 2 | (((uint8_t)table[code[i + 1]]) >> 4);			  // Combine the high 6 bits of the first character with the low 2 bits of the second character
		res[j + 1] = (((uint8_t)table[code[i + 1]]) << 4) | (((uint8_t)table[code[i + 2]]) >> 2); // Combine the low 4 bits of the second character with the high 4 bits of the third character
		res[j + 2] = (((uint8_t)table[code[i + 2]]) << 6) | ((uint8_t)table[code[i + 3]]);		  // Combine the low 2 bits of the third character with the fourth character
	}

	return res;
}