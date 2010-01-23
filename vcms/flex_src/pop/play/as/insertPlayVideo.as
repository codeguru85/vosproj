import dto.security.client.TermInfo;
import dto.security.play.PlayListShip;
import dto.security.play.PlayTime;
import dto.security.play.Playlist;
import dto.security.play.TermPlaylist;
import dto.security.video.VideoInfo;
import dto.security.video.VideoType;

import flash.events.Event;

import mx.collections.ArrayCollection;
import mx.collections.IList;
import mx.controls.Alert;
import mx.controls.buttonBarClasses.ButtonBarButton;
import mx.core.IUIComponent;
import mx.events.DragEvent;
import mx.events.ItemClickEvent;
import mx.managers.*;
import mx.rpc.events.ResultEvent;

import util.page.Finder;

/**
  * 定义选择的树型节点的XML对象
* */
[Bindable]	
private var videoTypeTree:XML;
[Bindable]
public var moveimages:ArrayCollection = new ArrayCollection();
[Bindable]
private var termnames:Finder;
[Bindable]
private var playterm:ArrayCollection = new ArrayCollection();
[Bindable]
private var imagelist:Finder;
/* private var img:Image; */
[Bindable]
private var videomes:ArrayCollection;
[Bindable]
private var typelist:XML;
private var videoinfo:VideoInfo = new VideoInfo();
private var terminfo:TermInfo = new TermInfo();
private var videotype:VideoType = new VideoType();
private var playinfo:Playlist = new Playlist();
private var playship:PlayListShip = new PlayListShip();
private var termship:TermPlaylist = new TermPlaylist();
private var addplay:ArrayCollection = new ArrayCollection();
public var videoLists:ArrayCollection = new ArrayCollection();
public var termLists:ArrayCollection= new ArrayCollection();
public var listName:String;
public var listid:int;
public var trag:int;
public var playState:String;

private function oncreationComplete():void{
	videoTypeService.getTreeVideoType();
	var videoinfo:VideoInfo = new VideoInfo();		
	playInfoService.getSelectVideo(videoinfo,1,14);			
}

private function getPlayVideoHandler(event:ResultEvent):void{
	moveimages = event.result as ArrayCollection;
	horizontallist.dataProvider	= moveimages;
}

public function getPaginations(page:int,pagesize:int):void{
	videoinfo.videoName = videoname.text;
	playInfoService.getSelectVideo(videoinfo,page,pagesize);
}
	
//查询视频类型
private function getTreeVideoTypeHandler(event:ResultEvent):void{
	videoTypeTree = XML(event.result);
   	type_tree.openItems=videoTypeTree.id.node;
   	type_tree.expandItem(videoTypeTree,true);
}

public function getPagesize(pagesize:int):void{
	playInfoService.getSelectVideo(videoinfo,1,pagesize);
}

//获取视频
private function getVideoInfoHandler(event:ResultEvent):void{
	imagelist = event.result as Finder;
}

private function videoHandler(event:ItemClickEvent):void{
 	var m1:ButtonBarButton = videoRemove.getChildAt(0) as ButtonBarButton;
 	var m2:ButtonBarButton = videoRemove.getChildAt(1) as ButtonBarButton;
 	if(event.index==0){		
 		/* var itemVideo:Object = horizontallist.selectedItem;  	
 		if(itemVideo==null){
 			Alert.show("请选择要删除的视频文件");
 			return;
 		}else{
 			var arrVideo:ArrayCollection = ArrayCollection(horizontallist.dataProvider);
 			arrVideo.removeItemAt(arrVideo.getItemIndex(itemVideo));
 		}  */
 		var videoCol:ArrayCollection = ArrayCollection(horizontallist.dataProvider);
		var objVideo:Object = new Object();
		videoCol.removeAll(); 			
 	}
 	if(event.index==1){
		PopUpManager.removePopUp(this);		
 	}
 	if(event.index==2){
 		addPlayList();		 		
 	} 	
}

