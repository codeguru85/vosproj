package com.broadengate.vcms.web.servlet;

import java.io.File;
import java.io.IOException;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Random;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.FileUploadException;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;
import org.apache.log4j.Logger;
import org.springframework.web.context.WebApplicationContext;
import org.springframework.web.context.support.WebApplicationContextUtils;

import com.broadengate.core.commons.fileUtils.FileUtils;
import com.broadengate.vcms.service.IVideoInfoService;
import com.broadengate.vcms.web.util.TimeUtil;

/**
 * 视频文件上传处理
 * 
 * @author kevin
 */
public class UploadVideo extends HttpServlet {

	private static final long serialVersionUID = 1L;
	/**
	 * 视频文件大小限制
	 * */
	private static final long maxFileSize = 5*1024*1024*1024;

	/**
	 * 调用视频接口
	 * */
	private IVideoInfoService videoInfoService;

	/**
	 * 调用日志
	 * */
	private static final Logger logger = Logger.getLogger(UploadVideo.class);

	/**
	 * @param videoInfoService
	 *            the videoInfoService to set
	 */
	public void setVideoInfoService(IVideoInfoService videoInfoService) {
		this.videoInfoService = videoInfoService;
	}

	public void init() throws ServletException {
		WebApplicationContext wac = WebApplicationContextUtils
				.getRequiredWebApplicationContext(this.getServletContext());
		videoInfoService = (IVideoInfoService) wac.getBean("videoInfoService");
	}

	/**
	 * @see HttpServlet#HttpServlet()
	 */
	public UploadVideo() {
		super();
	}

	public String getSystemPath() {
		return this.getServletContext().getRealPath("/");
	}

