/**
 * 
 */
package com.broadengate.core.commons.time;

import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * @author 龚磊
 * 
 */
public class DateUtils extends org.apache.commons.lang.time.DateUtils {
	private static String defaultPattern = "yyyy-MM-dd";

	public final static SimpleDateFormat FORMAT_TIMESTAMP = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

	public final static SimpleDateFormat FORMAT_TIME = new SimpleDateFormat("yyyy-MM-dd HH:mm");

	public final static SimpleDateFormat FORMAT_HOUR = new SimpleDateFormat("yyyy-MM-dd HH");

	public final static SimpleDateFormat FORMAT_DATE = new SimpleDateFormat("yyyy-MM-dd");

	public final static SimpleDateFormat FORMAT_DIR = new SimpleDateFormat("yyyy/MM/dd/");

	/**
	 * 根据pattern判断字符串是否为合法日期
	 * 
	 * @param dateStr
	 * @param pattern
	 * @return
	 */
	public static boolean isValidDate(String dateStr, String pattern) {
		boolean isValid = false;
		String patterns = "yyyy-MM-dd,MM/dd/yyyy";

		if (pattern == null || pattern.length() < 1) {
			pattern = "yyyy-MM-dd";
		}
		try {
			SimpleDateFormat sdf = new SimpleDateFormat(pattern);
			// sdf.setLenient(false);
			String date = sdf.format(sdf.parse(dateStr));
			if (date.equalsIgnoreCase(dateStr)) {
				isValid = true;
			}
		} catch (Exception e) {
			isValid = false;
		}
		// 如果目标格式不正确，判断是否是其它格式的日期
		if (!isValid) {
			isValid = isValidDatePatterns(dateStr, "");
		}
		return isValid;
	}

	public static boolean isValidDatePatterns(String dateStr, String patterns) {
		if (patterns == null || patterns.length() < 1) {
			patterns = "yyyy-MM-dd;dd/MM/yyyy;yyyy/MM/dd;yyyy/M/d h:mm";
		}
		boolean isValid = false;
		String[] patternArr = patterns.split(";");
		for (int i = 0; i < patternArr.length; i++) {
			try {
				SimpleDateFormat sdf = new SimpleDateFormat(patternArr[i]);
				// sdf.setLenient(false);
				String date = sdf.format(sdf.parse(dateStr));
				if (date.equalsIgnoreCase(dateStr)) {
					isValid = true;
					DateUtils.defaultPattern = patternArr[i];
					break;
				}
			} catch (Exception e) {
				isValid = false;
			}
		}
		return isValid;
	}

	public static String getFormatDate(String dateStr, String pattern) {
		if (pattern == null || pattern.length() < 1) {
			pattern = "yyyy-MM-dd";
		}
		try {
			SimpleDateFormat sdf = new SimpleDateFormat(DateUtils.defaultPattern);
			SimpleDateFormat format = new SimpleDateFormat(pattern);
			String date = format.format(sdf.parse(dateStr));
			return date;
		} catch (Exception e) {
			System.out.println("日期格转换失败！");
		}
		return null;
	}

	public static String getFormatDate(Date date, String pattern) {
		if (pattern == null || pattern.length() < 1) {
			pattern = "yyyy-MM-dd";
		}
		try {
			SimpleDateFormat sdf = new SimpleDateFormat(pattern);
			String strDate = sdf.format(date);
			return strDate;
		} catch (Exception e) {
			System.out.println("日期格转换失败！");
		}
		return null;
	}

	public static Date parseDate(String s) {
		DateFormat df = DateFormat.getDateInstance();
		Date myDate = null;
		try {
			myDate = df.parse(s);
		} catch (ParseException e) {
			e.printStackTrace();
		}
		return myDate;
	}

	public static Date getNowDate(String pattern) {
		if (pattern == null || pattern.length() < 1) {
			pattern = "yyyy-MM-dd";
		}
		try {
			Date date = new Date();
			SimpleDateFormat sdf = new SimpleDateFormat(pattern);
			String strDate = sdf.format(date);
			Date nowDate = sdf.parse(strDate);
			return nowDate;
		} catch (Exception e) {
			System.out.println("无法获得当前日期！");
		}
		return null;
	}

	public static void main(String[] args) {

		// boolean isd = DateUtils.isValidDate("08/09/2007", "yyyy-MM-dd");
		// if(isd){
		// String date = DateUtils.getFormatDate("08/09/2007", "yyyy-MM-dd");
		// System.out.println(date);
		// }
		// System.out.println(date);

		DateFormat df = DateFormat.getDateInstance();
		try {
			Date myDate = df.parse("2007-07-09");
			System.out.println(myDate.toString());
		} catch (ParseException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}
}
