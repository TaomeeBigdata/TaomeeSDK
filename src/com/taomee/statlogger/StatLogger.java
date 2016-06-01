package com.taomee.statlogger;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

/**
 * 淘米数据接口
 * 
 * @author looper
 * @date 2016年5月17日
 */
public class StatLogger {
	// OutputStream os;
	private static final int sc_appid_minsz = 6;
	private static final int sc_appid_sz = 9;
	private static final int sc_ip_minsz = 6;
	private static final int sc_ip_sz = 64;
	private static final int sc_path_sz = 256;
	private static final int sc_param_maxsz = 128;
	private int m_chksum1;
	private int m_siteid;
	private int m_appid;
	private int m_isgame;
	private int m_magic_num1;
	private int m_zoneid;
	private int m_svrid;
	private String m_hostip;
	private int m_magic_num2;
	private String m_path;
	private int m_basic_fd;
	private int m_custom_fd;
	private int m_magic_num3;
	private long m_basic_ts;
	private long m_custom_ts;
	private int m_inited;
	private boolean m_need_multi;
	private int m_chksum2;
	private static final int sc_magic1 = 0x01234567;
	private static final int sc_magic2 = 0x89ABCDEF;
	private static final int sc_magic3 = 0x13579BDF;

	public int getM_chksum1() {
		return m_chksum1;
	}

	public void setM_chksum1(int m_chksum1) {
		this.m_chksum1 = m_chksum1;
	}

	public int getM_siteid() {
		return m_siteid;
	}

	public void setM_siteid(int m_siteid) {
		this.m_siteid = m_siteid;
	}

	public int getM_appid() {
		return m_appid;
	}

	public void setM_appid(int m_appid) {
		this.m_appid = m_appid;
	}

	public int getM_isgame() {
		return m_isgame;
	}

	public void setM_isgame(int m_isgame) {
		this.m_isgame = m_isgame;
	}

	public int getM_magic_num1() {
		return m_magic_num1;
	}

	public void setM_magic_num1(int m_magic_num1) {
		this.m_magic_num1 = m_magic_num1;
	}

	public int getM_zoneid() {
		return m_zoneid;
	}

	public void setM_zoneid(int m_zoneid) {
		this.m_zoneid = m_zoneid;
	}

	public int getM_svrid() {
		return m_svrid;
	}

	public void setM_svrid(int m_svrid) {
		this.m_svrid = m_svrid;
	}

	public String getM_hostip() {
		return m_hostip;
	}

	public void setM_hostip(String m_hostip) {
		this.m_hostip = m_hostip;
	}

	public int getM_magic_num2() {
		return m_magic_num2;
	}

	public void setM_magic_num2(int m_magic_num2) {
		this.m_magic_num2 = m_magic_num2;
	}

	public String getM_path() {
		return m_path;
	}

	public void setM_path(String m_path) {
		this.m_path = m_path;
	}

	public int getM_basic_fd() {
		return m_basic_fd;
	}

	public void setM_basic_fd(int m_basic_fd) {
		this.m_basic_fd = m_basic_fd;
	}

	public int getM_custom_fd() {
		return m_custom_fd;
	}

	public void setM_custom_fd(int m_custom_fd) {
		this.m_custom_fd = m_custom_fd;
	}

	public int getM_magic_num3() {
		return m_magic_num3;
	}

	public void setM_magic_num3(int m_magic_num3) {
		this.m_magic_num3 = m_magic_num3;
	}

	public long getM_basic_ts() {
		return m_basic_ts;
	}

	public void setM_basic_ts(long m_basic_ts) {
		this.m_basic_ts = m_basic_ts;
	}

	public long getM_custom_ts() {
		return m_custom_ts;
	}

	public void setM_custom_ts(long m_custom_ts) {
		this.m_custom_ts = m_custom_ts;
	}

	public int getM_inited() {
		return m_inited;
	}

	public void setM_inited(int m_inited) {
		this.m_inited = m_inited;
	}

	public boolean isM_need_multi() {
		return m_need_multi;
	}

	public void setM_need_multi(boolean m_need_multi) {
		this.m_need_multi = m_need_multi;
	}

	public int getM_chksum2() {
		return m_chksum2;
	}

	public void setM_chksum2(int m_chksum2) {
		this.m_chksum2 = m_chksum2;
	}

