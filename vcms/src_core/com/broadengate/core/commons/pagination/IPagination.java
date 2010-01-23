/**
 * 
 */
package com.broadengate.core.commons.pagination;

/**
 * 通用分页接口. 实现类可继承此接口实现一般的数据分页,此接口与具体持久化框架无关.
 * 
 * @author 龚磊
 */
public interface IPagination {

	public static int DEFAULT_PAGE_SIZE = 10;

	/**
	 * 是否有下一页
	 * 
	 * @return
	 */
	public boolean isHasNext();

	/**
	 * 是否有下一页
	 * 
	 * @return
	 */
	public boolean isHasPrevious();

	/**
	 * @return 记录总数
	 */
	public int getTotalRows();

	/**
	 * @param totalRows
	 *            设置记录总数
	 */
	public void setTotalRows(int totalRows);

	/**
	 * @return 总页数
	 */
	public int getTotalPages();

	/**
	 * @return 每页记录数
	 */
	public int getPageSize();

	/**
	 * @param pageSize
	 *            设置每页记录数
	 */
	public void setPageSize(int pageSize);

	/**
	 * @return 路转页号
	 */
	public int getToPage();

	/**
	 * @param toPage
	 *            设置跳转页号
	 */
	public void setToPage(int toPage);

	/**
	 * 返回结果数据集
	 * 
	 * @return the data
	 */
	public Object getData();

	/**
	 * @param data
	 *            设置结果数据集
	 */
	public void setData(Object data);

	/**
	 * @return 当前页号
	 */
	public int getCurrentPage();
	
	/**
	 * 当前页第一条记录在数据库中的位置
	 * @return
	 */
	public int getStartIndex();
}
