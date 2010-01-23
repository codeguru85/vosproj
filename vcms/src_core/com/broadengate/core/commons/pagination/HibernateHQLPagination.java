/**
 * 
 */
package com.broadengate.core.commons.pagination;


/**
 * @author 龚磊
 *
 */
public class HibernateHQLPagination implements IPagination {
	
	/**
	 * hibernate查询
	 */
	private String hql;
	
	/**
	 * 参数列表
	 */
	private Object[] values;

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
	 * 是否有下一页
	 * @return
	 */
	public boolean isHasNext() {
		return hasNext;
	}

	/**
	 * 是否有下一页
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
	 * @param totalRows the totalRows to set
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
	 * @param pageSize the pageSize to set
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
	 * @param toPage the toPage to set
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
	 * @param data the data to set
	 */
	public void setData(Object data) {
		this.data = data;
	}


	/**
	 * @return the hql
	 */
	public String getHql() {
		return hql;
	}

	/**
	 * @param hql the hql to set
	 */
	public void setHql(String hql) {
		this.hql = hql;
	}

	/**
	 * @return the values
	 */
	public Object[] getValues() {
		return values;
	}

	/**
	 * @param values the values to set
	 */
	public void setValues(Object[] values) {
		this.values = values;
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
