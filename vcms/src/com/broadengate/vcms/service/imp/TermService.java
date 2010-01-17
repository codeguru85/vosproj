/**Administrator
 * 下午03:34:54
 */
package com.broadengate.vcms.service.imp;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import javax.annotation.Resource;

import org.hibernate.criterion.DetachedCriteria;
import org.hibernate.criterion.Restrictions;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import com.broadengate.vcms.dao.TermInfoDao;
import com.broadengate.vcms.dao.TermTypeDao;
import com.broadengate.vcms.dao.VideoInfoDao;
import com.broadengate.vcms.entity.TermInfo;
import com.broadengate.vcms.entity.TermType;
import com.broadengate.vcms.entity.VideoInfo;
import com.broadengate.vcms.service.ITermService;
import com.broadengate.vcms.tcp.data.TermsClient;
import com.broadengate.vcms.tcp.data.info.DeviceControl;
import com.broadengate.vcms.tcp.data.info.LoginCome;
import com.broadengate.vcms.tcp.data.info.SetDeviceProperty;
import com.broadengate.vcms.tcp.data.info.ShowPlayFile;
import com.broadengate.vcms.tcp.data.info.ShowPlayFileAck;
import com.broadengate.vcms.tcp.data.info.ToPageVideoInfo;
import com.broadengate.vcms.web.Finder;

/**
 * @author huang
 * 
 */
// Spring Service Bean的标识.
// 默认将类中的所有函数纳入事务管理.
@Service
@Transactional
public class TermService implements ITermService {
	@Resource
	private TermTypeDao termTypeDao;
	@Resource
	private VideoInfoDao videoInfoDao;

	public TermTypeDao getTermTypeDao() {
		return termTypeDao;
	}

	public void setTermTypeDao(TermTypeDao termTypeDao) {
		this.termTypeDao = termTypeDao;
	}

	public TermInfoDao getTermInfoDao() {
		return termInfoDao;
	}

	public void setTermInfoDao(TermInfoDao termInfoDao) {
		this.termInfoDao = termInfoDao;
	}

	@Resource
	private TermInfoDao termInfoDao;

	/**
	 * 查询所有的终端种类
	 */
	public List<TermType> getTermTypes() {
		return termTypeDao.getAll();
	}

	/**
	 * 删除类型
	 * 
	 * @param termType
	 * @return
	 */
	public TermType removeTermType(TermType termType) {
		try {
			return termTypeDao.remove(termType);
		} catch (Exception e) {
			TermType tempTermType = new TermType();
			tempTermType.setTypeId(-1);
			return tempTermType;
		}
	}

	/**
	 * 增加类型
	 * 
	 * @param termType
	 * @return
	 */
	public TermType addTermType(TermType termType) {
		termType.setTypeId(null);
		return termTypeDao.save(termType);
	}

	/**
	 * 修改类型
	 * 
	 * @param termType
	 * @return
	 */
	public TermType editTermType(TermType termType) {

		TermType termTypeForEdit = termTypeDao.getById(termType.getTypeId());
		termTypeForEdit.setDescn(termType.getDescn());
		termTypeForEdit.setTypeName(termType.getTypeName());

		return termTypeForEdit;
	}

	/**
	 * 重复终端判断
	 */
	public Boolean existTerm(TermInfo termInfo)
	{
		String hql="from TermInfo as t  where t.termNum='"+termInfo.getTermNum()+"'";
		List<TermInfo> lists=termInfoDao.find(hql);
		if(lists.size()>0){
			return true;
		}else{
			return false;
		}		
	}

	/**
	 * 增加终端(只是注册基本信息)
	 */
	public TermInfo addTermInfo(TermInfo termInfo) {
		termInfo.setTermId(null);
		//装载终端类型
		TermType tempTermType = termInfo.getTermType();
		if(tempTermType.getTypeId()!=-1){
			tempTermType = termTypeDao.getById(tempTermType.getTypeId());
			termInfo.setTermType(tempTermType);
		}else{
			termInfo.setTermType(null);
		}
		//装载注册时间
		termInfo.setCreateTime(new Date());
		return termInfoDao.save(termInfo);
	}

