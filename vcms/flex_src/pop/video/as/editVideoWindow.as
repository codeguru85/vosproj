// ActionScript file
import dto.security.video.VideoAlbum;
import dto.security.video.VideoAlbumShip;
import dto.security.video.VideoInfo;
import dto.security.video.VideoType;

import flash.net.FileReference;

import mx.collections.ArrayCollection;
import mx.controls.Alert;
import mx.managers.PopUpManager;
import mx.rpc.events.ResultEvent;

/**
 * 根分类ID
 * */
public var rootTypeId:int;
/**
 * 子分类ID
 * */
public var parentTypeId:int;

/**
 * 获取原来视频标题的值
 * */
public var beginVideoName:String;

/**
 * 专辑ID
 * */
public var curAlbumId:int;
/**
 * 视频信息
 * */
public var videoTemp:VideoInfo;

/**
 * 对修改功能的判断 
 * */
private var editVideoInfoBoolean:Boolean=true;
/**
 * 文件浏览窗口调用
 * */
private var file:FileReference;

/**
 * 定义选择的树型节点的XML对象
* */
[Bindable]
private var videoTypeTree:XML;
[Bindable]
private var videoAlbumTree:XML;

/**
 * 组件初始化调用
 * */
private function initApp():void{
	this.getTreeVideoType();
	this.getTreeVideoAlbum();	
}

/**
 * 调用视频专辑快捷窗口
 * */
// private function getQueryVideoAlbum():void{
// 	var queryVideoAlbum:QueryVideoAlbum = QueryVideoAlbum(PopUpManager.createPopUp(this,QueryVideoAlbum,true));
// 	var pt:Point = new Point(650,200);
// 	queryVideoAlbum.targetWindow="editVideoInfo";
// 	queryVideoAlbum.x = pt.x;
// 	queryVideoAlbum.y = pt.y;
// 	queryVideoAlbum.fatherFunction =this.setVideoAlbum;
// }
 
 /**
 * 对视频专辑进行赋值
 * */
// public function setVideoAlbum(albumId:String,albumName:String):void{
// 	txtAlbumId.text =  albumId;
// 	txtAlbumName.text = albumName;
// }
 
/**
 * 文件上传调用
 * */
//private function upload():void{
//	var fileTypes:FileFilter = new FileFilter("*","*");
//	var allTypes:Array = new Array(fileTypes);
//	file.browse(allTypes);
//}

/**
 * 关闭窗口调用
 * */
private function closeThis():void{
	PopUpManager.removePopUp(this);
}

/**
 * 检查修改的视频标题
 * */
private function getIsSetVideoInfo(videoId:int,videoName:String):void
{
	videoInfoService.getIsSetVideoInfo(videoId,videoName);
}
/**
 * 检查修改视频标题信息的响应
 * */
private function getIsSetVideoInfoHandler(event:ResultEvent):void
{
	var flag:Boolean = event.result as Boolean;
	
	var videoInfo:VideoInfo =new VideoInfo();
	//var videoType:VideoType =new VideoType();
	var videoAlbumShip:VideoAlbumShip =new VideoAlbumShip();
	//var videoAlbumShipId:VideoAlbumShipId =new VideoAlbumShipId();
	
	
	if(flag==true){
		Alert.show("视频标题已经存在","提示信息");
		videoInfo.videoName =beginVideoName;
	}else{
		 videoInfo.videoId 	=videoId;
		/**
		 * 检查视频标题是否已经存在
		 * */
	 	if(beginVideoName==txtVideoName.text){
	 		videoInfo.videoName =beginVideoName;
	 	}else{
	 		videoInfo.videoName =txtVideoName.text;
	 	}
		videoInfo.descn = txtDesc.text;
		var keyWords:String = txtKeyWords.text;
		//videoType.typeId=txtTypeId.getSelectedVideoType().typeId;
		
		//if(txtAlbumId.text==""||txtAlbumId.text==null){
		//	videoAlbumShipId.albumId=-1;
		//}else{
		//	videoAlbumShipId.albumId=parseInt(txtAlbumId.text);
		//}
		//videoAlbumShipId.albumSeq=1000;
		//videoAlbumShipId.videoId =videoId;
		//videoAlbumShip.id =videoAlbumShipId;
		//videoAlbumShip.videoInfo =videoInfo;
		//Alert.show("updateVideoInfo","提示信息");
		videoInfoService.updateVideoInfo(videoInfo,getCheckedVideoTypes(),getCheckedVideoAlbums(),keyWords);
	}
}


