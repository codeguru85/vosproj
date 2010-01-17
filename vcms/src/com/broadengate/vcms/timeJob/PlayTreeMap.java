package com.broadengate.vcms.timeJob;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import org.springframework.web.context.WebApplicationContext;
import com.broadengate.vcms.service.IPlayInfoService;
import com.broadengate.vcms.tcp.data.auxiliary.PlayTime;
import java.util.TreeMap;
import flex.messaging.io.ArrayList;

public class PlayTreeMap {

	private IPlayInfoService playInfo ;
	private WebApplicationContext applicationContext;


	public static TreeMap tm = new TreeMap();
		


	public PlayTreeMap()
	{

	}	
	
	public PlayTreeMap(WebApplicationContext applicationContext) {
		this.applicationContext = applicationContext;
		//将新的终端信息放入数据库
		playInfo = (IPlayInfoService) applicationContext.getBean("playInfoService");
		
	}
	
	public String isHaveSameKey(Date key,TreeMap map)
	{
		if(map != null && map.size()>0)
		{
			if(map.get(key) !=null)
			{
				String value = map.get(key).toString() ;
				if(value != null)
				{
					return value ;
				}
			}

		}
		return null ;
	}
	
	public void getAll()
	{
		System.out.println("初始化 播放列表 >>>>>>>>>>>>>>>>>") ;
		List pList= playInfo.getPlayList();
		for(int i=0;i<pList.size();i++){
			Object one[] = (Object[])pList.get(i) ;
			if(one!=null && one.length==3)
			{
				if(one[0]!=null && one[1]!= null && one[2]!= null)
				{
					Date dt = parse(one[2].toString());
//					System.out.println("}]]]]]]]]]]]]]]]]   "+dt) ;
					if(!dt.before(new Date()))
					{
						String isHave = isHaveSameKey(dt,tm) ;
						if(isHave ==null)
						{
							tm.put(dt, one[0].toString()+"!"+one[1].toString());
						}else{
							tm.put(dt, isHave+","+one[0].toString()+"!"+one[1].toString());
						}
					}

				}

			}

			
		}
	}
	
	
	public synchronized void  addPlayTreeMap(ArrayList videoList,int playId){
		for(int i=0;i<videoList.size();i++){
			PlayTime videoTime = (PlayTime) videoList.get(i);
			Date dt = parse(videoTime.getPlayTime());
			String isHave = isHaveSameKey(dt,tm) ;
			if(isHave ==null)
			{
				String playValue = videoTime.getPlayVideoName()+"!"+playId; 
				tm.put(dt,playValue);	
			}else{
				String playValue = isHave+","+videoTime.getPlayVideoName()+"!"+playId; 
				tm.put(dt,playValue);	
			}

		}
	}
	
	public synchronized void delPlayTreeMap(String key,int playId){		
		String name = tm.get(parse(key)).toString() ;
		String allMap[] = name.split(",");
		if(allMap.length>1){
			for(int m=0;m<allMap.length;m++){
				String allName[] = allMap[m].split("!");
				if(allName !=null && allName.length==2){					
					if(Integer.parseInt(allName[1])==playId){
						String value = replaceIt(name,allMap[m]);
						tm.put(parse(key), value);
					}				
				}
			}
		}else{
			tm.remove(parse(key));
		}		
	}
	
	public String replaceIt(String all,String old)
	{
		if(all!=null&&old!=null)
		{
			int s = all.indexOf(old) ;
			StringBuffer bu = new StringBuffer(all) ;
			if(s==0)
			{
				bu.replace(0, old.length()+1, "") ;
			}else{
				bu.replace(s-1, s+old.length(), "") ;
			}
			return bu.toString() ;
		}
		return null ;
	}
	
	//日期转换
	public static Date parse(String dateStr){
		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		try {
			return format.parse(dateStr);
		} catch (ParseException e) {
			throw new RuntimeException(e);
		}
	}
}
