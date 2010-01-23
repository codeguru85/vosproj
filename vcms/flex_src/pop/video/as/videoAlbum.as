// ActionScript file
import dto.security.video.VideoAlbum;

import mx.controls.Alert;
import mx.events.ItemClickEvent;
import mx.managers.PopUpManager;
import mx.rpc.events.ResultEvent;

import pop.video.AddVideoAlbum;
import pop.video.EditVideoAlbum;

import util.page.Finder;

/**
 * 视频专辑集合
 * */
[Bindable]
private var videoAlbums:Finder;

/**
 * 设置每页默认记录数
 * */
private var thisPageSize:String = '8';

/**
 * 组件初始化调用
 * */
private function onCreationComplete():void
{
	this.getVideoAlbums();
}

/**
 * 关闭窗口调用
 * */
private function closeThis():void
{
	PopUpManager.removePopUp(this);
	this.fatherFunction();
}

/**
 * 初始化数据查询调用
 * */
 private function getVideoAlbums():void{
 	videoAlbumService.getPageVideoAlbum(null,1,8);	
 }

/**
 * 分页查询响应调用
 * */
private function getPageVideoAlbumHandler(event:ResultEvent):void
{
	videoAlbums = event.result as Finder;
}

/**
 * 查询条件设置
 * */
 private function getVideoAlbumByParaments():void{
 	var videoAlbum:VideoAlbum =new VideoAlbum();
 	videoAlbum.albumName=txtAlbumName.text;
 	videoAlbumService.getPageVideoAlbum(videoAlbum,1,parseInt(thisPageSize));
 }

/**
 * 分页显示查询调用
 * */
public function getPaginations(toPage:int,pageSize:int):void
{
	var videoAlbum:VideoAlbum = new VideoAlbum();
	videoAlbum.albumName=txtAlbumName.text;
	videoAlbumService.getPageVideoAlbum(videoAlbum,toPage,pageSize);
}

/**
 *按钮组相应事件 
 * */
private function clickHandler(event:ItemClickEvent):void
{
	/**
	 * 新增视频专辑
	 * */
	if(event.index==0){
		this.addVideoAlbum();
	/**
	 * 修改视频专辑
	 * */
	}else if(event.index==1){
		this.editVideoAlbum();
	/**
	 * 删除视频专辑
	 * */
	}else if(event.index==2){
		this.removeVideoAlbum();
	}
	
}

/**
 * 新增视频专辑
 * */
 private function addVideoAlbum():void{
 	var addVideoAlbum:AddVideoAlbum =new AddVideoAlbum();
 	addVideoAlbum.fatherFunction = this.getVideoAlbums;
 	PopUpManager.addPopUp(addVideoAlbum,this,true);
	PopUpManager.centerPopUp(addVideoAlbum);
 }
 
 /**
 * 修改视频专辑
 * */
 private function editVideoAlbum():void{
 	if(videoAlbumDG.selectedItem==null){
 		Alert.show("请选择一条需要修改的专辑信息","提示信息");
 	}else{
 		var videoAlbum:VideoAlbum =VideoAlbum(videoAlbumDG.selectedItem);
 		var editVideoAlbum:EditVideoAlbum =EditVideoAlbum(PopUpManager.createPopUp(this,EditVideoAlbum,true));
 		editVideoAlbum.txtAlbumName.text = videoAlbum.albumName;
 		editVideoAlbum.txtDesc.text = videoAlbum.descn;
 		editVideoAlbum.albumId = videoAlbum.albumId;
 		editVideoAlbum.fatherFunction=this.getVideoAlbums;
		PopUpManager.centerPopUp(editVideoAlbum);
 	}
 }
 
 /**
 * 删除视频专辑
 * */
 private function removeVideoAlbum():void{
 	if(videoAlbumDG.selectedItem==null){
 		Alert.show("请选择要删除的视频专辑","提示信息");
 	}else{
 		var delVideoAlbum:VideoAlbum =VideoAlbum(videoAlbumDG.selectedItem);
 		videoAlbumService.removeVideoAlbum(delVideoAlbum);
 	}
 }
 
 /**
 * 删除视频专辑响应调用
 * */
 private function removeVideoAlbumHandler(event:ResultEvent):void{
 	var resVideoAlbum:VideoAlbum =VideoAlbum(event.result);
 	Alert.show("删除成功","提示信息");
 	this.getVideoAlbums();
 }

/**
 * 创建时间格式化调用
 * */
private function createTimeFormatDate(row:Object,column:DataGridColumn):String
{
	return timeFormatter.format(row.createTime);
}