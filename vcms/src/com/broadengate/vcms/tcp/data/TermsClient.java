package com.broadengate.vcms.tcp.data;

//200532580182 pengxu
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import javax.annotation.Resource;
import javax.servlet.ServletContext;

import org.springframework.context.ApplicationContext;

import com.broadengate.core.ContextUtil;
import com.broadengate.vcms.entity.PlayListShip;
import com.broadengate.vcms.entity.TermInfo;
import com.broadengate.vcms.entity.VideoInfo;
import com.broadengate.vcms.tcp.data.auxiliary.CaptionResultContainer;
import com.broadengate.vcms.tcp.data.auxiliary.UrlResultContainer;
import com.broadengate.vcms.tcp.data.auxiliary.ClientMessageManager;
import com.broadengate.vcms.tcp.data.auxiliary.DownloadResultContainer;
import com.broadengate.vcms.tcp.data.auxiliary.PlayResultContainer;
import com.broadengate.vcms.tcp.data.auxiliary.StopResultContainer;
import com.broadengate.vcms.tcp.data.info.CaptionReq;
import com.broadengate.vcms.tcp.data.info.CaptionReqAck;
import com.broadengate.vcms.tcp.data.info.UrlReq;
import com.broadengate.vcms.tcp.data.info.UrlReqAck;
import com.broadengate.vcms.tcp.data.info.DeviceControlAck;
import com.broadengate.vcms.tcp.data.info.DownloadReq;
import com.broadengate.vcms.tcp.data.info.DownloadReqAck;
import com.broadengate.vcms.tcp.data.info.PlayFileReq;
import com.broadengate.vcms.tcp.data.info.PlayFileReqAck;
import com.broadengate.vcms.tcp.data.info.PlayStateCommand;
import com.broadengate.vcms.tcp.data.info.PlayStateCommandAck;
import com.broadengate.vcms.tcp.data.info.SetDevicePropertyAck;
import com.broadengate.vcms.tcp.data.info.ShowPlayFileAck;
import com.broadengate.vcms.web.listener.MyServletContextListener;
import com.broadengate.vcms.web.util.TimeUtil;


public class TermsClient {
	@Resource 
	private ContextUtil contextUtil;
	// the client socket
	private Socket socket;
	// tcp connect port
	private int port = 0;
	// the round trip time
//	// server
//	private String remoteHost = "localhost";
	// server ip
	private InetAddress remoteIP;
	// stream write to server
	private DataOutputStream outToServer;
	// stream receive from the server
	private InputStream inFromServer;

	//暂存接受的消息
	private byte[] tempMsg = null;
	
	public TermsClient() {
	}
	// constructor
	public TermsClient(String ip, int con) {
		try {
			// get the host ip address
			remoteIP = InetAddress.getByName(ip);
			this.port = con;
		} catch (UnknownHostException e) {
			e.printStackTrace();
		}
	}

