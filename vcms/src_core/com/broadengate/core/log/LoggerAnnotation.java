/**
 * 
 */
package com.broadengate.core.log;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * @author gonglei
 * 
 */
@Retention(RetentionPolicy.RUNTIME)
public @interface LoggerAnnotation {

	public String moudle() default "cmp";

	public String action() default "noAction";

	public String operator() default "admin";
}
