/**
 * 
 */
package com.broadengate.security.service;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.List;
import java.util.Set;

import javax.annotation.Resource;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.springframework.security.Authentication;
import org.springframework.security.GrantedAuthority;
import org.springframework.security.GrantedAuthorityImpl;
import org.springframework.security.context.SecurityContextHolder;
import org.springframework.security.providers.dao.UserCache;
import org.springframework.security.userdetails.UserDetails;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import com.broadengate.core.commons.pagination.IPagination;
import com.broadengate.security.dao.GroupDao;
import com.broadengate.security.dao.RescDao;
import com.broadengate.security.dao.RoleDao;
import com.broadengate.security.dao.UserDao;
import com.broadengate.security.model.GroupInfo;
import com.broadengate.security.model.RescInfo;
import com.broadengate.security.model.RoleInfo;
import com.broadengate.security.model.UserInfo;
import com.broadengate.security.resourcedetails.ResourceCache;
import com.broadengate.security.resourcedetails.ResourceDetails;

/**
 * @author 龚磊
 * 
 */
@Service
@Transactional
public class SecurityService implements ISecurityService {
	/**
	 * Logger for this class
	 */
	private static final Logger logger = Logger
			.getLogger(SecurityService.class);

	/**
	 * 资源权限缓存
	 */
	private ResourceCache resourceCache;

	/**
	 * 用户缓存
	 */
	private UserCache userCache;

	@Resource
	private RoleDao roleDao;

	@Resource
	private UserDao userDao;

	@Resource
	private RescDao rescDao;

	@Resource
	private GroupDao groupDao;

	private boolean cacheInitialized = false;

	public List<UserInfo> getUsers() {
		List<UserInfo> users = userDao.getAll();
		return users;
	}

	public String getGroupTreeXML() {
		GroupInfo group = groupDao.getById(1);
		Document document = DocumentHelper.createDocument();
		group.getGroupInfos().remove(group);
		buildTree(document, null, group);
		return document.asXML();
	}

	private void buildTree(Document document, Element parentEle, GroupInfo group) {
		Element ele = null;
		if (document != null) {
			ele = document.addElement("node").addAttribute("label",
					group.getGroupName());
		} else {
			ele = parentEle.addElement("node").addAttribute("label",
					group.getGroupName());
		}
		if(group.getGroupInfos().size()>0){
			for (GroupInfo g : group.getGroupInfos()) {
				buildTree(null, ele, g);
			}
		}
	}

	public IPagination findPagedUsers(IPagination pagination) {
		return userDao.pagedByCriteria(pagination);
	}

	public IPagination findPagedResources(IPagination pagination) {
		return rescDao.pagedByCriteria(pagination);
	}

	public IPagination findPagedRoles(IPagination pagination) {
		return roleDao.pagedByCriteria(pagination);
	}

	public UserInfo addUser(UserInfo userInfo, Integer[] roleIds) {
		userInfo.setCreateTime(new Date());
		userInfo = userDao.save(userInfo);
		// if (roleIds != null && roleIds.length > 0) {
		// for (Integer roleId : roleIds) {
		// RoleInfo role = roleDao.getById(roleId);
		// role.getUserInfos().add(userInfo);
		// userInfo.getRoleInfos().add(role);
		// }
		// }
		authRoleInCache(userInfo);
		return userInfo;
	}

	public UserInfo getUserByName(String userName) {
		return userDao.findUniqueBy("userName", userName);
	}

	public RoleInfo getRoleByCode(String roleCode) {
		return roleDao.findUniqueBy("roleCode", roleCode);
	}

	public RoleInfo getRoleByName(String roleName) {
		return roleDao.findUniqueBy("roleName", roleName);
	}

	/**
	 * 根据实际名字查找用户
	 * 
	 * @param userInfo
	 * @return
	 */
	public UserInfo getUserByRealName(String realName) {
		return userDao.findUniqueBy("realName", realName);
	}

	public UserInfo updateUser(UserInfo user, Integer[] roleIds) {
		UserInfo userInfo = userDao.getById(user.getUserId());
		// 更新资源基本信息
		userInfo.setEnable(user.getEnable());
		userInfo.setPassword(user.getPassword());
		userInfo.setUserName(user.getUserName());
		userInfo.setRealName(user.getRealName());
		userInfo.setRemark(user.getRemark());

		// 删除资源以前关联的角色,资源为被控方
		// Set<RoleInfo> roles = userInfo.getRoleInfos();
		// for (RoleInfo role : roles) {
		// role.getUserInfos().remove(userInfo);
		// }
		// roles.clear();
		// // 插入新的资源角色关联
		// if (roleIds != null && roleIds.length > 0) {
		// for (Integer roleId : roleIds) {
		// RoleInfo role = roleDao.getById(roleId);
		// role.getUserInfos().add(userInfo);
		// roles.add(role);
		// }
		// }
		modifyUserInCache(userInfo, userInfo.getUserName());
		return userInfo;
	}

