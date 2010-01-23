/**
 * 
 */
package com.broadengate.core.web.filter;

import java.io.IOException;

import javax.servlet.Filter;
import javax.servlet.FilterChain;
import javax.servlet.FilterConfig;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpSession;

/**
 * @author gonglei-desktop
 * 
 */
public class CookieFilter implements Filter {

	/**
	 * 
	 */
	public CookieFilter() {
		// TODO Auto-generated constructor stub
	}

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
		HttpSession session = request.getSession();
		// 判断会话中是否存在loginFamily对象，如果不存在，试图在cookie中寻找family信息，如果找到，则初始化为登录状态，并更新会话
		 
			Cookie[] cookies = request.getCookies();
			if (cookies != null) {
				String familyId = null;
				String userId = null;
				// 从cookies中查找家庭信息
				for (Cookie cookie : cookies) {
					if (cookie.getName().equals("familyId")) {
						familyId = cookie.getValue();
						continue;
					}
					if (cookie.getName().equals("userId")) {
						userId = cookie.getValue();
					}
				}
			 
			}
		 
		arg2.doFilter(arg0, arg1);

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see javax.servlet.Filter#init(javax.servlet.FilterConfig)
	 */
	public void init(FilterConfig arg0) throws ServletException {
		// TODO Auto-generated method stub

	}

}
