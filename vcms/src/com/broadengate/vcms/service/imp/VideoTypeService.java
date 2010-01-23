/**Administrator
 * 下午03:34:54
 */
package com.broadengate.vcms.service.imp;

import java.util.Date;
import java.util.List;
import java.util.Set;

import javax.annotation.Resource;

import org.apache.commons.lang.StringUtils;
import org.hibernate.Criteria;
import org.hibernate.criterion.DetachedCriteria;
import org.hibernate.criterion.Order;
import org.hibernate.criterion.Restrictions;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import com.broadengate.vcms.dao.VideoTypeDao;
import com.broadengate.vcms.entity.VideoInfo;
import com.broadengate.vcms.entity.VideoType;
import com.broadengate.vcms.service.IVideoTypeService;
import com.broadengate.vcms.web.Finder;

/**
 * @author kevin
 */
// Spring Service Bean的标识.
// 默认将类中的所有函数纳入事务管理.
@Service
@Transactional
public class VideoTypeService implements IVideoTypeService{
	@Resource
	private VideoTypeDao videoTypeDao;
	
	/**
	 * @param videoTypeDao the videoTypeDao to set
	 */
	public void setVideoTypeDao(VideoTypeDao videoTypeDao) {
		this.videoTypeDao = videoTypeDao;
	}

	/**
	 * 定义返回所有视频分类的树型菜单
	 * */
	@Override
	public String getTreeVideoType(){
		String FlexTreeNodeString="";
		String hql="from VideoType as v  where v.videoType.typeId IS NULL order by v.createTime desc"; 
		List<VideoType> list = videoTypeDao.find(hql);
		
		FlexTreeNodeString =FlexTreeNodeString+"<Node id='typeDate' label='视频分类' selected='false'>";
		for(int i=0;i<list.size();i++)
		{
			FlexTreeNodeString=FlexTreeNodeString+"<Node id='"+list.get(i).getTypeId()+"' label='"+list.get(i).getTypeName()+" ("+list.get(i).getVideoNum()+")' num='"+list.get(i).getVideoNum()+"' type='-1' datetime='"+list.get(i).getCreateTime()+"' selected='false'>";
			FlexTreeNodeString=FlexTreeNodeString+getVideoTypeByParentID(list.get(i).getTypeId());
			FlexTreeNodeString=FlexTreeNodeString+"</Node>";
		}
		FlexTreeNodeString=FlexTreeNodeString+"</Node>";
		return FlexTreeNodeString;
	}
	
	/**
	 * 定义返回指定父ID分类的XML字符串
	 * */
	public String getVideoTypeByParentID(Integer Parentid){
		String curFlexTreeNodeString="";
		String strsql="from VideoType as v where v.videoType.typeId ="+Parentid+" order by v.createTime desc";
		List<VideoType> lss =videoTypeDao.find(strsql);
		for(int i=0;i<lss.size();i++){
			curFlexTreeNodeString=curFlexTreeNodeString+"<Node id='"+lss.get(i).getTypeId()+"'  label='"+lss.get(i).getTypeName()+" ("+lss.get(i).getVideoNum()+")' type='"+lss.get(i).getVideoType().getTypeId()+"' num='"+lss.get(i).getVideoNum()+"' datetime='"+lss.get(i).getCreateTime()+"' selected='false'>";
			curFlexTreeNodeString=curFlexTreeNodeString+getVideoTypeByParentID(lss.get(i).getTypeId());
			curFlexTreeNodeString=curFlexTreeNodeString+"</Node>";
		}
		return curFlexTreeNodeString;
	}
	
	/**
	 * 查询所有根视频分类
	 * */
	@Override
	public List<VideoType> getRootVideoTypes()
	{
		String hql="from VideoType as v ";
		List<VideoType> list = videoTypeDao.find(hql);
		return list;
	}
	
	/**
	 * 查询根视频分类下面的所有子分类
	 * */
	@Override
	public List<VideoType>getParentVideoTypes(Integer typeId)
	{
		String hql="from VideoType as v  where v.videoType.typeId="+typeId+"";
		List<VideoType> list = videoTypeDao.find(hql);
		return list; 
	}
	
	/**
	 * 查询所有的视频分类列表
	 * @return
	 * */
	@Override
	public List<VideoType> getVideoTypes(VideoType videoType){
		Criteria criteria = videoTypeDao.createCriteria();
		if(StringUtils.isNotEmpty(videoType.getTypeName())){
			criteria.add(Restrictions.like("typeName", "%"+videoType.getTypeName()+"%"));
		}
		return criteria.list();
	}
	
	/**
	 * 获取分页查询视频分类列表
	 * @return
	 * */
	@Override
	public Finder getPageVideoType(VideoType videoType,Integer toPage,Integer pageSize){
		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);
		
