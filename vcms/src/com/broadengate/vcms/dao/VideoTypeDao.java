/**
 * 
 */
package com.broadengate.vcms.dao;

import java.util.List;

import org.springframework.stereotype.Repository;

import com.broadengate.core.orm.hibernate.HibernateEntityDao;
import com.broadengate.vcms.entity.VideoType;

/**
 * @author kevin
 *
 */
@Repository
public class VideoTypeDao extends HibernateEntityDao<VideoType> {

	public List<VideoType> getSQLVideoType(String strsql)
	{
		return this.getSession().createQuery(strsql).list();
	}
}