/**
 *取得已选择的分类 
 */
private function getCheckedVideoTypes():ArrayCollection{
	var ac:ArrayCollection = new ArrayCollection();
	treeWalkXML(videoTypeTree.children(),ac,"videoTypeTree");
	return ac;
}

/**
 *取得已选择的专辑 
 */
private function getCheckedVideoAlbums():ArrayCollection{
	var ac:ArrayCollection = new ArrayCollection();
	treeWalkXML(videoAlbumTree.children(),ac,"videoAlbumTree");
	return ac;
}

/**
 * 迭代XML树找出已选择的最底层元素
 * */
private function treeWalkXML(list:XMLList,ac:ArrayCollection,tab:String):void{
	
		var vt:VideoType=null;
		var va:VideoAlbum = null;
		for each(var item:XML in list){
			var innerList:XMLList = item.children();
			//当没有子节点时表示为最下层分类，将之加到集合中
			if(innerList.length()==0&&item.@selected==true){
				if(tab == "videoTypeTree"){
		//			trace(item);
					vt = new VideoType();
					vt.typeId=item.@id;
					ac.addItem(vt);		
				}else if(tab == "videoAlbumTree"){
					va = new VideoAlbum();
					va.albumId = item.@id;
					var videoAlbumShip:VideoAlbumShip = new VideoAlbumShip();
 					videoAlbumShip.videoAlbum=va;
					ac.addItem(videoAlbumShip);	
				}
			}else{
				treeWalkXML(innerList,ac,tab);
			}
		}
}


 
private function setVideoTypesTree(ac:ArrayCollection):void{
	for(var i:int=0;i<ac.length;i++){
		parentNodeSelected(videoTypeTree.children(),VideoType(ac.getItemAt(i)));
	}
}

private function setVideoAlbumsTree(ac:ArrayCollection):void{
	var len:int=ac.length;
	for(var i:int=0;i<len;i++){
		var list:XMLList = videoAlbumTree.children();
		for each(var item:XML in list){
			if(item.@id == ac.getItemAt(i).videoAlbum.albumId){
				item.@selected = true;			
			}
		}
	}
}

private function parentNodeSelected(list:XMLList,type:VideoType):void{
	//Alert.show(type.typeId.toString(),"")
	for each(var item:XML in list){
		var innerList:XMLList = item.children();
		if(innerList.length()==0&&item.@id == type.typeId){
			item.@selected = true;
			if(item.parent() != null)
				rootNodeSelected(item);
			return;	
		}else
			parentNodeSelected(innerList,type);
	}
}

private function rootNodeSelected(item:XML):void{
	var root:XML = item.parent();
	this.expandTreeRoot(root);
	var innerList:XMLList = root.children();
	for each(var item:XML in innerList){
		if(item.@selected != true){
			return;
		}
	}
	root.@selected = true;
	if(root.parent()!= null){
		rootNodeSelected(root);
	}
}

private function expandTreeRoot(root:XML):void{
	while(root!=null && root.@id != "typeDate"){
		if(!selectTree.isItemOpen(root)){
		   	selectTree.expandItem(root,true);
		}
		root = root.parent();
	}
}

/**
 * 修改视频信息
 * */
private function updateVideoInfo():void
{
	if(!editVideoInfoBoolean){
			editVideoInfoBoolean=true;
			Alert.show("修改视频信息","提示信息");
		return;
	}else{
		
		 /**
		 * 检查视频标题不能为空
		 * */
		 if(txtVideoName.length==0)
		 {
		 	Alert.show("视频标题不能为空");
		 	return;
		 }
		 
		 /**
		 * 检查修改的视频标题是否已经存在
		 * */
		 this.getIsSetVideoInfo(videoId,txtVideoName.text);
		 
		 /**
		 * 检查shipin分类是否已经选择完成
		 * */
		 //if(txtTypeId.getSelectParentVideoType().typeId==-1){
		// 	Alert.show("请选择视频分类","提示信息");
		// 	return;
		// }
		/**
		 * 检查视频分类是否已经选择
		 * */
		//if(txtTypeId.getSelectedVideoType().typeId==-1){
		//	Alert.show("请选择子分类","提示信息");
		//	return;
		//}
		
		
	}
}

