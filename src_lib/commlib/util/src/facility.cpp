#include <time.h>
#include <pwd.h>
#include <sys/types.h>
#include <fstream>

#include "md5c.h"
#include "facility.h"
#include "mysql.h"


using namespace std;
using namespace boss;

string Util::timestr( time_t tmt )
{
    char szTime[ 32 ] = {0};
    struct tm p;
    time_t lTime = tmt;
    if ( lTime == 0 ) {
        lTime = time( ( time_t * ) NULL );
    }
    memcpy( &p, localtime( &lTime ), sizeof( p ) ); /*取得当地时间*/
    strftime( szTime, sizeof( szTime ),"%Y-%m-%d %H:%M:%S",&p);
    return szTime;
}

void Util::daemon_init( sighandler_t termfun)
{
    int iMaxFileOpen;
    char sDir[ 256 ] = {0};

    getcwd( sDir, sizeof( sDir ) );
    iMaxFileOpen = getdtablesize();

    if ( fork() != 0 ) {
        exit( 0 );
    }
    if ( setsid() < 0 ) {
        //BOSS_ERROR( "setsid() failed.%s", strerror( errno ) );
        exit( 1 );
    }
    if ( fork() != 0 ) {
        exit( 0 );
    }
    chdir( sDir );
    umask( 0 );
    signal( SIGINT, SIG_IGN );
    signal( SIGHUP, SIG_IGN );
    signal( SIGQUIT, SIG_IGN );
    signal( SIGPIPE, SIG_IGN );
    signal( SIGTTOU, SIG_IGN );
    signal( SIGTTIN, SIG_IGN );
    signal( SIGUSR1, SIG_IGN );
    signal( SIGUSR2, SIG_IGN );
    signal( SIGCHLD, SIG_IGN );

    if ( !termfun ) {
        termfun = &Util::sig_term;
    }
    signal( SIGTERM, termfun );
}


int Util::get_char_byte( const char * Data, int Size )
{
    const unsigned char * p = ( unsigned char * ) Data;
    // check arguments
    if ( Size == 0 )
        Size = strlen( Data );

    if ( p == NULL || Size <= 0 )
        return 0;
    // Chinese 1st byte 0x81-0xFE
    if ( p[ 0 ] < 0x81 || p[ 0 ] > 0xFE )
        return 1;
    // Chinese code size = 2, 4
    if ( Size < 2 )
        return -1;
    // Chinese 2nd byte 0x30-0x39, 0x40-0x7E, 0x80-0xFE
    if ( p[ 1 ] < 0x30 || p[ 1 ] > 0x39 && p[ 1 ] < 0x40 || p[ 1 ] == 0x7F
         || p[ 1 ] > 0xFE )
        return -1;
    // 2 bytes Chinese code
    if ( p[ 1 ] >= 0x40 )
        return 2;
    // Chinese code size = 4
    if ( Size < 4 )
        return -1;
    // Chinese 3rd byte 0x81-0xFE
    if ( p[ 2 ] < 0x81 || p[ 2 ] > 0xFE )
        return -1;
    // Chinese 4th byte 0x30-0x39
    if ( p[ 3 ] < 0x30 || p[ 3 ] > 0x39 )
        return -1;
    return 4;
}

string Util::truncate( int dLen, const char * src, int sLen )
{
    int len = sLen;
    if ( len == 0 )
        len = strlen( src );
    if ( len <= dLen )
        return string( src, len );

    int pos = 0;
    int chSize = 1;

    const char * p = src;

    while ( ( chSize = get_char_byte( p, len ) ) > 0 ) {
        if ( ( pos + chSize ) > dLen )
            break;
        p += chSize;
        len -= chSize;
        pos += chSize;
    }
    return string( src, pos );
}

void Util::md5( const char *skey, const char *in, char *out )
{
    unsigned char line[ 1024 ] ;
    char tmp[ 256 ];
    MD5_CTX md;
    int i = 0;

    snprintf( tmp, sizeof( tmp ), "%s%s", skey, in );
    MD5Init( &md );
    MD5Update( &md, ( unsigned char * ) tmp, strlen( tmp ) );
    MD5Final( line, &md );

    memset( tmp, 0, sizeof( tmp ) );
    for ( i = 0; i < 16; i++ ) {
        snprintf( tmp, sizeof( tmp ), "%02X", line[ i ] );
        strcat( out, tmp );
    }
}

int Util::makedir( const char *sFullName )
{
    char sDir[ 256 ] = {0};
    int iPos = 0;
    int iCnt = 0;
    char *ptr;

    while ( true ) {
        if ( ( ptr = index( sFullName + iPos, '/' ) ) == 0 )
            break;
        iCnt = ptr - sFullName + 1;
        iPos = iCnt;
        bzero( sDir, sizeof( sDir ) );
        strncpy( sDir, sFullName, iCnt );
        if ( mkdir( sDir, 0777 ) < 0 ) {
            if ( errno != EEXIST )
                return -1;
        }
    }
    return 0;
}

int Util::copyfile( const char * src, const char * dest )
{    
    int n = GetFileSize(src);
    if( n < 0)return -1;
    
    ifstream ifile( src );
    ofstream ofile( dest );
    if ( !ifile || !ofile )
        return -1;
 
  	char* ret = static_cast<char*>(calloc(n + 1, 1));	    
    ifile.read(ret,n);
    ofile.write (ret,n);
    free(ret);
    ifile.close();
    ofile.close();
    return 0;
}