	public static int getScAppidMinsz() {
		return sc_appid_minsz;
	}

	public static int getScAppidSz() {
		return sc_appid_sz;
	}

	public static int getScIpMinsz() {
		return sc_ip_minsz;
	}

	public static int getScIpSz() {
		return sc_ip_sz;
	}

	public static int getScPathSz() {
		return sc_path_sz;
	}

	public static int getScParamMaxsz() {
		return sc_param_maxsz;
	}

	public static int getScMagic1() {
		return sc_magic1;
	}

	public static int getScMagic2() {
		return sc_magic2;
	}

	public static int getScMagic3() {
		return sc_magic3;
	}

	/**
	 * 
	 * @param game
	 * @param zone
	 * @param svr
	 * @param site
	 * @param isgame
	 */
	public StatLogger(int game, int zone, int svr, int site, int isgame) {
		// super();
		//String path = "c:/opt/taomee/stat/data2";
		String path="";
		/**
		 * 根据操作系统类型，切换不同的目录
		 * windows的目录为测试目录
		 * Linux操作系统为
		 */
		if(StatCommon.isWindowsOS())
		{
			path="c:/opt/taomee/stat/data";
		}else
		{
			path="/opt/taomee/stat/data";
			//path="/home/looper/data";
		}
		
		if (!(StatCommon.stat_makedir(path + "/inbox")))
			return;
		this.m_path = path;
		this.m_appid = game;
		this.m_siteid = site;
		this.m_zoneid = zone;
		this.m_svrid = svr;
		this.m_isgame = isgame;
		if (this.m_appid == 1 || this.m_appid == 2 || this.m_appid == 5
				|| this.m_appid == 6 || this.m_appid == 10
				|| this.m_appid == 16 || this.m_appid == 19) {
			this.m_need_multi = false;
		} else
			this.m_need_multi = true;
		this.m_basic_fd = -1;
		this.m_custom_fd = -1;
		this.m_basic_ts = 0;
		this.m_custom_ts = 0;
		this.m_hostip = StatCommon.stat_get_ip_addr();
		this.m_inited = 1;
	}

	/**
	 * 在线统计
	 * 
	 * @param cnt
	 *            每分钟人数
	 * @param zone
	 *            telecom , netcom
	 * 
	 */
	public void online_count(int cnt, String zone) {
		if (cnt < 0)
			return;
		String s=StatCommon.stat_trim_underscore(zone);
		if(s=="")
		{
			s="_all_";
		}
		long ts = System.currentTimeMillis() / 1000;
		// OutputStream oss;
		// if()
		String basic_info = set_basic_info("_olcnt_", "_olcnt_", ts, "-1", "-1");
		// $oss =
		// $oss."\t_zone_=".$zone."\t_olcnt_=".$cnt."\t_op_=item_max:_zone_,_olcnt_\n";
		String online_info = basic_info + "\t_zone_=" + s + "\t_olcnt_="
				+ cnt + "\t_op_=item_max:_zone_,_olcnt_\n";
		write_basic_log(online_info, ts);
	}

	private String set_basic_info(String stid, String sstid, long ts,
			String acct, String plid)

	{
		return "_hip_=" + this.m_hostip + "\t_stid_=" + stid + "\t_sstid_="
				+ sstid + "\t_gid_=" + this.m_appid + "\t_zid_="
				+ this.m_zoneid + "\t_sid_=" + this.m_svrid + "\t_pid_="
				+ this.m_siteid + "\t_ts_=" + ts + "\t_acid_=" + acct
				+ "\t_plid_=" + plid;
		// return null;
	}

	/**
	 * 
	 * @param ads
	 * @return
	 */
	private String set_device_info(String ads) {
		return "\t_ad_=" + ads;
	}

