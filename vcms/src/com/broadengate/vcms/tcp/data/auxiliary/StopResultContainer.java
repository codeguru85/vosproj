package com.broadengate.vcms.tcp.data.auxiliary;

public class StopResultContainer {
	private String clientName;
	private String returnMark;//0:无法连接 1：播放失败 2：播放成功
	//getter & setter
	public String getClientName() {
		return clientName;
	}
	public void setClientName(String clientName) {
		this.clientName = clientName;
	}
	public String getReturnMark() {
		return returnMark;
	}
	public void setReturnMark(String returnMark) {
		this.returnMark = returnMark;
	}


}
