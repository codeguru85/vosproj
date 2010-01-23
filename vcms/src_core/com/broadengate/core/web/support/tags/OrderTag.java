/**
 * 
 */
package com.broadengate.core.web.support.tags;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.jsp.JspException;
import javax.servlet.jsp.JspTagException;
import javax.servlet.jsp.JspWriter;
import javax.servlet.jsp.tagext.TagSupport;

import org.apache.commons.lang.StringUtils;

import com.broadengate.core.commons.pagination.Query;

/**
 * @author 龚磊
 * 
 */
public class OrderTag extends TagSupport {

	/**
	 * 
	 */
	private static final long serialVersionUID = 3263943530160416907L;
	
	private String upSrc;
	
	private String downSrc;
	
	private String orderName;

	public int doStartTag() throws JspTagException {
		if(StringUtils.isEmpty(upSrc)){
			upSrc="../images/up.gif";
		}
		if(StringUtils.isEmpty(downSrc)){
			downSrc="../images/down.gif";
		}
		return SKIP_BODY;
	}
	
	@Override
	public int doEndTag() throws JspException {
		HttpServletRequest request = (HttpServletRequest) pageContext.getRequest();
		Query query = (Query) request.getAttribute("query");
		JspWriter writer = pageContext.getOut();
		try {
			String sb=null;
			if (StringUtils.isNotEmpty(query.getOrder())&&query.getOrder().equals(orderName)) {
				if(query.getIsAsc()){
					sb="<img src=\""+upSrc+"\" title=\"up\" alt=\""+orderName+"\" style=\"visibility:visible\"/>" ;
				}else{
					sb="<img src=\""+downSrc+"\" title=\"down\" alt=\""+orderName+"\" style=\"visibility:visible\"/>" ;
				}
			}else{
				sb="<img src=\""+upSrc+"\" title=\"up\" alt=\""+orderName+"\" style=\"visibility:hidden\"/>" ;
			}
			writer.print(sb);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return EVAL_PAGE;
	}

	public String getUpSrc() {
		return upSrc;
	}

	public void setUpSrc(String upSrc) {
		this.upSrc = upSrc;
	}

	public String getDownSrc() {
		return downSrc;
	}

	public void setDownSrc(String downSrc) {
		this.downSrc = downSrc;
	}

	public String getOrderName() {
		return orderName;
	}

	public void setOrderName(String orderName) {
		this.orderName = orderName;
	}

}
