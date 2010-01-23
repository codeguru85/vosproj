/**
 * 
 */
package com.broadengate.vcms.dao;

import java.util.List;

import org.hibernate.Criteria;
import org.hibernate.SQLQuery;
import org.hibernate.criterion.DetachedCriteria;
import org.springframework.stereotype.Repository;

import com.broadengate.core.orm.hibernate.HibernateEntityDao;
import com.broadengate.vcms.entity.VideoInfo;

/**
 * @author kevin
 *
 */
@Repository
public class VideoInfoDao extends HibernateEntityDao<VideoInfo> {
	public List<VideoInfo> getAnyVideo(final DetachedCriteria detachedCriteria){
		Criteria criteria = detachedCriteria.getExecutableCriteria(this.getSession());
		return criteria.setResultTransformer(Criteria.DISTINCT_ROOT_ENTITY).list();
	}
	
	public SQLQuery getCreateSQLQuery(String strsql){
		return this.getSession().createSQLQuery(strsql);
	}
}
