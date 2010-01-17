/**
 * 
 */
package com.broadengate.security.model;

import static javax.persistence.GenerationType.IDENTITY;

import java.io.Serializable;
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
import javax.persistence.OneToMany;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 * @author gonglei
 * 
 */
@Entity
@Table(name = "group_info")
public class GroupInfo implements Serializable {

	private static final long serialVersionUID = 1L;

	@Id
	@GeneratedValue(strategy = IDENTITY)
	@Column(name = "GROUP_ID", unique = true, nullable = false)
	private Integer groupId;

	@Column(name = "GROUP_NAME", nullable = false, length = 32)
	private String groupName;

	@Temporal(TemporalType.DATE)
	@Column(name = "CREATE_TIME", nullable = false)
	private Date createTime;

	@ManyToOne(fetch = FetchType.LAZY)
	@JoinColumn(name = "PTK_GROUP_ID", nullable = false, insertable = false, updatable = false)
	private GroupInfo groupInfo;

	@ManyToMany(fetch = FetchType.EAGER)
	@JoinTable(name = "user_group", joinColumns = { @JoinColumn(name = "GROUP_ID", nullable = false, updatable = false) }, inverseJoinColumns = { @JoinColumn(name = "USER_ID", nullable = false, updatable = false) })
	private Set<UserInfo> userInfos = new HashSet<UserInfo>(0);

	@OneToMany(fetch = FetchType.EAGER, mappedBy = "groupInfo")
	private Set<GroupInfo> groupInfos = new HashSet<GroupInfo>(0);

	public Date getCreateTime() {
		return createTime;
	}

	public Integer getGroupId() {
		return groupId;
	}

	public GroupInfo getGroupInfo() {
		return groupInfo;
	}

	public Set<GroupInfo> getGroupInfos() {
		return groupInfos;
	}

	public String getGroupName() {
		return groupName;
	}

	public Set<UserInfo> getUserInfos() {
		return userInfos;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}

	public void setGroupId(Integer groupId) {
		this.groupId = groupId;
	}

	public void setGroupInfo(GroupInfo groupInfo) {
		this.groupInfo = groupInfo;
	}

	public void setGroupInfos(Set<GroupInfo> groupInfos) {
		this.groupInfos = groupInfos;
	}

	public void setGroupName(String groupName) {
		this.groupName = groupName;
	}

	public void setUserInfos(Set<UserInfo> userInfos) {
		this.userInfos = userInfos;
	}

}
