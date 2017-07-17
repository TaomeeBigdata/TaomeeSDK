#include <cassert>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <utility>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "fs_utils.h"
#include "inet_utils.h"

#include "statlogger.h"

#include <iostream>

using namespace std;

#define MY_ASSERT(x)  {if(!(x))  return;}
#define VERSION ("5410_20150602")

inline void stat_trim_underscore(string& s)
{
	stat_trim(s, "_ ");
}

//---------------------------------------------------------------------
//  StatInfo的实现
//---------------------------------------------------------------------

//-------------------------------------------
// public methods
//-------------------------------------------
void StatInfo::add_info(std::string key, float value)
{
	stat_trim_underscore(key);

	MY_ASSERT(is_valid_key(key) && (value > 0) && (m_info.size() <= 30));

	ostringstream oss;
	oss << value;
	pair<Kv::iterator, bool> ret = m_info.insert(make_pair(key, oss.str()));
	MY_ASSERT(ret.second);
}

void StatInfo::add_info(string key, string value)
{
	stat_trim_underscore(key);

	MY_ASSERT(is_valid_key(key) && is_valid_value(value) && (m_info.size() <= 30));

	pair<Kv::iterator, bool> ret = m_info.insert(make_pair(key, value));
	MY_ASSERT(ret.second);
}

void StatInfo::add_op(OpCode op, std::string key1, std::string key2)
{
	stat_trim_underscore(key1);
	stat_trim_underscore(key2);

	MY_ASSERT(is_valid_op(op) && m_info.count(key1));

	switch (op) {
	case op_item_sum:
	case op_item_max:
	case op_item_set:
		MY_ASSERT(m_info.count(key2));
		key1 = key1 + "," + key2;
		break;
	default:
		MY_ASSERT(key2.empty());
		break;
	}

	m_ops[op].insert(key1);
	m_has_op = true;
}

void StatInfo::clear()
{
	m_info.clear();
	if (m_has_op) {
		for (int i = op_begin + 1; i != op_end; ++i) {
			m_ops[i].clear();
		}
		m_has_op = false;
	}
}

//-------------------------------------------
// private methods
//-------------------------------------------
void StatInfo::serialize(ostream& out) const
{
	for (Kv::const_iterator it = m_info.begin(); it != m_info.end(); ++it) {
		out << '\t' << it->first << '=' << it->second;
	}

	if (m_has_op) {
		// 不定义成static也是为了防止被越界写乱
		const char* op[op_end] = {
			"", /* op_begin */
			"sum:", "max:", "set:", "ucount:",
			"item:", "item_sum:", "item_max:", "item_set:",
			"sum_distr:", "max_distr:", "min_distr:",
            "set_distr:",
			"ip_distr:",
			/* op_end */
		};

		string vline;
		out << "\t_op_=";
		for (int i = op_begin + 1; i != op_end; ++i) {
			if (m_ops[i].size()) {
				out << vline;
				serialize_op(out, op[i], m_ops[i]);
				vline = "|";
			}
		}
	}
}

void StatInfo::serialize_op(ostream& out, const char *op, const OpKey& keys) const
{
	OpKey::const_iterator it = keys.begin();
    string vline;
	while (it != keys.end()) {
        out<< vline;
        out << op << *it;
        vline = "|";
        ++it;
	}
}

//---------------------------------------------------------------------
//  StatLogger的实现
//---------------------------------------------------------------------

//-------------------------------------------
// public methods
//-------------------------------------------
//

StatLogger::StatLogger()
{
	m_inited = 0;
    strcpy(m_version, VERSION);
}

StatLogger::StatLogger(int game_id, int32_t zone_id, int32_t svr_id, int32_t site_id, int isgame)
{

	// 创建保存统计数据的目录
	string path = "/opt/taomee/stat/data";//默认统计数据一律写入该目录下
	assert(stat_makedir(path + "/inbox"));
	path.copy(m_path, path.size());
	m_path[path.size()] = '\0';

	m_siteid = site_id;
	m_zoneid = zone_id;
	m_svrid  = svr_id;
	m_basic_fd  = -1;
	m_custom_fd = -1;
	m_basic_ts  = 0;
	m_custom_ts = 0;

	m_magic_num1 = sc_magic1;
	m_magic_num2 = sc_magic2;
	m_magic_num3 = sc_magic3;

    m_appid = game_id;
    m_isgame = isgame;
    if(game_id == 1 || game_id == 2 || game_id == 5 || game_id == 6 || game_id == 10 || game_id == 16 || game_id == 19) {
        m_need_multi = false;
    } else {
        m_need_multi = true;
    }
//	app_id.copy(m_appid, app_id.size());
//	m_appid[app_id.size()] = '\0';

	string ip = stat_get_ip_addr("eth1", 1);
	if (ip.empty()) {
		ip = stat_get_ip_addr("eth0", 1);
	}
	MY_ASSERT(ip.size() && (ip.size() < sc_ip_sz));
	ip.copy(m_hostip, ip.size());
	m_hostip[ip.size()] = '\0';

	init_errlog();
	m_inited = 1;
	m_chksum1 = m_chksum2 = calc_checksum();
}

StatLogger::~StatLogger()
{
	close(m_basic_fd);
	close(m_custom_fd);
	fini_errlog();
}

void StatLogger::init(int game_id, int32_t zone_id, int32_t svr_id, int32_t site_id, int isgame)
{
	// 创建保存统计数据的目录
	string path = "/opt/taomee/stat/data";//默认统计数据一律写入该目录下
	assert(stat_makedir(path + "/inbox"));
	path.copy(m_path, path.size());
	m_path[path.size()] = '\0';

	m_siteid = site_id;
	m_zoneid = zone_id;
	m_svrid  = svr_id;
	m_basic_fd = -1;
	m_custom_fd = -1;
	m_basic_ts  = 0;
	m_custom_ts = 0;

	m_magic_num1 = sc_magic1;
	m_magic_num2 = sc_magic2;
	m_magic_num3 = sc_magic3;

    m_appid = game_id;
    m_isgame = isgame;
    if(game_id == 1 || game_id == 2 || game_id == 5 || game_id == 6 || game_id == 10 || game_id == 16 || game_id == 19) {
        m_need_multi = false;
    } else {
        m_need_multi = true;
    }

	string ip = stat_get_ip_addr("eth1", 1);
	if (ip.empty()) {
		ip = stat_get_ip_addr("eth0", 1);
	}
	MY_ASSERT(ip.size() && (ip.size() < sc_ip_sz));
	ip.copy(m_hostip, ip.size());
	m_hostip[ip.size()] = '\0';

	init_errlog();
	m_inited = 1;
	m_chksum1 = m_chksum2 = calc_checksum();
}

