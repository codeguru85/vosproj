package com.broadengate.security.model;

import static javax.persistence.GenerationType.IDENTITY;

import java.util.Date;
import java.util.HashSet;
import java.util.Set;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.GeneratedValue;
import javax.persistence.Id;
import javax.persistence.ManyToMany;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 * UserInfo entity.
 * 
 * @author MyEclipse Persistence Tools
 */

@Entity
@Table(name = "user_info")
public class UserInfo implements java.io.Serializable {

	// Fields

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private Integer userId;
	private String userName;
	private String password;
	private String realName;
	private Date createTime;
	private Boolean enable;
	private String remark;
	private String address;
	private String mobile;
	private String telephone;
	private String zipCode;
	private String state;
	private String city;
	private String email;
	private Set<GroupInfo> groupInfos = new HashSet<GroupInfo>(0);
	private Set<RoleInfo> roleInfos = new HashSet<RoleInfo>(0);

	// Constructors

	/** default constructor */
	public UserInfo() {
	}

	public String getAddress() {
		return address;
	}

	public String getCity() {
		return city;
	}

	@Temporal(TemporalType.DATE)
	@Column(name = "CREATE_TIME", nullable = false)
	public Date getCreateTime() {
		return createTime;
	}

	@Column(name = "EMAIL", nullable = false, length = 64)
	public String getEmail() {
		return email;
	}

	@Column(name = "ENABLE", nullable = false)
	public Boolean getEnable() {
		return this.enable;
	}

	@ManyToMany(fetch = FetchType.EAGER, mappedBy = "userInfos")
	public Set<GroupInfo> getGroupInfos() {
		return groupInfos;
	}

	@Column(name = "MOBILE", nullable = false, length = 45)
	public String getMobile() {
		return mobile;
	}

	@Column(name = "PASSWORD", nullable = false, length = 128)
	public String getPassword() {
		return this.password;
	}

	@Column(name = "REAL_NAME", nullable = false, length = 32)
	public String getRealName() {
		return realName;
	}

	@Column(name = "REMARK", nullable = false, length = 255)
	public String getRemark() {
		return remark;
	}

	@ManyToMany(fetch = FetchType.EAGER, mappedBy = "userInfos")
	public Set<RoleInfo> getRoleInfos() {
		return roleInfos;
	}

	@Column(name = "STATE", nullable = false, length = 45)
	public String getState() {
		return state;
	}

	@Column(name = "TELEPHONE", nullable = false, length = 45)
	public String getTelephone() {
		return telephone;
	}

	// Property accessors
	@Id
	@GeneratedValue(strategy = IDENTITY)
	@Column(name = "USER_ID", unique = true, nullable = false)
	public Integer getUserId() {
		return this.userId;
	}

	@Column(name = "USER_NAME", nullable = false, length = 32)
	public String getUserName() {
		return this.userName;
	}

	@Column(name = "ZIP_CODE", nullable = false, length = 45)
	public String getZipCode() {
		return zipCode;
	}

	@Column(name = "ADDRESS", nullable = false, length = 255)
	public void setAddress(String address) {
		this.address = address;
	}

	@Column(name = "CITY", nullable = false, length = 45)
	public void setCity(String city) {
		this.city = city;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}

	public void setEmail(String email) {
		this.email = email;
	}

	public void setEnable(Boolean enable) {
		this.enable = enable;
	}

	public void setGroupInfos(Set<GroupInfo> groupInfos) {
		this.groupInfos = groupInfos;
	}

	public void setMobile(String mobile) {
		this.mobile = mobile;
	}

	public void setPassword(String password) {
		this.password = password;
	}

	public void setRealName(String realName) {
		this.realName = realName;
	}

	public void setRemark(String remark) {
		this.remark = remark;
	}

	public void setRoleInfos(Set<RoleInfo> roleInfos) {
		this.roleInfos = roleInfos;
	}

	public void setState(String state) {
		this.state = state;
	}

	public void setTelephone(String telephone) {
		this.telephone = telephone;
	}

	public void setUserId(Integer userId) {
		this.userId = userId;
	}

	public void setUserName(String userName) {
		this.userName = userName;
	}

	public void setZipCode(String zipCode) {
		this.zipCode = zipCode;
	}

}