int Util::GetFileSize(const char* sFileName) 
{
  std::ifstream f;
  f.open(sFileName);
  if (!f.good()) { return 0; }
  streampos begin_pos = f.tellg();
  f.seekg(0, ifstream::end);
  return f.tellg() - begin_pos;
}

string Util::GetFileContent(const char* sFileName) 
{  
  unsigned int n = GetFileSize(sFileName);   
  if (n == 0) return "";
  char* ret = static_cast<char*>(calloc(n + 1, 1));
  if (ret == NULL) return "";
  std::ifstream f;
  f.open(sFileName);      
  f.read(ret, n);
  f.close();
  string sContent;
  sContent.assign(ret,n);
  free(ret);
  return sContent;   
}

const char * Util::index2(const char *haystack, const char *needle)
{
	int len = strlen(haystack);
	const char * ret = NULL;
	int i = 0;
	for( ; i < len; ++i ){
		if(index(needle,haystack[i])){
			ret = haystack + i;
			break;	
		}
	}
	return ret;
}

string Util::map2str(map<string,string> & ssmap)
{   
    if ( ssmap.empty() ) {
        return "";
    }
    std::map<std::string,std::string>::iterator it = ssmap.begin();
    string d = it->first + "=" + it->second;
    for ( ++it;it != ssmap.end(); ++it ) {
        d += "&"+ it->first + "=" + it->second;
    }
    return d;
}

int Util::str2map(string & s,std::map<std::string,std::string> & ssmap)
{
    vector<string> vec;
    split(s.c_str(),"=&",vec);
    if ( (vec.size() %2) != 0 ) {//非偶数
        return -1;
    }

    for ( unsigned i = 0;i < vec.size();i+=2 ) {
        ssmap.insert(make_pair(vec[i],vec[i+1]));
    }
    return 0;
}

string Util::escape( const char * src )
{

	int len = strlen( src ) * 2 + 1;
	char * buf = new char[ len ];
	bzero( buf, len );

	int iLen = mysql_escape_string(buf,src, strlen( src ));

	buf[iLen+1]='\0';

	string sRet = buf;
	delete [] buf;
	return sRet;
}


int Util::WriteDateFile( const char *sLogCont, const char *sFileName )
{
    char sFileFullName[ 256 ] = {0};
    time_t tTime;
    struct tm tmTime;
    FILE *fpFile = NULL;

    time( &tTime );
    localtime_r( &tTime, &tmTime );

    if ( !sFileName || !*sFileName )
        return -2;

    snprintf( sFileFullName, 255, "%s_%d%02d%02d", sFileName, 1900 + tmTime.tm_year, 1 + tmTime.tm_mon, tmTime.tm_mday );

    struct stat szBuf;
    if ( stat( sFileFullName, &szBuf ) == 0 && time( ( time_t * ) NULL ) - szBuf.st_mtime > 24 * 3600 ) {
        fpFile = fopen( sFileFullName, "w" );
    } else {
        fpFile = fopen( sFileFullName, "a+" );
    }

    if ( !fpFile )
        return -1;

    fprintf( fpFile, "%d-%02d-%02d %02d:%02d:%02d : %s\n",
            1900 + tmTime.tm_year, 1 + tmTime.tm_mon, tmTime.tm_mday,
            tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec, sLogCont );
    fclose( fpFile );

    return 0;
}

string Util::XmlEsacape(string &str)
{
	string sResult="";
	for(unsigned int i=0;i<str.length();i++)
	{
		char c=str[i];
		if('<' == c)
			sResult += "&lt;";
		else if('>' == c)
			sResult += "&gt;";
		else if('&'==c)
			sResult += "&amp;";
		else if('\''==c)
			sResult += "&#39;";
		else if('\"'==c)
			sResult += "&quot;";
		else
			sResult += str.substr(i,1);
	}
	return sResult;
}

string Util::JsonEscape(const string &str)
{
    string sResult="";
    
	for(unsigned int i=0; i<str.length(); i++)
	{
		char c=str[i];

        if('\"' == c)
        {
            sResult += "\\\"";
        }
        else if('\'' == c)
        {
            sResult += "\\\'";
        }
        else if('\\' == c)
        {
            sResult += "\\\\";
        }
        else if('/' == c)
        {
            sResult += "\\/";
        }
        else if('\b' == c)
        {
            sResult += "\\b";
        }
        else if('\f' == c)
        {
            sResult += "\\f";
        }
        else if('\n' == c)
        {
            sResult += "\\n";
        }
        else if('\r' == c)
        {
            sResult += "\\r";
        }
        else if('\t' == c)
        {
            sResult += "\\t";
        }
		else
		{
			sResult += str.substr(i,1);
		}
	}
    
	return sResult;
}

string Util::JsEscape(const string &str)
{
    string sResult="";
    
	for(unsigned int i=0; i<str.length(); i++)
	{
		char c=str[i];

        if('\"' == c)
        {
            sResult += "\\\"";
        }
        else if('\'' == c)
        {
            sResult += "\\\'";
        }
        else if('\\' == c)
        {
            sResult += "\\\\";
        }
        else if('\b' == c)
        {
            sResult += " ";
        }
        else if('\f' == c)
        {
            sResult += " ";
        }
        else if('\n' == c)
        {
            sResult += " ";
        }
        else if('\r' == c)
        {
            sResult += " ";
        }
        else if('\t' == c)
        {
            sResult += " ";
        }
		else
		{
			sResult += str.substr(i,1);
		}
	}
    
	return sResult;
}

