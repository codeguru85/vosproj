/**Administrator
 * 下午02:11:03
 */
package com.broadengate.vcms.service.imp;

import java.util.Date;
import java.util.List;

import javax.annotation.Resource;

import org.hibernate.criterion.DetachedCriteria;
import org.hibernate.criterion.Order;
import org.hibernate.criterion.Restrictions;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import com.broadengate.vcms.entity.VideoAlbum;
import com.broadengate.vcms.entity.VideoInfo;
import com.broadengate.vcms.entity.VideoAlbumShipId;
import com.broadengate.vcms.entity.VideoAlbumShip;
import com.broadengate.vcms.entity.VideoType;
import com.broadengate.vcms.dao.VideoAlbumDao;
import com.broadengate.vcms.dao.VideoAlbumShipDao;
import com.broadengate.vcms.service.IVideoAlbumService;
import com.broadengate.vcms.web.Finder;

import flex.messaging.io.ArrayList;

/**
 * @author kevin
 * 
 */
// Spring Service Bean的标识.
// 默认将类中的所有函数纳入事务管理.
@Service
@Transactional
public class VideoAlbumService implements IVideoAlbumService{
	
	@Resource
	private VideoAlbumDao videoAlbumDao;

	@Resource
	private VideoAlbumShipDao videoAlbumShipDao;
	
	
	/**
	 * 加入视频专辑
	 * @return 
	 * */
	@Override
	public VideoAlbumShip addVideoAlbumShip(VideoAlbumShip videoAlbumShip)
	{
		return videoAlbumShipDao.save(videoAlbumShip);
	}
	
	/**
	 * 查询所加入的视频是否已经被加入
	 * */
	@Override
	public Integer getIsExitsVideoAlbumShip(VideoAlbumShip videoAlbumShip){
		String hql="from VideoAlbumShip as v  where v.videoInfo.videoId ="+videoAlbumShip.getId().getVideoId()+" and v.videoAlbum.albumId="+videoAlbumShip.getVideoAlbum().getAlbumId()+""; 
		List<VideoType> list = videoAlbumShipDao.find(hql);
		if(list.size()>0){
			return 0;
		}else{
			return videoAlbumShip.getId().getVideoId();
		}
	}
	/**
	 * 专辑内添加视频功能
	 * */
	public List<Integer> getIsExitsArrayVideoAlbumShip(Integer videoAlbumId,VideoInfo[] videoIds)
	{
		Integer count=0;
		List<Integer> temp=new ArrayList();
		for(int i=0;i<videoIds.length;i++)
		{
			String hql="from VideoAlbumShip as v  where v.videoInfo.videoId ="+videoIds[i].getVideoId()+" and v.videoAlbum.albumId="+videoAlbumId+""; 
			List<VideoType> list = videoAlbumShipDao.find(hql);
			if(list.size()>0){
				count =count+1;
			}else{
				temp.add(videoIds[i].getVideoId());
			}
		}
		if(count==videoIds.length){
			return null;
		}else{
			return temp;
		}
	}
	
	/**
	 * 查询获取专辑树型菜单列表
	 * */
	@Override
	public String getTreeVideoAlbum()
	{
		String FlexTreeNodeString="";
		String hql ="from VideoAlbum as v order by v.createTime desc";
		List<VideoAlbum> ls=videoAlbumDao.createQuery(hql).setFirstResult(0).setMaxResults(15).list();
		FlexTreeNodeString=FlexTreeNodeString+"<Node id='albumData' label='最新专辑'>";
		FlexTreeNodeString=FlexTreeNodeString+"<Node id='all' label='全部'></Node>";
		for(int i=0;i<ls.size();i++){
			FlexTreeNodeString=FlexTreeNodeString+"<Node id='"+ls.get(i).getAlbumId()+"' label='"+ls.get(i).getAlbumName()+" ("+ls.get(i).getVideoNum()+")'></Node>";
		}
		FlexTreeNodeString=FlexTreeNodeString+"</Node>";
		return FlexTreeNodeString;
	}
	
	/**
	 * 分页查询信息
	 * @return
	 * */
	@Override
	public Finder getPageVideoAlbum(VideoAlbum videoAlbum,Integer toPage,Integer pageSize)
	{
		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);
		
		//加载所有可变的查询条件
		DetachedCriteria criteria = DetachedCriteria.forClass(VideoAlbum.class);
		if(videoAlbum!=null){
			//视频对象
			if(videoAlbum!=null){
				
				//视频标题
				if(videoAlbum.getAlbumName()!=null && videoAlbum.getAlbumName()!=""){
					criteria.add(Restrictions.like("albumName", "%"+videoAlbum.getAlbumName()+"%"));
				}
				//视频个数大于0
				if(videoAlbum.getVideoNum()== -2){
					criteria.add(Restrictions.gt("videoNum", 0));
				}
			}
			
		}
		criteria.addOrder(Order.desc("createTime"));
		finder.setTotalRows(videoAlbumDao.getRowCount(criteria));
		List<VideoAlbum> videoAlbums=videoAlbumDao.getPageObjects(criteria, toPage, finder.getPageSize());
		finder.setList(videoAlbums);
		return finder; 
	}
	
	/**
	 * 获取视频专辑列表信息
	 * @return
	 * */
	@Override
	public List<VideoAlbum> getVideoAlbum(){
		return videoAlbumDao.getAll();
	}
	
	/**
	 * 添加视频专辑信息
	 * @return
	 * */
	@Override
	public VideoAlbum addVideoAlbum(VideoAlbum videoAlbum){
		videoAlbum.setAlbumId(null);
		videoAlbum.setCreateTime(new Date());
		videoAlbum.setImagePath(videoAlbum.getImagePath());
		return videoAlbumDao.save(videoAlbum);
	}
	
	/**
	 * 修改视频专辑信息
	 * @return 
	 * */
	@Override
	public VideoAlbum updateVideoAlbum(VideoAlbum videoAlbum,Integer albumId){
		VideoAlbum videoAlbumTemp =videoAlbumDao.getById(albumId);
		if(videoAlbum.getAlbumName()!=null){
			videoAlbumTemp.setAlbumName(videoAlbum.getAlbumName());
		}
		if(videoAlbum.getVideoNum()!=0){
			videoAlbumTemp.setVideoNum(videoAlbumTemp.getVideoNum()+(videoAlbum.getVideoNum()));
		}
		if(videoAlbum.getDescn()!=null){
			videoAlbumTemp.setDescn(videoAlbum.getDescn());
		}
		if(videoAlbum.getImagePath()!=null){
			videoAlbumTemp.setImagePath(videoAlbum.getImagePath());
		}
		return videoAlbumTemp;
	}
	
	
	/**
	 * 删除视频专辑信息
	 * @return
	 * */
	@Override
	public VideoAlbum removeVideoAlbum(VideoAlbum videoAlbum)
	{
		if(videoAlbumShipDao.delAlbum(videoAlbum.getAlbumId())=="success")
		{
			return videoAlbumDao.remove(videoAlbum);
		}
		return null;
	}
}
