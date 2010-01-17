import dto.security.client.TermInfo;
import dto.security.play.PlayListShip;
import dto.security.play.PlayTime;
import dto.security.play.Playlist;
import dto.security.play.TermPlaylist;
import dto.security.video.VideoAlbum;
import dto.security.video.VideoInfo;
import dto.security.video.VideoType;

import flash.events.Event;

import mx.collections.ArrayCollection;
import mx.collections.IList;
import mx.collections.Sort;
import mx.collections.SortField;
import mx.controls.Alert;
import mx.controls.TextInput;
import mx.controls.buttonBarClasses.ButtonBarButton;
import mx.core.IUIComponent;
import mx.events.DataGridEvent;
import mx.events.DragEvent;
import mx.events.ItemClickEvent;
import mx.events.ListEvent;
import mx.events.ValidationResultEvent;
import mx.managers.*;
import mx.rpc.events.ResultEvent;

import util.TimeValidator;
import util.page.Finder;

/**
  * 定义选择的树型节点的XML对象
* */
[Bindable]	
private var videoTypeTree:XML;
[Bindable]	
private var videoAlbumTree:XML;
[Bindable]
public var moveimages:ArrayCollection = new ArrayCollection();
[Bindable]
public var moveAlbums:ArrayCollection = new ArrayCollection();
[Bindable]
private var termnames:Finder;
[Bindable]
private var playterm:ArrayCollection = new ArrayCollection();
[Bindable]
private var imagelist:Finder;
[Bindable]
private var albumlist:Finder;
private var img:Image; 
[Bindable]
private var videomes:ArrayCollection;
[Bindable]
// 选定的视频分类树型节点的XML对象
private var typelist:XML;
 // 选定的视频专辑树型节点的XML对象
private var selectedAlbumNode:XML;
private var videoinfo:VideoInfo = new VideoInfo();
private var videoAlbum:VideoAlbum = new VideoAlbum();
private var terminfo:TermInfo = new TermInfo();
private var videotype:VideoType = new VideoType();
private var playinfo:Playlist = new Playlist();
private var playship:PlayListShip = new PlayListShip();
private var termship:TermPlaylist = new TermPlaylist();
private var addplay:ArrayCollection = new ArrayCollection();
private var flag:int=0;
public var videoLists:ArrayCollection = new ArrayCollection();
public var termLists:ArrayCollection= new ArrayCollection();
public var listName:String;
public var listid:int;
public var trag:int;
public var playState:String;
private var nowDate:Date ;
private var fdate:DateFormatter = new DateFormatter() ;
private var dragEvent:DragEvent;
private var videolistTemp:Array;
private var albumIdTemp:String;
private var typeIdTemp:String;
private var validator:TimeValidator;
private var dg:*;
private var errorFlag:Boolean = false;


private function oncreationComplete():void{
	var b1:ButtonBarButton = videobar.getChildAt(2) as ButtonBarButton;	
	b1.enabled = false;		
	//videoTypeService.getTreeVideoType();
	//playInfoService.getTreeVideoAlbum();
	this.getTreeVideoType();
	var terminfo:TermInfo = new TermInfo();
	var videoinfo:VideoInfo = new VideoInfo();		
	playInfoService.getSelectTerm(terminfo,1,20);	
	playInfoService.getSelectVideo(videoinfo,1,14);
	if(trag==2){
		Init();
	}
}

//获得更新数据
private function Init():void{
	playinfo.listId = listid;
	playInfoService.getPlayVideo(playinfo);
	playInfoService.getPlayTerm(playinfo);	
	playlistname.text = listName; 	
}

private function getPlayVideoHandler(event:ResultEvent):void{
	moveimages = event.result as ArrayCollection;
	if(trag == 2){
		var seqTime:Object = new Object();
		var shipArray:ArrayCollection = new ArrayCollection();
		var ship:Object = new Object();
		for(var i:int=0;i<moveimages.length;i++){
			seqTime = moveimages.getItemAt(i);
			for(var j:int=0;j<seqTime.playListShips.length;j++){
				if(seqTime.playListShips.getItemAt(j).playlist.listId == listid){
					seqTime.setTime = seqTime.playListShips.getItemAt(j).listSeq;
					shipArray.addItem(seqTime);
					if(this.isoToDate(seqTime.setTime)< this.getServerTime()){
						alertText.text="提示：您设置的播放时间小于服务器时间，任务建立后不会播放";
					}
				}
			}
		}
	}
	moveimages = shipArray;
	//sortAc();
}

private function getPlayTermHandler(event:ResultEvent):void{
	playterm = event.result as ArrayCollection;
} 

