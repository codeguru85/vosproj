package com.broadengate.vcms.tcp.data.auxiliary;

import com.broadengate.vcms.tcp.data.info.DeviceControlAck;
import com.broadengate.vcms.tcp.data.info.PlayFileReqAck;
import com.broadengate.vcms.tcp.data.info.DownloadReqAck;
import com.broadengate.vcms.tcp.data.info.CaptionReqAck;
import com.broadengate.vcms.tcp.data.info.UrlReqAck;
import com.broadengate.vcms.tcp.data.info.PlayStateCommandAck;
import com.broadengate.vcms.tcp.data.info.SetDevicePropertyAck;
import com.broadengate.vcms.tcp.data.info.ShowPlayFileAck;
import com.broadengate.vcms.tcp.data.tool.NumberToByte;

public class ClientMessageManager {
	// 得到消息类型
	public static int checkMsgTyper(byte[] msg) {
		// 处理消息
		byte[] commandid = new byte[4];
		System.arraycopy(msg, 0, commandid, 0, 4);
		int msgTypeId = NumberToByte.byte2int(commandid);
		return msgTypeId;
	}

	// 控制回复信息处理者
	public static DeviceControlAck playControlAcker(byte[] playControlAck) {
		ClientMessageManager cmm = new ClientMessageManager();
		// 得到消息头
		HeadBox headBox = cmm.msgHeader(playControlAck);
		DeviceControlAck dca = new DeviceControlAck();
		// 装载控制回复消息
		dca.setDevice_id(headBox.getMMachineId());
		dca.setMessageLength(headBox.getMLength());
		dca.setMessageType(headBox.getMHeadType());
		dca.setStatus_id(headBox.getMState());
		return dca;
	}

	// 修改参数回复信息处理者
	public static SetDevicePropertyAck setDevicePropertyAcker(
			byte[] setDeviceAcks) {
		ClientMessageManager cmm = new ClientMessageManager();
		// 得到消息头
		HeadBox headBox = cmm.msgHeader(setDeviceAcks);
		SetDevicePropertyAck sdpa = new SetDevicePropertyAck();
		// 装载修改参数回复信息
		sdpa.setDevice_id(headBox.getMMachineId());
		sdpa.setMessageLength(headBox.getMLength());
		sdpa.setMessageType(headBox.getMHeadType());
		sdpa.setStatus_id(headBox.getMState());
		return sdpa;
	}

	// 反向查询回复信息
	public static ShowPlayFileAck showPlayFileAcker(byte[] showPlayFiles) {
		byte[] propertyTempByte = new byte[130];
		String mPlayMode;
		String mMediaType;
		String mMediaName;
		String mSize;
		String mUrl;
		String mAllTime;
		String mNowTime;
		String mNextName;
////test
//		ShowPlayFileTest spft = new ShowPlayFileTest(
//				0X80000004,308,
//				"75845738457389457398765678934258", 0, 
//				"0", "avi",
//				"20090706174343.flv", "789223", "http://localhost:8080/vcms/security.html?debug=true#app=1061&30f5-selectedIndex=1"		
//		);
//		showPlayFiles = spft.getBuf();
//		
////
		ClientMessageManager cmm = new ClientMessageManager();
		// 得到消息头
		HeadBox headBox = cmm.msgHeader(showPlayFiles);
		// 分析消息体
		byte[] body = new byte[headBox.getMLength() - 48];
		System.arraycopy(showPlayFiles, 48, body, 0, headBox.getMLength()-48);
		// 消息体处理
		// 播放模式
		mPlayMode = getProperty(body, 0, propertyTempByte, 0, 4);
		propertyTempByte = new byte[130];
		// 文件类型
		mMediaType = getProperty(body, 4, propertyTempByte, 0, 32);
		propertyTempByte = new byte[130];
		// 文件名
		mMediaName = getProperty(body, 36, propertyTempByte, 0, 64);
		propertyTempByte = new byte[130];
		// 文件大小
		mSize = getProperty(body, 100, propertyTempByte, 0, 32);
		propertyTempByte = new byte[130];
		// 文件URL
		mUrl = getProperty(body, 132, propertyTempByte, 0, 128);
		propertyTempByte = new byte[130];
		//当前影片总时间
		mAllTime = getProperty(body, 260, propertyTempByte, 0, 8);
		propertyTempByte = new byte[130];
		//已播放时间
		mNowTime = getProperty(body, 268, propertyTempByte, 0, 8);
		propertyTempByte = new byte[130];
		//下部影片名称
//		mNextName = getProperty(body, 276, propertyTempByte, 0, 64);
//		propertyTempByte = new byte[130];
		//装配回复信息
		ShowPlayFileAck spfa = new ShowPlayFileAck();
		spfa.setMCommandId(headBox.getMHeadType());
		spfa.setMCommandLength(headBox.getMLength());
		spfa.setMMachineid(headBox.getMMachineId());
		spfa.setMState(headBox.getMState());
		
		spfa.setMMediaName(mMediaName);
		spfa.setMMediaType(mMediaType);
		spfa.setMPlayMode(mPlayMode);
		spfa.setMSize(mSize);
		spfa.setMUrl(mUrl);
		spfa.setMAllTime(mAllTime);
		spfa.setMNowTime(mNowTime);
//		spfa.setMNextName(mNextName);
		return spfa;
	}

//	public static void main(String[] args) {
//		
//		
//		//测试
//		ShowPlayFileTest spft = new ShowPlayFileTest(
//				0X80000004,308,
//				"75845738457389457398765678934258", 0, 
//				"0", "avi",
//				"huasnfsasd", "789223", "http://localhost:8080/vcms/security.html?debug=true#app=1061&30f5-selectedIndex=1"		
//		);
//		
//		ShowPlayFileAck test  = showPlayFileAcker(spft.getBuf());
//		
//		test.getMCommandId();
//	}
	// 消息头截取
	private HeadBox msgHeader(byte[] commands) {
		byte[] commandid = new byte[4];
		byte[] commandlength = new byte[4];
		byte[] machineid = new byte[36];
		byte[] state = new byte[4];

		System.arraycopy(commands, 0, commandid, 0, 4);
		System.arraycopy(commands, 4, commandlength, 0, 4);
		System.arraycopy(commands, 8, machineid, 0, 36);
		System.arraycopy(commands, 44, state, 0, 4);

		int mHeadType = 0;
		int mLength = 0;
		String mMachineId = "";
		int mState = 0;
		// 消息头
		mHeadType = NumberToByte.byte2int(commandid);
		mLength = NumberToByte.byte2int(commandlength);
		mMachineId = NumberToByte.byte2string(machineid, 0, 32);
		mState = NumberToByte.byte2int(state);
		// 构造装载器
		HeadBox hb = new HeadBox();
		hb.setMHeadType(mHeadType);
		hb.setMLength(mLength);
		hb.setMMachineId(mMachineId);
		hb.setMState(mState);
		return hb;
	}

