package com.broadengate.core;

import org.apache.log4j.Logger;

import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
/**
 * AMFContext类用于为Flex的请求线程绑定request,session变量
 * @author 龚磊
 *
 */
public class AMFContext {
	/**
	 * Logger for this class
	 */
	private static final Logger logger = Logger.getLogger(AMFContext.class);

	/**
	 * ThreadLocal object for storing object in current thread.
	 */
	private static ThreadLocal tl = new ThreadLocal();

	/**
	 * Set current context
	 * 
	 * @param request
	 *            The HttpRequest object
	 * @param response
	 *            The HttpResponses object
	 */
	static public void setCurrentContext(HttpServletRequest request, HttpServletResponse response) {
		AMFContext c = getCurrentContext();
		if (c == null) {
			c = new AMFContext(request, response);
			tl.set(c);
		} else {
			c.setRequest(request);
			c.setResponse(response);
		}
	}

	/**
	 * Get current context value
	 * 
	 * @return The current context
	 */
	static public AMFContext getCurrentContext() {
		return (AMFContext) tl.get();
	}

	/**
	 * The http request object. The lifecycle of the request object is defined
	 * as the request scope. It may be reused in another incoming connection, so
	 * dont use it in another thread.
	 */
	private HttpServletRequest request;

	/**
	 * The http response object. The lifecycle of the response object is defined
	 * as the request scope. Dont use it in another thread. Also dont write
	 * output to the response when it is used in the context, but you may get or
	 * set some response header when it is safe.
	 */
	private HttpServletResponse response;

	/**
	 * The constructor is private, to get an instance of the AMFContext, please
	 * use getCurrentContext() method.
	 * 
	 * @param request
	 * @param response
	 */
	private AMFContext(HttpServletRequest request, HttpServletResponse response) {
		this.request = request;
		this.response = response;
	}

	/**
	 * Get request object
	 * 
	 * @return Http request object
	 */
	public HttpServletRequest getRequest() {
		return request;
	}

	/**
	 * Set request object
	 * 
	 * @param Http
	 *            request object
	 */
	public void setRequest(HttpServletRequest request) {
		this.request = request;
	}

	/**
	 * Get response object
	 * 
	 * @return Http response object
	 */
	public HttpServletResponse getResponse() {
		return response;
	}

	/**
	 * Set response object
	 * 
	 * @param response
	 *            Http response object
	 */
	public void setResponse(HttpServletResponse response) {
		this.response = response;
	}

	/**
	 * Get the servlet context
	 * 
	 * @return
	 */
	public ServletContext getServletContext() {
		HttpSession session = this.getSession();
		return session.getServletContext();
	}

	/**
	 * Get the current running session
	 * 
	 * @return
	 */
	public HttpSession getSession() {
		return request.getSession(true);
	}

	/**
	 * Get an object stored in the session.
	 * 
	 * @param attr
	 *            Attribute Name
	 * @return The value stored under the attribute name.
	 */
	public Object getSessionAttribute(String attr) {
		HttpSession session = this.getSession();
		return session.getAttribute(attr);
	}

	/**
	 * Store an object in the session.
	 * 
	 * @param attr
	 *            Attribute Name
	 * @param value
	 *            The value.
	 */
	public void setSessionAttribute(String attr, Object value) {
		HttpSession session = this.getSession();
		session.setAttribute(attr, value);
	}

	/**
	 * Get an object stored in the servlet context.
	 * 
	 * @param attr
	 *            Attribute Name
	 * @return The value stored under the attribute name.
	 */
	public Object getContextAttribute(String attr) {
		ServletContext sc = this.getServletContext();
		return sc.getAttribute(attr);
	}

	/**
	 * Store an object in the servlet context.
	 * 
	 * @param attr
	 *            Attribute Name
	 * @param value
	 *            The value.
	 */
	public void setContextAttribute(String attr, Object value) {
		ServletContext sc = this.getServletContext();
		sc.setAttribute(attr, value);
	}

	/**
	 * Get an object stored in the current request.
	 * 
	 * @param attr
	 *            Attribute Name
	 * @return The value stored under the attribute name.
	 */
	public Object getRequestAttribute(String attr) {
		return request.getAttribute(attr);
	}

	/**
	 * Store an object in the current request.
	 * 
	 * @param attr
	 *            Attribute Name
	 * @param value
	 *            The value.
	 */
	public void setRequestAttribute(String attr, Object value) {
		request.setAttribute(attr, value);
	}

}
