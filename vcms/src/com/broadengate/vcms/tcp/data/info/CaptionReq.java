package com.broadengate.vcms.tcp.data.info;

import com.broadengate.vcms.tcp.data.tool.NumberToByte;

public class CaptionReq {
	  
	      private   byte[]   buf   =   new   byte[248];     //为说明问题，定死大小，事件中可以灵活处理   
	      
	      private String machineId = null;
	      /**   
	        *   构造并转换   
	        */   
	      public   CaptionReq(int   commandid,   int   commandlength, String smachineid,int state,
	    		  String text,String fontName,String fontSize,String fontColor,String bgColor,
	    		  String bgTransparency,String x,String y,String width,String height)   {   
	          byte[]   temp   =   NumberToByte.toLH(commandid);   
	          System.arraycopy(temp,   0,   buf,   0,   temp.length);   
//	    
	          temp   =   NumberToByte.toLH(commandlength);   
	          System.arraycopy(temp,   0,   buf,   4,   temp.length);   
//	    
	          temp   =   smachineid.getBytes();
	          System.arraycopy(temp,   0,   buf,   8,   temp.length); 
	          
	          temp   =   NumberToByte.toLH(state);   
	          System.arraycopy(temp,   0,   buf,   44,   temp.length);   
	          
	          temp 	= text.getBytes();
	          System.arraycopy(temp,   0,   buf,   48,   temp.length);   

	          temp 	= fontName.getBytes();
	          System.arraycopy(temp,   0,   buf,   176,   temp.length);   
	          
	          temp 	= fontSize.getBytes();
	          System.arraycopy(temp,   0,   buf,   208,   temp.length);   
	          
	          temp 	= fontColor.getBytes();
	          System.arraycopy(temp,   0,   buf,   212,   temp.length);   
	          
	          temp 	= bgColor.getBytes();
	          System.arraycopy(temp,   0,   buf,   220,   temp.length); 
	          
	          temp 	= bgTransparency.getBytes();
	          System.arraycopy(temp,   0,   buf,   228,   temp.length); 
	          
	          temp 	= x.getBytes();
	          System.arraycopy(temp,   0,   buf,   232,   temp.length); 
	          
	          temp 	= y.getBytes();
	          System.arraycopy(temp,   0,   buf,   236,   temp.length); 
	          
	          temp 	= width.getBytes();
	          System.arraycopy(temp,   0,   buf,   240,   temp.length); 
	          
	          temp 	= height.getBytes();
	          System.arraycopy(temp,   0,   buf,   244,   temp.length); 
	          
	          this.machineId = smachineid;
	      }   
	    
	      /**   
	        *   返回要发送的数组   
	        */   
	      public   byte[]   getBuf()   {   
	          return   buf;   
	      }

		public String getMachineId() {
			return machineId;
		}

		public void setMachineId(String machineId) {
			this.machineId = machineId;
		}     

}