/**
 * @brief reg_account_system 注册账号（用于账号系统）
 * @param acct_gid
 * @param mimi_id
 * @param tad
 * @param cli_ip
 * @param game_type
 * @param acct_type
 * @param acct_id
 */
void StatLogger::reg_account_system(int acct_gid, uint32_t mimi_id, std::string tad, uint32_t cli_ip, 
									GameType game_type, RegType acct_type, std::string acct_id)
{
	MY_ASSERT((m_appid == 169) && verify_checksum());
	check_valid_key(acct_id);
	check_valid_key(tad);

    time_t ts = time(0);
    string op("\t_op_=");
    ostringstream oss;
	string stid = "_regacct_", sstid = "_regacct_";

    set_user_info_reg(oss, stid, sstid, ts, mimi_id, op, tad, cli_ip, game_type, acct_type, acct_id, acct_gid, 0);

    if(op.size() > 6)
    {
        op.erase(op.size() - 1, 1);
        oss << op << '\n';
    }
    else
    {
        oss << '\n';
    }

	op="\t_op_=";
	set_user_info_reg(oss, stid, sstid, ts, mimi_id, op, tad, cli_ip, game_type, acct_type, acct_id, acct_gid, 1);

    if(op.size() > 6)
    {
        op.erase(op.size() - 1, 1);
        oss << op << '\n';
    }
    else
    {
        oss << '\n';
    }
    write_basic_log(oss.str(), ts);
}

/**
 * @brief login_account_system 登陆账号（用于账号系统）
 * @param acct_gid
 * @param mimi_id
 * @param tad
 * @param cli_ip
 * @param game_type
 * @param acct_type
 * @param acct_id
 */
void StatLogger::login_account_system(int acct_gid, uint32_t mimi_id, std::string tad, uint32_t cli_ip, 
									GameType game_type, LoginType acct_type, std::string acct_id)
{
	MY_ASSERT((m_appid == 169) && verify_checksum());
	check_valid_key(acct_id);
	check_valid_key(tad);

    time_t ts = time(0);
    string op("\t_op_=");
    ostringstream oss;
	string stid = "_loginacct_", sstid = "_loginacct_";

    set_user_info_login(oss, stid, sstid, ts, mimi_id, op, tad, cli_ip, game_type, acct_type, acct_id, acct_gid, 0);

    if(op.size() > 6)
    {
        op.erase(op.size() - 1, 1);
        oss << op << '\n';
    }
    else
    {
        oss << '\n';
    }

	op = "\t_op_=";
	set_user_info_login(oss, stid, sstid, ts, mimi_id, op, tad, cli_ip, game_type, acct_type, acct_id, acct_gid, 1);

    if(op.size() > 6)
    {
        op.erase(op.size() - 1, 1);
        oss << op << '\n';
    }
    else
    {
        oss << '\n';
    }
    write_basic_log(oss.str(), ts);
}

/**
 * @brief frozen_account_system 冻结账号（用于账号系统）
 * @param mimi_id
 */
void StatLogger::frozen_account_system(uint32_t mimi_id)
{
	string stid = "_frozenacct_", sstid = "_frozenacct_";
	frozen_account_process(mimi_id, stid, sstid);
}

/**
 * @brief activate_account_system 激活账号（用于账号系统）
 * @param mimi_id
 */
void StatLogger::activate_account_system(uint32_t mimi_id)
{
	string stid = "_activateacct_", sstid = "_activateacct_";
	frozen_account_process(mimi_id, stid, sstid);
}

void StatLogger::frozen_account_process(uint32_t mimi_id, std::string stid, std::string sstid)
{
	MY_ASSERT((m_appid == 169) && verify_checksum());

    time_t ts = time(0);
    ostringstream oss;
	char buf[100];
	string uid;
	sprintf(buf,"%u",mimi_id);
	uid = buf;

	set_basic_info(oss, stid, sstid, ts, uid);
	oss << '\n';

    write_basic_log(oss.str(), ts);
}

/**
 * @brief 自定义统计数据
 * @param acct_id,stat_id 
 */
void StatLogger::custom_log(std::string acct_id, uint32_t stat_id, float value)
{
	MY_ASSERT((stat_id != 0) && verify_checksum());

	time_t ts = time(0);
	ostringstream oss;
	char buf[100];
	string sstid;
	sprintf(buf,"%u",stat_id);
	sstid = buf;

	set_basic_info(oss, "msgid_", sstid, ts, acct_id);

	if(value == 0) 
	{
		oss << '\n';
		write_custom_log(oss.str(), ts);
	}
	else
	{
		oss << "\t_value_=" << value;
		oss << '\n';
		write_custom_log(oss.str(), ts);
	}
}

/**
 * @brief online_count 在线人数
 *
 * @param cnt
 */
void StatLogger::online_count(int cnt, std::string zone)
{
	MY_ASSERT((cnt >= 0) && verify_checksum());

	stat_trim_underscore(zone);

	time_t ts = time(0);
	ostringstream oss;
	// 当前在线人数
	set_basic_info(oss, "_olcnt_", "_olcnt_", ts, "-1");

    if(zone.size())
    {
    }
    else
    {
	    zone = "_all_";
    }
    oss << "\t_zone_=" << zone << "\t_olcnt_=" << cnt << "\t_op_=item_max:_zone_,_olcnt_\n";
	// TODO: 是否需要指定统计粒度为分钟？因为目前分钟数据不多，决定先在实时计算那边写死。

	write_basic_log(oss.str(), ts);
}