	/**
	 * 得到所有的终端信息
	 * 
	 * @return
	 */
	public List<TermInfo> getAllTermInfos() {
		return termInfoDao.getAll();
	}

	/**
	 * 删除终端信息
	 * 
	 * @return
	 */
	public TermInfo delTermInfo(TermInfo termInfo) {
		return termInfoDao.removeById(termInfo.getTermId());
	}

	/**
	 * 查询指定终端信息
	 * 
	 * @param toPage
	 * @return
	 */
	public Finder getTerms(TermInfo termInfo, Integer toPage, Integer pageSize) {
		Finder finder = new Finder();
		finder.setToPage(toPage);
		finder.setPageSize(pageSize);
		// 指定查询条件
		DetachedCriteria criteria = DetachedCriteria.forClass(TermInfo.class);
		// criteria.add(Restrictions.eq("state", "1"));
		if (termInfo != null) {
			
			if (termInfo.getTermName() != null && termInfo.getTermName() != "") {
				criteria.add(Restrictions.like("termName", "%"
						+ termInfo.getTermName() + "%"));
			}
			//
			if (termInfo.getTermIp() != null && termInfo.getTermIp() != "") {
				criteria.add(Restrictions.like("termIp", "%"+termInfo.getTermIp()+"%"));
			}
			//
			if (termInfo.getTermAddr()!= null && termInfo.getTermAddr()!= "") {
				criteria.add(Restrictions.like("termAddr", "%"
						+ termInfo.getTermAddr()+ "%"));
			}
			
			if(termInfo.getTermType()!=null && termInfo.getTermType().getTypeId()!=-2)
			{
				if(termInfo.getTermType().getTypeId()!=-1)
				{
					criteria.add(Restrictions.eq("termType.typeId",  termInfo.getTermType().getTypeId()));
				}
				else
				{//其他类型
					criteria.add(Restrictions.isNull("termType"));
				}
			}
			
			if(termInfo.getTermActivation()!=-1)
			{
				if(termInfo.getTermActivation()==0)
				{
					criteria.add(Restrictions.eq("termActivation",  0));
				}
				else if(termInfo.getTermActivation()==1)
				{
					criteria.add(Restrictions.eq("termActivation",  1));
				}
			}
			
			//
			if (termInfo.getTermScale()!= null && termInfo.getTermScale()!= "") {
				criteria.add(Restrictions.eq("termScale", termInfo.getTermScale()));
			}
			//
			if (termInfo.getTermResolution() != null && termInfo.getTermResolution() != "") {
				criteria.add(Restrictions.eq("termResolution", termInfo.getTermResolution()));
			}
		}
		//
		finder.setTotalRows(termInfoDao.getRowCount(criteria));
		List<TermInfo> clients = termInfoDao.getPageObjects(criteria, toPage,
				finder.getPageSize());
		finder.setList(clients);
		return finder;
	}

	/**
	 * 查询是否有指定termNum的机器
	 * 
	 * @param theTermNum
	 * @return
	 */
	public boolean getTermByTermNum(LoginCome loginCome) {
		boolean mark = false;
		// 指定查询条件
		DetachedCriteria criteria = DetachedCriteria.forClass(TermInfo.class);
		// criteria.add(Restrictions.eq("state", "1"));
		criteria.add(Restrictions.eq("termNum", loginCome.getTermNum()));

		//
		List<TermInfo> termInfos = termInfoDao.getTermInfosByNum(criteria);
		if (termInfos.size() > 0) {
			for (int i = 0; i < termInfos.size(); i++) {
				TermInfo tempTermInfo = termInfos.get(i);
				editTermInfo(tempTermInfo, loginCome);
			}
			mark = true;
		} else {

			mark = false;
		}
		return mark;
	}

