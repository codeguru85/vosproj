package com.broadengate.vcms.tcp.data.info;


public class PlayStateCommandAck {
	private int commandid;
	private int commandlength;
	private String smachineid;
	private int state;

	/**
	 * 构造并转换
	 */
	public PlayStateCommandAck(int commandid, int commandlength,
			String smachineid, int state) {

	}
	
	//getter & setter
	public int getCommandid() {
		return commandid;
	}

	public void setCommandid(int commandid) {
		this.commandid = commandid;
	}

	public int getCommandlength() {
		return commandlength;
	}

	public void setCommandlength(int commandlength) {
		this.commandlength = commandlength;
	}

	public String getSmachineid() {
		return smachineid;
	}

	public void setSmachineid(String smachineid) {
		this.smachineid = smachineid;
	}

	public int getState() {
		return state;
	}

	public void setState(int state) {
		this.state = state;
	}

}
