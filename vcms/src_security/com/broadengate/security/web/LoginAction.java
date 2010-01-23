/**
 * 
 */
package com.broadengate.security.web;

import javax.servlet.http.HttpSession;

import org.apache.log4j.Logger;
import org.apache.struts2.ServletActionContext;

import com.octo.captcha.service.image.ImageCaptchaService;
import com.opensymphony.xwork2.ActionContext;
import com.opensymphony.xwork2.ActionSupport;
import com.opensymphony.xwork2.validator.annotations.RequiredStringValidator;
import com.opensymphony.xwork2.validator.annotations.StringLengthFieldValidator;
import com.opensymphony.xwork2.validator.annotations.Validation;
import com.opensymphony.xwork2.validator.annotations.Validations;

/**
 * @author 龚磊
 * 
 */
@Validation
public class LoginAction extends ActionSupport {
	/**
	 * Logger for this class
	 */
	private static final Logger logger = Logger.getLogger(LoginAction.class);

	/**
	 * 
	 */
	private static final long serialVersionUID = -8734147070934873932L;

	/**
	 * 用户登陆名称
	 */
	private String userName;

	/**
	 * 用户登陆密码
	 */
	private String password;

	/**
	 * 验证码
	 */
	private String validateCode;

	private ImageCaptchaService jcaptchaService;

	public ImageCaptchaService getJcaptchaService() {
		return jcaptchaService;
	}

	public void setJcaptchaService(ImageCaptchaService jcaptchaService) {
		this.jcaptchaService = jcaptchaService;
	}

	/**
	 * 登陆处理
	 */
	@Validations(requiredStrings = {
			@RequiredStringValidator(fieldName = "userName", key = "security.user.userName.required", message = ""),
			@RequiredStringValidator(fieldName = "password", key = "security.user.password.required", message = ""),
			@RequiredStringValidator(fieldName = "validateCode", key = "security.user.validateCode.required", message = "") }, stringLengthFields = {
			@StringLengthFieldValidator(fieldName = "userName", minLength = "5", maxLength = "32", key = "security.user.userName.stringlength", message = ""),
			@StringLengthFieldValidator(fieldName = "password", minLength = "6", maxLength = "20", key = "security.user.password.stringlength", message = ""),
			@StringLengthFieldValidator(fieldName = "validateCode", minLength = "4", key = "security.user.validateCode.stringlength", message = "") })
	public String execute() throws Exception {
		// tempLocal是当前环境语言字符串
		String tempLocal = ActionContext.getContext().getLocale().toString();
		HttpSession session = ServletActionContext.getRequest().getSession();

		if (tempLocal.equals("zh_CN")) {
			session.setAttribute("DateFormatStandard", "yyyy-MM-dd");
			session.setAttribute("DateTimeFormatStandard", "yyyy-MM-dd HH:mm:ss");
		} else if (tempLocal.equals("en")) {
			session.setAttribute("DateFormatStandard", "MM/dd/yyyy");
			session.setAttribute("DateTimeFormatStandard", "MM/dd/yyyy HH:mm:ss");
		}
		String captchaValidator = ServletActionContext.getServletContext().getInitParameter(
				"captchaValidator");
		if (captchaValidator.equalsIgnoreCase("true")) {
			String captchaId = session.getId();
			boolean isCorrect = jcaptchaService.validateResponseForID(captchaId, validateCode);
			if (!isCorrect) {
				addFieldError("validateCode", getText("security.user.validateCode.error"));
				return INPUT;
			} else {
				return SUCCESS;
			}
		}else{
			return SUCCESS;
		}
	}

	public String getUserName() {
		return userName;
	}

	public void setUserName(String userName) {
		this.userName = userName;
	}

	public String getPassword() {
		return password;
	}

	public void setPassword(String password) {
		this.password = password;
	}

	public String getValidateCode() {
		return validateCode;
	}

	public void setValidateCode(String validateCode) {
		this.validateCode = validateCode;
	}

}