	/**
	 * 对指定Term进行修改
	 */
	public void editTermInfo(TermInfo tempTermInfo, LoginCome loginCome) {
		tempTermInfo.setTermActivation(loginCome.getTermActivation());
		tempTermInfo.setTermAddr(loginCome.getTermAddr());
		tempTermInfo.setTermDec(loginCome.getTermDec());
		tempTermInfo.setTermDns(loginCome.getTermDns());
		tempTermInfo.setTermGateway(loginCome.getTermGateway());
		// tempTermInfo.setTermId(termId);
		tempTermInfo.setTermInitialscreen(loginCome.getTermInitialscreen());
		tempTermInfo.setTermIp(loginCome.getTermIp());
		tempTermInfo.setTermMask(loginCome.getTermMask());
		//tempTermInfo.setTermName(loginCome.getTermName());
		// tempTermInfo.setTermNum(termNum);
		tempTermInfo.setTermResolution(loginCome.getTermResolution());
		// tempTermInfo.setTermPlaylists(termPlaylists);
		tempTermInfo.setTermRoot(loginCome.getTermRoot());
		tempTermInfo.setTermScale(loginCome.getTermScale());
		tempTermInfo.setTermScreen(loginCome.getTermScreen());
		// 设置最后的注册信息
		tempTermInfo.setTermLastLogintime(new Date());
		if (loginCome.getTermCom().equals("")
				|| loginCome.getTermCom().equals(null)) {
			tempTermInfo.setTermCom(0);
		} else {
			tempTermInfo.setTermCom(Integer.valueOf(loginCome.getTermCom()));
		}
		// tempTermInfo.setTermType();
	}

	/**
	 * 终端控制操作
	 * 
	 * @param ip
	 * @param con
	 * @param mark
	 */
	public String setOptionClient(String ip, int con, int mark, String num,String clientName) {
		
		String sendString = "succse";
		String sleepString = "休眠成功！";
		String reStartString = "终端唤醒命令发送成功！";
		TermsClient tc = new TermsClient(ip, con);
		DeviceControl dc = null;
		if (mark == 0) {
			dc = new DeviceControl(0X00000005, 48, num, 0);
		} else if (mark == 1) {
			dc = new DeviceControl(0X00000005, 48, num, 2);
		}
		Object returnObj = tc.talk(dc.getBuf());
//		if(returnObj==null)
//		{
//			return "试用期结束";
//		}
		if (returnObj instanceof String) {
			sendString = (String) returnObj;
			if (sendString.equals("succsee")) {
				if (mark == 0)
					sendString = clientName + reStartString;
				if (mark == 1)
					sendString = clientName + sleepString;
			} else if (sendString.equals("faild")) {

				sendString = "对终端的控制失败！";
			} else if (sendString.equals("noClient")) {
				sendString = "等待超时！请确定是终端是否存在或者线路是否通畅";
			} else if (sendString.equals("readOver")) {

				sendString = "终端应答超时！请检查终端是否开启";
			} else {

				sendString = "返回控制信息有误!！";
			}
		} else {

			sendString = "返回类型错误！终端可能非法停止";
		}

		return sendString;
	}

	/**
	 * 修改终端参数
	 */
	public TermInfo editClientParas(TermInfo tempEditTerm) 
	{
		TermInfo term=termInfoDao.getById(tempEditTerm.getTermId());
		
		term.setTermName(tempEditTerm.getTermName());
		term.setTermType(tempEditTerm.getTermType());
		
		return term;
	
//		String sendString = "succse";
//		
//		TermInfo editedTerm = termInfoDao.getById(tempEditTerm.getTermId());
//		TermsClient tc = new TermsClient(editedTerm.getTermIp(), editedTerm
//				.getTermCom());
//		SetDeviceProperty sdp = new SetDeviceProperty(0X00000008, 384,
//				editedTerm.getTermNum(), 0, tempEditTerm.getTermName(),
//				tempEditTerm.getTermIp(), tempEditTerm.getTermMask(),
//				tempEditTerm.getTermGateway(), tempEditTerm.getTermDns(),
//				tempEditTerm.getTermAddr(), tempEditTerm.getTermScale(),
//				tempEditTerm.getTermResolution(), tempEditTerm
//						.getTermInitialscreen(), tempEditTerm.getTermRoot());
//		Object returnObj = tc.talk(sdp.getBuf());
//		if (returnObj instanceof String) {
//			sendString = (String) returnObj;
//			if (sendString.equals("succsee")) {
//				sendString = "对" + tempEditTerm.getTermName() + "的参数修改成功!";
//			} else if (sendString.equals("faild")) {
//				sendString = "对终端参数修改失败！";
//			} else if (sendString.equals("noClient")) {
//				sendString = "等待超时！请确定是终端是否存在或者线路是否通畅";
//			} else if (sendString.equals("readOver")) {
//				sendString = "终端应答超时！请检查终端是否开启";
//			} else {
//
//				sendString = "返回修改回复信息有误!！";
//			}
//		} else {
//
//			sendString = "返回类型错误！";
//		}
//
//		return sendString;
	}

