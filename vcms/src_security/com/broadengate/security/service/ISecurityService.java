/**
 * 
 */
package com.broadengate.security.service;

import java.util.List;

import com.broadengate.core.commons.pagination.IPagination;
import com.broadengate.security.model.GroupInfo;
import com.broadengate.security.model.RescInfo;
import com.broadengate.security.model.RoleInfo;
import com.broadengate.security.model.UserInfo;
import com.broadengate.security.resourcedetails.ResourceDetails;

/**
 * @author 龚磊
 * 
 */
public interface ISecurityService {

	public List<UserInfo> getUsers();

	public String getGroupTreeXML();

	/**
	 * 初始化userCache
	 */
	public void initUserCache();

	/**
	 * 初始化resourceCache
	 */
	public void initResourceCache();

	/**
	 * 刷新resourceCache
	 */
	public void refreshResourceCache();

	/**
	 * 获取所有的url资源
	 */
	public List<String> getUrlResStrings();

	/**
	 * 获取所有的Funtion资源
	 */
	public List<String> getFunctions();

	/**
	 * 获取所有的Component资源
	 */
	public List<String> getComponents();

	/**
	 * 获取所有的menu资源
	 * 
	 * @return
	 */
	public List<String> getMenus();

	/**
	 * 根据资源串获取资源�
	 */
	public ResourceDetails getAuthorityFromCache(String resString);

	/**
	 * 按条件分页查询用户
	 * 
	 * @param pagination
	 * @return
	 */
	public IPagination findPagedUsers(IPagination pagination);

	/**
	 * 按条件分页查询资源
	 * 
	 * @param pagination
	 * @return
	 */
	public IPagination findPagedResources(IPagination pagination);

	/**
	 * 按条件分页查询角色
	 * 
	 * @param pagination
	 * @return
	 */
	public IPagination findPagedRoles(IPagination pagination);

	/**
	 * 根据实际名字查找用户
	 * 
	 * @param userInfo
	 * @return
	 */
	public UserInfo getUserByRealName(String realName);

	/**
	 * 新增一个用户
	 * 
	 * @param userInfo
	 * @return
	 */
	public UserInfo addUser(UserInfo userInfo, Integer[] roleIds);

	/**
	 * 根据用户名查唯一用户
	 * 
	 * @param userName
	 * @return
	 */
	public UserInfo getUserByName(String userName);

	/**
	 * 新增一个资源
	 * 
	 * @param resource
	 * @return
	 */
	public RescInfo addResource(RescInfo resource, Integer[] roleIds);

	/**
	 * 根据ID查询一个资源记录
	 * 
	 * @param resourceId
	 * @return
	 */
	public RescInfo getResourceById(Integer resourceId);

	/**
	 * 修改一个资源
	 * 
	 * @param resource
	 * @return
	 */
	public RescInfo updateResource(RescInfo resource, Integer[] roleIds);

	/**
	 * 新增一个角色
	 * 
	 * @param role
	 * @return
	 */
	public RoleInfo addRole(RoleInfo role, Integer[] userIds,
			Integer[] resourceIds);

	/**
	 * 修改一个角色
	 * 
	 * @param role
	 * @return
	 */
	public RoleInfo updateRole(RoleInfo role, Integer[] userIds,
			Integer[] resourceIds);

	/**
	 * 根据roleCode查询是否有同code的role
	 * 
	 * @param roleCode
	 * @return
	 */
	public RoleInfo getRoleByCode(String roleCode);

	/**
	 * 根据roleName查询是否有同name的role
	 * 
	 * @param roleName
	 * @return
	 */
	public RoleInfo getRoleByName(String roleName);

	/**
	 * 删除一个资源
	 * 
	 * @param resourceId
	 * @return
	 */
	public RescInfo removeResource(Integer resourceId);

	/**
	 * 根据resourceCode查询唯一的resource，用于判断是否有code重复
	 * 
	 * @param resourceCode
	 * @return
	 */
	public RescInfo getResourceByCode(String resourceCode);

	/**
	 * 根据resourceName查询唯一的resource，用于判断是否有Name重复
	 * 
	 * @param resourceName
	 * @return
	 */
	public RescInfo getResourceByName(String resourceName);

	/**
	 * 查询一个角色，并将相关联的用户和资源也同时查出
	 * 
	 * @param roleId
	 * @return
	 */
	public RoleInfo getRoleById(Integer roleId);

	/**
	 * 根据userId查询一个用户信息,并且将相关联的角色列表查出
	 * 
	 * @param userId
	 * @return
	 */
	public UserInfo getUserById(Integer userId);

	/**
	 * 修改一个用户信息
	 * 
	 * @param user
	 * @param roleIds
	 * @return
	 */
	public UserInfo updateUser(UserInfo user, Integer[] roleIds);

	/**
	 * 删除一个用户,并将相关联的角色也删除
	 * 
	 * @param userId
	 * @return
	 */
	public UserInfo removeUser(Integer userId);

	/**
	 * 删除一个角色
	 * 
	 * @param roleId
	 * @return
	 */
	public RoleInfo removeRole(Integer roleId);

	/**
	 * 根据类型查询所有的资源
	 * 
	 * @param resourceType
	 * @return
	 */
	public List<RescInfo> getResourcesByType(String resourceType);

	/**
	 * 根据资源code，更新一个角色的资源权限
	 * 
	 * @param roleId
	 * @param resourceCodes
	 * @return
	 */
	public RoleInfo updateRoleMenu(Integer roleId, String[] resourceCodes);

	/**
	 * 获得登陆用户信息
	 * 
	 * @return
	 */
	public UserInfo getLoginUser();

	public List<RescInfo> getResourcesByType(String resourceType, Integer roleId);

	public List<RescInfo> getResourcesByType(String menuType,
			String controlType, Integer roleId);

	/**
	 * 根据角色代码和资源类型找到具有这些角色代码具有的资源
	 * 
	 * @param resourceType
	 * @param roleCodes
	 * @return
	 */
	public List<RescInfo> getResourcesByRoles(String resourceType,
			String roleCodes);

	/**
	 * 修改密码
	 * 
	 */
	public UserInfo updatePassWords(String newPassWords, Integer userPasswordId);

}
