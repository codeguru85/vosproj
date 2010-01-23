package com.broadengate.vcms.tcp.data.tool;

public class NumberToByte {
    /**   
     *   将int转为低字节在前，高字节在后的byte数组   
     */   
   public   static   byte[]   toLH(int   n)   {   
       byte[]   b   =   new   byte[4];   
       b[0]   =   (byte)   (n   &   0xff);   
       b[1]   =   (byte)   (n   >>   8   &   0xff);   
       b[2]   =   (byte)   (n   >>   16   &   0xff);   
       b[3]   =   (byte)   (n   >>   24   &   0xff);   
       return   b;   
   }   
 
   /**   
     *   将float转为低字节在前，高字节在后的byte数组   
     */   
   public   static   byte[]   toLH(float   f)   {   
       return   toLH(Float.floatToRawIntBits(f));   
   }  
   
   
	/**
	 * 字节数组转换成整型。(网络字节序，高字节在前)
	 * 
	 * @param b
	 *            字节数组。
	 * @return 整数形式。
	 */
	public static int byte2int(byte[] cHead) {
		byte[] b = new byte[4];
		b[0] = (byte) cHead[3];
		b[1] = (byte) cHead[2];
		b[2] = (byte) cHead[1];
		b[3] = (byte) cHead[0];
		return (b[3] & 0xff) | ((b[2] & 0xff) << 8) | ((b[1] & 0xff) << 16)
				| ((b[0] & 0xff) << 24);
	}

	/**
	 * 从字节数组中读取字符串，按照协议方式， 遇到0 ，就认为是字符的结束，或者遇到数组结束。 added by zhw ,2005-01-17
	 * 字符默认以0结尾!! 所以判断字符结尾就判断是不是0
	 * 
	 * @param buf
	 *            将要从中间读取的字节数组。
	 * @param start_pos
	 *            读取的开始位置。
	 * @return 16进制度字符串。
	 */
	public static String byte2string(byte[] buf, int start_pos, int end_pos) {
		String str = "";
		for (int i = start_pos; i < end_pos; i++) {

				str += (char) buf[i];

		}
		return str;
	}
	


}
