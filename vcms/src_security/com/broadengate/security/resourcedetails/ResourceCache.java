package com.broadengate.security.resourcedetails;

import java.util.ArrayList;
import java.util.List;

import net.sf.ehcache.Cache;
import net.sf.ehcache.CacheException;
import net.sf.ehcache.Element;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.springframework.dao.DataRetrievalFailureException;

import com.broadengate.security.Constants;

/**
 * Caches <code>Resource</code> objects using a Spring IoC defined <a
 * HREF="http://ehcache.sourceforge.net">EHCACHE</a>. 资源的缓存类
 * 
 * @author 龚磊
 */
public class ResourceCache {

	private static final Log logger = LogFactory.getLog(ResourceCache.class);

	private Cache cache;

	public void setCache(Cache cache) {
		this.cache = cache;
	}

	public Cache getCache() {
		return this.cache;
	}

	/**
	 * 根据资源串从缓存中查询资源
	 * 
	 * @param resString
	 * @return
	 */
	public ResourceDetails getAuthorityFromCache(String resString) {
		Element element = null;

		try {
			element = cache.get(resString);
		} catch (CacheException cacheException) {
			throw new DataRetrievalFailureException("Cache failure: " + cacheException.getMessage(),
					cacheException);
		}

		if (logger.isDebugEnabled()) {
			logger.debug("Cache hit: " + (element != null) + "; resString: " + resString);
		}

		if (element == null) {
			return null;
		} else {
			return (ResourceDetails) element.getValue();
		}
	}

	/**
	 * 以资源串为key,将资源对象放入缓存池中
	 * 
	 * @param resourceDetails
	 */
	public void putAuthorityInCache(ResourceDetails resourceDetails) {
		Element element = new Element(resourceDetails.getResString(), resourceDetails);
		if (logger.isDebugEnabled()) {
			logger.debug("Cache put: " + element.getKey());
		}
		this.cache.put(element);
	}

	/**
	 * 根据资源串,将资源对象中缓存池中删除
	 * 
	 * @param resString
	 */
	public void removeAuthorityFromCache(String resString) {
		if (logger.isDebugEnabled()) {
			logger.debug("Cache remove: " + resString);
		}
		this.cache.remove(resString);
	}

	public List<String> getUrlResStrings() {
		return getResourcesByType(Constants.RESOURCE_URL);
	}

	public List<String> getFunctions() {
		return getResourcesByType(Constants.RESOURCE_FUNCTION);
	}

	public List<String> getComponents() {
		return getResourcesByType(Constants.RESOURCE_COMPONENT);
	}

	public List<String> getMenus() {
		return getResourcesByType(Constants.RESOURCE_MENU);
	}

	@SuppressWarnings("unchecked")
	private List<String> getResourcesByType(String type) {
		List resclist = new ArrayList();
		try {
			List<String> resources = this.cache.getKeys();
			for (String resString : resources) {
				ResourceDetails rds = getAuthorityFromCache(resString);
				if (rds != null && rds.getResType().equals(type))
					resclist.add(resString);
			}
		} catch (IllegalStateException e) {
			throw new IllegalStateException(e.getMessage(), e);
		} catch (CacheException e) {
			throw new UnsupportedOperationException(e.getMessage(), e);
		}
		return resclist;
	}
}
