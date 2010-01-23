package com.broadengate.vcms.tcp.data.auxiliary;

import java.util.TimerTask;

public class TcpOverTimeTask extends TimerTask {
	private Boolean overTimeMask;

	public Boolean getOverTimeMask() {
		return overTimeMask;
	}

	public void setOverTimeMask(Boolean overTimeMask) {
		this.overTimeMask = overTimeMask;
	}

	public TcpOverTimeTask() {
		
	}
	//
	public void run() {
		if(overTimeMask){
			  try {
				throw   new   InterruptedException("Tcp 连接超时！");
			} catch (InterruptedException e) {
				System.out.println(e.getMessage());
			}
		}
		else{
			
			overTimeMask = true;
		}
	}
}