	public RescInfo addResource(RescInfo resource, Integer[] roleIds) {
		// resource.setResourceCode(resource.getResourceCode().toUpperCase());
		// resource = rescDao.save(resource);
		// if (roleIds != null && roleIds.length > 0) {
		// for (Integer roleId : roleIds) {
		// RoleInfo role = roleDao.getById(roleId);
		// role.getRescInfos().add(resource);
		// resource.getRoleInfos().add(role);
		// }
		// }
		// // 更新缓存
		// modifyResourceInCache(resource, resource.getResourceString());
		return resource;
	}

	public RescInfo updateResource(RescInfo resource, Integer[] roleIds) {
		// RescInfo resourceInfo = rescDao.getById(resource.getResourceId());
		// // 更新资源基本信息
		// resourceInfo.setResourceCode(resource.getResourceCode().toUpperCase());
		// resourceInfo.setResourceName(resource.getResourceName());
		// resourceInfo.setResourceString(resource.getResourceString());
		// resourceInfo.setResourceType(resource.getResourceType());
		// resourceInfo.setDescn(resource.getDescn());
		// 删除资源以前关联的角色,资源为被控方
		// Set<RoleInfo> roles = resourceInfo.getRoleInfos();
		// for (RoleInfo role : roles) {
		// role.getRescInfos().remove(resourceInfo);
		// }
		// roles.clear();
		// // 插入新的资源角色关联
		// if (roleIds != null && roleIds.length > 0) {
		// for (Integer roleId : roleIds) {
		// RoleInfo role = roleDao.getById(roleId);
		// role.getRescInfos().add(resourceInfo);
		// roles.add(role);
		// }
		// }
		// // 更新缓存
		// modifyResourceInCache(resourceInfo,
		// resourceInfo.getResourceString());
		return null;
	}

	public RescInfo getResourceById(Integer resourceId) {
		return rescDao.getById(resourceId);
	}

	public RescInfo getResourceByCode(String resourceCode) {
		return rescDao.findUniqueBy("resourceCode", resourceCode);
	}

	public RescInfo getResourceByName(String resourceName) {
		return rescDao.findUniqueBy("resourceName", resourceName);
	}

	public UserInfo getUserById(Integer userId) {
		return userDao.getById(userId);
	}

	/**
	 * 新增一个角色，同时添加这个角色所包含的用户和资源，角色为主控方
	 */
	public RoleInfo addRole(RoleInfo role, Integer[] userIds,
			Integer[] resourceIds) {
		if (userIds != null && userIds.length > 0) {
			// Set<UserInfo> users = role.getUserInfos();
			// for (Integer userId : userIds) {
			// users.add(userDao.getById(userId));
			// }
		}
		if (resourceIds != null && resourceIds.length > 0) {
			Set<RescInfo> resources = role.getRescInfos();
			for (Integer resourceId : resourceIds) {
				resources.add(rescDao.getById(resourceId));
			}
		}
		role = roleDao.save(role);
		// 角色code根据数据库主键生成
		role.setRoleCode("ROLE_" + role.getRoleId().toString());
		authResourceInCache(role);
		return role;
	}

	public RoleInfo getRoleById(Integer roleId) {
		return roleDao.getById(roleId);
	}

	public RescInfo removeResource(Integer resourceId) {
		RescInfo resource = rescDao.getById(resourceId);
		// 删除资源以前关联的角色,资源为被控方
		Set<RoleInfo> roles = resource.getRoleInfos();
		for (RoleInfo role : roles) {
			role.getRescInfos().remove(resource);
		}
		return rescDao.remove(resource);
	}

	public UserInfo removeUser(Integer userId) {
		UserInfo user = userDao.getById(userId);
		// 删除资源以前关联的角色,资源为被控方
		// Set<RoleInfo> roles = user.getRoleInfos();
		// for (RoleInfo role : roles) {
		// role.getUserInfos().remove(user);
		// }
		return userDao.remove(user);
	}

