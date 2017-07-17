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
	private String[][] task_stid = new String[][] { { "", "", "" },
			{ "_getnbtsk_", "_donenbtsk_", "_abrtnbtsk_" },
			{ "_getmaintsk_", "_donemaintsk_", "_abrtmaintsk_" },
			{ "_getauxtsk_", "_doneauxtsk_", "_abrtauxtsk_" },
			{ "_getetctsk_", "_doneetctsk_", "_abrtetctsk_" } };
	private static final int sc_magic1 = 0x01234567;
	private static final int sc_magic2 = 0x89ABCDEF;
	private static final int sc_magic3 = 0x13579BDF;
	private String[] new_trans_stid = new String[] { "", "fGetRegSucc",
			"fLoadRegSucc", "fSendLoginReq", "bGetLoginReq", "bSendLoginReq",
			"bGetLoginSucc", "fGetLoginSucc", "fLoadLoginSucc",
			"fClickStartBtn", "bGetNewroleReq", "bSendNewroleSucc",
			"fStartSrvlistReq", "bStartGetSrvlist", "bGetSrvlistSucc",
			"fGetSrvlistSucc", "fSendOnlineReq", "fSend1001Req",
			"bSendOnlineSucc", "fOnlineSucc", "fLoadInfoSucc", "fInterGameSucc" };

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
	 * @brief 用户登录游戏时(验证用户名和密码)调用
	 * @param acct_id 用户账户(米米号)
	 * @param cli_ip 用户的 IP 地址, 无法获取时取值 0
	 * @param ads_id 广告渠道
	 */
	public void verify_passwd(String acct_id,String cli_ip,String ads_id){
		if(acct_id.length() == 0){
			acct_id = "-1";
		}
		
		long ts = System.currentTimeMillis() / 1000;
		String op = "\t_op_=";
		String oss = "";
		oss += this.set_basic_info("_veripass_", "_veripass_", ts, acct_id, "-1");
		oss += set_device_info(ads_id);
		
		if(cli_ip.length() != 0 && this.is_valid_ip(cli_ip)){
			oss += ("\t_cip_="+cli_ip);
			op += ("ip_distr:_cip_|");
		}
		
		if(op.length() > 6){
			op = op.substring(0, op.length()-1);
			oss += (op + "\n");
		}else{
			oss += "\n";
		}
		
		this.write_basic_log(oss, ts);
	}
	
	/**
	 * @brief 用户在游戏中创建角色时调用
	 * 
	 * @param acct_id 用户账户(米米号)
	 * @param player_id 角色 id
	 * @param race 职业
	 * @param cli_ip 用户的 IP 地址
	 * @param ads_id 广告渠道
	 * 
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
	 * @brief 登录接口
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
	
	/**
	 * @brief 用户退出系统
	 * 
	 * @param acct_id 账户id
	 * @param isvip  是否为vip
	 * @param lv  游戏等级
	 * @param oltime  游戏在线时间
	 * 
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
	 * @brief 在线统计
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
	
	/**
	 * @brief 玩家每次等级提升时调用
	 */
	public void level_up(String acct_id,String race,int lv){
		if(!this.is_valid_lv(lv))
			return;
		if(acct_id.length() == 0){
			acct_id = "-1";
		}
		
		long ts = System.currentTimeMillis() / 1000;
		String oss = "";
		oss += set_basic_info("_aclvup_", "_aclvup_", ts, acct_id, "-1");
		oss += ("\t_lv_="+lv+"\t_op_=set_distr:_lv_\n");
		
		if(!(race.length() == 0)){
			StatCommon.stat_trim_underscore(race);
			if(!this.is_valid_race(race))
				return;
			oss += set_basic_info("_racelvup_", race, ts, acct_id, "-1");
			oss += ("\t_lv_="+lv+"\t_op_=set_distr:_lv_\n");
		}
		
		this.write_basic_log(oss, ts);
	}
	
	/**
     * @brief 玩家每次在游戏内使用米币购买道具时调用
     * @param outcome 付费获得的道具名称。如果pay_reason选择的是pay_buy，则outcome赋值为相应的“道具名字”；
     *                如果pay_reason的值不是pay_buy，则本参数可以赋空字符串。
     * @param pay_channel 支付类型，如米币卡、米币帐户、支付宝、苹果官方、网银、手机付费等等，米币渠道传"1"。
     */
	public void pay(String acct_id, boolean isvip, int pay_amount,
			CurrencyType currency, PayReason pay_reason, String outcome,
			int outcnt, String pay_channel) {
		if(!(	pay_amount > 0
				&& this.is_valid_currency(currency) 
				&& this.is_valid_payreason(pay_reason)
				&& outcnt > 0
				&& (outcome.length() > 0 || pay_reason != PayReason.pay_buy)
				&& this.is_valid_common_utf8_parm(outcome,0,256)
				&& this.is_valid_common_utf8_parm(pay_channel,1,256)
			))
			return;
		
		if(acct_id.length() == 0){
			acct_id = "-1";
		}
		
		String reason;
		switch (pay_reason){
		case pay_charge:
			reason = "_buycoins_";
			break;
		case pay_vip :
			reason = "_vipmonth_";
            break;
		case pay_buy:
			reason = "_buyitem_";
			break;
		case pay_free:
			reason = "_costfree_";
			break;
		default:
			reason = "";
			break;
		}
		
		long ts = System.currentTimeMillis() / 1000;
		String op = "\t_op_=sum:_amt_|item_sum:_vip_,_amt_|item_sum:_paychannel_,_amt_|item_sum:_ccy_,_amt_|item:_paychannel_";
		String oss = "";
		if(pay_reason!=PayReason.pay_free){
			oss += this.set_basic_info("_acpay_", "_acpay_", ts, acct_id, "-1")
					+"\t_vip_="
					+this.convert_isvip(isvip)
					+"\t_amt_="
					+pay_amount
					+"\t_ccy_="
					+currency
					+"\t_paychannel_="
					+pay_channel
					+op
					+"\n";
		}
		oss += this.set_basic_info("_acpay_", reason,ts, acct_id, "-1")
				+"\t_vip_="
				+this.convert_isvip(isvip)
				+"\t_amt_="
				+pay_amount
				+"\t_ccy_="
				+currency
				+"\t_paychannel_="
				+pay_channel
				+op
				+"\n";
		
		this.write_basic_log(oss, ts);
		
		switch(pay_reason){
		case pay_charge:
			this.do_obtain_golds(acct_id,"_userbuy_", outcnt);
			break;
		case pay_vip:
			this.do_buy_vip(acct_id,pay_amount,outcnt);
			break;
		case pay_buy:
			this.do_buy_item(acct_id,isvip,0,pay_amount,"_mibiitem_", outcome, outcnt);
			break;
		default:
			break;
		}
	}
	
	/**
	 * 玩家每次获得游戏金币时调用 也就是游戏币产出。注意：玩家通过付费充值方式获得金币时，系统会自动调用该函数。
	 */
	public void obtain_golds(String acct_id,int amt){
		if(!(0 < amt && amt <= 1000000000))return;
		if(acct_id.length() == 0){
			acct_id = "-1";
		}
		this.do_obtain_golds(acct_id, "_systemsend_", this.m_need_multi?amt*100:amt);
	}
	
	/**
	 * 游戏内一级货币购买道具
	 * @param pay_amount 付出的米币/游戏金币数量
	 * @param outcome 购买的道具名称
	 * @param outcnt 道具数量
     * 通过一级游戏币购买道具时调用
	 */
	public void buy_item(String acct_id, boolean isvip, int lv, int pay_amount, String outcome, int outcnt){
		if(!this.is_valid_lv(lv)
				&& pay_amount > 0
				&& this.is_valid_common_utf8_parm(outcome, 1, 256)
				&& outcnt > 0)
			return;
		if(acct_id.length() == 0){
			acct_id = "-1";
		}
		if(outcome.length() == 0){
			outcome = "-1";
		}
		
		this.do_buy_item(acct_id, isvip, lv, this.m_need_multi?pay_amount*100:pay_amount, "_coinsbuyitem_", outcome, outcnt);
		this.use_golds_buyitem(acct_id,isvip,this.m_need_multi?pay_amount*100:pay_amount,lv);
		
	}
	
	/**
	 * @brief 玩家每次使用游戏金币时调用 也就是说游戏币消耗。
	 * @param reason 使用游戏内一级货币的原因，如获得特权、复活等等。 购买道具时无需调用。
	 * @param reason 使用的数量。1~100000。
	 */
	public void use_golds(String acct_id,boolean is_vip,String reason,float amt,int lv){
		StatCommon.stat_trim_underscore(reason);
		
		if(!(amt > 0
			&& amt <= 100000)
			&& this.is_valid_common_utf8_parm(reason,1,256)
			&& this.is_valid_lv(lv))
			return;
		
		if(acct_id.length() == 0){
			acct_id = "-1";
		}
		
		if(reason.length() == 0){
			reason = "unknown";
		}
		
		long ts = System.currentTimeMillis() / 1000;
		String oss = "";
		oss += this.set_basic_info("_usegold_", "_usegold_", ts, acct_id, "-1");
		oss += ("\t_golds_="+(this.m_need_multi?amt*100:amt)+"\t_op_=sum:_golds_\n");
		
		oss += set_basic_info("_usegold_", reason, ts, acct_id, "-1");
		oss += ("\t_golds_="+(this.m_need_multi?amt*100:amt)+"\t_isvip_="+is_vip+"\t_lv_="+lv+"\t_op_=item:_isvip_|item_sum:_lv_,_golds_|sum:_golds_\n");
		
		this.write_basic_log(oss,ts);
	}
	
	/**
	 * @brief 接受任务。任务预设新手、主线、支线和其他。其他类任务可以通过页面配置具体任务类型。
	 */
	public void accept_task(TaskType type, String acct_id, String task_name, int lv) {
        this.do_task(type, acct_id, task_name, lv, 0);
    }
	
	/**
	 *  @brief 完成任务。
	 */
    public void finish_task(TaskType type, String acct_id, String task_name, int lv) {
        this.do_task(type, acct_id, task_name, lv, 1);
    }
    
    /**
	 * @brief 放弃任务。
	 */
    public void abort_task(TaskType type, String acct_id, String task_name, int lv) {
        this.do_task(type, acct_id, task_name, lv, 2);
    }
	
    /**
	 * @brief 玩家获得精灵时调用
	 */
	public void obtain_spirit(String acct_id,boolean isvip,int lv ,String spirit){
		if(!this.is_valid_lv(lv))
			return;
		
		if(acct_id.length() == 0){
			acct_id = "-1";
		}
		
		long ts = System.currentTimeMillis() / 1000;
		String oss = "";
		StatCommon.stat_trim_underscore(spirit);
		oss += this.set_basic_info("_obtainspirit_", "_obtainspirit_", ts, acct_id, "-1");
		oss += ("\t_lv_="+lv+"\t_vip_="+this.convert_isvip(isvip)+"\t_spirit_="+spirit+"\n");
		
		this.write_basic_log(oss,ts);
	}
	
	/**
	 * @brief 玩家失去精灵时调用
	 */
	public void lose_spirit(String acct_id,boolean isvip,int lv,String spirit){
		if(!this.is_valid_lv(lv))
			return;
		
		if(acct_id.length() == 0){
			acct_id = "-1";
		}
		
		long ts = System.currentTimeMillis() / 1000;
		String oss = "";
		StatCommon.stat_trim_underscore(spirit);
		oss += this.set_basic_info("_losespirit_", "_losespirit_",ts,acct_id,"-1");
		oss += ("\t_lv_="+lv+"\t_vip_="+this.convert_isvip(isvip)+"\t_spirit_="+spirit+"\n");
		
		this.write_basic_log(oss,ts);
	}
	
	/**
     * unsubscribe 退订VIP服务
     *
     * @param std::acct_id  用户账户
     * @param channel   退订渠道(目前只有米币和短信两个渠道)
     */
	public void unsubscribe(String acct_id,UnsubscribeChannel uc){
		if(acct_id.length() == 0){
			acct_id = "-1";
		}
		
		long ts = System.currentTimeMillis() / 1000;
		String oss = "";
		String op = "\t_op_=item:_uc_";
		oss += set_basic_info("_unsub_", "_unsub_", ts, acct_id, "-1");
		oss += ("\t_uc_="+uc+op+"\n");
		this.write_basic_log(oss, ts);
	}
	
	/**
     * @brief cancel_acct 销户
     *
     * @param acct_id   用户帐户
     * @param channel   销户渠道
     */
	public void cancel_acct(String acct_id,String channel){
		if(acct_id.length() == 0)
			acct_id = "-1";
		
		long ts = System.currentTimeMillis() / 1000;
		String oss = "";
		String op = "\t_op_=item:_cac_";
		oss += this.set_basic_info("_ccacct_", "_ccacct_", ts, acct_id, "-1");
		oss += ("\t_cac_="+channel+op+"\n");
		this.write_basic_log(oss,ts);
		
	}
    
	/**
	 * @brief 玩家每次获得游戏金币时调用 也就是游戏币产出。注意：玩家通过付费充值方式获得金币时，系统会自动调用该函数。
	 * @param reason 获得的金币数量。1~1000000。 系统赠送游戏内一级货币时调用
	 */
	public void new_trans(NewTransStep step,String acct_id){
		if(!this.is_valid_newtransstep(step))
			return;
		
		if(acct_id.length() == 0){
			acct_id = "-1";
		}
		
		long ts = System.currentTimeMillis() / 1000;
		String oss = "";
		oss += this.set_basic_info("_newtrans_", this.get_new_trans_step(step), ts, acct_id, "-1")+"\n";
		
		this.write_basic_log(oss, ts);
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
	private Integer convert_isvip(boolean isvip) {
		Integer isvip_flag = 0;
		if (isvip) {
			isvip_flag = 1;
		}
		return isvip_flag;
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
				// System.out.println("设置文件的可执行权限");
				
				Runtime.getRuntime().exec("chmod 777 -R "+file);
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
	
	private boolean is_valid_race(String race) {
		return StatCommon.size_between(race, 1, 256)
				&& StatCommon.value_no_invalid_chars(race)
				&& StatCommon.stat_is_utf8(race);
	}

	
	private void do_obtain_golds(String acct_id, String reason, int amt) {
		long ts = System.currentTimeMillis() / 1000;
		String oss = "";
		oss += this.set_basic_info("_getgold_", reason, ts, acct_id, "-1")
				+"\t_golds_="
				+amt
				+"\t_op_=sum:_golds_\n";
		this.write_basic_log(oss, ts);
	}

	private void do_buy_vip(String acct_id, int pay_amount, int amt) {
		long ts = System.currentTimeMillis() / 1000;
		String oss = "";
		oss += this.set_basic_info("_buyvip_", "_buyvip_", ts, acct_id, "-1")
				+"\t_payamt_="
				+pay_amount
				+"\t_amt_="
				+amt
				+"\t_op_=item:_amt_|item_sum:_amt_,_payamt_\n";
		this.write_basic_log(oss, ts);
	}

	private void do_buy_item(String acct_id, boolean isvip, int lv,
			int pay_amount, String pay_type, String outcome, int outcnt) {
		long ts = System.currentTimeMillis() / 1000;
		String oss = "";
		oss += this.set_basic_info("_buyitem_", pay_type, ts, acct_id, "-1");
		String op = "\t_op_=sum:_golds_";
		if(this.is_valid_lv(lv) && lv != 0){
			op += "|item:_lv_";
		}
		oss += ("\t_isvip_="+this.convert_isvip(isvip)+"\t_item_="+outcome+"\t_itmcnt_="+outcnt+"\t_golds_="+pay_amount+"\t_lv_="+lv+op+"\n");
		this.write_basic_log(oss, ts);
	}

	
	private boolean is_valid_lv(int lv) {
		return lv >= 0 && lv <= 5000;
	}

	private boolean is_valid_common_utf8_parm(String parm, Integer min, Integer max) {
		return StatCommon.size_between(parm, min, max)
				&& StatCommon.value_no_invalid_chars(parm)
				&& StatCommon.stat_is_utf8(parm);
	}

	private boolean is_valid_payreason(PayReason r) {
		return r.compareTo(PayReason.pay_begin)>0 && r.compareTo(PayReason.pay_end)<0;
	}

	private boolean is_valid_currency(CurrencyType ccy) {
		return ccy.compareTo(CurrencyType.ccy_begin)>0 && ccy.compareTo(CurrencyType.ccy_end)<0;
	}
	
	private void use_golds_buyitem(String acct_id, boolean is_vip, int amt, int lv) {
		if(acct_id.length() == 0){
			acct_id = "-1";
		}
		
		long ts = System.currentTimeMillis() / 1000;
		String oss = "";
		oss += set_basic_info("_usegold_", "_usegold_", ts, acct_id, "-1");
		oss += ("\t_golds_="+amt+"\t_op_=sum:_golds_\n");
		
		oss += set_basic_info("_usegold_", "_buyitem_", ts, acct_id, "-1");
		oss += ("\t_golds_="+amt+"\t_isvip_="+this.convert_isvip(is_vip)+"\t_lv_="+lv+"\t_op_=item:_isvip_|item_sum:_lv_,_golds_|sum:_golds_\n");
		
		this.write_basic_log(oss, ts);
	}
	
	private void do_task(TaskType type, String acct_id, String task_name,int lv, int step) {
		StatCommon.stat_trim_underscore(task_name);//TODO
		
		if(!(this.is_valid_tasktype(type)
				&& this.is_valid_common_utf8_parm(task_name, 1, 256)))
			return;
		
		if(acct_id.length() == 0){
			acct_id = "-1";
		}
		if(task_name.length() == 0){
			task_name = "unknown";
		}
		
		long ts = System.currentTimeMillis() / 1000;
		String oss = "";
		
		oss += set_basic_info(this.get_task_stid(type,step), task_name, ts, acct_id, "-1");
		if(this.is_valid_lv(lv) && lv > 0){
			oss += ("\t_lv_="+lv+"\t_op_=item:_lv_\n");
		}else{
			oss += "\n";
		}
		
		this.write_basic_log(oss,ts);
	}

	private boolean is_valid_tasktype(TaskType type) {
		return type.compareTo(TaskType.task_begin)>0 && type.compareTo(TaskType.task_end)<0;
	}
	
	private String get_task_stid(TaskType type, int stage) {
		return this.task_stid[type.getValue()][stage];
	}
	
	public boolean is_valid_ip(String ip) {
		String[] dip = ip.split("\\.");
		if(dip.length != 4)
			return false;
		for(String num:dip){
			if(Integer.valueOf(num) >255 || Integer.valueOf(num) < 0){
				return false;
			}
		}
		return true;
	}
	
	private boolean is_valid_newtransstep(NewTransStep step) {
		return step.compareTo(NewTransStep.nw_begin)>0 && step.compareTo(NewTransStep.nw_end)<0;
	}
	
	private String get_new_trans_step(NewTransStep step) {
		return this.new_trans_stid[step.getValue()];
	}
	
}

//class CurrencyType{
//	public static final int CCY_BEGIN = 0;
//	public static final int CCY_MIBI  = 1;
//	public static final int CCY_CNY   = 2;
//	public static final int CCY_END   = 3;
//}



//class PayReason{
//	public static final int PAY_BEGIN = 0;
//	public static final int PAY_VIP   = 1;
//	public static final int PAY_BUY   = 2;
//	public static final int PAY_CHARGE= 3;
//	public static final int PAY_FREE  = 4;
//	public static final int PAY_END   = 5;
//}


