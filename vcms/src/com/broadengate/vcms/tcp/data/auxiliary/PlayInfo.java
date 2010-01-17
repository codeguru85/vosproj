package com.broadengate.vcms.tcp.data.auxiliary;

import java.util.Date;

public class PlayInfo {
	private Integer listId;
	private String listName;
	private Date createTime;
	private String playState;
	private Integer videoNum;
	private Date startDate;
	private Date endDate;
	public Date getStartDate() {
		return startDate;
	}
	public void setStartDate(Date startDate) {
		this.startDate = startDate;
	}
	public Date getEndDate() {
		return endDate;
	}
	public void setEndDate(Date endDate) {
		this.endDate = endDate;
	}
	public Integer getListId() {
		return listId;
	}
	public void setListId(Integer listId) {
		this.listId = listId;
	}
	public String getListName() {
		return listName;
	}
	public void setListName(String listName) {
		this.listName = listName;
	}
	public Date getCreateTime() {
		return createTime;
	}
	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}
	public String getPlayState() {
		return playState;
	}
	public void setPlayState(String playState) {
		this.playState = playState;
	}
	public Integer getVideoNum() {
		return videoNum;
	}
	public void setVideoNum(Integer videoNum) {
		this.videoNum = videoNum;
	}
}
