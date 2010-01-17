// ActionScript file
import mx.events.ListEvent;
import mx.managers.PopUpManager;
import mx.rpc.events.ResultEvent;
import mx.controls.Alert;

import util.page.Finder;
/**
 * 视频分类集合
 * */
[Bindable]
private var videoTypes:Finder;

/**
 * 获取传递更多快捷窗口的参数
 * */
public var targetWindow:String;

/**
 * 组件初始化调用
 * */
 private function initApp():void{
 	this.getQueryVideoType();
 }
/**
 * 关闭窗口调用
 * */
private function closeThis():void{
	PopUpManager.removePopUp(this);
}

/**
 * 获取初始化数据信息
 * */
private function getQueryVideoType():void{
	videoTypeService.getQueryVideoType(null,1,6);
} 


/**
 * 查询获取视频分类信息
 * */
private function getQueryVideoTypeHandler(event:ResultEvent):void{
	videoTypes = event.result as Finder;
}

/**
 * 选择 单条信息调用
 * */
private function clickHandler(event:ListEvent):void{
	if(targetWindow=="more")
	{
		
		this.closeThis();
	}else if(targetWindow=="addType")
	{
		 typeFatherFunction(event.itemRenderer.data.typeId,event.itemRenderer.data.typeName);
		 
	}else if(targetWindow=="editType")
	{	
		 editTypeFatherFunction(event.itemRenderer.data.typeId,event.itemRenderer.data.typeName);
	}else{
		 fatherFunction(event.itemRenderer.data.typeId,event.itemRenderer.data.typeName);
	}
}

///**
// * 双击选择单条信息调用响应
// * */
//private function itemDoubleClickHandler():void{
//	this.closeThis();
//}

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
			return "根目录";
		}else{
			return row.videoType.typeName;
		}
}