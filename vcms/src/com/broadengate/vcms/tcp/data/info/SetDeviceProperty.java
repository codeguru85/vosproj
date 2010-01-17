package com.broadengate.vcms.tcp.data.info;

import com.broadengate.vcms.tcp.data.tool.NumberToByte;

public class SetDeviceProperty {

    private   byte[]   buf   =   new   byte[384];     //为说明问题，定死大小，事件中可以灵活处理   
    
    private String machineId = null;
    /**   
      *   构造并转换   
      */   
    public   SetDeviceProperty
    (
		int commandid,int commandlength, String smachineid,int state,
		String deviceName,String networkIPAddress,String networkSubnetMask,String networkGateway,String networkDNS,
		String position,String scale,String resolution,String initialScreenMedia,String mediaRoot
    )   
    {   
        byte[]   temp   =   NumberToByte.toLH(commandid);   
        System.arraycopy(temp,   0,   buf,   0,   temp.length);   
        temp   =   NumberToByte.toLH(commandlength);   
        System.arraycopy(temp,   0,   buf,   4,   temp.length);   
        
        temp   =   smachineid.getBytes();  
        byte[]   tempTemp = new  byte[36];
        System.arraycopy(temp,   0,   tempTemp,   0,   temp.length); 
        System.arraycopy(tempTemp,   0,   buf,   8,   36); 
  	  
        temp   =   NumberToByte.toLH(state);   
        System.arraycopy(temp,   0,   buf,   44,   temp.length);   
        //消息体
        temp   =   deviceName.getBytes();  
        System.arraycopy(temp,   0,   buf,   48,   temp.length); 
        
        temp   =   networkIPAddress.getBytes();  
        System.arraycopy(temp,   0,   buf,   80,   temp.length); 
        
        temp   =   networkSubnetMask.getBytes();  
        System.arraycopy(temp,   0,   buf,   96,   temp.length); 
        
        temp   =   networkGateway.getBytes();  
        System.arraycopy(temp,   0,   buf,   112,   temp.length); 
        
        temp   =   networkDNS.getBytes();  
        System.arraycopy(temp,   0,   buf,   128,   temp.length); 
        
        
        temp   =   position.getBytes();  
        System.arraycopy(temp,   0,   buf,   144,   temp.length); 
        
        temp   =   scale.getBytes();  
        System.arraycopy(temp,   0,   buf,   208,   temp.length); 
        
        temp   =   resolution.getBytes();  
        System.arraycopy(temp,   0,   buf,   224,   temp.length); 
        
        temp   =   initialScreenMedia.getBytes();  
        System.arraycopy(temp,   0,   buf,   240,   temp.length); 
        
        temp   =   mediaRoot.getBytes();  
        System.arraycopy(temp,   0,   buf,   256,   temp.length); 
        
        this.machineId = String.valueOf(smachineid);
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
