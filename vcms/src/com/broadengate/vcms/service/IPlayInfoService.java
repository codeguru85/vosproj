package com.broadengate.vcms.service;

import java.util.List;

import com.broadengate.vcms.entity.PlayListShip;
import com.broadengate.vcms.entity.Playlist;
import com.broadengate.vcms.entity.TermInfo;
import com.broadengate.vcms.entity.TermPlaylist;
import com.broadengate.vcms.entity.VideoInfo;
import com.broadengate.vcms.entity.VideoType;
import com.broadengate.vcms.tcp.data.auxiliary.PlayInfo;
import com.broadengate.vcms.web.Finder;

import flex.messaging.io.ArrayList;

public interface IPlayInfoService {
	//查询视频类型
	public List<VideoType> getTypeList();
	//删除播放列表
	public Playlist getDelPlay(int playId);
	//查询播放列表
	public Finder getSelectPlay(Playlist playlist,Integer toPage,Integer pageSize);	
	//根据条件查询终端
	public Finder getSelectTerm(TermInfo terminfo,Integer toPage,Integer pageSize);	
	//查询视频文件
	public Finder getSelectVideo(VideoInfo videoinfo,Integer toPage,Integer pageSize);	
	//根据类型查询视频文件
	public Finder getVideoTypes(VideoType videotype,Integer toPage,Integer pageSize);	
	//添加播放单
	public Playlist getAddPlayList(Playlist playlist,PlayListShip playlistship,TermPlaylist termplaylist,ArrayList videolist,ArrayList termlist);
	//更新playlistship表
	public PlayListShip updatePlayShip(Playlist playList,VideoInfo videoinfo,PlayListShip playShip);
	//停止播放列表
	public List stopPlay(Playlist playInfo,int state);
	//播放
	public List startPlay(Playlist playInfo,String fileName);
	public List startPlay(Playlist playInfo);
	//插播
	public void insertPlay(ArrayList videoList,Playlist playList);
	//查询播放单中视频
	public List<VideoInfo> getPlayVideo(Playlist playlist);
	//查询播放单中终端
	public List<TermInfo> getPlayTerm(Playlist playlist);
	//设置滚动字幕
	public String pushCaption(ArrayList playInfo,String caption);
	//指定下载
	public String pushDownload(ArrayList videos,ArrayList terms);
	//浏览网页
	public String viewWeb(ArrayList playInfo,String url);
	//更新播放单
	public Playlist getUpdatePlayList(Playlist playlist,PlayListShip playlistship,TermPlaylist termPlayList,ArrayList videoList,ArrayList termList);	
	//查询视频播放时间
	public String selectVideoTime(Playlist playList,VideoInfo videoinfo);
	/**
	*删除视频
	**@return
	**/
	public void delVideo(Playlist playlist,int videoid);
	/**
	*删除终端
	**@return
	**/
	public void delTerm(Playlist playlist,int termid);
	//高级查询
	public Finder getTallSelectPlay(PlayInfo playInfo,Integer toPage,Integer pageSize);
	/**
	*查询播放任务名
	**@return
	**/
	public List<Playlist> getPlayListName(String playName);
	
	public List<PlayListShip> getPlayList();
	//获取视频专辑
	public String getTreeVideoAlbum();
}
