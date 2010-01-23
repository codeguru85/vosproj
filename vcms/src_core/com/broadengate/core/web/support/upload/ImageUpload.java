package com.broadengate.core.web.support.upload;

import java.awt.Graphics;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import java.awt.image.CropImageFilter;
import java.awt.image.FilteredImageSource;
import java.awt.image.ImageFilter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import javax.imageio.ImageIO;
import javax.servlet.ServletContext;


public class ImageUpload {

	private ServletContext servletContext;

	private File file;

	private String fileDir;

	private static final String JEP = "jpg";

	public ImageUpload(File file, String uploadDir, ServletContext servletContext) {
		this.fileDir = uploadDir  ;
		File temp = new File(servletContext.getRealPath(fileDir));
		if (!temp.exists()) {
			temp.mkdirs();
		}
		this.file = file;
		this.servletContext = servletContext;
	}

	/**
	 * 压缩jpg图片并保存
	 * 
	 * @param request
	 * @param file
	 * @param minPath
	 * @throws IOException
	 * @throws FileNotFoundException
	 */
	public String compressJpg(int widthSize, int heightSize) throws IOException, FileNotFoundException {
		BufferedImage bi = ImageIO.read(file);
		double width = bi.getWidth();
		double height = bi.getHeight();
		double ratio = width / height;
		double radioSize = widthSize / heightSize;
		Image itemp = null;
		ImageFilter cropFilter;
		if (ratio > radioSize) {
			// 缩放后的宽
			int scaledWidth = (int) (ratio * heightSize);
			// 开始缩放
			itemp = bi.getScaledInstance(scaledWidth, heightSize, BufferedImage.SCALE_SMOOTH);
			// 设置截切点
			cropFilter = new CropImageFilter((scaledWidth - widthSize) / 2, 0, widthSize, heightSize);//
		} else {
			// 缩放后的高
			int scaledHeight = (int) (widthSize / ratio);
			itemp = bi.getScaledInstance(widthSize, scaledHeight, BufferedImage.SCALE_SMOOTH);
			cropFilter = new CropImageFilter(0, (scaledHeight - heightSize) / 2, widthSize, heightSize);//
		}
		Image croppedImage = Toolkit.getDefaultToolkit().createImage(
				new FilteredImageSource(itemp.getSource(), cropFilter));
		BufferedImage tag = new BufferedImage(widthSize, heightSize, BufferedImage.TYPE_INT_RGB);
		Graphics g = tag.getGraphics();
		g.drawImage(croppedImage, 0, 0, null); // 绘制缩小后的图
		g.dispose();
		String filePath = fileDir + System.currentTimeMillis() + widthSize + ".jpg";
		OutputStream smallOut = new FileOutputStream(servletContext.getRealPath(filePath));
		ImageIO.write((BufferedImage) tag, JEP, smallOut);
		smallOut.close();
		return filePath;
	}

	/**
	 * 上传图片并写入服务器
	 * 
	 * @param request
	 * @param file
	 * @param maxPath
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	public String uploadImages() throws FileNotFoundException, IOException {
		String filePath = fileDir + System.currentTimeMillis() + ".jpg";
		InputStream stream = new FileInputStream(file);
		OutputStream bos = new FileOutputStream(servletContext.getRealPath(filePath));
		int bytesRead = 0;
		byte[] buffer = new byte[8192];
		while ((bytesRead = stream.read(buffer, 0, 8192)) != -1) {
			bos.write(buffer, 0, bytesRead);
		}
		bos.close();
		stream.close();
		return filePath;
	}
}
