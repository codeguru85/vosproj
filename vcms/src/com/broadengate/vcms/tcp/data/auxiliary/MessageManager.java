package com.broadengate.vcms.tcp.data.auxiliary;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;

import org.springframework.web.context.WebApplicationContext;

import com.broadengate.vcms.service.ITermService;
import com.broadengate.vcms.tcp.data.TermsClient;
import com.broadengate.vcms.tcp.data.info.LoginCome;
import com.broadengate.vcms.tcp.data.info.LoginReqAck;
import com.broadengate.vcms.tcp.data.tool.NumberToByte;



public class MessageManager {
    private WebApplicationContext webApp = null;//WebApplicationContext
    private TermsClient termsClient = null;
    private String farIp = "";
    private int farPort = 0;
    private SocketChannel tempChannel;
    
	public  boolean selector(byte[] commands,WebApplicationContext webApp,SocketChannel channel){	
		this.webApp = webApp;
		this.farIp = channel.socket().getInetAddress().getHostAddress();
		this.farPort = channel.socket().getPort();
		this.tempChannel = channel;
		 //处理消息
        byte[] commandid = new byte[4];
        System.arraycopy(commands, 0, commandid, 0, 4);

		int mHeadType = 0;
        //消息头
		mHeadType = NumberToByte.byte2int(commandid);
        //消息体
		if(mHeadType == 0X00000001){ 
			
			return loginAuxer(commands);
		}
		
		return true;
	}
	
	//处理注册信息

	public  boolean loginAuxer( byte[] commands){
		byte[] propertyTempByte = new byte[130];
		String deviceName = "";
		String networkIp = "";
		String networkMask = "";
		String networkGateway = "";
		String networkDns = "";
		String position = "";
		String scal = "";
		String resolution = "";
		String initialScreen = "";
		String mediaRoot = "";
		String termCom;
		int mLength = 0;
		String mMachineId = "";
		
		byte[] commandlength = new byte[4];
		byte[] machineid = new byte[36];
		
		System.arraycopy(commands, 4,commandlength ,0, 4);
		System.arraycopy(commands , 8, machineid, 0, 36);
		
		mLength = NumberToByte.byte2int(commandlength);
		mMachineId = NumberToByte.byte2string(machineid,0,32);
		byte [] loginMsg = new byte[mLength];
		byte [] body = new byte[mLength-48];
		
		//截取消息体
		System.arraycopy(commands, 0, loginMsg, 0,mLength);
		System.arraycopy(loginMsg, 48, body, 0,344);
		// deviceName设备名称
		deviceName = getProperty(body, 0,
				propertyTempByte, 0, 32);
		//清空数组
		propertyTempByte = new byte[130];
		// networkIp设备IP地址
		networkIp = getProperty(body, 32,
				propertyTempByte, 0, 16);
		//清空数组
		propertyTempByte = new byte[130];
		// networkMask设备子网掩码
		networkMask = getProperty(body, 48,
				propertyTempByte, 0, 16);
		//清空数组
		propertyTempByte = new byte[130];
		// networkGateway设备网关
		networkGateway = getProperty(body, 64,
				propertyTempByte, 0, 16);
		//清空数组
		propertyTempByte = new byte[130];
		// networkGateway设备DNS
		networkDns = getProperty(body, 80,
				propertyTempByte, 0, 16);
		//清空数组
		propertyTempByte = new byte[130];
		// position设备所在位置
		position = getProperty(body, 96,
				propertyTempByte, 0, 64);
		//清空数组
		propertyTempByte = new byte[130];
		// scal画面输出比例
		scal = getProperty(body, 160,
				propertyTempByte, 0, 16);
		//清空数组
		propertyTempByte = new byte[130];
		// resolution画面分辨率
		resolution = getProperty(body, 176,
				propertyTempByte, 0, 16);
		//清空数组
		propertyTempByte = new byte[130];
		// initialScreen初始画面内容
		initialScreen = getProperty(body, 192,
				propertyTempByte, 0, 16);
		//清空数组
		propertyTempByte = new byte[130];
		// mediaRoot媒体文件保存路径
		mediaRoot = getProperty(body, 208,
				propertyTempByte, 0, 128);
		//清空数组
		propertyTempByte = new byte[130];
		//bindport端口
		termCom = getProperty(body, 336,
				propertyTempByte, 0, 8);
		//查询数据库是否终端已注册
		//根据mMachineId 判断是否已存在有该TermNum的机器注册(TermNum就是 mMachineId)
		//实例化传递用容器
		LoginCome lc = new LoginCome();
		lc.setTermName(deviceName);
		//这里放置消息包的ip地址而不是消息中的ip
		lc.setTermIp(farIp);
		//这里放置的是远端端口
		lc.setTermCom(termCom);
		lc.setTermMask(networkMask);
		lc.setTermGateway(networkGateway);
		lc.setTermDns(networkDns);
		lc.setTermAddr(position);
		lc.setTermScale(scal);
		lc.setTermResolution(resolution);
		lc.setTermInitialscreen(initialScreen);
		lc.setTermRoot(mediaRoot);
		lc.setTermNum(mMachineId);
		lc.setTermActivation(1);
		//将新的终端信息放入数据库
		ITermService its = (ITermService) webApp.getBean("termService");
		boolean continuMark = its.getTermByTermNum(lc);
		
		//向终端发送注册回复信息
		
		try{
			ByteBuffer bytebuf = ByteBuffer.allocate(1024);
			if(tempChannel!=null||continuMark){
				LoginReqAck lra = new LoginReqAck(0X80000001,48,mMachineId,0);
				bytebuf = ByteBuffer.wrap(lra.getBuf());
				int isWin = tempChannel.write(bytebuf);
				System.out.println(isWin);
			}
			else{
				LoginReqAck lra = new LoginReqAck(0X80000001,48,mMachineId,1);
				bytebuf = ByteBuffer.wrap(lra.getBuf());
			}
		}catch (Exception e) {
			try {
				tempChannel.close();
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			System.out.println("有个终端关闭了！");
		}
		return continuMark;
	}
	
	
	// 截取读取设备参数响应报文的参数
	private static String getProperty(byte[] src, int srcPos, byte[] dest, int destPos,
			int length) {
		String TempString = "";
		System.arraycopy(src, srcPos, dest, destPos, length);
		for (int i = 0; i < dest.length; i++) {

			if (dest[i] == 0) {
				break;
			} else {
				TempString += (char) dest[i];
			}
		}
		return TempString;
	}
	
}
