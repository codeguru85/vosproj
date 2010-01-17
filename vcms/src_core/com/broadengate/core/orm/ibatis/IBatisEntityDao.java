package com.broadengate.core.orm.ibatis;

import java.io.Serializable;
import java.lang.reflect.InvocationTargetException;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.beanutils.PropertyUtils;
import org.apache.commons.lang.StringUtils;
import org.springframework.orm.ObjectRetrievalFailureException;
import org.springframework.orm.ibatis.support.SqlMapClientDaoSupport;
import org.springframework.util.Assert;

import com.broadengate.core.commons.pagination.Pagination;
import com.broadengate.core.commons.reflect.GenericsUtils;
import com.broadengate.core.orm.EntityDao;

/**
 * 负责为单个Entity 提供CRUD操作的IBatis DAO基类. <p/> 子类只要在类定义时指定所管理Entity的Class,
 * 即拥有对单个Entity对象的CRUD操作.
 * 
 * 
 * @author 龚磊
 */
public abstract class IBatisEntityDao<T> extends SqlMapClientDaoSupport implements EntityDao<T> {

	public static final String POSTFIX_INSERT = ".insert";

	public static final String POSTFIX_UPDATE = ".update";

	public static final String POSTFIX_DELETE = ".delete";

	public static final String POSTFIX_DELETE_PRIAMARYKEY = ".deleteByPrimaryKey";

	public static final String POSTFIX_SELECT = ".select";

	public static final String POSTFIX_SELECTMAP = ".selectByMap";

	public static final String POSTFIX_SELECTSQL = ".selectBySql";

	public static final String POSTFIX_COUNT = ".count";

	/**
	 * DAO所管理的Entity类型.
	 */
	protected Class<T> entityClass;

	protected String primaryKeyName;

	/**
	 * 在构造函数中将泛型T.class赋给entityClass.
	 */
	@SuppressWarnings("unchecked")
	public IBatisEntityDao() {
		entityClass = GenericsUtils.getSuperClassGenricType(getClass());
	}

	/**
	 * 根据属性名和属性值查询对象.
	 * 
	 * @return 符合条件的对象列表
	 */
	public List<T> findBy(String name, Object value) {
		Assert.hasText(name);
		Map map = new HashMap();
		map.put(name, value);
		return find(map);
	}

	/**
	 * 根据ID获取对象.
	 */
	public T getById(Serializable id) {
		T o = (T) getSqlMapClientTemplate().queryForObject(entityClass.getName() + POSTFIX_SELECT, id);
		if (o == null)
			throw new ObjectRetrievalFailureException(entityClass, id);
		return o;
	}

	/**
	 * 获取全部对象.
	 */
	public List<T> getAll() {
		return getSqlMapClientTemplate().queryForList(entityClass.getName() + POSTFIX_SELECT, null);
	}

	/**
	 * 取得entityClass. <p/> JDK1.4不支持泛型的子类可以抛开Class<T> entityClass,重载此函数达到相同效果。
	 */
	protected Class<T> getEntityClass() {
		return entityClass;
	}

	public String getIdName(Class clazz) {
		return "id";
	}

	public String getPrimaryKeyName() {
		if (StringUtils.isEmpty(primaryKeyName))
			primaryKeyName = "id";
		return primaryKeyName;
	}

	protected Object getPrimaryKeyValue(Object o) throws NoSuchMethodException, IllegalAccessException,
			InvocationTargetException, InstantiationException {
		// 2007-3-29
		// fix PropertyUtils.getProperty(entityClass.newInstance(),
		// getPrimaryKeyName()) always is null
		return PropertyUtils.getProperty(entityClass.cast(o), getPrimaryKeyName());
	}

	/**
	 * 保存对象. 为了实现IEntityDao 我在内部使用了insert和upate 2个方法.
	 */
	public T save(T o) {
		Object primaryKey;
		try {
			primaryKey = getPrimaryKeyValue(o);
		} catch (Exception e) {
			throw new ObjectRetrievalFailureException(entityClass, e);
		}

		if (primaryKey == null)
			insert(o);
		else
			update(o);
		return o;
	}

	public void setPrimaryKeyName(String primaryKeyName) {
		this.primaryKeyName = primaryKeyName;
	}

	/**
	 * 新增对象
	 */
	public void insert(Object o) {
		getSqlMapClientTemplate().insert(o.getClass().getName() + POSTFIX_INSERT, o);
	}

	/**
	 * 保存对象
	 */
	public void update(Object o) {
		getSqlMapClientTemplate().update(o.getClass().getName() + POSTFIX_UPDATE, o);
	}

	/**
	 * 删除对象
	 */
	public T remove(T o) {
		getSqlMapClientTemplate().delete(o.getClass().getName() + POSTFIX_DELETE, o);
		return o;
	}

