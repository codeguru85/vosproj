/**
 * 
 */
package com.broadengate.security.service;

import java.util.Collection;
import java.util.Collections;
import java.util.List;

import org.apache.log4j.Logger;
import org.apache.oro.text.regex.MalformedPatternException;
import org.apache.oro.text.regex.Pattern;
import org.apache.oro.text.regex.PatternMatcher;
import org.apache.oro.text.regex.Perl5Compiler;
import org.apache.oro.text.regex.Perl5Matcher;
import org.springframework.security.ConfigAttributeDefinition;
import org.springframework.security.ConfigAttributeEditor;
import org.springframework.security.GrantedAuthority;
import org.springframework.security.intercept.web.FilterInvocation;
import org.springframework.security.intercept.web.FilterInvocationDefinitionSource;
import org.springframework.util.AntPathMatcher;
import org.springframework.util.PathMatcher;

import com.broadengate.security.resourcedetails.ResourceDetails;

/**
 * @author 龚磊
 */
public class DBFilterInvocationDefinitionSource implements FilterInvocationDefinitionSource {
	/**
	 * Logger for this class
	 */
	private static final Logger logger = Logger.getLogger(DBFilterInvocationDefinitionSource.class);

	private boolean convertUrlToLowercaseBeforeComparison = false;

	private boolean useAntPath = false;

	private final PathMatcher pathMatcher = new AntPathMatcher();

	private final PatternMatcher matcher = new Perl5Matcher();

	private ISecurityService securityService;

	/**
	 * 返回当前URL对应的ROLE
	 * 
	 * @see org.acegisecurity.intercept.web.AbstractFilterInvocationDefinitionSource#lookupAttributes(java.lang.String)
	 */

	@SuppressWarnings("unchecked")
	public ConfigAttributeDefinition lookupAttributes(String url) {
		securityService.initResourceCache();
		logger.debug("request" + url);
		if (isUseAntPath()) {
			int firstQuestionMarkIndex = url.lastIndexOf("?");
			if (firstQuestionMarkIndex != -1) {
				url = url.substring(0, firstQuestionMarkIndex);
			}
		}
		List<String> urls = securityService.getUrlResStrings();
		// 倒叙排序
		// Collections.sort(urls);
		// Collections.reverse(urls);
		if (convertUrlToLowercaseBeforeComparison) {
			url = url.toLowerCase();
		}
		GrantedAuthority[] authorities = new GrantedAuthority[0];
		for (String resString : urls) {
			boolean matched = false;
			if (isUseAntPath()) {
				matched = pathMatcher.match(resString, url);
			} else {
				Pattern compiledPattern;
				Perl5Compiler compiler = new Perl5Compiler();
				try {
					compiledPattern = compiler.compile(resString, Perl5Compiler.READ_ONLY_MASK);
				} catch (MalformedPatternException mpe) {
					throw new IllegalArgumentException("Malformed regular expression: " + resString, mpe);
				}
				matched = matcher.matches(url, compiledPattern);
			}
			if (matched) {
				ResourceDetails rd = securityService.getAuthorityFromCache(resString);
				authorities = rd.getAuthorities();
				break;
			}
		}
		if (authorities.length > 0) {
			String authoritiesStr = " ";
			for (int i = 0; i < authorities.length; i++) {
				authoritiesStr += authorities[i].getAuthority() + ",";
			}
			String authStr = authoritiesStr.substring(0, authoritiesStr.length() - 1);
			ConfigAttributeEditor configAttrEditor = new ConfigAttributeEditor();
			configAttrEditor.setAsText(authStr.trim());
			return (ConfigAttributeDefinition) configAttrEditor.getValue();
		}
		return null;
	}

	public void setConvertUrlToLowercaseBeforeComparison(boolean convertUrlToLowercaseBeforeComparison) {
		this.convertUrlToLowercaseBeforeComparison = convertUrlToLowercaseBeforeComparison;
	}

	public boolean isConvertUrlToLowercaseBeforeComparison() {
		return convertUrlToLowercaseBeforeComparison;
	}

	public boolean isUseAntPath() {
		return useAntPath;
	}

	public void setUseAntPath(boolean useAntPath) {
		this.useAntPath = useAntPath;
	}

	public ConfigAttributeDefinition getAttributes(Object object) throws IllegalArgumentException {
		if (object == null || !supports(object.getClass())) {
			throw new IllegalArgumentException("Object must be a FilterInvocation");
		} else {
			String url = ((FilterInvocation) object).getRequestUrl();
			return lookupAttributes(url);
		}
	}

	@Override
	public Collection getConfigAttributeDefinitions() {
		return Collections.unmodifiableCollection(securityService.getUrlResStrings());
	}

	@Override
	public boolean supports(Class clazz) {
		return (org.springframework.security.intercept.web.FilterInvocation.class).isAssignableFrom(clazz);
	}

	public void setSecurityService(ISecurityService securityService) {
		this.securityService = securityService;
	}

}