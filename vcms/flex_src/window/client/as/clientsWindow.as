import dto.security.client.TermInfo;
import dto.security.client.TermType;
import dto.security.video.ToPageVideoInfo;
import dto.security.video.VideoType;

import flash.geom.Point;

import mx.collections.ArrayCollection;
import mx.controls.Alert;
import mx.controls.Menu;
import mx.controls.buttonBarClasses.ButtonBarButton;
import mx.events.CloseEvent;
import mx.events.ItemClickEvent;
import mx.events.ListEvent;
import mx.events.MenuEvent;
import mx.managers.PopUpManager;
import mx.rpc.events.ResultEvent;

import pop.client.*;

import util.page.Finder;

       	[Bindable] 
        private var groupCollection:ArrayCollection = new ArrayCollection();
       	[Bindable] 
        private var clients:Finder;
        
        
        [Bindable]
        public var clientTypes:ArrayCollection = new ArrayCollection();
        
       
        private var point1:Point = new Point();
        //终端控制菜单
        private var myMenu:Menu;
        //终端信息菜单
        private var clientInfoMenu:Menu;
        //终端信息修改后存放序号处
        private var editedMainDateGrid:ArrayCollection = new ArrayCollection();
        //全局页数
        private var thisPageSize:int = 20;
        //全局查询条件
        private var searchConditions:TermInfo = new TermInfo();
        //定时器参数
        public var intervalId:Number;
        //终端控制按钮原始长度
        private var sourWidth:int = 0;
        //是否第一次点击
        private var isOneClick = true;
        //高级查询查询条件全局变量
     	private var highClientName:String ="";
 		private var highClientIp:String="";
 		private var highClientPosition:String="";
 		private var highClientTypeId:int=-1;
 		private var highClientTermActive:int=-1;
 		private var highClientScal:String="";
 		private var highClientResolution:String="";
 		private var highSearchFlag=0;

		private var simpleClientInfo:TermInfo=null;

      	/**
      	 * 判断终端输入验证
      	 * */
      	private var addClientoBoolean:Boolean=true;
         
         //搜索条件标识
         private var searchMark:int = 0;
         
         private function init():void{
//         	var o:Object = new Object();
//         	o.name = "新群组";
//         	groupCollection.addItem(o);

			//初始化查询条件
			searchConditions.termType = new TermType();
			searchConditions.termType.typeId = -2;
			searchConditions.termActivation = -1;
         	
         	//初始化表格内容
         	
         	getAllTermInfos();
         	//启用定时器
         	initTimter();
         	
//         	clientButtonsBar.enabled = false;
			var b0:ButtonBarButton = clientButtonsBar.getChildAt(0) as ButtonBarButton;
			b0.enabled = false;
         }
         
       //定时器起动口
	   private function initTimter():void
	   {
	    intervalId = setInterval(doTimter,60000);
	   }
	   //定时器回调方法
	   private function doTimter():void
	   {
	   		//查询
			//termService.getTerms(searchConditions, clients.toPage, clients.pageSize);
			getAllTermInfos();
	    }
         
         //得到所有终端的信息
         private function getAllTermInfos():void
         {
         	clientName.text="";
         	
         	var searchTermTemp:TermInfo = new TermInfo();
 			searchTermTemp.termType=new TermType();
 			
 			searchTermTemp.termName="";
 			searchTermTemp.termIp="";
 			searchTermTemp.termAddr="";
 			searchTermTemp.termType.typeId=-2;
 			searchTermTemp.termActivation=-1;
 			
 			termService.getTerms(searchTermTemp,1,thisPageSize);
 			
         	//termService.getTerms(searchConditions,1,thisPageSize);
         }
         
         private function getAllTermInfosHandler(event:ResultEvent):void{
         	clients = event.result as Finder;
         	
         	var b0:ButtonBarButton = clientButtonsBar.getChildAt(0) as ButtonBarButton;
			b0.enabled = false;
         }
         
         private function addGroup():void{
         	var o:Object = new Object();
         	o.name = "新群组"+(groupCollection.length+1);
			groupCollection.addItem(o);
         } 
         
         //buttonBar item选择处理
         private function clickHandler(event:ItemClickEvent):void{
         	//终端控制
         	if(event.index == 0){
         		showMenu();
         	}
         	if(event.index == 1){
//         		popClientParamsWindow();
         		showClientInfoMenu();
//				toGetTermTypes();
         	}else if(event.index == 2){
        		var groupConWin:GroupConWin = new GroupConWin();
				PopUpManager.addPopUp(groupConWin,this,true);
			}
         }
         
         // 终端控制菜单
            private function showMenu():void {
                myMenu= Menu.createMenu(clientButtonsBar, myMenuData, false);
                myMenu.labelField="@label";
				myMenu.addEventListener("itemClick", optionHandleSwitch);
                
                // Calculate position of Menu in Application's coordinates. 
                point1.x=clientButtonsBar.x;
                point1.y=clientButtonsBar.y;                
                point1=clientButtonsBar.localToGlobal(point1);

                myMenu.show(point1.x + 25, point1.y + 25);
            }
         
         private function optionHandleSwitch(event:MenuEvent):void
         {
         	if(mainDateGrid.selectedItem!= null)
         	{
         		var mark:int = event.index;
         		if(mark==0)
         		{
					Alert.yesLabel="确定";
         			Alert.noLabel="取消";
         			Alert.show("无论终端是否正在休眠，终端都将被重新唤醒", "提示信息", 3, this, alertSwitch);
         		}
         		else if(mark==1)
         		{
         			optionClientHandle(1);
         		}
         	}
         }
         	
 		private function alertSwitch(event:CloseEvent):void
 		{
            if (event.detail==Alert.YES)
            {
            	optionClientHandle(0);
            }
 		}        
         
         //终端控制   
         private function optionClientHandle(index:int):void
         {
         		var mark:int = index;
         		var optionSelectedItem:TermInfo = mainDateGrid.selectedItem as TermInfo
         		var ipAddr:String = optionSelectedItem.termIp;
         		var port:int = optionSelectedItem.termCom;
         		var num:String = optionSelectedItem.termNum;
         		var clentName:String = optionSelectedItem.termName;
         		
         		termService.setOptionClient(ipAddr,port,mark,num,clentName);
         }
         //终端控制Handle
         private function setOptionClientHandel(event:ResultEvent):void{
         	Alert.show(event.result as String)
         }
       	 //终端信息管理
             private function showClientInfoMenu():void {
             	var temp:ArrayCollection=new ArrayCollection(mainDateGrid.selectedItems);
             	if(temp.length==0)
             	{
             		clientInfoMenu= Menu.createMenu(clientButtonsBar, clientInfosData2, false);
             	}
             	else
             	{
             		clientInfoMenu= Menu.createMenu(clientButtonsBar, clientInfosData, false);
             	}
                clientInfoMenu.labelField="@label";
				clientInfoMenu.addEventListener("itemClick", clientMenuHandler);
                
                // Calculate position of Menu in Application's coordinates. 
                point1.x=clientButtonsBar.x;
                point1.y=clientButtonsBar.y;                
                point1=clientButtonsBar.localToGlobal(point1);

                clientInfoMenu.show(point1.x + 100, point1.y + 25);
            }
            /**
            * 删除终端信息的处理
            * */
            private function delTermInfo(event:CloseEvent):void
            {
            	if(event.detail==Alert.YES){
            		var delTermInfo:TermInfo =  mainDateGrid.selectedItem as TermInfo;
					termService.delTermInfo(delTermInfo);
            	}
            }
            private function clientMenuHandler(event:MenuEvent):void{
            	var markStr:String = event.item.@eventName;
            	if(markStr=='addClent'){
            		openLoginWin();
            	}
            	else if(markStr =="delClient"){
            		var delTermInfos:TermInfo = mainDateGrid.selectedItem as TermInfo;
            		if(delTermInfos!=null){
            			Alert.yesLabel="确定";
         				Alert.noLabel="取消";
            			Alert.show("你确认删除此终端吗?","提示信息",Alert.YES|Alert.NO,null,delTermInfo);
            		}else{
            			Alert.show("请选择一个终端");
            		}
            	}
            	else if(markStr =="editClient"){
            		var editTermInfos:TermInfo =  mainDateGrid.selectedItem as TermInfo;
            		if(editTermInfos!=null){
            			
	            		var parametersWindow:ParametersWindow = new ParametersWindow();
	            		PopUpManager.addPopUp(parametersWindow,this,true);
	            		//初始化

	            		parametersWindow.PapClientName.text = editTermInfos.termName;
						parametersWindow.tempTermType = editTermInfos.termType;
	            		parametersWindow.PapClientId = editTermInfos.termId;
	            		parametersWindow.fatherFunction = sonEditPage;
	            		PopUpManager.centerPopUp(parametersWindow);
            		}
            		else{
            			Alert.show("请选择一个终端！");
            		}
            	}
				else if(markStr =="typeManage"){
					var termTypeWindow:TermTypeWindow = new TermTypeWindow();
					PopUpManager.addPopUp(termTypeWindow,this,true);
					PopUpManager.centerPopUp(termTypeWindow);
				}
            }
       		
       		//修改页面的父类投影方法
       		private function sonEditPage(editedTermInfo:TermInfo ):void{
					termService.editClientParas(editedTermInfo);
       		}
       		//editTermHandel
       		private function editTermHandel(event:ResultEvent):void
       		{
   				if(event!=null)
   				{
   					Alert.show("修改终端信息成功","提示信息");
   					getAllTermInfos();
   				}
       		}
       	   //删除后处理
	       private function delTermInfoHandel(event:ResultEvent):void{
	       		var termInfo:TermInfo = event.result as TermInfo;
	       		Alert.show(termInfo.termName+"删除成功！");
	       		getAllTermInfos();
	       }
       
         //终端参数调整界面
         private function popClientParamsWindow():void{
//         	  var para:ParametersWindow = new ParametersWindow();
//         	  PopUpManager.addPopUp(para, this, true); 
//              PopUpManager.centerPopUp(para); 
				var str:String = "";
				for(var i:int=0;i<editedMainDateGrid.length;i++){
					str+="@"+editedMainDateGrid.getItemAt(i);
				}
				
				Alert.show(str);
         }
         //显示搜索条件
