package com.broadengate.vcms.tcp.data.info;

import com.broadengate.vcms.tcp.data.tool.NumberToByte;



public class LoginReqAck {
	 private   byte[]   buf   =   new   byte[48];     //为说明问题，定死大小，事件中可以灵活处理   
	 
	 /**
	  * 终端唯一标示
	  */
	 private String machineid="";
	 
	 /**
	  * 终端注册状态
	  */
	 private int state = 0;
     /**   
       *   构造并转换   
       */   
     public   LoginReqAck(int   commandid,   int   commandlength,String machineid,int state)   {   
           
    	 //
    	 this.machineid = machineid;
    	 this.state = state;
    	 //
    	 
    	 byte[]   temp   =   NumberToByte.toLH(commandid); 
    	 System.arraycopy(temp,   0,   buf,   0,   temp.length);  
    	 
//   
         temp   =   NumberToByte.toLH(commandlength);   
         System.arraycopy(temp,   0,   buf,   4,   temp.length);   
//   
         temp   =   machineid.getBytes();  
         byte[]   tempTemp = new  byte[36];
         System.arraycopy(temp,   0,   tempTemp,   0,   temp.length); 
         
         System.arraycopy(tempTemp,   0,   buf,   8,   36);   
   	  
         temp   =   NumberToByte.toLH(state);   
         System.arraycopy(temp,   0,   buf,   44,   temp.length);   
     }   
   
     /**   
       *   返回要发送的数组   
       */   
     public   byte[]   getBuf()   {   
         return   buf;   
     }



	public String getMachineid() {
		return machineid;
	}

	public void setMachineid(String machineid) {
		this.machineid = machineid;
	}

	public int getState() {
		return state;
	}

	public void setState(int state) {
		this.state = state;
	}     
}