	public void destory() {
		super.destroy();
	}

	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse
	 *      response)
	 */
	@Override
	protected void doPost(HttpServletRequest request,
			HttpServletResponse response) throws ServletException, IOException {
		response.setContentType("text/html; charset=UTF-8");
		request.setCharacterEncoding("UTF-8");
//		Integer txtTypeId = Integer.parseInt(request.getParameter("txttypeId"));
//		Integer txtAlbumId = Integer.parseInt(request
//				.getParameter("txtAlbumId"));

		/**
		 * 获取Flex传来的参数
		 * */
//		String strParameter = new String(request.getParameter("videoName")
//				.getBytes("ISO8859-1"), "utf-8");
//		String txtDesc = new String(request.getParameter("txtDesc").getBytes(
//				"ISO8859-1"), "utf-8");
//		String fileFormat = new String(request.getParameter("fileFormat")
//				.getBytes("ISO8859-1"), "utf-8");
//		String videoName = strParameter;
//		String descript = txtDesc;

		/**
		 * 上传视频的路径设置
		 * */
		Date date = new Date();
		String filedir = this.getServletContext().getRealPath(
				"attachment/video/")
				+ File.separator + TimeUtil.getNowDateDir(date);
		String imagedir = this.getServletContext().getRealPath(
				"attachment/images/")
				+ File.separator + TimeUtil.getNowDateDir(date);

		String strFilePath = "attachment/video/" + TimeUtil.getNowDateDir(date);
		String strImagePath = "attachment/images/"
				+ TimeUtil.getNowDateDir(date);
		String strListPath = strFilePath;
		/**
		 * 创建视频文件目录
		 * */
		File temp = new File(filedir);
		if (!temp.exists()) {
			temp.mkdirs();
		}
		/**
		 * 创建图片文件目录
		 * */
		File imgTemp = new File(imagedir);
		if (!imgTemp.exists()) {
			imgTemp.mkdirs();
		}

		// 保存到服务器上
		DiskFileItemFactory factory = new DiskFileItemFactory();
		factory.setSizeThreshold(4096);
		ServletFileUpload upload = new ServletFileUpload(factory);
		upload.setHeaderEncoding("UTF-8");
		upload.setSizeMax(maxFileSize);
		try {
			List fileItems = upload.parseRequest(request);
			Iterator iter = fileItems.iterator();
			while (iter.hasNext()) {
				FileItem item = (FileItem) iter.next();
				if (!item.isFormField()) {
					String fileName = item.getName();
					long fileSize = item.getSize();
					Date dateTime = new Date();
					if (fileName != null && !fileName.equals("")
							&& fileSize < maxFileSize) {
						/**
						 * 生成随机数
						 * */
						Random random = new Random();
						Integer randNumber = random.nextInt();
						randNumber = Math.abs(randNumber);
						String randomResult = Integer.toString(randNumber)
								.substring(0, 6);
						/**
						 * 构造文件以及文件名
						 * */
						String suffix = fileName.substring(fileName
								.lastIndexOf("."));
						String strFileName = TimeUtil
								.getFormatDateFileName(date)
								+ randomResult + suffix;
						String strImageName = TimeUtil
								.getFormatDateFileName(date)
								+ randomResult + ".jpg";
						String filePath = strFilePath + strFileName;
						String imageName = "png-0075.png";
						// String imagePath ="images/" + imageName;
						String imagePath = strImagePath + strImageName;
						
						//判断磁盘空间是否足够
						File file = new File(this.getServletContext()
								.getRealPath(strFilePath));
						long usableSpace = file.getUsableSpace();
						long size = item.getSize();
						if(file.getUsableSpace() < item.getSize()){
							throw new IOException("磁盘空间不足");
						}
						/**
						 * 上传文件处理
						 * */
						item.write(new File(this.getServletContext()
								.getRealPath(filePath)));

						/**
						 * 实例化文件转换类
						 * */
						ConvertVideo cv = new ConvertVideo();

						/**
						 * 获取视频文件的总时间长
						 * */
						String countTime = cv.getVideoCountTime(this
								.getServletContext().getRealPath(filePath));

						Integer secondsCount = 0;
						if (countTime != "") {
							String[] arrTime = countTime.split(":");
							Integer hours = (Integer.parseInt(arrTime[0])) * 3600;
							Integer minutes = (Integer.parseInt(arrTime[1])) * 60;
							Integer seconds = Integer.parseInt(arrTime[2]);
							secondsCount = hours + minutes + seconds;
						}

						/**
						 * 生成缩略图的处理
						 * */
						cv.processVideoImages(this.getServletContext()
								.getRealPath(filePath), this
								.getServletContext().getRealPath(imagePath),
								secondsCount);
						StringBuilder builder = new StringBuilder();
						builder.append("<data>");
						builder.append("<time>").append(date.getTime()).append("</time>");
						builder.append("<filename>").append(strFileName).append("</filename>");
						builder.append("<filepath>").append(filePath).append("</filepath>");
						builder.append("<filesize>").append((double) fileSize / 1024 / 1024).append("</filesize>");
						builder.append("<imagename>").append(strImageName).append("</imagename>");
						builder.append("<imagepath>").append(imagePath).append("</imagepath>");
						builder.append("<timecount>").append(countTime).append("</timecount>");
						builder.append("<timesecond>").append(secondsCount).append("</timesecond>");
						builder.append("</data>");
						response.getWriter().print(builder.toString());
						
//						/**
//						 * 对视频文件更新
//						 * */
//						FileUtils fileUtils = new FileUtils();
//						String toWrite = "\n"+strFileName+"&"+item.getFieldName()+"&"+countTime;
//						fileUtils.updateListFile(this
//								.getServletContext().getRealPath(strListPath), toWrite);
						/**
						 * 对数据库进行赋值
						 * */
//						 UserInfo userInfo = new UserInfo();
//						 VideoInfo videoInfo = new VideoInfo();
//						
//						 userInfo.setUserId(1);
//						 videoInfo.setUserInfo(userInfo);
//						 videoInfo.setVideoName(videoName);
//						 videoInfo.setCreateTime(date);
//						 videoInfo.setFileName(strFileName);
//						 videoInfo.setFileFormat(fileFormat);
//						 videoInfo.setFilePath(filePath);
//						 videoInfo.setFileSize((double) fileSize / 1024 /
//						 1024);
//						 videoInfo.setImageName(strImageName);
//						 videoInfo.setImagePath(imagePath);
//						 videoInfo.setTimeCount(countTime);
//						 videoInfo.setTimeSecond(secondsCount);
//						 videoInfo.setDescn(descript);
						//
						// VideoType videoType = new VideoType();
						// videoType.setTypeId(txtTypeId);
						//
						// VideoAlbum videoAlbum = new VideoAlbum();
						// videoAlbum.setAlbumId(txtAlbumId);
						// VideoAlbumShip videoAlbumShip = new VideoAlbumShip();
						// videoAlbumShip.setVideoAlbum(videoAlbum);
						// videoInfoService.addVideoInfo(videoInfo, videoType,
						// videoAlbumShip);
					}
				}
			}

		} catch (FileUploadException e) {
			e.printStackTrace();
			throw new IOException();

		} catch (IOException e) {
			e.printStackTrace();
			throw e;
				
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			throw new IOException();
		}
	}

}