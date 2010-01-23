/**
 * 
 */
package com.broadengate.security.dao;

import org.springframework.stereotype.Repository;

import com.broadengate.core.orm.hibernate.HibernateEntityDao;
import com.broadengate.security.model.GroupInfo;

/**
 * @author gonglei
 *
 */
@Repository
public class GroupDao extends HibernateEntityDao<GroupInfo> {

}
