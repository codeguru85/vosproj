#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iconv.h>

int      g_nDebug = 0;

int my_iconv( const char *tocode, const char *fromcode,
              char **inbuf, size_t *inbytesleft,
              char **outbuf, size_t *outbytesleft
            )
{
    iconv_t cd = iconv_open(tocode,fromcode); //fromcode => tocode

    if(cd == (iconv_t) -1)
    {
        if( g_nDebug ) printf("iconv_open error!\n");
        return -1;
    }

    if( iconv(cd, inbuf, inbytesleft, outbuf, outbytesleft) == -1 )
    {
        if( g_nDebug ) printf("iconv error!\n");
        return -1;
    }

    return iconv_close(cd);
}

/*====================================================================
 * Function :    Ascii2Binary
 * Purpose  :    将字符串数字转换为二进制数据
 * Input    :    sAscii:字符串数字(十进制) , pBinary:转换后的二进制数据
 * Return   :    pBinary的长度, -1: Error
 =====================================================================*/
int Ascii2Binary(const char *sAscii,void *pBinary)
{
    int     i=0;
    int     nLen;
    int     nTmp;
    char    sBuf[3];
    unsigned char    *ucBinary;

    if(sAscii==NULL || pBinary==NULL)
        return -1;

    //if(!IsHex(sAscii))
    //    return -1;

    ucBinary=(unsigned char*)pBinary;
    nLen=strlen(sAscii);
    sBuf[2]='\0';
    if(nLen%2!=0)
    {
        sBuf[0]='0';
        sBuf[1]=sAscii[0];
        sscanf(sBuf,"%X",&nTmp);
        ucBinary[0]=nTmp;
        i=1;
        ucBinary++;
    }
    while(sAscii[i]!='\0')
    {
        memcpy(sBuf,&sAscii[i],2);
        sscanf(sBuf,"%X",&nTmp);
        ucBinary[0]=nTmp;
        i+=2;
        ucBinary++;
    }
    return (nLen+1)/2;
}

/*====================================================================
 * Function :   IsUtf8
 * Purpose  :   判断是否UTF-8编码  比如：拍拍: %E6%8B%8D%E6%8B%8D
 * Input    :   字符串
 * Return   :   是-true;否-false;
 =====================================================================*/
bool IsUtf8(const char * sBuf)
{
    int lLen=0,lCharCount=0;

    char sTmp[7];
    char b[7];

    int nLen = strlen(sBuf);
    if( sBuf[1] != 'E' || nLen < 9 || nLen%9 != 0 || sBuf[4] == 'E' || sBuf[7] == 'E' )
        return false;

    if( sBuf[0] != '%' || sBuf[3] != '%' || sBuf[6] != '%' )
        return false;

    //特别判断，如果从字符特征来看，每隔3个%都是%E开头，则认为是utf-8
    bool IsUtf8=true;
    for( int i=1; i < nLen; i = i+9 )
    {
    	if(sBuf[i] != 'E')
    	{
    		IsUtf8=true;
    		break;
    	}
    }
    if( IsUtf8 && nLen > 0 ) return true;
    //===============================================================

    //拷贝
    memcpy( sTmp, sBuf+1, 2 );
    memcpy( sTmp+2, sBuf+4, 2 );
    memcpy( sTmp+4, sBuf+7, 2 );
    sTmp[6]='\0';

    memset(b,0x00,sizeof(b));

    if( (lLen=Ascii2Binary(sTmp,b)) <= 0 )
    {
        if( g_nDebug ) printf( "Ascii2Binary error!\n");
        return false;
    }

    for(int i=0;i<lLen;)
    {
        char lByte=b[i++];//to fast operation, ++ now, ready for the following for(;;)

        if(lByte>=0) continue;//>=0 is normal ascii

        if(lByte<(char)0xc0 || lByte>(char)0xfd) return false;

        int lCount=lByte>(char)0xfc?5:lByte>(char)0xf8?4:lByte>(char)0xf0?3:lByte>(char)0xe0?2:1;

        if(i+lCount>lLen) return false;

        for(int j=0;j<lCount;++j,++i)
        {
            if(b[i]>=(char)0xc0)
            return false;
        }
    }
    return true;
}

/*====================================================================
 * Function :   iconv_string
 * Purpose  :   进行GBK,UNION,UTF-8字符编码解码
 * Input    :   编码串 明文解码串 nDebug(可选参数，打印DEBUG日志)
 * Return   :
 =====================================================================*/