/**
 * @brief reg_account 注册米米号
 *
 * @param acct_id
 * @param cli_ip
 * @param ads_id
 * @param browser
 * @param device
 * @param os
 * @param resolution
 * @param network
 * @param isp
 */
void StatLogger::reg_account(std::string acct_id, uint32_t cli_ip, std::string ads_id, std::string browser,
                            std::string device, std::string os, std::string resolution,
                            std::string network, std::string isp)
{
    MY_ASSERT(verify_checksum());

    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }

    time_t ts = time(0);
    string op("\t_op_=");
    ostringstream oss;
    // 帐号新增
    set_basic_info(oss, "_newuid_", "_newuid_", ts, acct_id);

    set_device_info(oss, op, ads_id, browser, device, os, resolution, network, isp);

    if(cli_ip != 0)
    {
        oss << "\t_cip_=" << cli_ip;
        op += "ip_distr:_cip_|";
    }

    if(op.size() > 6)
    {
        op.erase(op.size() - 1, 1);
        oss << op << '\n';
    }
    else
    {
        oss << '\n';
    }

    write_basic_log(oss.str(), ts);
}

void StatLogger::reg_role(string acct_id, string player_id, string race, uint32_t cli_ip, string ads_id,
						  string browser, string device, string os, string resolution, string network, string isp)
{
	MY_ASSERT(verify_checksum());
    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }

	time_t ts = time(0);
	string op("\t_op_=");
	ostringstream oss;
	// 帐号新增
	set_basic_info(oss, "_newac_", "_newac_", ts, acct_id);  //基础接口生成的日志
	set_device_info(oss, op, ads_id, browser, device, os, resolution, network, isp); 


    if(cli_ip != 0)
    {
	    oss << "\t_cip_=" << cli_ip;
        op += "ip_distr:_cip_|";
    }

    if(op.size() > 6)
    {
	    op.erase(op.size() - 1, 1);
	    oss << op << '\n';
    }
    else
    {
	    oss << '\n';
    }
	// 角色新增
	if (player_id.size()) {
		MY_ASSERT(is_valid_playerid(player_id));
		set_basic_info(oss, "_newpl_", "_newpl_", ts, acct_id, player_id);
		oss << '\n';
	}
	// 职业新增
	if (race.size()) {
		stat_trim_underscore(race);
		MY_ASSERT(is_valid_race(race));
		set_basic_info(oss, "_newrace_", race, ts, acct_id);
		oss << '\n';
	}

	write_basic_log(oss.str(), ts);
}

void StatLogger::verify_passwd(string acct_id, uint32_t cli_ip,
						string ads_id, string browser, string device, string os, string resolution, string network, string isp)
{
	MY_ASSERT(verify_checksum());
    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }

	time_t ts = time(0);
	//string op("\t_op_=ip_distr:_cip_|max_distr:_lv_|item:_vip_;"); // 暂时不用max_distr
	string op("\t_op_=");
	ostringstream oss;
	// 帐号登录
	set_basic_info(oss, "_veripass_", "_veripass_", ts, acct_id);
	set_device_info(oss, op, ads_id, browser, device, os, resolution, network, isp);

    if(cli_ip != 0)
    {
	    oss << "\t_cip_=" << cli_ip;
        op += "ip_distr:_cip_|";
    }


    	if(op.size() > 6)
    	{
		op.erase(op.size() - 1, 1);
		oss << op << '\n';
   	 }
    	else
    	{
	    oss << '\n';
    	}

	write_basic_log(oss.str(), ts);
}


void StatLogger::login_online(string acct_id, string player_id, string race, bool isvip, int lv, uint32_t cli_ip,
						string ads_id, string zone, string browser, string device, string os, string resolution, string network, string isp)
{
	MY_ASSERT(is_valid_lv(lv)  && verify_checksum());
    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }


	time_t ts = time(0);
	//string op("\t_op_=ip_distr:_cip_|max_distr:_lv_|item:_vip_;"); // 暂时不用max_distr
	string op("\t_op_=item:_vip_|");
    if(lv != 0)
    {
        op += "item:_lv_|";
    }
	ostringstream oss;
	// 帐号登录
	set_basic_info(oss, "_lgac_", "_lgac_", ts, acct_id);
	oss << "\t_vip_=" << isvip << "\t_lv_=" << lv;
	set_device_info(oss, op, ads_id, browser, device, os, resolution, network, isp);

    	if(cli_ip != 0)
        {
            oss << "\t_cip_=" << cli_ip;
        	op += "ip_distr:_cip_|";
    	}

	if(zone.size())
        {//统计各区的登录人数
	        stat_trim_underscore(zone);
		oss << "\t_zone_="<<zone;
		op += "item:_zone_|";
        }

    	if(op.size() > 6)
    	{
		op.erase(op.size() - 1, 1);
		oss << op << '\n';
   	     }
    	else
    	{
	    oss << '\n';
    	}


	// 角色登录
	if (player_id.size()) {
		MY_ASSERT(is_valid_playerid(player_id));
		set_basic_info(oss, "_lgpl_", "_lgpl_", ts, acct_id, player_id);
		oss << '\n';
	}
	// 职业登录
	if (race.size()) {
		stat_trim_underscore(race);
		MY_ASSERT(is_valid_race(race));
		set_basic_info(oss, "_lgrace_", race, ts, acct_id);
		//oss << "\t_vip_=" << isvip << "\t_lv_=" << lv << "\t_op_=max_distr:_lv_|item:_vip_\n";
		oss << "\t_vip_=" << isvip << "\t_lv_=" << lv << "\t_op_=item:_vip_|item:_lv_\n"; // 暂时不用max_distr
		//oss << "\t_vip_=" << isvip << "\t_lv_=" << lv << "\t_cip_=" << cli_ip;

	}

	write_basic_log(oss.str(), ts);
}

void StatLogger::start_device(string device_id)
{

    if(device_id.size() == 0)
    {
        device_id = "-1";
    }
	time_t ts = time(0);
	ostringstream oss;
	// 帐号登录
	set_basic_info(oss, "_startdev_", "_startdev_", ts, device_id);
	oss << '\n';
	write_basic_log(oss.str(), ts);
}

