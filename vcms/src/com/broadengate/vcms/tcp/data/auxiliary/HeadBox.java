package com.broadengate.vcms.tcp.data.auxiliary;

public class HeadBox {
	private int mHeadType = 0;
	private int mLength = 0;
	private String mMachineId = "";
	private int mState = 0;
	//geter & setter
	public int getMHeadType() {
		return mHeadType;
	}
	public void setMHeadType(int headType) {
		mHeadType = headType;
	}
	public int getMLength() {
		return mLength;
	}
	public void setMLength(int length) {
		mLength = length;
	}
	public String getMMachineId() {
		return mMachineId;
	}
	public void setMMachineId(String machineId) {
		mMachineId = machineId;
	}
	public int getMState() {
		return mState;
	}
	public void setMState(int state) {
		mState = state;
	}
}
