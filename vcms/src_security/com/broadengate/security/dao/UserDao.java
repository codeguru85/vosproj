/**
 * 
 */
package com.broadengate.security.dao;

import org.springframework.stereotype.Repository;

import com.broadengate.core.orm.hibernate.HibernateEntityDao;
import com.broadengate.security.model.UserInfo;

/**
 * @author 龚磊
 *
 */
@Repository
public class UserDao extends HibernateEntityDao<UserInfo> {
	
}
