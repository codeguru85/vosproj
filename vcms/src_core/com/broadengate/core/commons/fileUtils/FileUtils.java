/**
 * 
 */
package com.broadengate.core.commons.fileUtils;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

/**
 * @author 龚磊
 * 
 */
public class FileUtils extends org.apache.commons.io.FileUtils {
	private void copyFile(String src, String dest) throws IOException {
		FileInputStream in = new FileInputStream(src);
		File file = new File(dest);
		if (!file.exists())
			file.createNewFile();
		FileOutputStream out = new FileOutputStream(file);
		int c;
		byte buffer[] = new byte[1024];
		while ((c = in.read(buffer)) != -1) {
			for (int i = 0; i < c; i++)
				out.write(buffer[i]);
		}
		in.close();
		out.close();
	}
	
	/**
	 * 删除文件
	 * @param request
	 * @param file
	 * @param maxPath
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	public boolean delFile(String filePath){
		File file=new File(filePath);
		if(file.exists() && file.isFile()){
			file.delete();
			return true;
		}
		return false;
	}
	
	/**
	 * 检查文件是否存在
	 * */
	public boolean isSetFile(String filePath){
		File file = new File(filePath);
		if(file.exists()&&file.isFile()){
			return true;
		}
		return false;
	}
	
	/**
	 * 更新播放列表文件
	 * @throws IOException 
	 * */
	public synchronized void saveListFile(String filePath,String toWrite) throws IOException{
		File file = new File(filePath);
		if (!file.exists())
			file.createNewFile();
		FileOutputStream out = new FileOutputStream(file);
		out.getChannel().write(ByteBuffer.wrap(toWrite.getBytes()));
		out.close();
	}
}
