package com.broadengate.vcms.web.servlet;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.fileupload.FileUploadException;
import org.springframework.web.context.WebApplicationContext;
import org.springframework.web.context.support.WebApplicationContextUtils;

import com.broadengate.core.commons.fileUtils.FileUtils;
import com.broadengate.vcms.entity.VideoInfo;
import com.broadengate.vcms.service.IVideoInfoService;

public class createInfoFile extends HttpServlet
{
	private static final long serialVersionUID = 1L;
	
	/**
	 * 调用视频接口
	 * */
	private IVideoInfoService videoInfoService;
	
	public void init() throws ServletException {
		WebApplicationContext wac = WebApplicationContextUtils
				.getRequiredWebApplicationContext(this.getServletContext());
		videoInfoService = (IVideoInfoService) wac.getBean("videoInfoService");
	}
	
	public createInfoFile() {
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
			HttpServletResponse response) throws ServletException {
		try{
			response.setContentType("text/html; charset=UTF-8");
			request.setCharacterEncoding("UTF-8");
			//获取视频信息
			int videoId = Integer.parseInt(request.getParameter("videoId"));
			VideoInfo videoInfo = new VideoInfo();
			videoInfo.setVideoId(videoId);
			videoInfo = videoInfoService.getVideoInfo(videoInfo);
			String filePath = videoInfo.getFilePath();
			String[] arr = filePath.split("\\.");
			String strVideoInfoPath = arr[0];
			//strVideoInfoPath = strVideoInfoPath + ".txt";
			String videoInfoPath = this.getServletContext().getRealPath(strVideoInfoPath);
			
			String toWrite = videoInfo.getFileName()+"&"+videoInfo.getVideoName()+
								"&"+videoInfo.getTimeCount();
			toWrite = toWrite+"\n"+videoInfo.getDescn();
			
			FileUtils fileUtils = new FileUtils();
			fileUtils.saveListFile(videoInfoPath, toWrite);
		}catch(IOException e){
			e.printStackTrace();
		}

	}
}
