/**Administrator
 * 下午03:34:54
 */
package com.broadengate.vcms.service.imp;

import java.io.File;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import javax.annotation.Resource;

import org.hibernate.Criteria;
import org.hibernate.criterion.DetachedCriteria;
import org.hibernate.criterion.Order;
import org.hibernate.criterion.Restrictions;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import com.broadengate.core.commons.fileUtils.FileUtils;
import com.broadengate.core.orm.hibernate.HibernateTool;
import com.broadengate.vcms.dao.KeywordDao;
import com.broadengate.vcms.dao.VideoAlbumDao;
import com.broadengate.vcms.dao.VideoAlbumShipDao;
import com.broadengate.vcms.dao.VideoInfoDao;
import com.broadengate.vcms.dao.VideoTypeDao;
import com.broadengate.vcms.entity.Keyword;
import com.broadengate.vcms.entity.VideoAlbum;
import com.broadengate.vcms.entity.VideoAlbumShip;
import com.broadengate.vcms.entity.VideoAlbumShipId;
import com.broadengate.vcms.entity.VideoInfo;
import com.broadengate.vcms.entity.VideoInfoBean;
import com.broadengate.vcms.entity.VideoType;
import com.broadengate.vcms.service.IVideoInfoService;
import com.broadengate.vcms.web.Finder;
import com.broadengate.vcms.web.listener.MyServletContextListener;

/**
 * @author kevin
 * 
 */
// Spring Service Bean的标识.
// 默认将类中的所有函数纳入事务管理.
@Service
@Transactional
public class VideoInfoService implements IVideoInfoService {

	@Resource
	private VideoInfoDao videoInfoDao;

	@Resource
	private VideoAlbumShipDao videoAlbumShipDao;

	@Resource
	private VideoTypeDao videoTypeDao;
	
	@Resource
	private KeywordDao keywordDao;
	
	@Resource
	private VideoAlbumDao videoAlbumDao;
	

	/**
	 * @param videoInfoDao
	 *            the videoInfoDao to set
	 */
	public void setVideoInfoDao(VideoInfoDao videoInfoDao) {
		this.videoInfoDao = videoInfoDao;
	}

	/**
	 * @param videoAlbumShipDao
	 *            the videoAlbumShipDao to set
	 */
	public void setVideoAlbumShipDao(VideoAlbumShipDao videoAlbumShipDao) {
		this.videoAlbumShipDao = videoAlbumShipDao;
	}

