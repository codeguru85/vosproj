package com.broadengate.vcms.entity;

// default package
// Generated 2009-7-15 10:27:13 by Hibernate Tools 3.2.2.GA

import java.util.Date;
import java.util.HashSet;
import java.util.Set;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.GeneratedValue;
import static javax.persistence.GenerationType.IDENTITY;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 * VideoType generated by hbm2java
 */
@Entity
@Table(name = "video_type", catalog = "vcms")
public class VideoType implements java.io.Serializable {

	private Integer typeId;
	private VideoType videoType;
	private String typeName;
	private Date createTime;
	private Integer videoNum;
	private Set<VideoInfo> videoInfos = new HashSet<VideoInfo>(0);
	private Set<VideoType> videoTypes = new HashSet<VideoType>(0);

	public VideoType() {
	}

	public VideoType(VideoType videoType, String typeName, Date createTime,
			Integer videoNum, Set<VideoInfo> videoInfos,
			Set<VideoType> videoTypes) {
		this.videoType = videoType;
		this.typeName = typeName;
		this.createTime = createTime;
		this.videoNum = videoNum;
		this.videoInfos = videoInfos;
		this.videoTypes = videoTypes;
	}

	@Id
	@GeneratedValue(strategy = IDENTITY)
	@Column(name = "TYPE_ID", unique = true, nullable = false)
	public Integer getTypeId() {
		return this.typeId;
	}

	public void setTypeId(Integer typeId) {
		this.typeId = typeId;
	}

	@ManyToOne(fetch = FetchType.EAGER)
	@JoinColumn(name = "VID_TYPE_ID")
	public VideoType getVideoType() {
		return this.videoType;
	}

	public void setVideoType(VideoType videoType) {
		this.videoType = videoType;
	}

	@Column(name = "TYPE_NAME", length = 64)
	public String getTypeName() {
		return this.typeName;
	}

	public void setTypeName(String typeName) {
		this.typeName = typeName;
	}

	@Temporal(TemporalType.TIMESTAMP)
	@Column(name = "CREATE_TIME", length = 0)
	public Date getCreateTime() {
		return this.createTime;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}

	@Column(name = "VIDEO_NUM")
	public Integer getVideoNum() {
		return this.videoNum;
	}

	public void setVideoNum(Integer videoNum) {
		this.videoNum = videoNum;
	}

	@ManyToMany(cascade = CascadeType.ALL, fetch = FetchType.LAZY)
	@JoinTable(name = "video_type_ship", catalog = "vcms", joinColumns = { @JoinColumn(name = "TYPE_ID", nullable = false, updatable = false) }, inverseJoinColumns = { @JoinColumn(name = "VIDEO_ID", nullable = false, updatable = false) })
	public Set<VideoInfo> getVideoInfos() {
		return this.videoInfos;
	}

	public void setVideoInfos(Set<VideoInfo> videoInfos) {
		this.videoInfos = videoInfos;
	}

	@OneToMany(cascade = CascadeType.ALL, fetch = FetchType.EAGER, mappedBy = "videoType")
	public Set<VideoType> getVideoTypes() {
		return this.videoTypes;
	}

	public void setVideoTypes(Set<VideoType> videoTypes) {
		this.videoTypes = videoTypes;
	}

}