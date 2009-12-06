#include "client_key.h"

typedef unsigned long WORD32;
const WORD32 DELTA = 0x9e3779b9;

#define ROUNDS 16
#define LOG_ROUNDS 4

/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
void TeaEncryptECB(const BYTE *pInBuf, const BYTE *pKey, BYTE *pOutBuf)
{
	WORD32 y, z;
	WORD32 sum;
	WORD32 k[4];
	int i;

	/*plain-text is TCP/IP-endian;*/
	/*GetBlockBigEndian(in, y, z);*/
	y = ntohl(*((WORD32*)pInBuf));
	z = ntohl(*((WORD32*)(pInBuf+4)));
	/*TCP/IP network byte order (which is big-endian).*/

	for ( i = 0; i<4; i++)
	{
		/*now key is TCP/IP-endian;*/
		k[i] = ntohl(*((WORD32*)(pKey+i*4)));
	}

	sum = 0;
	for (i=0; i<ROUNDS; i++)
	{   
		sum += DELTA;
		y += (z << 4) + k[0] ^ z + sum ^ (z >> 5) + k[1];
    	z += (y << 4) + k[2] ^ y + sum ^ (y >> 5) + k[3];
	}

	*((WORD32*)pOutBuf) = htonl(y);
	*((WORD32*)(pOutBuf+4)) = htonl(z);

	/*now encrypted buf is TCP/IP-endian;*/
}

/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
void TeaDecryptECB(const BYTE *pInBuf, const BYTE *pKey, BYTE *pOutBuf)
{
	WORD32 y, z, sum;
	WORD32 k[4];
	int i;

	/*now encrypted buf is TCP/IP-endian;*/
	/*TCP/IP network byte order (which is big-endian).*/
	y = ntohl(*((WORD32*)pInBuf));
	z = ntohl(*((WORD32*)(pInBuf+4)));

	for ( i=0; i<4; i++)
	{
		/*key is TCP/IP-endian;*/
		k[i] = ntohl(*((WORD32*)(pKey+i*4)));
	}

	sum = DELTA << LOG_ROUNDS;
	for (i=0; i<ROUNDS; i++)
	{
		z -= (y << 4) + k[2] ^ y + sum ^ (y >> 5) + k[3]; 
		y -= (z << 4) + k[0] ^ z + sum ^ (z >> 5) + k[1];
		sum -= DELTA;
	}

	*((WORD32*)pOutBuf) = htonl(y);
	*((WORD32*)(pOutBuf+4)) = htonl(z);

	/*now plain-text is TCP/IP-endian;*/
}

#define SALT_LEN 2
#define ZERO_LEN 7

