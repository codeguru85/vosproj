/**
 * 
 */
package com.broadengate.security.service;

/**
 * @author 龚磊
 * 
 */
public enum RescType {
	
	URL("URL"), FUNCTION("FUNCTION"), COMPONENT("COMPONENT"), MENU("MENU");

	private String value;

	private RescType(String value) {
		this.value = value;
	}
}
