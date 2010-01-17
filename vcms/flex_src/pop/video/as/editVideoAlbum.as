// ActionScript file
import mx.managers.PopUpManager;
import mx.rpc.events.ResultEvent;
private var editAlbumBoolean:Boolean;
import mx.controls.Alert;
import dto.security.video.VideoAlbum;

private var editAlbumFlag:Boolean=true;

public var albumId:int;

/**
 * 初始化组件调用
 * */
 private function initApp():void{
 	
 }
 
 /**
 * 修改视频专辑调用
 * */
private function updateVideoAlbum():void
{
	if(!editAlbumFlag){
		editAlbumFlag = true;
		return;
	}else{
		var videoAlbum:VideoAlbum = new VideoAlbum();
		videoAlbum.albumName = txtAlbumName.text;
		videoAlbum.descn = txtDesc.text;
		videoAlbum.imagePath=imagePath;
		videoAlbumService.updateVideoAlbum(videoAlbum,albumId);
	}
}

/**
 * 修改视频专辑调用响应函数
 * */
private function updateVideoAlbumHandler(event:ResultEvent):void
{
	if(event.result!=null){
		fatherFunction1(txtAlbumName.text,txtDesc.text);
		Alert.show("修改专辑成功","提示信息");
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