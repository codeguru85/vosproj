package com.broadengate.vcms.web.util;
import org.apache.log4j.Logger;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

public class TimeUtil {
	/**
	 * Logger for this class
	 */
	private static final Logger logger = Logger.getLogger(TimeUtil.class);

	public final static String TIMESTAMP_FORMAT = "yyyy-MM-dd HH:mm:ss";

	public final static SimpleDateFormat formatTimestamp = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

	public final static SimpleDateFormat formatTime = new SimpleDateFormat("yyyy-MM-dd HH:mm");

	public final static SimpleDateFormat formatHour = new SimpleDateFormat("yyyy-MM-dd HH");

	public final static SimpleDateFormat formatDate = new SimpleDateFormat("yyyy-MM-dd");

	public final static SimpleDateFormat formatDir = new SimpleDateFormat("yyyy/MM/dd/");
	
	public final static SimpleDateFormat formatFile = new SimpleDateFormat("yyyyMMddHHmmss");

	// public final static Calendar CurrentCalendar = new GregorianCalendar();

	public static final Date getTimeDate(String dateString) {
		Date date = null;
		try {
			if (dateString != null || dateString.length() > 0) {
				date = formatDate.parse(dateString);
			} else {
				date = new Date();
			}
		} catch (Exception e) {
			logger.warn("getTimeDate(String) - error");
		}
		return date;
	}

	public static final Date getTimeStampDate(String dateString) {
		Date date = null;
		try {
			if (dateString != null || dateString.length() > 0) {
				date = formatTime.parse(dateString);
			} else {
				date = new Date();
			}
		} catch (Exception e) {
			logger.warn("getTimeDate(String) - error");
		}
		return date;
	}

	public static final Date getTimeDate(String dateString, String format) {
		try {
			if (format == null) {
				return getTimeDate(dateString);
			} else {
				return new SimpleDateFormat(format).parse(dateString);
			}
		} catch (ParseException e) {
			return new Date();
		}
	}

	public static final String getTimeString(Date date) {
		String dateString = null;
		try {
			if (date != null) {
				dateString = formatTime.format(date);
			}
		} catch (Exception e) {
			logger.error("转换时间失败");
		}
		return dateString;
	}

	public static final Date getTimestampDate(String dateString) {
		Date date = new Date();
		dateString = dateString.trim();
		try {
			if (dateString != null || dateString.length() != 0) {
				date = formatTimestamp.parse(dateString);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return date;
	}

	/**
	 * ��ʱ��ת��Ϊ�ַ�
	 * 
	 * @param date
	 * @return
	 */
	public static final String getDateString(Date date) {
		String dateString = null;
		try {
			if (date != null) {
				dateString = formatDate.format(date);
			}
		} catch (Exception e) {
			logger.warn("getTimeString(Date) ", e);
		}
		return dateString;
	}

	public static final Date getNowDate() {
		return new Date();
	}

	public static final String getNowDateString() {
		String dateString = null;
		try {
			dateString = formatDate.format(new Date());
		} catch (Exception e) {
		}
		return dateString;
	}

	public static final String getNowDateDir(Date date) {
		String dateString = null;
		try {
			dateString = formatDir.format(date);
		} catch (Exception e) {
		}
		return dateString;
	}
	
	public static final String getFormatDateFileName(Date date){
		String dateString = null;
		try{
			dateString = formatFile.format(date);
		}catch(Exception e){
		}
		return dateString;
	}
	
	public static final String getHourString(Date date) {
		String dateString = null;
		try {
			if (date != null) {
				dateString = formatHour.format(date);
			}
		} catch (Exception e) {
			logger.warn("getTimeString(Date) ", e);
		}
		return dateString;
	}

	public static final String getTimeInterval(long timeInterval) {
		long day = 0;
		long hour = 0;
		long minute = 0;
		if (timeInterval >= 60000) {
			minute = timeInterval / 60000;
			if (minute >= 60) {
				hour = minute / 60;
				minute = minute % 60;
				if (hour >= 24) {
					day = hour / 24;
					hour = hour % 24;
				}
			}
		}
		String temp = day + "天" + hour + "小时" + minute + "分";
		return temp;
	}
	
	/**
	 * 比较两个日期时间
	 * @param startDate
	 * 开始日期 格式 20041121130000
	 * @param endDate
	 * 结束日期 200412041125959 @ return isBiger 如果开始时间在结束时间之前则返回true，否则返回fasle
	 */
	public static boolean compareDateTime(String startDateTime, String endDateTime)
	{
		boolean isBiger = false;
		try
		{

			int sYear = Integer.parseInt(startDateTime.substring(0, 4));
			int sMonth = Integer.parseInt(startDateTime.substring(4, 6));
			int sDay = Integer.parseInt(startDateTime.substring(6, 8));
			int sHour = Integer.parseInt(startDateTime.substring(8, 10));
			int sMinute = Integer.parseInt(startDateTime.substring(10, 12));
			int sSecond = Integer.parseInt(startDateTime.substring(12, 14));

			int eYear = Integer.parseInt(endDateTime.substring(0, 4));
			int eMonth = Integer.parseInt(endDateTime.substring(4, 6));
			int eDay = Integer.parseInt(endDateTime.substring(6, 8));
			int eHour = Integer.parseInt(endDateTime.substring(8, 10));
			int eMinute = Integer.parseInt(endDateTime.substring(10, 12));
			int eSecond = Integer.parseInt(endDateTime.substring(12, 14));

			java.util.Date sDate = new Date(sYear, sMonth, sDay, sHour, sMinute, sSecond);
			java.util.Date eDate = new Date(eYear, eMonth, eDay, eHour, eMinute, eSecond);
			isBiger = eDate.after(sDate);
		}
		catch(Exception e)
		{
		}
		return isBiger;
	}
}
