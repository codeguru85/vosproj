package com.broadengate.vcms.service.imp;

import java.util.Date;
import java.util.List;
import java.util.Timer;

import javax.annotation.Resource;

import org.apache.commons.lang.StringUtils;
import org.hibernate.Criteria;
import org.hibernate.criterion.DetachedCriteria;
import org.hibernate.criterion.Order;
import org.hibernate.criterion.Restrictions;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import com.broadengate.core.orm.hibernate.HibernateTool;
import com.broadengate.vcms.dao.PlayListShipDao;
import com.broadengate.vcms.dao.PlaylistDao;
import com.broadengate.vcms.dao.TermInfoDao;
import com.broadengate.vcms.dao.TermPlaylistDao;
import com.broadengate.vcms.dao.VideoAlbumDao;
import com.broadengate.vcms.dao.VideoInfoDao;
import com.broadengate.vcms.dao.VideoTypeDao;
import com.broadengate.vcms.entity.PlayListShip;
import com.broadengate.vcms.entity.Playlist;
import com.broadengate.vcms.entity.TermInfo;
import com.broadengate.vcms.entity.TermPlaylist;
import com.broadengate.vcms.entity.VideoAlbum;
import com.broadengate.vcms.entity.VideoInfo;
import com.broadengate.vcms.entity.VideoType;
import com.broadengate.vcms.service.IPlayInfoService;
import com.broadengate.vcms.tcp.data.TermsClient;
import com.broadengate.vcms.tcp.data.auxiliary.CaptionResultContainer;
import com.broadengate.vcms.tcp.data.auxiliary.DownloadResultContainer;
import com.broadengate.vcms.tcp.data.auxiliary.PlayInfo;
import com.broadengate.vcms.tcp.data.auxiliary.PlayTask;
import com.broadengate.vcms.tcp.data.auxiliary.PlayTime;
import com.broadengate.vcms.tcp.data.auxiliary.UrlResultContainer;
import com.broadengate.vcms.timeJob.PlayTreeMap;
import com.broadengate.vcms.web.Finder;
import com.broadengate.vcms.web.util.TimeUtil;

import flex.messaging.io.ArrayList;

@Service
@Transactional
public class PlayInfoService implements IPlayInfoService{
	@Resource
	private PlaylistDao playlistDao;
	@Resource
	private VideoTypeDao videotypeDao;
	@Resource
	private TermInfoDao terminfoDao;
	@Resource
	private VideoInfoDao videoinfoDao;	
	@Resource
	private PlayListShipDao playlistshipDao;
	@Resource
	private TermPlaylistDao termplaylistDao;
	@Resource
	private VideoAlbumDao videoAlbumDao;	
	/**
	*查询视频类型
	**@return
	**/
	public List<VideoType> getTypeList(){
		String hql = "select typeName from VideoType";
		return videotypeDao.find(hql);
	}
	
	/**
	*获取视频专辑
	**@return
	**/
	@Override
	public String getTreeVideoAlbum()
	{
		String FlexTreeNodeString="";
		String hql ="from VideoAlbum as v order by v.createTime desc";
		List<VideoAlbum> ls=videoAlbumDao.find(hql);
		FlexTreeNodeString=FlexTreeNodeString+"<Node id='albumData' label='视频专辑'>";
		FlexTreeNodeString=FlexTreeNodeString+"<Node id='all' label='全部'></Node>";
		for(int i=0;i<ls.size();i++){
			FlexTreeNodeString=FlexTreeNodeString+"<Node id='"+ls.get(i).getAlbumId()+"' label='"+ls.get(i).getAlbumName()+" ("+ls.get(i).getVideoNum()+")'></Node>";
		}
		//FlexTreeNodeString=FlexTreeNodeString+"<Node id='more' label='更多(Ctrl+Q)'></Node>";
		FlexTreeNodeString=FlexTreeNodeString+"</Node>";
		return FlexTreeNodeString;
	}
	
	/**
	*查询PlayListShip表
	**@return
	**/
	@Override

	public List getPlayList(){
		return playlistshipDao.getList();

	}
	
	/**
	*查询播放任务名
	**@return
	**/
	public List<Playlist> getPlayListName(String playName){
		String hql = "from Playlist where listName='"+playName+"'";
		return playlistDao.find(hql);
	}
	