//         private function showSearch():void{
//         	searchCavas.visible = true;
//         	searchCavas.includeInLayout = true;
//         	slipshow.visible = false;
//         	slipshow.includeInLayout = false;
//         }
//         
//        //隐藏搜索条件
//         private function hidSearch():void{
//         	searchCavas.visible = false;
//         	searchCavas.includeInLayout = false;
//         	slipshow.visible = true;
//         	slipshow.includeInLayout = true;
//         }
         
         //搜索条件显示/隐藏 判断
//         private function checkSearch():void{
//         	if(searchMark == 0){
//         		showSearch();
//         		searchMark = 1;
//         		reSetWidth("show");
//         	}
//         	else if(searchMark == 1){
//         		hidSearch();
//         		searchMark = 0;
//         		reSetWidth("unshow");
//         	}
//         }
         
         //重新设计排版
         private function reSetWidth(wmark:String):void{
         	if(wmark == "show"){
         		mainCanvas.percentWidth = 74;
         	}
         	if(wmark == "unshow"){
    			mainCanvas.percentWidth = 100;
        	}
         }
         
         private function mainClickHandler(event:ListEvent):void
         {
//         	clientButtonsBar.enabled = true;
			var temp:TermInfo=TermInfo(mainDateGrid.selectedItem);
			var b0:ButtonBarButton = clientButtonsBar.getChildAt(0) as ButtonBarButton;
			if(temp.termActivation==1)
			{
				b0.enabled = true;
			}
			else
			{
				b0.enabled = false;
			}
         }
         
        //双击响应事件
 		private function itemDoubleClickHandler(event:ListEvent):void{
//			videoInfo.visible=true;
//			videoInfo.x = mouseX;
//			videoInfo.y = mouseY;
			var nowPlayClientInfo:TermInfo =  mainDateGrid.selectedItem as TermInfo;
			termService.showPlayFile(nowPlayClientInfo);
			
 		}
 		
 		private function showPlayFileHandel(event:ResultEvent):void{
 			var showVideoInfo:ToPageVideoInfo =  event.result as ToPageVideoInfo;
// 			if(showVideoInfo==null)
// 			{
// 				Alert.show("试用期结束","提示信息");
// 				return;
// 			}
 			if(showVideoInfo!=null){
	 			if(showVideoInfo.stateMark==1){
		 			var tempArray:ArrayCollection = showVideoInfo.videoTypes;
		 			if(tempArray!=null){
		 				if(tempArray.length>0){
				 			var tempVideoType:VideoType = showVideoInfo.videoTypes.getItemAt(0) as VideoType;
				 			mediaType.text = tempVideoType.typeName;
			 			}
			 			
		 			}
		 			mediaName.text = showVideoInfo.videoName;
					
					mediaState.text = "正常";
					timeCount.text = showVideoInfo.timeCount.toString();
	//				fileTime.text = showVideoInfo.fileSize.toString();
					fileDec.text = showVideoInfo.descn;
					fileCreatTime.text = timeFormatter.format(showVideoInfo.createTime.toDateString());
					allTime.text = secondToTime(showVideoInfo.allTimes)
					nowTime.text = secondToTime(showVideoInfo.nowTimes);
					nextName.text = showVideoInfo.nextName;
					//
					videoInfo.visible=true;
					videoInfo.x = mouseX;
					videoInfo.y = mouseY;
				}
				else{
					videoInfo.visible=false;
					Alert.show(showVideoInfo.videoName);
				}
 			}
 		}
 		
 		//秒转时间
 		private function secondToTime(sec:String):String{
 			var reTimr:String = "00时00分00秒" 
 			var tempSec:int = parseInt(sec);
 			var hours:int =0;
 			var min:int = 0;
 			var second:int =0;
 			hours = Math.floor(tempSec/3600);
 			//
 			min =  Math.floor(tempSec/60-hours*60);
 			//
 			second = Math.floor(tempSec-hours*3600-min*60)
 			
 			reTimr = hours+"时"+ min+"分"+second+"秒";
 			return reTimr;
 			  
 		}
 		
 		//高级搜索弹出框
 		private function highSearchWin():void{
 			  var hightwin:HightSearchWindow = new HightSearchWindow();
         	  PopUpManager.addPopUp(hightwin, this, true); 
         	  hightwin.fatherFunction = highSearch;
              PopUpManager.centerPopUp(hightwin); 
 		}
 		
 		//高级搜素
 		private function highSearch( clientName:String,clientIp:String,clientPosition:String,typeId:int,termActive:int):void//,clientScal:String,clientResolution:String
 		{
	 		highClientName =clientName;
	 		highClientIp=clientIp;
	 		highClientPosition=clientPosition;
	 		highClientTypeId=typeId;
	 		highClientTermActive=termActive;
//	 		highClientScal=clientScal;
//	 		highClientResolution=clientResolution;
	 		highSearchFlag=1;
 			searchTerms()
 		}
 		
 		//普通查询
 		private function normalSearch():void{
	 		highClientIp="";
	 		highClientPosition="";
	 		highClientTypeId=-2;//termTypeId不作为查询条件
	 		highClientTermActive=-1;
	 		highClientScal="";
	 		highClientResolution="";
 			searchTerms()
 		}
