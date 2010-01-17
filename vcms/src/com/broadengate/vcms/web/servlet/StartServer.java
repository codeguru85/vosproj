package com.broadengate.vcms.web.servlet;

import java.io.IOException;

import javax.annotation.Resource;

import org.springframework.context.ApplicationContext;
import org.springframework.context.support.ClassPathXmlApplicationContext;

import com.broadengate.vcms.dao.TermTypeDao;
import com.broadengate.vcms.service.IPlayInfoService;
import com.broadengate.vcms.service.ITermService;
import com.broadengate.vcms.service.imp.TermService;

public class StartServer {
	
	
	private final int DEFAULT_SERVERPORT = 6325;//默认端口

	@Resource
	private IPlayInfoService  playInfoService;
	
	public StartServer() {
//		playInfoService.getClass();
//		System.out.println("服务器预加载……");
//	    System.out.println("服务器启动");
//	    Server server = null;
//		try {
//			server = new Server(DEFAULT_SERVERPORT);
//		} catch (IOException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//	    server.start();
	}
	
	
	
	public static void main(String[] args) {
//		 ApplicationContext context = new ClassPathXmlApplicationContext("applicationContext.xml");
//		 TermTypeDao s = (TermTypeDao)context.getBean("termTypeDao");
//		 ITermService its = m
//		 System.out.println(s);
	}
}
