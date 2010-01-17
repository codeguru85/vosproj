// ActionScript file
import dto.security.video.VideoAlbum;
import dto.security.video.VideoAlbumShip;
import dto.security.video.VideoAlbumShipId;
import dto.security.video.VideoInfo;

import mx.controls.Alert;
import mx.controls.dataGridClasses.DataGridColumn;
import mx.events.ListEvent;
import mx.managers.PopUpManager;
import mx.rpc.events.ResultEvent;

import util.page.Finder;

/**
 * 所选中要加入专辑的数据
 * */
public var videoIds:Array;

/**
 * 视频专辑集合
 * */
[Bindable]
private var videoAlbums:Finder;

/**
 * 提示信息的开关
 * */
 private var isFlag:int=0;


/**
 * 设置每页默认记录数
 * */
private var thisPageSize:String = '6';

/**
 * 判断从某处弹出的窗口
 * */
 public var targetWindow:String;
 
 /**
 * 记录所添加视频个数
 * */
 
 private var videoCount:int;
 
 
 /**
 * 记录视频存在次数
 * */
 private var isSetCount:int=0;

/**
 * 组件初始化调用
 * */
 private function initApp():void{
 	if(targetWindow=="addAlbum"){
 		cmdAddAlbum.visible =true;
 	}
 	this.getVideoAlbums();
 }


/**
 * 关闭窗口调用
 * */
private function closeThis():void{
	PopUpManager.removePopUp(this);
}

/**
 * 初始化数据查询调用
 * */
 private function getVideoAlbums():void{
 	videoAlbumService.getPageVideoAlbum(null,1,6);	
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
 * 设置每页显示记录数
 * */
public function getPagesize(pageSize:String):void{
 	thisPageSize = pageSize;
 	var videoAlbum:VideoAlbum = new VideoAlbum();
 	videoAlbum.albumName=txtAlbumName.text;
 	videoAlbumService.getPageVideoAlbum(videoAlbum,1,pageSize);
}

/**
 * 检查所加入的视频
 * */
private function getIsExitsVideoAlbumShip(videoAlbumShip:VideoAlbumShip):void
{
	videoAlbumService.getIsExitsVideoAlbumShip(videoAlbumShip);
}

/**
 * 检查所加入的视频在专辑内是否已经存在响应
 * */
private function getIsExitsVideoAlbumShipHandler(event:ResultEvent):void
{
	var videoId:int = event.result as int;
	if(videoId!=0){
			isFlag=1;
			var videoAlbum:VideoAlbum = VideoAlbum(videoAlbumSelect.selectedItem);
 			var videoAlbumShipId:VideoAlbumShipId = new VideoAlbumShipId();
 			var videoAlbumShip:VideoAlbumShip = new VideoAlbumShip();
 			var videoInfo:VideoInfo = new VideoInfo();
 		
 			videoAlbumShipId.albumId = videoAlbum.albumId;
 			videoAlbumShipId.videoId = videoId;
 			videoAlbumShipId.createTime = new Date();
 			videoAlbumShip.id = videoAlbumShipId;
 			videoAlbumShip.videoAlbum = videoAlbum;
 			videoAlbumShip.videoInfo =videoInfo;
			videoAlbumService.addVideoAlbumShip(videoAlbumShip);
	}else{
		isSetCount=isSetCount+1;
	}
	/**
	 * 当所选视频都已经存在某专辑的时候提示
	 * */
	if(videoCount==isSetCount){
		Alert.show("你所加入的视频在专辑中已经存在","提示信息");
		return;	
	}
}

/**
 * 确定加入专辑,建立关联关系调用
 * */
 private function getAddVideoAlbum():void
 {
 		if(videoAlbumSelect.selectedItem==null)
 		{
 			Alert.show("请选择需要加入到的专辑","提示信息");
 			return;
 		}else{
 			
 			var videoAlbum:VideoAlbum = VideoAlbum(videoAlbumSelect.selectedItem);
 			var videoAlbumShipId:VideoAlbumShipId = new VideoAlbumShipId();
 			var videoAlbumShip:VideoAlbumShip = new VideoAlbumShip();
 			var videoInfo:VideoInfo = new VideoInfo();
 			isFlag=0;
 			/**
 			 * 视频添加数量记录
 			 * */
 			videoCount=videoIds.length;
 			for(var i:int=0;i<videoIds.length;i++)
 			{
 				videoAlbumShipId.albumId = videoAlbum.albumId;
 				videoAlbumShipId.videoId = videoIds[i].videoId;
 				videoAlbumShipId.createTime = new Date();
 				videoAlbumShip.id = videoAlbumShipId;
 				videoAlbumShip.videoAlbum = videoAlbum;
 				videoAlbumShip.videoInfo =videoInfo;
 				this.getIsExitsVideoAlbumShip(videoAlbumShip);
 			}
 		}
 }
 
/**
 * 确定加入专辑响应调用
 * */
private function addVideoAlbumShipHandler(event:ResultEvent):void
{
	if(event.result!=null){
		if(isFlag==1){
			Alert.show("专辑加入成功","提示信息");
			isFlag=2;
		}
	}
}
 

/**
 * 选单条信息调用
 * */
private function clickHandler(event:ListEvent):void
{
	if(targetWindow=="more")
	{
		this.closeThis();
		
	}else if(targetWindow=="addAlbum")
	{
	}else if(targetWindow=="addVideoInfo"){
		fatherFunction(event.itemRenderer.data.albumId,event.itemRenderer.data.albumName);
	}else if(targetWindow=="editVideoInfo"){
		fatherFunction(event.itemRenderer.data.albumId,event.itemRenderer.data.albumName);
	}else{
		fatherFunction(event.itemRenderer.data.albumId,event.itemRenderer.data.albumName);
	}
}

///**
// * 双击响应时间进行查询视频列表信息
// * */
// private function itemDoubleClickHandler():void{
// 	this.closeThis();
// }

/**
 * 创建时间格式化调用
 * */
private function createTimeFormatDate(row:Object,column:DataGridColumn):String
{
	return timeFormatter.format(row.createTime);
}