	/**
	 * 反向查询
	 * 
	 * @return
	 */
	public ToPageVideoInfo showPlayFile(TermInfo tempQueryTerm) {

		VideoInfo tempSendVideoInfo = null;
		ToPageVideoInfo tpvi = null;
		Object tempobj = null;
		//
		TermsClient tc = new TermsClient(tempQueryTerm.getTermIp(),
				tempQueryTerm.getTermCom());
		ShowPlayFile spf = new ShowPlayFile(0X00000004, 48, tempQueryTerm
				.getTermNum(), 0);
		// 发送反向查询消息(返回正在播放的文件名)

		tempobj = tc.talk(spf.getBuf());
		if(tempobj==null)
		{
			return null;
		}
		if (tempobj instanceof ShowPlayFileAck) {
			ShowPlayFileAck fileInfo = (ShowPlayFileAck) tempobj;
			// 得到播放文件名称
			String[] arr = fileInfo.getMMediaName().split("/");
			
			String fileName = arr[arr.length-1];
			// 查询与该名称匹配的video
			DetachedCriteria criteria = DetachedCriteria
					.forClass(VideoInfo.class);
			criteria.add(Restrictions.eq("fileName", fileName));
			List<VideoInfo> videoInfos = videoInfoDao.getAnyVideo(criteria);

			if (videoInfos.size() > 0) {
				tempSendVideoInfo = videoInfos.get(0);
				tpvi = new ToPageVideoInfo();
				tpvi.setCreateTime(tempSendVideoInfo.getCreateTime());
				tpvi.setDescn(tempSendVideoInfo.getDescn());
				tpvi.setFileName(tempSendVideoInfo.getFileName());
				tpvi.setFileSize(tempSendVideoInfo.getFileSize());
				tpvi.setTimeCount(tempSendVideoInfo.getTimeCount());
				tpvi.setVideoId(tempSendVideoInfo.getVideoId());
				tpvi.setVideoName(tempSendVideoInfo.getVideoName());
				tpvi.setVideoTypes(new ArrayList(tempSendVideoInfo
						.getVideoTypes()));
				tpvi.setAllTimes(fileInfo.getMAllTime());
				tpvi.setNextName(fileInfo.getMNextName());
				tpvi.setNowTimes(fileInfo.getMNowTime());
				// 失败标志至1（成功）
				tpvi.setStateMark(1);
			} else {
				
				tpvi = new ToPageVideoInfo();
				tpvi.setVideoName("无法得到视频信息！");
				// 失败标志至0
				tpvi.setStateMark(0);
			}

		} else if(tempobj instanceof String){
			if(((String)tempobj).equals("readOver")){
				tpvi = new ToPageVideoInfo();
				// 失败标志0
				tpvi.setStateMark(0);
				tpvi.setVideoName("终端应答超时！");
			}
			else if(((String)tempobj).equals("noClient")){
				tpvi = new ToPageVideoInfo();
				// 失败标志0
				tpvi.setStateMark(0);
				tpvi.setVideoName("终端未连接或者未播放视频！");
			}

		}
		return tpvi;
	}

	/**
	 * 检查终端是否处于激活状态(5分钟检查一次)
	 */
	public void setCheckTermActivation() {
		Date nowDate = new Date();
		Date diffDate = new Date(nowDate.getTime() - 300000);
		DetachedCriteria criteria = DetachedCriteria.forClass(TermInfo.class);
		criteria.add(Restrictions.lt("termLastLogintime", diffDate));
		List<TermInfo> termInfos = termInfoDao.getTermInfosByNum(criteria);
		for (TermInfo termInfo : termInfos) {
			termInfo.setTermActivation(0);
		}
	}
	/**
	 * 获取服务器时间
	 */
	public Date getServerTime(){
		Date date = new Date();
		return date;
	}
}
