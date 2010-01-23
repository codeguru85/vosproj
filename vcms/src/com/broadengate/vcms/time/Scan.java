package com.broadengate.vcms.time;


import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.TimerTask;
import java.util.TreeMap;

import org.springframework.web.context.WebApplicationContext;

import com.broadengate.vcms.entity.Playlist;
import com.broadengate.vcms.service.IPlayInfoService;
import com.broadengate.vcms.tcp.data.auxiliary.PlayResultContainer;
import com.broadengate.vcms.timeJob.PlayTreeMap;
import java.util.ArrayList;

/*
 * 扫描类
 */
public class Scan extends TimerTask {
	private WebApplicationContext applicationContext;
	private IPlayInfoService play ;
	//初始化
	public Scan(WebApplicationContext applicationContext) {
		this.applicationContext = applicationContext;
		//将新的终端信息放入数据库
		play = (IPlayInfoService) applicationContext.getBean("playInfoService");
		
	}
	
	
	
	/**
	 * 执行扫描任务并调任务接口
	 */
	public  void doIt( TreeMap map,int i)
	{
		
		Date date = new Date() ;
		if(map != null && map.size()>0)
		{
//			System.out.println("执行扫描任务并调任务接口>>>>>>>>>>>>>>>>>>>>>>>> ") ;
			Set keySet = map.keySet() ;
			Iterator iter = keySet.iterator() ;
			ArrayList<Date> list = new ArrayList<Date>() ;
			while(iter.hasNext())
			{
				Object d = iter.next() ;
			Date key =(Date) d;
//			System.out.println("DDDDDDDDDd "+d+'\n');
			Date wnat = Scan.getWantDate(date,i) ;
						if((key.after(date)&&key.before(wnat))||
					key.equals(date)||key.equals(wnat))
			{
				if(map.get(key) !=null)
				{

					String name = map.get(key).toString() ;
//					 map.remove(key) ;
					String allName[] = name.split(",") ;
					for(int i2=0;i2<allName.length;i2++)
					{
						String[] sArray = allName[i2].split("!") ;
						if(sArray != null && sArray.length ==2)
						{
							try{
								//这要调接口，执行业务
								Playlist playList = new Playlist() ;
								playList.setListId(Integer.parseInt(sArray[1]));
								List mark = play.startPlay(playList, sArray[0]) ;
								String returnMsg ="";
								for(int m=0;m<mark.size();m++){
									PlayResultContainer container = (PlayResultContainer) mark.get(m);	
									if(container.getReturnMark()=="1"){
										returnMsg = "终端" + container.getClientName() + "无法连接！"+'\n';
									}else if(container.getReturnMark()=="0"){
										returnMsg = "终端" + container.getClientName() + "播放失败！"+'\n';
									}else if(container.getReturnMark()=="2"){
										returnMsg = "终端" + container.getClientName() + "播放成功！"+'\n';
									}else if(container.getReturnMark()=="3"){
										returnMsg = "终端" + container.getClientName() + "文件本地不存在！"+'\n';
									}else if(container.getReturnMark()=="4"){
										returnMsg = "终端" + container.getClientName() + "应答超时！"+'\n';
									}else if(container.getReturnMark()=="5"){
										returnMsg = "终端" + container.getClientName() + "播放列表中无视频！"+'\n';
									}else if(container.getReturnMark()=="6"){
										returnMsg = "终端" + container.getClientName() + "发送停止，找不到本地IP！"+'\n';
									}
									System.out.println(returnMsg);
								}								
							}catch(Exception e)
							{
								System.out.println(e.getStackTrace()) ;
							}
						}
						
					}
					list.add(key) ;

				}
				
			}
			
			}
			
			if(list != null )
			{
				for(int li=0 ;li<list.size();li++)
				{
					Date dateKey = list.get(li) ;
					PlayTreeMap.tm.remove(dateKey) ;
				}
			}
			
			
			
		}
		
		
	}
	
	
	/**
	 * 要扫描的时间
	 * @param i
	 * @return
	 */
	private static Date getWantDate(Date date,int i)
	{
		int h = i*1000 ;
		Date as = new Date(date.getTime()+h) ;
		return as ;
	}

	@Override
	public void run() {
//		PlayTreeMap p = new PlayTreeMap() ;
//		TreeMap d =  p.getTm() ;
		doIt(PlayTreeMap.tm,3) ;
	}
	
//	private static Date parse(String dateStr){
//		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
//		try {
//			return format.parse(dateStr);
//		} catch (ParseException e) {
//			throw new RuntimeException(e);
//		}
//	}
	
	
	
}