	/**
	 * 创建角色
	 * 
	 * @param acct_id
	 *            账户Id
	 * @param player_id
	 *            角色Id
	 * @param race
	 *            角色名称
	 * @param cli_ip
	 *            客户端IP
	 * @param ads_id
	 *            广告ID
	 */
	public void reg_role(String acct_id, String player_id, String race,
			String cli_ip, String ads_id) {
		long ts = System.currentTimeMillis() / 1000;
		String op = "\t_op_=";
		String clip = null;
		String basic_info = set_basic_info("_newac_", "_newac_", ts, acct_id,
				player_id);
		String device_info = set_device_info(ads_id);
		String newac_info = "";
		if (cli_ip.length() != 0) {
			clip = "\t_cip_" + cli_ip;
			op = op + "ip_distr:_cip_|";
		}
		if (op.length() > 6) {
			op = op.substring(0, op.length() - 1) + "\n";
			newac_info = basic_info + device_info + clip + op;
			// System.out.println(op);
		} else {
			newac_info = basic_info + device_info + cli_ip + "\n";
		}
		write_basic_log(newac_info, ts);
		// 新建角色
		String newpl_info = set_basic_info("_newpl_", "_newpl_", ts, acct_id,
				player_id) + "\n";
		write_basic_log(newpl_info, ts);
		// 职业新增
		String race2=StatCommon.stat_trim_underscore(race);
		String newrace_info = set_basic_info("_newrace_", race2, ts, acct_id,
				player_id) + "\n";
		write_basic_log(newrace_info, ts);

	}

	/**
	 * 登录接口
	 * 
	 * @param acct_id
	 *            账户ID
	 * @param player_id
	 *            角色ID，没有就输入"-1"
	 * @param race
	 *            角色，没有就输入"-1"
	 * @param isvip
	 *            是否为vip
	 * @param lv
	 *            游戏等级
	 * @param cli_ip
	 *            客户端IP
	 * @param ads_id
	 *            广告位
	 * @param zone
	 *            _all_ , telecom , netcom
	 */
	public void login_online(String acct_id, String player_id, String race,
			boolean isvip, Integer lv, String cli_ip, String ads_id, String zone) {
		long ts = System.currentTimeMillis() / 1000;
		String op = "\t_op_=item:_vip_|";
		if (lv > 0) {
			op = op + "item:_lv_|";
		}
		String lgac_info = set_basic_info("_lgac_", "_lgac_", ts, acct_id,
				player_id)
				+ "\t_vip_="
				+ convert_isvip(isvip)
				+ "\t_lv_="
				+ lv
				+ set_device_info(ads_id);
		if (cli_ip.length() != 0) {
			lgac_info = lgac_info + "\t_cip_=" + cli_ip;
			op = op + "ip_distr:_cip_|";
		}
		if(zone=="")
		{
			zone="_all_";
		}

		lgac_info = lgac_info + "\t_zone_=" + zone;
		op = op + "item:_zone_|";

		if (op.length() > 6) {
			op = op.substring(0, op.length() - 1) + "\n";
			lgac_info = lgac_info + op;
			// System.out.println(op);
		} else {
			lgac_info = lgac_info + "\n";
		}
		write_basic_log(lgac_info, ts);

		String lgpl_info = set_basic_info("_lgpl_", "_lgpl_", ts, acct_id,
				player_id) + "\n";
		write_basic_log(lgpl_info, ts);

		String race2=StatCommon.stat_trim_underscore(race);
		String lgrace_info = set_basic_info("_lgrace_", race2, ts, acct_id,
				player_id)
				+ "\t_vip_="
				+ this.convert_isvip(isvip)
				+ "\t_lv_"
				+ lv + "\t_op_=item:_vip_|item:_lv_\n";
		write_basic_log(lgrace_info, ts);

	}

	private Integer convert_isvip(boolean isvip) {
		Integer isvip_flag = 0;
		if (isvip) {
			isvip_flag = 1;
		}
		return isvip_flag;
	}

	/**
	 * 用户退出系统
	 * 
	 * @param acct_id 账户id
	 * @param isvip  是否为vip
	 * @param lv  游戏等级
	 * @param oltime  游戏在线时间
	 */
	public void logout(String acct_id, boolean isvip, String lv, Integer oltime) {
		long ts = System.currentTimeMillis() / 1000;
		String logout_info = set_basic_info("_logout_", "_logout_", ts,
				acct_id, "-1")
				+ "\t_vip_="
				+ convert_isvip(isvip)
				+ "\t_lv_="
				+ lv
				+ "\t_oltm_="
				+ oltime
				+ "\t_intv_="
				+ logout_time_interval(oltime)
				+ "\t_op_=sum:_oltm_|item:_intv_\n";
		write_basic_log(logout_info, ts);
	}