private var VTime:ArrayCollection = new ArrayCollection();
//添加播放单
private function addPlayList():void{	
	var dt:Date = new Date();
	var pTime:PlayTime; 		
	videoLists = ArrayCollection(horizontallist.dataProvider);
	if(videoLists.length==0){
		Alert.show("请选择要插播的视频");
		return;
	}else{		
		VTime.removeAll();
		for(var j:int=0;j<videoLists.length;j++){
			pTime = new PlayTime();
			pTime.playVideoName = videoLists.getItemAt(j,0).fileName;
			pTime.playFilePath = videoLists.getItemAt(j,0).filePath;	
			pTime.timeCount = videoLists.getItemAt(j,0).timeCount;		
			VTime.addItem(pTime);
		}
		playinfo.listId = listid;
		playInfoService.insertPlay(VTime,playinfo);
	}
	
}

private function insertPlayHandler(event:ResultEvent):void{	
	PopUpManager.removePopUp(this);
	/* var returnArray:ArrayCollection = event.result as ArrayCollection;
	var returnMsg:String = "";	
	for(var i:int = 0;i<returnArray.length;i++){
		var returnString:String="";
		var clientName:String = returnArray.getItemAt(i).clientName;
		var ReturnMark:String = returnArray.getItemAt(i).returnMark;
		
		if(ReturnMark=='0'){
			
			returnString = "插播失败！"+'\n';
		}
		else if(ReturnMark == '1'){
			returnString = "无法连接！"+'\n';
		}
		else if(ReturnMark == '2'){
			returnString = "播放成功！"+'\n';	
			PopUpManager.removePopUp(this);		
		}
		else if(ReturnMark == '3'){
			returnString = "发送停止，找不到本地IP"+'\n';
		}    
		else if(ReturnMark == '4'){
			returnString = "应答超时！"+'\n';
		}else if(ReturnMark == '5'){
			returnString = "播放列表中无视频"+'\n';
		}
		returnMsg+= "终端"+clientName+returnString;
		
	}
	if(returnArray.length==0){
		returnMsg = "视频不存在，请点击'指定下载'按钮，将视频下载到终端";
	}
	Alert.show(returnMsg); */
}

//根据类型查询视频文件
private function getReadFile(event:Event):void{	
	typelist=Tree(event.target).selectedItem as XML
	var videoinfo:VideoInfo = new VideoInfo();
	if(typelist.@id=="all"){
		playInfoService.getSelectVideo(videoinfo,1,20);
	}else{	
		var itemIsBranch:Boolean =type_tree.dataDescriptor.isBranch(typelist);
        if(!itemIsBranch)
        {
  			videotype.typeId = typelist.@id;		
			playInfoService.getVideoTypes(videotype,1,20);
        }	
	}
}

//判断视频重复
private function doDragOverVideo():void{
	var video:Object = videotilelist.selectedItem;
	var videolist:ArrayCollection = new ArrayCollection();
	videolist = ArrayCollection(horizontallist.dataProvider);	
	for(var i:int;i<videolist.length;i++){
		if(video.videoId == videolist[i].videoId){
			Alert.show(video.videoName+"视频己经存在");
			return;
		}
	}
}

private function getCreageTime(row:Object,column:DataGridColumn):String{
	return dateFormatter.format(row.createTime);	
}

private function getUserName(row:Object,column:DataGridColumn):String{
	return row.userInfo.userName;
}

//移动视频文件
private function removeHandler(event:ItemClickEvent):void{
	if(event.index == 0){
		var i : int = horizontallist.selectedIndex;   
          if (i >= 1&&horizontallist.selectedItem)   
          {                
            IList(horizontallist.dataProvider).addItemAt(horizontallist.selectedItem,i-1);                     
            IList(horizontallist.dataProvider).removeItemAt(i+1);   
          	horizontallist.selectedIndex = i;  
          } 
	}
	if(event.index == 1){
		var i : int = horizontallist.selectedIndex;         
       if (i < (ArrayCollection(horizontallist.dataProvider).length - 1) && horizontallist.selectedItem)   
       {                
          IList(horizontallist.dataProvider).addItemAt(horizontallist.selectedItem,i + 2);                     
          IList(horizontallist.dataProvider).removeItemAt(i);      
          horizontallist.selectedIndex = i;     
       } 
	}
}