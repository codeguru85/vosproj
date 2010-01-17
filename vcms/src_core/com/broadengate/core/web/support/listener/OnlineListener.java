/**
 * 
 */
package com.broadengate.core.web.support.listener;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import javax.servlet.http.HttpSessionAttributeListener;
import javax.servlet.http.HttpSessionBindingEvent;

import org.apache.log4j.Logger;
 

/**
 * @author 龚磊
 * 
 */
public class OnlineListener implements HttpSessionAttributeListener {

	private static final Logger logger = Logger.getLogger(OnlineListener.class);

	public static Map onlines;

	public void attributeAdded(HttpSessionBindingEvent event) {
//		if (event.getName().equals(Constants.USER_INFO)) {
//			UserInfo userInfo = (UserInfo) event.getSession().getAttribute(Constants.USER_INFO);
//			if (userInfo != null) {
//				if (onlines == null) {
//					onlines = new HashMap();
//				}
//				onlines.put(String.valueOf(userInfo.getUserId()), event.getSession());
//				logger.info(userInfo.getUserName() + " has login!");
//			}	
//		}
	}

	public void attributeRemoved(HttpSessionBindingEvent event) {
//		if (event.getName().equals(Constants.USER_INFO) && onlines != null) {
//			 
//			logger.info(user.getUserName() + " has logouted!");
//			onlines.remove(String.valueOf(user.getUserId()));
//		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see javax.servlet.http.HttpSessionAttributeListener#attributeReplaced(javax.servlet.http.HttpSessionBindingEvent)
	 */
	public void attributeReplaced(HttpSessionBindingEvent event) {
//		if (event.getName().equals(Constants.USER_INFO) && onlines != null) {
////			UserInfo userInfo = (UserInfo) event.getSession().getAttribute(Constants.USER_INFO);
////			if (userInfo != null) {
////				if (onlines == null) {
////					onlines = new HashMap();
////				}
////				onlines.put(String.valueOf(userInfo.getUserId()), event.getSession());
////			}
//		}
	}

}
