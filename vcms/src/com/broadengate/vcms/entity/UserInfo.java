package com.broadengate.vcms.entity;

// default package
// Generated 2009-7-15 10:27:13 by Hibernate Tools 3.2.2.GA

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

/**
 * UserInfo generated by hbm2java
 */
@Entity
@Table(name = "user_info", catalog = "vcms")
public class UserInfo implements java.io.Serializable {

	private Integer userId;
	private String userName;
	private String password;
	private Boolean sex;
	private Set<VideoInfo> videoInfos = new HashSet<VideoInfo>(0);

	public UserInfo() {
	}

	public UserInfo(String userName, String password, Boolean sex,
			Set<VideoInfo> videoInfos) {
		this.userName = userName;
		this.password = password;
		this.sex = sex;
		this.videoInfos = videoInfos;
	}

	@Id
	@GeneratedValue(strategy = IDENTITY)
	@Column(name = "USER_ID", unique = true, nullable = false)
	public Integer getUserId() {
		return this.userId;
	}

	public void setUserId(Integer userId) {
		this.userId = userId;
	}

	@Column(name = "USER_NAME", length = 32)
	public String getUserName() {
		return this.userName;
	}

	public void setUserName(String userName) {
		this.userName = userName;
	}

	@Column(name = "PASSWORD", length = 128)
	public String getPassword() {
		return this.password;
	}

	public void setPassword(String password) {
		this.password = password;
	}

	@Column(name = "SEX")
	public Boolean getSex() {
		return this.sex;
	}

	public void setSex(Boolean sex) {
		this.sex = sex;
	}

	@OneToMany(cascade = CascadeType.ALL, fetch = FetchType.LAZY, mappedBy = "userInfo")
	public Set<VideoInfo> getVideoInfos() {
		return this.videoInfos;
	}

	public void setVideoInfos(Set<VideoInfo> videoInfos) {
		this.videoInfos = videoInfos;
	}

}
