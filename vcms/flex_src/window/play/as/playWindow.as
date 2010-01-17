import dto.security.client.TermInfo;
import dto.security.play.Playlist;
import dto.security.video.VideoInfo;

import mx.collections.ArrayCollection;
import mx.collections.Sort;
import mx.collections.SortField;
import mx.controls.Alert;
import mx.controls.dataGridClasses.DataGridColumn;
import mx.events.CloseEvent;
import mx.events.ItemClickEvent;
import mx.events.ListEvent;
import mx.managers.PopUpManager;
import mx.rpc.events.ResultEvent;

import pop.play.SetDownload;
import pop.play.SetScrollMes;
import pop.play.appointVideo;
import pop.play.tallVideo;

import util.page.Finder;

[Bindable] 
private var playlists:Finder;
[Bindable]
private var playvideo:ArrayCollection = new ArrayCollection();
[Bindable]
private var playterm:ArrayCollection = new ArrayCollection();

private var playinfo:Playlist = new Playlist();
private var videoinfo:VideoInfo = new VideoInfo();
private var terminfo:TermInfo = new TermInfo();
private var flag:int=0;
private var state:int;
	
//获取播放列表
private function oncreationComplete():void{
	//playBar.getChildAt(0).visible = false;
	var playinfo:Playlist = new Playlist();
	playInfoService.getSelectPlay(playinfo,1,20);  
}

//刷新播放列表 
private function regetPlay():void {
    var playinfo:Playlist = new Playlist();
	playInfoService.getSelectPlay(playinfo,1,20);
}

//查询播放单
public function getPaginations(page:int,pagesize:int):void{
	var playinfo:Playlist = new Playlist();
	playinfo.listName = listname.text; 
	playInfoService.getSelectPlay(playinfo,page,pagesize);
}

//设置条数
public function getPagesize(pageSize:int):void{
	playInfoService.getSelectPlay(playinfo,1,pageSize);
}

private function getSelectPlayHandler(event:ResultEvent):void{
	playlists = event.result as Finder;
}

//高级查询
private function tallselect():void{
	var tallvideo:tallVideo = new tallVideo();
	tallvideo.fatherfunction = this.queryResult;
	PopUpManager.addPopUp(tallvideo,this,true);
	PopUpManager.centerPopUp(tallvideo);	
}

//删除播放单
/* private function imageDrop(event:DragEvent):void{
	var item:Array = videolists.selectedItems;
	for(var i:int=0;i<item.length;i++){
		var del:int = playlists.list.getItemIndex(item[i]); 		
		playlists.list.removeItemAt(del);
		playInfoService.getDelPlay(item[i].listId);
	}				
} */

private function removePlay():void{
	var item:Array = videolists.selectedItems;
	if(item.length<=0){
		Alert.show("请选择要删除的播放单");
	}else{
//		for(var i:int=0;i<item.length;i++){
//			var del:int = playlists.list.getItemIndex(item[i]); 		
//			playlists.list.removeItemAt(del);
//			playInfoService.getDelPlay(item[i].listId);
//		}
		Alert.yesLabel="确定";
     	Alert.noLabel="取消";
       	Alert.show("你确认删除所选播放单吗?","提示信息",Alert.YES|Alert.NO,null,removePlayList);
	}
}

private function removePlayList(event:CloseEvent):void{
	if(event.detail==Alert.YES){
		var item:Array = videolists.selectedItems;
		for(var i:int=0;i<item.length;i++){
				var del:int = playlists.list.getItemIndex(item[i]); 		
				playlists.list.removeItemAt(del);
				playInfoService.getDelPlay(item[i].listId);
		}
	}
}
		
/* private function imageEnter(event:DragEvent):void{
	var obj:IUIComponent = IUIComponent(event.currentTarget);
	DragManager.acceptDragDrop(obj);
} */

