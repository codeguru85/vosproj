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

import flex.messaging.Destination;
import flex.messaging.config.ConfigMap;
import flex.messaging.messages.Message;
import flex.messaging.messages.RemotingMessage;
import flex.messaging.services.remoting.adapters.JavaAdapter;

@SuppressWarnings("unchecked")
public class HibernateAdapter extends JavaAdapter {
	// private String scope = "request";
	protected Destination destination;

	/**
	 * Initialize the adapter properties from the flex services-config.xml file
	 */
	public void initialize(String id, ConfigMap properties) {
		super.initialize(id, properties);
	}

	public Object superInvoke(Message message) {
		return super.invoke(message);
	}

	/**
	 * Invoke the Object.method() called through FlashRemoting
	 */
	public Object invoke(Message message) {
		Object results = null;
		if (message instanceof RemotingMessage) {
			RemotingMessage remotingMessage = (RemotingMessage) message;
			results = super.invoke(remotingMessage);

			// serialize the result out
			try {
				HibernateSerializer serializer = new HibernateSerializer();
				results = serializer.translate(results);
			} catch (Exception ex) {
				ex.printStackTrace();
				RuntimeException re = new RuntimeException(ex.getMessage());
				re.setStackTrace(ex.getStackTrace());
				throw re;
			}
		}
		return results;
	}

}