	// 截取读取设备参数响应报文的参数
	private static String getProperty(byte[] src, int srcPos, byte[] dest,
			int destPos, int length) {
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
	
	/**
	 * 处理播放回复信息
	 */
	public static PlayFileReqAck playFileReqAcker(byte[] playAcks){
		ClientMessageManager cmm = new ClientMessageManager();
		HeadBox headBox = cmm.msgHeader(playAcks);
		PlayFileReqAck tempPlayAck = new PlayFileReqAck(headBox.getMHeadType(),headBox.getMLength(),headBox.getMMachineId(),headBox.getMState());
		return tempPlayAck;
	}
	
	/**
	 * 处理停止回复信息
	 */
	public static PlayStateCommandAck stopFileReqAcker(byte[] stopAcks){
		ClientMessageManager cmm = new ClientMessageManager();
		HeadBox headBox = cmm.msgHeader(stopAcks);
		PlayStateCommandAck tempStopAck = new PlayStateCommandAck(headBox.getMHeadType(),headBox.getMLength(),headBox.getMMachineId(),headBox.getMState());
		return tempStopAck;
	}
	/**
	 * 处理指定下载回复信息
	 */
	public static DownloadReqAck downloadReqAcker(byte[] downloadAcks){
		ClientMessageManager cmm = new ClientMessageManager();
		HeadBox headBox = cmm.msgHeader(downloadAcks);
		DownloadReqAck tempDownloadAck = new DownloadReqAck(headBox.getMHeadType(),headBox.getMLength(),headBox.getMMachineId(),headBox.getMState());
		return tempDownloadAck;
	}
	/**
	 * 处理滚动字幕回复信息
	 */
	public static CaptionReqAck captionReqAcker(byte[] captionAcks){
		ClientMessageManager cmm = new ClientMessageManager();
		HeadBox headBox = cmm.msgHeader(captionAcks);
		CaptionReqAck tempCaptionAck = new CaptionReqAck(headBox.getMHeadType(),headBox.getMLength(),headBox.getMMachineId(),headBox.getMState());
		return tempCaptionAck;
	}
	/**
	 * 处理网址设置回复信息
	 */
	public static UrlReqAck urlReqAcker(byte[] urlAcks){
		ClientMessageManager cmm = new ClientMessageManager();
		HeadBox headBox = cmm.msgHeader(urlAcks);
		UrlReqAck tempUrlAck = new UrlReqAck(headBox.getMHeadType(),headBox.getMLength(),headBox.getMMachineId(),headBox.getMState());
		return tempUrlAck;
	}
}

//class ShowPlayFileTest {
//	// String mPlayMode;
//	// String mMediaType;
//	// String mMediaName;
//	// String mSize;
//	// String mUrl;
//	private byte[] buf = new byte[308]; // 为说明问题，定死大小，事件中可以灵活处理
//
//	public ShowPlayFileTest(int commandid, int commandlength,
//			String smachineid, int state,
//
//			String mPlayMode, String mMediaType, String mMediaName,
//			String mSize, String mUrl) {
//
//		byte[] temp = NumberToByte.toLH(commandid);
//		System.arraycopy(temp, 0, buf, 0, temp.length);
//		//   
//		temp = NumberToByte.toLH(commandlength);
//		System.arraycopy(temp, 0, buf, 4, temp.length);
//		//   
//		temp = smachineid.getBytes();
//		byte[] tempTemp = new byte[36];
//		System.arraycopy(temp, 0, tempTemp, 0, temp.length);
//		System.arraycopy(tempTemp, 0, buf, 8, 36);
//
//		temp = NumberToByte.toLH(state);
//		System.arraycopy(temp, 0, buf, 44, temp.length);
//
//		//
//
//		temp = mPlayMode.getBytes();
//		System.arraycopy(temp, 0, buf, 48, temp.length);
//
//		temp = mMediaType.getBytes();
//		System.arraycopy(temp, 0, buf, 52, temp.length);
//
//		temp = mMediaName.getBytes();
//		System.arraycopy(temp, 0, buf, 84, temp.length);
//
//		temp = mSize.getBytes();
//		System.arraycopy(temp, 0, buf, 148, temp.length);
//
//		temp = mUrl.getBytes();
//		System.arraycopy(temp, 0, buf, 180, temp.length);
//	}
//	
//
//	
//    /**   
//     *   返回要发送的数组   
//     */   
//   public   byte[]   getBuf()   {   
//       return   buf;   
//   }
//}