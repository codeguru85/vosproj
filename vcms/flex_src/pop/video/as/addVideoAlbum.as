// ActionScript file
import dto.security.video.VideoAlbum;

import mx.controls.Alert;
import mx.managers.PopUpManager;
import mx.rpc.events.ResultEvent;
private var addAlbumBoolean:Boolean=true;

/**
 * 初始化组件调用
 * */
 private function initApp():void{
 	
 }
 
 /**
 * 新增视频专辑调用
 * */
private function addVideoAlbum():void
{
	if(!addAlbumBoolean){
		addAlbumBoolean =true;
		return;
	}else{
		var videoAlbum:VideoAlbum =new VideoAlbum();
		videoAlbum.albumName =txtAlbumName.text;
		videoAlbum.descn = txtDesc.text;
		videoAlbum.imagePath = "images/default-album.png";
		videoAlbumService.addVideoAlbum(videoAlbum);
	}
}

/**
 * 新增视频专辑调用响应函数
 * */
private function addVideoAlbumHandler(event:ResultEvent):void
{
	var resVideoAlbum:VideoAlbum =event.result as VideoAlbum;
	if(event.result!=null){
		Alert.show("视频专辑新增成功","提示信息");
	}
	this.closeThis();
	this.fatherFunction();
}
/**
 * 关闭窗口调用
 * */
private function closeThis():void{
	PopUpManager.removePopUp(this);
}