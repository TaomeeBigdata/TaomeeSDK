package com.taomee.statlogger;

public enum NewTransStep {
	nw_begin(0),
	fgetregsucc(1),
	floadregsucc(2),
	fsendloginreq(3),
	bgetloginreq(4),
	bsendloginreq(5),
	bgetloginsucc(6),
	fgetloginsucc(7),
	floadloginsucc(8),
	fclickstartbtn(9),
	bgetnewrolereq(10),
	bsendnewrolesucc(11),
	fstartsrvlistreq(12),
	bstartgetsrvlist(13),
	bgetsrvlistsucc(14),
	fgetsrvlistsucc(15),
	fsendonlinereq(16),
	fsend1001req(17),
	bsendonlinesucc(18),
	fonlinesucc(19),
	floadinfosucc(20),
	fintergamesucc(21),
	nw_end(22);
	private int value;
	private NewTransStep(){
		;
	}
	private NewTransStep(int value){
		this.value = value;
	}
	public int getValue(){
		return this.value;
	}
}
