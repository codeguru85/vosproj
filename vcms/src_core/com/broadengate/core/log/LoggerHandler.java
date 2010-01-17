/**
 * 
 */
package com.broadengate.core.log;

import org.apache.log4j.Logger;

import java.lang.reflect.Method;

import org.springframework.aop.MethodBeforeAdvice;

/**
 * @author gonglei
 * 
 */
public class LoggerHandler implements MethodBeforeAdvice {
	/**
	 * Logger for this class
	 */
	private static final Logger logger = Logger.getLogger(LoggerHandler.class);

	public void afterLogging() {
		logger.info("invoking end");
	}

	@Override
	public void before(Method method, Object[] args, Object target) throws Throwable {
		method.getAnnotations();
		boolean isExist = method.isAnnotationPresent(LoggerAnnotation.class);
		if (isExist) {
			LoggerAnnotation annotation = method.getAnnotation(LoggerAnnotation.class);
			System.out.println(annotation.action());
			System.out.println(annotation.moudle());
		}
		logger.info("begin invoking " + target.getClass().getSimpleName() + "." + method.getName());
	}
}
