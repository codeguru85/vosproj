package com.broadengate.vcms.tcp.data.auxiliary;

import java.util.List;
import java.util.TimerTask;

import com.broadengate.vcms.entity.TermInfo;
import com.broadengate.vcms.tcp.data.TermsClient;

public class PlayTask extends TimerTask {
	List<TermInfo> termListsShip = null;
	String fileName;
	int videoSize;
	String filePath;
	int i=0;
	public PlayTask(List<TermInfo> termShip,String Name,int Size,String Path){
		this.termListsShip = termShip;
		this.fileName = Name;
		this.videoSize = Size;
		this.filePath = Path;
	}
	public void play(List<TermInfo> termListsShip,String fileName,int videoSize,String filePath){
		TermsClient tc = new TermsClient();
		tc.playMsgTalk(termListsShip,fileName,videoSize,filePath);
	}
	
	@Override
	public void run() {
		i++;
		// TODO Auto-generated method stub
		if(i>videoSize){
			this.cancel();
		}else{
			this.play(termListsShip, fileName, videoSize, filePath);
		}
	}

}
