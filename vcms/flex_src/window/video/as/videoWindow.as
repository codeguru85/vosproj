// ActionScript file
	import dto.security.video.Keyword;
	import dto.security.video.VideoAlbum;
	import dto.security.video.VideoAlbumShip;
	import dto.security.video.VideoInfo;
	import dto.security.video.VideoInfoBean;
	import dto.security.video.VideoType;
	
	import flash.events.Event;
	
	import mx.collections.ArrayCollection;
	import mx.controls.Alert;
	import mx.controls.TileList;
	import mx.controls.dataGridClasses.DataGridColumn;
	import mx.events.CloseEvent;
	import mx.events.ItemClickEvent;
	import mx.events.ListEvent;
	import mx.managers.PopUpManager;
	import mx.rpc.events.FaultEvent;
	import mx.rpc.events.ResultEvent;
	
	import pop.video.AddAlbumVideo;
	import pop.video.AddVideoAlbum;
	import pop.video.AddVideoWindow;
	import pop.video.EditVideoAlbum;
	import pop.video.EditVideoWindow;
	import pop.video.QueryVideoAlbum;
	import pop.video.QueryVideoInfo;
	import pop.video.QueryVideoType;
	import pop.video.VideoTypeCptPrep;
	
	import util.page.Finder;
	
	/**
	 * 视频信息集合
	 * */
  	[Bindable]
  	private var videoInfos:Finder;

  	[Bindable]
  	private var videoinfos2:Finder;  	
  	
  	/**
  	 * 视频专辑列表集合
  	 * */
  	[Bindable]
  	private var videoAlbums:Finder;
  	
  	/**
  	 * 设置默认显示记录数
  	 * */
  	private var thisPageSize:String = '5';
  	
  	/**
  	 * 定义选定的视频专辑树型节点的XML对象
  	 * */
  	private var selectedAlbumNode:XML;
  	
  	/**
  	 * 定义选定的视频分类树形节点的XML对象
  	 * */
  	private var selectedTypeNode:XML;
  	
  	/**
  	 * 定义专辑树型菜单数据
  	 * */
  	[Bindable]
  	private var videoAlbumTree:XML;
  	
  	/**
  	 * 定义选择的树型节点的XML对象
  	 * */
  	[Bindable]
  	private var videoTypeTree:XML;
  	
  	/**
  	 * 选择的专辑ID
  	 * */
  	private var videoAlbumId:int;
  	
  	/**
  	 * 选择视频分类
  	 * */
  	private var videoTypeIdTemp:String;
  	
  	 /**
  	 * 专辑图片信息地址
  	 * */
  	private var albumImageTemp:String;
  		
	 /**
  	 * 是否专辑封面标志
  	 * */
  	private var AbulmCoverFlag:Boolean = false;
  	
  	 /**
  	 * 是否专辑封面设置操作
  	 * */
	private var setCoverFlag:Boolean = false;
  	
  	/**
  	 * 选择视频专辑
  	 * */
  	private var videoAlbumIdTemp:String; 	
  	
  	/**
  	 * 高级查询参数的传递
  	 * */
  	private var videoInfoTemp:VideoInfoBean = new VideoInfoBean();
	
  	/**
  	 * 查询类型的定义
  	 * */
  	[Bindable]
  	private var queryTypes:ArrayCollection =new ArrayCollection(
  				[ {label:"视频标题", data:1},
  				   {label:"关键字",data:2}
                ]);
                

  	/**
  	 * 组件初始化应用
  	 * */
  	private function initApp():void{
  		menuTree.selectedIndex=0;
 		this.getTreeVideoType();
 		this.getVideoInfos();
  	}
  	
  	/**
  	 * 初始化查询视频信息
  	 **/
  	 private function getVideoInfos():void{
  	 	var videoInfo:VideoInfo = new VideoInfo();
  	 	videoTypeIdTemp="";
  	 	videoAlbumIdTemp="";
  	 	videoInfoTemp =null;
  	 	videoInfoService.getPageVideoInfo(null,1,5);
  	 }
  	
  	/**
  	 * 响应视频列表查询数据分页
  	 * */
  	private function getPageVideoInfoHandler(event:ResultEvent):void
  	{
  			videoInfos = event.result as Finder;
  	}
  	
  	
  	 /**
  	 * 显示专辑列表
  	 * */
  	 private function getAlbumList():void{
		videoListView.visible=false;
		videoListView.includeInLayout=false;
		
		albumListView.visible=true;
		albumListView.includeInLayout=true;
  	 	
  	 	videoVS.selectedChild=albumList;
  	 	this.getPageVideoAlbum();
  	 }
  	
  	/**
  	 * 响应选择树型菜单
  	 * */
  	 private function clickAlbumTree():void{
  	 	this.getTreeVideoAlbum();
  	 }
  	
  	/**
  	 * 初始化调用视频分类树型
  	 * */
  	 private function getTreeVideoType():void{
  	 	videoTypeService.getTreeVideoType();
  	 }
  	 
  	 /**
  	 * 初始化视频分类信息
  	 * */
  	 private function getChangeTypeHandler(event:Event):void
  	 {
	  	 	selectedTypeNode =Tree(event.target).selectedItem as XML;
			if(selectedTypeNode.@id=="all"){
	 	 		this.getVideoInfos();
	 	 	}else{
                var itemIsBranch:Boolean =type_tree.dataDescriptor.isBranch(selectedTypeNode);
                if(!itemIsBranch)
                {
	 	 			var videoType:VideoType =new VideoType();
	 	 			videoType.typeId = selectedTypeNode.@id;
	 	 			videoTypeIdTemp  = videoType.typeId.toString();
	 	 			videoInfoService.getTypeVideoInfoType(videoType,1,5);
                }
	 	 	}
  	 }
  	 
  	 /**
  	 * 根据视频分类查询视频列表信息
  	 * */
  	 private function getTypeVideoInfoTypeHandler(event:ResultEvent):void
  	 {
  	 	videoInfos = event.result as Finder;
  	 }
  	 
  	  /**
 	 * 根据视频专辑菜单查询视频列表信息
 	 * */
 	 private function getChangeAlbumHander(event:Event):void
 	 {
 	 	selectedAlbumNode =Tree(event.target).selectedItem as XML;
		if(selectedAlbumNode.@id=="all"){
 	 		this.getVideoInfos();
 	 	}else{
 	 		var itemIsBranch:Boolean =album_tree.dataDescriptor.isBranch(selectedAlbumNode);
 	 		if(!itemIsBranch)
 	 		{
 	 			var videoAlbum:VideoAlbum = new VideoAlbum();
 	 			videoAlbum.albumId =selectedAlbumNode.@id;
 	 			videoAlbumIdTemp = videoAlbum.albumId.toString();
 	 			videoInfoService.getAlbumVideoInfo(videoAlbum,1,5);
 	 		}
 	 	}
 	 }
 	 
 	 /**
 	 * 响应视频专辑菜单查询结果列表
 	 * */
 	 private function getAlbumVideoInfoHandler(event:ResultEvent):void
 	 {
 	 	if(videoVS.selectedChild==videoList)
 	 	{
 	 		videoInfos = event.result as Finder;
 	 	}
 	 	else if(videoVS.selectedChild==album)
 	 	{
 	 		videoinfos2 = event.result as Finder;
 	 	}
 	 }
  	 
  	/**
  	 * 查询树型视频分类信息响应调用
  	 * */
  	 private function getTreeVideoTypeHandler(event:ResultEvent):void{
  	 	var xml:XML =<Node id='all' label='全部'></Node>;
  	 	videoTypeTree = XML(event.result);
  	 	videoTypeTree.prependChild(xml);
  	 	type_tree.openItems=videoTypeTree.id.node;
  	 	type_tree.expandItem(videoTypeTree,true);
  	 }
  	 /**
  	 * 查询树型菜单视频专辑信息
  	 * */
  	 private function getTreeVideoAlbum():void{
  	 	videoAlbumService.getTreeVideoAlbum();
  	 }
  	 /**
  	 * 查询树型菜单响应视频专辑信息
  	 * */
  	 private function getTreeVideoAlbumHandler(event:ResultEvent):void{
  	 	videoAlbumTree = XML(event.result);
  	 	album_tree.openItems=videoAlbumTree.id.node;
  	 	album_tree.expandItem(videoAlbumTree,true);
  	 }
  	
  	 /**
  	 * 初始化查询视频专辑列表信息
  	 * */
  	 private function getPageVideoAlbum():void{
  	 	videoAlbumService.getPageVideoAlbum(null,1,24);
  	 }
  	 
  	 /**
  	 * 选择视频专辑列表显示详细专辑信息
  	 * */
  	 private function getVideoAlbumInfos(event:ListEvent):void{

		var fdate:DateFormatter = new DateFormatter();
		fdate.formatString = "YYYY-MM-DD JJ:NN:SS";
		
		picture.source = event.itemRenderer.data.imagePath;
		albumImageTemp = event.itemRenderer.data.imagePath;
		albumName.text = event.itemRenderer.data.albumName;	
		videoNum.text = event.itemRenderer.data.videoNum;
		albumDesc.text = event.itemRenderer.data.descn;
		createTime.text = fdate.format(event.itemRenderer.data.createTime);
  	 }
  	 
  	 /**
  	 * 响应查询视频专辑列表信息
  	 * */
  	 private function getPageVideoAlbumsHandler(event:ResultEvent):void
  	 {
  	 	videoAlbums = event.result as Finder;
  	 }
 	 
  	/**
  	 * 查询信息设置
  	 * */
  	 private function getVideoInfoByParaments():void{
  	 	var videoInfo:VideoInfo =new VideoInfo();
  	 	var keyword:Keyword = new Keyword();
  	 	
  	 	
  	 	if(labelType.selectedItem.data==1){
  	 		videoInfo.videoName = txtVideoName.text;
  	 		videoInfoService.getPageVideoInfo(videoInfo,1,5);
  	 	}else{
  	 		keyword.wordName = txtVideoName.text;
  	 		
  	 		videoInfoService.getKeyWordVideoInfo(keyword,1,5);
  	 	}
  	 }
  	 
  	 /**
  	 * 相应通过关键字查询数据分页
  	 * */
  	private function getKeyWordVideoInfoHandler(event:ResultEvent):void
  	{
  		videoInfos = event.result as Finder;
  	}
  	 /**
  	 * 关于视频专辑的查询
  	 * */
  	 private function getVideoAlbumVideoinfoByParaments():void
  	 {
  	 	var videoAlbum:VideoAlbum = new VideoAlbum();
  	 	var videoInfo:VideoInfo =new VideoInfo();
  	 }
  	
  	
  	/**
  	 * 执行高级查询后的赋值
  	 * */
  	 private function getResultVideoInfo(queryVideoInfo:Finder,videoInfoBean:VideoInfoBean):void
  	 {
  	 	videoInfos = queryVideoInfo;
  	 	videoInfoTemp = videoInfoBean;
  	 }
  	
  	/**
  	 * 分页按钮调用的分页方法
  	 * */
  	 public function getPaginations(toPage:int,pageSize:int):void
  	 {
  	 	var videoInfo:VideoInfo =new VideoInfo();
		var videoAlbum:VideoAlbum =new VideoAlbum();
		var videoType:VideoType = new VideoType();
			 	
	  	 	if(videoVS.selectedIndex==0)
	  	 	{
	  	 		if(menuTree.selectedIndex==0){
	  	 			/**
	  	 			 * 视频分类的处理
	  	 			 * */
		  	 		if(videoTypeIdTemp && videoTypeIdTemp!="all")
			  	 	{
			  	 		videoType.typeId = parseInt(videoTypeIdTemp);
			  	 		videoInfoService.getTypeVideoInfoType(videoType,toPage,pageSize);
			  	 	}else{
			  	 		
			  	 		/**
			  	 		 * 普通查询
			  	 		 * */
		  	 			 videoInfo.videoName = txtVideoName.text;
		  	 			
		  	 			/**
		  	 			 * 高级查询传值
		  	 			 * */
		  	 			 if(videoInfoTemp){
		  	 			 	videoInfoService.getMoreVideoInfo(videoInfoTemp,toPage,pageSize);
		  	 			 }else{
		  	 			 	videoInfoService.getPageVideoInfo(videoInfo,toPage,pageSize);
		  	 			 }
		  	 
		  	 		}
		  	 	}else{
		  	 		/**
		  	 		 * 视频专辑的处理
		  	 		 * */
		  	 		if(videoAlbumIdTemp && videoAlbumIdTemp!="all"){
		  	 			videoAlbum.albumId =parseInt(videoAlbumIdTemp);
	  	 				videoInfoService.getAlbumVideoInfo(videoAlbum,toPage,pageSize);
		  	 		}else{
		  	 			/**
					  	 * 普通查询
					  	 * */ 			
		  	 			 videoInfo.videoName = txtVideoName.text;
		  	 			 
		  	 			/**
		  	 			 * 高级查询传值
		  	 			 * */
		  	 			 if(videoInfoTemp){
		  	 			 	videoInfoService.getMoreVideoInfo(videoInfoTemp,toPage,pageSize);
		  	 			 }else{
		  	 			 	videoInfoService.getPageVideoInfo(videoInfo,toPage,pageSize);
		  	 			 }
		  	 			 
		  	 		}
		  	 	}
	  	 	}
	  	 	else if(videoVS.selectedIndex==1)
	  	 	{	
	  	 		/**
	  	 		 *专辑管理列表的处理
	  	 		 * */
	  	 		videoAlbum.albumName = txtAlbumName.text;
	  	 		videoAlbumService.getPageVideoAlbum(videoAlbum,toPage,pageSize);
	  	 	}
	  	 	else if(videoVS.selectedIndex==2)
	  	 	{
	  	 		/**
	  	 		 * 专辑管理中视频列表的处理
	  	 		 * */
	  	 		videoAlbum.albumId =videoAlbumId;
	  	 		videoInfoService.getAlbumVideoInfo(videoAlbum,toPage,pageSize);
	  	 	}
	  	 
  	 }
  	 
  	/**
  	 * 设置每页记录数
  	 * */
  	 public function getPagesize(pageSize:String):void{
  	 	thisPageSize = pageSize;
  	 	var videoInfo:VideoInfo = new VideoInfo();
  	 	var videoAlbum:VideoAlbum = new VideoAlbum();
  	 	var videoType:VideoType = new VideoType();
  	 	
  	 	
  	 	/**
  	 	 * 视频列表信息参数传递
  	 	 * */
  	 	if(videoVS.selectedIndex==0)
  	 	{
 
	  	 	if(menuTree.selectedIndex==0)
	  	 	{
	  	 		/**
	  	 		 * 视频分类的处理
	  	 		 * */
		  	 	if(videoTypeIdTemp)
				{
					if(videoTypeIdTemp != "all"){
						videoType.typeId = parseInt(videoTypeIdTemp);
					}
		  	 		videoInfoService.getTypeVideoInfoType(videoType,1,pageSize);
		  	 	}else{
	  	 			
	  	 			if(videoInfoTemp)
		  	 		{
		  	 			videoInfoService.getPageVideoInfo(videoInfoTemp,1,pageSize);
		  	 		}else{
		  	 			videoInfoService.getPageVideoInfo(videoInfo,1,pageSize);
		  	 		}
	  	 		}
	  	 	}else{
	  	 		/**
	  	 		 * 视频专辑的处理
	  	 		 * */
	  	 		if(videoAlbumIdTemp)
	  	 		{
  	 				if(videoAlbumIdTemp!="all"){
	  	 				videoAlbum.albumId =parseInt(videoAlbumIdTemp);
	  	 			}
	  	 			videoInfoService.getAlbumVideoInfo(videoAlbum,1,pageSize);
	  	 		}else{
	  	 			
	  	 			if(videoInfoTemp)
		  	 		{
		  	 			videoInfoService.getPageVideoInfo(videoInfoTemp,1,pageSize);
		  	 		}else{
		  	 			videoInfoService.getPageVideoInfo(videoInfo,1,pageSize);
		  	 		}
	  	 		}
	  	 	}
  	 	}
  	 	else if(videoVS.selectedIndex==1)
  	 	{
  	 		/**
  	 		 * 专辑管理列表的处理
  	 		 * */
			videoAlbumService.getPageVideoAlbum(videoAlbum,1,pageSize);
  	 	}
  	 	else if(videoVS.selectedIndex==2)
  	 	{
  	 		/**
  	 		 * 专辑管理中的视频列表处理
  	 		 * */
  	 		videoAlbum.albumId =videoAlbumId;
  	 		videoInfoService.getAlbumVideoInfo(videoAlbum,1,pageSize);
  	 	
  	 	}
  	 	
  	 }
  	
  	/**
  	 * 双击响应单条视频信息
  	 * */
 	private function itemDoubleClickHandler(event:ListEvent):void{
			var vi:VideoInfo;
			if(videoVS.selectedIndex==0)
			{
				vi =VideoInfo(videoInfoDG.selectedItem);
			}else if(videoVS.selectedIndex==2)
			{
				vi =VideoInfo(videoInfoDG2.selectedItem);
			}
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
			videoInfo.x =mouseX;
			videoInfo.y =mouseY;

 	}
 	
 	/**
 	 * 获得单项视频信息
 	 * */
 	private function getClickHandler(event:ItemClickEvent):void{
 		if(event.index==0){
 			this.addVideoInfo();
 		}else if(event.index==1){
			this.editVideoInfo();
 		}else if(event.index==2){
			this.removeVideoInfo();
 		}
 	}
 	/**
 	 * 新增视频信息
 	 * */
 	 private function addVideoInfo():void{
 	 	var addVideo:AddVideoWindow =new AddVideoWindow();
 	 		addVideo.fatherFunction = this.initApp;
// 			PopUpManager.addPopUp(addVideo,this,true);
// 			PopUpManager.centerPopUp(addVideo);
		addVideo.title = "上传视频";			
		 if(this.windowManager.windowList.length >= 3){
			return;	
		}else{		
				windowManager.add(addVideo);
				if(this.windowManager.windowList.length == 1){
					addVideo.x = 190;
					addVideo.y = 40;
				}
		}
				
 	 }
 	 
 	 /**
 	 * 修改视频信息
 	 * */
 	 private function editVideoInfo():void
 	 {
  		if(videoInfoDG.selectedItem==null){
 			Alert.show("请选择一个视频","提示信息");
 		}else{
 			var videoInfo:VideoInfo =VideoInfo(videoInfoDG.selectedItem);
 			var editVideoWindow:EditVideoWindow =EditVideoWindow(PopUpManager.createPopUp(this,EditVideoWindow,true));
 			editVideoWindow.videoId = videoInfo.videoId;
 			editVideoWindow.txtVideoName.text = videoInfo.videoName;
 			editVideoWindow.beginVideoName = videoInfo.videoName;
 			editVideoWindow.txtDesc.text =videoInfo.descn;
 			editVideoWindow.videoTemp = videoInfo;
 			//var videoType:ArrayCollection =videoInfo.videoTypes; 			
 			//editVideoWindow.types = videoType;
 			//if(videoType.length==1)
 			//{
 			//	var rootId:int =videoType.getItemAt(0).videoType.typeId;
 			//	var parentId:int=videoType.getItemAt(0).typeId;
 				//editVideoWindow.txtTypeId.getRootVideoTypeSetSelectIndex(rootId);
 				//editVideoWindow.txtTypeId.getParentVideoTypes(rootId);
 				//editVideoWindow.txtTypeId.getSelectIndex(parentId);
 			//	editVideoWindow.rootTypeId =rootId;
 			//	editVideoWindow.parentTypeId =parentId;
 			//}
 			//var videoAlbum:ArrayCollection = videoInfo.videoAlbumShips;
 			//if(videoAlbum.length==1){
 			//	var albumId:int=videoInfo.videoAlbumShips.getItemAt(0).videoAlbum.albumId;
 			//	var albumName:String=videoInfo.videoAlbumShips.getItemAt(0).videoAlbum.albumName;
 			//	editVideoWindow.txtAlbumId.text=albumId.toString();
 			//	editVideoWindow.txtAlbumName.text=albumName;
 			//}
 			//editVideoWindow.curAlbumId=albumId;
 			var keywords:ArrayCollection = videoInfo.keywords;
 			if(keywords.length>0){
 				var strWordName:String="";
 				var strSign:String ="";
 				for(var i:int=0;i<keywords.length;i++){
					if(i<keywords.length-1){
						strSign=",";
					}else{
						strSign="";
					}
					strWordName =strWordName+keywords.getItemAt(i).wordName+strSign;
				}
				editVideoWindow.txtKeyWords.text = strWordName;
 			}else{
 				editVideoWindow.txtKeyWords.text="";
 			}
 			
 			editVideoWindow.fatherFunction =this.initApp;
 			PopUpManager.centerPopUp(editVideoWindow);
 		}
 	 }
 	 
 	/**
 	 * 删除视频信息
 	 * */
 	 private function removeVideoInfo():void{
 	 	
 	 	var items:Array = videoInfoDG.selectedItems;
 	 	if(items.length<=0){
 	 		Alert.show("请选择要删除的视频信息","提示信息");
 	 		return;
 	 	}else{
 	 		Alert.yesLabel="确定";
         	Alert.noLabel="取消";
           	Alert.show("你确认删除所选视频吗?","提示信息",Alert.YES|Alert.NO,null,deleteVideoInfo);
 	 	}
 	 }
 	 
 	 /**
 	 * 删除视频信息
 	 * */
 	 private function deleteVideoInfo(event:CloseEvent):void
 	 {
 	 	if(event.detail==Alert.YES){
 	 		var items:Array = videoInfoDG.selectedItems;
 	 		for(var i:int=0;i<items.length;i++){
 	 			var index:int=videoInfos.list.getItemIndex(items[i]);
 	 			videoInfos.list.removeItemAt(index);
 	 			videoInfoService.removeVideoInfo(items[i]);
 	 			
 	 		}
 	 	}
 	 }
 	 
 	 /**
 	 * 删除视频信息调用
 	 * */
 	 private function removeVideoInfoHandler(event:ResultEvent):void{
 	 	var resVideoInfo:VideoInfo = VideoInfo(event.result);
 	 	if(resVideoInfo==null){
 	 		Alert.show("此视频已被收藏在播放任务内,请先移除播放列表","提示信息");
 			return;
 		}	
 		this.getVideoInfos();
 		this.getTreeVideoType(); 		
 		this.isAlbumsCovers(resVideoInfo);	 
 	 }
 	 
 	  /**
 	 * 删除信息是否为所有所在专辑封面的处理
 	 * */
 	 private function isAlbumsCovers(videoInfo:VideoInfo):void{ 
 	 	for(var i:int=0;i<videoInfo.videoAlbumShips.length;i++){
 	 		var videoAlbumShip:VideoAlbumShip = videoInfo.videoAlbumShips.getItemAt(i) as VideoAlbumShip;
 	 		if(videoAlbumShip.videoAlbum.imagePath == videoInfo.imagePath){	 			
 	 			videoAlbumShip.videoAlbum.imagePath = './images/default-album.png';
 	 			videoAlbumService.updateVideoAlbum(videoAlbumShip.videoAlbum,videoAlbumShip.videoAlbum.albumId);
 	 		}
 	 	}
 	 	if(this.isShowCover(videoInfo)){
 			picture.source = './images/default-album.png';
 		}			 			
 	 }
 	 
 	 /**
 	 * 删除信息是否为正在显示的封面
 	 * */
 	 private function isShowCover(videoInfo:VideoInfo):Boolean{	 
 	 	if(videoInfo.imagePath == albumImageTemp){
 	 		var isFlag:Boolean = true;
 	 	}else
 	 		isFlag = false;
 	 	return isFlag;	
 	 }
 	 
 	 /**
 	 * 删除异常提示信息处理
 	 * */
 	 private function getExecptionHander(event:FaultEvent):void{
 	 	Alert.show("此视频处于播放状态,请先移除播放列表","提示信息");
 	 	return;
 	 }
 	 
 	 /**
  	 * 高级查询窗口调用
  	 * */
  	 private function getMoreVideoInfo():void{
  	 	var queryVideoInfo:QueryVideoInfo =new QueryVideoInfo();
  	 	queryVideoInfo.fatherFunction = this.getResultVideoInfo;
  	 	PopUpManager.addPopUp(queryVideoInfo,this,true);
 		PopUpManager.centerPopUp(queryVideoInfo);
  	 }
 	
 	/**
 	 * 加入专辑
 	 * */
 	private function addClickHandler():void{
 			var items:Array = videoInfoDG.selectedItems;
 			if(items.length<=0){
 				Alert.show("请选择你要加入的视频","提示信息");
 			}else{
 				var videoInfo:VideoInfo = VideoInfo(videoInfoDG.selectedItem);
 				var queryVideoAlbum:QueryVideoAlbum=QueryVideoAlbum(PopUpManager.createPopUp(this,QueryVideoAlbum,true));
 				queryVideoAlbum.targetWindow="addAlbum";
 				queryVideoAlbum.videoIds =items;
 				PopUpManager.centerPopUp(queryVideoAlbum);
 			}
 	}
 	
 	/**
 	 * 分类管理
 	 * */
