/**
 * 
 */
package com.broadengate.security.web.filter;

import java.io.IOException;

import javax.servlet.Filter;
import javax.servlet.FilterChain;
import javax.servlet.FilterConfig;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.springframework.security.Authentication;
import org.springframework.security.context.SecurityContextHolder;

/**
 * @author 龚磊
 * 
 */
public class LoginFilter implements Filter {

	private String loginPage;

	/*
	 * (non-Javadoc)
	 * 
	 * @see javax.servlet.Filter#destroy()
	 */
	public void destroy() {
		// TODO Auto-generated method stub

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see javax.servlet.Filter#doFilter(javax.servlet.ServletRequest,
	 *      javax.servlet.ServletResponse, javax.servlet.FilterChain)
	 */
	public void doFilter(ServletRequest arg0, ServletResponse arg1, FilterChain arg2) throws IOException,
			ServletException {
		HttpServletRequest request = (HttpServletRequest) arg0;
		String servletPath = request.getServletPath();
		if (servletPath.equals("/login.action") ||servletPath.equals("/ca/ensureSendCaRecord.action")|| servletPath.equals(loginPage)) {
			arg2.doFilter(arg0, arg1);
		} else {
			// 判断认证信息
			Authentication auth = SecurityContextHolder.getContext().getAuthentication();
			if (!auth.getPrincipal().equals("anonymousUser")) {
				arg2.doFilter(arg0, arg1);
			} else {
				HttpServletResponse response = (HttpServletResponse) arg1;
				response.sendRedirect(request.getContextPath() + loginPage);
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see javax.servlet.Filter#init(javax.servlet.FilterConfig)
	 */
	public void init(FilterConfig config) throws ServletException {
		loginPage = config.getInitParameter("loginPage");
	}

}
