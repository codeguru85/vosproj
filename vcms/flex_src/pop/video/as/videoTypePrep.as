import dto.security.video.VideoType;

import flash.events.Event;

import mx.controls.Alert;
import mx.events.CloseEvent;
import mx.managers.PopUpManager;
import mx.rpc.events.FaultEvent;
import mx.rpc.events.ResultEvent;

  /**
   * 定义选择的树型节点的XML对象
   * */
  [Bindable]
  private var videoTypeTree:XML;
 	
[Bindable]
 private var selectedType:XML;
 
 [Bindable]
 private var dateTmp:String;
 
 [Bindable]
 private var labelTmp:String;
  
public var fatherFunction:Function;

private var videoTypeIdTmp:int;

private var numTemp:int;

private var rootIdTemp:int;

 /**
 * 关闭窗口调用
 * */
private function closeThis():void{
	PopUpManager.removePopUp(this);
	this.fatherFunction();
}

	/**
  	 * 组件初始化应用
  	 * */
  private function initApp():void{
 	getTreeVideoType();
 	disableInit();
  }
  
	private function disableInit():void
	{
    	typeName.enabled=false;
    	txtTypeId.enabled=false;
    	addBtn.enabled=false;
    	cmdOk.enabled=false;
    	delBtn.enabled=false;
	}
	
	private function enableInit():void
	{
    	typeName.enabled=true;
    	txtTypeId.enabled=true;
    	addBtn.enabled=true;
    	cmdOk.enabled=true;
    	delBtn.enabled=true;
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
  	 	videoTypeTree = XML(event.result);
  	 	type_tree.openItems=videoTypeTree.id.node;
  	 	type_tree.expandItem(videoTypeTree,true);
  	 }
  	 
  	/**
  	 * 树节点变更
  	 * */
	public function treeChanged(event:Event):void
	{
	    var xml:XML=Tree(event.target).selectedItem as XML;
	    selectedType=xml;
	    if(String(selectedType.@id)=="typeDate"||String(selectedType.@id)=="all")
	    {
			//disableInit();
	    	typeName.enabled=false;
	    	txtTypeId.enabled=false;
	    	addBtn.enabled=true;
	    	cmdOk.enabled=false;
	    	delBtn.enabled=false;
	    	
	    	labelTmp="顶级分类";
	    }
	    else
	    {
			enableInit();
	    		    	
		    dateTmp=String(selectedType.@datetime).substring(0,19);	//2009-07-27 17:17:03长度19
		    labelTmp=String(selectedType.@label).split(' ')[0];		//去掉分类后面的括号和数字，取分类名称
		    
//		    txtTypeId.onCreationComplete();
//		    txtTypeId.getSelectIndex(parseInt(selectedType.@type));
			txtTypeId.treeSelectedItem = selectedType.parent();
			txtTypeId._label = String(txtTypeId.treeSelectedItem.@label).split(' ')[0];	
	    }
	}
	
	/**
	 * 新增目录
	 * */
	private function showNewTemplateWindow():void{
		if(type_tree.selectedItem == null){
			Alert.show("请从目录树中选择父目录！");
			return;
		}
		var addVideoTypePrep:AddVideoTypePrep=new AddVideoTypePrep();
		if(selectedType!=null){			
			if(selectedType.children().length() == 0 && selectedType.@num > 0){
				Alert.show("所选分类中存在视频，为最终分类，不能添加子分类","提示信息");
				return;
			}
			addVideoTypePrep.fatherFunction=getTreeVideoType;
			addVideoTypePrep.fatherFunction2=disableInit;
			addVideoTypePrep.typeIdTmp=selectedType.@id;
			addVideoTypePrep.typeNameTmp=labelTmp;
            PopUpManager.addPopUp(addVideoTypePrep,this, true);
	    	PopUpManager.centerPopUp(addVideoTypePrep);
		}
//		else{
//			CocoaAlert.show("请您选择父目录", "提示", null, true,AlertType.FORBID);
//		}
	}

	/**
	 * 删除视频分类信息
	 * */ 
	 private function removeVideoType():void{
	 	if(type_tree.selectedItem==null){
	 		Alert.show("请选择要删除的分类视频","提示信息");
	 	}else{
	 		
	 		videoTypeIdTmp=parseInt(selectedType.@id);
	 		numTemp = parseInt(selectedType.@num);
			rootIdTemp =  parseInt(selectedType.parent().@id);
	 		
			Alert.yesLabel="确定";
			Alert.noLabel="取消";
			Alert.show("你确认删除此分类吗?","提示信息",Alert.YES|Alert.NO,null,delVideoTypeconf);
	 	}
	 }
	 
	 private function delVideoTypeconf(event:CloseEvent)
	 {
		if(event.detail==Alert.YES)
		{
			var delVideoType:VideoType =new VideoType();
			delVideoType.typeId=videoTypeIdTmp;
			videoTypeService.removeVideoType(delVideoType);
		}
	 }
	 
	 /**
	 * 删除视频分类信息响应调用
	 * */
	 private function removeVideoTypeHandler(event:ResultEvent):void{
	 	var resVideoType:VideoType = VideoType(event.result);
//	 	videoTypes.list.removeItemAt(videoTypeDG.selectedIndex);
	 	if(resVideoType!=null){
	 		Alert.show("删除成功","提示信息");
	 		disableInit();
	 	}else{
	 		Alert.show("此分类包含子分类,无法删除","提示信息");
	 	}
	 	//this.getTreeVideoType();
	 	this.updateVideoNum(resVideoType);
	 }
	 
	 private function updateVideoNum(delVideoType:VideoType):void{
	 	delVideoType.videoNum = numTemp;
	 	videoTypeService.updateVideoNum(delVideoType,rootIdTemp);
	 }
	 
	 private function updateVideoNumHandler(event:ResultEvent):void{
	 	var videoType:VideoType = event.result as VideoType;
	 	if(videoType!=null){
	 		this.getTreeVideoType();
	 	} 	
	 }
	 
	 //更新视频分类信息
	 private function updateVideoType():void
	 {
	 	if(type_tree.selectedItem==null)
	 	{
	 		Alert.show("请选择要更新的分类视频","提示信息");
	 	}
	 	else
	 	{
			var videoType:VideoType =new VideoType();
			//var typeId:int = parseInt(selectedType.@type);
//			var txtType:VideoType = txtTypeId.selectedItem as VideoType;
//			var typeId:int = txtType.typeId;	
			var txtType:XML = txtTypeId.treeSelectedItem as XML;
			if(txtType.@label=="视频分类"){
				txtType.@id = -1;	
			}
			var typeId:int = parseInt(txtType.@id);		
			videoType.typeId = parseInt(selectedType.@id);
			videoType.typeName = typeName.text;
			if(videoType.typeId==0&&typeId==0){
				Alert.show("此分类不能更改名称","提示信息");
				return;
			}
			if(videoType.typeId==typeId ){
				Alert.show("所属分类不能为自己","提示信息");
				return;
			}
			if(selectedType.parent().@id == typeId){
				Alert.show("此分类已经为"+String(selectedType.parent().@label).split(' ')[0]+"的子分类","提示信息");
				return;
			}
//			if(typeId != -1 && txtType.videoTypes.length == 0 && txtType.videoNum > 0){
			if(typeId != -1 && txtType.children().length()==0 && txtType.@num > 0){
				Alert.show("所属分类中存在视频，为最终分类，不能添加子分类","提示信息");
				return;
			}
			while(txtType.parent()!=null){
				if(txtType.parent().@id == videoType.typeId){
					Alert.show("所属分类不能为子分类","提示信息");
					return;
				}else{
					txtType = txtType.parent();
				}
			}
		
			videoTypeService.updateVideoType(videoType,typeId);			
		
	 	}
	 }
	 
	 //响应更新视频分类信息
	 private function updateVideoTypeHandler(event)
	 {
		if(event.result!=null)
		{
			Alert.show("视频分类更新成功","提示信息");
			disableInit();
		}
	 	getTreeVideoType();
	 }
	 
	 /**
	 * 处理错误异常的调用
	 * */
	 private function getExecptionHander(event:FaultEvent):void
	 {
	 	Alert.show("请先删除与此分类关联的子分类","提示信息");
	 	disableInit();
	 }
	 
	 private function getLabelFunction(item:Object):String{
	 	return String(item.@label).split(' ')[0];
	 }