//根据条件查询终端信息
public function queryInfoByName(page:int,pagesize:int):void{
	albumIdTemp = "";
	typeIdTemp = "";
		if(viewVS.selectedChild == videoWindow){
			videoinfo.videoName = videoname.text;
		}
		if(viewVS.selectedChild == albumWindow){
			videoAlbum.albumName = albumname.text;
		}
	getPaginations(page,pagesize);
}

//根据条件查询终端信息
public function getPaginations(page:int,pagesize:int):void{
	if(viewstack.selectedChild == viewvideo){
		if(viewVS.selectedChild == videoWindow){		
			if(typeIdTemp && typeIdTemp != "all"){
				videotype.typeId = parseInt(typeIdTemp);		
				playInfoService.getVideoTypes(videotype,page,pagesize);
			}
			else if(albumIdTemp && albumIdTemp != "all"){
				videoAlbum.albumId = parseInt(albumIdTemp);
				videoInfoService.getAlbumVideoInfo(videoAlbum,page,pagesize);
			}
			else{
				playInfoService.getSelectVideo(videoinfo,page,pagesize);
			}
		}
		if(viewVS.selectedChild == albumWindow){
			videoAlbumService.getPageVideoAlbum(videoAlbum,page,pagesize);
		}
	}
	if(viewstack.selectedChild == viewterm){
		terminfo.termName = termname.text;
		terminfo.termAddr = termaddr.text;
		if(termip.value.toString()=="0.0.0.0"){
			terminfo.termIp = "";
		}else{
			terminfo.termIp = termip.value.toString();
		}
		playInfoService.getSelectTerm(terminfo,page,pagesize);	
	}
}

public function getPagesize(pagesize:int):void{
	if(viewstack.selectedChild == viewvideo){
		if(viewVS.selectedChild == videoWindow){
			if(typeIdTemp && typeIdTemp != "all"){
				videotype.typeId = parseInt(typeIdTemp);		
				playInfoService.getVideoTypes(videotype,1,pagesize);
			}
			else if(albumIdTemp && albumIdTemp != "all"){
				videoAlbum.albumId = parseInt(albumIdTemp);
				videoInfoService.getAlbumVideoInfo(videoAlbum,1,pagesize);
			}
			else{
				playInfoService.getSelectVideo(videoinfo,1,pagesize);
			}
		}
		if(viewVS.selectedChild == albumWindow){
			videoAlbumService.getPageVideoAlbum(videoAlbum,1,pagesize);			
		}
	}
	if(viewstack.selectedChild == viewterm){
		playInfoService.getSelectTerm(terminfo,1,pagesize);
	}
}
//获取终端
private function getSelectTermHandler(event:ResultEvent):void{
	termnames = event.result as Finder;
}
//获取视频
private function getVideoInfoHandler(event:ResultEvent):void{
	imagelist = event.result as Finder;
}

