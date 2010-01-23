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

private function closeHandler(event:Event):void
{
	if(ComboBox(event.currentTarget).selectedItem=="group1")
	{
		DG.dataProvider=arr1;
	}
	else if(ComboBox(event.currentTarget).selectedItem=="group2")
	{
		DG.dataProvider=arr2;
	}
}

private function groupHandler(event:ItemClickEvent):void
{
	if(event.index==0)
	{
		Alert.show("成功");
	}
	else if(event.index==1)
	{
		Alert.show("成功");
	}
}

private function termHandler(event:ItemClickEvent):void
{
	if(event.index==0)
	{
		Alert.show("成功");
	}
	else if(event.index==1)
	{
		Alert.show("成功");
	}
}