	public RoleInfo updateRole(RoleInfo role, Integer[] userIds,
			Integer[] resourceIds) {
		RoleInfo roleInfo = roleDao.getById(role.getRoleId());
		roleInfo.setRoleName(role.getRoleName());
		roleInfo.setDescn(role.getDescn());
		if (userIds != null && userIds.length > 0) {
			// Set<UserInfo> users = roleInfo.getUserInfos();
			// // 清空以前的角色用户关系,数据库将自动删除
			// users.clear();
			// for (Integer userId : userIds) {
			// users.add(userDao.getById(userId));
			// }
		}
		if (resourceIds != null && resourceIds.length > 0) {
			Set<RescInfo> resources = roleInfo.getRescInfos();
			// 清空以前的角色资源关系,数据库将自动删除
			// 按资源类型清空
			List<RescInfo> unMenuResources = new ArrayList<RescInfo>();
			// for (RescInfo resource : resources) {
			// if (!resource.getResourceType().equals(Constants.RESOURCE_MENU))
			// {
			// resource.getRoleInfos().remove(roleInfo);
			// modifyResourceInCache(resource, resource.getResourceCode());
			// unMenuResources.add(resource);
			// }
			// }
			resources.removeAll(unMenuResources);
			// 给角色授予新的资源权限
			for (Integer resourceId : resourceIds) {
				RescInfo resource = rescDao.getById(resourceId);
				resource.getRoleInfos().add(roleInfo);
				resources.add(resource);
			}
		}
		modifyRoleInCache(roleInfo, roleInfo.getRoleCode());
		return roleInfo;
	}

	public RoleInfo updateRoleMenu(Integer roleId, String[] resourceCodes) {
		RoleInfo role = roleDao.getById(roleId);
		if (resourceCodes != null && resourceCodes.length > 0) {
			Set<RescInfo> resources = role.getRescInfos();
			// 清空以前的角色资源关系,数据库将自动删除
			// 清空以前的角色资源关系,数据库将自动删除
			// 按资源类型清空,如果是menu就清空
			List<RescInfo> menuResources = new ArrayList<RescInfo>();
			// for (RescInfo resource : resources) {
			// if (resource.getResourceType().equals(RescType.MENU.toString())
			// || resource.getResourceType().equals(
			// RescType.COMPONENT.toString())) {
			// resource.getRoleInfos().remove(role);
			// modifyResourceInCache(resource, resource.getResourceCode());
			// menuResources.add(resource);
			// }
			// }
			resources.removeAll(menuResources);
			for (String resourceCode : resourceCodes) {
				RescInfo resource = rescDao.findUniqueBy("resourceCode",
						resourceCode);
				if (resource != null) {
					resource.getRoleInfos().add(role);
					resources.add(resource);
				}
			}
		}
		modifyRoleInCache(role, role.getRoleCode());
		return role;
	}

	public RoleInfo removeRole(Integer roleId) {
		RoleInfo role = roleDao.getById(roleId);
		// role.getUserInfos().clear();
		role.getRescInfos().clear();
		return roleDao.remove(role);
	}

	public List<RescInfo> getResourcesByType(String resourceType) {
		return rescDao.findBy("resourceType", resourceType);
	}

	public List<RescInfo> getResourcesByType(String resourceType, Integer roleId) {
		return rescDao.findByNameQuery("queryResourcesByTypeAndRole",
				resourceType, roleId);
	}

	public List<RescInfo> getResourcesByType(String menuType,
			String controlType, Integer roleId) {
		return rescDao.findByNameQuery("queryMenuAndControlByTypeAndRole",
				menuType, controlType, roleId);
	}

	public List<RescInfo> getResourcesByRoles(String resourceType,
			String roleCodes) {
		String hql = "select distinct(resource) from ResourceInfo resource left join resource.roleInfos role where resource.resourceType=? and role.roleCode in ("
				+ roleCodes + ")";
		return rescDao.find(hql, resourceType);
	}

	/**
	 * 修改User时更改userCache
	 * 
	 * @param user
	 * @param orgUserName
	 */
	private void modifyUserInCache(UserInfo user, String orgUserName) {
		UserDetails ud = userCache.getUserFromCache(orgUserName);
		if (ud != null) {
			userCache.removeUserFromCache(orgUserName);
		}
		userDetailsInCache(user);
	}

	/**
	 * 修改Resource时更改resourceCache
	 * 
	 * @param resource
	 * @param orgResourcename
	 */
	private void modifyResourceInCache(RescInfo resource,
			String orgResourceString) {
		ResourceDetails rds = resourceCache
				.getAuthorityFromCache(orgResourceString);
		if (rds != null)
			resourceCache.removeAuthorityFromCache(orgResourceString);
		resourceDetailsInCache(resource);
	}

	/**
	 * 修改Role时同时修改userCache和resourceCache
	 * 
	 * @param role
	 * @param orgRoleCode
	 */
	private void modifyRoleInCache(RoleInfo role, String orgRoleCode) {
		// Set<UserInfo> users = role.getUserInfos();
		// for (UserInfo user : users) {
		// userDetailsInCache(user);
		// }
		Set<RescInfo> resources = role.getRescInfos();
		for (RescInfo resource : resources) {
			resourceDetailsInCache(resource);
		}
	}

