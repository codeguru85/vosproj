package com.broadengate.vcms.tcp.data.auxiliary;

import java.util.TimerTask;

import org.springframework.web.context.WebApplicationContext;

import com.broadengate.vcms.service.ITermService;

public class DbChecker extends TimerTask{
	private WebApplicationContext applicationContext;
	private ITermService termService;
	//初始化
	public DbChecker(WebApplicationContext applicationContext) {
		this.applicationContext = applicationContext;
		//将新的终端信息放入数据库
		termService = (ITermService) applicationContext.getBean("termService");
		
	}
	
	public void run() {
		termService.setCheckTermActivation();
	}

}