//滚动字幕投影方法
private function captionFather(info:String):void{
		playinfo.listId = videolists.selectedItem.listId;
		playinfo.listName = videolists.selectedItem.listName;
		playinfo.createTime = videolists.selectedItem.createTime;
		playinfo.videoNum = videolists.selectedItem.videoNum;
		playinfo.playState = videolists.selectedItem.playState;	
		playInfoService.pushCaption(playinfo,info);	
}
private function clickHandler0(event:ItemClickEvent):void
{
	if(event.index==0)
	{
		var setScrollMes:SetScrollMes = new SetScrollMes();
		PopUpManager.addPopUp(setScrollMes,this,true);
	}
	else if(event.index==1)
	{
		var setDownload:SetDownload = new SetDownload();
		PopUpManager.addPopUp(setDownload,this,true);
	}
	//else if(event.index==2)
	//{
	//	var setWeb:SetWeb=new SetWeb();
	//	PopUpManager.addPopUp(setWeb,this,true);
	//}
}

/* private function changePlay(event:Event):void{
	var b0:ButtonBarButton = playBar.getChildAt(0) as ButtonBarButton;	
	var b1:ButtonBarButton = playBar.getChildAt(1) as ButtonBarButton;	
	var b2:ButtonBarButton = playBar.getChildAt(2) as ButtonBarButton;
	if(videolists.selectedItem.playState=="1"){
		b0.enabled = false;
		b1.enabled = false;
		b2.enabled = true;
	}else if(videolists.selectedItem.playState=="2"){
		b0.enabled = true;
		b1.enabled = false;
		b2.enabled = false;
	}else if(videolists.selectedItem.playState=="0"){
		b0.enabled = false;
		b1.enabled = true;
		b2.enabled = false;
	}else if(videolists.selectedItem.playState=="3"){
		b0.enabled = false;
		b1.enabled = true;
		b2.enabled = false;
	}	
} */

 private function clickHandler(event:ItemClickEvent):void{
//	if(event.index==0){
//		if(videolists.selectedItem==null){
//			Alert.show("请选择要插播的列表");
//		}else{
//			var insertPlay:insertPlayVideo = new insertPlayVideo();
//			insertPlay.listid = videolists.selectedItem.listId;
//			PopUpManager.addPopUp(insertPlay,this,true);
//			PopUpManager.centerPopUp(insertPlay);		
//		}
//	}
	if(event.index == 0){
		if(videolists.selectedItem == null){
			Alert.show("请选择要继立即播放的任务");
			return;
		}else{
			playinfo.listId = videolists.selectedItem.listId;	
			playInfoService.startPlay(playinfo);
		}
	}
	if(event.index==1){	
		if(videolists.selectedItem==null){
			Alert.show("请选择要暂停的播放列表");
			return;
		}else{
			state = 8001;
			playinfo.listId = videolists.selectedItem.listId;			
			playInfoService.stopPlay(playinfo,state);
		}
	}
	if(event.index == 2){
		if(videolists.selectedItem==null){
			Alert.show("请选择要继续播放的任务");
			return;
		}else{			
			state = 8002;
			playinfo.listId = videolists.selectedItem.listId;			
			playInfoService.stopPlay(playinfo,state);
		}
	}
}

//停止
private function stopPlayHandler(event:ResultEvent):void{
	var returnArray:ArrayCollection = event.result as ArrayCollection;
	var returnMsg:String = "";
	for(var i:int = 0;i<returnArray.length;i++){
		var returnString:String="";
		var clientName:String = returnArray.getItemAt(i).clientName;
		var returnMark:String = returnArray.getItemAt(i).returnMark;
//		if(returnMark==null){
//			Alert.show("试用期已结束","提示信息");
//			return;
//		}
		if(state == 8001){
			if(returnMark=='0'){
				returnString = "暂停成功！"+'\n';
//				playinfo.listId = videolists.selectedItem.listId;
//				playinfo.listName = videolists.selectedItem.listName;
//				playinfo.createTime = videolists.selectedItem.createTime;
//				playinfo.videoNum = videolists.selectedItem.videoNum;
//				playinfo.playState = "1";	
//				playInfoService.updateStartPlay(playinfo);
			}
			else if(returnMark == '1'){
				returnString = "暂停失败！"+'\n';
			}
			else if(returnMark == '2'){
				returnString = "应答超时！"+'\n';
			}
		}
		if(state == 8002){
			if(returnMark=='0'){
				returnString = "继续播放成功！"+'\n';
//				playinfo.listId = videolists.selectedItem.listId;
//				playinfo.listName = videolists.selectedItem.listName;
//				playinfo.createTime = videolists.selectedItem.createTime;
//				playinfo.videoNum = videolists.selectedItem.videoNum;
//				playinfo.playState = "0";
//				playInfoService.updateStartPlay(playinfo);
			}
			else if(returnMark == '1'){
				returnString = "继续播放失败！"+'\n';
			}
			else if(returnMark == '2'){
				returnString = "应答超时！"+'\n';
			}
		}		
		returnMsg+= "终端"+clientName+returnString;
	}
	Alert.show(returnMsg);
	oncreationComplete();
}

