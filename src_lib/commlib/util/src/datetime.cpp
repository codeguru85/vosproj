#include "datetime.h"

namespace boss
{
namespace util
{

std::ostream& operator <<(std::ostream &os, const Day &cDay)
{
	    os<<cDay.m_iDays;
		return os;		
}



std::ostream& operator <<(std::ostream &os, const Week &cWeek)
{
	    os<<cWeek.m_iWeeks;
		return os;		
}

std::ostream& operator <<(std::ostream &os, const DateTime &cDateTime)
{
	os<<cDateTime.GetDateTimeStr();
    return os;
}

DateTime operator-(const DateTime &cDateTime, const Day &cDay)
{
    DateTime cDateTime1(cDateTime);
		
    cDateTime1.m_uSeconds -= cDay.GetDays() * 3600 *24;

	return cDateTime1;
}

DateTime operator+(const DateTime &cDateTime, const Day &cDay)
{
    DateTime cDateTime1(cDateTime);
		
    cDateTime1.m_uSeconds += cDay.GetDays() * 3600 *24;

	return cDateTime1;
}

DateTime operator-(const DateTime &cDateTime, const Week &cWeek)
{
    DateTime cDateTime1(cDateTime);
		
    cDateTime1.m_uSeconds -= cWeek.GetWeeks() * 3600 * 24 * 7;

	return cDateTime1;
}

DateTime operator+(const DateTime &cDateTime, const Week &cWeek)
{
    DateTime cDateTime1(cDateTime);
		
    cDateTime1.m_uSeconds += cWeek.GetWeeks() * 3600 * 24 * 7;

	return cDateTime1;
}


std::ostream& operator <<(std::ostream &os, const Date &cDate)
{
	    os<<cDate.GetDateStr();
		return os;
}

Date operator-(const Date &cDate, const Day &cDay)
{
    Date cDate1(cDate);
		
    cDate1.m_uSeconds -= cDay.GetDays() * 3600 *24;

	return cDate1;
}

Date operator+(const Date &cDate, const Day &cDay)
{
    Date cDate1(cDate);
		
    cDate1.m_uSeconds += cDay.GetDays() * 3600 *24;

	return cDate1;
}

Date operator-(const Date &cDate, const Week &cWeek)
{
    Date cDate1(cDate);
		
    cDate1.m_uSeconds -= cWeek.GetWeeks() * 3600 * 24 * 7;

	return cDate1;
}

Date operator+(const Date &cDate, const Week &cWeek)
{
    Date cDate1(cDate);
		
    cDate1.m_uSeconds += cWeek.GetWeeks() * 3600 * 24 * 7;

	return cDate1;
}

DateTime operator -(const DateTime &cDateTime, const Month &cMonth)
{
    time_t uSeconds = cDateTime.m_uSeconds;
	struct tm* pstTm = localtime(&uSeconds);

	int iYear = pstTm->tm_year + 1900;
	int iMonth = pstTm->tm_mon + 1;
	int iDay = pstTm->tm_mday;
	int iHour = pstTm->tm_hour;
	int iMin = pstTm->tm_min;
	int iSec = pstTm->tm_sec;

	int iMonths = 12 * iYear + iMonth - cMonth.GetMonths();
	iYear = iMonths / 12;
	iMonth = iMonths % 12;

	char sTmp[100];
	snprintf(sTmp, sizeof(sTmp), "%d-%02d-%02d %02d:%02d:%02d", iYear, iMonth, iDay, iHour, iMin, iSec);

	return DateTime(sTmp);
	
}

DateTime operator +(const DateTime &cDateTime, const Month &cMonth)
{
    time_t uSeconds = cDateTime.m_uSeconds;
	struct tm* pstTm = localtime(&uSeconds);

	int iYear = pstTm->tm_year + 1900;
	int iMonth = pstTm->tm_mon + 1;
	int iDay = pstTm->tm_mday;
	int iHour = pstTm->tm_hour;
	int iMin = pstTm->tm_min;
	int iSec = pstTm->tm_sec;

	int iMonths = 12 * iYear + iMonth + cMonth.GetMonths();
	iYear = iMonths / 12;
	iMonth = iMonths % 12;

	char sTmp[100];
	snprintf(sTmp, sizeof(sTmp), "%d-%02d-%02d %02d:%02d:%02d", iYear, iMonth, iDay, iHour, iMin, iSec);

	return DateTime(sTmp);
}

Date operator -(const Date &cDate, const Month &cMonth)
{
    time_t uSeconds = cDate.m_uSeconds;
	struct tm* pstTm = localtime(&uSeconds);

	int iYear = pstTm->tm_year + 1900;
	int iMonth = pstTm->tm_mon + 1;
	int iDay = pstTm->tm_mday;

	int iMonths = 12 * iYear + iMonth - cMonth.GetMonths();
	iYear = iMonths / 12;
	iMonth = iMonths % 12;

	char sTmp[100];
	snprintf(sTmp, sizeof(sTmp), "%d-%02d-%02d", iYear, iMonth, iDay);

    
	return Date(sTmp);
}

Date operator +(const Date &cDate, const Month &cMonth)
{
    time_t uSeconds = cDate.m_uSeconds;
	struct tm* pstTm = localtime(&uSeconds);

	int iYear = pstTm->tm_year + 1900;
	int iMonth = pstTm->tm_mon + 1;
	int iDay = pstTm->tm_mday;

	int iMonths = 12 * iYear + iMonth + cMonth.GetMonths();
	iYear = iMonths / 12;
	iMonth = iMonths % 12;

	char sTmp[100];
	snprintf(sTmp, sizeof(sTmp), "%d-%02d-%02d", iYear, iMonth, iDay);

	return Date(sTmp);
}
}
}