private var delArrVideo:ArrayCollection = new ArrayCollection();
private var delArrTerm:ArrayCollection = new ArrayCollection();
private function videoHandler(event:ItemClickEvent):void{
 	var b1:ButtonBarButton = videobar.getChildAt(2) as ButtonBarButton;
 	var b2:ButtonBarButton = videobar.getChildAt(3) as ButtonBarButton;
 	var m1:ButtonBarButton = videoRemove.getChildAt(0) as ButtonBarButton;
 	var m2:ButtonBarButton = videoRemove.getChildAt(1) as ButtonBarButton;
 	if(event.index==0){		
 		if(viewstack.selectedChild == viewvideo){ 
 			if(viewVS.selectedChild == videoWindow){ 					 						
	 			var itemVideo:Object = horizontallist.selectedItem;
	 			var videoIndex:int =  horizontallist.selectedIndex; 			
 			}
 			if(viewVS.selectedChild == albumWindow){
 				var itemVideo:Object = horizontallist2.selectedItem;
	 			var videoIndex:int =  horizontallist2.selectedIndex;	 			
 			}
 			if(itemVideo!=null)
 				var time:int =  itemVideo.timeSecond; 
 			moveimages = ArrayCollection(horizontallist.dataProvider);						
 			var objTime:Object = new Object();
 			var arrMove:ArrayCollection = new ArrayCollection();		
			fdate.formatString = "YYYY-MM-DD JJ:NN:SS" ;
 			if(trag == 2&&itemVideo!=null){
 				delArrVideo.addItem(itemVideo.videoId); 							
 			} 					
 			if(itemVideo==null){
 				Alert.show("请选择要删除的视频文件");
 				return;
 			}else{
 				var arrVideo:ArrayCollection = ArrayCollection(horizontallist.dataProvider);
 				arrVideo.removeItemAt(arrVideo.getItemIndex(itemVideo));
 			} 
 			for(var j:int=0;j<videoIndex;j++){
 				objTime = moveimages.getItemAt(j);
 				arrMove.addItem(objTime); 
 			}
 			for(var i:int=videoIndex;i<moveimages.length;i++){
 				objTime = moveimages.getItemAt(i);
 				var upTime:String = objTime.setTime;
 				var upDate:Date = isoToDate(upTime); 				
				var upSDate:Date = dateAdd("milliseconds",-time*1000,upDate);
 				objTime.setTime = fdate.format(upSDate) ;
 				arrMove.addItem(objTime); 				
 			} 			
 			 moveimages = arrMove;
 			 		 	
 		}else{ 	
 			var itemterm:Object = selectTerm.selectedItem;
 			if(trag == 2&&itemterm!=null){
 				delArrTerm.addItem(selectTerm.selectedItem.termId);
 			}
 			if(itemterm==null){
 				Alert.show("请选择要删除的终端");
 				return;
 			}else{
 				var arrterm:ArrayCollection = ArrayCollection(selectTerm.dataProvider);
 				arrterm.removeItemAt(arrterm.getItemIndex(itemterm));
 			} 			
 		}
 	}
 	if(event.index==1){
 		if(viewstack.selectedChild == viewvideo){
			var videoCol:ArrayCollection = ArrayCollection(horizontallist.dataProvider);
			var objVideo:Object = new Object();
			if(trag == 2){				
				for(var v:int=0;v<videoCol.length;v++){
					delArrVideo.addItem(videoCol.getItemAt(v).videoId);
				}
			}
			videoCol.removeAll(); 		
 		}else{
 			var termCol:ArrayCollection = ArrayCollection(selectTerm.dataProvider);
 			var objTerm:Object = new Object();
 			if(trag == 2){
 				for(var n:int=0;n<termCol.length;n++){
 					delArrTerm.addItem(termCol.getItemAt(n).termId);
 				}
 			}
 			termCol.removeAll();
 		}
 	}
 	if(event.index==2){
 		b1.enabled = false;
 		b2.enabled = true;
 		m1.enabled = true;
 		m2.enabled = true;
 		viewstack.selectedChild = viewvideo;			 		
 	}
 	if(event.index==3){
 		b1.enabled = true;
 		b2.enabled = false;
 		m1.enabled = false;
 		m2.enabled = false;
 		flag = 1;
 		viewstack.selectedChild = viewterm;
 	}
 	if(event.index==4){
 		var nowDate:Date = this.getServerTime();
 		var jectTime:Object = new Object();
 		var moveVideo:ArrayCollection = ArrayCollection(horizontallist.dataProvider);
 		if(playlistname.text==null||playlistname.text==""){
 			Alert.show("任务名不能为空");
 		}else{
 			var AlertFlag:Boolean = false;
 			for(var t:int=0;t<moveVideo.length;t++){
				jectTime = moveVideo.getItemAt(t);
//				if(isoToDate(jectTime.setTime)<nowDate){
//					Alert.show("设置视频播放时间必须大于当前时间，请先设置播放时间");
//					return;
//				}
				if(isoToDate(jectTime.setTime)<nowDate){
					alertText.text="提示：您设置的播放时间小于服务器时间，任务建立后不会播放";
					AlertFlag = true;
		        }
			}
			if(!AlertFlag){
				alertText.text="";
			}
			if(trag==2){
 				this.updatePlayList();
 			}else{
 				this.addPlayList();
 			}	
 		}
 	}
}