//播放
private function startPlayHandler(event:ResultEvent):void{
	var returnArray:ArrayCollection = event.result as ArrayCollection;
	var returnMsg:String = "";	
	for(var i:int = 0;i<returnArray.length;i++){
		var returnString:String="";
		var clientName:String = returnArray.getItemAt(i).clientName;
		var ReturnMark:String = returnArray.getItemAt(i).returnMark;
		
		if(ReturnMark=='0'){
			
			returnString = "播放失败！"+'\n';
		}
		else if(ReturnMark == '1'){
			returnString = "无法连接！"+'\n';
		}
		else if(ReturnMark == '2'){
			returnString = "播放成功！"+'\n';			
//			playinfo.listId = videolists.selectedItem.listId;
//			playinfo.listName = videolists.selectedItem.listName;
//			playinfo.createTime = videolists.selectedItem.createTime;
//			playinfo.videoNum = videolists.selectedItem.videoNum;
//			playinfo.playState = "0";	
//			playInfoService.updateStartPlay(playinfo);
		}
		else if(ReturnMark == '3'){
			returnString = "视频不存在，请点击'指定下载'按钮，将视频下载到终端";
		}
		else if(ReturnMark == '4'){
			returnString = "应答超时！"+'\n';
		}else if(ReturnMark == '5'){
			returnString = "播放列表中无视频"+'\n';
		}
		returnMsg+= "终端"+clientName+returnString;
		
	}
	if(returnArray.length==0){
		returnMsg = "播放失败！"+'\n';
	}
	Alert.show(returnMsg,"提示信息");
	oncreationComplete();
} 

private function clickvideoHandler(event:ItemClickEvent):void{
	if(event.index==0){
		flag=1;
		var appointvideo:appointVideo = new appointVideo();
		PopUpManager.addPopUp(appointvideo,this,true);	
		appointvideo.fatherfunction = this.getAddPlayLists;
		PopUpManager.centerPopUp(appointvideo);
	}else if(event.index==2){
		this.removePlay();
	}else if(event.index==1){
		flag=2;
		if(videolists.selectedItem==null){
			Alert.show("请选择要修改的播放列表");
			return;
		}else{			
			var updatePlay:appointVideo = new appointVideo();
			updatePlay.fatherfunction = this.getAddPlayLists;	
			updatePlay.listid = videolists.selectedItem.listId;
			updatePlay.listName = videolists.selectedItem.listName;
			updatePlay.playState = videolists.selectedItem.playState;
			updatePlay.trag = flag;
			PopUpManager.addPopUp(updatePlay,this,true);	
			PopUpManager.centerPopUp(updatePlay);
		}		
	}
}

//添加视频
private function getAddPlayLists():void{
	oncreationComplete();
}

private function selectedHandler(event:ListEvent):void
{
	playBar.enabled = true;
}

//查看播放单详细信息
private function clickMessage(event:ListEvent):void{
	//playMessage.visible = true;
	//playMessage.x = mouseX;
	//playMessage.y = mouseY;
	//playMessage.width = 300;
	//videolists.width = 100% - playMessage.width;
	playMessage.visible=true;
	playMessage.includeInLayout=true;
	playMessage.width = playListsBox.width * 0.35;
	playinfo.listId = videolists.selectedItem.listId;	
	playMessage.toolTip = "任务："+videolists.selectedItem.listName;
	playInfoService.getPlayVideo(playinfo);
	playInfoService.getPlayTerm(playinfo);	
/* 	videoname.text = "";
	filesize.text = "";
	timecount.text = "";
	userinfo.text = "";
	createtime.text = "";
	vodeoid.text = "";
	videotime.text = ""; */
}

private function closeHandler():void{
	playMessage.width = 0;
}

//private function showHandler():void{
//	playMessage.width = 400;
//	showIcon.visible=false;
//	showIcon.includeInLayout=false;
//}

