package com.broadengate.security.resourcedetails;

import java.io.Serializable;

import org.springframework.security.GrantedAuthority;

/**
 * 提供资源信息
 *
 * @author 龚磊
 */
public interface ResourceDetails extends Serializable {

	/**
	 * 资源串
	 */
	public String getResString();

	/**
	 * 资源类型,如URL,FUNCTION
	 */
	public String getResType();

	/**
	 * 返回属于该resource的authorities
	 */
	public GrantedAuthority[] getAuthorities();
}
