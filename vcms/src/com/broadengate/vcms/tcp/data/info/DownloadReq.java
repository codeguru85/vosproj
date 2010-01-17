package com.broadengate.vcms.tcp.data.info;

import com.broadengate.vcms.tcp.data.tool.NumberToByte;

public class DownloadReq {
	  
	      private   byte[]   buf   =   new   byte[404];     //为说明问题，定死大小，事件中可以灵活处理   
	      
	      private String machineId = null;
	      /**   
	        *   构造并转换   
	        */   
	      public   DownloadReq(int   commandid,   int   commandlength, String smachineid,int state,
	    		  String mediaType,String mediaName,String size,String url,String title)   {   
	          byte[]   temp   =   NumberToByte.toLH(commandid);   
	          System.arraycopy(temp,   0,   buf,   0,   temp.length);   
//	    
	          temp   =   NumberToByte.toLH(commandlength);   
	          System.arraycopy(temp,   0,   buf,   4,   temp.length);   
//	    
	          temp   =   smachineid.getBytes();
	          byte[]   tempTemp = new  byte[36];
	          System.arraycopy(temp,   0,   tempTemp,   0,   temp.length); 
	          System.arraycopy(tempTemp,   0,   buf,   8,   36); 
	          
	    	  
	          temp   =   NumberToByte.toLH(state);   
	          System.arraycopy(temp,   0,   buf,   44,   temp.length);   
	          
	          temp 	= mediaType.getBytes();
	          System.arraycopy(temp,   0,   buf,   48,   temp.length);   

	          temp 	= mediaName.getBytes();
	          System.arraycopy(temp,   0,   buf,   80,   temp.length);   
	          
	          temp 	= size.getBytes();
	          System.arraycopy(temp,   0,   buf,   144,   temp.length);   
	          
	          temp 	= url.getBytes();
	          System.arraycopy(temp,   0,   buf,   176,   temp.length);   
	          
	          temp 	= title.getBytes();
	          System.arraycopy(temp,   0,   buf,   304,   temp.length);   
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
