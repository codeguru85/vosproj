#include "buffered_file_reader.h"

BufferedFileReader::BufferedFileReader()
{
    m_iFD = -1;
    m_iBufLen = 0;

	m_iStartNum = 0;
	m_iEndNum = 0;

	m_iBytes = 0;
	m_iLines = 0;

	m_psData = NULL;
}


BufferedFileReader::~BufferedFileReader()
{
    Close();
}

void BufferedFileReader::Close()
{
    if(m_iFD >= 0)
    {
        close(m_iFD);
		m_iFD = -1;

		//BOSS_INFO("Close File %s", m_sFilePath.c_str());
    }

	if(m_psData)
	{
	    delete []m_psData;
		m_psData = NULL;
		m_iBufLen = 0;
	}
}


int BufferedFileReader::Open(const string &sFilePath, int iBufLen)
{
    assert(iBufLen > 0);
	
    Close();

	m_sFilePath = sFilePath;
    m_iFD = open(sFilePath.c_str(), O_RDONLY);
    if(-1 == m_iFD)
	{
	    //BOSS_ERROR("open file[%s] error: %s\n", sFilePath.c_str(), strerror(errno));
		return -1;
	}

    m_psData = new char[iBufLen + 1];
	if(NULL == m_psData)
	{
	    BOSS_ERROR("new[%d bytes] error: %s\n", iBufLen + 1, strerror(errno));
		Close();
		return -1;
	}
	m_iBufLen = iBufLen;

	//BOSS_INFO("Open File %s", m_sFilePath.c_str());
	
	return 0;
}

char* BufferedFileReader::ReadLine()
{
    if(-1 == m_iFD)
		return NULL;

    while(true)
    {
        if(0 == m_iStartNum)
        {
            int iReadNum = read(m_iFD, m_psData + m_iEndNum, m_iBufLen - m_iEndNum);
			if(iReadNum < 0)
				return NULL;
			
            m_iBytes += iReadNum;
			
	        if(0 == iReadNum)
	        {
	            if(0 == m_iEndNum)
	                return NULL;
		        else
		        {
		            m_psData[m_iEndNum] = '\0';
				    m_iEndNum = 0;
				    m_iLines++;
					
				    return m_psData;
		        }
	        }
		    else
	        {
		        m_iEndNum += iReadNum;

				m_psData[m_iEndNum] = '\0';
	        }
        }

	    int iLinePos = m_iStartNum;
	    int iIdx;
	    for(iIdx = m_iStartNum; iIdx < m_iEndNum; iIdx++)
	    {
	        if(m_psData[iIdx] == '\n')
	        {
			    m_psData[iIdx] = '\0';
				
			    m_iStartNum = iIdx + 1;
                if(m_iStartNum >= m_iEndNum)
                {
                    m_iStartNum = m_iEndNum = 0;
                }
				m_iLines++;
			    return m_psData + iLinePos;
	        }
	    }

	    if(0 == m_iStartNum && iIdx >= m_iEndNum)
	    {
	        m_psData[m_iEndNum] = '\0';
		    m_iStartNum = m_iEndNum = 0;

			m_iLines++;
		    return m_psData;
	    }
	    else if(iIdx >= m_iEndNum)
	    {
	        memmove(m_psData, m_psData + m_iStartNum, m_iEndNum - m_iStartNum);
		    m_iEndNum = m_iEndNum - m_iStartNum;
		    m_iStartNum = 0;
	    }		 
	}
}
