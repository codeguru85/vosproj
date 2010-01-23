// ActionScript file
import dto.security.client.TermInfo;
import dto.security.video.VideoAlbum;
import dto.security.video.VideoInfo;
import dto.security.video.VideoType;

import mx.collections.ArrayCollection;
import mx.controls.Alert;
import mx.controls.buttonBarClasses.ButtonBarButton;
import mx.events.DragEvent;
import mx.events.ItemClickEvent;
import mx.events.ListEvent;
import mx.managers.PopUpManager;
import mx.rpc.events.ResultEvent;

import util.page.Finder;

[Bindable]	
private var videoTypeTree:XML;
[Bindable]	
private var videoAlbumTree:XML;
[Bindable]
private var typelist:XML;
// 选定的视频专辑树型节点的XML对象
private var selectedAlbumNode:XML;
[Bindable]
private var termList:Finder;
private var terminfo:TermInfo = new TermInfo();
[Bindable]
private var videoList:Finder;
[Bindable]
private var albumlist:Finder;
private var videoinfo:VideoInfo = new VideoInfo();
private var videotype:VideoType = new VideoType();
private var videoAlbum:VideoAlbum = new VideoAlbum();
private var dataProvider:ArrayCollection = new ArrayCollection();
private var albumIdTemp:String;
private var typeIdTemp:String;

private function close():void
{
	PopUpManager.removePopUp(this);
}

private function showTermGroup():void
{
	if(termGroupChk.selected==true)
	{
		termGroup.visible=true;
		termGroup.includeInLayout=true;
		termGroup.width=DGHB.width*0.29;
		TermOneDG.width=DGHB.width*0.69;
	}
	else
	{
		termGroup.visible=false;
//		termGroup.width=0;
		termGroup.includeInLayout=false;
		TermOneDG.width=DGHB.width;
	}
}

 /**
  * 格式化时间值
  * @return
  * */
 private function getCrateTimeFormatDate(value:Date):String{
 	return timeFormatter.format(value);
 }
