// ActionScript file

import dto.security.client.TermInfo;

import mx.collections.ArrayCollection;
import mx.controls.Alert;
import mx.managers.PopUpManager;
import mx.rpc.events.ResultEvent;

import util.page.Finder;


[Bindable]
private var termnames:Finder;
private var terminfo:TermInfo = new TermInfo();

private var captionFlag:uint=0;

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

private function termOneDbClick():void
{
	var te:TermInfo =TermInfo(TermOneDG.selectedItem);
	termName.text=te.termName;
	termIp.text=te.termIp;

	termInfo.visible=true;
	termInfo.x=this.width/2-termInfo.width/2;
	termInfo.y=this.height/2-termInfo.height/2;
//	termInfo.x =mouseX;
//	termInfo.y =mouseY;
}
private function termTwoDbClick():void
{
	var te:TermInfo =TermInfo(TermTwoDG.selectedItem);
	termName.text=te.termName;
	termIp.text=te.termIp;

	termInfo.visible=true;
	termInfo.x=this.width/2-termInfo.width/2;
	termInfo.y=this.height/2-termInfo.height/2;
//	termInfo.x =mouseX;
//	termInfo.y =mouseY-250;
}

private function delSelt():void
{
	var items:Array=TermTwoDG.selectedItems;
	if(items.length==0)
		Alert.show("没有选择任何一行");
	else
	{
		for each(var item:Object in items)
		{
			var arr:ArrayCollection=ArrayCollection(TermTwoDG.dataProvider);
			arr.removeItemAt(arr.getItemIndex(item));
		}
	}
}
private function delAll():void
{
	var arr:ArrayCollection=ArrayCollection(TermTwoDG.dataProvider);
	if(arr==null)
		Alert.show("没有终端");
	else arr.removeAll();
}
private function judgeExist():void
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
private function confirm():void
{
	var arr:ArrayCollection=ArrayCollection(TermTwoDG.dataProvider);
	if(arr==null)
	{
	 	Alert.show("请选择终端");
	 	return;
	}
	if(caption.text==""||captionFlag==0)
	{
	 	Alert.show("请输入动态信息");
	 	return;
	}
	var txt:String=caption.text;
	playInfoService.pushCaption(arr,txt);
}

private function inputInit():void
{
	if(captionFlag==0)
	{
		captionFlag=1;
		caption.text="";
	}
}

private function pushCaptionHandler(event:ResultEvent):void
{	
	if(event.result!=null)
	{		
		Alert.show(String(event.result));
		PopUpManager.removePopUp(this);
//		playInfoService.getSelectPlay(playinfo,1,20);
	}	
}

private function cancel():void
{
	PopUpManager.removePopUp(this);
}

private function oncreationComplete():void
{
	var terminfo:TermInfo = new TermInfo();
	terminfo.termActivation = 1;
	playInfoService.getSelectTerm(terminfo,1,20);	
}

//获取终端
private function getSelectTermHandler(event:ResultEvent):void
{
	termnames = event.result as Finder;
}

//根据条件查询终端信息，供Page.mxml调用
public function getPaginations(page:int,pagesize:int):void
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
public function getPagesize(pagesize:int):void
{
	playInfoService.getSelectTerm(terminfo,1,pagesize);
}