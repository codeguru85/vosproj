#include "url_decode.h"

static char Char2Int(char ch)
{        
    if(ch >= '0' && ch <= '9')
		return (char)(ch - '0');   
	if(ch >= 'a' && ch <= 'f')
		return (char)(ch - 'a' + 10);        
	if(ch >= 'A' && ch <= 'F')
		return (char)(ch - 'A' + 10);        
	return -1;
}

static char Str2Bin(char *str)
{   
    assert(str);
	
    char tempWord[2];        
	char chn;        

	tempWord[0] = Char2Int(str[0]);  //make the B to 11 -- 00001011        
    tempWord[1] = Char2Int(str[1]);  //make the 0 to 0  -- 00000000        

	chn = (tempWord[0] << 4) | tempWord[1];  //to change the BO to 10110000        

	return chn;
}

static string UrlDecode(const string &str)
{        
    string output = "";        
	char tmp[2];        
    int i=0, idx=0, ndx, len = str.length();  
	
	while(i < len)
    {                
        if('%' == str[i])
		{   
		    if(i + 2 < len
            && (
              (str[i + 1] >= '0' && str[i + 1] <= '9')
              || (str[i + 1] >= 'a' && str[i + 1] <= 'f')
              || (str[i + 1] >= 'A' && str[i + 1] <= 'F')
             )
            && (
              (str[i + 2] >= '0' && str[i + 2] <= '9')
              || (str[i + 2] >= 'a' && str[i + 2] <= 'f')
              || (str[i + 2] >= 'A' && str[i + 2] <= 'F')
             )
			)
		    {
		        tmp[0] = str[i+1];           
			    tmp[1] = str[i+2];                        

			    output += Str2Bin(tmp);                        
				i = i+3;
		    }
			else
			{
			    output += str[i];                        
			    i++;
			}
			
	    }                
        else if(str[i] == '+')
		{   
		    output += ' ';                        
			i++;                
		}
		else
        {
            output += str[i];                        
			i++;                
		}        
	}
	return output;
}

CodeConverter::CodeConverter(const char *from_charset,const char *to_charset) 
{                                
    cd = iconv_open(to_charset,from_charset);                        
}  

CodeConverter::~CodeConverter() 
{
    iconv_close(cd);                        
}
	
int CodeConverter::convert(char *inbuf,int inlen,char *outbuf,int outlen) 
{ 
    assert((iconv_t)-1 != cd);
	
    char **pin = &inbuf;                                
	char **pout = &outbuf;                                
	memset(outbuf, 0, outlen);                                
	return iconv(cd, pin, (size_t *)&inlen, pout, (size_t *)&outlen); 
}

string URL_Decode_UTF8(const string &instr)
{        
    string input = UrlDecode(instr);        
	const int outlen = instr.length();        
	char output[outlen];        
	CodeConverter cc("UTF-8", "GB2312");        
	cc.convert((char *)input.c_str(),strlen(input.c_str()),output,outlen);
	return output;
} 

string URL_Decode_GB2312(const string &str)
{        
    return UrlDecode(str);
}