private function videoOneDBClick():void
{
	var vi:VideoInfo =VideoInfo(VideoOneDG.selectedItem);
	videoInfo.labVideoName.text = vi.videoName;
	var videoType:ArrayCollection=vi.videoTypes;
	if(videoType.length>0){
		var strTypeName:String="";
		var strSign:String ="";
		for(var i:int=0;i<videoType.length;i++){
			if(i<videoType.length-1){
				strSign="，";
			}else{
				strSign="";
			}
			strTypeName =strTypeName+videoType.getItemAt(i).typeName+strSign;
		}
		videoInfo.labTypeName.text = strTypeName;
	}else{
		videoInfo.labTypeName.text ="无分类";
	}
	videoInfo.labTimeCount.text =vi.timeCount;
	videoInfo.labFileSize.text = vi.fileSize.toString()+"MB";
	videoInfo.labFileFormat.text = vi.fileFormat;
	videoInfo.labVideoDesc.text = vi.descn;
	videoInfo.labImage.source = vi.imagePath;
	videoInfo.labCreateTime.text = this.getCrateTimeFormatDate(vi.createTime);
	videoInfo.height = 260;
	videoInfo.labVideoDesc.height = 44;
	videoInfo.visible=true;
	videoInfo.x =mouseX;
	videoInfo.y =mouseY;
}
private function videoTwoDBClick():void
{
	var vi:VideoInfo =VideoInfo(VideoTwoDG.selectedItem);
	videoInfo.labVideoName.text = vi.videoName;
	var videoType:ArrayCollection=vi.videoTypes;
	if(videoType.length>0){
		var strTypeName:String="";
		var strSign:String ="";
		for(var i:int=0;i<videoType.length;i++){
			if(i<videoType.length-1){
				strSign="，";
			}else{
				strSign="";
			}
			strTypeName =strTypeName+videoType.getItemAt(i).typeName+strSign;
		}
		videoInfo.labTypeName.text = strTypeName;
	}else{
		videoInfo.labTypeName.text ="无分类";
	}
	videoInfo.labTimeCount.text =vi.timeCount;
	videoInfo.labFileSize.text = vi.fileSize.toString()+"MB";
	videoInfo.labFileFormat.text = vi.fileFormat;
	videoInfo.labVideoDesc.text = vi.descn;
	videoInfo.labImage.source = vi.imagePath;
	videoInfo.labCreateTime.text = this.getCrateTimeFormatDate(vi.createTime);
	videoInfo.visible=true;
	videoInfo.height = 260;
	videoInfo.labVideoDesc.height = 44;
	videoInfo.x=this.width/2-videoInfo.width/2;
	videoInfo.y=this.height/2-videoInfo.height/2;
//	videoInfo.x =mouseX-videoInfo.width;
//	videoInfo.y =mouseY;
}
private function termOneDbClick():void
{
	var te:TermInfo =TermInfo(TermOneDG.selectedItem);
	termInfo.labTermName.text = te.termName;
	termInfo.labTermIp.text = te.termIp;
	termInfo.labTermMask.text = te.termMask;
	termInfo.labTermGateway.text = te.termGateway;
	termInfo.labTermDns.text = te.termDns;
	termInfo.labTermAddr.text = te.termAddr;
	if(te.termType)
		termInfo.labType.text = te.termType.typeName;
	if(te.termActivation == 0){
		termInfo.labTermActivation.text = "否";
	}else{
		termInfo.labTermActivation.text = "是";
	}	 

	termInfo.visible=true;
	termInfo.x=this.width/2-termInfo.width/2;
	termInfo.y=this.height/2-termInfo.height/2;
//	termInfo.x =mouseX;
//	termInfo.y =mouseY;
}
private function termTwoDbClick():void
{
	var te:TermInfo =TermInfo(TermTwoDG.selectedItem);
	termInfo.labTermName.text = te.termName;
	termInfo.labTermIp.text = te.termIp;
	termInfo.labTermMask.text = te.termMask;
	termInfo.labTermGateway.text = te.termGateway;
	termInfo.labTermDns.text = te.termDns;
	termInfo.labTermAddr.text = te.termAddr;
	if(te.termType)
		termInfo.labType.text = te.termType.typeName;
	if(te.termActivation == 0){
		termInfo.labTermActivation.text = "否";
	}else{
		termInfo.labTermActivation.text = "是";
	}

	termInfo.visible=true;
	termInfo.x=this.width/2-termInfo.width/2;
	termInfo.y=this.height/2-termInfo.height/2;
//	termInfo.x =mouseX;
//	termInfo.y =mouseY-250;
}
private function delVideoHandler(event:ItemClickEvent):void
{
	var arr:ArrayCollection=ArrayCollection(VideoTwoDG.dataProvider);
	if(event.index==0)
	{
		var items:Array=VideoTwoDG.selectedItems;
		if(items.length==0)
			Alert.show("没有选择任何一行");
		else
		{
			for each(var item:Object in items)
			{
				arr.removeItemAt(arr.getItemIndex(item));
			}
		}
	}
	else if(event.index==1)
	{
		if(arr==null)
			Alert.show("没有视频");
		else arr.removeAll();
	}
}
private function delTermHandler(event:ItemClickEvent):void
{
	var arr:ArrayCollection=ArrayCollection(TermTwoDG.dataProvider);
	if(event.index==0)
	{
		var items:Array=TermTwoDG.selectedItems;
		if(items.length==0)
			Alert.show("没有选择任何一行");
		else
		{
			for each(var item:Object in items)
			{
				arr.removeItemAt(arr.getItemIndex(item));
			}
		}
	}
	else if(event.index==1)
	{
		if(arr==null)
			Alert.show("没有终端");
		else arr.removeAll();
	}
}
////查询视频类型
//private function getTreeVideoTypeHandler(event:ResultEvent):void
//{
//	videoTypeTree = XML(event.result);
//   	type_tree.openItems=videoTypeTree.id.node;
//   	type_tree.expandItem(videoTypeTree,true);
//}
//根据类型查询视频文件
private function getReadFile(event:Event):void
{	
	typelist=Tree(event.target).selectedItem as XML;
	var videoinfo:VideoInfo = new VideoInfo();
	typeIdTemp = typelist.@id.toString();
	albumIdTemp = "";
	if(typelist.@id=="all")
	{
		playInfoService.getSelectVideo(videoinfo,1,20);
	}
	else
	{
        var itemIsBranch:Boolean =type_tree.dataDescriptor.isBranch(typelist);
        if(!itemIsBranch)
        {
			videotype.typeId = typelist.@id;		
			playInfoService.getVideoTypes(videotype,1,20);
        }
	}
}

private function judgeVideoExist():void
{
	if(this.viewVS.selectedChild == videoWindow){
		var items:Array=VideoOneDG.selectedItems;
		var arr:ArrayCollection=ArrayCollection(VideoTwoDG.dataProvider);
		if(arr!=null)
		{
			for(var i:int=0;i<items.length;i++)
			{
				for(var j:int=0;j<arr.length;j++)
				if(items[i].videoId==arr[j].videoId)
				{
					Alert.show("视频 "+items[i].videoName+" 已经存在");
					return;
				}
			}
		}
	}
}

