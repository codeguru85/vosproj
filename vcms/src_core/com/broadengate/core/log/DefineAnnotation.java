/**
 * 
 */
package com.broadengate.core.log;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * @author gonglei
 * 
 */
@Retention(RetentionPolicy.RUNTIME)
public @interface DefineAnnotation {
	 

	// 有默认值的属性
	public String value() default "aaa"; 

}