private var terms:ArrayCollection = new ArrayCollection();
private var VTime:ArrayCollection = new ArrayCollection();
//添加播放单
private function addPlayList():void{	
	var dt:Date = getServerTime();
	var pTime:PlayTime;
	videoLists = ArrayCollection(horizontallist.dataProvider);
	if(videoLists.length>0&&flag==0){
		Alert.show("请给播放视频指定终端");
		return;
	}
	if(videoLists.length==0){
		Alert.show("请选择要播放的视频");
		return;
	}	
	termLists = ArrayCollection(selectTerm.dataProvider);
	if(termLists.length==0){
		Alert.show("请选择要播放的视频的终端");
		return;
	}else{	
		VTime.removeAll();
		terms.removeAll();
		var startTimeArr:Array = new Array(); 	
		var endTimeArr:Array = new Array(); 
		for(var j:int=0;j<videoLists.length;j++){
			pTime = new PlayTime();
			pTime.videoId = videoLists.getItemAt(j,0).videoId;
			pTime.playVideoName = videoLists.getItemAt(j,0).fileName;
			pTime.playTime = videoLists.getItemAt(j,0).setTime;
			startTimeArr.push(this.isoToDate(pTime.playTime));
			var count:int = videoLists.getItemAt(j,0).timeSecond;
			endTimeArr.push(dateAdd("milliseconds",count*1000,isoToDate(pTime.playTime)));
			 for(var n:int=0;n<VTime.length;n++){
				if(videoLists.getItemAt(j,0).setTime==VTime.getItemAt(n,0).playTime){
					Alert.show("同一播放列表里不能设有相同时间播放的视频");
					return;
				}
			}  
			VTime.addItem(pTime);
		}
		for(var i:int=0;i<termLists.length;i++){
			terms.addItem(termLists.getItemAt(i,0).termId);		
		}
	}
	var sort:Sort = new Sort();
	sort.sort(startTimeArr);
	sort.sort(endTimeArr);
	playinfo.startTime = startTimeArr[0];
	playinfo.endTime = endTimeArr[endTimeArr.length-1];
	playinfo.listName = playlistname.text;
	playinfo.videoNum = videoLists.length;
	playinfo.playState = "1";
	playinfo.createTime = dt;	 
	playship.createTime = dt;
	termship.createTime = dt;
	 playInfoService.getPlayListName(playlistname.text);	 
}

private function getAddPlayListHandler(event:ResultEvent):void{	
	 if(event.result!=null){		
		Alert.show("添加播放单成功");
		PopUpManager.removePopUp(this);
		playInfoService.getSelectPlay(playinfo,1,20);
	} 	
}

//更新播放单
private function updatePlayList():void{	
	var dt:Date = getServerTime();	
	var pTime:PlayTime = new PlayTime();
	videoLists = ArrayCollection(horizontallist.dataProvider);	
	if(videoLists.length==0){
		Alert.show("请选择播放要播放的视频");
		return;
	}	
	if(flag == 0){
		termLists = playterm;
	}else{
		termLists = ArrayCollection(selectTerm.dataProvider);	
	}
	if(termLists.length==0){
		Alert.show("请选择要播放的视频和终端");
		return;
	}else{
		VTime.removeAll();
		terms.removeAll();
		var startTimeArr:Array = new Array(); 	
		var endTimeArr:Array = new Array(); 
		for(var j:int=0;j<videoLists.length;j++){
			pTime = new PlayTime();
			pTime.videoId = videoLists.getItemAt(j,0).videoId;
			pTime.playVideoName = videoLists.getItemAt(j,0).fileName;
			pTime.playTime = videoLists.getItemAt(j,0).setTime;
			startTimeArr.push(this.isoToDate(pTime.playTime));
			var count:int = videoLists.getItemAt(j,0).timeSecond;
			endTimeArr.push(dateAdd("milliseconds",count*1000,isoToDate(pTime.playTime)));
			 for(var n:int=0;n<VTime.length;n++){
				if(videoLists.getItemAt(j,0).setTime==VTime.getItemAt(n,0).playTime){
					Alert.show("同一播放列表里不能设有相同时间播放的视频");
					return;
				}
			}  
			VTime.addItem(pTime);
		}
		for(var i:int=0;i<termLists.length;i++){
			terms.addItem(termLists.getItemAt(i,0).termId);		
		}
	}
	var sort:Sort = new Sort();
	sort.sort(startTimeArr);
	sort.sort(endTimeArr);
	playinfo.startTime = startTimeArr[0];
	playinfo.endTime = endTimeArr[endTimeArr.length-1];
	playinfo.listId = listid;
	playinfo.listName = playlistname.text;
	playinfo.videoNum = videoLists.length;
	playinfo.playState = playState;
	playinfo.createTime = dt;	 
	playship.createTime = dt;
	termship.createTime = dt;
	if(playlistname.text==listName){
		for(var p:int=0;p<delArrVideo.length;p++){
 			playInfoService.delVideo(playinfo,delArrVideo.getItemAt(p));
 		} 
 		for(var f:int=0;f<delArrTerm.length;f++){
 			playInfoService.delTerm(playinfo,delArrTerm.getItemAt(f));
 		} 	
		playInfoService.getUpdatePlayList(playinfo,playship,termship,VTime,terms);
	}else{
		playInfoService.getPlayListName(playlistname.text);	
	}
}

private function getUpdatePlayListHandler(event:ResultEvent):void{	
	if(event.result!=null){		
		Alert.show("更新播放单成功");
		PopUpManager.removePopUp(this);
		playInfoService.getSelectPlay(playinfo,1,20);
// 		for(var p:int=0;p<delArrVideo.length;p++){
// 			playInfoService.delVideo(playinfo,delArrVideo.getItemAt(p));
// 		} 
// 		for(var f:int=0;f<delArrTerm.length;f++){
// 			playInfoService.delTerm(playinfo,delArrTerm.getItemAt(f));
// 		} 		
	}	
}

