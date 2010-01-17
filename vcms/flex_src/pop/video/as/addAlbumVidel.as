// ActionScript file
import dto.security.video.VideoAlbum;
import dto.security.video.VideoAlbumShip;
import dto.security.video.VideoAlbumShipId;
import dto.security.video.VideoInfo;

import mx.collections.ArrayCollection;
import mx.controls.Alert;
import mx.managers.PopUpManager;
import mx.rpc.events.*;

import util.page.Finder;

[Bindable]
private var videoList:Finder;
public var fatherFunction:Function;
public var albumId:int;

private var sum:int;
private var count:int;

private function closeWin():void
{
	PopUpManager.removePopUp(this);
}

private function init():void
{
	getVideoInfos();
}

/**
  * 初始化查询视频信息
  * */
private function getVideoInfos():void
{
   	var videoInfo:VideoInfo = new VideoInfo();
   	videoInfoService.getPageVideoInfo(videoInfo,1,10);
}

/**
  * 响应视频列表查询数据分页
  * */
private function getPageVideoInfoHandler(event:ResultEvent):void
{
  	videoList = event.result as Finder;
}

//搜索
private function search():void
{
	var videoInfo:VideoInfo = new VideoInfo();
	videoInfo.videoName=videoName.text;
	videoInfoService.getPageVideoInfo(videoInfo,1,10);
}

/**
 * 分页按钮调用的分页方法
 * */
public function getPaginations(toPage:int,pageSize:int):void
{
	var videoInfo:VideoInfo =new VideoInfo();
	videoInfo.videoName = videoName.text;
	videoInfoService.getPageVideoInfo(videoInfo,toPage,pageSize);
}
   
/**
 * 设置每页记录数
 * */
public function getPagesize(pageSize:String):void
{
	var videoInfo:VideoInfo = new VideoInfo();
	videoInfo.videoName = videoName.text;
	videoInfoService.getPageVideoInfo(videoInfo,1,pageSize);
}

 /**
  * 加入专辑
  * */
private function addClickHandler():void
{
 	var videoItems:Array = videoInfoDG.selectedItems;
 	if(videoItems.length<=0)
 	{
 		Alert.show("请选择你要加入的视频","提示信息");
 		return;
 	}else{
 		videoAlbumService.getIsExitsArrayVideoAlbumShip(albumId,videoItems);
 	}
}

/**
 * 检查所加入的视频在专辑内是否已经存在响应
 * */
private function getIsExitsArrayVideoAlbumShipHandler(event:ResultEvent):void
{
	var videoIds:ArrayCollection = event.result as ArrayCollection;
	if(videoIds==null){
		Alert.show("你所加入的视频在专辑中已经存在","提示信息");
		return;
	}else{
			var videoInfo:VideoInfo = new VideoInfo();
			var videoAlbum:VideoAlbum = new VideoAlbum();
 			var videoAlbumShipId:VideoAlbumShipId = new VideoAlbumShipId();
 			var videoAlbumShip:VideoAlbumShip = new VideoAlbumShip();
 			sum=videoIds.length;
 			
 			for(var i:int=0;i<videoIds.length;i++){
 				videoAlbum.albumId=albumId;
 				videoAlbumShipId.albumId = albumId;
 				videoAlbumShipId.videoId = int(videoIds[i]);
 				videoAlbumShipId.createTime = new Date();
 				videoAlbumShip.id = videoAlbumShipId;
 				videoAlbumShip.videoAlbum = videoAlbum;
 				videoAlbumShip.videoInfo =videoInfo;
				videoAlbumService.addVideoAlbumShip(videoAlbumShip);
			}
	}
}

/**
 * 确定加入专辑响应调用
 * */
private function addVideoAlbumShipHandler(event:ResultEvent):void
{
	count++;
	if(count==sum)
	{
		Alert.show("专辑加入成功","提示信息");
		var videoAlbum:VideoAlbum = new VideoAlbum();
		videoAlbum.albumId = this.albumId;
		videoAlbum.videoNum = sum;
		videoAlbumService.updateVideoAlbum(videoAlbum,albumId);
//		fatherFunction();
		count = 0;
	}
}

private function updateVideoAlbumHandler(event:ResultEvent):void{
	if(event.result){
		fatherFunction(event.result);
	}
}
	