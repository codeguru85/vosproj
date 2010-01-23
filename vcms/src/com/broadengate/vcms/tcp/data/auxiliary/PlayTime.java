package com.broadengate.vcms.tcp.data.auxiliary;

public class PlayTime {
	private Integer videoId;
	private String playFilePath;
	private String playVideoName;
	private String playTime;
	private String timeCount;
	public String getTimeCount() {
		return timeCount;
	}
	public void setTimeCount(String timeCount) {
		this.timeCount = timeCount;
	}
	public String getPlayFilePath() {
		return playFilePath;
	}
	public void setPlayFilePath(String playFilePath) {
		this.playFilePath = playFilePath;
	}
	public String getPlayVideoName() {
		return playVideoName;
	}
	public void setPlayVideoName(String playVideoName) {
		this.playVideoName = playVideoName;
	}
	public Integer getVideoId() {
		return videoId;
	}
	public void setVideoId(Integer videoId) {
		this.videoId = videoId;
	}
	public String getPlayTime() {
		return playTime;
	}
	public void setPlayTime(String playTime) {
		this.playTime = playTime;
	}
}
