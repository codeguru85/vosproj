// ActionScript file
import mx.controls.Alert;
import mx.events.ItemClickEvent;
import mx.managers.PopUpManager;

[Bindable]
var arr1:Array=[
	{termName:'111',
	termIp:'192.168.1.1',
	termMask:'255.255.255.0',
	termGateway:'192.168.1.1',
	termDns:'192.168.1.1',
	termAddr:'shenzhen',
	termType:'adv'},
	{termName:'222',
	termIp:'192.168.1.2',
	termMask:'255.255.255.0',
	termGateway:'192.168.1.1',
	termDns:'192.168.1.1',
	termAddr:'shanghai',
	termType:'adv'}
	];
	
[Bindable]
var arr2:Array=[
	{termName:'333',
	termIp:'192.168.1.1',
	termMask:'255.255.255.0',
	termGateway:'192.168.1.1',
	termDns:'192.168.1.1',
	termAddr:'shenzhen',
	termType:'adv'},
	{termName:'444',
	termIp:'192.168.1.2',
	termMask:'255.255.255.0',
	termGateway:'192.168.1.1',
	termDns:'192.168.1.1',
	termAddr:'shanghai',
	termType:'adv'}
	];
	
[Bindable]
var arr3:Array=[
	{termName:'5555',
	termIp:'192.168.1.1',
	termMask:'255.255.255.0',
	termGateway:'192.168.1.1',
	termDns:'192.168.1.1',
	termAddr:'shenzhen',
	termType:'adv'},
	{termName:'6666',
	termIp:'192.168.1.2',
	termMask:'255.255.255.0',
	termGateway:'192.168.1.1',
	termDns:'192.168.1.1',
	termAddr:'shanghai',
	termType:'adv'}
	];

private function closeHandler(event:Event):void
{
	if(ComboBox(event.currentTarget).selectedItem=="group1")
	{
		DG1.dataProvider=arr1;
	}
	else if(ComboBox(event.currentTarget).selectedItem=="group2")
	{
		DG1.dataProvider=arr2;
	}
}

private function groupNameEdit(event:ItemClickEvent):void
{
	if(event.index==0)
	{
		Alert.show("重命名页面");
	}
	else if(event.index==1)
	{
		Alert.show("删除");
	}
}

private function addGroup():void
{
	Alert.show("输入新建群组名称页面");
}

private function addTermHandler():void
{
	CB1.visible=false;
	CB1.includeInLayout=false;
	CB2.visible=true;
	CB2.includeInLayout=true;
	
//	DG1.width=HB.width*0.49;
	DG1.visible=false;
	DG1.includeInLayout=false;
	DG10.visible=true;
	DG10.includeInLayout=true;
//	DG2.width=HB.width*0.49;
	DG2.visible=true;
	DG2.includeInLayout=true;
	
	pageId.visible=true;
	pageId.includeInLayout=true;
}

private function selectToMain():void
{
	CB1.visible=true;
	CB1.includeInLayout=true;
	
	CB2.visible=false;
	CB2.includeInLayout=false;
	
//	DG1.width=HB.width;
//	DG2.width=0;
	DG1.visible=true;
	DG1.includeInLayout=true;
	DG10.visible=false;
	DG10.includeInLayout=false;
	DG2.visible=false;
	DG2.includeInLayout=false;
	
	pageId.visible=false;
	pageId.includeInLayout=false;
}

private function search():void
{
	Alert.show("搜索");
}

private function advSearch():void
{
	Alert.show("高级搜索");
}

private function submit(event:ItemClickEvent):void
{
	if(event.index==0)
	{
		PopUpManager.removePopUp(this);
	}
	else
	{
		PopUpManager.removePopUp(this);
	}
}