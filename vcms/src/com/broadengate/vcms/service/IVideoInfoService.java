/**Administrator
 * 下午03:32:06
 */
package com.broadengate.vcms.service;

import java.util.List;

import com.broadengate.core.orm.hibernate.HibernateEntityDao;
import com.broadengate.vcms.entity.Keyword;
import com.broadengate.vcms.entity.VideoAlbum;
import com.broadengate.vcms.entity.VideoAlbumShip;
import com.broadengate.vcms.entity.VideoAlbumShipId;
import com.broadengate.vcms.entity.VideoInfo;
import com.broadengate.vcms.entity.VideoInfoBean;
import com.broadengate.vcms.entity.VideoType;
import com.broadengate.vcms.web.Finder;

/**
 * @author Administrator
 *
 */
public interface IVideoInfoService{
	
	/**
	 * 分页查询信息
	 * @return 
	 * */
	public Finder getPageVideoInfo(VideoInfo videoInfo,Integer toPage,Integer pageSize);
	
	/**
	 * 视频列表高级查询的处理
	 * */
	public Finder getMoreVideoInfo(VideoInfoBean videoInfoBean,Integer toPage,Integer pageSize);
	/**
	 * 查询所有的视频内容列表
	 * @return 
	 * */
	public List<VideoInfo> getVideoInfos();
	/**
	 * 添加视频信息
	 * @return
	 * */
	VideoInfo addVideoInfo(VideoInfo videoInfo,VideoType videoType,VideoAlbumShip videoAlbumShip);
	
	VideoInfo addVideoInfo(VideoInfo videoInfo,VideoType[] videoTypes,VideoAlbumShip[] videoAlbumShips,String keyWords);
	/**
	 * 修改视频信息
	 * @return
	 * */
	VideoInfo updateVideoInfo(VideoInfo videoInfo,VideoType[] videoTypes,VideoAlbumShip[] videoAlbumShips,String keyWords);
	/**
	 * 删除视频信息
	 * @return
	 * */
	VideoInfo removeVideoInfo(VideoInfo videoInfo);
	/**
	 * 根据视频专辑查询视频信息
	 * */
	Finder getAlbumVideoInfo(VideoAlbum videoAlbum,Integer toPage,Integer pageSize);
	
	/**
	 * 根据视频专辑和视频查询条件查询视频信息
	 * */
	Finder getAlbumVideoInfo2(VideoAlbum videoAlbum,VideoInfo videoInfo,Integer toPage,Integer pageSize);
	
	/**
	 * 根据视频专辑查询视频信息
	 * */
	public List<VideoInfo> getAlbumVideoInfo3(VideoAlbum[] videoAlbums);
	/**
	 * 根据视频分类查询视频信息
	 * */
	Finder getTypeVideoInfoType(VideoType videoType, Integer toPage,
			Integer pageSize);
	
	/**
	 * 检查视频标题是否已经存在
	 * */
	public Boolean getExistVideoName(String videoName);
	
	/**
	 * 检查修改视频时标题是否已近存在
	 * */
	public Boolean getIsSetVideoInfo(Integer videoId,String videoName);
	/**
	 * 从专辑中移除指定视频
	 * */
	public String removeVideoAlbumShip(int videoAlbumId,VideoInfo[] videoInfos);
	
	/**
	 * 查询指定视频信息
	 * */
	public VideoInfo getVideoInfo(VideoInfo videoInfo);
	
	/**
	 * 根据所填关键字查询视频信息
	 * @return
	 * */
	public Finder getKeyWordVideoInfo(Keyword keyword,Integer toPage,Integer pageSize);
}
