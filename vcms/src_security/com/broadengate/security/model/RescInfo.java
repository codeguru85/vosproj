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
 * Resource entity.
 * 
 * @author MyEclipse Persistence Tools
 */
@Entity
@Table(name = "resc_info")
public class RescInfo implements java.io.Serializable {

	// Fields

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	private Integer rescId;
	private String rescCode;
	private String rescName;
	private String rescType;
	private String rescString;
	private Date createTime;
	private String descn;
	private Set<RoleInfo> roleInfos = new HashSet<RoleInfo>(0);

	/** default constructor */
	public RescInfo() {
	}

	@Temporal(TemporalType.DATE)
	@Column(name = "CREATE_TIME", nullable = false)
	public Date getCreateTime() {
		return createTime;
	}

	@Column(name = "DESCN", nullable = false, length = 32)
	public String getDescn() {
		return descn;
	}

	@Column(name = "RESC_CODE", nullable = false, length = 45)
	public String getRescCode() {
		return rescCode;
	}

	@Id
	@GeneratedValue(strategy = IDENTITY)
	@Column(name = "RESC_ID", unique = true, nullable = false)
	public Integer getRescId() {
		return rescId;
	}

	@Column(name = "RESC_NAME", nullable = false, length = 128)
	public String getRescName() {
		return rescName;
	}

	@Column(name = "RESC_STRING", nullable = false, length = 255)
	public String getRescString() {
		return rescString;
	}

	@Column(name = "RESC_TYPE", nullable = false, length = 32)
	public String getRescType() {
		return rescType;
	}

	@ManyToMany(fetch = FetchType.LAZY, mappedBy = "rescInfos")
	public Set<RoleInfo> getRoleInfos() {
		return roleInfos;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}

	public void setDescn(String descn) {
		this.descn = descn;
	}

	public void setRescCode(String rescCode) {
		this.rescCode = rescCode;
	}

	public void setRescId(Integer rescId) {
		this.rescId = rescId;
	}

	public void setRescName(String rescName) {
		this.rescName = rescName;
	}

	public void setRescString(String rescString) {
		this.rescString = rescString;
	}

	public void setRescType(String rescType) {
		this.rescType = rescType;
	}

	// Constructors

	public void setRoleInfos(Set<RoleInfo> roleInfos) {
		this.roleInfos = roleInfos;
	}

}