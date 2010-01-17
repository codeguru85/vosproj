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
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 * RoleInfo entity.
 * 
 * @author MyEclipse Persistence Tools
 */
@Entity
@Table(name = "role_info")
public class RoleInfo implements java.io.Serializable {

	// Fields

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private Integer roleId;
	private String roleCode;
	private String roleName;
	private String descn;
	private Date createTime;
 
	private GroupInfo groupInfo;
	private Set<RescInfo> rescInfos = new HashSet<RescInfo>(0);
	private Set<UserInfo> userInfos = new HashSet<UserInfo>(0);

	// Constructors

	/** default constructor */
	public RoleInfo() {
	}

	/** full constructor */
	public RoleInfo(String roleCode, String roleName, String descn,
			GroupInfo groupInfo, Set<RescInfo> rescInfos) {
		this.roleCode = roleCode;
		this.roleName = roleName;
		this.descn = descn;
		this.groupInfo = groupInfo;
		this.rescInfos = rescInfos;
	}

	@Column(name = "DESCN", nullable = false, length = 255)
	public String getDescn() {
		return this.descn;
	}

	@ManyToOne(fetch = FetchType.LAZY)
	@JoinColumn(name = "GROUP_ID", nullable = false, insertable = false, updatable = false)
	public GroupInfo getGroupInfo() {
		return groupInfo;
	}

	@ManyToMany(fetch = FetchType.LAZY)
	@JoinTable(name = "role_resc", joinColumns = { @JoinColumn(name = "ROLE_ID", nullable = false, updatable = false) }, inverseJoinColumns = { @JoinColumn(name = "RESC_ID", nullable = false, updatable = false) })
	public Set<RescInfo> getRescInfos() {
		return rescInfos;
	}

	@Column(name = "ROLE_CODE", nullable = false, length = 45)
	public String getRoleCode() {
		return this.roleCode;
	}

	// Property accessors
	@Id
	@GeneratedValue(strategy = IDENTITY)
	@Column(name = "ROLE_ID", unique = true, nullable = false)
	public Integer getRoleId() {
		return this.roleId;
	}

	@Column(name = "ROLE_NAME", nullable = false, length = 45)
	public String getRoleName() {
		return this.roleName;
	}

	@ManyToMany(fetch = FetchType.LAZY)
	@JoinTable(name = "user_role", joinColumns = { @JoinColumn(name = "ROLE_ID", nullable = false, updatable = false) }, inverseJoinColumns = { @JoinColumn(name = "USER_ID", nullable = false, updatable = false) })
	public Set<UserInfo> getUserInfos() {
		return userInfos;
	}

	public void setDescn(String descn) {
		this.descn = descn;
	}

	public void setGroupInfo(GroupInfo groupInfo) {
		this.groupInfo = groupInfo;
	}

	@Temporal(TemporalType.DATE)
	@Column(name = "CREATE_TIME", nullable = false)
	public Date getCreateTime() {
		return createTime;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}

	public void setRescInfos(Set<RescInfo> rescInfos) {
		this.rescInfos = rescInfos;
	}

	public void setRoleCode(String roleCode) {
		this.roleCode = roleCode;
	}

	public void setRoleId(Integer roleId) {
		this.roleId = roleId;
	}

	public void setRoleName(String roleName) {
		this.roleName = roleName;
	}

	public void setUserInfos(Set<UserInfo> userInfos) {
		this.userInfos = userInfos;
	}

}