int iconv_string(const char * sSrc, char ** sOut, const int nDebug)
{
	char * sRet = *sOut;

    g_nDebug    = nDebug;

    //=========================================================
    int nLen = strlen(sSrc);
    int i = 0;
    int j = 0;

    int nUTF8_GBK_Flag =0; //0-初始化值；1-UTF-8; 2-gbk

    for ( i = 0; i < nLen; )
    {
        if( sSrc[i] == '%' )
        {
            if( i+1 < nLen && sSrc[i+1] == 'u')
            {
                //unicode编码 %u62CD%u62CD
                if( g_nDebug )
                {
                    printf( "unicode string:[%s]\n", sSrc+i );
                }

                if( i+6 > nLen )
                {
                    if( g_nDebug ) printf("unicode string error!\n");
                    return -1;
                }

                char sTmp[5];
                memcpy(sTmp,sSrc+i+2,4);
                sTmp[4]='\0';

                //十六进制->二进制
                char sTmpBin[3];
                size_t  nBinLen=0;
                memset( sTmpBin, 0x00, sizeof(sTmpBin) );
                nBinLen = Ascii2Binary(sTmp,sTmpBin);

                sTmpBin[2]=sTmpBin[0];
                sTmpBin[0]=sTmpBin[1];
                sTmpBin[1]=sTmpBin[2];
                sTmpBin[2]=0;

                //unicode->gbk
                char sTmpGBK[33];
                size_t  nGBKLen = 32;
                memset( sTmpGBK, 0x00, sizeof(sTmpGBK) );

                char * pBin = sTmpBin;
                char * pGBK = sTmpGBK;

                if( my_iconv("GBK","UNICODE",&pBin,(size_t *)&nBinLen,&pGBK,(size_t *)&nGBKLen) < 0 )
                {
                    if( g_nDebug ) printf("my_iconv error!\n");
                    return -1;
                }

                nGBKLen = strlen(sTmpGBK);
                memcpy(sRet+j,sTmpGBK,nGBKLen);

                if( g_nDebug )
                {
                    printf( "union:%s[%d]\n", sTmpGBK,nGBKLen );
                }

                i += 6;
                j += nGBKLen;
            }else{
                if( IsUtf8( sSrc+i ) && nUTF8_GBK_Flag <= 1 )
                {
                    //utf8编码 //%E6%8B%8D %E6%8B%8D
                    if( i+9 > nLen )
                    {
                        if( g_nDebug ) printf("utf8 string error!\n");
                        return -1;
                    }

                    if( g_nDebug )
                    {
                        printf( "utf-8 string:[%s]\n", sSrc+i );
                    }

                    char sTmp[7];
                    memcpy( sTmp  , sSrc+i+1, 2 );
                    memcpy( sTmp+2, sSrc+i+4, 2 );
                    memcpy( sTmp+4, sSrc+i+7, 2 );
                    sTmp[6]=0;

                    //十六进制->二进制
                    char sTmpBin[7];
                    size_t  nBinLen=0;
                    memset( sTmpBin, 0x00, sizeof(sTmpBin) );
                    nBinLen = Ascii2Binary(sTmp,sTmpBin);

                    //utf8->gbk
                    char sTmpGBK[33];
                    size_t  nGBKLen = 32;
                    memset( sTmpGBK, 0x00, sizeof(sTmpGBK) );

                    char * pBin = sTmpBin;
                    char * pGBK = sTmpGBK;

                    if( my_iconv("GBK","UTF-8",&pBin,(size_t *)&nBinLen,&pGBK,(size_t *)&nGBKLen) < 0 )
                    {
                        if( g_nDebug ) printf("my_iconv error!\n");
                        //return -1;
                        nUTF8_GBK_Flag = 2;
                        continue;
                    }else{
                    	nUTF8_GBK_Flag = 1;
                    }

                    nGBKLen = strlen(sTmpGBK);
                    memcpy(sRet+j,sTmpGBK,nGBKLen);

                    if( g_nDebug )
                    {
                        printf( "utf-8:[%s]->[%s](%d)\n", sSrc+i,sTmpGBK,nGBKLen );
                    }

                    i += 9;
                    j += nGBKLen;
                }else{
                    //gbk编码

                    if( g_nDebug )
                    {
                        printf( "gbk string:[%s]\n", sSrc+i );
                    }

                    nUTF8_GBK_Flag = 2;

                    if( i+3 > nLen )
                    {
                        if( g_nDebug ) printf("gbk string error!\n");
                        return -1;
                    }

                    //转换为二进制形式
                    char sTmp[3];
                    memcpy(sTmp,sSrc+i+1,2);
                    sTmp[2]='\0';

                    if( Ascii2Binary(sTmp,sRet+j) <= 0 )
                    {
                        if( g_nDebug ) printf( "Ascii2Binary error!\n");
                        return -1;
                    }
                    i += 3;
                    j++;
                }
            }
        }else{
            sRet[j++] = sSrc[i++];
        }
    }

    if( g_nDebug ) printf( "%s\n", sRet );

    return 0;
}
