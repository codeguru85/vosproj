/**Administrator
 * 下午01:45:00
 */
package com.broadengate.vcms.web.servlet;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Calendar;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * @author Kevin
 *
 */
public class ConvertVideo {

	/**
	 * 处理
	 * */
	public static boolean process(String resourcePath) 
	{
		int type = checkContentType(resourcePath);
		boolean status =false;
		if(type==0)
		{
			status = processImages(resourcePath);
		}
		return status;
	}
	/**
	 * 检查文件是否已经存在
	 * */
	private static boolean  checkFile(String path)
	{
		File file =new File(path);
		if(!file.isFile())
		{
			return false;
		}
		return true;
	}
	/**
	 * 文件类型的检查处理
	 * */
	private static int checkContentType(String resourcePath)
	{
		String type = resourcePath.substring(resourcePath.lastIndexOf(".")+1,resourcePath.length()).toLowerCase();
		//ffmpeg能解析的格式(asx,asf,mpg,wmv,3gp,mp4,mov,avi,flv等)
		if(type.equals("avi"))
		{
			return 0;
		}else if(type.equals("mpg"))
		{
			return 0;
		}else if(type.equals("wmv"))
		{
			return 0;
		}else if(type.equals("3gp"))
		{
			return 0;
		}else if(type.equals("mov"))
		{
			return 0;
		}else if(type.equals("mp4"))
		{
			return 0;
		}else if(type.equals("asf"))
		{
			return 0;
		}else if(type.equals("asx"))
		{
			return 0;
		}else if(type.equals("flv"))
		{
			return 0;
		}else if(type.equals("mpeg"))
		{
			return 0;
		}else if(type.equals("wmv9"))
		{
			return 1;
		}else if(type.equals("rm"))
		{
			return 1;
		}else if(type.equals("rmvb"))
		{
			return 1;
		}
		return 9;
	}
	
	/**
	 * 处理转换视频文件
	 * */
	private static boolean processFile(String resourcePath)
	{
		return true;
	}
	
	/**
	 * 处理获取缩略图
	 * */
	private static boolean processImages(String resourcePath)
	{
		if(!checkFile(resourcePath))
		{
			System.out.println("resourcePath" + "is not file");
			return false;
		}
		
		/**
		 * 文件名的处理
		 * */
		Calendar c = Calendar.getInstance();
		String savename = String.valueOf(c.getTimeInMillis())+Math.round(Math.random()*100000);
		
		//List commend = new ArrayList();
		//commend.add("ffmpeg");		
		//commend.add("-i");		
		//commend.add(resourcePath);		
		//commend.add("-ab");		
		//commend.add("56");		
		//commend.add("-ar");		
		//commend.add("22050");		
		//commend.add("-qscale");		
		//commend.add("8");		
		//commend.add("-r");		
		//commend.add("15");		
		//commend.add("-s");		
		//commend.add("600x500");		
		//commend.add("e:\\" + savename + ".flv");
		try
		{
			Runtime runtime =Runtime.getRuntime();
			Process proce = null;
			String cmd ="";
			String cut ="ffmpeg -i "
						+ resourcePath
						+ " -y -f image2 -ss 8 -t 0.001 -s 160*120 e:\\"
						+ savename
						+".jpg";
			String cutcmd = cmd + cut;
			proce =runtime.exec(cutcmd);
			//ProcessBuilder builder = new ProcessBuilder(commend);
			//builder.start();
			return true;
		}catch(Exception e){
			e.printStackTrace();
			return false;
		}
	}
	
	/**
	 * 外部处理视频缩略图的转换
	 * */
	public boolean processVideoImages(String resourcePath,String savePath,Integer secondsCount)
	{
		try
		{
			Integer getImgTime =secondsCount/2;
			Runtime runtime =Runtime.getRuntime();
			Process proce = null;
			String cmd ="";
			String cut ="ffmpeg -i "
						+ resourcePath
						+ " -y -f image2 -ss "+getImgTime+" -t 0.002 -s 400*300 "
						+ savePath;
			
			String cutcmd = cmd + cut;
			proce =runtime.exec(cutcmd);
			return true;
		}catch(Exception e){
			e.printStackTrace();
			return false;
		}
	}
	
	/**
	 * 获取视频的总时长
	 * */
	public String getVideoCountTime(String resourcePath)
	{
		String timeCount="";
		try{
			Runtime runtime =Runtime.getRuntime();
			Process proce = null;
			String cmd ="";
			String cut ="ffmpeg -i "
						+ resourcePath;
			String cutcmd = cmd + cut;
			proce =runtime.exec(cutcmd);
			InputStream strerr= proce.getErrorStream();
			InputStreamReader isr = new InputStreamReader(strerr);
            BufferedReader br = new BufferedReader(isr);
            String line =null;
            System.out.println("<begin>"); 
            while ( (line = br.readLine()) != null){
            	int d = line.indexOf("Duration:");
            	if(d>0){
            		timeCount =line.replace("Duration:", "").replace(" ", "").substring(0,8);
            		break;
            	}
            }
            System.out.println("<end>");
		}catch(Exception e)
		{
			e.printStackTrace();
		}
		return timeCount;
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		String path ="E:\\video\\monstersvsaliens-sbspot_h480p.mov";
		/**
		 * 文件的检查
		 * */
		if(!checkFile(path)){
			System.out.println(path +"is not file");
			return;
		}
		
		/**
		 * 文件的转换过程
		 * */
		if(process(path)){
			System.out.println(path +"is ok");
		}
	}

}