private function getPlayListNameHandler(event:ResultEvent):void{
	var arrayName:ArrayCollection = event.result as ArrayCollection;
	if(arrayName.length>0){
		Alert.show("该任务名已经存在");
		return;
	}else{
		if(trag==2){
			for(var p:int=0;p<delArrVideo.length;p++){
	 			playInfoService.delVideo(playinfo,delArrVideo.getItemAt(p));
	 		} 
	 		for(var f:int=0;f<delArrTerm.length;f++){
	 			playInfoService.delTerm(playinfo,delArrTerm.getItemAt(f));
	 		} 	
			playInfoService.getUpdatePlayList(playinfo,playship,termship,VTime,terms);
		}else{
			playInfoService.getAddPlayList(playinfo,playship,termship,VTime,terms);		
		}
	}
}

private function getSelectPlayHandler(event:ResultEvent):void{	
	fatherfunction();
}

	//添加全部视频或终端
/* private function selectBox():void{	
 	if(termbox.selected){
  		selectTerm.dataProvider = 	termlist.dataProvider;			
 	}
} */

//查看视频详细信息
private function getVideoMessage(evet:ListEvent):void{ 
	videoInfoMessage.visible = true;
	videoInfoMessage.x = mouseX;
	videoInfoMessage.y = mouseY;
	img = new Image();
	var fdate:DateFormatter = new DateFormatter();
	fdate.formatString = "YYYY-MM-DD";
	labVideoName.text = evet.itemRenderer.data.videoName;	
	labFileSize.text = evet.itemRenderer.data.fileSize;
	labTimeCount.text = evet.itemRenderer.data.timeCount; 
	labVideoDesc.text = evet.itemRenderer.data.descn;
	picture.source = "./"+evet.itemRenderer.data.imagePath;
	labCreateTime.text = fdate.format(evet.itemRenderer.data.createTime);
	var videoType:ArrayCollection=evet.itemRenderer.data.videoTypes;
	if(videoType.length==1){
		var typeName:String=videoType.getItemAt(0).typeName;
		labTypeName.text = typeName;
	}else{
		labTypeName.text ="无分类";
	}
} 

//查看终端详细信息
private function getTermMessage(event:ListEvent):void{
	termInfoMessage.visible = true;
	termInfoMessage.x = mouseX;
	termInfoMessage.y = mouseY;
	labTermName.text = event.itemRenderer.data.termName;
	labTermIp.text = event.itemRenderer.data.termIp;
	labTermMask.text = event.itemRenderer.data.termMask;
	labTermGateway.text = event.itemRenderer.data.termGateway;
	labTermDns.text = event.itemRenderer.data.termDns;
	labTermAddr.text = event.itemRenderer.data.termAddr;
	labType.text = event.itemRenderer.data.termType.typeName;
	if(event.itemRenderer.data.termActivation == 0){
		labTermActivation.text = "否";
	}else{
		labTermActivation.text = "是";
	}	 
}

