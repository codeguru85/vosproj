package com.broadengate.core.orm.hibernate;

import java.io.Serializable;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.annotation.PostConstruct;
import javax.annotation.Resource;

import org.apache.commons.beanutils.PropertyUtils;
import org.hibernate.Criteria;
import org.hibernate.Query;
import org.hibernate.Session;
import org.hibernate.SessionFactory;
import org.hibernate.Transaction;
import org.hibernate.criterion.CriteriaSpecification;
import org.hibernate.criterion.Criterion;
import org.hibernate.criterion.DetachedCriteria;
import org.hibernate.criterion.Order;
import org.hibernate.criterion.Projection;
import org.hibernate.criterion.Projections;
import org.hibernate.criterion.Restrictions;
import org.hibernate.impl.CriteriaImpl;
import org.hibernate.metadata.ClassMetadata;
import org.springframework.orm.hibernate3.support.HibernateDaoSupport;
import org.springframework.util.Assert;
import org.springframework.util.ReflectionUtils;

import com.broadengate.core.commons.beanutils.BeanUtils;
import com.broadengate.core.commons.pagination.HibernateCriteriaPagination;
import com.broadengate.core.commons.pagination.HibernateHQLPagination;
import com.broadengate.core.commons.pagination.IPagination;
import com.broadengate.core.commons.pagination.Pagination;
import com.broadengate.core.commons.reflect.GenericsUtils;
import com.broadengate.core.orm.EntityDao;

/**
 * 负责为单个Entity对象提供CRUD操作的Hibernate DAO基类.
 * <p/>
 * 子类只要在类定义时指定所管理Entity的Class, 即拥有对单个Entity对象的CRUD操作.
 * 
 * @author 龚磊
 * @see HibernateDaoSupport
 * @see GenericsUtils
 */
