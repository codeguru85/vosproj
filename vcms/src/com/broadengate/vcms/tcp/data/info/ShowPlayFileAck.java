package com.broadengate.vcms.tcp.data.info;

public class ShowPlayFileAck {
	private int mCommandId;
	private int mCommandLength;
	private String mMachineid;
	private int mState;
	private String mPlayMode;
	private String mMediaType;
	private String mMediaName;
	private String mSize;
	private String mUrl;
	private String mAllTime;
	private String mNowTime;
	private String mNextName;
	
	//getter & setter
	public String getMAllTime() {
		return mAllTime;
	}
	public void setMAllTime(String allTime) {
		mAllTime = allTime;
	}
	public String getMNowTime() {
		return mNowTime;
	}
	public void setMNowTime(String nowTime) {
		mNowTime = nowTime;
	}
	public String getMNextName() {
		return mNextName;
	}
	public void setMNextName(String nextName) {
		mNextName = nextName;
	}
	
	public int getMCommandId() {
		return mCommandId;
	}
	public void setMCommandId(int commandId) {
		mCommandId = commandId;
	}
	public int getMCommandLength() {
		return mCommandLength;
	}
	public void setMCommandLength(int commandLength) {
		mCommandLength = commandLength;
	}
	public String getMMachineid() {
		return mMachineid;
	}
	public void setMMachineid(String machineid) {
		mMachineid = machineid;
	}
	public int getMState() {
		return mState;
	}
	public void setMState(int state) {
		mState = state;
	}
	public String getMPlayMode() {
		return mPlayMode;
	}
	public void setMPlayMode(String playMode) {
		mPlayMode = playMode;
	}
	public String getMMediaType() {
		return mMediaType;
	}
	public void setMMediaType(String mediaType) {
		mMediaType = mediaType;
	}
	public String getMMediaName() {
		return mMediaName;
	}
	public void setMMediaName(String mediaName) {
		mMediaName = mediaName;
	}
	public String getMSize() {
		return mSize;
	}
	public void setMSize(String size) {
		mSize = size;
	}
	public String getMUrl() {
		return mUrl;
	}
	public void setMUrl(String url) {
		mUrl = url;
	}
}
