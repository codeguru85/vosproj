// ActionScript file

import dto.security.video.Keyword;
import dto.security.video.VideoInfoBean;

import mx.collections.ArrayCollection;
import mx.controls.Alert;
import mx.managers.PopUpManager;
import mx.rpc.events.ResultEvent;

import util.page.Finder;

private var videoInfoBean:VideoInfoBean =new VideoInfoBean();

/**
 * 绑定数据集合
 * */
 private var videoInfos:Finder;

/**
 * 初始化组件调用
 * */
 private function initApp():void{
 	
 }

/**
 * 关闭窗口调用
 * */
private function closeThis():void{
	PopUpManager.removePopUp(this);
}

/**
 * 执行条件的查询
 * */
private function getQueryVideoInfo():void
{
	
		var strStartDate:Date =txtstartDate.selectedDate;
		var strEndDate:Date = txtendDate.selectedDate;
		
		var sHours:int=0;
		var sMinuts:int=0;
		var sSecond:int=0;
		
		videoInfoBean.videoName = txtVideoName.text;
		/**
		 * 文件格式的搜索
		 * */
		if(comVideoFormat.selectedItem.data=="all"){
			videoInfoBean.fileFormat = null;
		}else{
			videoInfoBean.fileFormat = comVideoFormat.selectedItem.data;
		}
		/**
		 * 文件上传日期的搜索
		 * */
		if(strStartDate!=null && strEndDate!=null){
			if(strStartDate>strEndDate)
			{
				Alert.show("起始时间不能大于结束时间！");
				return;
			}
		}
		
		
		
		/**
		 * 开始时间
		 * */
		if(strStartDate!=null)
		{
			if(startTimes.Caption!=null){
				sHours=parseInt(startTimes.Caption.substr(0,2));
	       	 	sMinuts=parseInt(startTimes.Caption.substr(3,2));
	        	sSecond=parseInt(startTimes.Caption.substr(6,2));
			}else{
				sHours=0;
				sMinuts=0;
				sSecond=0;
			}
			strStartDate.setHours(sHours,sMinuts,sSecond);
			videoInfoBean.startDate = strStartDate;
		}
		/**
		 * 结束时间
		 * */
		if(strEndDate!=null)
		{
			if(endTimes.Caption!=null){
				sHours=parseInt(endTimes.Caption.substr(0,2));
	       	 	sMinuts=parseInt(endTimes.Caption.substr(3,2));
	        	sSecond=parseInt(endTimes.Caption.substr(6,2));
			}else{
				sHours=0;
				sMinuts=0;
				sSecond=0;
			}
			strEndDate.setHours(sHours,sMinuts,sSecond);
			videoInfoBean.endDate = strEndDate;
		}
		
		
		videoInfoBean.startSize = txtStartSize.text;
		videoInfoBean.endSize = txtEndSize.text;
		videoInfoBean.startTime = txtStartTime.text;
		videoInfoBean.endTime = txtEndTime.text;
		
		if(keyWords.text!=null){
			
			var myPattern:RegExp = /，/gi;  

			var text:String = keyWords.text.replace(myPattern,',');
			var keys:Array = text.split(',');
			videoInfoBean.keywords = new ArrayCollection();
			for(var i:int=0;i<keys.length;i++){
				if(keys[i]!=null &&  keys[i]!=""){
					var keyWord:Keyword = new Keyword();
					keyWord.wordName = keys[i];
					videoInfoBean.keywords.addItem(keyWord);
				}
			}	
		}
		videoInfoService.getMoreVideoInfo(videoInfoBean,1,5);
}
/**
 * 响应条件的查询
 * */
private function getMoreVideoInfoHandler(event:ResultEvent):void
{
	videoInfos = event.result as Finder;
	fatherFunction(videoInfos,videoInfoBean);
	this.closeThis();
}