void StatLogger::logout(string acct_id, bool isvip, int lv, int oltime)
{
	MY_ASSERT(is_valid_lv(lv) && is_valid_oltm(oltime) && verify_checksum());

    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }

	time_t ts = time(0);
	ostringstream oss;
	// 帐号登出
	set_basic_info(oss, "_logout_", "_logout_", ts, acct_id);
	oss << "\t_vip_=" << isvip << "\t_lv_=" << lv << "\t_oltm_=" << oltime
		<< "\t_intv_=" << logout_time_interval(oltime) << "\t_op_=sum:_oltm_|item:_intv_\n";

	write_basic_log(oss.str(), ts);
}

/**
 * @brief 对战统计-适用于极战联盟或有对战机制的游
 *
 * @param acct_id: 用户米米号
 * @param iswin: 对战是否胜利，胜利：1 失败：0
 * @param combat_time: 战斗时长，单位秒
 */
void StatLogger::combat(string acct_id, bool iswin, int combat_time)
{
	MY_ASSERT(verify_checksum());

    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }

	time_t ts = time(0);
	ostringstream oss;
	// 帐号登出
	set_basic_info(oss, "_combat_", "_combat_", ts, acct_id);
	oss << "\t_win_=" << iswin << "\t_cbtime_=" << combat_time << "\n";

	write_basic_log(oss.str(), ts);
}

void StatLogger::accumulate_online_time(string acct_id, string race, int oltime)
{
	MY_ASSERT(is_valid_oltm(oltime) && verify_checksum());
    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }

	time_t ts = time(0);
	ostringstream oss;
	// 帐号在线时长
	set_basic_info(oss, "_acoltm_", "_acoltm_", ts, acct_id);
	oss << "\t_oltm_=" << oltime << "\t_op_=sum:_oltm_\n";
	// 职业在线时长
	if (race.size()) {
		stat_trim_underscore(race);
		MY_ASSERT(is_valid_race(race));
		set_basic_info(oss, "_raceoltm_", race, ts, acct_id);
		oss << "\t_oltm_=" << oltime << "\t_op_=sum:_oltm_\n";
	}

	write_basic_log(oss.str(), ts);
}

void StatLogger::level_up(string acct_id, string race, int lv)
{
	MY_ASSERT(is_valid_lv(lv) && verify_checksum());
    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }

	time_t ts = time(0);
	ostringstream oss;
	// 帐号等级变化
	set_basic_info(oss, "_aclvup_", "_aclvup_", ts, acct_id);
	oss << "\t_lv_=" << lv << "\t_op_=set_distr:_lv_" << '\n';
	// 职业等级变化
	if (race.size()) {
		stat_trim_underscore(race);
		MY_ASSERT(is_valid_race(race));
		set_basic_info(oss, "_racelvup_", race, ts, acct_id);
		oss << "\t_lv_=" << lv << "\t_op_=set_distr:_lv_" << '\n';
	}

	write_basic_log(oss.str(), ts);
}

void StatLogger::obtain_spirit(std::string acct_id, bool isvip, int lv, std::string spirit)
{
    MY_ASSERT(is_valid_lv(lv) && verify_checksum());
    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }

    time_t ts = time(0);
    ostringstream oss;

    stat_trim_underscore(spirit);
    set_basic_info(oss, "_obtainspirit_", "_obtainspirit_", ts, acct_id);
    oss << "\t_lv_=" << lv << "\t_vip_=" << isvip << "\t_spirit_=" << spirit << '\n';

    write_basic_log(oss.str(), ts);
}

void StatLogger::lose_spirit(std::string acct_id, bool isvip, int lv, std::string spirit)
{
    MY_ASSERT(is_valid_lv(lv) && verify_checksum());
    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }

    time_t ts = time(0);
    ostringstream oss;

    stat_trim_underscore(spirit);
    set_basic_info(oss, "_losespirit_", "_losespirit_", ts, acct_id);
    oss << "\t_lv_=" << lv << "\t_vip_=" << isvip << "\t_spirit_=" << spirit << '\n';

    write_basic_log(oss.str(), ts);
}

void StatLogger::pay_item(string acct_id, bool isvip, float pay_amount,
                        CurrencyType currency, string outcome, int outcnt)
{
	MY_ASSERT( (pay_amount > 0) && is_valid_currency(currency) && (outcnt > 0) 
			&& is_valid_common_utf8_parm(outcome, 0) 
			&& verify_checksum());
	time_t ts = time(0);
    //这里可以统计出付费总额(通常单个游戏，不应该出现多种货币单位)、VIP和非VIP用户付费总额、各种货币单位付费总额
	string op = "\t_op_=sum:_amt_";
	ostringstream oss;
	// 帐号付费 //不包含赠送渠道
    set_basic_info(oss, "_mibiitem_", "_mibiitem_", ts, acct_id);
    oss << "\t_vip_=" << isvip << "\t_amt_=" << pay_amount << "\t_ccy_=" << currency << op << '\n';
	write_basic_log(oss.str(), ts);
}

