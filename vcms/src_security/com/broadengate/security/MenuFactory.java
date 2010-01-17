/**
 * 
 */
package com.broadengate.security;

import java.io.File;

import javax.servlet.ServletContext;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.io.SAXReader;

import com.broadengate.core.ContextUtil;

/**
 * 单例的菜单树工厂
 * 
 * @author 龚磊
 * 
 */
public class MenuFactory {
	/**
	 * Logger for this class
	 */
	private static final Logger logger = Logger.getLogger(MenuFactory.class);

	private static MenuFactory menuFactory = new MenuFactory();

	private static Document document;
	
	private static Document documentScontrol;

	private MenuFactory() {

	}

	public static MenuFactory getInstance() {
		return menuFactory;
	}

	public synchronized Document getMenuXML() {
		if (document == null) {
			SAXReader reader = new SAXReader();
			ServletContext context = ContextUtil.getServletContext();
			File file = new File(context.getRealPath(context.getInitParameter("menuConfigLocation")));
			try {
				document = reader.read(file);
			} catch (Exception e) {
				logger.info("load menu error");
			}
		}
		return document;
	}
	
	public synchronized Document getMenuXML(String path) {
		if (documentScontrol == null) {
			SAXReader reader = new SAXReader();
			ServletContext context = ContextUtil.getServletContext();
			File file = new File(context.getRealPath(context.getInitParameter("scontrolConfigLocation")));
			try {
				documentScontrol = reader.read(file);
			} catch (Exception e) {
				logger.info("load menu error");
			}
		}
		return documentScontrol;
	}

}
