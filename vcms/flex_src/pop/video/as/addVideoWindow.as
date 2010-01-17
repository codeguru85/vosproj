
// ActionScript file
import dto.security.video.UserInfo;
import dto.security.video.VideoAlbum;
import dto.security.video.VideoAlbumShip;
import dto.security.video.VideoInfo;
import dto.security.video.VideoType;

import flash.events.DataEvent;
import flash.events.Event;
import flash.events.IOErrorEvent;
import flash.events.ProgressEvent;
import flash.geom.Point;
import flash.net.FileFilter;
import flash.net.FileReference;
import flash.net.URLLoader;
import flash.net.URLRequest;

import mx.collections.ArrayCollection;
import mx.controls.Alert;
import mx.events.CloseEvent;
import mx.managers.PopUpManager;
import mx.rpc.events.ResultEvent;

import pop.video.QueryVideoAlbum;
/**
 * 文件上传组件调用
 * */
private var uploadFile:FileReference;
/**
 * 操作文件
 * */
private var openFile:FileReference;
/**
 * 判断视频信息
 * */
private var addVideoInfoBoolean:Boolean = true;

/**
 * 定义选择的树型节点的XML对象
* */
[Bindable]
private var videoTypeTree:XML;

/**
 * 组件初始化调用
 * */
private function initApp():void
{
	Security.allowDomain("*");
	openFile = new FileReference();
	openFile.addEventListener(ProgressEvent.PROGRESS,onProgress);
	openFile.addEventListener(Event.SELECT,onSelect);
	openFile.addEventListener(Event.COMPLETE,completeHandle);
	openFile.addEventListener(DataEvent.UPLOAD_COMPLETE_DATA,uploadDataComplete);
	this.getTreeVideoType();	
}
/**
 * 选择上传文件调用
 * */
private function upload():void{
	/**
	 * 支持的视频文件格式
	 * */
	var fileTypes:FileFilter = new FileFilter("支持的视频格式","*.avi;*.wmv;*.mpg;*.mpeg;*.vob;*.mp4;*.tp;*.trp;*.ts;*.m2ts;*.mov;*.mkv;");
	/**
	 * 支持所有视频格式的选择
	 * */
	var fileAllTypes:FileFilter = new FileFilter("*.*","*.*");
	var allTypes:Array = new Array();
	allTypes.push(fileTypes);
	allTypes.push(fileAllTypes);
	openFile.browse(allTypes);
}
private function uploadDataComplete(event:DataEvent):void 
{
	//分析客户端上传返回的数据
    var xmlData:XML = new XML(event.data);
 	var time:String= xmlData.time.text();
 	var filename:String= xmlData.filename.text();
 	var filepath:String= xmlData.filepath.text();
 	var filesize:String= xmlData.filesize.text();
 	var imagename:String= xmlData.imagename.text();
 	var imagepath:String= xmlData.imagepath.text();
 	var timecount:String= xmlData.timecount.text();
 	var timesecond:String= xmlData.timesecond.text();
 	
 	var info:VideoInfo = new VideoInfo();
 	info.fileName = filename;
 	info.fileFormat = uploadFile.type.substring(1,uploadFile.type.length).toLocaleLowerCase();
 	info.filePath = filepath;
 	info.fileSize = Number(filesize);
 	info.imageName = imagename;
 	info.imagePath = imagepath;
 	var d:Date = new Date();
 	d.setTime(time);
 	info.createTime =d;
 	info.videoName = txtVideoName.text;
 	info.descn = (txtDesc.text==null||txtDesc.text=="")?"没有视频描述":txtDesc.text;
 	
 	info.timeCount = timecount;
 	info.timeSecond = int(timesecond);
 	
 	var userInfo:UserInfo = new UserInfo();
 	userInfo.userId=1;
 	info.userInfo=userInfo;
 	
 	var videoAlbum:VideoAlbum = new VideoAlbum();
 	videoAlbum.albumId=(txtAlbumId.text==null||txtAlbumId.text=="")?-1:int(txtAlbumId.text);
 	
 	var videoAlbumShip:VideoAlbumShip = new VideoAlbumShip();
 	videoAlbumShip.videoAlbum=videoAlbum;
 	
 	var videoAlbumShips:ArrayCollection = new ArrayCollection();
 	videoAlbumShips.addItem(videoAlbumShip);
 	
 	var keyWords:String = txtKeyWords.text;
 	videoInfoService.addVideoInfo(info,getCheckedVideoTypes(),videoAlbumShips,keyWords);
 	
}

/**
 *取得已选择的分类 
 */
private function getCheckedVideoTypes():ArrayCollection{
	var ac:ArrayCollection = new ArrayCollection();
	treeWalkXML(videoTypeTree.children(),ac);
	return ac;
}
/**
 * 迭代XML树找出已选择的最底层元素
 * */
