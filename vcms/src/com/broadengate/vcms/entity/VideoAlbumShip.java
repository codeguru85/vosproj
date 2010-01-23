package com.broadengate.vcms.entity;

// default package
// Generated 2009-7-15 10:27:13 by Hibernate Tools 3.2.2.GA

import javax.persistence.AttributeOverride;
import javax.persistence.AttributeOverrides;
import javax.persistence.Column;
import javax.persistence.EmbeddedId;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;

/**
 * VideoAlbumShip generated by hbm2java
 */
@Entity
@Table(name = "video_album_ship", catalog = "vcms")
public class VideoAlbumShip implements java.io.Serializable {

	private VideoAlbumShipId id;
	private VideoInfo videoInfo;
	private VideoAlbum videoAlbum;

	public VideoAlbumShip() {
	}

	public VideoAlbumShip(VideoAlbumShipId id) {
		this.id = id;
	}

	public VideoAlbumShip(VideoAlbumShipId id, VideoInfo videoInfo,
			VideoAlbum videoAlbum) {
		this.id = id;
		this.videoInfo = videoInfo;
		this.videoAlbum = videoAlbum;
	}

	@EmbeddedId
	@AttributeOverrides( {
			@AttributeOverride(name = "albumId", column = @Column(name = "ALBUM_ID")),
			@AttributeOverride(name = "videoId", column = @Column(name = "VIDEO_ID")),
			@AttributeOverride(name = "albumSeq", column = @Column(name = "ALBUM_SEQ")),
			@AttributeOverride(name = "createTime", column = @Column(name = "CREATE_TIME", length = 0)) })
	public VideoAlbumShipId getId() {
		return this.id;
	}

	public void setId(VideoAlbumShipId id) {
		this.id = id;
	}

	@ManyToOne(fetch = FetchType.LAZY)
	@JoinColumn(name = "VIDEO_ID", insertable = false, updatable = false)
	public VideoInfo getVideoInfo() {
		return this.videoInfo;
	}

	public void setVideoInfo(VideoInfo videoInfo) {
		this.videoInfo = videoInfo;
	}

	@ManyToOne(fetch = FetchType.EAGER)
	@JoinColumn(name = "ALBUM_ID", insertable = false, updatable = false)
	public VideoAlbum getVideoAlbum() {
		return this.videoAlbum;
	}

	public void setVideoAlbum(VideoAlbum videoAlbum) {
		this.videoAlbum = videoAlbum;
	}

}