	/**
	 * 根据视频专辑查询视频信息
	 * */
	@Override
	public Finder getAlbumVideoInfo(VideoAlbum videoAlbum, Integer toPage,
			Integer pageSize) {
		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);
		DetachedCriteria criteria = DetachedCriteria.forClass(VideoInfo.class);
		criteria.createAlias("videoAlbumShips", "albumShips",
				DetachedCriteria.LEFT_JOIN);
		criteria.add(Restrictions.eq("albumShips.videoAlbum.albumId",
				videoAlbum.getAlbumId()));
		finder.setTotalRows(videoInfoDao.getRowCount(criteria));
		List<VideoInfo> types = videoInfoDao.getPageObjects(criteria, toPage,
				finder.getPageSize());
		finder.setList(types);
		return finder;
	}

	/**
	 * 根据视频专辑和视频查询条件查询视频信息
	 * */
	@Override
	public Finder getAlbumVideoInfo2(VideoAlbum videoAlbum,
			VideoInfo videoInfo, Integer toPage, Integer pageSize) {
		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);
		DetachedCriteria criteria = DetachedCriteria.forClass(VideoInfo.class);
		criteria.createAlias("videoAlbumShips", "albumShips",
				DetachedCriteria.LEFT_JOIN);
		criteria.add(Restrictions.eq("albumShips.videoAlbum.albumId",
				videoAlbum.getAlbumId()));

		if (videoInfo != null) {
			if (videoInfo.getVideoName() != null
					&& videoInfo.getVideoName() != "") {
				criteria.add(Restrictions.like("videoName", "%"
						+ videoInfo.getVideoName() + "%"));
			}
		}

		finder.setTotalRows(videoInfoDao.getRowCount(criteria));
		List<VideoInfo> types = videoInfoDao.getPageObjects(criteria, toPage,
				finder.getPageSize());
		finder.setList(types);
		return finder;
	}
	
	public List<VideoInfo> getAlbumVideoInfo3(VideoAlbum[] videoAlbums){
		List<Integer> AlbumIds = new ArrayList<Integer>();
		Criteria criteria = videoInfoDao.createCriteria();		
		criteria.createAlias("videoAlbumShips", "albumShips",
				DetachedCriteria.LEFT_JOIN);	
		for(VideoAlbum videoAlbum:videoAlbums){
			AlbumIds.add(videoAlbum.getAlbumId());
			System.out.println("videoAlbum.getAlbumId():"+videoAlbum.getAlbumId());
		}
		criteria.add(Restrictions.in("albumShips.videoAlbum.albumId",AlbumIds));
				
		criteria.setResultTransformer(Criteria.DISTINCT_ROOT_ENTITY);		
		List<VideoInfo> VideoInfos = criteria.list();		
		return VideoInfos;
	}

	/**
	 * 根据视频分类查询视频信息
	 * */
	@Override
	public Finder getTypeVideoInfoType(VideoType videoType, Integer toPage,
			Integer pageSize) {
		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);
		DetachedCriteria criteria = DetachedCriteria.forClass(VideoInfo.class);
		criteria.createAlias("videoTypes", "types", DetachedCriteria.LEFT_JOIN);
		criteria.add(Restrictions.eq("types.typeId", videoType.getTypeId()));
		criteria.addOrder(Order.desc("createTime"));
		finder.setTotalRows(videoInfoDao.getRowCount(criteria));
		List<VideoInfo> types = videoInfoDao.getPageObjects(criteria, toPage,
				finder.getPageSize());
		finder.setList(types);
		return finder;
	}
	
	/**
	 * 根据所填关键字查询视频信息
	 * @return
	 * */
	public Finder getKeyWordVideoInfo(Keyword keyword,Integer toPage,Integer pageSize)
	{
		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);
	
		DetachedCriteria criteria = DetachedCriteria.forClass(VideoInfo.class);
		criteria.createAlias("keywords", "keyword",DetachedCriteria.LEFT_JOIN);
		
		if(keyword!=null){
			if(keyword.getWordName()!=null && keyword.getWordName()!=""){
				criteria.add(Restrictions.like("keyword.wordName", "%"+keyword.getWordName()+"%"));
			}
		}
		criteria.addOrder(Order.desc("createTime"));
		finder.setTotalRows(videoInfoDao.getRowCount(criteria));
		List<VideoInfo> keywords = videoInfoDao.getPageObjects(criteria,toPage,
				finder.getPageSize());
		finder.setList(keywords);
		return finder;
		
	}
	
	/**
	 * 分页查询信息
	 * 
	 * @return
	 * */
	public Finder getPageVideoInfo(VideoInfo videoInfo, Integer toPage,
			Integer pageSize) {

		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);
		/**
		 * 加载所有可变的查询条件
		 * */
		DetachedCriteria criteria = DetachedCriteria.forClass(VideoInfo.class);
		/**
		 * 视频对象
		 * */
		if (videoInfo != null) {
			if (videoInfo.getVideoName() != null
					&& videoInfo.getVideoName() != "") {
				criteria.add(Restrictions.like("videoName", "%"
						+ videoInfo.getVideoName() + "%"));
			}
		}

		criteria.addOrder(Order.desc("createTime"));
		finder.setTotalRows(videoInfoDao.getRowCount(criteria));
		List<VideoInfo> videoInfos = videoInfoDao.getPageObjects(criteria,
				toPage, finder.getPageSize());
		finder.setList(videoInfos);
		return finder;
	}

	/**
	 * 视频列表高级查询的处理
	 * */
	public Finder getMoreVideoInfo(VideoInfoBean videoInfoBean, Integer toPage,
			Integer pageSize) {
		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);

		/**
		 * 加载所有可变的查询条件
		 * */
		DetachedCriteria criteria = DetachedCriteria.forClass(VideoInfo.class);

		/**
		 * 视频对象
		 * */
		if (videoInfoBean != null) {
			if (videoInfoBean.getVideoName() != null
					&& videoInfoBean.getVideoName() != "") {
				criteria.add(Restrictions.like("videoName", "%"
						+ videoInfoBean.getVideoName() + "%"));
			}
		}
		/**
		 * 视频文件格式的查询
		 * */
		if (videoInfoBean != null) {
			if (videoInfoBean.getFileFormat() != null
					&& videoInfoBean.getFileFormat() != "") {
				criteria.add(Restrictions.like("fileFormat", "%"
						+ videoInfoBean.getFileFormat() + "%"));
			}
		}

		/**
		 * 时间长的查询
		 * */
		if (videoInfoBean != null) {
			if (videoInfoBean.getStartTime() != ""
					&& videoInfoBean.getEndTime() == "") {
				criteria.add(Restrictions.le("timeSecond", Integer
						.parseInt(videoInfoBean.getStartTime()) * 60));
			} else if (videoInfoBean.getEndTime() != ""
					&& videoInfoBean.getStartTime() == "") {
				criteria.add(Restrictions.ge("timeSecond", Integer
						.parseInt(videoInfoBean.getEndTime()) * 60));
			} else if (videoInfoBean.getStartTime() != ""
					&& videoInfoBean.getEndTime() != "") {
				criteria.add(Restrictions.ge("timeSecond", Integer
						.parseInt(videoInfoBean.getStartTime()) * 60));
				criteria.add(Restrictions.le("timeSecond", Integer
						.parseInt(videoInfoBean.getEndTime()) * 60));
			}

		}

		/**
		 * 视频文件大小的查询
		 * */
		if (videoInfoBean != null) {
			/**
			 * 第一个参数
			 * */
			if (videoInfoBean.getStartSize() != ""
					&& videoInfoBean.getEndSize() == "") {
				criteria.add(Restrictions.le("fileSize", Double
						.parseDouble(videoInfoBean.getStartSize())));
			} else if (videoInfoBean.getEndSize() != ""
					&& videoInfoBean.getStartSize() == "") {
				criteria.add(Restrictions.ge("fileSize", Double
						.parseDouble(videoInfoBean.getEndSize())));
			} else if (videoInfoBean.getStartSize() != ""
					&& videoInfoBean.getEndSize() != "") {
				criteria.add(Restrictions.ge("fileSize", Double
						.parseDouble(videoInfoBean.getStartSize())));
				criteria.add(Restrictions.le("fileSize", Double
						.parseDouble(videoInfoBean.getEndSize())));
			}
		}

		/**
		 * 上传时间
		 * */
		criteria = HibernateTool.getQueryDateRange(
				videoInfoBean.getStartDate(), videoInfoBean.getEndDate(),
				"createTime", criteria);
		/**
		 * 关键字
		 */
		if(videoInfoBean.getKeywords()!=null && !videoInfoBean.getKeywords().isEmpty()){
			Iterator<Keyword> keyWords = videoInfoBean.getKeywords().iterator();
			List<String> words = new ArrayList();
			while(keyWords.hasNext()){
				words.add(keyWords.next().getWordName());
			}
			criteria.createAlias("keywords", "keys", DetachedCriteria.LEFT_JOIN);
			criteria.add(Restrictions.in("keys.wordName", words));
		}
		
		criteria.addOrder(Order.desc("createTime"));
		finder.setTotalRows(videoInfoDao.getRowCount(criteria));
		List<VideoInfo> videoInfos = videoInfoDao.getPageObjects(criteria,
				toPage, finder.getPageSize());
		finder.setList(videoInfos);
		return finder;
	}

	/**
	 * 查询所有的视频内容列表
	 * 
	 * @return
	 * */
	@Override
	public List<VideoInfo> getVideoInfos() {
		return videoInfoDao.getAll();
	}

	/**
	 * 添加视频信息
	 * 
	 * @return
	 * */
	@Override
	public VideoInfo addVideoInfo(VideoInfo videoInfo, VideoType videoType,
			VideoAlbumShip videoAlbumShip) {
		videoInfo.setVideoId(null);
		videoInfo.setCreateTime(new Date());
		VideoInfo videoInfos = videoInfoDao.save(videoInfo);

		if (videoType != null) {
			VideoType v = videoTypeDao.getById(videoType.getTypeId());
			Integer count = v.getVideoNum() + 1;
			v.setVideoNum(count);
			VideoType rootType = v.getVideoType();
			while(rootType != null){
				rootType.setVideoNum(rootType.getVideoNum() + 1);
				rootType = rootType.getVideoType();
			}
			videoInfos.getVideoTypes().add(v);
			v.getVideoInfos().add(videoInfos);
		}

		if (videoAlbumShip != null) {
			VideoAlbumShipId videoAlbumShipId = new VideoAlbumShipId();
			if (videoAlbumShip.getVideoAlbum().getAlbumId() != -1) {
				videoAlbumShipId.setAlbumId(videoAlbumShip.getVideoAlbum()
						.getAlbumId());
				videoAlbumShipId.setVideoId(videoInfos.getVideoId());
				videoAlbumShipId.setAlbumSeq(1000);
				videoAlbumShipId.setCreateTime(new Date());
				videoAlbumShip.setId(videoAlbumShipId);
				VideoAlbumShip va = videoAlbumShipDao.save(videoAlbumShip);
			}
		}
		return videoInfos;
	}

	/**
	 * 修改视频信息
	 * 
	 * @return
	 * */
	@Override
	public VideoInfo updateVideoInfo(VideoInfo videoInfo, VideoType[] videoTypes,
			VideoAlbumShip videoAlbumShip,String keyWords) {
		VideoInfo videoInfoTemp = videoInfoDao.getById(videoInfo.getVideoId());
		videoInfoTemp.setVideoName(videoInfo.getVideoName());
		videoInfoTemp.setDescn(videoInfo.getDescn());
		Set<VideoType> typeSet = videoInfoTemp.getVideoTypes();
		Set<Keyword> setKeywords = videoInfoTemp.getKeywords();

		/**
		 * 分类
		 * */
		if(videoTypes.length!=0){
			for (VideoType typeOne : typeSet) {
				typeOne.getVideoInfos().remove(videoInfoTemp);
				typeOne.setVideoNum(typeOne.getVideoNum() - 1);
				VideoType rootType = typeOne.getVideoType();
				while(rootType != null){
					rootType.setVideoNum(rootType.getVideoNum()-1);
					rootType = rootType.getVideoType();
				}
			}

			
			for(VideoType videoType:videoTypes){
				VideoType v1 = videoTypeDao.getById(videoType.getTypeId());
				Integer count = v1.getVideoNum() + 1;
				v1.setVideoNum(count);
				VideoType rootType = v1.getVideoType();
				while(rootType != null){
					rootType.setVideoNum(rootType.getVideoNum()+1);
					rootType = rootType.getVideoType();
				}
				v1.getVideoInfos().add(videoInfoTemp);
				typeSet.add(v1);					
			}
			
		}
		
		/**
		 * 关键字的处理
		 * */
		if(keyWords!=null&&keyWords!=""){
					
				String strstr;
				String arr[];
					
				strstr = keyWords.replaceAll("，", ",");
				arr= strstr.split(",");
				
				videoInfoTemp.getKeywords().removeAll(setKeywords);
				for(Keyword keyword:setKeywords){					
					keywordDao.remove(keyword);
				}
				
				for(int i=0;i<arr.length;i++){
					Keyword kw = new Keyword();
					kw.setWordId(null);
					kw.setWordName(arr[i]);
					kw.setCreateTime(new Date());
					kw.setVideoNum(1);
					
					keywordDao.save(kw);
					videoInfoTemp.getKeywords().add(kw);
				}
		}
		
		
/*		if (videoType != null) {
			if (!parentTypeId.equals(videoType.getTypeId())) {
				if (!parentTypeId.equals(0)) {
					VideoType v = videoTypeDao.getById(parentTypeId);
					Integer count = v.getVideoNum() - 1;
					v.setVideoNum(count);
					v.getVideoInfos().remove(videoInfoTemp);
				}

				// =======================================================
				VideoType v1 = videoTypeDao.getById(videoType.getTypeId());
				Integer countV1 = v1.getVideoNum() + 1;
				v1.setVideoNum(countV1);
				videoInfoTemp.getVideoTypes().add(v1);
				v1.getVideoInfos().add(videoInfoTemp);

			}
		}*/
		typeSet.clear();
		return videoInfoTemp;
	}

	/**
	 * 删除视频信息
	 * 
	 * @return
	 * */
	@Override
	public VideoInfo removeVideoInfo(VideoInfo videoInfo) {
		VideoInfo videoInfoTemp = videoInfoDao.getById(videoInfo.getVideoId());
		if (videoInfoTemp.getPlayListShips().size() > 0) {
			return null;
		} else {
			/**
			 * 删除视频文件
			 * */
			String filedir = MyServletContextListener.servletContext
					.getRealPath("/");
			FileUtils file = new FileUtils();
			if (videoInfo.getFilePath() != null) {
				String filePath = videoInfo.getFilePath();
				file.delFile(filedir + File.separator + filePath);
				//删除视频信息文本
				String fileInfoPath = videoInfo.getFilePath().split("\\.")[0];
				file.delFile(filedir + File.separator + fileInfoPath);
			}

			/**
			 * 删除视频对应的缩略图
			 * */
			if (videoInfo.getImagePath() != null) {
				String imagePath = videoInfo.getImagePath();
				file.delFile(filedir + File.separator + imagePath);
			}

			Set<VideoType> typeSet = videoInfoTemp.getVideoTypes();
			for (VideoType typeOne : typeSet) {
				typeOne.getVideoInfos().remove(videoInfoTemp);
				typeOne.setVideoNum(typeOne.getVideoNum() - 1);
				VideoType rootType = typeOne.getVideoType();
				while(rootType != null){
					rootType.setVideoNum(rootType.getVideoNum()-1);
					rootType = rootType.getVideoType();
				}
			}
			typeSet.clear();

			/**
			 * 删除并修改关联的专辑信息
			 */
			Set<VideoAlbumShip> albumShipSet = videoInfoTemp.getVideoAlbumShips();
			for(VideoAlbumShip videoAlbumShip:albumShipSet){

				videoAlbumShip = this.videoAlbumShipDao.remove(videoAlbumShip);
				VideoAlbum album = videoAlbumShip.getVideoAlbum();
				album.setVideoNum(album.getVideoNum()-1);
			}
			
			/**
			 * 删除视频关键字的关联
			 * */
			Set<Keyword> keySet = videoInfoTemp.getKeywords();
			for(Keyword keyOne :keySet)
			{
				keyOne.getVideoInfos().remove(videoInfoTemp);
				keywordDao.remove(keyOne);
			}
			return videoInfoDao.remove(videoInfoTemp);
		}
	}
	
	/**
	 * 查询指定视频的信息
	 * */
	public VideoInfo getVideoInfo(VideoInfo videoInfo) {
		VideoInfo videoInfoTemp = videoInfoDao.getById(videoInfo.getVideoId());
		return videoInfoTemp;
	}

	/**
	 * 查询播放列表内是否存在有视频
	 * */

	/**
	 * 检查视频标题是否已经存在
	 * */
	@Override
	public Boolean getExistVideoName(String videoName) {
		String hql = "from VideoInfo as v  where v.videoName='" + videoName
				+ "'";
		List<VideoInfo> lists = videoInfoDao.find(hql);
		if (lists.size() > 0) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * 检查修改视频时标题是否已近存在
	 * */
	@Override
	public Boolean getIsSetVideoInfo(Integer videoId, String videoName) {
		String hql = "from VideoInfo as v  where v.videoName='" + videoName
				+ "' and v.videoId!=" + videoId + "";
		List<VideoInfo> lists = videoInfoDao.find(hql);
		if (lists.size() > 0) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * 从专辑中移除指定视频
	 * 
	 * @return
	 * */
	@Override
	public String removeVideoAlbumShip(int videoAlbumId, VideoInfo[] videoInfos) {
		int n = videoInfos.length;
		int[] arr = new int[n];
		for (int i = 0; i < n; i++) {
			arr[i] = videoInfos[i].getVideoId();
		}
		return videoAlbumShipDao.delVideoAlbum(videoAlbumId, arr);
	}

	/**
	 * 添加多选分类的新增视频
	 * */
	@Override
	public VideoInfo addVideoInfo(VideoInfo videoInfo, VideoType[] videoTypes,
			VideoAlbumShip[] videoAlbumShips,String keyWords) {
		videoInfo.setVideoId(null);
		VideoInfo vi = videoInfoDao.save(videoInfo);
		if (vi.getVideoTypes() == null) {
			vi.setVideoTypes(new HashSet<VideoType>(0));
		}
		Integer count;
		for (VideoType type : videoTypes) {
			type = videoTypeDao.getById(type.getTypeId());
			count = type.getVideoNum() + 1;
			type.setVideoNum(count);
			VideoType rootType = type.getVideoType();
			while(rootType != null){
				rootType.setVideoNum(rootType.getVideoNum() + 1);
				rootType = rootType.getVideoType();
			}

			vi.getVideoTypes().add(type);
			type.getVideoInfos().add(vi);
		}
		for(VideoAlbumShip videoAlbumShip:videoAlbumShips){
			if (videoAlbumShip != null) {
				VideoAlbumShipId videoAlbumShipId = new VideoAlbumShipId();
				if (videoAlbumShip.getVideoAlbum().getAlbumId() != -1) {
					videoAlbumShipId.setAlbumId(videoAlbumShip.getVideoAlbum()
							.getAlbumId());
					videoAlbumShipId.setVideoId(vi.getVideoId());
					videoAlbumShipId.setAlbumSeq(1000);
					videoAlbumShipId.setCreateTime(new Date());
					videoAlbumShip.setId(videoAlbumShipId);
					videoAlbumShipDao.save(videoAlbumShip);
					
					//更新专辑信息
					VideoAlbum videoAlbum = videoAlbumDao.getById(videoAlbumShip.getVideoAlbum().getAlbumId());
					videoAlbum.setVideoNum(videoAlbum.getVideoNum()+1);
				}
			}
		}
		
		if(keyWords!=null&&keyWords!=""){
			
			String strstr;
			String arr[];
			
			strstr = keyWords.replaceAll("，", ",");
			arr= strstr.split(",");
			if (vi.getKeywords() == null) {
				vi.setKeywords(new HashSet<Keyword>(0));
			}
			for(int i=0;i<arr.length;i++){
				Keyword keyword = new Keyword();
				
				keyword.setWordName(arr[i]);
				keyword.setCreateTime(new Date());
				keyword.setVideoNum(1);
				keywordDao.save(keyword);
				vi.getKeywords().add(keyword);
				keyword.getVideoInfos().add(vi);
			}
		}
		return vi;
	}
}