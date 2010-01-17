/**
 * 
 */
package com.broadengate.core.log;

import java.lang.annotation.Annotation;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

/**
 * @author gonglei
 * 
 */
public class TestAnnotation {

	@LoggerAnnotation(moudle = "admin", action = "action11", operator = "gonglei")
	public void say() {
		System.out.println("say hello");
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		TestAnnotation ta = new TestAnnotation();
		try {
			Method[] methods = ta.getClass().getMethods();
			for (Method method : methods) {
				Annotation[] annotations = method.getAnnotations();
				for (Annotation annotation : annotations) {
					LoggerAnnotation annota =(LoggerAnnotation)annotation;
					System.out.println(annota.action());
					System.out.println(annota.operator());
				}
			}
		} catch (SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		ta.say();
	}
}
