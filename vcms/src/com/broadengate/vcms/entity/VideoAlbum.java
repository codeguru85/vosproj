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
import javax.persistence.OneToMany;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 * VideoAlbum generated by hbm2java
 */
@Entity
@Table(name = "video_album", catalog = "vcms")
public class VideoAlbum implements java.io.Serializable {

	private Integer albumId;
	private String albumName;
	private Integer videoNum;
	private Date createTime;
	private String descn;
	private String imagePath;
	private Set<VideoAlbumShip> videoAlbumShips = new HashSet<VideoAlbumShip>(0);

	public VideoAlbum() {
	}

	public VideoAlbum(String albumName, Integer videoNum, Date createTime, String descn,String imagePath,
			Set<VideoAlbumShip> videoAlbumShips) {
		this.albumName = albumName;
		this.videoNum = videoNum;
		this.createTime = createTime;
		this.descn = descn;
		this.imagePath = imagePath;
		this.videoAlbumShips = videoAlbumShips;
	}

	@Id
	@GeneratedValue(strategy = IDENTITY)
	@Column(name = "ALBUM_ID", unique = true, nullable = false)
	public Integer getAlbumId() {
		return this.albumId;
	}

	public void setAlbumId(Integer albumId) {
		this.albumId = albumId;
	}

	@Column(name = "ALBUM_NAME", length = 128)
	public String getAlbumName() {
		return this.albumName;
	}

	public void setAlbumName(String albumName) {
		this.albumName = albumName;
	}
	
	@Column(name = "VIDEO_NUM")
	public Integer getVideoNum() {
		return this.videoNum;
	}

	public void setVideoNum(Integer videoNum) {
		this.videoNum = videoNum;
	}

	@Temporal(TemporalType.TIMESTAMP)
	@Column(name = "CREATE_TIME", length = 0)
	public Date getCreateTime() {
		return this.createTime;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}

	@Column(name = "DESCN", length = 2000)
	public String getDescn() {
		return this.descn;
	}

	public void setDescn(String descn) {
		this.descn = descn;
	}
	
	@Column(name = "IMAGE_PATH", nullable = false)
	public String getImagePath() {
		return this.imagePath;
	}

	public void setImagePath(String imagePath) {
		this.imagePath = imagePath;
	}

	@OneToMany(cascade = CascadeType.ALL, fetch = FetchType.LAZY, mappedBy = "videoAlbum")
	public Set<VideoAlbumShip> getVideoAlbumShips() {
		return this.videoAlbumShips;
	}

	public void setVideoAlbumShips(Set<VideoAlbumShip> videoAlbumShips) {
		this.videoAlbumShips = videoAlbumShips;
	}

}