private function treeWalkXML(list:XMLList,ac:ArrayCollection):void{
	var vt:VideoType=null;
	for each(var item:XML in list){
		var innerList:XMLList = item.children();
		//当没有子节点时表示为最下层分类，将之加到集合中
		if(innerList.length()==0&&item.@selected==true){
//			trace(item);
			vt = new VideoType();
			vt.typeId=item.@id;
			ac.addItem(vt);
		}else{
			treeWalkXML(innerList,ac);
		}
	}
}






/**
 * 文件上传成功的处理
 * */
 private function completeHandle(event:Event):void
 {
 	
// 	Alert.yesLabel="是";
// 	Alert.noLabel ="否";
// 	Alert.show("文件上传成功,是否继续上传?","提示信息",Alert.YES|Alert.NO,null,getIsLastUpload);
 }
 
 /**
 * 判断是否继续上传后的处理
 * */
 private function getIsLastUpload(event:CloseEvent):void
 {
 	if(event.detail==Alert.YES){
 		this.fatherFunction();
 		this.initApp();
 		this.getInitInfo();
 	}else{
 		this.fatherFunction();
 		this.initApp();x
 		this.getInitInfo();
 		this.closeThis();
 	}
 }
 
/**
 * 上传完成后进行初始化组件
 * */
 private function getInitInfo():void
 {
 	txtVideoName.text="";
 	txtDesc.text ="";
 	bar.setProgress(0,100);
 	lbProgress.text="请选择";
 	//txtTypeId.getRootVideoTypeSetSelectIndex(-1);
 	txtAlbumId.text="";
 	txtAlbumName.text="";
 }
 
/**
 * 上传进度的显示调用
 * */
private function onProgress(event:ProgressEvent):void
{
	var uploaded:Number= event.bytesLoaded/1024/1024;
	lbProgress.text = "已上传" + uploaded.toFixed(2) + "MB","共"+ event.bytesTotal+"字节";
	var proc:uint =event.bytesLoaded/event.bytesTotal*100;
	bar.setProgress(proc,100);
	bar.label = "当前进度:"+" "+proc+"%";
}

/**
 * 选择上传的调用
 * */
 private function onSelect(event:Event):void
 {
 	uploadFile = FileReference(event.target);
 	var fileName:String=uploadFile.name;
 	var fileType:String=uploadFile.type.toLocaleLowerCase();
 	var fileSize:int = fileName.length;
 	var fileTypeSize:int = fileType.length;
 	if(uploadFile==null){
 		Alert.show("请选择上传的视频文件");
 		this.initApp();
 	}
 	else if(uploadFile.size>1024*1024*1024*5){
 		Alert.show("文件太大,请上传5G以内的文件","提示信息");
 		this.initApp();
 	}else if((fileType==".avi")||(fileType==".wmv")||(fileType==".mpg")||(fileType==".mpeg")||(fileType==".vob")||(fileType==".mp4")||(fileType==".tp")||(fileType==".trp")||(fileType==".ts")||(fileType==".m2ts")||(fileType==".mov")||(fileType==".mkv")){
 		lbProgress.text ="选择完成";
 		txtVideoName.text = fileName.substr(0,(fileSize-fileTypeSize));
 	}else{
 		Alert.show("请选择正确的文档格式!!\n\n温馨提示：\n 上传只支持avi,wmv,mpg,mpeg,vob,mp4,tp,trp,ts,m2ts,mov,mkv格式的文档！","提示信息");
 		this.initApp();
 	}
 }
 
/**
 * 调用视频专辑快捷窗口
 * */
 private function getQueryVideoAlbum():void{
 	var queryVideoAlbum:QueryVideoAlbum = QueryVideoAlbum(PopUpManager.createPopUp(this,QueryVideoAlbum,true));
 	var pt:Point = new Point(650,200);
 	queryVideoAlbum.targetWindow="addVideoInfo";
 	queryVideoAlbum.x = pt.x;
 	queryVideoAlbum.y = pt.y;
 	queryVideoAlbum.fatherFunction =this.setVideoAlbum;
 }
 /**
 * 对视频专辑进行赋值
 * */
 public function setVideoAlbum(albumId:String,albumName:String):void{
 	txtAlbumId.text =  albumId;
 	txtAlbumName.text = albumName;
 }
 /**
 * 检查获得视频标题是否已经存在调用
 * */
private function getExistVideoName():void{
	var videoName:String=txtVideoName.text;
	videoInfoService.getExistVideoName(videoName);
}
/**
 * 检查视频标题是否已经存在调用响应
 * */