	public synchronized Object talk(byte[] msg) {
		
		/**
		 * 判断软件是否已经到了有效期
		 * */
//		String currDate=TimeUtil.getFormatDateFileName(new Date());
//		Boolean flag=TimeUtil.compareDateTime(currDate, "20100113000000");
//		if(!flag){
//			return null; 
//		}
		
		// 回复信息
		Object returnObj = null;
		try {
			
			//设置超时定时器
//			Timer timer = new Timer();
//			TcpOverTimeTask tott= new TcpOverTimeTask();
//			tott.setOverTimeMask(true);
//			timer.schedule(tott, 7000);
//			// connect to the server
//			socket = new Socket(remoteIP, port);
//			//当tcp通过后将overtimeMark置为false
//			tott.setOverTimeMask(false);
			boolean enableMark = checkService(remoteIP, port);
			if(!enableMark){
				returnObj = "noClient";
				return returnObj;
			}
			//设置读超时时间
			socket = new Socket(remoteIP, port);
			socket.setSoTimeout(10000);
			outToServer = new DataOutputStream(socket.getOutputStream());
			inFromServer = socket.getInputStream();
			// write data to the server
			outToServer.write(msg);
			// 通用的消息储存器
			tempMsg = new byte[500];
			// receive data from the server
			inFromServer.read(tempMsg);
			// 判断回复的消息类型
			int msgType = ClientMessageManager.checkMsgTyper(tempMsg);
			// 根据回复消息类型进行分别处理
			if (msgType == 0X80000005) {
				// 播放控制响应
				DeviceControlAck dca = ClientMessageManager.playControlAcker(tempMsg);
				if (dca.getStatus_id() == 1000||dca.getStatus_id() == 2000) {
					returnObj = new String("succsee");
				} else {

					returnObj = new String("faild");
				}
			}
			//修改参数
			else if (msgType == 0X80000008) {
				SetDevicePropertyAck sdpa = ClientMessageManager.setDevicePropertyAcker(tempMsg);
				if (sdpa.getStatus_id() == 8000) {
					returnObj = new String("succsee");
				} else {
					returnObj = new String("faild");
				}
			}
			//反向查询
			else if (msgType == 0X80000004) {
				ShowPlayFileAck spfa = ClientMessageManager.showPlayFileAcker(tempMsg);
				returnObj = spfa;
			}

			// close this tco connection
			socket.close();
		} catch (Exception e) {
			try {
				if (socket != null) {
					socket.close();
					socket = null;
				}
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				returnObj = "readOver";
				return returnObj;
			}
			returnObj = "noClient";
			return returnObj;
		}
		return returnObj;
	}
	
