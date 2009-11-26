/************************************************
* Project: Public
* Source: TIniFile.cpp
* Description: TIniFile¿‡ µœ÷
* Version: V1.0
* History:
* 
************************************************/

#include "inifile.h"

TIniFile::TIniFile()
{

}

TIniFile::~TIniFile()
{

}

int TIniFile::getValue( char *ini_file, char *section, char *key, char *value )
{
	FILE *fd;
	int ret;
	int cur_line;

	if ( NULL == ini_file || NULL == section || NULL == key || NULL == value )
	{
		return -1;
	}
	
	fd = fopen( ini_file, "r" );
	if ( NULL == fd ) 
	{
		//writeLog( __FILE__, __LINE__, "Open File Error\n" );
		return -2;
	}

	cur_line = 0;
	ret = findSection( fd, section );
	if ( ret < 0 )
	{
		//writeLog( __FILE__, __LINE__, "AAAAret = [%d]\n ", ret );
		fclose( fd );
		return -3;
	}

	cur_line += ret;

	ret = readValue( fd, key, value );
	if ( ret < 0 )
	{
		//writeLog( __FILE__, __LINE__, "BBBBret = [%d]\n ", ret );
		fclose( fd );
		return -4;
	}
	cur_line += ret;

	fclose(fd);
    Trim(value);
	return cur_line;
}

int TIniFile::getValue( char *ini_file, char *section, char *key, int *value )
{
	char ch_value[VALUELEN];
	int ret;
	unsigned int i;

	if ( NULL == ini_file || NULL == section || NULL == key || NULL == value )
	{
		//writeLog( __FILE__, __LINE__, "Parameter is Null\n" );
		return -1;
	}

	ret = getValue( ini_file, section, key, ch_value );
	if ( ret < 0 )
	{
		//writeLog( __FILE__, __LINE__, "getStrValue Error, ret = [%d]\n", ret );
		return -2;
	}

	for (i = 0; i<strlen(ch_value); i++)
	{
		if ( !isdigit(ch_value[i]) )
		{
			return -3;
		}
	}
	*value = atol( ch_value );
	return 0;
}

int TIniFile::setValue( char *ini_file, char *section, char *key, char *value )
{
	FILE *fd;
	FILE *fd_t;
	char file_name[NAMELEN];
	char new_line[LINELEN];
	char old_value[VALUELEN];

	int ret;
	size_t offset;
	int cur_line;

	if ( NULL == ini_file || NULL == section || NULL == key || NULL == value )
	{
		return -1;
	}
	
	memset( file_name, 0x0, sizeof(file_name) );
	sprintf( file_name, "%s.bak", ini_file );

	fd = fopen( ini_file, "r" );
	if ( NULL == fd ) 
	{
		//writeLog( __FILE__, __LINE__, "Open File Error\n" );
		return -2;
	}
	
	cur_line = 0;
	ret = findSection( fd, section );
	if ( ret < 0 )
	{
		//writeLog( __FILE__, __LINE__, "AAAAret = [%d]\n " );
		fclose( fd );
		return -3;
	}

	cur_line += ret;

	ret = readValue( fd, key, old_value );
	if ( ret < 0 )
	{
		//writeLog( __FILE__, __LINE__, "BBBBret = [%d]\n ", ret );
		fclose( fd );
		return -4;
	}

	cur_line += ret;

	goLine( fd, cur_line-1 );

	offset = ftell( fd );
	
	fd_t = fopen( file_name, "w" );
	if ( NULL == fd_t )
	{
		//writeLog( __FILE__, __LINE__, "Open File [%s] Failure\n", file_name );
		fclose( fd );
		return -5;
	}

	ret = copyFile( fd_t, fd, offset );
	if ( ret != 0 )
	{
		//writeLog( __FILE__, __LINE__, "copyFile Error, ret = [%d]\n", ret );
		fclose(fd);
		fclose(fd_t);
		unlink( file_name );
		return -6;
	}
	
	setValue( fd, new_line, key, value );
	fputs( new_line, fd_t );

	while ( !feof(fd) )
	{
		memset( new_line, 0x0, sizeof(new_line) );
		fgets( new_line, LINELEN, fd );
		fputs( new_line, fd_t );
	}

	rename( file_name, ini_file );
	fclose(fd);
	fclose(fd_t);

	return 0;
}