/**
 * 修改视频信息响应调用
 * */
private function updateVideoInfoHandler(event:ResultEvent):void
{
	if(event.result!=null){
		var videoId:int = event.result.videoId;
	 	var url:String = "/vcms/createInfoFile?resumeId=";
	 	var request:URLRequest = new URLRequest();
	 	var variables:URLVariables = new URLVariables();
		variables.videoId = videoId;
	 	request.url = url;
	 	request.data = variables;
	 	request.method =URLRequestMethod.POST;
	 	var loader:URLLoader = new URLLoader();
	 	try{
			loader.load(request);
	 	}catch(error:Error){
//	 		Alert.okLabel="确定";
// 			Alert.show("文件上传成功，视频文本信息写入失败");
	 	}
		Alert.show("修改视频信息成功","提示信息");
	}
	this.closeThis();
	this.fatherFunction();
}

/**
   * 初始化调用视频分类树型
   * */
private function getTreeVideoType():void{
   		videoTypeService.getTreeVideoType();
}

/**
   * 查询树型视频分类信息响应调用
   * */
   private function getTreeVideoTypeHandler(event:ResultEvent):void{
   	videoTypeTree=XML(event.result);
   	selectTree.openItems=videoTypeTree.id.node;
   	selectTree.expandItem(videoTypeTree,true);
   	this.getVideoInfo();
}
private function getTreeVideoAlbum():void{
    videoAlbumService.getTreeVideoAlbum();
}
/**
  	 * 查询树型菜单响应视频专辑信息
  	 * */
private function getTreeVideoAlbumHandler(event:ResultEvent):void{
    videoAlbumTree = XML(event.result);
    //删除all节点
    delete videoAlbumTree.children()[0];
}

private function showTreeVideoAlbum():void{
 	selectTree_Album.openItems=videoAlbumTree.id.node;
    selectTree_Album.expandItem(videoAlbumTree,true);
    this.setVideoAlbumsTree(videoTemp.videoAlbumShips);
}

private function getVideoInfo():void{
   		videoInfoService.getVideoInfo(videoTemp);
}
 /**
   * 查询视频所有所在分类响应调用
   * */
 private function getVideoInfoHandler(event:ResultEvent):void{
   	videoTemp = event.result as VideoInfo;
	var videoTypes:ArrayCollection = videoTemp.videoTypes;
	setVideoTypesTree(videoTypes);
 }
 
//添加视频分类、专辑
private function addTreeNode():void{
	if(this.menuTree.selectedIndex == 0){
		if(selectTree.selectedItem == null){
			Alert.show("请从目录树中选择父目录！");
			return;
		}
		var addVideoTypePrep:AddVideoTypePrep=new AddVideoTypePrep();
		var selectedType:XML = selectTree.selectedItem as XML;
		if(selectedType!=null){			
			if(selectedType.children().length() == 0 && selectedType.@num > 0){
				Alert.show("所选分类中存在视频，为最终分类，不能添加子分类","提示信息");
				return;
			}
			addVideoTypePrep.fatherFunction=getTreeVideoType;
			addVideoTypePrep.fatherFunction2=disableInit;
			addVideoTypePrep.typeIdTmp=selectedType.@id;
			addVideoTypePrep.typeNameTmp=selectedType.@label;
	        PopUpManager.addPopUp(addVideoTypePrep,this, true);
	    	PopUpManager.centerPopUp(addVideoTypePrep);
		}
	}else if(this.menuTree.selectedIndex == 1){
			var addVideoAlbum:AddVideoAlbum=new AddVideoAlbum();
			addVideoAlbum.fatherFunction = getTreeVideoAlbum;
	        PopUpManager.addPopUp(addVideoAlbum,this, true);
	    	PopUpManager.centerPopUp(addVideoAlbum);
		
	}
}

private function disableInit():void
{
}