	/**
	*查询播放列表
	**@return
	**/
	public Finder getSelectPlay(Playlist playlist,Integer toPage,Integer pageSize){
		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);
		DetachedCriteria criteria = DetachedCriteria.forClass(Playlist.class);
		if(StringUtils.isNotEmpty(playlist.getListName())){
			criteria.add(Restrictions.like("listName", "%"+playlist.getListName()+"%"));
		}			
		finder.setTotalRows(playlistDao.getRowCount(criteria));
		List<Playlist> playlistpage = playlistDao.getPageObjects(criteria, toPage, finder.getPageSize());
		finder.setList(playlistpage);
		return finder;
	}	
	
	//高级查询
	public Finder getTallSelectPlay(PlayInfo playInfo,Integer toPage,Integer pageSize){
		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);
		DetachedCriteria criteria = DetachedCriteria.forClass(Playlist.class);
		if(StringUtils.isNotEmpty(playInfo.getListName())){
			criteria.add(Restrictions.like("listName", "%"+playInfo.getListName()+"%"));
		}
		HibernateTool.getQueryDateRange(playInfo.getStartDate(), playInfo.getEndDate(), "createTime", criteria);	
		finder.setTotalRows(playlistDao.getRowCount(criteria));
		List<Playlist> playlistpage = playlistDao.getPageObjects(criteria, toPage, finder.getPageSize());
		finder.setList(playlistpage);
		return finder;
	}		
	
	/**
	*根据条件查询终端
	**@return
	**/
	public Finder getSelectTerm(TermInfo terminfo,Integer toPage,Integer pageSize){
		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);
		DetachedCriteria criteria = DetachedCriteria.forClass(TermInfo.class);
		if(StringUtils.isNotEmpty(terminfo.getTermName())){
			criteria.add(Restrictions.like("termName", "%"+terminfo.getTermName()+"%"));
		}
		if(StringUtils.isNotEmpty(terminfo.getTermIp())){
			criteria.add(Restrictions.like("termIp", terminfo.getTermIp()));			
		}
		if(StringUtils.isNotEmpty(terminfo.getTermAddr())){
			criteria.add(Restrictions.like("termAddr", "%"+terminfo.getTermAddr()+"%"));
		}
		if(terminfo.getTermActivation()==1){
			criteria.add(Restrictions.eq("termActivation", 1));
		}
		finder.setTotalRows(terminfoDao.getRowCount(criteria));
		List<TermInfo> termpage = terminfoDao.getPageObjects(criteria, toPage, finder.getPageSize());
		finder.setList(termpage);
		return finder;
	}
	
	/**
	*删除播放单
	**@return
	**/
	@Override
	public Playlist getDelPlay(int playId){
		String hql = "from PlayListShip p where p.playlist.listId='"+playId+"'";
		List<PlayListShip> seqArray = playlistshipDao.find(hql);
		for(int s=0;s<seqArray.size();s++){
			PlayListShip seqKey = seqArray.get(s);
			PlayTreeMap playTreeMap = new PlayTreeMap();
			Date nowDate = new Date();
			Date dt = (Date)playTreeMap.parse(seqKey.getListSeq());
			if(dt.after(nowDate)){
				playTreeMap.delPlayTreeMap(seqKey.getListSeq(), playId);
			}
		}
		return playlistDao.removeById(playId);
	}
	
	/**
	*查询视频文件
	**@return
	**/
	public Finder getSelectVideo(VideoInfo videoinfo,Integer toPage,Integer pageSize){
		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);
		DetachedCriteria criteria = DetachedCriteria.forClass(VideoInfo.class);
		if(StringUtils.isNotEmpty(videoinfo.getVideoName())){
			criteria.add(Restrictions.like("videoName", "%"+videoinfo.getVideoName()+"%"));
		}
		criteria.addOrder(Order.desc("createTime"));
		finder.setTotalRows(videoinfoDao.getRowCount(criteria));
		List<VideoInfo> videopage = videoinfoDao.getPageObjects(criteria, toPage, finder.getPageSize());
		finder.setList(videopage);
		return finder;
	}	
		
	/**
	*根据类型查询视频文件
	**@return
	**/
	public Finder getVideoTypes(VideoType videotype,Integer toPage,Integer pageSize){
		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);
		DetachedCriteria criteria = DetachedCriteria.forClass(VideoInfo.class);
		criteria.createAlias("videoTypes", "types", DetachedCriteria.LEFT_JOIN);
		criteria.add(Restrictions.eq("types.typeId", videotype.getTypeId()));
		criteria.addOrder(Order.desc("createTime"));
		finder.setTotalRows(videoinfoDao.getRowCount(criteria));
		List<VideoInfo> types = videoinfoDao.getPageObjects(criteria, toPage, finder.getPageSize());
		finder.setList(types);
		return finder;
	}
	
	/**
	*查询播放单详细信息
	**@return
	**/
	public List<VideoInfo> getPlayVideo(Playlist playlist){
		Criteria criteria = videoinfoDao.createCriteria();		
		criteria.createAlias("playListShips", "playship", Criteria.INNER_JOIN);
		criteria.createAlias("playListShips.playlist", "playlist", Criteria.INNER_JOIN);
		criteria.add(Restrictions.eq("playship.playlist.listId", playlist.getListId()));
		criteria.setResultTransformer(Criteria.DISTINCT_ROOT_ENTITY);
		List<VideoInfo> playlistsship = criteria.list();
		return playlistsship;
	} 
	
	public List<TermInfo> getPlayTerm(Playlist playlist){
		Criteria criteria = terminfoDao.createCriteria();
		criteria.createAlias("termPlaylists", "termship", Criteria.LEFT_JOIN);
		criteria.add(Restrictions.eq("termship.playlist.listId", playlist.getListId()));
		criteria.setResultTransformer(Criteria.DISTINCT_ROOT_ENTITY);
		List<TermInfo> termlistsship = criteria.list();
		return termlistsship;
	}	
	
	/**
	*添加播放单
	**@return
	**/
	@Override
	public Playlist getAddPlayList(Playlist playlist,PlayListShip playlistship,TermPlaylist termPlayList,ArrayList videoList,ArrayList termList){
		//播放列表 
		Playlist playList = new Playlist();
		playList.setListName(playlist.getListName());
		playList.setStartTime(playlist.getStartTime());
		playList.setEndTime(playlist.getEndTime());
		playList.setCreateTime(playlist.getCreateTime());
		playList.setVideoNum(playlist.getVideoNum());
		playList.setPlayState(playlist.getPlayState());
		Playlist hadSavedPlaylist = playlistDao.save(playList);
		
		//视频
		PlayListShip playListShip  = null;
		PlayListShip pls = null;
		for(int j=0;j<videoList.size();j++){	
			playListShip  = new PlayListShip();
			PlayTime videoTime = (PlayTime) videoList.get(j);
			int videoid = videoTime.getVideoId();
			VideoInfo tempVideoInfo = videoinfoDao.getById(videoid);
			playListShip.setPlaylist(hadSavedPlaylist);
			playListShip.setVideoInfo(tempVideoInfo);
			playListShip.setCreateTime(playlistship.getCreateTime());
			playListShip.setListSeq(videoTime.getPlayTime());				
			playListShip.setShipId(null);
			pls = playlistshipDao.save(playListShip);
			playlistshipDao.flush();
			hadSavedPlaylist.getPlayListShips().add(pls);			
			playlistDao.flush();
		}	
		
		//终端
		TermPlaylist termship  = null;
		TermPlaylist tempTermPlaylist = null;
		//得到需要放入的终端信息
		for(int i=0;i<termList.size();i++){
			termship  = new TermPlaylist();
			int termid = Integer.parseInt(termList.get(i).toString());
			TermInfo tempTrminfo = terminfoDao.getById(termid);
			termship.setPlaylist(hadSavedPlaylist);
			termship.setTermInfo(tempTrminfo);
			termship.setCreateTime(termPlayList.getCreateTime());	
			termship.setTermListId(null);
			tempTermPlaylist = termplaylistDao.save(termship);
			termplaylistDao.flush();
			hadSavedPlaylist.getTermPlaylists().add(tempTermPlaylist);
			playlistDao.flush();
		}		
		PlayTreeMap playTreeMap = new PlayTreeMap();
		playTreeMap.addPlayTreeMap(videoList, termship.getPlaylist().getListId());
		return hadSavedPlaylist;
	}
	
	/**
	*更新播放单
	**@return
	**/
	@Override
	public Playlist getUpdatePlayList(Playlist playlist,PlayListShip playlistship,TermPlaylist termPlayList,ArrayList videoList,ArrayList termList){
		//删除内在表中的数据
		String hql = "from PlayListShip p where p.playlist.listId='"+playlist.getListId()+"'";
		List<PlayListShip> seqArray = playlistshipDao.find(hql);
		for(int s=0;s<seqArray.size();s++){
			PlayListShip seqKey = seqArray.get(s);
			PlayTreeMap playTreeMap = new PlayTreeMap();
			Date nowDate = new Date();
			Date dt = (Date)playTreeMap.parse(seqKey.getListSeq());
			if(dt.after(nowDate)){
				playTreeMap.delPlayTreeMap(seqKey.getListSeq(), playlist.getListId());
			}
		}		
		
		//播放列表 
		Playlist playList = playlistDao.getById(playlist.getListId());
		playList.setListName(playlist.getListName());
		playList.setStartTime(playlist.getStartTime());
		playList.setEndTime(playlist.getEndTime());
		playList.setCreateTime(playlist.getCreateTime());
		playList.setVideoNum(playlist.getVideoNum());
		playList.setPlayState(playlist.getPlayState());
		Playlist hadSavedPlaylist = playlistDao.save(playList);
		
		//视频
		PlayListShip playListShip  = null;
		PlayListShip pls = null;
		for(int j=0;j<videoList.size();j++){			
			PlayTime videoTime = (PlayTime) videoList.get(j);
			int videoid = videoTime.getVideoId();
			String videoHql = "select shipId from PlayListShip p where p.playlist.listId = '"+playList.getListId()+"' and p.videoInfo.videoId='"+videoid+"'";
			List<PlayListShip> videoLists = playlistshipDao.find(videoHql);
			if(videoLists.size()>0){
				playListShip = playlistshipDao.getById(videoLists.get(0));
			}else{
				playListShip  = new PlayListShip();
				playListShip.setShipId(null);
			}			
			VideoInfo tempVideoInfo = videoinfoDao.getById(videoid);
			playListShip.setPlaylist(hadSavedPlaylist);
			playListShip.setVideoInfo(tempVideoInfo);
			playListShip.setCreateTime(playlistship.getCreateTime());
			playListShip.setListSeq(videoTime.getPlayTime());
			pls = playlistshipDao.save(playListShip);
			playlistshipDao.flush();
			hadSavedPlaylist.getPlayListShips().add(pls);			
			playlistDao.flush();
		}
		
		//终端
		TermPlaylist termship  = null;
		TermPlaylist tempTermPlaylist = null;
		//得到需要放入的终端信息
		for(int i=0;i<termList.size();i++){			
			int termid = Integer.parseInt(termList.get(i).toString());
			String termHql = "select termListId from TermPlaylist p where p.playlist.listId = '"+playList.getListId()+"' and p.termInfo.termId='"+termid+"'";
			List<TermPlaylist> termLists = termplaylistDao.find(termHql);
			if(termLists.size()>0){
				termship  = termplaylistDao.getById(termLists.get(0));
			}else{
				termship  = new TermPlaylist();
				termship.setTermListId(null);
			}			
			TermInfo tempTrminfo = terminfoDao.getById(termid);
			termship.setPlaylist(hadSavedPlaylist);
			termship.setTermInfo(tempTrminfo);
			termship.setCreateTime(termPlayList.getCreateTime());	
			tempTermPlaylist = termplaylistDao.save(termship);
			termplaylistDao.flush();
			hadSavedPlaylist.getTermPlaylists().add(tempTermPlaylist);
			playlistDao.flush();
		}
		PlayTreeMap playTreeMap = new PlayTreeMap();
		playTreeMap.addPlayTreeMap(videoList, playList.getListId());
		return hadSavedPlaylist;
	}
	
	/**
	*删除视频
	**@return
	**/
	public void delVideo(Playlist playlist,int videoid){
		String videoHql = "select shipId from PlayListShip p where p.playlist.listId = '"+playlist.getListId()+"' and p.videoInfo.videoId='"+videoid+"'";
		List<PlayListShip> videoLists = playlistshipDao.find(videoHql);
		if(videoLists.size()>0){
			playlistshipDao.removeById(videoLists.get(0));
		}		
	}
	
	/**
	*删除终端
	**@return
	**/
	public void delTerm(Playlist playlist,int termid){
		String termHql = "select termListId from TermPlaylist p where p.playlist.listId = '"+playlist.getListId()+"' and p.termInfo.termId='"+termid+"'";
		List<TermPlaylist> termLists = termplaylistDao.find(termHql);
		if(termLists.size()>0){
			termplaylistDao.removeById(termLists.get(0));
		}		
	}
	
	//更新playlistship表
	public PlayListShip updatePlayShip(Playlist playList,VideoInfo videoinfo,PlayListShip playShip){
		String hql = "select shipId from PlayListShip p where p.playlist.listId = '"+playList.getListId()+"' and p.videoInfo.videoId='"+videoinfo.getVideoId()+"'";
		List<PlayListShip> lists = playlistshipDao.find(hql);
		 PlayListShip ship = playlistshipDao.getById(lists.get(0));		 
		 ship.setListSeq(playShip.getListSeq());
		 ship.setCreateTime(playShip.getCreateTime());
		 return ship;
	} 
	
	//查询视频播放时间
	public String selectVideoTime(Playlist playList,VideoInfo videoinfo){
		String time = "";
		String hql = "select listSeq from PlayListShip p where p.playlist.listId = '"+playList.getListId()+"' and p.videoInfo.videoId='"+videoinfo.getVideoId()+"'";
		List listTime = playlistshipDao.find(hql);
		if(listTime.get(0)!=null){
			time = listTime.get(0).toString();
		}else{
			time = "";
		}
		return time;
	} 
	
	/**
	*播放播放单
	**@return
	**/
	@Override
	public List startPlay(Playlist playInfo,String fileName){
		
		List reutrnMsgs =null;
		List<VideoInfo> playListsShip = this.getPlayVideo(playInfo);
		List<TermInfo> termListsShip = this.getPlayTerm(playInfo);
		String hql = " from VideoInfo where fileName='"+fileName+"'";
		List<VideoInfo> videoInfoList = videoinfoDao.find(hql);
		String filePath="";
		for(int i=0;i<videoInfoList.size();i++){
			VideoInfo videoInfo = videoInfoList.get(i);
			filePath = videoInfo.getFilePath();
		}
		/**
		 * 判断软件是否已经到了有效期
		 * */
//		String currDate=TimeUtil.getFormatDateFileName(new Date());
//		Boolean flag=TimeUtil.compareDateTime(currDate, "20100113000000");
//		if(flag){
			TermsClient tc = new TermsClient();
			reutrnMsgs = tc.playMsgTalk(termListsShip, fileName,playListsShip.size(),filePath);
//		}
		return reutrnMsgs;
	}
	
	public List startPlay(Playlist playInfo){
		//删除PlayTreeMap中的时间数据
		String hql = "from PlayListShip p where p.playlist.listId='"+playInfo.getListId()+"'";
		List<PlayListShip> seqArray = playlistshipDao.find(hql);
		for(int s=0;s<seqArray.size();s++){
			PlayListShip seqKey = seqArray.get(s);
			PlayTreeMap playTreeMap = new PlayTreeMap();
			Date nowDate = new Date();
			Date dt = (Date)playTreeMap.parse(seqKey.getListSeq());
			if(dt.after(nowDate)){
				playTreeMap.delPlayTreeMap(seqKey.getListSeq(), playInfo.getListId());
			}
		}	
		List reutrnMsgs =null;
		VideoInfo videoInfo = new VideoInfo();
		Date dt = null;
		Date endTime = null;
		ArrayList playTimeList = new ArrayList();
		List<TermInfo> termListsShip = this.getPlayTerm(playInfo);
		List<VideoInfo> videoList = this.getPlayVideo(playInfo);
		//Timer timer = new Timer();
		for(int i=0;i<videoList.size();i++){
			videoInfo = videoList.get(i);
			String timeCount = videoInfo.getTimeCount();
			PlayTime videoTime = new PlayTime();			
			videoTime.setPlayFilePath(videoInfo.getFilePath());
			videoTime.setPlayVideoName(videoInfo.getFileName());
			videoTime.setTimeCount(timeCount);
			videoTime.setVideoId(videoInfo.getVideoId());
			int period = (Integer.parseInt(timeCount.substring(0, 2))*60*60+Integer.parseInt(timeCount.substring(3, 5))*60+Integer.parseInt(timeCount.substring(6, 8)))*1000;
			if(i == 0){
				dt = new Date();
				TermsClient tc = new TermsClient();
				reutrnMsgs = tc.playMsgTalk(termListsShip, videoInfo.getFileName(),videoList.size(),videoInfo.getFilePath());
			}else{
				dt = endTime;
			}
			videoTime.setPlayTime(TimeUtil.formatTimestamp.format(dt));
			endTime = new Date(dt.getTime()+ period);
			playTimeList.add(videoTime);			
		}
		PlayTreeMap playTreeMap = new PlayTreeMap();
		playTreeMap.addPlayTreeMap(playTimeList, playInfo.getListId());
		return reutrnMsgs;
	}

	/**
	*插播
	**@return
	**/
	@Override
	public void insertPlay(ArrayList videoList,Playlist playList){
		PlayTime videoTime = new PlayTime();
		List<TermInfo> termListsShip = this.getPlayTerm(playList);
		Timer timer = new Timer();
		for(int i=0;i<videoList.size();i++){
			videoTime = (PlayTime) videoList.get(i);
			String timeCount = videoTime.getTimeCount();
			int period = (Integer.parseInt(timeCount.substring(0, 2))*60*60+Integer.parseInt(timeCount.substring(3, 5))*60+Integer.parseInt(timeCount.substring(6, 8)))*1000;
			PlayTask task = new PlayTask(termListsShip,videoTime.getPlayVideoName(),videoList.size(),videoTime.getPlayFilePath());
			timer.schedule(task, 2, period);	
		}
	}
		
	/**
	*停止播放单
	**@return
	**/
	@Override
	public List stopPlay(Playlist playInfo,int state){
		
		List reutrnMsgs =null;
		List<TermInfo> termlistsship = this.getPlayTerm(playInfo);
		/**
		 * 判断软件是否已经到了有效期
		 * */
//		String currDate=TimeUtil.getFormatDateFileName(new Date());
//		Boolean flag=TimeUtil.compareDateTime(currDate, "20100113000000");
//		if(flag){
			TermsClient tc = new TermsClient();
			reutrnMsgs =  tc.stopMsgTalk(termlistsship,state);
//		}
		return reutrnMsgs;
	}	
	
	/**
	 * 设置滚动字幕
	 */
	public String pushCaption(ArrayList terms,String caption)
	{
		TermsClient tc = new TermsClient();
		List<CaptionResultContainer> returnMsgs = tc.captionMsgTalk(terms, caption);
		
//		if(returnMsgs==null){
//			return "试用期结束";
//		}
		String result="";
		//0:无法链接   1：服务端不存在   2：下载成功   3：找不到IP  4：读超时
		if(returnMsgs.get(0).getReturnMark()=="3")
		{
			result="找不到IP";
		}
		else
		{
			String result0="",result1="",result2="",result4="";
			int j0=0,j1=0,j2=0,j4=0;
			for(int i=0;i<returnMsgs.size();i++)
			{
				if(returnMsgs.get(i).getReturnMark()=="2")
				{
					result2+=returnMsgs.get(i).getClientName();
					result2+="  ";
					j2++;
					if(j2%10==0)result2+='\n';
				}
				if(returnMsgs.get(i).getReturnMark()=="0")
				{
					result0+=returnMsgs.get(i).getClientName();
					result0+="  ";
					j0++;
					if(j0%10==0)result0+='\n';
				}
				if(returnMsgs.get(i).getReturnMark()=="1")
				{
					result1+=returnMsgs.get(i).getClientName();
					result1+="  ";
					j1++;
					if(j1%10==0)result1+='\n';
				}
				if(returnMsgs.get(i).getReturnMark()=="4")
				{
					result4+=returnMsgs.get(i).getClientName();
					result4+="  ";
					j4++;
					if(j4%10==0)result4+='\n';
				}
			}
			if(result2!="")
				result+="设置字幕成功："+'\n'+result2;
			if(result0!=""||result1!=""||result4!="")
				result+='\n'+"设置字幕失败:";
			if(result0!="")
				result+='\n'+"无法链接:"+'\n'+result0;
			if(result1!="")
				result+='\n'+"服务端不存在:"+'\n'+result1;
			if(result4!="")
				result+='\n'+"读超时:"+'\n'+result4;
		}
		return result;
	}
	
	/**
	 * 指定下载
	 */
	public String pushDownload(ArrayList videos,ArrayList terms)
	{
		TermsClient tc = new TermsClient();
		List<DownloadResultContainer> returnMsgs = tc.downloadMsgTalk(videos, terms);

//		if(returnMsgs==null)
//		{
//			return "试用期结束";
//		}
		String result="";
		//0:无法链接   1：服务端不存在   2：下载成功   3：找不到IP  4：读超时
		if(returnMsgs.get(0).getReturnMark()=="3")
		{
			result="找不到IP";
		}
		else
		{
			String result0="",result1="",result2="",result4="";
			int j0=0,j1=0,j2=0,j4=0;
			for(int i=0;i<returnMsgs.size();i++)
			{
				if(returnMsgs.get(i).getReturnMark()=="2")
				{
					result2+=returnMsgs.get(i).getClientName();
					result2+="  ";
					j2++;
					if(j2%10==0)result2+='\n';
				}
				if(returnMsgs.get(i).getReturnMark()=="0")
				{
					result0+=returnMsgs.get(i).getClientName();
					result0+="  ";
					j0++;
					if(j0%10==0)result0+='\n';
				}
				if(returnMsgs.get(i).getReturnMark()=="1")
				{
					result1+=returnMsgs.get(i).getClientName();
					result1+="  ";
					j1++;
					if(j1%10==0)result1+='\n';
				}
				if(returnMsgs.get(i).getReturnMark()=="4")
				{
					result4+=returnMsgs.get(i).getClientName();
					result4+="  ";
					j4++;
					if(j4%10==0)result4+='\n';
				}
			}
			if(result2!="")
				result+="指定下载成功："+'\n'+result2;
			if(result0!=""||result1!=""||result4!="")
				result+='\n'+"指定下载失败:";
			if(result0!="")
				result+='\n'+"无法链接:"+'\n'+result0;
			if(result1!="")
				result+='\n'+"服务端不存在:"+'\n'+result1;
			if(result4!="")
				result+='\n'+"读超时:"+'\n'+result4;
		}
		return result;
	}
	
	/**
	 * 浏览网页
	 */
	public String viewWeb(ArrayList terms,String url)
	{
		TermsClient tc = new TermsClient();
		List<UrlResultContainer> returnMsgs = tc.urlMsgTalk(terms, url);
		
		String result="";
		//0:无法链接   1：服务端不存在   2：下载成功   3：找不到IP  4：读超时
		if(returnMsgs.get(0).getReturnMark()=="3")
		{
			result="找不到IP";
		}
		else
		{
			String result0="",result1="",result2="",result4="";
			int j0=0,j1=0,j2=0,j4=0;
			for(int i=0;i<returnMsgs.size();i++)
			{
				if(returnMsgs.get(i).getReturnMark()=="2")
				{
					result2+=returnMsgs.get(i).getClientName();
					result2+="  ";
					j2++;
					if(j2%10==0)result2+='\n';
				}
				if(returnMsgs.get(i).getReturnMark()=="0")
				{
					result0+=returnMsgs.get(i).getClientName();
					result0+="  ";
					j0++;
					if(j0%10==0)result0+='\n';
				}
				if(returnMsgs.get(i).getReturnMark()=="1")
				{
					result1+=returnMsgs.get(i).getClientName();
					result1+="  ";
					j1++;
					if(j1%10==0)result1+='\n';
				}
				if(returnMsgs.get(i).getReturnMark()=="4")
				{
					result4+=returnMsgs.get(i).getClientName();
					result4+="  ";
					j4++;
					if(j4%10==0)result4+='\n';
				}
			}
			if(result2!="")
				result+="设置网址成功："+'\n'+result2;
			if(result0!=""||result1!=""||result4!="")
				result+='\n'+"设置网址失败:";
			if(result0!="")
				result+='\n'+"无法链接:"+'\n'+result0;
			if(result1!="")
				result+='\n'+"服务端不存在:"+'\n'+result1;
			if(result4!="")
				result+='\n'+"读超时:"+'\n'+result4;
		}
		return result;
	}
}

