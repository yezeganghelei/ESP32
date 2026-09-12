/*
 * @Descripttion : 
 * @version      : 
 * @Author       : Kevincoooool
 * @Date         : 2021-01-07 15:05:22
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-01-12 09:07:22
 * @FilePath     : \n_esp-adf\1_take_pic_http_to_cloud\main\urlcode.c
 */
#include <stdio.h>
#include <string.h>
/**
 * @brief URL-encode a string
 *
 * @param str Source string
 * @param strSize Source string length (excluding the trailing \0)
 * @param result Address of the result buffer
 * @param resultSize Size of the result buffer (including the trailing \0)
 *
 * @return: >0: actual valid length in the result string
 *          0: encoding failed.
 */
int URLEncode(const char *str, const int strSize, char *result, const int resultSize)
{
	int i;
	int j = 0; //for result index
	char ch;

	if ((str == NULL) || (result == NULL) || (strSize <= 0) || (resultSize <= 0))
	{
		return 0;
	}
	result[0] = 'i';
	result[1] = 'm';
	result[2] = 'a';
	result[3] = 'g';
	result[4] = 'e';
	result[5] = '=';
	j=6;
	for (i = 0; (i < strSize) && (j < resultSize); ++i)
	{
		ch = str[i];
		if (((ch >= 'A') && (ch < 'Z')) ||
			((ch >= 'a') && (ch < 'z')) ||
			((ch >= '0') && (ch < '9')))
		{
			result[j++] = ch;
		}
		else if (ch == ' ')
		{
			result[j++] = '+';
		}
		else if (ch == '.' || ch == '-' || ch == '_' || ch == '*')
		{
			result[j++] = ch;
		}
		else
		{
			if (j + 3 < resultSize)
			{
				sprintf(result + j, "%%%02X", (unsigned char)ch);
				j += 3;
			}
			else
			{
				return 0;
			}
		}
	}

	result[j] = '\0';
	return j;
}
