/**Administrator
 * 下午03:32:06
 */
package com.broadengate.vcms.service;

import java.util.Date;
import java.util.List;

import com.broadengate.vcms.entity.TermInfo;
import com.broadengate.vcms.entity.TermType;
import com.broadengate.vcms.entity.VideoInfo;
import com.broadengate.vcms.tcp.data.info.LoginCome;
import com.broadengate.vcms.tcp.data.info.ToPageVideoInfo;
import com.broadengate.vcms.web.Finder;

/**
 * @author Administrator
 *
 */
public interface ITermService{
	
	/**
	 * 查询所有的视频内容列表
	 * @return 
	 * */
	public List<TermType> getTermTypes();
	
	/**
	 * 删除类型
	 * @param termType
	 * @return
	 */
	public TermType removeTermType(TermType termType);
	
	/**
	 * 增加类型
	 * @param termType
	 * @return
	 */
	public TermType addTermType(TermType termType);
	
	/**
	 * 修改类型
	 * @param termType
	 * @return
	 */
	public TermType editTermType(TermType termType);
	
	/**
	 * 重复终端判断
	 */
	public Boolean existTerm(TermInfo termInfo);
	
	/**
	 * 存储终端信息
	 * @param termInfo
	 * @return
	 */
	public TermInfo addTermInfo(TermInfo termInfo);
	
	/**
	 * 得到所有的终端信息
	 * @return
	 */
	public List<TermInfo> getAllTermInfos();
	
	/**
	 * 删除终端信息
	 * @param termInfo
	 * @return
	 */
	public TermInfo delTermInfo(TermInfo termInfo);
	

	/**
	 * 查询指定终端信息
	 * @param toPage
	 * @return
	 */
	public Finder getTerms(TermInfo termInfo, Integer toPage, Integer pageSize);
	
	/**
	 * 查询是否有指定termNum的机器
	 * @param theTermNum
	 * @return
	 */
	
	public  boolean getTermByTermNum(LoginCome loginCome);
	
	/**
	 * 终端控制操作
	 * @param ip
	 * @param con
	 * @param mark
	 * @return
	 */
	
	public String setOptionClient(String ip, int con, int mark,String num ,String clientName);
	
	/**
	 * 修改终端参数
	 * @param tempEditTerm
	 * @return
	 */

	public TermInfo editClientParas(TermInfo tempEditTerm);
	
	/**
	 * 反向查询
	 * @return
	 */
	public ToPageVideoInfo showPlayFile(TermInfo tempQueryTerm);
	
	/**
	 * 检查终端是否处于激活状态
	 */
	public void setCheckTermActivation();
	
	public Date getServerTime();
}
