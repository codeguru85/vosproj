/*****************************************************************************
Module :     URLEncode.cpp
Notices:     Written 2002 by ChandraSekar Vuppalapati
Description: CPP URL Encoder
*****************************************************************************/
#define _CRTDBG_MAP_ALLOC


#include "url_encode.h"

#define MAX_BUFFER_SIZE 4096

#include <stdlib.h>
// HEX Values array
char hexVals[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
// UNSAFE String
std::string CURLEncode::csUnsafeString= "\"<>%\\^[]`+$,@:;/!#?=&";

// PURPOSE OF THIS FUNCTION IS TO CONVERT A GIVEN CHAR TO URL HEX FORM
std::string CURLEncode::convert(char val)
{
	std::string csRet;
	csRet += "%";
	csRet += decToHex(val, 16);
	return  csRet;
}


static void reverseStrings(char **a, int length)
/* Reverse the order of the char* array. */
{
int halfLen = (length>>1);
char **end = a+length;
char *c;
while (--halfLen >= 0)
    {
    c = *a;
    *a++ = *--end;
    *end = c;
    }
}


// THIS IS A HELPER FUNCTION.
// PURPOSE OF THIS FUNCTION IS TO GENERATE A HEX REPRESENTATION OF GIVEN CHARACTER
std::string CURLEncode::decToHex(char num, int radix)
{
	int temp=0;
	std::string csTmp;
	int num_char;
	num_char = (int) num;

	// ISO-8859-1
	// IF THE IF LOOP IS COMMENTED, THE CODE WILL FAIL TO GENERATE A
	// PROPER URL ENCODE FOR THE CHARACTERS WHOSE RANGE IN 127-255(DECIMAL)
	if (num_char < 0)
		num_char = 256 + num_char;

	while (num_char >= radix)
    {
		temp = num_char % radix;
		num_char = (int)floor(num_char / radix);
		csTmp = hexVals[temp];
    }

	csTmp += hexVals[num_char];

	if(csTmp.length() < 2)
	{
		csTmp += '0';
	}

	std::string strdecToHex(csTmp);
	// Reverse the String
	//strdecToHex.MakeReverse();

	return strdecToHex;
}

// PURPOSE OF THIS FUNCTION IS TO CHECK TO SEE IF A CHAR IS URL UNSAFE.
// TRUE = UNSAFE, FALSE = SAFE
bool CURLEncode::isUnsafe(char compareChar)
{
	bool bcharfound = false;
	char tmpsafeChar;
	int m_strLen = 0;

	m_strLen = csUnsafeString.length();
	for(int ichar_pos = 0; ichar_pos < m_strLen ;ichar_pos++)
	{
		tmpsafeChar = csUnsafeString[ichar_pos];
		if(tmpsafeChar == compareChar)
		{
			bcharfound = true;
			break;
		}
	}
	int char_ascii_value = 0;
	//char_ascii_value = __toascii(compareChar);
	char_ascii_value = (int) compareChar;

	if(bcharfound == false &&  char_ascii_value > 32 && char_ascii_value < 123)
	{
		return false;
	}
	// found no unsafe chars, return false
	else
	{
		return true;
	}

	return true;
}
// PURPOSE OF THIS FUNCTION IS TO CONVERT A STRING
// TO URL ENCODE FORM.
std::string CURLEncode::URLEncode(std::string pcsEncode)
{
	int ichar_pos;
	std::string csEncode;
	std::string csEncoded;
	int m_length;
	int ascii_value;

	csEncode = pcsEncode;
	m_length = csEncode.length();

	for(ichar_pos = 0; ichar_pos < m_length; ichar_pos++)
	{
		char ch = csEncode[ichar_pos];
		if (ch < ' ')
		{
			ch = ch;
		}
		if(!isUnsafe(ch))
		{
			// Safe Character
			char szStr[8] = {0};
			szStr[0] = ch ; szStr[1] = '\0' ;
			csEncoded += std::string(szStr);
		}
		else
		{
			// get Hex Value of the Character
			csEncoded += convert(ch);
		}
	}


	return csEncoded;

}