private function doDragDrop(event:DragEvent):void{
	if(this.viewVS.selectedChild == albumWindow){
		dataProvider = ArrayCollection(VideoTwoDG.dataProvider);
		event.preventDefault();
//		Alert.show("doDragDrop"+dataProvider.length.toString());
		var albums:ArrayCollection = new ArrayCollection();	
		for(var i:int;i<albumtilelist.selectedItems.length;i++){
			var album:VideoAlbum = new VideoAlbum();
			album.albumId = albumtilelist.selectedItems[i].albumId;
			albums.addItem(album);		
		}
		videoInfoService.getAlbumVideoInfo3(albums);
	}
}

private function getAlbumVideoInfo3Handler(event:ResultEvent):void{
	var videoInfos:ArrayCollection = event.result as ArrayCollection;
	if(videoInfos && videoInfos.length>0)
		isExistVideoInfo(videoInfos);	
}
private function isExistVideoInfo(videoInfos:ArrayCollection):void{
	var videolistTemp:ArrayCollection = dataProvider;
	if(dataProvider){
		for(var i:int;i<videoInfos.length;i++){
			var flag:Boolean = false;
			for(var j:int=0;j<dataProvider.length;j++){
				if(dataProvider.getItemAt(j).videoId == videoInfos.getItemAt(i).videoId){			
					flag = true;				
				}			
			}
			if(!flag){
				videolistTemp.addItem(videoInfos.getItemAt(i));
			}	
		}
		VideoTwoDG.dataProvider = videolistTemp;
	}else{
		VideoTwoDG.dataProvider = videoInfos;
	}
}

private function judgeTermExist():void
{
	var items:Array=TermOneDG.selectedItems;
	var arr:ArrayCollection=ArrayCollection(TermTwoDG.dataProvider);
	if(arr!=null)
	{
		for(var i:int=0;i<items.length;i++)
		{
			for(var j:int=0;j<arr.length;j++)
			if(items[i].termId==arr[j].termId)
			{
				Alert.show("终端 "+items[i].termName+" 已经存在");
				return;
			}
		}
	}
}
private function btnbarHandler(event:ItemClickEvent):void
{
 	var b0:ButtonBarButton = btnbar.getChildAt(0) as ButtonBarButton;
 	var b1:ButtonBarButton = btnbar.getChildAt(1) as ButtonBarButton;
 	if(event.index==0)
 	{
 		b0.enabled = false;
 		b1.enabled = true;
 		downloadVS.selectedChild = VB1;
		width=this.parentApplication.width*0.8;
		height=this.parentApplication.height*0.9;
//		this.width=1000;
//		this.height=600;
 	}
 	else if(event.index==1)
 	{
 		b0.enabled = true;
 		b1.enabled = false;
 		downloadVS.selectedChild = VB2; 
		width=this.parentApplication.width*0.64;
		height=this.parentApplication.height*0.9; 
// 		this.width=800;
// 		this.height=600;
 	}
 	else if(event.index==2)
 	{
 		this.addDownloadList();
 	}
 	else if(event.index==3)
 	{
		PopUpManager.removePopUp(this);
 	}
}

private function addDownloadList():void
{

	var videos:ArrayCollection = ArrayCollection(VideoTwoDG.dataProvider);
	var terms:ArrayCollection = ArrayCollection(TermTwoDG.dataProvider);

	if(videos==null)
	{
		Alert.show("请选择视频");
		return;
	}
	else if(terms==null)
	{
		Alert.show("请选择终端");
		return;
	}
	else
	{
		playInfoService.pushDownload(videos,terms);
	}
}

private function pushDownloadHandler(event:ResultEvent):void
{	
	if(event.result!=null)
	{		
		Alert.show(String(event.result));
		PopUpManager.removePopUp(this);
//		playInfoService.getSelectPlay(playinfo,1,20);
	}	
}

private function oncreationComplete():void
{
	var b1:ButtonBarButton = btnbar.getChildAt(0) as ButtonBarButton;
	b1.enabled = false;
	videoTypeService.getTreeVideoType();
	var terminfo:TermInfo = new TermInfo();
	//显示激活状态终端
	terminfo.termActivation = 1;
	var videoinfo:VideoInfo = new VideoInfo();
	playInfoService.getSelectTerm(terminfo,1,20);
	playInfoService.getSelectVideo(videoinfo,1,20);	
}

//获取终端
private function getSelectTermHandler(event:ResultEvent):void
{
	termList = event.result as Finder;
}