private function getPlayVideoHandler(event:ResultEvent):void{	
	playvideo = event.result as ArrayCollection;	
	var seqTime:Object = new Object();
	var shipArray:ArrayCollection = new ArrayCollection();
	for(var i:int=0;i<playvideo.length;i++){
		seqTime = playvideo.getItemAt(i);
		for(var j:int=0;j<seqTime.playListShips.length;j++){
			if(seqTime.playListShips.getItemAt(j).playlist.listId == playinfo.listId){
				seqTime.setTime = seqTime.playListShips.getItemAt(j).listSeq;
				shipArray.addItem(seqTime);
			}
		}
	}
	
	playvideo = shipArray;
	//sortAc();
}

private function getPlayTermHandler(event:ResultEvent):void{
	playterm = event.result as ArrayCollection;
} 

private function queryResult(tallitem:Finder):void{
	playlists = tallitem;
}

//日期转换
private function getCreageTime(row:Object,column:DataGridColumn):String{
	return dateFormatter.format(row.createTime);	
}

//查看视频信息
/* private function getSelectVideo(event:ListEvent):void{
	var fdate:DateFormatter = new DateFormatter();
	fdate.formatString = "YYYY-MM-DD";
	picture.source = readlist.selectedItem.imagePath;
	videoname.text = readlist.selectedItem.videoName;
	filesize.text = readlist.selectedItem.fileSize;
	timecount.text = readlist.selectedItem.timeCount;
	userinfo.text = readlist.selectedItem.userInfo.userName;
	createtime.text = fdate.format(readlist.selectedItem.createTime);
	vodeoid.text = readlist.selectedItem.videoId;
	videoinfo.videoId = readlist.selectedItem.videoId;	
	playInfoService.selectVideoTime(playinfo,videoinfo);
}

private function selectVideoTimeHandler(event:ResultEvent):void{
	videotime.text = event.result as String;
} */

private var playId:int;
private var playName:String;
private var playTime:Date;
private var playNum:int;
private var playState:String;
//设置视频播放时间
/* private function setVideoTime():void{
	var playShip:PlayListShip = new PlayListShip();
	var playinfo:Playlist = new Playlist();
	var dt:Date = new Date();
	if(vodeoid.text==""||vodeoid.text==null){
		Alert.show("请单击视频列表选择要设置播放时间的视频");
		return;
	}else{
		if(videotime.text==null){
			this.closeWindows();
		}else{			
			playShip.listSeq = videotime.text;
			playShip.createTime = dt;	
			playInfoService.updatePlayShip(playinfo,videoinfo,playShip);
						
			playId = videolists.selectedItem.listId;
			playName = videolists.selectedItem.listName;
			playTime = videolists.selectedItem.createTime;
			playNum = videolists.selectedItem.videoNum;	
			playState = videolists.selectedItem.playState;
			
			playinfo.listId = playId;
			playinfo.listName = playName;
			playinfo.createTime = playTime;
			playinfo.videoNum = playNum;
			if(playState=="1"){
				playinfo.playState = "0";
			}else{
				playinfo.playState = playState;				
			}
			playInfoService.updateStartPlay(playinfo);				
		}		
	}
} */

//关闭
private function closeWindows():void{
	playMessage.visible=false
}

private function updatePlayShipHandler(event:ResultEvent):void{
	if(event.result!=null){
		Alert.show("设置视频播放时间成功");
	}
}

//更新播放单状态
private function updateStartPlayHandler(event:ResultEvent):void{
	if(event.result!=null){
		oncreationComplete();
	}
}
/* 
private function getPlayState(row:Object,column:DataGridColumn):String{
	var st:String;
	if(row.playState=="0"){
		st = "正在播放";
	}else if(row.playState=="1"){
		st = "未播放";
	}
	return st;
} */

//日期转换--开始时间 
private function getStartTime(row:Object,column:DataGridColumn):String{
	return dateFormatter.format(row.startTime);	
}

//日期转换--结束时间
private function getEndTime(row:Object,column:DataGridColumn):String{
	return dateFormatter.format(row.endTime);	
}

//排序
private function sortAc():ArrayCollection{   
    var sort:Sort=new Sort();   
    //按照setTime升序排序   
    sort.fields=[new SortField("setTime")];   
    playvideo.sort=sort;   
    playvideo.refresh();//更新   
    return playvideo;   
} 