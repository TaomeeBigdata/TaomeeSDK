package com.taomee.statlogger.main;

import com.taomee.statlogger.StatLogger;

public class MainClass {

	static StatLogger statLogger=new StatLogger(652, -1, -1, -1, 1);
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		statLogger.online_count(35, "");
		statLogger.reg_role("331025680", "-1", "男生", "0", "0");
		statLogger.login_online("331025680", "-1", "1战士", true, 12, "192.168.12.15", "0", "");
		statLogger.logout("331025680", false, "20", 650);
		
	}

}
