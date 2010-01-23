// ActionScript file
import dto.security.video.VideoType;

import mx.controls.Alert;
import mx.controls.dataGridClasses.DataGridColumn;
import mx.events.ItemClickEvent;
import mx.managers.PopUpManager;
import mx.rpc.events.FaultEvent;
import mx.rpc.events.ResultEvent;

import pop.video.AddVideoType;
import pop.video.EditVideoType;

import util.page.Finder;
/**
 * 视频分类集合
 * */
[Bindable]
private var videoTypes:Finder;


/**
 * 设置默认记录数
 * */
private var thisPageSize:String='8';

/**
 * 组件初始化调用
 * */
private function initApp():void{
	this.getVideoTypes();
}
/**
 * 关闭窗口调用
 * */
private function closeThis():void{
	PopUpManager.removePopUp(this);
	this.fatherFunction();
}


/**
 * 获取初始化数据信息
 * */
private function getVideoTypes():void{
	videoTypeService.getPageVideoType(null,1,8);
} 

/**
 * 查询获取视频分类信息
 * */
private function getVideoTypesHandler(event:ResultEvent):void{
	videoTypes = event.result as Finder;
}

/**
 * 查询条件设置
 * */
private function getVideoTypeByParaments():void{
	var videoType:VideoType =new VideoType();
	videoType.typeName = txtTypeName.text;
	videoTypeService.getPageVideoType(videoType,1,parseInt(thisPageSize));
}

/**
 * 分页显示查询调用
 * */
public function getPaginations(toPage:int,pageSize:int):void
{
	var videoType:VideoType =new VideoType();
	videoType.typeName = txtTypeName.text;
	videoTypeService.getPageVideoType(videoType,toPage,pageSize);
}

/**
 * 设置每页显示记录数
 * */
public function getPagesize(pageSize:String):void
{
	thisPageSize = pageSize;
	var videoType:VideoType=new VideoType;
	videoType.typeName = txtTypeName.text;
	videoTypeService.getPagesize(pageSize);
}

/**
 * 设置每页显示记录数的响应
 * */
public function getPagesizeHandler(event:ResultEvent):void{
	videoTypes=event.result as Finder;
	var pageSizeTemp:int = new int(thisPageSize);
	videoTypeDG.rowCount = pageSizeTemp;
}

/**
 * 按钮组的事件响应
 * */
private function clickHandler(event:ItemClickEvent):void{
	/**
	 * 新增视频分类窗口
	 * */
	if(event.index==0){
		this.addVideoType();
	/**
	 * 修改视频分类窗口
	 * */
	}else if(event.index==1){
		this.editVideoType();
	/**
	 * 删除视频分类
	 * */
	}else if(event.index==2){
		this.removeVideoType();
	}
}

/**
 * 新增视频分类信息
 * */
 private function addVideoType():void{
 	var addVideoType:AddVideoType=new AddVideoType();
 	addVideoType.fatherFunction =this.getVideoTypes;
	PopUpManager.addPopUp(addVideoType,this,true);
	PopUpManager.centerPopUp(addVideoType);
 }

/**
 * 修改视频分类信息
 * */
 private function editVideoType():void{
 	if(videoTypeDG.selectedItem==null){
 		Alert.show("请选择你要修改的分类视频","提示信息");
 	}else{
 		var videoType:VideoType =VideoType(videoTypeDG.selectedItem);
 		var editVideoType:EditVideoType =EditVideoType(PopUpManager.createPopUp(this,EditVideoType,true));
 		editVideoType.videoTypeId =videoType.typeId;
 		if(videoType.videoType==null){
 			editVideoType.txtTypeId.getSelectIndex(-1);
 		}else{
 			editVideoType.txtTypeId.getSelectIndex(videoType.videoType.typeId);
 		}
 		editVideoType.typeName.text = videoType.typeName;
 		editVideoType.fatherFunction=this.getVideoTypes;
 		PopUpManager.centerPopUp(editVideoType);
 	}
 }
 
/**
 * 删除视频分类信息
 * */ 
 private function removeVideoType():void{
 	if(videoTypeDG.selectedItem==null){
 		Alert.show("请选择要删除的分类视频","提示信息");
 	}else{
 		var delVideoType:VideoType =VideoType(videoTypeDG.selectedItem);
 		videoTypeService.removeVideoType(delVideoType);
 	}
 	this.getVideoTypes();
 }
 
 /**
 * 删除视频分类信息响应调用
 * */
 private function removeVideoTypeHandler(event:ResultEvent):void{
 	var resVideoType:VideoType = VideoType(event.result);
 	videoTypes.list.removeItemAt(videoTypeDG.selectedIndex);
 	if(resVideoType!=null){
 		Alert.show("删除成功","提示信息");
 	}else{
 		Alert.show("此分类包含子分类,无法删除");
 	}
 	this.getVideoTypes();
 }
 
 /**
 * 处理错误异常的调用
 * */
 private function getExecptionHander(event:FaultEvent):void
 {
 	Alert.show("请先删除与此分类关联的子分类","提示信息");
 }

/**
 * 格式化创建时间
 * */
private function createTimeFormatDate(row:Object,column:DataGridColumn):String{
	return timeFormatter.format(row.createTime);
}

/**
 * 格式化获取子分类名称
 * */
private function getParentTypeName(row:Object,column:DataGridColumn):String{
		if(row.videoType==null){
			return "顶级分类";
		}else{
			return row.videoType.typeName;
		}
}