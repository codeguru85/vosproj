/**Administrator
 * 下午02:11:32
 */
package com.broadengate.vcms.service;
import java.util.List;

import com.broadengate.core.orm.hibernate.HibernateEntityDao;
import com.broadengate.vcms.entity.VideoAlbum;
import com.broadengate.vcms.entity.VideoAlbumShip;
import com.broadengate.vcms.entity.VideoAlbumShipId;
import com.broadengate.vcms.entity.VideoInfo;
import com.broadengate.vcms.web.Finder;

/**
 * @author Administrator
 *
 */
public interface IVideoAlbumService {

	
	/**
	 * 查询获取专辑树型菜单列表
	 * */
	String getTreeVideoAlbum();
	
	/**
	 * 分页查询信息
	 * @return
	 * */
	Finder getPageVideoAlbum(VideoAlbum videoAlbum, Integer toPage,
			Integer pageSize);
	
	/**
	 * 获取视频专辑列表信息
	 * @return
	 * */
	List<VideoAlbum> getVideoAlbum();
	
	/**
	 * 添加视频专辑信息
	 * @return
	 * */
	VideoAlbum addVideoAlbum(VideoAlbum videoAlbum);
	/**
	 * 修改视频专辑信息
	 * @return
	 * */
	VideoAlbum updateVideoAlbum(VideoAlbum videoAlbum, Integer albumId);
	/**
	 * 删除视频专辑信息
	 * @return
	 * */
	VideoAlbum removeVideoAlbum(VideoAlbum videoAlbum);
	/**
	 * 加入视频专辑
	 * */
	VideoAlbumShip addVideoAlbumShip(VideoAlbumShip videoAlbumShip);
	/**
	 * 查询所加入的视频是否已经被加入
	 * */
	public Integer getIsExitsVideoAlbumShip(VideoAlbumShip videoAlbumShip);
	/**
	 * 专辑内添加视频功能
	 * */
	public List<Integer>  getIsExitsArrayVideoAlbumShip(Integer videoAlbumId,VideoInfo[] videoIds);
}
