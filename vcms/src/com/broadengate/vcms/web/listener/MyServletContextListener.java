package com.broadengate.vcms.web.listener;

import java.util.Timer;

import javax.servlet.ServletContext;
import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

import org.springframework.context.ApplicationContext;
import org.springframework.web.context.WebApplicationContext;
import org.springframework.web.context.support.WebApplicationContextUtils;

import com.broadengate.vcms.tcp.data.TermsServer;
import com.broadengate.vcms.tcp.data.auxiliary.DbChecker;
import com.broadengate.vcms.time.Scan;
import com.broadengate.vcms.timeJob.PlayTreeMap;

public class MyServletContextListener implements ServletContextListener {
	private static int DEFAULT_SERVERPORT = 6325;// 默认端口
	/**
	 * spring上下文环境
	 */
	public static ApplicationContext applicationContext;

	/**
	 * servlet上下文环境
	 */
	public static ServletContext servletContext;
	@Override
	public void contextDestroyed(ServletContextEvent arg0) {
		// TODO Auto-generated method stub

	}

	@Override
	public void contextInitialized(ServletContextEvent event) {
		try {
			//初始化上下文
			servletContext = event.getServletContext();
			applicationContext = WebApplicationContextUtils.getRequiredWebApplicationContext(servletContext);
			// 提供spring注入的再注入
			WebApplicationContext ctx = WebApplicationContextUtils
					.getRequiredWebApplicationContext(event.getServletContext());
			// ITermService its = (ITermService) ctx.getBean("termService");
			System.out.println("服务器预加载……");
			System.out.println("服务器启动");
			TermsServer server = null;
			server = new TermsServer(MyServletContextListener.DEFAULT_SERVERPORT,ctx);
			server.start();
			
			PlayTreeMap treeMap = new PlayTreeMap(ctx) ;
			//初始化播放列表
			treeMap.getAll() ;
			//启动数据库检查程序
			Timer timer=new Timer();//生成一个Timer对象
			DbChecker myTask=new DbChecker(ctx);//初始化我们的任务
			//5分钟循环一次
	        timer.schedule(myTask,1,300000);//还有其他重载方法..
	        
	        Timer times=new Timer();//生成一个Timer对象
	        Scan s = new Scan(ctx) ; //播放列表
	        times.schedule(s, 2, 3000) ;//3秒循环一次
	        
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.out.println("<<<< "+e.getStackTrace());
		}

	}

}
