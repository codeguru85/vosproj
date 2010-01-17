import dto.security.play.PlayInfo;

import mx.controls.Alert;
import mx.formatters.DateFormatter;
import mx.managers.PopUpManager;
import mx.rpc.events.ResultEvent;

import util.page.Finder;
[Bindable]
public var playitem:Finder;

private function closeWindow():void{
	PopUpManager.removePopUp(this);
}

private function queryTallVideo():void{
	var playInfo:PlayInfo = new PlayInfo();
	playInfo.listName = playname.text;
	
	var sHours:int=0;
    var sMinuts:int=0;
    var sSecond:int=0;		
	var beforeStart:Date = sstartDate.selectedDate;
	var beforeEnd:Date = sendDate.selectedDate
	//时、分、秒
	if(beforeStart!=null){
		if(sstartTime.Caption!=null){
			sHours=parseInt(sstartTime.Caption.substr(0,2));
	        sMinuts=parseInt(sstartTime.Caption.substr(3,2));
	        sSecond=parseInt(sstartTime.Caption.substr(6,2));
		  }
		else{
			sHours=0;
	        sMinuts=0;
	        sSecond=0;
		}
        //放置时、分、秒
        beforeStart.setHours(sHours,sMinuts,sSecond,0);
    }
    
    if(beforeEnd!=null)
    {
    	if(sendTime.Caption!=null){
        //时、分、秒
        sHours=parseInt(sendTime.Caption.substr(0,2));
        sMinuts=parseInt(sendTime.Caption.substr(3,2));
        sSecond=parseInt(sendTime.Caption.substr(6,2));
        }
     	else{
			sHours=0;
	        sMinuts=0;
	        sSecond=0;
		}
        //放置时、分、秒
        beforeEnd.setHours(sHours,sMinuts,sSecond,0);
    }
    
    if(beforeStart> beforeEnd){
    	Alert.show("起始时间不能大于结束时间！");
    	return;
    }
    else
    {
        //装配起始时间
        playInfo.startDate = beforeStart;   
        //装配结束时间
		playInfo.endDate = beforeEnd;		
    }	
	playInfoService.getTallSelectPlay(playInfo,1,20);
}

private function getTallSelectPlayHandler(event:ResultEvent):void{	
	playitem = event.result as Finder;
	fatherfunction(playitem);
	this.closeWindow();
}