void StatLogger::pay(string acct_id, bool isvip, float pay_amount,
						CurrencyType currency, PayReason pay_reason, string outcome, int outcnt,
						string pay_channel)
{
	MY_ASSERT( (pay_amount > 0) && is_valid_currency(currency) && is_valid_payreason(pay_reason)
			&& (outcnt > 0) && (outcome.size() || (pay_reason != pay_buy))
			&& is_valid_common_utf8_parm(outcome, 0) && is_valid_common_utf8_parm(pay_channel, 1)
			&& verify_checksum());

    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }

	string reason;
	switch (pay_reason) {
	case pay_charge:
		reason   = "_buycoins_";
		break;
	case pay_vip:
		reason   = "_vipmonth_";
		break;
	case pay_buy:
		reason   = "_buyitem_";
		break;
    	case pay_free:
        	reason = "_costfree_";
	default:
		break;
	}

	time_t ts = time(0);
    //这里可以统计出付费总额(通常单个游戏，不应该出现多种货币单位)、VIP和非VIP用户付费总额、各种货币单位付费总额
	string op = "\t_op_=sum:_amt_|item_sum:_vip_,_amt_|item_sum:_paychannel_,_amt_|item_sum:_ccy_,_amt_|item:_paychannel_";
	ostringstream oss;
	// 帐号付费 //不包含赠送渠道
    if(pay_reason != pay_free)
    {
        set_basic_info(oss, "_acpay_", "_acpay_", ts, acct_id);
	    oss << "\t_vip_=" << isvip << "\t_amt_=" << pay_amount << "\t_ccy_=" << currency << "\t_paychannel_=" << pay_channel
		    << op << '\n';
    }

	set_basic_info(oss, "_acpay_", reason , ts, acct_id);
	oss << "\t_vip_=" << isvip << "\t_amt_=" << pay_amount << "\t_ccy_=" << currency << "\t_paychannel_=" << pay_channel
		<< op << '\n';


	write_basic_log(oss.str(), ts);

	switch (pay_reason)
    {
	case pay_charge:
        //需要统计金币产出
		do_obtain_golds(acct_id,  "_userbuy_", outcnt);
		break;
    case pay_vip:
        //需要统计各个包月时长
        do_buy_vip(acct_id, pay_amount, outcnt);
        break;
	case pay_buy:
        //需要统计道具产出
		do_buy_item(acct_id, isvip, 0,  pay_amount, "_mibiitem_", outcome, outcnt);
		break;
	default:
		break;
	}
}

void StatLogger::unsubscribe(string acct_id, UnsubscribeChannel uc)
{
    MY_ASSERT(verify_checksum());
    if(acct_id.size() == 0)
    {
        return;
    }

    time_t ts = time(0);
	ostringstream oss;
    string op = "\t_op_=item:_uc_";
    set_basic_info(oss, "_unsub_", "_unsub_", ts, acct_id);
    oss << "\t_uc_=" << uc << op << "\n";
    write_basic_log(oss.str(), ts);
}

void StatLogger::cancel_acct(string acct_id, string channel)
{
    MY_ASSERT(verify_checksum());
    if(acct_id.size() == 0)
    {
        return;
    }

    time_t ts = time(0);
	ostringstream oss;
    string op = "\t_op_=item:_cac_";
    set_basic_info(oss, "_ccacct_", "_ccacct_", ts, acct_id);
    oss << "\t_cac_=" << channel << op << "\n";
    write_basic_log(oss.str(), ts);

}

void StatLogger::obtain_golds(string acct_id, int amt)
{

	MY_ASSERT( (amt > 0) && (amt <= 1000000000) && verify_checksum());

    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }
	do_obtain_golds(acct_id, "_systemsend_", m_need_multi?amt*100:amt);
}


void StatLogger::use_golds_buyitem(std::string acct_id, bool is_vip, float amt, int lv)
{

    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }


	time_t ts = time(0);
	ostringstream oss;
	// 帐号使用游戏金币
	set_basic_info(oss, "_usegold_", "_usegold_", ts, acct_id);
	oss << "\t_golds_=" << amt << "\t_op_=sum:_golds_\n";

	set_basic_info(oss, "_usegold_", "_buyitem_", ts, acct_id);
	oss << "\t_golds_=" << amt << "\t_isvip_=" << is_vip << "\t_lv_=" << lv << "\t_op_=item:_isvip_|item_sum:_lv_,_golds_|sum:_golds_\n";

	write_basic_log(oss.str(), ts);

}


//金币消耗
void StatLogger::use_golds(std::string acct_id, bool is_vip, std::string reason, float amt, int lv)
{
	stat_trim_underscore(reason);

	MY_ASSERT((amt > 0) && (amt <= 100000)
			&& is_valid_common_utf8_parm(reason)
			&& is_valid_lv(lv) && verify_checksum());


    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }

    if(reason.size() == 0)
    {
        reason = "unknown";
    }

	time_t ts = time(0);
	ostringstream oss;
	// 帐号使用游戏金币
	set_basic_info(oss, "_usegold_", "_usegold_", ts, acct_id);
	oss << "\t_golds_=" << (m_need_multi?amt*100:amt) << "\t_op_=sum:_golds_\n";

	set_basic_info(oss, "_usegold_", reason, ts, acct_id);
	oss << "\t_golds_=" << (m_need_multi?amt*100:amt) << "\t_isvip_=" << is_vip << "\t_lv_=" << lv << "\t_op_=item:_isvip_|item_sum:_lv_,_golds_|sum:_golds_\n";

	write_basic_log(oss.str(), ts);
}

//游戏内一级货币购买道具
void StatLogger::buy_item(string acct_id,  bool isvip, int lv,  float pay_amount, string outcome, int outcnt)
{
    //printf("check param\n");
	MY_ASSERT(is_valid_lv(lv) && (pay_amount > 0)
			&& is_valid_common_utf8_parm(outcome) && (outcnt > 0)
			&& verify_checksum());
    //printf("check param ok\n");

    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }
    if(outcome.size() == 0)
    {
        outcome = "-1";
    }

	do_buy_item(acct_id, isvip, lv,  m_need_multi?pay_amount*100:pay_amount, "_coinsbuyitem_", outcome, outcnt);
    use_golds_buyitem(acct_id, isvip, m_need_multi?pay_amount*100:pay_amount,lv);
}

void StatLogger::accept_task(TaskType type, std::string acct_id, std::string task_name, int lv)
{
	stat_trim_underscore(task_name);

	MY_ASSERT(is_valid_tasktype(type)
			&& is_valid_common_utf8_parm(task_name) && verify_checksum());

    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }

    if(task_name.size() == 0)
    {
        task_name = "unknown";
    }

	time_t ts = time(0);
	ostringstream oss;
	// 帐号接取任务
	set_basic_info(oss, get_task_stid(type, 0), task_name, ts, acct_id);
    if(lv != 0)
    {
        oss << "\t_lv_=" << lv << "\t_op_=item:_lv_\n";
    }
    else
    {
        oss << "\n";
    }
	write_basic_log(oss.str(), ts);
}

