/**Administrator
 * 下午05:54:28
 */
package com.broadengate.vcms.service;

import java.util.List;

import com.broadengate.core.orm.hibernate.HibernateEntityDao;
import com.broadengate.vcms.entity.VideoType;
import com.broadengate.vcms.web.Finder;
/**
 * @author Administrator
 *
 */
public interface IVideoTypeService {
	
	
	/**
	 * 定义返回所有视频分类的树型菜单
	 * */
	String getTreeVideoType();
	/**
	 * 查询所有的视频分类列表
	 * @return 
	 * */
	List<VideoType> getVideoTypes(VideoType videoType);

	/**
	 * 获取分页查询视频分类列表
	 * @return
	 * */
	Finder getPageVideoType(VideoType videoType, Integer toPage,
			Integer pageSize);
	
	/**
	 * 设置每页显示数据条数
	 * */
	Finder getPagesize(Integer pageSize);
	
	/**
	 * 添加视频分类信息
	 * @return
	 * */
	VideoType addVideoType(VideoType videoType,Integer txtTypeId);
	
	/**
	 * 修改视频分类信息
	 * @return
	 * */
	VideoType updateVideoType(VideoType videoType,Integer txtTypeId);
	
	/**
	 * 删除视频分类信息
	 * @return
	 * 
	 * */
	VideoType removeVideoType(VideoType videoType);
	
	/**
	 * 修改视频分类的视频数
	 * @return
	 * 
	 * */
	VideoType updateVideoNum(VideoType videoType,Integer txtTypeId);

	/**
	 * 快捷窗口获取视频分类列表
	 * @return
	 * */
	Finder getQueryVideoType(VideoType videoType, Integer toPage,
			Integer pageSize);
	
	/**
	 * 查询所有根视频分类
	 * */
	List<VideoType> getRootVideoTypes();
	/**
	 * 查询根视频分类下面的所有子分类
	 * */
	List<VideoType> getParentVideoTypes(Integer typeId);

}
