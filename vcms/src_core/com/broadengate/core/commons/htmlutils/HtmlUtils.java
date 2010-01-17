/**
 * 
 */
package com.broadengate.core.commons.htmlutils;

/**
 * @author ��b��
 * 
 */
public class HtmlUtils {

	public static String convertToHtml(String content) {
		content = content.replaceAll("\t", "&nbsp;&nbsp;&nbsp;&nbsp;");
		content = content.replaceAll(" ", "&nbsp;");
		content = content.replaceAll("\n", "<br>");
		return content;
	}

	/**
	 * 将含有html标签的string转化成纯文本
	 * 
	 * @param html
	 * @return
	 */
	public static String convertToString(String html) {
		return html.replaceAll("<[^>]*>", "");
	}

}