// 		//修改完成后的操作
// 		private function afterEditMain(event:DataGridEvent):void{
// 			
// 			var dataField:String = event.dataField;
// 			trace("从前:"+event.itemRenderer.data[dataField]);
// 			trace("现在:"+event.target.itemEditorInstance.text);
// 			var beforeString:String = event.itemRenderer.data[dataField];
// 			var nowString:String = event.target.itemEditorInstance.text;
// 			if(beforeString!= nowString){
// 				editedMainDateGrid.addItem(event.itemRenderer.data.id);
// 			}
// 			
// 		}
 		
// 		//得到所有终端类型
// 		private function getAllTermTypes():void{
// 			termService2.getTermTypes();
// 		}
// 		
// 		//得到所有终端类型Handle
// 		private function getAllTermTypesHandle(event:ResultEvent):void{
// 			var tempClientTypes:ArrayCollection = event.result as ArrayCollection;
// 		}
 		
 		//开启注册终端窗口
 		private function openLoginWin():void{
			//初始化终端类型
			termService2.getTermTypes();
 		}
 		//开启注册终端窗口handle
 		private function openLoginWinHandle(event:ResultEvent):void{
 			var tempClientTypes:ArrayCollection = event.result as ArrayCollection;
 			var tempData:Object = null;
 			tempData = new Object();
 			tempData.label = "请选择";
 			tempData.data = -1;
 			
 			clientTypes.removeAll();
 			
 			clientTypes.addItem(tempData);
 			for(var i:int;i<tempClientTypes.length;i++){
 				tempData = new Object();
 				var aClientType:TermType = tempClientTypes.getItemAt(i) as TermType;
 				tempData.label = aClientType.typeName;
 				tempData.data = aClientType.typeId;
 				clientTypes.addItem(tempData);
 			}
 			
 			deviceId.text="";
 			deviceName.text="";
 			
 		 	loginWin.visible = true;
 			loginWin.includeInLayout = true;

 		}
 		//注册终端
 		private function loginClient():void{
 			if(!addClientoBoolean){
 				addClientoBoolean=true;
 				return;
 			}else{
 				
 				var term_num:String = deviceId.text;
 				var term_name:String = deviceName.text;
 				
 				//注册开始
 				simpleClientInfo = new TermInfo();
 				simpleClientInfo.termNum = term_num;
 				simpleClientInfo.termName = term_name;
 				//构造termType
 				var termTypeForLogin:TermType = new TermType();
 				termTypeForLogin.typeId = deviceType.selectedItem.data as int;
 				simpleClientInfo.termType = termTypeForLogin;
 				termService.existTerm(simpleClientInfo);
 			}
 		}
 		
 		private function existTermHandler(event:ResultEvent):void
 		{
 			if(event.result==true)
 			{
 				Alert.show('终端唯一标识'+simpleClientInfo.termNum+'重复');
 			}
 			else
 			{
 				termService.addTermInfo(simpleClientInfo);
 			}
 		}
 		
 		private function addTermInfoHandler(event:ResultEvent):void{
 			if(event.result!=null){
 				Alert.show('终端'+event.result.termName+'添加成功!');
 				//关闭注册界面
 				loginWin.visible = false;
 				loginWin.includeInLayout = false;
 				//刷新结果集
 				getAllTermInfos();
 			}
 			else{
 				Alert.show("注册失败！");
 			}
 		}
 		
 		//关闭注册窗口
 		private function removeLoginWin():void{
 		 	loginWin.visible = false;
 			loginWin.includeInLayout = false;
 		}
 		
 		
 		//查询
 		private function searchTerms():void{
 			//暂存查询条件
 			var searchTermTemp:TermInfo = new TermInfo();
 			searchTermTemp.termType=new TermType();
// 			highClientName =clientName;
//	 		highClientIp=clientIp;
//	 		highClientPosition=clientPosition;
//	 		highClientScal=clientScal;
//	 		highClientResolution=clientResolution;
			if(highSearchFlag==0)
			{
 				searchTermTemp.termName = clientName.text;
 			}
 			else
 			{
 				searchTermTemp.termName = highClientName;
 				highSearchFlag=0;
 			}
 		
 			searchTermTemp.termIp = highClientIp;
 			searchTermTemp.termAddr = highClientPosition;
 			searchTermTemp.termType.typeId=highClientTypeId;
 			searchTermTemp.termActivation=highClientTermActive;
// 			searchTermTemp.termScale = highClientScal;
// 			searchTermTemp.termResolution = highClientResolution;
 			//暂存查询条件
 			searchConditions = searchTermTemp;
 			//查询
 			termService.getTerms(searchTermTemp, 1, thisPageSize);
 			
 		}
		
		//首页,上页,下页,末页,跳转
		public function getPaginations(toPage:int,pageSize:int):void{

			//查询
			termService.getTerms(searchConditions, toPage, pageSize);
		}
		
		//设置每页显示个数
		public function getPagesize(setPageSize:String):void{
			//设置全局页数
			thisPageSize = parseInt(setPageSize);
			var paginTerm:TermInfo = new TermInfo();
			//设置查询条件
				paginTerm.termName = clientName.text;
			//查询
			termService.getTerms(paginTerm, 1, setPageSize);
		}
		//得到终端类型
		private function getTermType(row:Object,column:DataGridColumn):String{
			var typeStr:String = "";
			if(row.termType!=null){
				typeStr = row.termType.typeName;
			}
			else{
				typeStr = "其他类型";
			}
			return  typeStr ;	
		}
		
		//判断被点击的终端是否可以使用终端控制
		private function checkContractBar(event:ListEvent):void{
			if(isOneClick){
				sourWidth = clientButtonsBar.getChildAt(0).width;
				isOneClick = false;
			}
			var nowPlayClientInfo:TermInfo =  mainDateGrid.selectedItem as TermInfo;
			if(nowPlayClientInfo.termActivation == 0)
			{
				clientButtonsBar.getChildAt(0).width = 0;
			}
			else{
				clientButtonsBar.getChildAt(0).width = sourWidth;
			}
		}
		
		//弹出群组窗口
		private function groupHandler(event:ItemClickEvent):void
		{
			if(event.index==0)
			{
				var groupManWin:GroupManWin = new GroupManWin();
				PopUpManager.addPopUp(groupManWin,this,true);
			}
			else if(event.index==1)
			{
				var groupConWin:GroupConWin = new GroupConWin();
				PopUpManager.addPopUp(groupConWin,this,true);
			}
		}
		