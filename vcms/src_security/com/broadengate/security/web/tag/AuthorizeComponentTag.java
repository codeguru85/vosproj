/**
 * 
 */
package com.broadengate.security.web.tag;

import java.util.List;

import javax.servlet.ServletContext;
import javax.servlet.jsp.JspException;

import org.apache.commons.lang.StringUtils;
import org.springframework.context.ApplicationContext;
import org.springframework.security.GrantedAuthority;
import org.springframework.security.taglibs.authz.AuthorizeTag;
import org.springframework.web.context.support.WebApplicationContextUtils;

import com.broadengate.security.resourcedetails.ResourceDetails;
import com.broadengate.security.service.ISecurityService;

/**
 * @author 龚磊
 * 
 */
public class AuthorizeComponentTag extends AuthorizeTag {

	private static final long serialVersionUID = 1319741697827107207L;

	private String name;

	@Override
	public int doStartTag() throws JspException {
		// 如果设置的funcString为空则显示标签体
		if (StringUtils.isBlank(name)) {
			return EVAL_BODY_INCLUDE;
		}
		ServletContext context = pageContext.getServletContext();
		ApplicationContext ctx = WebApplicationContextUtils.getRequiredWebApplicationContext(context);
		ISecurityService securityService = (ISecurityService) ctx.getBean("securityService");
		List<String> components = securityService.getComponents();
		boolean findFlag = false;
		for (String component : components) {
			if (component.equals(name)) {
				findFlag = true;
			}
		}
		// 如果设置的funcString没有在acegi的权限控制范围内则显示标签体
		if (!findFlag) {
			return EVAL_BODY_INCLUDE;
		}
		// 如果在acegi的权限控制范围则取出该资源相对应的权限设置到setIfAnyGranted()方法中
		ResourceDetails rd = securityService.getAuthorityFromCache(name);
		GrantedAuthority[] authorities = new GrantedAuthority[0];
		String roles = " ";
		if (rd != null) {
			authorities = rd.getAuthorities();
		}
		if (authorities.length > 0) {
			for (GrantedAuthority authority : authorities) {
				roles += authority.getAuthority() + ",";
			}
			roles = roles.substring(0, roles.length() - 1);
		}
		this.setIfAnyGranted(roles);
		return super.doStartTag();
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

}