	/**
	 * 退出时间分布
	 * 
	 * @param tm
	 * @return
	 */
	private String logout_time_interval(Integer tm) {
		// String lv;
		if (tm <= 0) {
			return "0";
		} else if (tm < 11) {
			return "0~10";
		} else if (tm < 61) {
			return "11~60";
		} else if (tm < 301) {
			return "61~300";
		} else if (tm < 601) {
			return "301~600";
		} else if (tm < 1201) {
			return "601~1200";
		} else if (tm < 1801) {
			return "1201~1800";
		} else if (tm < 2401) {
			return "1801~2400";
		} else if (tm < 3001) {
			return "2401~3000";
		} else if (tm < 3601) {
			return "3001~3600";
		} else if (tm < 7201) {
			return "3601~7200";
		} else {
			return "大于7200";
		}

	}

	/*	*//**
	 * 把日志写入文件当中
	 * 
	 * @param info
	 *            要写入的日志记录
	 * @param ts
	 */
	/*
	 * private void write_basic_log(String info, long ts) {
	 * 
	 * if (this.m_inited == 0) { System.out.println("对象不能被初始化"); return; }
	 * m_basic_ts = ts - (ts % 20); System.out.println("1:"+ts);
	 * System.out.println("1:"+this.m_basic_ts); //|| (m_basic_ts + 8 * 60 * 60)
	 * / 86400 != (ts + 8 * 60 * 60 )/ 86400) if ((ts < this.m_basic_ts) || ((ts
	 * - this.m_basic_ts) > 19)) { //m_basic_ts = ts - (ts % 20);
	 * System.out.println("2:"+ts); System.out.println("2:"+this.m_basic_ts);
	 * //System.out.println(); String file_name = this.m_path + "/inbox/" +
	 * m_appid + "_game_basic_" + m_basic_ts; // System.out.println(file_name);
	 * FileOutputStream fos = null; File file = new File(file_name); try { fos =
	 * new FileOutputStream(file, true); String info2 = info + "\n"; byte[]
	 * bytes = info.getBytes(); fos.write(bytes); fos.flush();
	 *//**
	 * 这个设置文件的权限得等文件刷新到系统当中再去执行。
	 */
	/*
	 * if (!file.canExecute()) { file.setExecutable(true); //
	 * System.out.println("设置文件的可执行权限"); }
	 * 
	 * // System.out.println("写入数据成功"); } catch (Exception e) { // TODO
	 * Auto-generated catch block e.printStackTrace(); } finally { //
	 * System.out.println(fos); if (fos != null) { try { fos.close(); } catch
	 * (IOException e) { // TODO Auto-generated catch block e.printStackTrace();
	 * } } }
	 * 
	 * } }
	 */
	private void write_basic_log(String info, long ts) {
		if (this.m_inited == 0) {
			System.out.println("对象不能被初始化");
			return;
		}
		String file_name = null;
		// if()
		if ((ts < this.m_basic_ts) || ((ts - this.m_basic_ts) > 19) || ((m_basic_ts + 8*60*60)/86400 != (ts + 8*60*60)/ 86400)) {
			m_basic_ts = ts - (ts % 20);
			file_name = this.m_path + "/inbox/" + m_appid + "_game_basic_"
					+ m_basic_ts;
		}
		/*
		 * if(file_name==null) { file_name = this.m_path + "/inbox/" + m_appid +
		 * "_game_basic_" + m_basic_ts; }
		 */
		// m_basic_ts = ts - (ts % 20);
		file_name = this.m_path + "/inbox/" + m_appid + "_game_basic_"
				+ m_basic_ts;
		//System.out.println(file_name);
		FileOutputStream fos = null;
		File file = new File(file_name);
		try {
			fos = new FileOutputStream(file, true);
			String info2 = info + "\n";
			//String info2 = info;
			byte[] bytes = info.getBytes();
			fos.write(bytes);
			fos.flush();
			if (!file.canExecute()) {
				file.setExecutable(true);
				Runtime.getRuntime().exec("chmod 777 -R "+file);
				// System.out.println("设置文件的可执行权限");
			}

			// System.out.println("写入数据成功");
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {
			if (fos != null) {
				try {
					fos.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}

}
