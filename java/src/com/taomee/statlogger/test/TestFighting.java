package com.taomee.statlogger.test;

import javax.swing.plaf.SliderUI;

import org.junit.Test;

import com.taomee.statlogger.StatCommon;
import com.taomee.statlogger.StatLogger;

/**
 * 测试战争使命调用
 * @author looper
 * @date 2016年5月18日
 */
public class TestFighting {
	/**
	 * 
	 * StatLogger(参数1，参数2，参数3，参数4，参数5)
	 * 参数1：game_id  type:int
	 * 参数2：pid   type:int
	 * 参数3：zid   type:int
	 * 参数4：sid   type:int
	 * 参数5：是不是游戏(默认该值都写1) type:int
	 * 说明:一般设置该类StatLogger实例为全局变量
	 */
	static final StatLogger statLogger=new StatLogger(652, -1, -1, -1, 1);
	
	/**
	 * 第一个参数表示每分钟在线人数，每分钟调用一次,游戏那边如果可以直接传递第一个每分钟的在线人数
	 * online_count(参数1，参数2)
	 * 参数1:每分钟在线人数      type:int
	 * 参数2:电信or网通or:telecom , netcom
	 * 
	 * 
	 * 
	 */
	@Test
	public void test_online_count()
	{
		// for (int i = 0; i < 3; i++) {
		for (int i = 0; i < 50; i++) {

			statLogger.online_count(i, "");
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	//	}
		//statLogger.online_count(50, "_all_", "-1", "-1");
		//statLogger.online_count(50, "_all_", "-1", "-1");
		//System.out.println(Integer.MAX_VALUE);
		
	}
   /**
    * 注册角色
    */
	@Test
	public void test_reg_role()
	{
		
		for(int i=0;i<5;i++)
		{
		statLogger.reg_role("331025680", "-1", "-1", "192.168.11.120", "-1");
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		}
		
	}
	
	@Test
	public void test_utf8()
	{
		//System.out.println(StatCommon.stat_is_utf8("鎴樺"));
		//System.out.println("中国");
		System.out.println(StatCommon.stat_is_utf8("*��JTP.jar�ļ����JTP�ļ���ȡ��ͼƬ��Դ"));
	}
	/**
	 * 登录
	 */
	@Test
	public void test_login_online()
	{
		statLogger.login_online("331025680", "-1", "-1", false, 1, "192.168.11.120", "-1", "");
	}
	
	/**
	 * 登出
	 */
	@Test
	public void test_logout()
	{
		statLogger.logout("331025680",false, "5", 300);
	}
}
