/**
 * 
 */
package com.broadengate.core.commons.pagination;

import org.hibernate.criterion.DetachedCriteria;

/**
 * @author 龚磊
 * 
 */
public class HibernateCriteriaPagination implements IPagination {
	/**
	 * hibernate查询器
	 */
	private DetachedCriteria criteria;

	/**
	 * 记录总数
	 */
	private int totalRows = 0;

	/**
	 * 总页数
	 */
	private int totalPages = 0;

	/**
	 * 每页显示数量
	 */
	private int pageSize = DEFAULT_PAGE_SIZE;

	/**
	 * 当前页号
	 */
	private int currentPage = 1;

	/**
	 * 转向页号
	 */
	private int toPage = 1;

	/**
	 * 是否有上一页
	 */
	private boolean hasPrevious = false;

	/**
	 * 是否有下一页
	 */
	private boolean hasNext = false;

	/**
	 * 结果数据集,集合类型,可为List, set, collection等,当前页中存放的记录,类型一般为List
	 */
	private Object data;

	/**
	 * 构造方法,注意只有当toPage, pageSize大于0时才会更改默认值
	 * @param criteria
	 * @param pageSize
	 * @param toPage
	 */
	public HibernateCriteriaPagination(DetachedCriteria criteria, int toPage, int pageSize) {
		super();
		this.criteria = criteria;
		if (toPage > 0) {
			setToPage(toPage);
		}
		if (pageSize > 0) {
			setPageSize(pageSize);
		}
	}

	/**
	 * @param criteria
	 */
	public HibernateCriteriaPagination(DetachedCriteria criteria) {
		super();
		this.criteria = criteria;
	}

	/**
	 * @param criteria
	 * @param toPage
	 */
	public HibernateCriteriaPagination(DetachedCriteria criteria, int toPage) {
		super();
		this.criteria = criteria;
		this.toPage = toPage;
	}

	/**
	 * 是否有下一页
	 * 
	 * @return
	 */
	public boolean isHasNext() {
		return hasNext;
	}

	/**
	 * 是否有下一页
	 * 
	 * @return
	 */
	public boolean isHasPrevious() {
		return hasPrevious;
	}

	/**
	 * 刷新当前分页对象数据
	 */
	private void refresh() {
		if (totalPages <= 1) {
			hasPrevious = false;
			hasNext = false;
		} else if (currentPage == 1) {
			hasPrevious = false;
			hasNext = true;
		} else if (currentPage == totalPages) {
			hasPrevious = true;
			hasNext = false;
		} else {
			hasPrevious = true;
			hasNext = true;
		}
	}

	/**
	 * @return the totalRows
	 */
	public int getTotalRows() {
		return totalRows;
	}

	/**
	 * @param totalRows
	 *            the totalRows to set
	 */
	public void setTotalRows(int totalRows) {
		this.totalRows = totalRows;
		this.totalPages = ((totalRows + pageSize) - 1) / pageSize;
		refresh();
	}

	/**
	 * @return the totalPages
	 */
	public int getTotalPages() {
		return totalPages;
	}

	/**
	 * @return the pageSize
	 */
	public int getPageSize() {
		return pageSize;
	}

	/**
	 * @param pageSize
	 *            the pageSize to set
	 */
	public void setPageSize(int pageSize) {
		this.pageSize = pageSize;
		this.totalPages = ((totalRows + pageSize) - 1) / pageSize;
	}

	/**
	 * @return the toPage
	 */
	public int getToPage() {
		return toPage;
	}

	/**
	 * @param toPage
	 *            the toPage to set
	 */
	public void setToPage(int toPage) {
		this.toPage = toPage;
		this.currentPage = toPage;
	}

	/**
	 * @return the data
	 */
	public Object getData() {
		return data;
	}

	/**
	 * @param data
	 *            the data to set
	 */
	public void setData(Object data) {
		this.data = data;
	}

	/**
	 * @return the criteria
	 */
	public DetachedCriteria getCriteria() {
		return criteria;
	}

	/**
	 * @return the currentPage
	 */
	public int getCurrentPage() {
		return currentPage;
	}

	@Override
	public int getStartIndex() {
		return (toPage - 1) * pageSize;
	}
}
