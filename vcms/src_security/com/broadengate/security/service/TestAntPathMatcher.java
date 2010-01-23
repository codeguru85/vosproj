package com.broadengate.security.service;

import org.springframework.util.AntPathMatcher;
import org.springframework.util.PathMatcher;

public class TestAntPathMatcher {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		 PathMatcher pathMatcher = new AntPathMatcher();
		System.out.println(pathMatcher.match("/a11", "/A11"));

	}

}
