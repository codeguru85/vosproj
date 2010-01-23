/**
 * 
 */
package com.broadengate.security.web;

import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.hibernate.criterion.DetachedCriteria;
import org.hibernate.criterion.Restrictions;

import com.broadengate.core.commons.pagination.HibernateCriteriaPagination;
import com.broadengate.core.commons.pagination.IPagination;
import com.broadengate.core.commons.pagination.Query;
import com.broadengate.security.model.RoleInfo;
import com.broadengate.security.service.ISecurityService;
import com.opensymphony.xwork2.ActionContext;
import com.opensymphony.xwork2.ActionSupport;
import com.opensymphony.xwork2.validator.annotations.RequiredStringValidator;
import com.opensymphony.xwork2.validator.annotations.StringLengthFieldValidator;
import com.opensymphony.xwork2.validator.annotations.Validation;
import com.opensymphony.xwork2.validator.annotations.Validations;

/**
 * @author 龚磊
 * 
 */
@Validation
public class RoleAction extends ActionSupport {

	/**
	 * 
	 */
	private static final long serialVersionUID = 3244468058436440496L;

	private ISecurityService securityService;

	private RoleInfo role;

	private Integer roleId;

	private Integer[] userId;

	private Integer[] resourceId;

	private Query query = new Query();

	private IPagination pagination;

	@Override
	public String execute() throws Exception {
		// 获得当前的httpsession
		Map session = ActionContext.getContext().getSession();
		// 如果查询条件为空,则尝试从会话中取出查询条件
		if (role == null) {
			role = (RoleInfo) session.get("queryRole");
			query = role != null ? (Query) session.get("roleQuery") : query;
		}
		DetachedCriteria criteria = DetachedCriteria.forClass(RoleInfo.class);

		if (role != null && StringUtils.isNotEmpty(role.getRoleName())) {
			criteria.add(Restrictions.like("roleName", "%" + role.getRoleName().trim() + "%").ignoreCase());
		}
		if (role != null && StringUtils.isNotEmpty(role.getRoleCode())) {
			criteria.add(Restrictions.like("roleCode", "%" + role.getRoleCode().trim() + "%"));
		}

		query.makeOrder(criteria, "roleId", true);
		pagination = new HibernateCriteriaPagination(criteria, query.getPage(), query.getPageSize());
		pagination = securityService.findPagedResources(pagination);
		// 将当前的查询状态存入会话中
		session.put("queryRole", role);
		session.put("roleQuery", query);
		return SUCCESS;
	}

	/**
	 * 保存一个角色信息，并同时保存相关联的用户和这个角色所能访问的资源权限
	 * 
	 * @return
	 * @throws Exception
	 */
	@Validations(requiredStrings = { @RequiredStringValidator(fieldName = "role.roleName", key = "security.role.roleName.required", message = ""), }, stringLengthFields = { @StringLengthFieldValidator(fieldName = "role.roleName", minLength = "5", maxLength = "10", key = "security.role.roleName.stringlength", message = ""), })
	public String saveRole() throws Exception {
		role.setRoleName(role.getRoleName().trim());
		RoleInfo repeatRole = securityService.getRoleByCode(role.getRoleCode());
		RoleInfo repeatRoleName = securityService.getRoleByName(role.getRoleName());
		if (repeatRole != null) {
			addFieldError("role.roleCode", getText("security.role.roleCode.repeat"));
			return INPUT;
		} else if (repeatRoleName != null) {
			addFieldError("role.roleName", getText("security.role.roleName.repeat"));
			return INPUT;
		} else {
			role = securityService.addRole(role, userId, resourceId);
			return SUCCESS;
		}
	}

	public String toUpdateRole() throws Exception {
		role = securityService.getRoleById(roleId);
		return SUCCESS;
	}

	/**
	 * 查看一个角色的信息
	 * 
	 * @return
	 * @throws Exception
	 */
	public String viewRole() throws Exception {
		role = securityService.getRoleById(roleId);
		return SUCCESS;
	}

	@Validations(requiredStrings = { @RequiredStringValidator(fieldName = "role.roleName", key = "security.role.roleName.required", message = ""), }, stringLengthFields = { @StringLengthFieldValidator(fieldName = "role.roleName", minLength = "5", maxLength = "10", key = "security.role.roleName.stringlength", message = ""), })
	public String updateRole() throws Exception {
		role.setRoleName(role.getRoleName().trim());
		RoleInfo repeatRoleName = securityService.getRoleByName(role.getRoleName());
		RoleInfo newRole = securityService.getRoleById(role.getRoleId());
		if (repeatRoleName != null && !newRole.getRoleName().equals(repeatRoleName.getRoleName())) {
			addFieldError("role.roleName", getText("security.role.roleName.repeat"));
			return INPUT;
		} else {
			role = securityService.updateRole(role, userId, resourceId);
			return SUCCESS;
		}
	}

	public String removeRole() throws Exception {
		securityService.removeRole(roleId);
		return SUCCESS;
	}

	public ISecurityService getSecurityService() {
		return securityService;
	}

	public void setSecurityService(ISecurityService securityService) {
		this.securityService = securityService;
	}

	public RoleInfo getRole() {
		return role;
	}

	public void setRole(RoleInfo role) {
		this.role = role;
	}

	public Query getQuery() {
		return query;
	}

	public void setQuery(Query query) {
		this.query = query;
	}

	public IPagination getPagination() {
		return pagination;
	}

	public void setPagination(IPagination pagination) {
		this.pagination = pagination;
	}

	public Integer[] getUserId() {
		return userId;
	}

	public void setUserId(Integer[] userId) {
		this.userId = userId;
	}

	public Integer[] getResourceId() {
		return resourceId;
	}

	public void setResourceId(Integer[] resourceId) {
		this.resourceId = resourceId;
	}

	public Integer getRoleId() {
		return roleId;
	}

	public void setRoleId(Integer roleId) {
		this.roleId = roleId;
	}

}
