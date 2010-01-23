package com.broadengate.vcms.tcp.data.auxiliary;

public class DownloadResultContainer {
	private String clientName;
	private String ReturnMark;//0:无法链接   1：服务端不存在   2：下载成功   3：找不到IP  4：读超时
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