private function getExistVideoNameHandler(event:ResultEvent):void{
	var res:Boolean = event.result as Boolean;
	if(res==true){
		Alert.show("此视频标题已经存在");
		return;
	}else{
		
		/**
		 * 检查食品分类是否已经选择完成
		 * */
		/*  if(txtTypeId.getSelectParentVideoType().typeId==-1){
		 	Alert.okLabel="确定";
		 	Alert.show("请选择视频分类","提示信息");
		 	return;
		 } */
		/**
		 * 检查视频分类是否已经选择
		 * */
		/* if(txtTypeId.getSelectedVideoType().typeId==-1){
			Alert.okLabel="确定";
			Alert.show("请选择子分类","提示信息");
			return;
		} */
		//取得分类树上选择的元素
//		var selectedItems:ArrayCollection = getCheckedVideoTypes();
//		if(selectedItems.length==0){
//			Alert.okLabel="确定";
//			Alert.show("请选择子分类","提示信息");
//			return;
//		}
		
		
		if(!addVideoInfoBoolean){
			addVideoInfoBoolean=true;
				return;
		}else{
							
			this.title = "正在上传--"+txtVideoName.text;
			/**
			 * 定义发送请求的URL
			 * */
 			var uploadUrl:String = "";
 			uploadUrl="/vcms/UploadVideo?resumeId=";
			/* uploadUrl=uploadUrl+"&txttypeId="+txtTypeId.getSelectedVideoType().typeId;
			if(txtAlbumId.text==null||txtAlbumId.text=="")
			{
				uploadUrl=uploadUrl+"&txtAlbumId=-1";
			}else{
				uploadUrl=uploadUrl+"&txtAlbumId="+txtAlbumId.text;
			}
				uploadUrl=uploadUrl+"&videoName="+encodeURIComponent(txtVideoName.text);
				uploadUrl=uploadUrl+"&fileFormat="+encodeURIComponent(uploadFile.type.substring(1,uploadFile.type.length).toLocaleLowerCase());
			if(txtDesc.text==null||txtDesc.text==""){
				uploadUrl=uploadUrl+"&txtDesc="+encodeURIComponent("没有视频描述");
			}else{
				uploadUrl=uploadUrl+"&txtDesc="+encodeURIComponent(txtDesc.text);
			} */
						 
			/**
			 * 发送请求到Servlet
			 * */
			var request:URLRequest = new URLRequest();
			
 			request.url  =uploadUrl;
 			request.method =URLRequestMethod.POST;
 			try{
 				uploadFile.upload(request,txtVideoName.text);
 				uploadFile.addEventListener(IOErrorEvent.IO_ERROR,onErr);   
 			}catch(error:Error){
 				uploadFile.cancel();
 				Alert.okLabel="确定";
 				Alert.show("文件上传失败");
 				this.title = "上传失败";
 			}
		}
	}
}

private function onErr(event:IOErrorEvent):void{
 	uploadFile.cancel();
 	Alert.okLabel="确定";
 	Alert.show("文件上传过程错误或磁盘空间不足");
 	this.title = "上传失败";
}


/**
 * 新增视频信息调用
 * */
private function addVideoInfo():void{
	if(lbProgress.text=="请选择"){
		Alert.okLabel="确定";
		Alert.show("请选择要上传的文件","提示信息");
		return;
	}
	
	 var pattern:RegExp = /&|;/;
	if(txtVideoName.text.search(pattern)!=-1){
		Alert.okLabel="确定";
		Alert.show("视频标题中存在非法字符&或;","提示信息");
		return;
	}
	/**
	 * 检查视频标题是否已经存在
	 * */
	
	if(txtVideoName.text.length>0){
		this.getExistVideoName();
	}
	
}

/**
 * 新增视频信息响应调用
 * */
 private function addVideoInfoHandler(event:ResultEvent):void
 {
 	var videoInfo:VideoInfo =event.result as VideoInfo;
 	if(videoInfo!=null){
	 	var videoId:int = videoInfo.videoId;
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
	 	this.title = "上传成功--"+txtVideoName.text;
 	 	Alert.yesLabel="是";
	 	Alert.noLabel ="否";
	 	Alert.show("文件上传成功,是否继续上传?","提示信息",Alert.YES|Alert.NO,null,getIsLastUpload);
 	}else{
 		this.closeThis();
 	}
 }

/**
 * 关闭窗口调用
 * */
private function closeThis():void{
	if(uploadFile){
		uploadFile.cancel();
	}
	if(openFile){
		openFile.cancel();
	}
	this.windowManager.remove(this);
	
}


/**
   * 初始化调用视频分类树型
   * */
private function getTreeVideoType():void{
   		videoTypeService.getTreeVideoType();
}

/**
 * 查询树型视频分类信息响应调用
 **/
private function getTreeVideoTypeHandler(event:ResultEvent):void{
	videoTypeTree = XML(event.result);
   	selectTree.openItems=videoTypeTree.id.node;
   	selectTree.expandItem(videoTypeTree,true);
}

//添加视频分类
private function addVideoType():void{
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
}

private function disableInit():void
{
}