//获取视频
private function getVideoInfoHandler(event:ResultEvent):void{
	videoList = event.result as Finder;
	page.finder = videoList;
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

public function getPaginations(page:int,pagesize:int):void
{
	if(downloadVS.selectedChild == VB1)
	{
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
	if(downloadVS.selectedChild == VB2)
	{
		terminfo.termName = termname.text;
		terminfo.termAddr = termaddr.text;
		if(termip.value.toString()=="0.0.0.0")
		{
			terminfo.termIp = "";
		}
		else
		{
			terminfo.termIp = termip.value.toString();
		}
		playInfoService.getSelectTerm(terminfo,page,pagesize);	
	}
}
public function getPagesize(pagesize:int):void{
	if(downloadVS.selectedChild == VB1){
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
	if(downloadVS.selectedChild == VB2){
		playInfoService.getSelectTerm(terminfo,1,pagesize);
	}
}

/**
  *所属视频分类名称处理 
  * */
  private function getVideoTypesTypeName(row:Object,column:DataGridColumn):String
  {
  	var videoType:ArrayCollection = row.videoTypes;
  	if(videoType.length==1)
  	{
 		return videoType.getItemAt(0).typeName;
  	}else{
  		return "未分类";
  	}
  }
  
  /**
	 * 格式化文件大小(单位计算)
	 * @return
	 * */
	private function getFormatFileSize(row:Object,column:DataGridColumn):String{
		return row.fileSize + "MB";
	}
	
private function changeHandler():void{
	this.videoinfo.videoName = "";
   	this.videoAlbum.albumName = "";
   	albumIdTemp = "";
   	typeIdTemp = "";
   	videoInfo.visible = false;
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

	queryVideoBox.visible=true;
	queryVideoBox.includeInLayout=true;
	
	queryAlbumBox.visible=false;
	queryAlbumBox.includeInLayout=false;
	
	viewVS.selectedChild = videoWindow;
	var videoInfo:VideoInfo = new VideoInfo();
	playInfoService.getSelectVideo(videoInfo,1,14);
}

private function clickVideoAlbum():void{
	albumWindow.visible=true;
	albumWindow.includeInLayout=true;
	videoWindow.visible=false;
	videoWindow.includeInLayout=false;
	
	queryVideoBox.visible=false;
	queryVideoBox.includeInLayout=false;
	
	queryAlbumBox.visible=true;
	queryAlbumBox.includeInLayout=true;
	

	viewVS.selectedChild = albumWindow;		
	this.getPageVideoAlbum();
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
		
		queryVideoBox.visible=false;
		queryVideoBox.includeInLayout=false;
		
		queryAlbumBox.visible=true;
		queryAlbumBox.includeInLayout=true;

		viewVS.selectedChild = albumWindow;
		
		this.getPageVideoAlbum();
  	}else{
  		 var itemIsBranch:Boolean =album_tree.dataDescriptor.isBranch(selectedAlbumNode);
  		if(!itemIsBranch)
  		{
  			videoWindow.visible=true;
			videoWindow.includeInLayout=true;
			
			albumWindow.visible=false;
			albumWindow.includeInLayout=false;
			
			queryVideoBox.visible=true;
			queryVideoBox.includeInLayout=true;
			
			queryAlbumBox.visible=false;
			queryAlbumBox.includeInLayout=false;

			viewVS.selectedChild = videoWindow;
			
  			var videoAlbum:VideoAlbum = new VideoAlbum();
  			videoAlbum.albumId =selectedAlbumNode.@id;
  			videoInfoService.getAlbumVideoInfo(videoAlbum,1,14);
  		} 
  	}
}

//初始化查询视频专辑列表信息

private function getPageVideoAlbum():void{
	var videoAlbum:VideoAlbum = new VideoAlbum();
	//-2为查询视频个数大于0的专辑
	videoAlbum.videoNum = -2;
	videoAlbumService.getPageVideoAlbum(videoAlbum,1,18);
}

private function getAlbumVideoInfoHandler(event:ResultEvent):void
{
  	videoList = event.result as Finder;  
  	page.finder = videoList;	
}

//响应查询视频专辑列表信息
private function getPageVideoAlbumsHandler(event:ResultEvent):void{
   	albumlist = event.result as Finder;
   	page.finder = albumlist;
}
//进入专辑，显示专辑视频列表
private function getAlbumVideos(event:ListEvent):void
{
	var videoAlbum:VideoAlbum =new VideoAlbum();
	videoWindow.visible=true;
	videoWindow.includeInLayout=true;
	
	albumWindow.visible=false;
	albumWindow.includeInLayout=false;
	
	queryVideoBox.visible = true;
	queryVideoBox.includeInLayout=true;
	
	queryAlbumBox.visible=false;
	queryAlbumBox.includeInLayout=false;

	viewVS.selectedChild = videoWindow;
	
	videoAlbum.albumId =TileList(event.currentTarget).selectedItem.albumId;
	albumIdTemp = videoAlbum.albumId.toString();
	typeIdTemp = "";
  	videoInfoService.getAlbumVideoInfo(videoAlbum,1,14);
}