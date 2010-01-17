package com.broadengate.core.web.support.tags;

import java.util.Locale;

import javax.servlet.jsp.JspException;
import javax.servlet.jsp.JspWriter;
import javax.servlet.jsp.tagext.TagSupport;

public class I18nTag extends TagSupport {

	private static final long serialVersionUID = 1L;

	private String zh_cn;

	private String en_us;

	@Override
	public int doStartTag() throws JspException {
		JspWriter writer = this.pageContext.getOut();
		Locale locale = (Locale) pageContext.getSession().getAttribute("WW_TRANS_I18N_LOCALE");
		try {
			if ((locale.toString()).equals((locale.SIMPLIFIED_CHINESE).toString())) {
				writer.print(zh_cn);
			} else {
				writer.print(en_us);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return this.EVAL_PAGE;
	}

	public String getZh_cn() {
		return zh_cn;
	}

	public void setZh_cn(String zh_cn) {
		this.zh_cn = zh_cn;
	}

	public String getEn_us() {
		return en_us;
	}

	public void setEn_us(String en_us) {
		this.en_us = en_us;
	}

}
