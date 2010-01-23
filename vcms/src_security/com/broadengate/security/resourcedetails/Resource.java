package com.broadengate.security.resourcedetails;

import org.apache.commons.lang.StringUtils;
import org.apache.log4j.Logger;
import org.springframework.security.GrantedAuthority;
import org.springframework.util.Assert;

/**
 * ResourceDetails的实现类
 * 
 * @author 龚磊
 */
@SuppressWarnings("serial")
public class Resource implements ResourceDetails {
	/**
	 * Logger for this class
	 */
	private static final Logger logger = Logger.getLogger(Resource.class);

	/**
	 * 资源串
	 */
	private String resString;

	/**
	 * 资源类型(url, function,componet, menu)
	 */
	private String resType;

	/**
	 * 此资源被授予的角色
	 */
	private GrantedAuthority[] authorities;

	public Resource(String resString, String resType, GrantedAuthority[] authorities) {
		if (StringUtils.isEmpty(resString)) {
			throw new IllegalArgumentException("Cannot pass null or empty values to resource string");
		}
		if (StringUtils.isEmpty(resType)) {
			throw new IllegalArgumentException("Cannot pass null or empty values to resource type");
		}
		this.resString = resString;
		this.resType = resType;
		setAuthorities(authorities);
	}

	@Override
	public boolean equals(Object rhs) {
		if (!(rhs instanceof Resource))
			return false;
		Resource resauth = (Resource) rhs;
		if (resauth.getAuthorities().length != getAuthorities().length)
			return false;
		for (int i = 0; i < getAuthorities().length; i++) {
			if (!getAuthorities()[i].equals(resauth.getAuthorities()[i]))
				return false;
		}
		return getResString().equals(resauth.getResString()) && getResType().equals(resauth.getResType());
	}

	@Override
	public int hashCode() {
		int code = 168;
		if (getAuthorities() != null) {
			for (int i = 0; i < getAuthorities().length; i++)
				code *= getAuthorities()[i].hashCode() % 7;
		}
		if (getResString() != null)
			code *= getResString().hashCode() % 7;
		return code;
	}

	public String getResString() {
		return resString;
	}

	public void setResString(String resString) {
		this.resString = resString;
	}

	public GrantedAuthority[] getAuthorities() {
		return authorities;
	}

	public String getResType() {
		return resType;
	}

	public void setResType(String resType) {
		this.resType = resType;
	}

	protected void setAuthorities(GrantedAuthority[] authorities) {
		Assert.notNull(authorities, "Cannot pass a null GrantedAuthority array");
		for (int i = 0; i < authorities.length; i++) {
			Assert.notNull(authorities[i], "Granted authority element " + i
					+ " is null - GrantedAuthority[] cannot contain any null elements");
		}
		this.authorities = authorities;
	}

}
