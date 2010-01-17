package com.broadengate.vcms.web;

/**
 * 
 */


import java.util.List;

/**
 * @author ����
 * 
 */
public class Finder {

	private List list;

	private long totalRows = 0;

	private int totalPages = 0;

	private int pageSize = 20;

	private int currentPage = 1;

	private int toPage = 1;

	private boolean hasPrevious = false;

	private boolean hasNext = false;

	public void setTotalRows(long totalRowsNum) {
		this.totalRows = totalRowsNum;
		this.totalPages = (int)((totalRowsNum + pageSize) - 1) / pageSize;
		refresh();
	}

	public void first() {
		this.currentPage = 1;
		this.toPage = 1;
		refresh();
	}

	public void previous() {
		this.currentPage--;
		this.toPage--;
		refresh();
	}

	public void next() {
		if (currentPage < totalPages) {
			currentPage++;
			this.toPage++;
		}
		refresh();
	}

	public void last() {
		currentPage = totalPages;
		refresh();
	}

	public boolean isHasNext() {
		return hasNext;
	}

	public boolean isHasPrevious() {
		return hasPrevious;
	}

	public void refresh() {
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

	public int getCurrentPage() {
		return currentPage;
	}

	public void setCurrentPage(int currentPage) {
		this.currentPage = currentPage;
	}

	public int getPageSize() {
		return pageSize;
	}

	public void setPageSize(int pageSize) {
		this.pageSize = pageSize;
	}

	public int getToPage() {
		return toPage;
	}

	public void setToPage(int toPage) {
		this.toPage = toPage;
		this.currentPage = toPage;
	}

	public int getTotalPages() {
		return totalPages;
	}

	public void setTotalPages(int totalPages) {
		this.totalPages = totalPages;
	}

	public long getTotalRows() {
		return totalRows;
	}

	public void setHasNext(boolean hasNext) {
		this.hasNext = hasNext;
	}

	public void setHasPrevious(boolean hasPrevious) {
		this.hasPrevious = hasPrevious;
	}

	public List getList() {
		return list;
	}

	public void setList(List list) {
		this.list = list;
	}
}