// 	private function getVideoType():void{
// 		var videoType:VideoTypeCpt=new VideoTypeCpt();
// 		videoType.fatherFunction=this.getVideoTypeColoseHandler;
// 		PopUpManager.addPopUp(videoType,this,true);
// 		PopUpManager.centerPopUp(videoType);
// 	}
 	
 	/**
 	 * 分类管理
 	 * */
 	private function getVideoTypePrep():void
 	{
 		var videoType:VideoTypeCptPrep=new VideoTypeCptPrep();
 		videoType.fatherFunction=this.getVideoTypeColoseHandler;
 		PopUpManager.addPopUp(videoType,this,true);
 		PopUpManager.centerPopUp(videoType);
 	}
 	
 	/**
 	 * 专辑管理
 	 * */
// 	private function getVideoAlbum():void{
// 		var videoAlbum:VideoAlbumCpt=new VideoAlbumCpt();
// 		videoAlbum.fatherFunction = this.getVideoAlbumCloseHandler;
// 		PopUpManager.addPopUp(videoAlbum,this,true);
// 		PopUpManager.centerPopUp(videoAlbum);
// 	}
 	/**
 	 * 关闭分类管理并更新树
 	 * */
 	 private function getVideoTypeColoseHandler():void
 	 {
 	 	menuTree.selectedIndex=0;
 	 	this.getTreeVideoType();
 	 }
 	
 	/**
 	 * 关闭专辑管理并更新树
 	 * */
 	 private function getVideoAlbumCloseHandler():void
 	 {
 	 	menuTree.selectedIndex=1;
 	 	this.getTreeVideoAlbum();
 	 }
 	
 	/**
 	 * 快捷专辑窗口
 	 * */
 	 private function getQueryVideoAlbum():void{
 	 	var queryVideoAlbum:QueryVideoAlbum = new QueryVideoAlbum();
 	 	 queryVideoAlbum.targetWindow="more";
 	 	 PopUpManager.addPopUp(queryVideoAlbum,this,true);
 	 	 PopUpManager.centerPopUp(queryVideoAlbum);
 	 }
 	 
 	 /**
 	 * 快捷分类窗口
 	 * */
 	 private function getQueryVideoType():void{
 	 	var queryVideoType:QueryVideoType = new QueryVideoType();
 	 	queryVideoType.targetWindow="more";
 	 	PopUpManager.addPopUp(queryVideoType,this,true);
 	 	PopUpManager.centerPopUp(queryVideoType);
 	 }
 	 
 	
 	/**
 	 * 格式化时间值
 	 * @return
 	 * */
 	private function getCrateTimeFormatDate(value:Date):String{
 		return timeFormatter.format(value);
 	}
 	/**
 	 * 格式化创建时间
 	 * @return
 	 * */
	private function getFormatCreateTime(row:Object,column:DataGridColumn):String{
		return timeFormatter.format(row.createTime);
	}
	/**
	 * 格式化名称
	 * @reutrn
	 * */
	private function getUserName(row:Object,column:DataGridColumn):String{
		return row.userInfo.userName;
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
  	 *文件格式信息 
  	 * */
  	 private function getFileFormatInfo(row:Object,column:DataGridColumn):String
  	 {
  	 	var fileFormatTemp:String=row.fileFormat;
  	 	return fileFormatTemp.toLocaleUpperCase();
  	 }
 	 
	/**
	 * 格式化文件大小(单位计算)
	 * @return
	 * */
	private function getFormatFileSize(row:Object,column:DataGridColumn):String{
		return row.fileSize + "MB";
	}
	
	/**
	 * 返回到媒体内容管理主页面
	 * */
	private function backToVideoList():void
	{
		videoVS.selectedChild=videoList;
		
		videoListView.visible=true;
		videoListView.includeInLayout=true;
		
		albumListView.visible=false;
		albumListView.includeInLayout=false;
	}
	/**
	 * 进入专辑，显示专辑视频列表
	 * */
	private function getAlbumVideos(event:ListEvent):void
	{
		var videoAlbum:VideoAlbum =new VideoAlbum();
		albumVideoView.visible=true;
		albumVideoView.includeInLayout=true;
		
		albumListView.visible=false;
		albumListView.includeInLayout=false;
		
		videoVS.selectedChild=album;
		videoAlbumId =TileList(event.currentTarget).selectedItem.albumId;
		albumImageTemp = TileList(event.currentTarget).selectedItem.imagePath;
		videoAlbum.albumId =videoAlbumId;
  		videoInfoService.getAlbumVideoInfo(videoAlbum,1,6);
	}
	
	/**
	 * 返回到专辑列表页面
	 * */
	private function backToAlbumList():void
	{
		videoVS.selectedChild=albumList;
		
		albumVideoView.visible=false;
		albumVideoView.includeInLayout=false;
		
		albumListView.visible=true;
		albumListView.includeInLayout=true;
	}
	
	/**
	 * 从专辑中移除视频列表信息
	 * */
	private function removeClickHandler():void
	{
 		if(videoInfoDG2.selectedItem==null)
 		{
 			Alert.show("请选择需要移除的视频","提示信息");
 		}else{
// 			var videoInfo:VideoInfo = VideoInfo(videoInfoDG2.selectedItem);
// 			var videoAlbum:VideoAlbum =new VideoAlbum();
// 			var videoAlbumShip:VideoAlbumShip = new VideoAlbumShip();
// 			var videoAlbumShipId:VideoAlbumShipId = new VideoAlbumShipId();
// 			
// 			videoAlbum.albumId = videoAlbumId;
// 			videoAlbumShip.id = videoAlbumShipId;
// 			videoAlbumShip.videoInfo =videoInfo;
// 			videoAlbumShip.videoAlbum = videoAlbum;
// 			videoAlbumShipId.videoId = videoInfo.videoId;
// 			videoAlbumShipId.albumId = videoAlbumId;
// 			
// 			videoInfoService.removeVideoAlbumShip(videoAlbumShip);

			var videoInfos:Array=videoInfoDG2.selectedItems;
			this.isAbulmCover(videoInfos);
			videoInfoService.removeVideoAlbumShip(videoAlbumId,videoInfos);
 		}
	}
	
	/**
	 * 判断移出视频是否为专辑封面
	 * */
	 private function isAbulmCover(videoInfos:Array):void{
	 	for(var i:int=0; i<videoInfos.length;i++){
				var videoInfo:VideoInfo = VideoInfo(videoInfos[i]);			
				if(videoInfo.imagePath == albumImageTemp){
					AbulmCoverFlag = true;
				}
			}
	 }
	
	/**
	 * 移出专辑结果
	 * */
	private function removeVideoAlbumShipHandler(event:ResultEvent):void
	{	
		var result:String = String(event.result);
		if(result=="success")
		{
			var videoAlbum:VideoAlbum =new VideoAlbum();
			videoAlbum.albumId =videoAlbumId;
			videoInfoService.getAlbumVideoInfo(videoAlbum,1,6);
			if(AbulmCoverFlag){				
				picture.source =  './images/default-album.png';
				picture2.source =  './images/default-album.png';				
				AbulmCoverFlag = false;
				videoAlbum.albumName = albumName2.text;
				videoAlbum.descn = albumDesc2.text;
				videoAlbum.imagePath =  './images/default-album.png';
				
			}
			videoAlbum.videoNum = 0 - videoInfoDG2.selectedItems.length;
			videoAlbumService.updateVideoAlbum(videoAlbum,videoAlbumId);
			Alert.show("移除专辑成功","提示信息");
			
		}
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
		/**
		 * 刷新视频专辑列表
		 * */
		}else if(event.index==3){
			this.getPageVideoAlbum();
		}
		
	}
	
	/**
	 * 新增视频专辑
	 * */
	 private function addVideoAlbum():void{
	 	var addVideoAlbum:AddVideoAlbum =new AddVideoAlbum();
	 	addVideoAlbum.fatherFunction = getPageVideoAlbum;
	 	PopUpManager.addPopUp(addVideoAlbum,this,true);
		PopUpManager.centerPopUp(addVideoAlbum);
	 }
	 
	 /**
	 * 修改视频专辑
	 * */
	 private function editVideoAlbum():void{
	 	if(videotilelist.selectedItem==null){
	 		Alert.show("请选择需要修改的专辑","提示信息");
	 	}else{
	 		var videoAlbum:VideoAlbum =VideoAlbum(videotilelist.selectedItem);
	 		var editVideoAlbum:EditVideoAlbum =EditVideoAlbum(PopUpManager.createPopUp(this,EditVideoAlbum,true));
	 		editVideoAlbum.txtAlbumName.text = videoAlbum.albumName;
	 		editVideoAlbum.txtDesc.text = videoAlbum.descn;
	 		editVideoAlbum.albumId = videoAlbum.albumId;
	 		editVideoAlbum.imagePath=videoAlbum.imagePath;
	 		editVideoAlbum.fatherFunction=getPageVideoAlbum;
	 		editVideoAlbum.fatherFunction1=updateAlbumInfo;
			PopUpManager.centerPopUp(editVideoAlbum);
	 	}
	 }
	 
	 /**
	 * 删除视频专辑
	 * */
	 private function removeVideoAlbum():void{
	 	if(videotilelist.selectedItem==null){
	 		Alert.show("请选择要删除的视频专辑","提示信息");
	 	}else{
	 		
	 		Alert.yesLabel="确认";
         	Alert.noLabel="取消";
           	Alert.show("你确认删除所选择专辑吗?","提示信息",Alert.YES|Alert.NO,null,deleteVideoAlbum);
	 	}
	 }
	 
	 /**
	 * 执行删除专辑操作的响应
	 * */
	 private function deleteVideoAlbum(event:CloseEvent):void
	 {		
	 	if(event.detail==Alert.YES)
	 	{
	 		var delVideoAlbum:VideoAlbum =VideoAlbum(videotilelist.selectedItem);
	 		videoAlbumService.removeVideoAlbum(delVideoAlbum);
	 	}
	 }
	 
	 /**
	 * 删除视频专辑响应调用
	 * */
	 private function removeVideoAlbumHandler(event:ResultEvent):void{
	 	var result:String =String(event.result);
	 	if(result!=null)
	 	{
		 	Alert.show("删除成功","提示信息");
		 	this.getPageVideoAlbum();
			this.deleteAbulmInfos();
	 	}
	 }
	 
	 /**
	 * 删除出错
	 * */
	 private function removeVideoAlbumFault():void
	 {
	 	Alert.show("请先移除专辑内部视频","提示信息");
	 }
	 
	/**
	 * 修改视频专辑信息，并更新
	 */
	private function updateAlbumInfo(name:String,desc:String):void
	{
		albumName.text = name;
		albumDesc.text = desc;
	}

	/**
	 * 更新专辑封面
	 **/
	private function setCover():void
	{
		if(videoInfoDG2.selectedItem==null){
 			Alert.show("请选择需要提取图片的视频","提示信息");
 		}else{
 			setCoverFlag = true;
			var videoAlbum:VideoAlbum = new VideoAlbum();
			videoAlbum.albumName = albumName2.text;
			videoAlbum.descn = albumDesc2.text;
			videoAlbum.imagePath = VideoInfo(videoInfoDG2.selectedItem).imagePath;			
			videoAlbumService.updateVideoAlbum(videoAlbum,videoAlbumId);
 		}
	}
	
	/**
	 * 删除专辑显示信息
	 */
	 private function deleteAbulmInfos():void{
	 		picture.source = null;
	 		albumName.text = null;
	 		videoNum.text = null;
	 		albumDesc.text = null;
	 		createTime.text = null;		
	 }
	 
	/**
	 * 修改专辑信息
	 * **/
	private function updateVideoAlbumHandler(event:ResultEvent):void
	{
		if(event.result!=null){
			//Alert.show("修改专辑成功","提示信息");
			if(videoInfoDG2!=null){	
				if(videoInfoDG2.selectedItem!=null && setCoverFlag){	
					Alert.show("修改专辑成功","提示信息");				
					picture.source = VideoInfo(videoInfoDG2.selectedItem).imagePath;
					albumImageTemp = VideoInfo(videoInfoDG2.selectedItem).imagePath;
					setCoverFlag = false;
				}
			}		
			videoNum.text = event.result.videoNum;	
			this.getPageVideoAlbum();
		}
	}

	private function AlbumVideoControlbarHandler(event:ItemClickEvent):void
	{
		/**
		 * 设为封面
		 * */
		if(event.index==0){
			this.setCover();
		/**
		 * 移出专辑
		 * */
		}else if(event.index==1){
			this.removeClickHandler();
		}
	}

	/**
	 * 专辑列表添加视频回调函数
	 * */
	 private function addAlbumVideosHandler(album:VideoAlbum):void{
	 	videoNum.text = album.videoNum.toString();
	 	getAlbumVideos2();
	 	this.getPageVideoAlbum();
	 }

	/**
	 * 专辑视频列表中专辑添加视频回调函数
	 * */
	private function getAlbumVideos2():void
	{		
		var videoAlbum:VideoAlbum =new VideoAlbum();
		videoAlbum.albumId =videoAlbumId;
  		videoInfoService.getAlbumVideoInfo(videoAlbum,1,6);	
	}
	
	private function addAlbumVideo():void
	{
	 	var addAlbumVideo:AddAlbumVideo =new AddAlbumVideo();
	 	addAlbumVideo.albumId=videoAlbumId;
	 	addAlbumVideo.fatherFunction = addAlbumVideosHandler;
	 	PopUpManager.addPopUp(addAlbumVideo,this,true);
		PopUpManager.centerPopUp(addAlbumVideo);
	}

	private function searchByAlbumVideo():void
	{
		var videoAlbum:VideoAlbum =new VideoAlbum();
		videoAlbum.albumId =videoAlbumId;
		var videoInfo:VideoInfo = new VideoInfo();
		videoInfo.videoName=txtAlbumVideoName.text;
		videoInfoService.getAlbumVideoInfo2(videoAlbum, videoInfo, 1, 6)
	}
	
 	 /**
 	 * 响应视频专辑视频列表页面查询结果列表
 	 * */
 	 private function getAlbumVideoInfo2Handler(event:ResultEvent):void
 	 {
 	 	videoinfos2 = event.result as Finder;
 	 }
