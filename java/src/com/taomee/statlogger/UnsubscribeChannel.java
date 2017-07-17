package com.taomee.statlogger;

public enum UnsubscribeChannel {
	uc_begin(0),uc_duanxin(1),uc_mibi(2),uc_end();
	private int value;
	private UnsubscribeChannel(){
		;
	}
	private UnsubscribeChannel(int value){
		this.value = value;
	}
}