	/**
	 * 根据ID删除对象
	 */
	public T removeById(Serializable id) {
		getSqlMapClientTemplate().delete(entityClass.getName() + POSTFIX_DELETE_PRIAMARYKEY, id);
		return null;
	}

	/**
	 * map查询.
	 * 
	 * @param map
	 *            包含各种属性的查询
	 */
	public List<T> find(Map map) {
		if (map == null)
			return this.getSqlMapClientTemplate().queryForList(entityClass.getName() + POSTFIX_SELECT, null);
		else {
			map.put("findBy", "True");
			return this.getSqlMapClientTemplate()
					.queryForList(entityClass.getName() + POSTFIX_SELECTMAP, map);
		}
	}

	/**
	 * sql 查询.
	 * 
	 * @param sql
	 *            直接sql的语句(需要防止注入式攻击)
	 */
	public List<T> find(String sql) {
		Assert.hasText(sql);
		if (StringUtils.isEmpty(sql))
			return this.getSqlMapClientTemplate().queryForList(entityClass.getName() + POSTFIX_SELECT, null);
		else
			return this.getSqlMapClientTemplate()
					.queryForList(entityClass.getName() + POSTFIX_SELECTSQL, sql);
	}

	/**
	 * 根据属性名和属性值查询对象.
	 * 
	 * @return 符合条件的唯一对象
	 */
	public T findUniqueBy(String name, Object value) {
		Assert.hasText(name);
		Map map = new HashMap();
		try {
			PropertyUtils.getProperty(entityClass.newInstance(), name);
			map.put(name, value);
			map.put("findUniqueBy", "True");
			return (T) getSqlMapClientTemplate().queryForObject(entityClass.getName() + POSTFIX_SELECTMAP,
					map);
		} catch (Exception e) {
			logger.error("Error when propertie on entity," + e.getMessage(), e.getCause());
			return null;
		}

	}

	/**
	 * 根据属性名和属性值以Like AnyWhere方式查询对象.
	 * 
	 * @param name
	 * @param value
	 * @return
	 */
	public List<T> findByLike(String name, String value) {
		Assert.hasText(name);
		Map map = new HashMap();
		map.put(name, value);
		map.put("findLikeBy", "True");
		return getSqlMapClientTemplate().queryForList(entityClass.getName() + POSTFIX_SELECTMAP, map);

	}

	/**
	 * 判断对象某些属性的值在数据库中不存在重复
	 * 
	 * @param tableName
	 *            数据表名字
	 * @param names
	 *            在POJO里不能重复的属性列表,以逗号分割 如"name,loginid,password" FIXME how about
	 *            in different schema?
	 */
	public boolean isNotUnique(Object entity, String tableName, String names) {
		try {
			String primarykey;
			Connection con = getSqlMapClient().getCurrentConnection();
			ResultSet dbMetaData = con.getMetaData().getPrimaryKeys(con.getCatalog(), null, tableName);
			dbMetaData.next();
			if (dbMetaData.getRow() > 0) {
				primarykey = dbMetaData.getString(4);
				if (names.indexOf(primarykey) > -1)
					return false;
			} else {
				return true;
			}

		} catch (SQLException e) {
			logger.error(e.getMessage(), e);
			return false;
		}
		return false;
	}

	/**
	 * 分页查询函数，使用PaginatedList.
	 * 
	 * @param pageNo页号,从1开始.
	 * @return 含总记录数和当前页数据的Page对象.
	 */
	public Pagination pagedQuery(Object parameterObject, int pageNo, int pageSize) {

		Assert.isTrue(pageNo >= 1, "pageNo should start from 1");

		// 计算总数
		Integer totalCount = (Integer) this.getSqlMapClientTemplate().queryForObject(
				entityClass.getName() + POSTFIX_COUNT, parameterObject);

		// 如果没有数据则返回Empty Page
		Assert.notNull(totalCount, "totalCount Error");

		if (totalCount.intValue() == 0) {
			return new Pagination();
		}

		List list;
		int totalPageCount = 0;
		int startIndex = 0;

		// 如果pageSize小于0,则返回所有数据,等同于getAll
		if (pageSize > 0) {

			// 计算页数
			totalPageCount = (totalCount / pageSize);
			totalPageCount += ((totalCount % pageSize) > 0) ? 1 : 0;

			// 计算skip数量
			if (totalPageCount > pageNo) {
				startIndex = (pageNo - 1) * pageSize;
			} else {
				startIndex = (totalPageCount - 1) * pageSize;
			}

			list = getSqlMapClientTemplate().queryForList(entityClass.getName() + POSTFIX_SELECT,
					parameterObject, startIndex, pageSize);

		} else {
			list = getSqlMapClientTemplate().queryForList(entityClass.getName() + POSTFIX_SELECT,
					parameterObject);
		}
		return new Pagination(startIndex, totalCount, pageSize, list);
	}

}
