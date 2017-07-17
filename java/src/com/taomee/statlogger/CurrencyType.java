package com.taomee.statlogger;

public enum CurrencyType{
	ccy_begin(0),ccy_mibi(1),ccy_cny(2),ccy_end(3);
	private int value;
	private CurrencyType(){
		;
	}
	private CurrencyType(int value){
		this.value = value;
	}
	public int getValue() {
		return value;
	}
	public String toString(){
		return this.getValue()+"";
	}
}