@SuppressWarnings("unchecked")
public abstract class HibernateEntityDao<T> extends HibernateDaoSupport
		implements EntityDao<T> {

	/**
	 * DAO所管理的Entity类型.
	 */
	protected Class<T> entityClass;

	private SessionFactory mySessionFacotry;

	/**
	 * 在构造函数中将泛型T.class赋给entityClass.
	 */
	public HibernateEntityDao() {
		entityClass = GenericsUtils.getSuperClassGenricType(getClass());
	}

	/**
	 * 取得entityClass.JDK1.4不支持泛型的子类可以抛开Class<T> entityClass,重载此函数达到相同效果。
	 */
	protected Class<T> getEntityClass() {
		return entityClass;
	}

	@Resource
	public void setMySessionFacotry(SessionFactory sessionFacotry) {
		this.mySessionFacotry = sessionFacotry;
	}

	@PostConstruct
	public void injectSessionFactory() {
		super.setSessionFactory(mySessionFacotry);
	}

	/**
	 * 保存对象.
	 */
	public T save(T o) {
		getHibernateTemplate().saveOrUpdate(o);
		return o;
	}

	/**
	 * 根据ID获取对象. 实际调用Hibernate的session.load()方法返回实体或其proxy对象. 如果对象不存在，抛出异常.
	 */
	public T getById(Serializable id) {
		return (T) getHibernateTemplate().get(entityClass, id);
	}
	
	public List getList(){
		Session s = mySessionFacotry.openSession() ;
		List list = s.createSQLQuery("SELECT v.file_name,p.List_ID,p.List_seq FROM video_info v,play_list_ship p where v.video_id =p.video_id").list() ;
		s.close() ;
		return  list ;
	}

	/**
	 * 获取全部对象
	 */
	public List<T> getAll() {
		return getHibernateTemplate().loadAll(entityClass);
	}

	/**
	 * 获取全部对象,带排序字段与升降序参数.
	 * 
	 * @param orderBy
	 * @param isAsc
	 * @return
	 */
	public List<T> getAll(String orderBy, boolean isAsc) {
		Assert.hasText(orderBy);
		if (isAsc)
			return getHibernateTemplate().findByCriteria(
					DetachedCriteria.forClass(entityClass).addOrder(
							Order.asc(orderBy)));
		else
			return getHibernateTemplate().findByCriteria(
					DetachedCriteria.forClass(entityClass).addOrder(
							Order.desc(orderBy)));
	}

	/**
	 * 删除对象.
	 */
	public T remove(T o) {
			getHibernateTemplate().delete(o);
		return o;
	}

	/**
	 * 移除专辑内部单个视频
	 */
	public String  delVideoAlbum(int a ,int[] t) 
	{
		try
		{
			for(int i=0;i<t.length;i++)
			{
				Session session =mySessionFacotry.openSession() ;
				Transaction   tx   =   session.beginTransaction(); 
				
				session.createSQLQuery(" DELETE FROM video_album_ship WHERE ALBUM_ID='" +
						a +
						"'  AND VIDEO_ID='" +
						t[i] +
						"' ").executeUpdate() ;
				tx.commit();
				session.close() ;
			}
			return "success";
		}
			catch(Exception e )
		{
			return "failure";
		}

	}
	
	/**
	 * 移除专辑
	 */
	public String  delAlbum(int i) {
		try{
			
			System.out.println("///////////     "+
					" DELETE FROM video_album_ship WHERE ALBUM_ID='" +
					i +
					"' ") ;
			
			Session session =mySessionFacotry.openSession() ;
			Transaction   tx   =   session.beginTransaction(); 
			
			session.createSQLQuery(" DELETE FROM video_album_ship WHERE ALBUM_ID='" +
					i +
					"' ").executeUpdate() ;
			tx.commit();
			session.close() ;
			return "success";
		}catch(Exception e )
		{
			System.out.println("MMMMMMMMMM") ;
			System.out.println(e) ;
			System.out.println(e.getStackTrace()) ;
			return "failure";
		}

	}
	
	/**
	 * 根据ID移除对象.
	 */
	public T removeById(Serializable id) {
		return remove(getById(id));
	}

	public void flush() {
		getHibernateTemplate().flush();
	}

	public void clear() {
		getHibernateTemplate().clear();
	}

	/**
	 * 取得Entity的Criteria. 可变的Restrictions条件列表,见
	 * {@link #createQuery(String,Object...)}
	 * 
	 * @param criterions
	 * @return
	 */
	public Criteria createCriteria(Criterion... criterions) {
		Criteria criteria = getSession().createCriteria(entityClass);
		for (Criterion c : criterions) {
			criteria.add(c);
		}
		return criteria;
	}

	/**
	 * 根据属性名和属性值查询对象.
	 * 
	 * @return 符合条件的对象列表
	 * @see HibernateGenericDao#findBy(Class,String,Object)
	 */
	public List<T> findBy(String propertyName, Object value) {
		Assert.hasText(propertyName);
		return createCriteria(Restrictions.eq(propertyName, value)).list();
	}

	/**
	 * 根据属性名和属性值查询对象,带排序参数.
	 * 
	 * @return 符合条件的对象列表
	 */
	public List<T> findBy(String propertyName, Object value, String orderBy,
			boolean isAsc) {
		Assert.hasText(propertyName);
		Assert.hasText(orderBy);
		return createCriteria(orderBy, isAsc,
				Restrictions.eq(propertyName, value)).list();
	}

	/**
	 * 根据属性名和属性值查询单个对象.
	 * 
	 * @return 符合条件的唯一对象 or null
	 */
	public T findUniqueBy(String propertyName, Object value) {
		Assert.hasText(propertyName);
		return (T) createCriteria(Restrictions.eq(propertyName, value))
				.uniqueResult();
	}

	/**
	 * 判断对象某些属性的值在数据库中唯一.
	 * 
	 * @param uniquePropertyNames
	 *            在POJO里不能重复的属性列表,以逗号分割 如"name,loginid,password"
	 * @see HibernateGenericDao#isUnique(Class,Object,String)
	 */
	public boolean isUnique(Object entity, String uniquePropertyNames) {
		Assert.hasText(uniquePropertyNames);
		Criteria criteria = createCriteria().setProjection(
				Projections.rowCount());
		String[] nameList = uniquePropertyNames.split(",");
		try {
			// 循环加入唯一列
			for (String name : nameList) {
				criteria.add(Restrictions.eq(name, PropertyUtils.getProperty(
						entity, name)));
			}
			// 以下代码为了如果是update的情况,排除entity自身.
			String idName = getIdName();
			// 取得entity的主键值
			Serializable id = getId(entity);

			// 如果id!=null,说明对象已存在,该操作为update,加入排除自身的判断
			if (id != null)
				criteria.add(Restrictions.not(Restrictions.eq(idName, id)));
		} catch (Exception e) {
			ReflectionUtils.handleReflectionException(e);
		}
		return (Integer) criteria.uniqueResult() == 0;
	}

	/**
	 * 消除与 Hibernate Session 的关联
	 * 
	 * @param entity
	 */
	public void evict(Object entity) {
		getHibernateTemplate().evict(entity);
	}

	public void evict(String collection, Serializable id) {
		getSessionFactory().evictCollection(collection, id);
	}

	/**
	 * 取得Entity的Criteria对象，带排序字段与升降序字段.
	 * 
	 * @param orderBy
	 * @param isAsc
	 * @param criterions
	 * @return
	 */
	public Criteria createCriteria(String orderBy, boolean isAsc,
			Criterion... criterions) {
		Assert.hasText(orderBy);
		Criteria criteria = createCriteria(criterions);
		if (isAsc)
			criteria.addOrder(Order.asc(orderBy));
		else
			criteria.addOrder(Order.desc(orderBy));

		return criteria;
	}

	/**
	 * 根据hql查询,直接使用HibernateTemplate的find函数,不推荐使用.
	 * 
	 * @param values
	 *            可变参数,见{@link #createQuery(String,Object...)}
	 */
	public List find(String hql, Object... values) {
		Assert.hasText(hql);
		return getHibernateTemplate().find(hql, values);
	}

	/**
	 * 根据外置命名查询
	 * 
	 * @param queryName
	 * @param values
	 *            参数值列表
	 * @return
	 */
	public List findByNameQuery(String queryName, Object... values) {
		Assert.hasText(queryName);
		return getHibernateTemplate().findByNamedQuery(queryName, values);
	}

	/**
	 * 创建Query对象.
	 * 对于需要first,max,fetchsize,cache,cacheRegion等诸多设置的函数,可以在返回Query后自行设置.
	 * 留意可以连续设置,如下：
	 * 
	 * <pre>
	 * dao.getQuery(hql).setMaxResult(100).setCacheable(true).list();
	 * </pre>
	 * 
	 * 调用方式如下：
	 * 
	 * <pre>
	 *        dao.createQuery(hql)
	 *        dao.createQuery(hql,arg0);
	 *        dao.createQuery(hql,arg0,arg1);
	 *        dao.createQuery(hql,new Object[arg0,arg1,arg2])
	 * </pre>
	 * 
	 * @param values
	 *            可变参数.
	 */
	public Query createQuery(String hql, Object... values) {
		Assert.hasText(hql);
		Query query = getSession().createQuery(hql);
		for (int i = 0; i < values.length; i++) {
			query.setParameter(i, values[i]);
		}
		return query;
	}

	/**
	 * 按hibernate标准查询器进行分页查询
	 * 
	 * @param pagination
	 * @return
	 */
	public IPagination pagedByCriteria(IPagination pagination) {
		// 断言此分页对象是否为HibernateCriteriaPagination的实例
		Assert.isInstanceOf(HibernateCriteriaPagination.class, pagination);
		Criteria criteria = ((HibernateCriteriaPagination) pagination)
				.getCriteria().getExecutableCriteria(this.getSession());
		Integer totalRows = (Integer) criteria.setProjection(
				Projections.rowCount()).uniqueResult();
		criteria.setProjection(null);
		pagination.setTotalRows(totalRows.intValue());
		List<T> list = criteria.setFirstResult(
				(pagination.getToPage() - 1) * pagination.getPageSize())
				.setMaxResults(pagination.getPageSize()).setResultTransformer(
						Criteria.DISTINCT_ROOT_ENTITY).list();
		pagination.setData(list);
		return pagination;
	}

	/**
	 * 分页实体,按分页对象类型调用相应的分页实现
	 * 
	 * @param pagination
	 * @return
	 */
	public IPagination pagedEntity(IPagination pagination) {
		if (pagination instanceof HibernateCriteriaPagination) {
			return pagedByCriteria(pagination);
		} else if (pagination instanceof HibernateHQLPagination) {
			return pagedByHQL(pagination);
		} else {
			return null;
		}
	}

	/**
	 * 按HQL方式进行分页查询
	 * 
	 * @param pagination
	 * @return
	 */
	public IPagination pagedByHQL(IPagination pagination) {
		// 断言此分页对象是否为HibernateHQLPagination的实例
		Assert.isInstanceOf(HibernateHQLPagination.class, pagination);
		String hql = ((HibernateHQLPagination) pagination).getHql();
		Object[] values = ((HibernateHQLPagination) pagination).getValues();
		String countQueryString = " select count (*) "
				+ removeSelect(removeOrders(hql));
		List countlist = getHibernateTemplate().find(countQueryString, values);
		Integer totalCount = (Integer) countlist.get(0);
		pagination.setTotalRows(totalCount.intValue());
		Query query = createQuery(hql, values);
		List list = query.setFirstResult(pagination.getStartIndex())
				.setMaxResults(pagination.getPageSize()).list();
		pagination.setData(list);
		return pagination;
	}

	/**
	 * 分页查询函数，使用hql.
	 * 
	 * @param pageNo
	 *            页号,从1开始.
	 */
	public Pagination pagedQuery(String hql, int pageNo, int pageSize,
			Object... values) {
		Assert.hasText(hql);
		Assert.isTrue(pageNo >= 1, "pageNo should start from 1");
		// Count查询
		String countQueryString = " select count (*) "
				+ removeSelect(removeOrders(hql));
		List countlist = getHibernateTemplate().find(countQueryString, values);
		long totalCount = (Long) countlist.get(0);

		if (totalCount < 1)
			return new Pagination();
		// 实际查询返回分页对象
		int startIndex = Pagination.getStartOfPage(pageNo, pageSize);
		Query query = createQuery(hql, values);
		List list = query.setFirstResult(startIndex).setMaxResults(pageSize)
				.list();

		return new Pagination(startIndex, totalCount, pageSize, list);
	}

	/**
	 * 分页查询函数，使用已设好查询条件与排序的<code>Criteria</code>.
	 * 
	 * @param pageNo
	 *            页号,从1开始.
	 * @return 含总记录数和当前页数据的Page对象.
	 */
	public Pagination pagedQuery(Criteria criteria, int pageNo, int pageSize) {
		Assert.notNull(criteria);
		Assert.isTrue(pageNo >= 1, "pageNo should start from 1");
		CriteriaImpl impl = (CriteriaImpl) criteria;

		// 先把Projection和OrderBy条件取出来,清空两者来执行Count操作
		Projection projection = impl.getProjection();
		List<CriteriaImpl.OrderEntry> orderEntries;
		try {
			orderEntries = (List) BeanUtils.forceGetProperty(impl,
					"orderEntries");
			BeanUtils.forceSetProperty(impl, "orderEntries", new ArrayList());
		} catch (Exception e) {
			throw new InternalError(" Runtime Exception impossibility throw ");
		}

		// 执行查询
		int totalCount = (Integer) criteria.setProjection(
				Projections.rowCount()).uniqueResult();

		// 将之前的Projection和OrderBy条件重新设回去
		criteria.setProjection(projection);
		if (projection == null) {
			criteria.setResultTransformer(CriteriaSpecification.ROOT_ENTITY);
		}

		try {
			BeanUtils.forceSetProperty(impl, "orderEntries", orderEntries);
		} catch (Exception e) {
			throw new InternalError(" Runtime Exception impossibility throw ");
		}

		// 返回分页对象
		if (totalCount < 1)
			return new Pagination();

		int startIndex = Pagination.getStartOfPage(pageNo, pageSize);
		List list = criteria.setFirstResult(startIndex).setMaxResults(pageSize)
				.list();
		return new Pagination(startIndex, totalCount, pageSize, list);
	}

	/**
	 * 分页查询函数，根据entityClass和查询条件参数创建默认的<code>Criteria</code>.
	 * 
	 * @param pageNo
	 *            页号,从1开始.
	 * @return 含总记录数和当前页数据的Page对象.
	 */
	public Pagination pagedQuery(int pageNo, int pageSize,
			Criterion... criterions) {
		Criteria criteria = createCriteria(criterions);
		return pagedQuery(criteria, pageNo, pageSize);
	}

	/**
	 * 分页查询函数，根据entityClass和查询条件参数,排序参数创建默认的<code>Criteria</code>.
	 * 
	 * @param pageNo
	 *            页号,从1开始.
	 * @return 含总记录数和当前页数据的Page对象.
	 */
	public Pagination pagedQuery(int pageNo, int pageSize, String orderBy,
			boolean isAsc, Criterion... criterions) {
		Criteria criteria = createCriteria(orderBy, isAsc, criterions);
		return pagedQuery(criteria, pageNo, pageSize);
	}

	/**
	 * 取得对象的主键值,辅助函数.
	 */
	public Serializable getId(Object entity) throws NoSuchMethodException,
			IllegalAccessException, InvocationTargetException {
		Assert.notNull(entity);
		return (Serializable) PropertyUtils.getProperty(entity, getIdName());
	}

	/**
	 * 取得对象的主键名,辅助函数.
	 */
	public String getIdName() {
		ClassMetadata meta = getSessionFactory().getClassMetadata(entityClass);
		Assert.notNull(meta, "Class " + entityClass
				+ " not define in hibernate session factory.");
		String idName = meta.getIdentifierPropertyName();
		Assert.hasText(idName, entityClass.getSimpleName()
				+ " has no identifier property define.");
		return idName;
	}

	/**
	 * 去除hql的select 子句，未考虑union的情况,用于pagedQuery.
	 * 
	 * @param hql
	 * @return
	 */
	private final static String removeSelect(String hql) {
		Assert.hasText(hql);
		int beginPos = hql.toLowerCase().indexOf("from");
		Assert.isTrue(beginPos != -1, " hql : " + hql
				+ " must has a keyword 'from'");
		return hql.substring(beginPos);
	}

	/**
	 * 去除hql的orderby 子句，用于pagedQuery.
	 * 
	 * @param hql
	 * @return
	 */
	private final static String removeOrders(String hql) {
		Assert.hasText(hql);
		Pattern p = Pattern.compile("order\\s*by[\\w|\\W|\\s|\\S]*",
				Pattern.CASE_INSENSITIVE);
		Matcher m = p.matcher(hql);
		StringBuffer sb = new StringBuffer();
		while (m.find()) {
			m.appendReplacement(sb, "");
		}
		m.appendTail(sb);
		return sb.toString();
	}
	
	public int getRowCount(final DetachedCriteria criteria) {
		criteria.setProjection(Projections.rowCount());
		Integer totalRows = (Integer) this.getHibernateTemplate().findByCriteria(criteria).iterator().next();
		criteria.setProjection(null);
		return totalRows.intValue();
	}
	
	public List<T> getPageObjects(final DetachedCriteria detachedCriteria, int pageNo, int pageSize) {
		Criteria criteria = detachedCriteria.getExecutableCriteria(this.getSession());
		criteria.setFirstResult((pageNo - 1) * pageSize).setMaxResults(pageSize);
		return criteria.setResultTransformer(Criteria.DISTINCT_ROOT_ENTITY).list();
	}
}
