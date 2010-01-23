/**
 * 
 */
package com.broadengate.core.orm.hibernate;

import java.util.Date;

import org.apache.commons.lang.StringUtils;
import org.hibernate.criterion.DetachedCriteria;
import org.hibernate.criterion.Restrictions;

import com.broadengate.core.commons.time.DateUtils;

/**
 * @author 龚磊����
 * 
 */
public class HibernateTool {

	public static final DetachedCriteria getQueryDateRange(String beginDate, String endDate,
			String propertyName, DetachedCriteria criteria) {
		if (StringUtils.isNotEmpty(beginDate)) {
			if (StringUtils.isNotEmpty(endDate)) {
				criteria.add(Restrictions.between(propertyName, DateUtils.parseDate(beginDate), DateUtils .parseDate(endDate)));
			} else {
				criteria.add(Restrictions.ge(propertyName, DateUtils.parseDate(beginDate)));
			}
		} else if (StringUtils.isNotEmpty(endDate)) {
			criteria.add(Restrictions.le(propertyName, DateUtils.parseDate(endDate)));
		}
		return criteria;
	}

	public static final DetachedCriteria getQueryDateRange(Date beginDate, Date endDate, String propertyName,
			DetachedCriteria criteria) {
		if (beginDate != null) {
			if (endDate != null) {
				criteria.add(Restrictions.between(propertyName, beginDate, endDate));
			} else {
				criteria.add(Restrictions.ge(propertyName, beginDate));
			}
		} else if (endDate != null) {
			criteria.add(Restrictions.le(propertyName, endDate));
		}
		return criteria;
	}
	
	//以时分钞的时期结尾须向前推进一天
	public static final DetachedCriteria getQueryDateRangeMore(Date beginDate, Date endDate, String propertyName,
			DetachedCriteria criteria) {
		if (beginDate != null) {
			if (endDate != null) {
				endDate.setDate(endDate.getDate()+1);
				criteria.add(Restrictions.between(propertyName, beginDate, endDate));
			} else {
				criteria.add(Restrictions.ge(propertyName, beginDate));
			}
		} else if (endDate != null) {
			endDate.setDate(endDate.getDate()+1);
			criteria.add(Restrictions.le(propertyName, endDate));
		}
		return criteria;
	}

	public static final DetachedCriteria getQueryNumberRange(Number smallNum, Number bigNum,
			String propertyName, DetachedCriteria criteria) {
		if (smallNum != null) {
			if (bigNum != null) {
				criteria.add(Restrictions.between(propertyName, smallNum, bigNum));
			} else {
				criteria.add(Restrictions.ge(propertyName, smallNum));
			}
		} else if (bigNum != null) {
			criteria.add(Restrictions.le(propertyName, bigNum));
		}
		return criteria;
	}

}