int TIniFile::setValue( char *ini_file, char *section, char *key, int *value )
{
	char ch_value[VALUELEN];

	if ( NULL == ini_file || NULL == section || NULL == key || NULL == value )
	{
		//writeLog( __FILE__, __LINE__, "Parameter is Null\n" );
		return -1;
	}
	memset( ch_value, 0x0, sizeof(ch_value) );
	sprintf( ch_value, "%d", *value );
	setValue( ini_file, section, key, ch_value );
	return 0;
}

int TIniFile::readLine( FILE *pFile, char *buf, int len )
{
	int iLen;

	memset( buf, 0x0, len );
	fgets( buf, len, pFile );
	iLen = strlen( buf );

	/***End Of File***/
	if ( iLen == 0 ) 
	{
		return -1;
	}

	buf[iLen - 1] = 0x0;
	
	LTrim( buf );

	/***Blank Line***/
	if ( strlen(buf) == 0 ) 
	{
		return -2;
	}

	/***Remark Line***/
	if ( buf[0] == '#' ) 
	{
		return -3;
	}
	/***Section Line***/
	if ( buf[0] == '[' )
	{
		return -4;
	}
	
	return 0;
}

int TIniFile::goLine( FILE *pFile, int line_num )
{
	char linestr[LINELEN];
	int ret;
	int i;

	if ( NULL == pFile )
	{
		return -1;
	}

	fseek( pFile, 0, SEEK_SET );
	i = 0;
	while ( 1 )
	{
		ret = readLine( pFile, linestr, LINELEN );
		if ( -1 == ret )
		{
			break;
		}
		i ++;
		if ( i == line_num )
		{
			break;
		}
	}

	if ( -1 == ret )
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int TIniFile::findSection( FILE *pFile, char *Section )
{
	int ret;
	char linestr[LINELEN];
	int cur_line;

	cur_line = 0;
	while( 1 )
	{
		ret = readLine( pFile, linestr, LINELEN );
		if ( ret == -1 )
		{
			break;
		}
		cur_line ++;
		if ( ret == -2 || ret == -3 )
		{
			continue;
		}

		if ( ret == -4 )
		{
			getMid( linestr, linestr, '[', ']' );
			if ( !strcmp( linestr, Section ) )
			{
				break;
			}
		}
	}

	if ( ret == -4 )
	{
		return cur_line;
	}
	else
	{
		return -1;
	}
}

int TIniFile::findKey( FILE *pFile, char *key )
{
	char cKey[KEYLEN];
	char linestr[LINELEN];
	int ret;
	int cur_line;

	if ( NULL == pFile )
	{
		return -1;
	}

	cur_line = 0;
	while( 1 )
	{
		ret = readLine( pFile, linestr, LINELEN );
		if ( ret == -4 || ret == -1 )
		{
			break;
		}
		cur_line ++;

		if ( ret == -2 || ret == -3 )
		{
			continue;
		}

		if ( ret == 0 )
		{
			getLeft( cKey, linestr, '=' );
			RTrim( cKey );

			if ( !strcmp( cKey, key ) )
			{
				break;
			}
		}
	}

	if ( ret == 0 )
	{
		return cur_line;
	}
	else 
	{
		return -2;
	}
}

int TIniFile::readValue( FILE *pFile, char *key, char *value )
{
	char cKey[KEYLEN];
	char linestr[LINELEN];
	int ret;
	int iRtnCode;
	int cur_line;

	if ( NULL == pFile )
	{
		return -1;
	}

	cur_line = 0;
	while( 1 )
	{
		ret = readLine( pFile, linestr, LINELEN );
		if ( ret == -4 || ret == -1 )
		{
			break;
		}
		cur_line ++;
		if ( ret == -2 || ret == -3 )
		{
			continue;
		}
		if ( ret == 0 )
		{
			iRtnCode = getLeft( cKey, linestr, '=' );
			RTrim( cKey );

			if ( !strcmp( cKey, key ) )
			{
				iRtnCode = getMid( value, linestr, '=', '#' );
				Trim( value );
				break;
			}
		}
	}

	if ( ret == 0 )
	{
		return cur_line;
	}
	else 
	{
		return -2;
	}
}

int TIniFile::setValue( FILE *pFile, char *new_line, char *key, char *value )
{
	char linestr[LINELEN];
	int ret;

	if ( NULL == pFile || NULL == new_line || NULL == key || NULL == value )
	{
		return -1;
	}
	
	ret = readLine( pFile, linestr, LINELEN );
	if ( ret < 0 )
	{
		//writeLog( __FILE__, __LINE__, "readLine error, ret = [%d]\n" );
		return -2;
	}
	
	strcpy( new_line, key );
	strcat( new_line, " = " );
	strcat( new_line, value );
	
	ret = getRight( linestr, linestr, '#' );
	if ( ret != -1 )
	{
		strcat( new_line, "\t#" );
		strcat( new_line, linestr );
	}
	strcat( new_line, "\n" );
	return 0;
}

int TIniFile::copyFile( FILE *dst, FILE *src, int offset )
{
	char buf[TMPLEN];
	int ret;

	if ( NULL == dst || NULL == src )
	{
		return -1;
	}

	fseek( src, 0, SEEK_SET );
	memset( buf, 0x0, TMPLEN );
	ret = read( fileno(src), buf, offset );
	if ( -1 == ret )
	{
		//writeLog( __FILE__, __LINE__, "Read From File Error, ret = [%d]\n", ret );
		return -2;
	}

	ret = write( fileno(dst), buf, offset );
	if ( -1 == ret )
	{
		//writeLog( __FILE__, __LINE__, "Write To File Error, ret = [%d]\n", ret );
		return -3;
	}

	return 0;
}
int TIniFile::LTrim( char *src )
{
	int iLen;
	int i;

	iLen = strlen( src );
	for( i=0; i<iLen; i++ )
	{
		if ( src[i] != ' ' && src[i] != '\t' )
		{
			break;
		}
	}

	strcpy( src, src+i );
	return 0;
}

int TIniFile::RTrim( char *src )
{
	int iLen;
	int i;
	iLen = strlen( src );
	for ( i=iLen-1; i>=0; i-- )
	{
		if( src[i] != ' ' && src[i] != '\t' )
		{
			break;
		}
	}

	src[i+1] = 0x0;
	return 0;
}

int TIniFile::Trim( char *s )
{
	char *pb;
	char *pe;
	char *ps;
	char *pd;

	if (strcmp(s, "") == 0) 
		return 0;
	
	pb = s;
		 
	while (((*pb == ' ') || (*pb == '\t') || (*pb == '\n') || (*pb == '\r')) && (*pb != 0))
	{
		pb ++;
	}
	
	pe = s;
	while (*pe != 0)
	{
		pe ++;
	}
	pe --;
	while ((pe >= s) && ((*pe == ' ') || (*pe == '\t') || (*pe == '\n') || (*pe == '\r')))
	{
		pe --;
	}
	
	ps = pb;
	pd = s;
	while (ps <= pe)
	{
		*pd = *ps;
		ps ++;
		pd ++;
	}
	*pd = 0;
	return 0;
}

int TIniFile::getMid( char *dst, char *src, char b, char e )
{
	int i;
	int j;
	int iLen;

	iLen = strlen( src );

	for ( i=0; i<iLen; i++ )
	{
		if ( src[i] == b )
		{
			break;
		}
	}

	for ( j=0; j<iLen; j++ )
	{
		if ( src[j] == e )
		{
			break;
		}
	}

	strncpy( dst, src+i+1, j-i-1 );
	dst[j-i-1] = 0x0;

	return 0;
}

int TIniFile::getLeft( char *dst, char *src, char ch )
{
	int i;
	int iLen;
	
	iLen = strlen( src );

	for ( i=0; i<iLen; i++ )
	{
		if ( src[i] == ch )
		{
			break;
		}
	}

	if ( i == iLen )
	{
		return -1;
	}

	strncpy( dst, src, i );
	dst[i] = 0x0;

	return 0;
}

int TIniFile::getRight( char *dst, char *src, char ch )
{
	int i;
	int iLen;

	iLen = strlen( src );

	for( i=0; i<iLen; i++ )
	{
		if ( src[i] == ch )
		{
			break;
		}
	}

	if( i == iLen )
	{
		return -1;
	}

	strncpy( dst, src+i+1, iLen-i );
	dst[iLen-i] = 0x0;

	return 0;
}

