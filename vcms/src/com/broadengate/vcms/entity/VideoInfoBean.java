/**Administrator
 * 上午11:26:14
 */
package com.broadengate.vcms.entity;

import java.util.Date;
import java.util.HashSet;
import java.util.Set;

/**
 * @author Administrator
 *
 */
public class VideoInfoBean {

	
	
	/**
	 * 视频信息列表传值的bean
	 * */
	private Integer videoId;
	private UserInfo userInfo;
	private String videoName;
	private Date createTime;
	private String fileName;
	private String fileFormat;
	private String filePath;
	private Double fileSize;
	private String imageName;
	private String imagePath;
	private String timeCount;
	private String descn;
	
	private String startTime;
	private String endTime;
	private String startSize;
	private String endSize;
	private Date startDate;
	private Date endDate;
	
	private Set<Keyword> keywords = new HashSet<Keyword>(0);
	private Set<VideoType> videoTypes = new HashSet<VideoType>(0);
	private Set<PlayListShip> playListShips = new HashSet<PlayListShip>(0);
	private Set<VideoAlbumShip> videoAlbumShips = new HashSet<VideoAlbumShip>(0);
	
	/**
	 * @return the videoId
	 */
	public Integer getVideoId() {
		return videoId;
	}
	/**
	 * @return the userInfo
	 */
	public UserInfo getUserInfo() {
		return userInfo;
	}
	/**
	 * @return the videoName
	 */
	public String getVideoName() {
		return videoName;
	}
	/**
	 * @return the createTime
	 */
	public Date getCreateTime() {
		return createTime;
	}
	/**
	 * @return the fileName
	 */
	public String getFileName() {
		return fileName;
	}
	/**
	 * @return the filePath
	 */
	public String getFilePath() {
		return filePath;
	}
	/**
	 * @return the fileSize
	 */
	public Double getFileSize() {
		return fileSize;
	}
	/**
	 * @return the imageName
	 */
	public String getImageName() {
		return imageName;
	}
	/**
	 * @return the imagePath
	 */
	public String getImagePath() {
		return imagePath;
	}
	/**
	 * @return the timeCount
	 */
	public String getTimeCount() {
		return timeCount;
	}
	/**
	 * @return the descn
	 */
	public String getDescn() {
		return descn;
	}
	/**
	 * @param videoId the videoId to set
	 */
	public void setVideoId(Integer videoId) {
		this.videoId = videoId;
	}
	/**
	 * @param userInfo the userInfo to set
	 */
	public void setUserInfo(UserInfo userInfo) {
		this.userInfo = userInfo;
	}
	/**
	 * @param videoName the videoName to set
	 */
	public void setVideoName(String videoName) {
		this.videoName = videoName;
	}
	/**
	 * @param createTime the createTime to set
	 */
	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}
	/**
	 * @param fileName the fileName to set
	 */
	public void setFileName(String fileName) {
		this.fileName = fileName;
	}
	
	
	/**
	 * @return the fileFormat
	 */
	public String getFileFormat() {
		return fileFormat;
	}
	/**
	 * @param fileFormat the fileFormat to set
	 */
	public void setFileFormat(String fileFormat) {
		this.fileFormat = fileFormat;
	}
	/**
	 * @param filePath the filePath to set
	 */
	public void setFilePath(String filePath) {
		this.filePath = filePath;
	}
	/**
	 * @param fileSize the fileSize to set
	 */
	public void setFileSize(Double fileSize) {
		this.fileSize = fileSize;
	}
	/**
	 * @param imageName the imageName to set
	 */
	public void setImageName(String imageName) {
		this.imageName = imageName;
	}
	/**
	 * @param imagePath the imagePath to set
	 */
	public void setImagePath(String imagePath) {
		this.imagePath = imagePath;
	}
	/**
	 * @param timeCount the timeCount to set
	 */
	public void setTimeCount(String timeCount) {
		this.timeCount = timeCount;
	}
	/**
	 * @param descn the descn to set
	 */
	public void setDescn(String descn) {
		this.descn = descn;
	}
	
	/**
	 * @return the startTime
	 */
	public String getStartTime() {
		return startTime;
	}
	/**
	 * @return the endTime
	 */
	public String getEndTime() {
		return endTime;
	}
	/**
	 * @return the startSize
	 */
	public String getStartSize() {
		return startSize;
	}
	/**
	 * @return the endSize
	 */
	public String getEndSize() {
		return endSize;
	}
	/**
	 * @param startTime the startTime to set
	 */
	public void setStartTime(String startTime) {
		this.startTime = startTime;
	}
	/**
	 * @param endTime the endTime to set
	 */
	public void setEndTime(String endTime) {
		this.endTime = endTime;
	}
	/**
	 * @param startSize the startSize to set
	 */
	public void setStartSize(String startSize) {
		this.startSize = startSize;
	}
	/**
	 * @param endSize the endSize to set
	 */
	public void setEndSize(String endSize) {
		this.endSize = endSize;
	}
	/**
	 * @return the startDate
	 */
	public Date getStartDate() {
		return startDate;
	}
	/**
	 * @return the endDate
	 */
	public Date getEndDate() {
		return endDate;
	}
	/**
	 * @param startDate the startDate to set
	 */
	public void setStartDate(Date startDate) {
		this.startDate = startDate;
	}
	/**
	 * @param endDate the endDate to set
	 */
	public void setEndDate(Date endDate) {
		this.endDate = endDate;
	}
	/**
	 * @return the keywords
	 */
	public Set<Keyword> getKeywords() {
		return keywords;
	}
	/**
	 * @return the videoTypes
	 */
	public Set<VideoType> getVideoTypes() {
		return videoTypes;
	}
	/**
	 * @return the playListShips
	 */
	public Set<PlayListShip> getPlayListShips() {
		return playListShips;
	}
	/**
	 * @return the videoAlbumShips
	 */
	public Set<VideoAlbumShip> getVideoAlbumShips() {
		return videoAlbumShips;
	}
	/**
	 * @param keywords the keywords to set
	 */
	public void setKeywords(Set<Keyword> keywords) {
		this.keywords = keywords;
	}
	/**
	 * @param videoTypes the videoTypes to set
	 */
	public void setVideoTypes(Set<VideoType> videoTypes) {
		this.videoTypes = videoTypes;
	}
	/**
	 * @param playListShips the playListShips to set
	 */
	public void setPlayListShips(Set<PlayListShip> playListShips) {
		this.playListShips = playListShips;
	}
	/**
	 * @param videoAlbumShips the videoAlbumShips to set
	 */
	public void setVideoAlbumShips(Set<VideoAlbumShip> videoAlbumShips) {
		this.videoAlbumShips = videoAlbumShips;
	}
	
}
