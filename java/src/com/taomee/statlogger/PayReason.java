package com.taomee.statlogger;

public enum PayReason{
	pay_begin(0),pay_vip(1),pay_buy(2),pay_charge(3),pay_free(4),pay_end(5);
	private int value;
	private PayReason(){
		
	}
	private PayReason(int value){
		this.value = value;
	}
	public boolean equals(PayReason pr){
		return this.value == pr.value;
	}
}