package util
{
	import flash.events.Event;
	
	import mx.controls.CheckBox;
	import mx.controls.treeClasses.TreeItemRenderer;
	import mx.controls.treeClasses.TreeListData;
	import mx.controls.Alert;
	
	/**
	 * 支持CheckBox的TreeItemRenderer
	 * */
	public class TreeCheckBoxRenderer extends TreeItemRenderer
	{
		
		/**
		 * 表示checkbox控件从data中所取数据的字段
		 * */
		protected var checkBox:CheckBox;
		
		[Bindable]
		public var selectMyTreeNodeTypeId:String=""; 
		/**
		 * 构造函数
		 * */
		public function TreeCheckBoxRenderer()
		{
			super();
		}
		
	   	/**
  		 * 赋值
  	   	 * */
	     override public function set data(value:Object):void
	     {
	     	super.data = value;
	     }
	      
		/**
		 * 构建CheckBox
		 * */
		 override protected function createChildren():void
		 {
		 	super.createChildren();
		 	mouseEnabled = false;
		 	checkBox = new CheckBox();
		 	checkBox.addEventListener(Event.CHANGE,changeHandler);
		 	addChild(checkBox);
		 }
		 
		 /**
		 * 点击checkBox时,更新dataProvider
		 * @param event
		 * */
		 protected function changeHandler(event:Event):void
		 {
		 	var node:XMLList = new XMLList(TreeListData(super.listData).item);
		 	node[0].@selected = String(event.currentTarget.selected);
		 	if(node[0].@selected==true){
		 		this.selectMyTreeNodeTypeId=node[0].@id;
		 	}
		 	 //设置子结点
            recursiveCheck(node.children(),event.currentTarget.selected);
		 }
		 
		 /**
        * 设置所有子结点状态跟本结点一致
        **/ 
        private function recursiveCheck(node:XMLList, selected:Boolean):void{
        	for each (var prop:XML in node) {
        		prop.@selected = selected;
        		
        		//se setar invalidate no ultimo no ele fica nesse estavo no selected false
        		if (prop.children().length()){
        			prop.@indeterminate = selected;
        		}
        		recursiveCheck(prop.children(),selected);
        	}
        }
		 
		 /**
		 * 初始化控件时,给checkbox赋值
		 * */
		 override protected function commitProperties():void
		 {
		 	super.commitProperties();
		 }
		 
		 /**
		 * 重置itemRenderer的宽度
		 * */
		 override protected function measure():void
		 {
		 	super.measure();
		 	measuredWidth +=checkBox.getExplicitOrMeasuredWidth();
		 }
		 
		 /**
		 * 重新排列位置,将label后移动
		 * @param unscaledWidth
		 * @param uncaledHeight
		 * */
		override protected function updateDisplayList(unscaledWidth:Number, unscaledHeight:Number):void  
    	{
        	super.updateDisplayList(unscaledWidth, unscaledHeight);   
        	var startx:Number = data ? TreeListData(listData).indent : 0;   
           if(super.data)
           {
           		var tld:TreeListData = TreeListData(super.listData);
           		var node:XMLList = new XMLList(TreeListData(super.listData).item);
           		
                 if(node.@selected=="true"){
                    this.checkBox.selected = true;
                 }else{
                    this.checkBox.selected = false;
                 }
                 
        		if(disclosureIcon){
            		disclosureIcon.x = startx;
            		startx = disclosureIcon.x + disclosureIcon.width;
            		disclosureIcon.setActualSize(disclosureIcon.width,disclosureIcon.height);
            		disclosureIcon.visible = data ? TreeListData( listData ).hasChildren:false;
        		}
				
        		if(icon)
        		{
            		icon.x = startx;
            		startx = icon.x + icon.measuredWidth;
            		icon.setActualSize(icon.measuredWidth, icon.measuredHeight);
        		}
				
        		checkBox.move(startx,(unscaledHeight - checkBox.height)/2);
        		label.x = startx + checkBox.getExplicitOrMeasuredWidth();
    	   }
    	}
	}
}