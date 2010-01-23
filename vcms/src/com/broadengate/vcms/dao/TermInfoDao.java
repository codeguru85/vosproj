/**
 * 
 */
package com.broadengate.vcms.dao;

import java.util.List;

import org.hibernate.Criteria;
import org.hibernate.criterion.DetachedCriteria;
import org.springframework.stereotype.Repository;

import com.broadengate.core.orm.hibernate.HibernateEntityDao;
import com.broadengate.vcms.entity.TermInfo;

/**
 * @author huang
 *
 */
@Repository
public class TermInfoDao extends HibernateEntityDao<TermInfo> {
	public List<TermInfo> getTermInfosByNum(final DetachedCriteria detachedCriteria){
		Criteria criteria = detachedCriteria.getExecutableCriteria(this.getSession());
		return criteria.setResultTransformer(Criteria.DISTINCT_ROOT_ENTITY).list();
	}
}
