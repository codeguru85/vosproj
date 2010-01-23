package com.broadengate.vcms.web.servlet;

import java.io.IOException;

import javax.annotation.Resource;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.broadengate.vcms.dao.TermTypeDao;
/**
 * 初始化启动
 * @author huang
 */
public class StartTermServer extends HttpServlet {
	
	private static final long serialVersionUID = 1L;
	
//	private final int DEFAULT_SERVERPORT = 6325;//默认端口
//
//	@Resource
//	private TermTypeDao termTypeDao;
	
	public void init() throws ServletException {
//			System.out.println("!!!!!!!!!!!!!!!!!!!!!"+termTypeDao);
//		 ApplicationContext context = new ClassPathXmlApplicationContext("applicationContext.xml");
//		 TermTypeDao s = (TermTypeDao)context.getBean("termTypeDao");
//		 ITermService its = m
//		 System.out.println(s);
//		playInfoService.getClass();
//    	System.out.println("服务器预加载……");
//        System.out.println("服务器启动");
//        Server server = null;
//		try {
//			server = new Server(6325);
//		} catch (IOException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//        server.start();
	}
	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse response)
	 */
    @Override
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
//    	response.setContentType("text/html; charset=UTF-8");
//    	//String filedir = ContextUtil.getServletContext().getRealPath("/");

//    	}
	}

}