void StatLogger::finish_task(TaskType type, std::string acct_id, std::string task_name, int lv)
{

	stat_trim_underscore(task_name);

	MY_ASSERT(is_valid_tasktype(type) && is_valid_common_utf8_parm(task_name)
			&& verify_checksum());

    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }

    if(task_name.size() == 0)
    {
        task_name = "unknown";
    }

	time_t ts = time(0);
	ostringstream oss;
	// 帐号完成任务
	set_basic_info(oss, get_task_stid(type, 1), task_name, ts, acct_id);
    if(lv != 0)
    {
        oss << "\t_lv_=" << lv << "\t_op_=item:_lv_\n";
    }
    else
    {
        oss << "\n";
    }
	write_basic_log(oss.str(), ts);
}

void StatLogger::abort_task(TaskType type, std::string acct_id, std::string task_name, int lv)
{
	stat_trim_underscore(task_name);

	MY_ASSERT(is_valid_tasktype(type)
			&& is_valid_common_utf8_parm(task_name) && verify_checksum());

    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }

    if(task_name.size() == 0)
    {
        task_name = "unknown";
    }

	time_t ts = time(0);
	ostringstream oss;
	// 帐号放弃任务
	set_basic_info(oss, get_task_stid(type, 2), task_name, ts, acct_id);
    if(lv != 0)
    {
        oss << "\t_lv_=" << lv << "\t_op_=item:_lv_\n";
    }
    else
    {
        oss << "\n";
    }
	write_basic_log(oss.str(), ts);
}

void StatLogger::new_trans(NewTransStep step, std::string acct_id)
{
	MY_ASSERT(is_valid_newtransstep(step));
    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }
	time_t ts = time(0);
	ostringstream oss;
	// 注册转化率
	set_basic_info(oss, "_newtrans_", get_new_trans_step(step), ts, acct_id);
    oss << "\n";
	write_basic_log(oss.str(), ts);
}

void StatLogger::log(string stat_name, string sub_stat_name, string acct_id, string player_id, const StatInfo& info)
{
	stat_trim_underscore(stat_name);
	stat_trim_underscore(sub_stat_name);

	MY_ASSERT(is_valid_common_utf8_parm(stat_name) && is_valid_common_utf8_parm(sub_stat_name) && verify_checksum());

    if(acct_id.size() == 0)
    {
        acct_id = "-1";
    }
    if(stat_name.size() == 0)
    {
        stat_name = "unknown";
    }

    if(sub_stat_name.size() == 0)
    {
        sub_stat_name = "unknown";
    }
	time_t ts = time(0);
	ostringstream oss;
	set_basic_info(oss, stat_name, sub_stat_name, ts, acct_id, player_id);
	oss << info << '\n';

	write_custom_log(oss.str(), ts);
}

//-------------------------------------------
// private methods
//-------------------------------------------
void StatLogger::do_obtain_golds(const string& acct_id, const string& reason, int amt)
{
	time_t ts = time(0);
	ostringstream oss;
	// 帐号获得游戏金币
	set_basic_info(oss, "_getgold_", reason, ts, acct_id);
	oss << "\t_golds_=" << amt << "\t_op_=sum:_golds_\n";

	write_basic_log(oss.str(), ts);
}

//统计包月时长的人数人次以及金额
void StatLogger::do_buy_vip(const string & acct_id, float pay_amount, int amt)
{

    time_t ts = time(0);
    ostringstream oss;
    set_basic_info(oss, "_buyvip_", "_buyvip_", ts, acct_id);
    oss<< "\t_payamt_="<< pay_amount << "\t_amt_=" << amt << "\t_op_=item:_amt_|item_sum:_amt_,_payamt_\n";
    write_basic_log(oss.str(), ts);

}

void StatLogger::do_buy_item(const string& acct_id, bool isvip, int lv, float pay_amount, const string& pay_type, const string& outcome, int outcnt)
{

	time_t ts = time(0);
	ostringstream oss;

    set_basic_info(oss, "_buyitem_", pay_type, ts, acct_id);
    string op("\t_op_=sum:_golds_");
    if(lv != 0)
    {
	    op += "|item:_lv_";
    }

    oss << "\t_isvip_="<< isvip << "\t_item_="<< outcome << "\t_itmcnt_="<<outcnt << "\t_golds_="<<pay_amount<<  "\t_lv_=" << lv << op << "\n";


   // string stid;
   // if(isvip == true)
   // {
   //     stid = "_VIP用户购买道具_";
   // }
   // else
   // {
   //     stid = "_非VIP用户购买道具_";
   // }
	// 帐号购买道具
//	string op_v("\t_op_=sum:_golds_|item:_item_|item_sum:_item_,_itemcnt_|item_sum:_item_,_golds_");
//	set_basic_info(oss, stid, pay_type, ts, acct_id);
//	oss << "\t_golds_=" << pay_amount << "\t_item_=" << outcome << "\t_itemcnt_=" << outcnt << op_v << '\n';
//
//	string op("\t_op_=sum:_golds_|item:_item_|item_sum:_item_,_itemcnt_|item_sum:_item_,_golds_|item:_lv_");
//   	 set_basic_info(oss, "_购买道具_", pay_type, ts, acct_id);
//	oss << "\t_lv_=" << lv << "\t_golds_=" << pay_amount
//        << "\t_item_=" << outcome << "\t_itemcnt_=" << outcnt
//		<< op << '\n';
//
	write_basic_log(oss.str(), ts);
}

void StatLogger::monitor_dbserver(int gameid, const char* type) {
	time_t ts = time(0);
	ostringstream oss;

    set_basic_info(oss, "_monitor_", "_notutf8-dbserver_", ts, "-1");
    string op("\t_op_=item:game|item:");

    //_op_=item:game|item:gameid
    oss << "\tgame=" << gameid << "\t" << gameid << "=" << type << op << gameid << "\n";
	write_basic_log(oss.str(), ts);
}