//根据类型查询视频文件
private function getReadFile(event:Event):void{	
	typelist=Tree(event.target).selectedItem as XML;
	typeIdTemp = typelist.@id.toString();
	albumIdTemp = "";
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

 /**
  * 根据视频专辑菜单查询视频列表信息
 * */
private function getChangeAlbumHander(event:Event):void
{
  	selectedAlbumNode =Tree(event.target).selectedItem as XML;
  	albumIdTemp = selectedAlbumNode.@id.toString();
  	typeIdTemp = "";
	if(selectedAlbumNode.@id=="all"){
		albumWindow.visible=true;
		albumWindow.includeInLayout=true;
		videoWindow.visible=false;
		videoWindow.includeInLayout=false;
		viewVS.selectedChild = albumWindow;
//		maintainFocus();
		
		this.getPageVideoAlbum();
  	}else{
  		 var itemIsBranch:Boolean =album_tree.dataDescriptor.isBranch(selectedAlbumNode);
  		if(!itemIsBranch)
  		{
  			videoWindow.visible=true;
			videoWindow.includeInLayout=true;
			
			albumWindow.visible=false;
			albumWindow.includeInLayout=false;
			viewVS.selectedChild = videoWindow;
//			maintainFocus();
			
  			var videoAlbum:VideoAlbum = new VideoAlbum();
  			videoAlbum.albumId =selectedAlbumNode.@id;
  			videoInfoService.getAlbumVideoInfo(videoAlbum,1,14);
  		} 
  	}
}

private function getAlbumVideoInfoHandler(event:ResultEvent):void
{
  	imagelist = event.result as Finder;  	
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

//判断终端重复
private function doDragOverTerm():void{
	var term:Array = termlist.selectedItems;
	var termlist:ArrayCollection = new ArrayCollection();
	termlist = ArrayCollection(selectTerm.dataProvider);
	if(termlist!=null){
	for(var n:int=0;n<term.length;n++){
		for(var m:int=0;m<termlist.length;m++){
			if(term[n].termId==termlist[m].termId){
				Alert.show(term[n].termName+"终端己经存在");
				return;
			}
		}
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
		var e : int = horizontallist.selectedIndex;         
       if (e < (ArrayCollection(horizontallist.dataProvider).length - 1) && horizontallist.selectedItem)   
       {                
          IList(horizontallist.dataProvider).addItemAt(horizontallist.selectedItem,e + 2);                     
          IList(horizontallist.dataProvider).removeItemAt(e);      
          horizontallist.selectedIndex = e;     
       } 
	}
}

//拖动视频文件
 private function doDragDrop(event:DragEvent):void{
 	var moveVideo:ArrayCollection = ArrayCollection(horizontallist.dataProvider);
 	var videoInfo:VideoInfo = new VideoInfo();
 	if(viewVS.selectedChild == videoWindow){
 		var item:Array = videotilelist.selectedItems;
 	}else{
 		var item:Array = videolistTemp;
 	} 	
	var seqTime:Object = new Object();
	var objTime:Object = new Object();
	nowDate = this.getServerTime();
	fdate.formatString = "YYYY-MM-DD JJ:NN:SS" ;
	for(var i:int=0;i<item.length;i++){
		if(moveVideo.length >= 1){
			var AlertFlag:Boolean = false;
			for(var n:int=0;n<moveVideo.length;n++){
				objTime = moveVideo.getItemAt(n);
//				if(isoToDate(objTime.setTime)<nowDate){
//					Alert.show("设置视频播放时间必须大于当前时间，请先设置播放时间");
//					return;
//				}
				if(isoToDate(objTime.setTime)<nowDate){
					alertText.text="提示：您设置的播放时间小于服务器时间，任务建立后不会播放";
					AlertFlag = true;
		        }
			}
			if(!AlertFlag){
				alertText.text="";
			}
			for(var j:int=moveVideo.length-1;j<moveVideo.length;j++){
				seqTime = item[i];
				objTime = moveVideo.getItemAt(j);
			}
			var sTime:String = objTime.setTime;
			var dt:Date = isoToDate(sTime);	
			var count:int = objTime.timeSecond;
	 		var sDate:Date = dateAdd("milliseconds",count*1000,dt);
	 		seqTime.setTime = fdate.format(sDate);
	 		moveimages.addItem(seqTime);
		}else{
			alertText.text="提示：您设置的播放时间小于服务器时间，任务建立后不会播放";
			AlertFlag = true;
			item[i].setTime = getStringTime();
			moveimages.addItem(item[i]);
		}
	} 
	//sortAc();
}

//时间相加
private function isoToDate(value:String):Date {
    var dateStr:String = value;
    var pattern:RegExp = /\.\d+/;
    dateStr = dateStr.replace(pattern, "");
    dateStr = dateStr.replace(/-/g, "/");
    dateStr = dateStr.replace("T", " ");
    dateStr = dateStr.replace("Z", " GMT-0000");
    dateStr = dateStr.replace("+08:00", " GMT-0000");
    var tmpDate : Date = new Date(Date.parse(dateStr));
    /* tmpDate.hours -= 8; *//*时差，非常重要*/
    return tmpDate;
}


private function doDragEnter(event:DragEvent):void{
	var obj:IUIComponent = IUIComponent(event.currentTarget);
	DragManager.acceptDragDrop(obj);
} 

private function onEditEnd(event:DataGridEvent):void{
	fdate.formatString = "YYYY-MM-DD JJ:NN:SS" ;
	moveimages = ArrayCollection(horizontallist.dataProvider);
	event.preventDefault();
	var beingEditField:String = event.dataField;  //获得当前列的dataField
	var beforeTime:String = event.itemRenderer.data.setTime;
	var BTime:Date =  isoToDate(beforeTime);
	var afterTime:String = "";
	nowDate = this.getServerTime();
	var ATime:Date = null;
    if(beingEditField == "setTime") {
    	afterTime = TextInput(event.currentTarget.itemEditorInstance).text;   
        ATime = isoToDate(afterTime);
//        if(isValidDate(afterTime) == false){
//        	Alert.show('时间格式不对，请输入格式为"YYYY-MM-DD HH:NN:SS"');
//        }
		var fCell:Array=[event.columnIndex,event.rowIndex];
		dg = event.currentTarget;
		validator = new TimeValidator();
		validator.source = event.currentTarget.itemEditorInstance;
		validator.property = "text";
		validator.timeFormatError = '时间格式不对，请输入格式为"YYYY-MM-DD HH:NN:SS"';
		validator.isValidDate = isValidDate;
		var val:ValidationResultEvent = validator.validate();
		if(val.type == "invalid")
		{
		} else{
			callLater(maintainFocus);

        	if(ATime<nowDate){
	        	//Alert.show("设置视频播放时间必须大于当前时间，请先设置播放时间");
				//return;
				alertText.text="提示：您设置的播放时间小于服务器时间，任务建立后不会播放";
	        }else{
	        	alertText.text="";
	        }
        	if(viewVS.selectedChild == videoWindow){
        		horizontallist.selectedItem.setTime = afterTime;
        	}if(viewVS.selectedChild == albumWindow){
        		horizontallist2.selectedItem.setTime = afterTime;
        		moveimages = ArrayCollection(horizontallist2.dataProvider);
        	}
        		
        	if(moveimages.length > 1){ 
        		if(viewVS.selectedChild == videoWindow)      		
        			var index:int = horizontallist.selectedIndex;
        		if(viewVS.selectedChild == albumWindow)
        			var index:int = horizontallist2.selectedIndex;
        		for(index ;index < moveimages.length-1;index++){									
					var sTime:String = moveimages.getItemAt(index).setTime;
					var dt:Date = isoToDate(sTime);	
					var count:int = moveimages.getItemAt(index).timeSecond;
			 		var sDate:Date = dateAdd("milliseconds",count*1000,dt);
			 		moveimages.getItemAt(index+1).setTime = fdate.format(sDate);			 		
		 		}       		
        	}
        	/* if(moveimages.length > 1){
        		var arrDiff:ArrayCollection = new ArrayCollection();   
				var result:Object = new Object(); 
				var num1:Number =ATime.valueOf();
				var num2:Number =BTime.valueOf();
				var d1Days:int = int(num1/24/60/60/1000);
				var d2Days:int = int(num2/24/60/60/1000);
				var diffDay:int = d1Days - d2Days ;
				fdate.formatString = "YYYY-MM-DD JJ:NN:SS" ; 
				Alert.show(" ATime =========== " + ATime + " BTime========  " + BTime + " ;;;;;;;;;;; " + diffDay);
				for(var y:int=0;y<horizontallist.selectedIndex+1;y++){
					result = moveimages.getItemAt(y);
					arrDiff.addItem(result);
				} 
				for(var r:int=horizontallist.selectedIndex+1;r<moveimages.length;r++){
					result = moveimages.getItemAt(r);
					var diffTime:String = result.setTime;
					result.setTime = fdate.format(dateAdd("milliseconds",diffDay,isoToDate(diffTime)));
					arrDiff.addItem(result);
				}	
				moveimages = arrDiff;
			} */
        }
	}			 
}
				
//prevents enter/tab from moving onto the next cell after finishing editing.
private function maintainFocus():void
{
	if(dg)
   		dg.editedItemPosition = null;
}
		

//prevents enter/tab from moving onto the next cell after finishing editing.

/* private function configureListeners():void {
	horizontallist.addEventListener(DataGridEvent.ITEM_EDIT_END, onEditEnd);
} */

private function dateAdd(datepart:String = "", number:Number = 0, date:Date = null):Date { 
    if (date == null) { 
        /* Default to current date. */ 
        date = getServerTime(); 
    } 
  
    var returnDate:Date = new Date(date.time);
  
    switch (datepart.toLowerCase()) { 
        case "fullyear": 
        case "month": 
        case "date": 
        case "hours": 
        case "minutes": 
        case "seconds": 
        case "milliseconds": 
            returnDate[datepart] += number; 
            break; 
        default: 
            /* Unknown date part, do nothing. */ 
            break; 
    } 
    return returnDate; 
}

public function isValidDate(inDate:String):Boolean {    
    if (inDate == null){    
        return false;    
    }    
    // set the format to use as a constructor argument      
    if(isoToDate(inDate).valueOf()>0){
    	return true;
    }else{
    	return false;
    }   
}   

//排序
private function sortAc():ArrayCollection{   
    var sort:Sort=new Sort();   
    //按照setTime升序排序   
    sort.fields=[new SortField("setTime")];   
    moveimages.sort=sort;   
    moveimages.refresh();//更新   
    return moveimages;   
} 
//返回服务器时间字符串
 private function getStringTime():String{
 	var time:Date = getServerTime();
	fdate.formatString = "YYYY-MM-DD HH:NN:SS" ;
	return fdate.format(time) ; 
 	
 }
 
private function getServerTime():Date{
	var dt:Date = security.time;
	return dt;
}

// 初始化调用视频分类树型

private function getTreeVideoType():void{
   	videoTypeService.getTreeVideoType();
}
   
// 查询树型菜单视频专辑信息
private function getTreeVideoAlbum():void{
   	playInfoService.getTreeVideoAlbum();
}

//树视频类型
private function getTreeVideoTypeHandler(event:ResultEvent):void{
	videoTypeTree = XML(event.result);
   	type_tree.openItems=videoTypeTree.id.node;
   	type_tree.expandItem(videoTypeTree,true);
}

//树专辑
private function getTreeVideoAlbumHandler(event:ResultEvent):void{
   	videoAlbumTree = XML(event.result);
   	album_tree.openItems=videoAlbumTree.id.node;
   	album_tree.expandItem(videoAlbumTree,true); 
} 

//初始化查询视频专辑列表信息

private function getPageVideoAlbum():void{
	var videoAlbum:VideoAlbum = new VideoAlbum();
	//-2为查询视频个数大于0的专辑
	videoAlbum.videoNum = -2;
	videoAlbumService.getPageVideoAlbum(videoAlbum,1,14);
}

//响应查询视频专辑列表信息
private function getPageVideoAlbumsHandler(event:ResultEvent):void{
   	albumlist = event.result as Finder;
}

//选择视频专辑列表显示详细专辑信息
private function getVideoAlbumInfos(event:ListEvent):void{
	
}

//进入专辑，显示专辑视频列表
private function getAlbumVideos(event:ListEvent):void
{
	var videoAlbum:VideoAlbum =new VideoAlbum();
	videoWindow.visible=true;
	videoWindow.includeInLayout=true;
	
	albumWindow.visible=false;
	albumWindow.includeInLayout=false;
	viewVS.selectedChild = videoWindow;
//	this.maintainFocus();
	
	videoAlbum.albumId =TileList(event.currentTarget).selectedItem.albumId;
	albumIdTemp = videoAlbum.albumId.toString();
	typeIdTemp = "";
  	videoInfoService.getAlbumVideoInfo(videoAlbum,1,14);
}

private function doAlbumDragDrop(event:DragEvent):void{
	var albums:ArrayCollection = new ArrayCollection();	
	for(var i:int;i<albumtilelist.selectedItems.length;i++){
		var album:VideoAlbum = new VideoAlbum();
		album.albumId = albumtilelist.selectedItems[i].albumId;
		albums.addItem(album);		
	}
	videoInfoService.getAlbumVideoInfo3(albums);
	dragEvent = event;
}
 
private function getAlbumVideoInfo3Handler(event:ResultEvent):void{
	var videoInfos:ArrayCollection = event.result as ArrayCollection;
	isExistVideoInfo(videoInfos);	
}
private function isExistVideoInfo(videoInfos:ArrayCollection):void{	
	videolistTemp = new Array();
	var videolist:ArrayCollection = ArrayCollection(horizontallist.dataProvider);
	for(var i:int;i<videoInfos.length;i++){
		var flag:Boolean = false;
		for(var j:int=0;j<videolist.length;j++){
			if(videolist.getItemAt(j).videoId == videoInfos.getItemAt(i).videoId){			
				flag = true;				
			}			
		}
		if(!flag){
			videolistTemp.push(videoInfos.getItemAt(i));
		}	
	}
	doDragDrop(dragEvent);
}

private function changeHandler():void{
	this.videoinfo.videoName = "";
   	this.videoAlbum.albumName = "";
   	albumIdTemp = "";
   	typeIdTemp = "";
//   	maintainFocus();
	if(treeTab.selectedIndex == 0){
		clickVideoType();	
	}
	if(treeTab.selectedIndex == 1){
		clickVideoAlbum();
	}
}

private function clickVideoType():void{
	videoWindow.visible=true;
	videoWindow.includeInLayout=true;
	
	albumWindow.visible=false;
	albumWindow.includeInLayout=false;
	viewVS.selectedChild = videoWindow;
	var videoInfo:VideoInfo = new VideoInfo();
	playInfoService.getSelectVideo(videoInfo,1,14);
}

private function clickVideoAlbum():void{
	albumWindow.visible=true;
	albumWindow.includeInLayout=true;
	videoWindow.visible=false;
	videoWindow.includeInLayout=false;
	viewVS.selectedChild = albumWindow;		
	this.getPageVideoAlbum();
}