	/**
	 * 播放消息收发
	 * @param msg
	 * @return
	 */
	public synchronized List playMsgTalk(List<TermInfo> termListsShip,String fileName,int videoSize,String filePath) {
		
		//无连接标志 0
		PlayResultContainer noClient = null;
		//播放失败标志 1
		PlayResultContainer noEnable = null;
		//播放失败标志 2
		PlayResultContainer playEnable = null;
		//找不到服务器IP标志 3
		PlayResultContainer noIP = null;
		//读超时 4
		PlayResultContainer readNotEnable =null;
		// socket
		Socket playSocket = null;
		// 对方端口
		int playPort = 0;
		// server
		String playRemoteHost = "localhost";
		// server ip
		InetAddress playRemoteIP;
		// stream write to server
		DataOutputStream playOutToServer;
		// stream receive from the server
		InputStream playInFromServer;
		//结果搜集器
		List resultsArray = new ArrayList();
		//IP地址容器
		String url;
		//拼接IP地址
		
		try {
			ServletContext servletContext =MyServletContextListener.servletContext;
			url = "http://"+InetAddress.getLocalHost().getHostAddress()+":8080/"+servletContext.getServletContextName()+"/";
		} catch (UnknownHostException e) {
			noIP= new PlayResultContainer();
			noIP.setClientName("");
			noIP.setReturnMark("6");
			resultsArray.add(noIP);
			return resultsArray;
		}
		
		//主体程序
		//发送所有video信息
		for(int i=0;i<termListsShip.size();i++){
			try{
			//先测试service是否存在
			boolean isHas = checkService(termListsShip.get(i));
			if(!isHas){
				//如果无法连接则放入结果搜集器
				noClient= new PlayResultContainer();
				noClient.setClientName(termListsShip.get(i).getTermName());
				noClient.setReturnMark("1");
				resultsArray.add(noClient);
				continue;
			}
			else{
				//初始化流
				playSocket = new Socket(termListsShip.get(i).getTermIp(), termListsShip.get(i).getTermCom());
				//设置超时
				playSocket.setSoTimeout(5000);
				playOutToServer = new DataOutputStream(playSocket.getOutputStream());
				playInFromServer = playSocket.getInputStream();
				//开始向一个终端中发送播放信息
				if(fileName==null){
					playEnable = new PlayResultContainer();
					playEnable.setClientName(termListsShip.get(i).getTermName());
					playEnable.setReturnMark("5");
					resultsArray.add(playEnable);					
				}else{
						PlayFileReq pfr = new PlayFileReq(0X00000003, 340, termListsShip.get(i).getTermNum(), 1,
						"0","",fileName,String.valueOf(videoSize),url+filePath,""		
						);
						//发送消息
						Thread.sleep(3000);
						playOutToServer.write(pfr.getBuf());
				}				
				// 通用的消息储存器
				tempMsg = new byte[500];
				// receive data from the server
				playInFromServer.read(tempMsg);
				// 判断回复的消息类型
				int msgType = ClientMessageManager.checkMsgTyper(tempMsg);

				//播放请求应答
				if(msgType == 0X80000003){
					PlayFileReqAck tempPlayFileReqAck = ClientMessageManager.playFileReqAcker(tempMsg);
					if(tempPlayFileReqAck.getState() == 3000)
					{	
						playEnable = new PlayResultContainer();
						playEnable.setClientName(termListsShip.get(i).getTermName());
						playEnable.setReturnMark("2");
						resultsArray.add(playEnable);
					}else if(tempPlayFileReqAck.getState() == 3001){
						playEnable = new PlayResultContainer();
						playEnable.setClientName(termListsShip.get(i).getTermName());
						playEnable.setReturnMark("3");//文件本地不存在
						resultsArray.add(playEnable);
					}else{
						noEnable = new PlayResultContainer();
						noEnable.setClientName(termListsShip.get(i).getTermName());
						noEnable.setReturnMark("0");
						resultsArray.add(noEnable);
					}
				}
				// close this tco connection
				playSocket.close();
			}
		}catch (IOException e ) {
			if (playSocket != null) {
				try {
					playSocket.close();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				playSocket = null;
				//如果读超时则放入结果搜集器
				readNotEnable= new PlayResultContainer();
				readNotEnable.setClientName(termListsShip.get(i).getTermName());
//				读取超时的错误码是2
				readNotEnable.setReturnMark("4");
				resultsArray.add(readNotEnable);
			}
			continue;
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
		return resultsArray;
	}
	
	/**
	 * 重播消息收发
	 * @param msg
	 * @return 
	 * */
	public synchronized List resetMsgTalk(List<TermInfo>termListsShip){
		return null;
	}
	
	/**
	 * 停止消息收发
	 * @param msg
	 * @return
	 */
	public synchronized List stopMsgTalk(List<TermInfo> termListsShip,int state) {
		
		//无连接标志 0
		StopResultContainer stopClient = null;
		//播放失败标志 1
		StopResultContainer noStopEnable = null;
		//读超时标志 2
		StopResultContainer readNotEnable = null;
		// socket
		Socket playSocket = null;
		// 对方端口
		int playPort = 0;
		// server
		String playRemoteHost = "localhost";
		// server ip
		InetAddress playRemoteIP;
		
		// stream write to server
		DataOutputStream playOutToServer;
		// stream receive from the server
		InputStream playInFromServer;
		//结果搜集器
		List resultsArray = new ArrayList();

		//主体程序
		//发送所有video信息
		for(int i=0;i<termListsShip.size();i++){
			try{
			//先测试service是否存在
			boolean isHas = checkService(termListsShip.get(i));
			if(!isHas){
				//如果无法连接则放入结果搜集器
				noStopEnable= new StopResultContainer();
				noStopEnable.setClientName(termListsShip.get(i).getTermName());
				noStopEnable.setReturnMark("1");
				resultsArray.add(noStopEnable);
				continue;
			}
			else{
				//初始化流
				playSocket = new Socket(termListsShip.get(i).getTermIp(), termListsShip.get(i).getTermCom());
				//设置超时
				playSocket.setSoTimeout(5000);
				playOutToServer = new DataOutputStream(playSocket.getOutputStream());
				playInFromServer = playSocket.getInputStream();
				//开始向一个终端中发送停止信息
					
				PlayStateCommand psc = new PlayStateCommand(0X00000008,48,termListsShip.get(i).getTermNum(),state);
					
				//发送消息
				playOutToServer.write(psc.getBuf());

				// 通用的消息储存器
				tempMsg = new byte[500];
				// receive data from the server
				playInFromServer.read(tempMsg);
				// 判断回复的消息类型
				int msgType = ClientMessageManager.checkMsgTyper(tempMsg);

				//停止请求应答
				if(msgType == 0X80000008){
					PlayStateCommandAck tempStopFileReqAck = ClientMessageManager.stopFileReqAcker(tempMsg);
					if(tempStopFileReqAck.getState() == 0)
					{	
						stopClient = new StopResultContainer();
						stopClient.setClientName(termListsShip.get(i).getTermName());
						stopClient.setReturnMark("0");
						resultsArray.add(stopClient);
					}
					else
					{
						noStopEnable = new StopResultContainer();
						noStopEnable.setClientName(termListsShip.get(i).getTermName());
						noStopEnable.setReturnMark("1");
						resultsArray.add(noStopEnable);
					}
				}
				// close this tco connection
				playSocket.close();
			}
		}
			
		catch (IOException e ) {
			if (playSocket != null) {
				try {
					playSocket.close();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				playSocket = null;
				//如果读超时则放入结果搜集器
				readNotEnable= new StopResultContainer();
				readNotEnable.setClientName(termListsShip.get(i).getTermName());
				//读取超时的错误码是2
				readNotEnable.setReturnMark("2");
				resultsArray.add(readNotEnable);
			}
			continue;
		}
	}
		return resultsArray;
	}
	
	/**
	 * 检查服务端是否存在
	 * @param tempTerm
	 * @return
	 */
	private synchronized boolean checkService(TermInfo tempTerm) {
        SocketAddress add = new InetSocketAddress(tempTerm.getTermIp(), tempTerm.getTermCom());
        Socket  socket = new Socket();
        try {
			socket.connect(add,1000);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			return false;
		}

       
		return true;
	}
	
	/**
	 * 检查服务端是否存在
	 * @param clientIp
	 * @param clientport
	 * @return
	 */
	private synchronized boolean checkService(InetAddress clientIp, int clientport) {
        SocketAddress add = new InetSocketAddress(clientIp, clientport);
        Socket  socket = new Socket();
        try {
			socket.connect(add,1000);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			return false;
		}

       
		return true;
	}
	// public static void main(String[] args) throws Exception {
	// new TermsClient().talk();
	// }
	
	/**
	 * 指定终端下载
	 * @param msg
	 * @return
	 */
	public synchronized List downloadMsgTalk(List<VideoInfo> videos,List<TermInfo> terms)
	{
		
		/**
		 * 判断软件是否已经到了有效期
		 * */
//		String currDate=TimeUtil.getFormatDateFileName(new Date());
//		Boolean flag=TimeUtil.compareDateTime(currDate, "20100113000000");
//		if(!flag){
//			return null; 
//		}
		//无连接标志 0
		DownloadResultContainer noClient = null;
		//服务端不存在 1
		DownloadResultContainer noEnable = null;
		//成功 2
		DownloadResultContainer downloadEnable = null;
		//找不到服务器IP标志 3
		DownloadResultContainer noIP = null;
		//读超时 4
		DownloadResultContainer readNotEnable =null;
		// socket
		Socket downloadSocket = null;
		// 对方端口
		
//		int playPort = 0;
//		// server
//		String playRemoteHost = "localhost";
//		// server ip
//		InetAddress playRemoteIP;
		
		// stream write to server
		DataOutputStream downloadOutToServer;
		// stream receive from the server
		InputStream downloadInFromServer;
		//结果搜集器
		List resultsArray = new ArrayList();
		//IP地址容器
		String url;
		//拼接IP地址
		try 
		{
			ServletContext servletContext =MyServletContextListener.servletContext;
			url = "http://"+InetAddress.getLocalHost().getHostAddress()+":8080/"+servletContext.getServletContextName()+"/";
		} 
		catch (UnknownHostException e) 
		{
			noIP= new DownloadResultContainer();
			noIP.setClientName("");
			noIP.setReturnMark("3");
			resultsArray.add(noIP);
			return resultsArray;
		}
		
		
		//主体程序
		//发送所有video信息
		
		for(int i=0;i<terms.size();i++)
		{
			try
			{
				//先测试service是否存在
				boolean isHas = checkService(terms.get(i));
				if(!isHas)
				{
					//如果无法连接则放入结果搜集器
					noClient= new DownloadResultContainer();
					noClient.setClientName(terms.get(i).getTermName());
					noClient.setReturnMark("1");
					resultsArray.add(noClient);
					continue;
				}
				else
				{
					//初始化流
					downloadSocket = new Socket(terms.get(i).getTermIp(), terms.get(i).getTermCom());
					//设置超时
					downloadSocket.setSoTimeout(7000);
					downloadOutToServer = new DataOutputStream(downloadSocket.getOutputStream());
					downloadInFromServer = downloadSocket.getInputStream();
					//开始向一个终端中发送播放信息
					for(int j=0;j<videos.size();j++)
					{
						DownloadReq pfr = new DownloadReq(0X00000002, 404, terms.get(i).getTermNum(), j,
						"",videos.get(j).getFileName(),String.valueOf(videos.size()),url+videos.get(j).getFilePath(),		
						videos.get(j).getVideoName());
						
						System.out.println("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"+url+videos.get(j).getFilePath());
						//发送消息
						downloadOutToServer.write(pfr.getBuf());
						
					}
					// 通用的消息储存器
					tempMsg = new byte[500];
					// receive data from the server
					downloadInFromServer.read(tempMsg);
					// 判断回复的消息类型
					int msgType = ClientMessageManager.checkMsgTyper(tempMsg);
	
					//指定下载应答
					if(msgType == 0X80000002)
					{
						DownloadReqAck tempDownloadReqAck = ClientMessageManager.downloadReqAcker(tempMsg);
						if(tempDownloadReqAck.getState() == 0)
						{	
							downloadEnable = new DownloadResultContainer();
							downloadEnable.setClientName(terms.get(i).getTermName());
							downloadEnable.setReturnMark("2");
							resultsArray.add(downloadEnable);
						}
						else
						{
							noEnable = new DownloadResultContainer();
							noEnable.setClientName(terms.get(i).getTermName());
							noEnable.setReturnMark("2");
							resultsArray.add(noEnable);
						}

					}
					// close this tco connection
					downloadSocket.close();
				}
			}
				
			catch (IOException e ) 
			{
				if (downloadSocket != null) 
				{
					try 
					{
						downloadSocket.close();
					} catch (IOException e1) 
					{
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
					downloadSocket = null;
					//如果读超时则放入结果搜集器
					readNotEnable= new DownloadResultContainer();
					readNotEnable.setClientName(terms.get(i).getTermName());
					//读取超时的错误码是2
					readNotEnable.setReturnMark("4");
					resultsArray.add(readNotEnable);
				}
				continue;
			}
		}
		return resultsArray;
	}
	
	//滚动字幕
	public synchronized List captionMsgTalk(List<TermInfo> terms,String caption)
	{
		
		/**
		 * 判断软件是否已经到了有效期
		 * */
//		String currDate=TimeUtil.getFormatDateFileName(new Date());
//		Boolean flag=TimeUtil.compareDateTime(currDate, "20100113000000");
//		if(!flag){
//			return null; 
//		}
		
		//无连接标志 0
		CaptionResultContainer noClient = null;
		//服务端不存在1
		CaptionResultContainer noEnable = null;
		//成功  2
		CaptionResultContainer captionEnable = null;
		//找不到服务器IP标志 3
		CaptionResultContainer noIP = null;
		//读超时 4
		CaptionResultContainer readNotEnable =null;
		// socket
		Socket captionSocket = null;
		// 对方端口
		
//		int playPort = 0;
//		// server
//		String playRemoteHost = "localhost";
//		// server ip
//		InetAddress playRemoteIP;
		
		// stream write to server
		DataOutputStream captionOutToServer;
		// stream receive from the server
		InputStream captionInFromServer;
		//结果搜集器
		List resultsArray = new ArrayList();
		//IP地址容器
		String url;
		//拼接IP地址
		try 
		{
			url = "http://"+InetAddress.getLocalHost().getHostAddress()+"/";
		} 
		catch (UnknownHostException e) 
		{
			noIP= new CaptionResultContainer();
			noIP.setClientName("");
			noIP.setReturnMark("3");
			resultsArray.add(noIP);
			return resultsArray;
		}
		
		
		//主体程序
		//发送所有video信息
		
		for(int i=0;i<terms.size();i++)
		{
			try
			{
				//先测试service是否存在
				boolean isHas = checkService(terms.get(i));
				if(!isHas)
				{
					//如果无法连接则放入结果搜集器
					noClient= new CaptionResultContainer();
					noClient.setClientName(terms.get(i).getTermName());
					noClient.setReturnMark("1");
					resultsArray.add(noClient);
					continue;
				}
				else
				{
					//初始化流
					captionSocket = new Socket(terms.get(i).getTermIp(), terms.get(i).getTermCom());
					//设置超时
					captionSocket.setSoTimeout(5000);
					captionOutToServer = new DataOutputStream(captionSocket.getOutputStream());
					captionInFromServer = captionSocket.getInputStream();
					//开始向一个终端中发送播放信息

					CaptionReq pfr = new CaptionReq(0X00000006, 248, terms.get(i).getTermNum(), 0,
					caption,"","","","","","","","",""
					);
					
					//发送消息
					captionOutToServer.write(pfr.getBuf());
						
					// 通用的消息储存器
					tempMsg = new byte[500];
					// receive data from the server
					captionInFromServer.read(tempMsg);
					// 判断回复的消息类型
					int msgType = ClientMessageManager.checkMsgTyper(tempMsg);
	
					//播放请求应答
					if(msgType == 0X80000006)
					{
						CaptionReqAck tempCaptionReqAck = ClientMessageManager.captionReqAcker(tempMsg);
						if(tempCaptionReqAck.getState() == 1)
						{	
							captionEnable = new CaptionResultContainer();
							captionEnable.setClientName(terms.get(i).getTermName());
							captionEnable.setReturnMark("2");
							resultsArray.add(captionEnable);
						}
						else
						{
							noEnable = new CaptionResultContainer();
							noEnable.setClientName(terms.get(i).getTermName());
							noEnable.setReturnMark("0");
							resultsArray.add(noEnable);
							
						}
					}
					// close this tco connection
					captionSocket.close();
				}
			}
				
			catch (IOException e ) 
			{
				if (captionSocket != null) 
				{
					try 
					{
						captionSocket.close();
					} catch (IOException e1) 
					{
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
					captionSocket = null;
					//如果读超时则放入结果搜集器
//					readNotEnable= new CaptionResultContainer();
//					readNotEnable.setClientName(terms.get(i).getTermName());
//					//读取超时的错误码是2
//					readNotEnable.setReturnMark("4");
//					resultsArray.add(readNotEnable);
					captionEnable = new CaptionResultContainer();
					captionEnable.setClientName(terms.get(i).getTermName());
					captionEnable.setReturnMark("2");
					resultsArray.add(captionEnable);
				}
				continue;
			}
		}
		return resultsArray;
	}
	
	//网页浏览
	public synchronized List urlMsgTalk(List<TermInfo> terms,String url)
	{
		//无连接标志 0
		UrlResultContainer noClient = null;
		//服务端不存在1
		UrlResultContainer noEnable = null;
		//成功  2
		UrlResultContainer urlEnable = null;
		//找不到服务器IP标志 3
		UrlResultContainer noIP = null;
		//读超时 4
		UrlResultContainer readNotEnable =null;
		// socket
		Socket urlSocket = null;
		// 对方端口
		
//		int playPort = 0;
//		// server
//		String playRemoteHost = "localhost";
//		// server ip
//		InetAddress playRemoteIP;
		
		// stream write to server
		DataOutputStream urlOutToServer;
		// stream receive from the server
		InputStream urlInFromServer;
		//结果搜集器
		List resultsArray = new ArrayList();
		//IP地址容器
		String url0;
		//拼接IP地址
		try 
		{
			url0 = "http://"+InetAddress.getLocalHost().getHostAddress()+"/";
		} 
		catch (UnknownHostException e) 
		{
			noIP= new UrlResultContainer();
			noIP.setClientName("");
			noIP.setReturnMark("3");
			resultsArray.add(noIP);
			return resultsArray;
		}
		
		
		//主体程序
		//发送所有video信息
		
		for(int i=0;i<terms.size();i++)
		{
			try
			{
				//先测试service是否存在
				boolean isHas = checkService(terms.get(i));
				if(!isHas)
				{
					//如果无法连接则放入结果搜集器
					noClient= new UrlResultContainer();
					noClient.setClientName(terms.get(i).getTermName());
					noClient.setReturnMark("1");
					resultsArray.add(noClient);
					continue;
				}
				else
				{
					//初始化流
					urlSocket = new Socket(terms.get(i).getTermIp(), terms.get(i).getTermCom());
					//设置超时
					urlSocket.setSoTimeout(5000);
					urlOutToServer = new DataOutputStream(urlSocket.getOutputStream());
					urlInFromServer = urlSocket.getInputStream();
					//开始向一个终端中发送播放信息

					UrlReq pfr = new UrlReq(0X00000009, 176, terms.get(i).getTermNum(), 0,url);
					
					//发送消息
					urlOutToServer.write(pfr.getBuf());
						
					// 通用的消息储存器
					tempMsg = new byte[500];
					// receive data from the server
					urlInFromServer.read(tempMsg);
					// 判断回复的消息类型
					int msgType = ClientMessageManager.checkMsgTyper(tempMsg);
	
					//播放请求应答
					if(msgType == 0X80000006)
					{
						UrlReqAck tempCaptionReqAck = ClientMessageManager.urlReqAcker(tempMsg);
						if(tempCaptionReqAck.getState() == 1)
						{	
							urlEnable = new UrlResultContainer();
							urlEnable.setClientName(terms.get(i).getTermName());
							urlEnable.setReturnMark("2");
							resultsArray.add(urlEnable);
						}
						else
						{
							noEnable = new UrlResultContainer();
							noEnable.setClientName(terms.get(i).getTermName());
							noEnable.setReturnMark("0");
							resultsArray.add(noEnable);
							
						}
					}
					// close this tco connection
					urlSocket.close();
				}
			}
				
			catch (IOException e ) 
			{
				if (urlSocket != null) 
				{
					try 
					{
						urlSocket.close();
					} catch (IOException e1) 
					{
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
					urlSocket = null;
					//如果读超时则放入结果搜集器
//					readNotEnable= new UrlResultContainer();
//					readNotEnable.setClientName(terms.get(i).getTermName());
//					//读取超时的错误码是2
//					readNotEnable.setReturnMark("4");
//					resultsArray.add(readNotEnable);
					urlEnable = new UrlResultContainer();
					urlEnable.setClientName(terms.get(i).getTermName());
					urlEnable.setReturnMark("2");
					resultsArray.add(urlEnable);
				}
				continue;
			}
		}
		return resultsArray;
	}
}
	