		//加载所有可变的查询条件
		DetachedCriteria criteria = DetachedCriteria.forClass(VideoType.class);
		
		if(videoType!=null){
			//分类名称
			if(videoType.getTypeName()!=null && videoType.getTypeName()!=""){
				criteria.add(Restrictions.like("typeName", "%"+videoType.getTypeName()+"%"));
			}
		}
		criteria.addOrder(Order.desc("createTime"));
		finder.setTotalRows(videoTypeDao.getRowCount(criteria));
		List<VideoType> videoTypes=videoTypeDao.getPageObjects(criteria, toPage, finder.getPageSize());
		finder.setList(videoTypes);
		return finder;
	}
	
	/**
	 * 快捷窗口获取视频分类列表
	 * @return
	 * */
	@Override
	public Finder getQueryVideoType(VideoType videoType,Integer toPage,Integer pageSize){
		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);
		
		//加载所有可变的查询条件
		DetachedCriteria criteria = DetachedCriteria.forClass(VideoType.class);
		
		if(videoType!=null){
			//分类名称
			if(videoType.getTypeName()!=null && videoType.getTypeName()!=""){
				criteria.add(Restrictions.like("typeName", "%"+videoType.getTypeName()+"%"));
			}
		}
		
		finder.setTotalRows(videoTypeDao.getRowCount(criteria));
		List<VideoType> videoTypes=videoTypeDao.getPageObjects(criteria, toPage, finder.getPageSize());
		finder.setList(videoTypes);
		return finder;
	}
	
	/**
	 * 设置每页显示数据条数
	 * */
	@Override
	public Finder getPagesize(Integer pageSize){
		Finder finder = new Finder();
		finder.setPageSize(pageSize);
		
		
		DetachedCriteria criteria = DetachedCriteria.forClass(VideoType.class);
		List<VideoType> customers = videoTypeDao.getPageObjects(criteria, 1, finder.getPageSize());
		finder.setTotalRows(videoTypeDao.getRowCount(criteria));
		finder.setList(customers);
		return finder;
	}
	/**
	 * 添加视频分类信息
	 * @return
	 * */
	@Override
	public VideoType addVideoType(VideoType videoType,Integer txtTypeId){
		if(txtTypeId==-1){
			videoType.setTypeId(null);
			videoType.setVideoNum(0);
			videoType.setCreateTime(new Date());
		}else{
			videoType.setTypeId(null);
			VideoType v = new VideoType();
			v.setTypeId(txtTypeId);
			videoType.setVideoType(v);
			videoType.setVideoNum(0);
			videoType.setCreateTime(new Date());
		}
		return videoTypeDao.save(videoType);
	}
	
	/**
	 * 修改视频分类信息
	 * @return
	 * */
	@Override
	public VideoType updateVideoType(VideoType videoType,Integer txtTypeId){
		VideoType videoTypeTemp=videoTypeDao.getById(videoType.getTypeId());
		VideoType rootType = videoTypeTemp.getVideoType();
		int num = videoTypeTemp.getVideoNum();
		boolean flag = false; 
		if(rootType!=null && (int)txtTypeId==(int)rootType.getTypeId()){
			videoTypeTemp.setTypeName(videoType.getTypeName());
		}else{
			if(txtTypeId==-1){
				videoTypeTemp.setVideoType(null);
				videoTypeTemp.setTypeName(videoType.getTypeName());
			}else{					
				//videoType.setVideoType(videoType);
				videoTypeTemp.setTypeName(videoType.getTypeName());
				videoType.setTypeId(txtTypeId);
				videoTypeTemp.setVideoType(videoType);
			}
			while(rootType!=null){
				if((int)rootType.getTypeId() == (int)txtTypeId){
					flag = true;
					break;
				}
				rootType.setVideoNum(rootType.getVideoNum()-num);
				rootType = rootType.getVideoType();
			}
 
			videoType = videoTypeDao.getById(txtTypeId);
			while(videoType!=null && !flag){
				videoType.setVideoNum(videoType.getVideoNum()+num);
				videoType = videoType.getVideoType();
			}
		}
		return videoTypeTemp;
	}
	
	/**
	 * 删除视频分类信息
	 * @return
	 * */
	@Override
	public VideoType removeVideoType(VideoType videoType){	
		return videoTypeDao.remove(videoType);
	}
	
	public VideoType updateVideoNum(VideoType videoType,Integer txtTypeId){
		VideoType rootType = videoTypeDao.getById(txtTypeId);
		while(rootType!=null){
			rootType.setVideoNum(rootType.getVideoNum()-videoType.getVideoNum());
			rootType = rootType.getVideoType();
		}
		return videoType;
	}
}
