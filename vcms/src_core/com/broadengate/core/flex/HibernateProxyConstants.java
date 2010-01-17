/**
	Copyright (c) 2008. Digital Primates IT Consulting Group
	http://www.digitalprimates.net
	All rights reserved.
	
	This library is free software; you can redistribute it and/or modify it under the 
	terms of the GNU Lesser General Public License as published by the Free Software 
	Foundation; either version 2.1 of the License.

	This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	See the GNU Lesser General Public License for more details.

	
	@author: Mike Nimer
	@ignore
**/


package com.broadengate.core.flex;

/**
 * These are the field names HibernateProxy properties referenced by the Serializers. 
 * We have them called out here so we can rename the properties in the proxy object in needed
 * 
 * @author mnimer
 */
public class HibernateProxyConstants
{
	public static String UID = "uid";
	public static String PKEY = "proxyKey";
	public static String PROXYINITIALIZED = "proxyInitialized";
}
