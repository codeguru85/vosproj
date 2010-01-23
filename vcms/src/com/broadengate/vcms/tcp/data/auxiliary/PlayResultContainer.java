package com.broadengate.vcms.tcp.data.auxiliary;

public class PlayResultContainer {
	private String clientName;
	private String ReturnMark;//0:无法连接 1：播放失败 2：播放成功3:文件本地不存在
	//getter & setter
	public String getClientName() {
		return clientName;
	}
	public void setClientName(String clientName) {
		this.clientName = clientName;
	}
	public String getReturnMark() {
		return ReturnMark;
	}
	public void setReturnMark(String returnMark) {
		ReturnMark = returnMark;
	}

}
