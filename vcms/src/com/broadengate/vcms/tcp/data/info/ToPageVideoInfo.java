package com.broadengate.vcms.tcp.data.info;

import java.util.Date;
import java.util.List;

public class ToPageVideoInfo {
	private String allTimes;
	private String nowTimes;
	private String nextName;
	private Integer videoId;
	private String videoName;
	private Date createTime;
	private String fileName;
	private Double fileSize;
	private String timeCount;
	private String descn;
	private List videoTypes;
	private int stateMark;

	


	public String getAllTimes() {
		return allTimes;
	}

	public void setAllTimes(String allTimes) {
		this.allTimes = allTimes;
	}

	public String getNowTimes() {
		return nowTimes;
	}

	public void setNowTimes(String nowTimes) {
		this.nowTimes = nowTimes;
	}

	public String getNextName() {
		return nextName;
	}

	public void setNextName(String nextName) {
		this.nextName = nextName;
	}



	// setter & getter
	public Integer getVideoId() {
		return videoId;
	}

	public void setVideoId(Integer videoId) {
		this.videoId = videoId;
	}

	public String getVideoName() {
		return videoName;
	}

	public void setVideoName(String videoName) {
		this.videoName = videoName;
	}

	public Date getCreateTime() {
		return createTime;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}

	public String getFileName() {
		return fileName;
	}

	public void setFileName(String fileName) {
		this.fileName = fileName;
	}

	public Double getFileSize() {
		return fileSize;
	}

	public void setFileSize(Double d) {
		this.fileSize = d;
	}

	public String getTimeCount() {
		return timeCount;
	}

	public void setTimeCount(String timeCount) {
		this.timeCount = timeCount;
	}

	public String getDescn() {
		return descn;
	}

	public void setDescn(String descn) {
		this.descn = descn;
	}

	public List getVideoTypes() {
		return videoTypes;
	}

	public void setVideoTypes(List videoTypes) {
		this.videoTypes = videoTypes;
	}
	
	public int getStateMark() {
		return stateMark;
	}

	public void setStateMark(int stateMark) {
		this.stateMark = stateMark;
	}
}
