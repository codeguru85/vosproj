package com.broadengate.core.orm;

import java.io.Serializable;
import java.util.List;

/**
 * 针对单个Entity对象的操作定义.不依赖于具体ORM实现方案.
 * 
 * @author calvin
 */
public interface EntityDao<T> {

	public T getById(Serializable id);

	public List<T> getAll();

	public T save(T o);

	public T remove(T o);

	public T removeById(Serializable id);

	/**
	 * 获取Entity对象的主键名.
	 */
	public String getIdName( );
}
