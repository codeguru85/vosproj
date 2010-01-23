package com.broadengate.vcms.tcp.data.info;

import com.broadengate.vcms.tcp.data.tool.NumberToByte;

public class UrlReq {
	  
	      private   byte[]   buf   =   new   byte[176];     //为说明问题，定死大小，事件中可以灵活处理   
	      
	      private String machineId = null;
	      /**   
	        *   构造并转换   
	        */   
	      public   UrlReq(int   commandid,   int   commandlength, String smachineid,int state,String url)
	      {   
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
	          
	          temp 	= url.getBytes();
	          System.arraycopy(temp,   0,   buf,   48,   temp.length);   
	          
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