/*pKey为16byte*/
/*
	输入:pInBuf为需加密的明文部分(Body),nInBufLen为pInBuf长度;
	输出:pOutBuf为密文格式,pOutBufLen为pOutBuf的长度是8byte的倍数;
*/
/*TEA加密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(8byte)*/
void oi_symmetry_encrypt(const BYTE* pInBuf, int nInBufLen, const BYTE* pKey, BYTE* pOutBuf, int *pOutBufLen)
{
	int nPadSaltBodyZeroLen/*PadLen(1byte)+Salt+Body+Zero的长度*/;
	int nPadlen;
	BYTE src_buf[8], zero_iv[8], *iv_buf;
	int src_i, i, j;

	/*根据Body长度计算PadLen,最小必需长度必需为8byte的整数倍*/
	nPadSaltBodyZeroLen = nInBufLen/*Body长度*/+1+SALT_LEN+ZERO_LEN/*PadLen(1byte)+Salt(2byte)+Zero(8byte)*/;
	if((nPadlen=nPadSaltBodyZeroLen%8)) /*len=nSaltBodyZeroLen%8*/
	{
		/*模8余0需补0,余1补7,余2补6,...,余7补1*/
		nPadlen=8-nPadlen;
	}

	/*srand( (unsigned)time( NULL ) ); 初始化随机数*/
	/*加密第一块数据(8byte),取前面10byte*/
	src_buf[0] = (((BYTE)rand()) & 0x0f8/*最低三位存PadLen,清零*/) | (BYTE)nPadlen;
	src_i = 1; /*src_i指向src_buf下一个位置*/

	while(nPadlen--)
		src_buf[src_i++]=(BYTE)rand(); /*Padding*/

	/*come here, i must <= 8*/
	memset(zero_iv, 0, 8);
	iv_buf = zero_iv; /*make iv*/
	*pOutBufLen = 0; /*init OutBufLen*/

	for (i=1;i<=SALT_LEN;) /*Salt(2byte)*/
	{
		if (src_i<8)
		{
			src_buf[src_i++]=(BYTE)rand();
			i++; /*i inc in here*/
		}

		if (src_i==8)
		{
			/*src_i==8*/
			for (j=0;j<8;j++) /*CBC XOR*/
				src_buf[j]^=iv_buf[j];
			/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);
			src_i=0;
			iv_buf=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}

	/*src_i指向src_buf下一个位置*/

	while(nInBufLen)
	{
		if (src_i<8)
		{
			src_buf[src_i++]=*(pInBuf++);
			nInBufLen--;
		}

		if (src_i==8)
		{
			/*src_i==8*/
			for (i=0;i<8;i++) /*CBC XOR*/
				src_buf[i]^=iv_buf[i];

			/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
    		TeaEncryptECB(src_buf, pKey, pOutBuf);
			src_i=0;
			iv_buf=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}

	/*src_i指向src_buf下一个位置*/
	for (i=1;i<=ZERO_LEN;)
	{
		if (src_i<8)
		{
			src_buf[src_i++]=0;
			i++; /*i inc in here*/
		}
		if (src_i==8)
		{
			/*src_i==8*/
			for (j=0;j<8;j++) /*CBC XOR*/

				src_buf[j]^=iv_buf[j];
			/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);
			src_i=0;
			iv_buf=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}
}

/*pKey为16byte*/
/*
	输入:pInBuf为密文格式,nInBufLen为pInBuf的长度是8byte的倍数;
	输出:pOutBuf为明文(Body),pOutBufLen为pOutBuf的长度;
	返回值:如果格式正确返回TRUE;
*/
/*TEA解密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(8byte)*/
BOOL oi_symmetry_decrypt(const BYTE* pInBuf, int nInBufLen, const BYTE* pKey, BYTE* pOutBuf, int *pOutBufLen)
{
	int nPadLen, nPlainLen;
	BYTE dest_buf[8];
	const BYTE *iv_buf;
	int dest_i, i, j;

	//if (nInBufLen%8) return FALSE; BUG!
	if ((nInBufLen%8) || (nInBufLen<16)) return FALSE;

	TeaDecryptECB(pInBuf, pKey, dest_buf);
	nPadLen = dest_buf[0] & 0x7/*只要最低三位*/;

	/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(8byte)*/
	i = nInBufLen-1/*PadLen(1byte)*/-nPadLen-SALT_LEN-ZERO_LEN; /*明文长度*/
	if (*pOutBufLen<i) return FALSE;
	*pOutBufLen = i;
	if (*pOutBufLen < 0) return FALSE;

	iv_buf = pInBuf; /*init iv*/
	nInBufLen -= 8;
	pInBuf += 8;

	dest_i=1; /*dest_i指向dest_buf下一个位置*/

	/*把Padding滤掉*/
	dest_i+=nPadLen;
	/*dest_i must <=8*/
	/*把Salt滤掉*/
	for (i=1; i<=SALT_LEN;)
	{
		if (dest_i<8)
		{
			dest_i++;
			i++;
		}
		else if (dest_i==8)
		{
			/*dest_i==8*/
			TeaDecryptECB(pInBuf, pKey, dest_buf);
			for (j=0; j<8; j++)
				dest_buf[j]^=iv_buf[j];
			iv_buf = pInBuf;
			nInBufLen -= 8;
			pInBuf += 8;
			dest_i=0; /*dest_i指向dest_buf下一个位置*/
		}
	}

	/*还原明文*/
	nPlainLen=*pOutBufLen;
	while (nPlainLen)
	{
		if (dest_i<8)
		{
			*(pOutBuf++)=dest_buf[dest_i++];
			nPlainLen--;
		}
		else if (dest_i==8)
		{
			/*dest_i==8*/
			TeaDecryptECB(pInBuf, pKey, dest_buf);
			for (i=0; i<8; i++)
				dest_buf[i]^=iv_buf[i];

			iv_buf = pInBuf;
			nInBufLen -= 8;
			pInBuf += 8;
			dest_i=0; /*dest_i指向dest_buf下一个位置*/
		}
	}

	/*校验Zero*/
	for (i=1;i<=ZERO_LEN;)
	{
		if (dest_i<8)
		{
			if(dest_buf[dest_i++]) return FALSE;
			i++;
		}
		else if (dest_i==8)
		{
			/*dest_i==8*/
			TeaDecryptECB(pInBuf, pKey, dest_buf);
			for (j=0; j<8; j++)
				dest_buf[j]^=iv_buf[j];

			iv_buf = pInBuf;
			nInBufLen -= 8;
			pInBuf += 8;
			dest_i=0; /*dest_i指向dest_buf下一个位置*/
		}
	}
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
/*pKey为16byte*/
/*
	输入:pInBuf为需加密的明文部分(Body),nInBufLen为pInBuf长度;
	输出:pOutBuf为密文格式,pOutBufLen为pOutBuf的长度是8byte的倍数;
*/
/*TEA加密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(8byte)*/
void oi_symmetry_encrypt2(const BYTE* pInBuf, int nInBufLen, const BYTE* pKey, BYTE* pOutBuf, int *pOutBufLen)
{
	int nPadSaltBodyZeroLen/*PadLen(1byte)+Salt+Body+Zero的长度*/;
	int nPadlen;
	BYTE src_buf[8], iv_plain[8], *iv_crypt;
	int src_i, i, j;

	/*根据Body长度计算PadLen,最小必需长度必需为8byte的整数倍*/
	nPadSaltBodyZeroLen = nInBufLen/*Body长度*/+1+SALT_LEN+ZERO_LEN/*PadLen(1byte)+Salt(2byte)+Zero(8byte)*/;
	if((nPadlen=nPadSaltBodyZeroLen%8)) /*len=nSaltBodyZeroLen%8*/
	{
		/*模8余0需补0,余1补7,余2补6,...,余7补1*/
		nPadlen=8-nPadlen;
	}

	/*srand( (unsigned)time( NULL ) ); 初始化随机数*/
	/*加密第一块数据(8byte),取前面10byte*/
	src_buf[0] = (((BYTE)rand()) & 0x0f8/*最低三位存PadLen,清零*/) | (BYTE)nPadlen;
	src_i = 1; /*src_i指向src_buf下一个位置*/

	while(nPadlen--)
		src_buf[src_i++]=(BYTE)rand(); /*Padding*/

	/*come here, src_i must <= 8*/
	for ( i=0; i<8; i++)
		iv_plain[i] = 0;
	iv_crypt = iv_plain; /*make zero iv*/

	*pOutBufLen = 0; /*init OutBufLen*/
	for (i=1;i<=SALT_LEN;) /*Salt(2byte)*/
	{
		if (src_i<8)
		{
			src_buf[src_i++]=(BYTE)rand();
			i++; /*i inc in here*/
		}

		if (src_i==8)
		{
			/*src_i==8*/
    		for (j=0;j<8;j++) /*加密前异或前8个byte的密文(iv_crypt指向的)*/
				src_buf[j]^=iv_crypt[j];

			/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
			/*加密*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);
			for (j=0;j<8;j++) /*加密后异或前8个byte的明文(iv_plain指向的)*/
				pOutBuf[j]^=iv_plain[j];

			/*保存当前的iv_plain*/
			for (j=0;j<8;j++)
				iv_plain[j]=src_buf[j];

			/*更新iv_crypt*/
			src_i=0;
			iv_crypt=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}

	/*src_i指向src_buf下一个位置*/
	while(nInBufLen)
	{
		if (src_i<8)
		{
			src_buf[src_i++]=*(pInBuf++);
			nInBufLen--;
		}

		if (src_i==8)
		{
			/*src_i==8*/
			
			for (j=0;j<8;j++) /*加密前异或前8个byte的密文(iv_crypt指向的)*/
				src_buf[j]^=iv_crypt[j];

			/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);

			for (j=0;j<8;j++) /*加密后异或前8个byte的明文(iv_plain指向的)*/
				pOutBuf[j]^=iv_plain[j];

			/*保存当前的iv_plain*/
			for (j=0;j<8;j++)
				iv_plain[j]=src_buf[j];

			src_i=0;
			iv_crypt=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}

	/*src_i指向src_buf下一个位置*/

	for (i=1;i<=ZERO_LEN;)
	{
		if (src_i<8)
		{
			src_buf[src_i++]=0;
			i++; /*i inc in here*/
		}

		if (src_i==8)
		{
			/*src_i==8*/
			for (j=0;j<8;j++) /*加密前异或前8个byte的密文(iv_crypt指向的)*/
				src_buf[j]^=iv_crypt[j];

			/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);

			for (j=0;j<8;j++) /*加密后异或前8个byte的明文(iv_plain指向的)*/
				pOutBuf[j]^=iv_plain[j];

	    	/*保存当前的iv_plain*/
			for (j=0;j<8;j++)
				iv_plain[j]=src_buf[j];

			src_i=0;
			iv_crypt=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}
}

/*pKey为16byte*/
/*
	输入:pInBuf为密文格式,nInBufLen为pInBuf的长度是8byte的倍数;
	输出:pOutBuf为明文(Body),pOutBufLen为pOutBuf的长度;
	返回值:如果格式正确返回TRUE;
*/
/*TEA解密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(8byte)*/
BOOL oi_symmetry_decrypt2(const BYTE* pInBuf, int nInBufLen, const BYTE* pKey, BYTE* pOutBuf, int *pOutBufLen)
{
	int nPadLen, nPlainLen;
	BYTE dest_buf[8], zero_buf[8];
	const BYTE *iv_pre_crypt, *iv_cur_crypt;
	int dest_i, i, j;

	//if (nInBufLen%8) return FALSE; BUG!
	if ((nInBufLen%8) || (nInBufLen<16)) return FALSE;

	TeaDecryptECB(pInBuf, pKey, dest_buf);
	nPadLen = dest_buf[0] & 0x7/*只要最低三位*/;

	/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(8byte)*/
	i = nInBufLen-1/*PadLen(1byte)*/-nPadLen-SALT_LEN-ZERO_LEN; /*明文长度*/
	if (*pOutBufLen<i) return FALSE;
	*pOutBufLen = i;
	if (*pOutBufLen < 0) return FALSE;

	for ( i=0; i<8; i++)
		zero_buf[i] = 0;

	iv_pre_crypt = zero_buf;
	iv_cur_crypt = pInBuf; /*init iv*/
	nInBufLen -= 8;
	pInBuf += 8;

	dest_i=1; /*dest_i指向dest_buf下一个位置*/

	/*把Padding滤掉*/
	dest_i+=nPadLen;
	/*dest_i must <=8*/
	/*把Salt滤掉*/
	for (i=1; i<=SALT_LEN;)
	{
		if (dest_i<8)
		{
			dest_i++;
			i++;
		}
		else if (dest_i==8)
		{
			/*解开一个新的加密块*/
			/*改变前一个加密块的指针*/
			iv_pre_crypt = iv_cur_crypt;
			iv_cur_crypt = pInBuf; 

			/*异或前一块明文(在dest_buf[]中)*/
			for (j=0; j<8; j++)
				dest_buf[j]^=pInBuf[j];

			/*dest_i==8*/
			TeaDecryptECB(dest_buf, pKey, dest_buf);

			/*在取出的时候才异或前一块密文(iv_pre_crypt)*/
			nInBufLen -= 8;
			pInBuf += 8;
			dest_i=0; /*dest_i指向dest_buf下一个位置*/
		}
	}

	/*还原明文*/
	nPlainLen=*pOutBufLen;
	while (nPlainLen)
	{
		if (dest_i<8)
		{
			*(pOutBuf++)=dest_buf[dest_i]^iv_pre_crypt[dest_i];
			dest_i++;
			nPlainLen--;
		}
		else if (dest_i==8)
		{
			/*dest_i==8*/
			/*改变前一个加密块的指针*/
			iv_pre_crypt = iv_cur_crypt;
			iv_cur_crypt = pInBuf; 

			/*解开一个新的加密块*/
			/*异或前一块明文(在dest_buf[]中)*/
			for (j=0; j<8; j++)
				dest_buf[j]^=pInBuf[j];

			TeaDecryptECB(dest_buf, pKey, dest_buf);
			/*在取出的时候才异或前一块密文(iv_pre_crypt)*/
			nInBufLen -= 8;
			pInBuf += 8;
			dest_i=0; /*dest_i指向dest_buf下一个位置*/
		}
	}

	/*校验Zero*/
	for (i=1;i<=ZERO_LEN;)
	{
		if (dest_i<8)
		{
			if(dest_buf[dest_i]^iv_pre_crypt[dest_i]) return FALSE;
			dest_i++;
			i++;
		}
		else if (dest_i==8)
		{
			/*改变前一个加密块的指针*/
			iv_pre_crypt = iv_cur_crypt;
			iv_cur_crypt = pInBuf; 
			/*解开一个新的加密块*/

    		/*异或前一块明文(在dest_buf[]中)*/
			for (j=0; j<8; j++)
				dest_buf[j]^=pInBuf[j];

			TeaDecryptECB(dest_buf, pKey, dest_buf);

    		/*在取出的时候才异或前一块密文(iv_pre_crypt)*/

			nInBufLen -= 8;
			pInBuf += 8;
			dest_i=0; /*dest_i指向dest_buf下一个位置*/
		}
	}
	return TRUE;
}

bool MakeUserSignature4(uint32_t dwUserId, uint32_t timestamp, char* sSignature, short& shSigLen, const char* pszKey)
{
    char achDecryptBuf[sizeof(dwUserId) + sizeof(timestamp) + 8];       // 8 bytes just for padding
    memset(achDecryptBuf, 0, sizeof(achDecryptBuf));
    int iDecryptBufLen = sizeof(achDecryptBuf);

    dwUserId = htonl(dwUserId);
    timestamp = htonl(timestamp);
    memset(achDecryptBuf, 0, sizeof(achDecryptBuf));
    memcpy(achDecryptBuf, &dwUserId, sizeof(dwUserId));
    memcpy(achDecryptBuf + sizeof(dwUserId), &timestamp, sizeof(timestamp));
    int iSigLen = 0;

    oi_symmetry_encrypt2(achDecryptBuf, iDecryptBufLen, pszKey, sSignature, &iSigLen);

    shSigLen = iSigLen;
    return true;
}

char* bintostr(const unsigned char* sBin, char* sAscii, int binlen)
{
        for(int i=0; i<binlen; i++)
                sprintf(&sAscii[i*2], "%02x", (unsigned char)sBin[i]);
        return sAscii;
}


string GetLoginKey(u_int32_t uiUserID)
{
    char signature[512];
	char hexsignature[1024];

	memset(signature, 0, sizeof(signature));
	memset(hexsignature, 0, sizeof(hexsignature));		

	short sign_len = 512;	   

	MakeUserSignature4(uiUserID, time(NULL), signature,
	                     	sign_len, TIPS_KEY);
	
	bintostr((const unsigned char *)signature, hexsignature, sign_len);
	 
	return hexsignature;
}


bool MakeVerifyCode(std::string sIn, std::string& sOut, std::string sKey)
{
    char achEncryptBuffer[1024] = {0};
    int iEncryptBufferLen = sizeof(achEncryptBuffer);
    oi_symmetry_encrypt2(sIn.c_str(), sIn.length(), sKey.c_str(), achEncryptBuffer, &iEncryptBufferLen);      // return void

	char sTmp[2048 + 1] = {0};

	for(int i = 0; i < iEncryptBufferLen; i++)
	{
	    sprintf(&sTmp[i * 2], "%02x", (u_int8_t)achEncryptBuffer[i]);
	}

    sOut.assign(sTmp, iEncryptBufferLen * 2);

	return true;
}

bool ParseVerifyCode(std::string sIn, std::string& sOut, std::string sKey)
{
    if(sIn.length() % 2 != 0)
		return false;

	string sRealIn;
	for(unsigned int i = 0; i < sIn.length()/2; i++)
	{
	    char sTmp[3];
		sTmp[0] = sIn.c_str()[i * 2]; sTmp[1] = sIn.c_str()[i * 2 + 1]; sTmp[2] = '\0';

		int j;
		if(sscanf(sTmp, "%02X", &j) != 1)
			return false;
		
		sRealIn += (char)j;
	}
	
    char achDecryptBuffer[1024] = {0};
    int iDecryptBufferLen = sizeof(achDecryptBuffer);
    if(!oi_symmetry_decrypt2(sRealIn.c_str(), sRealIn.length(), sKey.c_str(), achDecryptBuffer, &iDecryptBufferLen))
    {
        return false;
    }
    else
    {
        achDecryptBuffer[iDecryptBufferLen] = '\0';
        sOut.assign(achDecryptBuffer, iDecryptBufferLen + 1);
        return true;
    }
}