const char* StatLogger::logout_time_interval(int tm) const
{
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

void StatLogger::
set_basic_info(ostringstream& oss, const string& statname, const string& sub_statname,
								time_t ts, const string& acct_id, const string& player_id) const
{
	oss << "_hip_=" << m_hostip << "\t_stid_=" << statname << "\t_sstid_=" << sub_statname
		<< "\t_gid_=" << m_appid << "\t_zid_=" << m_zoneid
		<< "\t_sid_=" << m_svrid << "\t_pid_=" << m_siteid
        << "\t_ts_=" << ts << "\t_acid_=" << (acct_id.size() ? acct_id: "-1")
		<< "\t_plid_=" << (player_id.size() ? player_id : "-1");
}


void StatLogger::set_device_info(ostringstream& oss, string& op, const string& ads_id, const string& browser,
									const string& device, const string& os, const string& resolution,
									const string& network, const string& isp) const
{
	if (ads_id.size()) {
		// 广告字段长度暂时不做长度限制
		// 一般都是英文字符，无需判断utf8
    MY_ASSERT(value_no_invalid_chars(ads_id));
		oss << "\t_ad_=" << ads_id;
		//op += "|item:_ad_,";
	}
	if (browser.size()) {
		MY_ASSERT(is_valid_common_utf8_parm(browser));
		oss << "\t_ie_=" << browser;
		op += "item:_ie_|";
	}
	if (device.size()) {
		MY_ASSERT(is_valid_common_utf8_parm(device));
		oss << "\t_dev_=" << device;
		op += "item:_dev_|";
	}
	if (os.size()) {
		// 一般都是英文字符，无需判断utf8
		MY_ASSERT(is_valid_common_parm(os));
		oss << "\t_os_=" << os;
		op += "item:_os_|";
	}
	if (resolution.size()) {
		// 一般都是数字，无需判断utf8
		MY_ASSERT(is_valid_common_parm(resolution));
		oss << "\t_res_=" << resolution;
		op += "item:_res_|";
	}
	if (network.size()) {
		// 一般都是英文字符，无需判断utf8
		MY_ASSERT(is_valid_common_parm(network));
		oss << "\t_net_=" << network;
		op += "item:_net_|";
	}
	if (isp.size()) {
		MY_ASSERT(is_valid_common_utf8_parm(isp));
		oss << "\t_isp_=" << isp;
		op += "item:_isp_|";
	}
}

void StatLogger::set_user_info_reg(ostringstream& oss, const string& statname, const string& sub_statname,
			time_t ts, const uint32_t& mimi_id, string& op, const string& tad, const uint32_t& cli_ip, 
			GameType game_type, RegType reg_type, const string& acct_id, const int& acct_gid, int flag) const
{
	int gameid = -1;
	if(flag == 0) gameid = m_appid;
	else gameid = acct_gid;
	oss << "_hip_=" << m_hostip << "\t_stid_=" << statname << "\t_sstid_=" << sub_statname
		<< "\t_gid_=" << gameid << "\t_zid_=" << m_zoneid
		<< "\t_sid_=" << m_svrid << "\t_pid_=" << m_siteid
        << "\t_ts_=" << ts << "\t_acid_=" << mimi_id;

	string gtype, rtype;
	switch (game_type) {
	case gtype_web:
		gtype = "webgame";
		break;
	case gtype_mobile:
		gtype = "mobilegame";
		break;
	default:
		break;
	}
	switch (reg_type) {
	case rtype_mail:
		rtype = "mail";
		break;
	case rtype_apple:
		rtype = "appleid";
		break;
	case rtype_b03:
		rtype = "b03mobile";
		break;
	case rtype_android:
		rtype = "android";
		break;
	case rtype_mac:
		rtype = "mac";
		break;
	case rtype_username:
		rtype = "userdefine";
		break;
	case rtype_open:
		rtype = "openudid";
		break;
	case rtype_mobile:
		rtype = "mobile";
		break;
	case rtype_token:
		rtype = "tokenid";
		break;
	case rtype_mimi:
		rtype = "mimi";
		break;
	default:
		break;
	}
	if(tad.size()) {
		oss << "\t_tad_=" << tad;
	}
	if(gtype.size() && flag == 0) {
		oss << "\t_gmtype_=" << gtype;
		op += "item:_gmtype_|";
	}
	if(rtype.size()) {
		oss << "\t_actype_=" << rtype;
		op += "item:_actype_|";
	}
	if(acct_id.size()) {
		oss << "\t_acctid_=" << acct_id;
	}
	if(flag == 0)
	{
		oss << "\t_acgid_=" << acct_gid;
		op += "item:_acgid_|";
	}
	oss << "\t_cip_=" << cli_ip;
}

void StatLogger::set_user_info_login(ostringstream& oss, const string& statname, const string& sub_statname,
			time_t ts, const uint32_t& mimi_id, string& op, const string& tad, const uint32_t& cli_ip, 
			GameType game_type, LoginType login_type, const string& acct_id, const int& acct_gid, int flag) const
{
	int gameid = -1;
	if(flag == 0) gameid = m_appid;
	else gameid = acct_gid;
	oss << "_hip_=" << m_hostip << "\t_stid_=" << statname << "\t_sstid_=" << sub_statname
		<< "\t_gid_=" << gameid << "\t_zid_=" << m_zoneid
		<< "\t_sid_=" << m_svrid << "\t_pid_=" << m_siteid
        << "\t_ts_=" << ts << "\t_acid_=" << mimi_id;

	string gtype, ltype;
	switch (game_type) {
	case gtype_web:
		gtype = "webgame";
		break;
	case gtype_mobile:
		gtype = "mobilegame";
		break;
	default:
		break;
	}
	switch (login_type) {
	case ltype_mail:
		ltype = "mail";
		break;
	case ltype_apple:
		ltype = "appleid";
		break;
	case ltype_b03:
		ltype = "b03mobile";
		break;
	case ltype_android:
		ltype = "android";
		break;
	case ltype_mac:
		ltype = "mac";
		break;
	case ltype_username:
		ltype = "userdefine";
		break;
	case ltype_open:
		ltype = "openudid";
		break;
	case ltype_mobile:
		ltype = "mobile";
		break;
	case ltype_token:
		ltype = "tokenid";
		break;
	case ltype_unknown:
		ltype = "unknown";
		break;
	case ltype_mimi:
		ltype = "mimi";
		break;
	default:
		break;
	}
	if(tad.size()) {
		oss << "\t_tad_=" << tad;
	}
	if(gtype.size() && flag == 0) {
		oss << "\t_gmtype_=" << gtype;
		op += "item:_gmtype_|";
	}
	if(ltype.size()) {
		oss << "\t_actype_=" << ltype;
		op += "item:_actype_|";
	}
	if(acct_id.size()) {
		oss << "\t_acctid_=" << acct_id;
	}
	if(flag == 0)
	{
		oss << "\t_acgid_=" << acct_gid;
		op += "item:_acgid_|";
	}
	oss << "\t_cip_=" << cli_ip;
}

void StatLogger::write_basic_log(const std::string& s, time_t ts)
{
	if(m_inited == 0)
	{
		errlog(string("object not inited"));
		return;
	}

	// 每20秒时间切换文件，跨天也切换文件
	if ((ts < m_basic_ts) || ((ts - m_basic_ts) > 19) || (m_basic_fd == -1) || ((m_basic_ts + 8*60*60)/86400 != (ts + 8*60*60)/86400))
    {
		close(m_basic_fd);
		m_basic_ts = ts - (ts % 20);
		ostringstream oss;
		if(m_isgame)
		{//游戏后台落的日志
			oss << m_path << "/inbox/" << m_appid << "_game_basic_" << m_basic_ts;
		}
		else
		{//平台组落得日志
			oss << m_path << "/inbox/" << m_appid << "_account_basic_" << m_basic_ts;
		}
			m_basic_fd = open(oss.str().c_str(), O_WRONLY | O_APPEND | O_CREAT, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
		if (m_basic_fd == -1) {
			errlog(string("[write_basic_log]open(): ") + strerror(errno));
		}
		fchmod(m_basic_fd, 0777);
		m_chksum1 = m_chksum2 = calc_checksum();
	}

	if (write(m_basic_fd, s.c_str(), s.size()) == -1) {
		errlog(string("[write_basic_log]write(): ") + strerror(errno));
        close(m_basic_fd);
		m_basic_fd = -1;
		m_chksum1 = m_chksum2 = calc_checksum();
	}
}

void StatLogger::write_custom_log(const std::string& s, time_t ts)
{
	if(m_inited == 0)
	{
		errlog(string("object not inited"));
		return;
	}

	// 每20秒时间切换文件，跨天也切换文件
	if ((ts < m_custom_ts) || ((ts - m_custom_ts) > 19) || (m_custom_fd == -1) ||  ((m_custom_ts + 8*60*60)/86400 != (ts + 8*60*60)/86400)) {
		close(m_custom_fd);
		m_custom_ts = ts - (ts % 20);
		ostringstream oss;
		if(m_isgame)
		{
			oss << m_path << "/inbox/" << m_appid << "_game_custom_" << m_custom_ts;
		}
		else
		{
			oss << m_path << "/inbox/" << m_appid << "_account_custom_" << m_custom_ts;
		}

		m_custom_fd = open(oss.str().c_str(), O_WRONLY | O_APPEND | O_CREAT,
							S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
		if (m_custom_fd == -1) {
			errlog(string("[write_custom_log]open(): ") + strerror(errno));
		}
		fchmod(m_custom_fd, 0777);
		m_chksum1 = m_chksum2 = calc_checksum();
	}

	if (write(m_custom_fd, s.c_str(), s.size()) == -1) {
		errlog(string("[write_custom_log]write(): ") + strerror(errno));
        close(m_custom_fd);
		m_custom_fd = -1;
		m_chksum1 = m_chksum2 = calc_checksum();
	}
}

void StatLogger::init_errlog()
{
	// 创建保存程序运行日志文件的目录
	string logpath = string(m_path) + "/log";
	bool ret = stat_makedir(logpath);
	MY_ASSERT(ret);
	// 为了尽可能保证在各种异常情况下仍然可以正常写错误日志（比如fd耗尽），
	// 所以创建StatLogger时就同时打开日志文件，所有日志都写到这一个文件里，
	// 不做任何形式的日志切换/轮转。
	// 统计客户端必须每天把这个文件里的内容清空一下，避免空间写满。
	logpath += "/error.log";
	m_of.open(logpath.c_str(), ios_base::app);
	MY_ASSERT(m_of && m_of.is_open());

	errlog("StatLogger Constructed.");
}

void StatLogger::errlog(const std::string& msg)
{
	char buf[64];
	time_t ts = time(0);
	ctime_r(&ts, buf);
	buf[strlen(buf) - 1] = '\0';
	m_of << '[' << buf << "][" << getpid() << ']' << msg << endl;
}

void StatLogger::fini_errlog()
{
	errlog("StatLogger Destructed.");
}

int StatLogger::calc_checksum() const
{
	int nleft         = reinterpret_cast<const uint8_t*>(&m_chksum2) - reinterpret_cast<const uint8_t*>(&m_siteid);
	int chksum        = 0;
	const uint16_t* w = reinterpret_cast<const uint16_t*>(&m_siteid);

	while (nleft > 1)  {
		chksum += *(w++);
		nleft  -= sizeof(*w);
	}
	if (nleft) {
		chksum += *reinterpret_cast<const uint8_t*>(w);
	}

	return chksum;
}

bool StatLogger::verify_checksum() const
{
	return ((m_chksum1 == m_chksum2) && (m_magic_num1 == sc_magic1)
			&& (m_magic_num2 == sc_magic2) && (m_magic_num3 == sc_magic3)
			&& (m_chksum1 == calc_checksum()));
}

bool StatLogger::is_valid_appid(const string& appid) const
{
	if ((appid.size() < sc_appid_sz) && (appid.size() > sc_appid_minsz)) {
		for (string::size_type i = 0; i != appid.size(); ++i) {
			if (((appid[i] >= '0') && (appid[i] <= '9')) || ((appid[i] >= 'A') && (appid[i] <= 'F'))) {
				continue;
			}
			return false;
		}
		return true;
	}

	return false;
}

void StatLogger::check_valid_key(string& key) const
{
	size_t index = 0;
	index = key.find_first_of("=\t");
	while(index != std::string::npos)
	{
		key[index] = '@';
		index = key.find_first_of("=\t", index+1);
	}
}