	public UserInfo getLoginUser() {
		Authentication auth = SecurityContextHolder.getContext()
				.getAuthentication();
		UserDetails userDetails = (UserDetails) auth.getPrincipal();
		return userDao.findUniqueBy("userName", userDetails.getUsername());
	}

	/**
	 * 新增User授予角色时更改userCache
	 * 
	 * @param user
	 */
	private void authRoleInCache(UserInfo user) {
		userDetailsInCache(user);
	}

	/**
	 * 新增Role授予资源时更改resourceCache
	 * 
	 * @param role
	 */
	private void authResourceInCache(RoleInfo role) {
		Set<RescInfo> resources = role.getRescInfos();
		for (RescInfo resource : resources) {
			resourceDetailsInCache(resource);
		}
	}

	/**
	 * 初始化userCache
	 */
	public void initUserCache() {
		List<UserInfo> users = userDao.getAll();
		for (UserInfo user : users) {
			userDetailsInCache(user);
		}
	}

	/**
	 * 初始化resourceCache,加上同步机制, 防止同时进行缓存加载
	 */
	public void initResourceCache() {
		if (!cacheInitialized) {
			synchronized (this) {
				List<RescInfo> resources = rescDao.getAll();
				for (RescInfo resource : resources) {
					resourceDetailsInCache(resource);
				}
				cacheInitialized = true;
			}
		}
	}

	/**
	 * 刷新resource cache(不刷新也可) 见applicationContext-quartz.xml下cacheTrigger
	 */
	public void refreshResourceCache() {
		cacheInitialized = false;
		try {
			resourceCache.getCache().removeAll();
		} catch (Exception e) {
			logger.error(e.getMessage(), e);
		}
		initResourceCache();
	}

	/**
	 * 获取所有的url资源
	 */
	public List<String> getUrlResStrings() {
		return resourceCache.getUrlResStrings();
	}

	/**
	 * 获取所有的Funtion资源
	 */
	public List<String> getFunctions() {
		return resourceCache.getFunctions();
	}

	/**
	 * 获取所有的Component资源
	 */
	public List<String> getComponents() {
		return resourceCache.getComponents();
	}

	/**
	 * 获取所有菜单资源
	 * 
	 * @return
	 */
	public List<String> getMenus() {
		return resourceCache.getMenus();
	}

	/**
	 * 根据资源串获取资源
	 */
	public ResourceDetails getAuthorityFromCache(String resString) {
		return resourceCache.getAuthorityFromCache(resString);
	}

	/**
	 * 获取user相应的权限并置于usercache中
	 * 
	 * @param user
	 *            需要初始化user对象的role集合
	 */
	private void userDetailsInCache(UserInfo user) {
		// GrantedAuthority[] authorities =
		// role2authorities(user.getRoleInfos());
		// UserDetails uds = new org.springframework.security.userdetails.User(
		// user.getUserName(), user.getPassword(), user.getEnable(), true,
		// true, true, authorities);
		// userCache.putUserInCache(uds);
	}

	/**
	 * 获取resource相应的权限并置于resourcecache中
	 * 
	 * @param resource
	 *            需要初始化resource对象的role集合
	 */
	private void resourceDetailsInCache(RescInfo resource) {
		GrantedAuthority[] authorities = role2authorities(resource
				.getRoleInfos());
		// ResourceDetails rd = new
		// com.broadengate.security.resourcedetails.Resource(
		// resource.getResourceString(), resource.getResourceType(),
		// authorities);
		// resourceCache.putAuthorityInCache(rd);
	}

	/**
	 * 由role转为GrantedAuthority, 将角色信息转换成认证列表
	 * 
	 * @param roles
	 * @return
	 */
	private GrantedAuthority[] role2authorities(Collection<RoleInfo> roles) {
		GrantedAuthority[] authorities = new GrantedAuthority[roles.size()];
		int i = 0;
		for (RoleInfo role : roles) {
			authorities[i] = new GrantedAuthorityImpl(role.getRoleCode());
			i++;
		}
		return authorities;
	}

	/**
	 * 修改密码
	 */
	public UserInfo updatePassWords(String newPassWords, Integer userPasswordId) {
		UserInfo user = userDao.getById(userPasswordId);
		user.setPassword(newPassWords);
		return user;
	}

	public final void setResourceCache(ResourceCache resourceCache) {
		this.resourceCache = resourceCache;
	}

	public final void setUserCache(UserCache userCache) {
		this.userCache = userCache;
	}

}
