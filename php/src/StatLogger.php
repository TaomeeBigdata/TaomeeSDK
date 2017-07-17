<?php
date_default_timezone_set('Asia/Shanghai');

define("VERSION", "5410_20150602");

class StatCommon {
    // 去除字符串s头尾的charlist字符
    public static function stat_trim_underscore(&$s) {
        $s = trim($s, "_ \t\n\r\0\x0B");
    }

    // 根据delim分解字符串
    public static function stat_split($s, $delim) {
        return split($delim, $s);
    }

    // 判断给定的字符串是否utf8编码
    public static function stat_is_utf8($str) {
        $len = strlen($str);
        for($i = 0; $i < $len; $i++) {
            $c = ord($str[$i]);
            if ($c > 128) {
                if (($c > 247))
                    return false;
                elseif ($c > 239)
                    $bytes = 4;
                elseif ($c > 223)
                    $bytes = 3;
                elseif ($c > 191)
                    $bytes = 2;
                else
                    return false;

                if (($i + $bytes) > $len)
                    return false;

                while ($bytes > 1) {
                    $i++;
                    $b = ord($str[$i]);
                    if ($b < 128 || $b > 191)
                        return false;
                    $bytes--;
                }
            }
        }
        return true;
    }

    public static function size_between($str, $min, $max) {
        $l = strlen($str);
        return ($min <= $l && $l <= $max);
    }

    public static function value_no_invalid_chars($str) {
        return strpos($str, "=") === false &&
            strpos($str, "|") === false &&
            strpos($str, "\t") === false;
    }

    public static function stat_makedir($path) {
        if(strlen($path) == 0)  return false;

        $subdirs = StatCommon::stat_split($path, "/");
        if(strlen($subdirs[0]) == 0) {
            $dir = "";
            array_splice($subdirs, 0, 1);
        } else if($subdirs[0] == ".") {
            $dir = ".";
            array_splice($subdirs, 0, 1);
        } else if($subdirs[0] == "..") {
            $dir = "..";
            array_splice($subdirs, 0, 1);
        } else {
            $dir = ".";
        }
        foreach($subdirs as $p) {
            $dir = $dir."/".$p;
            @mkdir($dir);
            @chmod($dir, 0777);
        }
        return true;
    }

    public static function stat_get_ip_addr() {
        $ip = StatCommon::stat_get_ip_addr_by_net("eth1");
        if(strlen($ip) == 0) {
            $ip = StatCommon::stat_get_ip_addr_by_net("eth0");
        }
        if(strlen($ip) == 0) {
            $ip = "127.0.0.1";
        }
        return $ip;
    }

    private static function stat_get_ip_addr_by_net($net) {
        unset($out);
        @exec("/sbin/ifconfig ".$net." 2>/dev/null | grep inet | grep -v inet6 | awk -F':' '{print $2}' | awk '{print $1}'", $out);
        return $out[0];
    }
}

class StatInfo {
    public function __construct() {
        for($i=0; $i<OpCode::OP_END; $i++) {
            $this->m_ops[] = array();
        }
    }

	/**
	 * @brief 添加一项数据。最多只能增加30个kev-value对。
	 * @param key 键。不能以_开头或结束，StatInfo会自动过滤，比如_KEY_，会自动被修改成KEY；
	 *        不能够有“= : , ; . | \t”字符中的任何一个，否则程序运行时会崩溃
	 * @param value 值。不能有“= | \t”字符中的任何一个，否则程序运行时会崩溃
	 */
    public function add_info($key, $value) {
        StatCommon::stat_trim_underscore($key);

        if(is_string($value)) {
            if (!($this->is_valid_value($value) && count($this->m_info) <= 30)) return;
        } else {
            if (!($value > 0 && count($this->m_info) <= 30)) return;
        }
        if(!($this->is_valid_key($key)))   return;
        $this->m_info[$key] = $value;
    }

	/**
	 * @brief 添加一项操作。
	 * @param key1 键1。key1必须已经通过add_info方法添加好了
	 * @param key2 键2。key2必须已经通过add_info方法添加好了。仅当op的值为op_distr_use、
	 *             op_distr_sum、op_distr_max、op_distr_min之一时，key2的值才有意义
	 */
    public function add_op($op, $key1, $key2 = "") {
        StatCommon::stat_trim_underscore($key1);
        StatCommon::stat_trim_underscore($key2);

        if(!($this->is_valid_op($op) && array_key_exists($key1, $this->m_info))) return;

        switch($op) {
        case OpCode::OP_ITEM_SUM:
        case OpCode::OP_ITEM_MAX:
        case OpCode::OP_ITEM_SET:
            if(!array_key_exists($key2, $this->m_info))   return;
            $key1 = $key1.",".$key2;
            break;
        default:
            break;
        }

        $this->m_ops[$op][] = $key1;
        $this->m_has_op = true;
    }

	/**
	 * @brief 清空已经添加了的所有数据
	 */
    public function clear() {
        unset($this->m_info);
        $this->m_info = array();
        if($this->m_has_op === true) {
            for($i = OpCode::OP_BEGIN+1; $i != OpCode::OP_END; $i++) {
                $unset($this->m_ops[$i]);
                $this->m_ops[$i] = array();
            }
            $this->m_has_op = false;
        }
    }

    public function serialize(&$out) {
        foreach($this->m_info as $key => $value) {
            $out = $out."\t".$key."=".$value;
        }

        if($this->m_has_op) {
            $op = array(
                "", /* op_begin */
                "sum:", "max:", "set:", "ucount:",
                "item:", "item_sum:", "item_max:", "item_set:",
                "sum_distr:", "max_distr:", "min_distr:",
                "set_distr:",
                "ip_distr:",
                /* op_end */
            );

            $vline = "";
            $out = $out."\t_op_=";
            for($i = OpCode::OP_BEGIN+1; $i != OpCode::OP_END; $i++) {
                if(count($this->m_ops[$i])) {
                    $out = $out.$vline;
                    $this->serialize_op($out, $op[$i], $this->m_ops[$i]);
                    $vline = "|";
                }
            }
        }
    }

    private function serialize_op(&$out, $op, $keys) {
        $vline = "";
        foreach($keys as $key) {
            $out = $out.$vline.$op.$key;
            $vline = "|";
        }
    }

    private function is_valid_key($key) {
        return StatCommon::size_between($key, 1, $this->sc_key_maxsz)
            && strpos($key, "=") === false
            && strpos($key, ":") === false
            && strpos($key, ",") === false
            && strpos($key, ";") === false
            && strpos($key, "|") === false
            && strpos($key, "\t") === false
            && StatCommon::stat_is_utf8($key);
    }

    private function is_valid_value($value) {
        return StatCommon::size_between($value, 1, $this->sc_value_maxsz)
            && StatCommon::value_no_invalid_chars($value)
            && StatCommon::stat_is_utf8($value);
    }

    private function is_valid_op($op) {
        return OpCode::OP_BEGIN < $op && $op < OpCode::OP_END;
    }

    private $sc_key_maxsz = 64;
    private $sc_value_maxsz = 64;
    private $m_has_op = false;
    private $m_info = array();
    private $m_ops;
}

class StatLogger {
    /**
     * @brief StatLogger 构造函数
     *
     * @param game_id 应用ID 即游戏ID
     * @param zone_id 区ID
     * @param svr_id 服ID
     * @param site_id 平台ID 默认 -1表示该游戏不会拿出去放在不同的平台上运营  1：表示淘米平台
     * @param is_game 是否游戏后台络的数据
     */
    public function __construct($game, $zone = -1, $svr = -1, $site = -1, $isgame = 1) {
        $path = "/opt/taomee/stat/data";
        if(!(StatCommon::stat_makedir($path + "/inbox")))  return;
        $this->m_path = $path;

        $this->m_appid = $game;
        $this->m_siteid = $site;
        $this->m_zoneid = $zone;
        $this->m_svrid = $svr;
        $this->m_isgame = $isgame;
        if($this->m_appid == 1 ||
            $this->m_appid == 2 ||
            $this->m_appid == 5 ||
            $this->m_appid == 6 ||
            $this->m_appid == 10 ||
            $this->m_appid == 16 ||
            $this->m_appid == 19) {
                $this->m_need_multi = false;
            } else {
                $this->m_need_multi = true;
            }

        $this->m_basic_fd = false;
        $this->m_custom_fd = false;
        $this->m_basic_ts  = 0;
        $this->m_custom_ts = 0;

        $this->m_hostip = StatCommon::stat_get_ip_addr();

        $this->init_errlog();
        $this->m_inited = 1;
    }

    public function __destruct() {
        @fclose($this->m_basic_fd);
        @fclose($this->m_custom_fd);
        $this->fini_errlog();
    }

	/**
	 * @brief 统计当前在线人数，每分钟调用一次
	 * @param cnt 某个区服当前的总在线人数
     * @param zone 电信和网通 默认参数表示总体在线 这里主要是针对单区服类游戏需要分电信网通来看在线
	 */
    public function online_count($cnt, $zone = "") {
        if(cnt < 0) return;

        StatCommon::stat_trim_underscore($zone);
        $ts = time();

        $oss = "";

        $this->set_basic_info($oss, "_olcnt_", "_olcnt_", $ts, "-1");

        if(strlen($zone) == 0) {
            $zone = "_all_";
        }

        $oss = $oss."\t_zone_=".$zone."\t_olcnt_=".$cnt."\t_op_=item_max:_zone_,_olcnt_\n";

        $this->write_basic_log($oss, $ts);
    }

    /**
     * @brief 在注册账户（米米号）时调用。
     */
    public function reg_account($acct, $cli_ip, $ads="", $browser="", $device="", $os="", $resolution="", $network="", $isp="") {
        if(strlen($acct) == 0)  $acct = "-1";

        $ts = time();
        $op = "\t_op_=";
        $oss = "";

        $this->set_basic_info($oss, "_newuid_", "_newuid_", $ts, $acct);
        $this->set_device_info($oss, $op, $ads, $browser, $device, $os, $resolution, $network, $isp);

        if(strlen($cli_ip) != 0 && $this->is_valid_ip($cli_ip)) {
            $oss .= ("\t_cip_=".$cli_ip);
            $op .= ("ip_distr:_cip_|");
        }

        if(strlen($op) > 6) {
            $op = substr($op, 0, strlen($op) - 1);
            $oss .= ($op."\n");
        } else {
            $oss .= "\n";
        }

        $this->write_basic_log($oss, $ts);
    }

	// race: 职业  isp: 运营商
	/**
	 * @brief 用户在游戏中创建角色时调用
	 */
    public function reg_role($acct_id, $player_id = "", $race = "", $cli_ip = "", $ads_id = "", $browser = "", $device = "", $os = "", $resolution = "", $network = "", $isp = "") {
        if(strlen($acct) == 0)  $acct = "-1";

        $ts = time();
        $op = "\t_op_=";
        $oss = "";

        $this->set_basic_info($oss, "_newac_", "_newac_", $ts, $acct);
        $this->set_device_info($oss, $op, $ads, $browser, $device, $os, $resolution, $network, $isp);

        if(strlen($cli_ip) != 0 && $this->is_valid_ip($cli_ip)) {
            $oss .= ("\t_cip_=".$cli_ip);
            $op .= ("ip_distr:_cip_|");
        }

        if(strlen($op) > 6) {
            $op = substr($op, 0, strlen($op) - 1);
            $oss .= ($op."\n");
        } else {
            $oss .= "\n";
        }
        //角色新增
        if(strlen($player_id)) {
            if(!$this->is_valid_playerid($player_id))   return;
            $this->set_basic_info($oss, "_newpl_", "_newpl_", $ts, $acct_id, $player_id);
            $oss .= "\n";
        }
        //职业新增
        if(strlen($race)) {
            StatCommon::stat_trim_underscore($race);
            if(!$this->is_valid_race($race))    return;
            $this->set_basic_info($oss, "_newrace_", $race, $ts, $acct_id);
            $oss .= "\n";
        }

        $this->write_basic_log($oss, $ts);
    }

     /**
	 * @brief 用户登录游戏时(验证用户名和密码)调用
	 */
    public function verify_passwd($acct_id, $cli_ip = "", $ads_id = "", $browser = "", $device = "", $os = "", $resolution = "", $network = "", $isp = "") {
        if(strlen($acct_id) == 0) {
            $acct_id = "-1";
        }

        $ts = time();
        $op = "\t_op_=";
        $oss = "";
        $this->set_basic_info($oss, "_veripass_", "_veripass_", $ts, $acct_id);
        $this->set_device_info($oss, $op, $ads_id, $browser, $device, $os, $resolution, $network, $isp);

        if(strlen($cli_ip) != 0 && $this->is_valid_ip($cli_ip)) {
            $oss .= ("\t_cip_=".$cli_ip);
            $op .= ("ip_distr:_cip_|");
        }

        if(strlen($op) > 6) {
            $op = substr($op, 0, strlen($op) - 1);
            $oss .= ($op."\n");
        } else {
            $oss .= "\n";
        }

        $this->write_basic_log($oss, $ts);
    }

	/**
	 * @brief 用户登录游戏时(登录online)调用
	 * @param lv 取值范围1～5000
     * @param zone 为空表示总体，该字段主要时给公司老的单区服游戏使用，用于区分电信登录和网通登录
	 */
    public function login_online($acct_id, $player_id, $race, $isvip, $lv, $cli_ip = "", $ads_id = "", $zone="", $browser = "", $device = "", $os = "", $resolution = "", $network = "", $isp = "") {
        if(strlen($acct_id) == 0) {
            $acct_id = "-1";
        }

        $ts = time();
        $op = "\t_op_=item:_vip_|";
        if($this->is_valid_lv(lv) && $lv != 0) {
            $op .= "item:_lv_|";
        }
        $oss = "";
        //账号登陆
        $this->set_basic_info($oss, "_lgac_", "_lgac_", $ts, $acct_id);
        $oss .= ("\t_vip_=".$this->convert_isvip($isvip)."\t_lv_=".$lv);
        $this->set_device_info($oss, $op, $ads_id, $browser, $device, $os, $resolution, $network, $isp);

        if(strlen($cli_ip) != 0 && $this->is_valid_ip($cli_ip)) {
            $oss .= ("\t_cip_=".$cli_ip);
            $op .= ("ip_distr:_cip_|");
        }

        if(strlen($zone)) {//统计各区的登录人数
            StatCommon::stat_trim_underscore($zone);
            $oss .= ("\t_zone_=".$zone);
            $op .= "item:_zone_|";
        }

        if(strlen($op) > 6) {
            $op = substr($op, 0, strlen($op) - 1);
            $oss .= ($op."\n");
        } else {
            $oss .= "\n";
        }

        if(strlen($player_id)) {//角色登陆
            if(!$this->is_valid_playerid($player_id))   return;
            $this->set_basic_info($oss, "_lgpl_", "_lgpl_", $ts, $acct_id, $player_id);
            $oss .= "\n";
        }

        if(strlen($race)) {//职业登陆
            StatCommon::stat_trim_underscore($race);
            if(!$this->is_valid_race($race))    return;
            $this->set_basic_info($oss, "_lgrace_", $race, $ts, $acct_id);
            $oss .= ("\t_vip_=".$this->convert_isvip($isvip)."\t_lv_".$lv."\t_op_=item:_vip_|item:_lv_\n");
        }

        $this->write_basic_log($oss, $ts);
    }

    //手套启动设备
    public function start_device($device) {
        if(strlen($device) == 0) {
            $device = -1;
        }

        $ts = time();
        $oss = "";
        $this->set_basic_info($oss, "_startdev_", "_startdev_", $ts, $device);
        $oss .= "\n";
        $this->write_basic_log($oss, $ts);
    }

	/**
	 * @brief 用于统计用户登出人次和每次登录的游戏时长，只需在用户退出游戏的时候调用一次。
	 * @param oltime 本次登录游戏时长
	 */
    public function logout($acct_id, $isvip, $lv, $oltime) {
        if(!($this->is_valid_oltm($oltime) 
            && $this->is_valid_lv($lv)))   return;

        if(strlen($acct_id) == 0) {
            $acct_id = "-1";
        }

        $ts = time();
        $oss = "";
        $this->set_basic_info($oss, "_logout_", "_logout_", $ts, $acct_id);
        $oss .= ("\t_vip_=".$this->convert_isvip($isvip)."\t_lv_=".$lv."\t_oltm_=".$oltime."\t_intv_=".$this->logout_time_interval($oltime)."\t_op_=sum:_oltm_|item:_intv_\n");

        $this->write_basic_log($oss, $ts);
    }

	/**
	 * @brief 玩家每次等级提升时调用
	 */
    public function level_up($acct_id, $race, $lv) {
        if(!$this->is_valid_lv($lv))    return;

        if(strlen($acct_id) == 0) {
            $acct_id = -1;
        }

        $ts = time();
        $oss = "";
        //账号等级变化
        $this->set_basic_info($oss, "_aclvup_", "_aclvup_", $ts, $acct_id);
        $oss .= ("\t_lv_=".$this->convert_isvip($isvip)."\t_op_=set_distr:_lv_\n");
        //职业等级变化
        if(strlen($race)) {
            StatCommon::stat_trim_underscore($race);
            if(!$this->is_valid_race($race))    return;
            $this->set_basic_info($oss, "_racelvup_", $race, $ts, $acct_id);
            $oss .= ("\t_lv_=".$this->convert_isvip($isvip)."\t_op_=set_distr:_lv_\n");
        }

        $this->write_basic_log($oss, $ts);
    }

	/**
	 * @brief 玩家获得精灵时调用
	 */
    public function obtain_spirit($acct_id, $isvip, $lv, $spirit) {
        if(!$this->is_valid_lv($lv))    return;

        if(strlen($acct_id) == 0) {
            $acct_id = -1;
        }

        $ts = time();
        $oss = "";
        StatCommon::stat_trim_underscore($spirit);
        $this->set_basic_info($oss, "_obtainspirit_", "_obtainspirit_", $ts, $acct_id);
        $oss .= ("\t_lv_=".$this->convert_isvip($isvip)."\t_spirit_=".$spirit."\n");

        $this->write_basic_log($oss, $ts);
    }

	/**
	 * @brief 玩家失去精灵时调用
	 */
    public function lose_spirit($acct_id, $isvip, $lv, $spirit) {
        if(!$this->is_valid_lv($lv))    return;

        if(strlen($acct_id) == 0) {
            $acct_id = -1;
        }

        $ts = time();
        $oss = "";
        StatCommon::stat_trim_underscore($spirit);
        $this->set_basic_info($oss, "_losespirit_", "_losespirit_", $ts, $acct_id);
        $oss .= ("\t_lv_=".$this->convert_isvip($isvip)."\t_spirit_=".$spirit."\n");

        $this->write_basic_log($oss, $ts);
    }

    /**
     * @brief 商品系统统计米币购买道具时调用，游戏不需要调用
     * @param outcome 付费获得的道具名称
     */
    public function pay_item($acct_id, $isvip, $pay_amount, $currency, $outcome, $outcnt) {
        if(!( $pay_amount > 0)
            && $this->is_valid_currency($currency)
            && $outcnt > 0
            //&& $this->is_valid_common_utf8_parm($outcome, 0)
        )
            return ;
        $ts = time();
        //这里可以统计出付费总额(通常单个游戏，不应该出现多种货币单位)、VIP和非VIP用户付费总额、各种货币单位付费总额
        $op = "\t_op_=sum:_amt_";
        $oss = "";
        $this->set_basic_info($oss, "_mibiitem_", "_mibiitem_", $ts, $acct_id);
        $oss .= ("\t_vip_=".$this->convert_isvip($isvip)."\t_amt_=".$pay_amount."\t_ccy_=".$currency.$op."\n");
        $this->write_basic_log($oss, $ts);
    }

    /**
     * @brief 玩家每次在游戏内使用米币购买道具时调用
     * @param outcome 付费获得的道具名称。如果pay_reason选择的是pay_buy，则outcome赋值为相应的“道具名字”；
     *                如果pay_reason的值不是pay_buy，则本参数可以赋空字符串。
     * @param pay_channel 支付类型，如米币卡、米币帐户、支付宝、苹果官方、网银、手机付费等等，米币渠道传"1"。
     */
    public function pay($acct_id, $isvip, $pay_amount, $currency, $pay_reason, $outcome, $outcnt, $pay_channel) {
        if(!( ($pay_amount > 0) 
            && $this->is_valid_currency($currency)
            && $this->is_valid_payreason($pay_reason)
            && $outcnt > 0
            && ( strlen(outcome) || $pay_reason != PayReason::PAY_BUY)
            && $this->is_valid_common_utf8_parm($outcome, 0)
            && $this->is_valid_common_utf8_parm($pay_channel, 1) )
        )
            return;

        if(strlen($acct_id) == 0) {
            $acct_id = -1;
        }

        switch($pay_reason) {
        case PayReason::PAY_CHARGE :
            $reason = "_buycoins_";
			break;
        case PayReason::PAY_VIP :
            $reason   = "_vipmonth_";
            break;
        case PayReason::PAY_BUY:
            $reason   = "_buyitem_";
            break;
        case PayReason::PAY_FREE:
            $reason = "_costfree_";
            break;
        default:
            $reason = "";
            break;
        }

        $ts = time();
        //这里可以统计出付费总额(通常单个游戏，不应该出现多种货币单位)、VIP和非VIP用户付费总额、各种货币单位付费总额
        $op = "\t_op_=sum:_amt_|item_sum:_vip_,_amt_|item_sum:_paychannel_,_amt_|item_sum:_ccy_,_amt_|item:_paychannel_";
        $oss = "";
        // 帐号付费 //不包含赠送渠道
        if($pay_reason != PayReason::PAY_FREE)
        {
            $this->set_basic_info($oss, "_acpay_", "_acpay_", $ts, $acct_id);
            $oss .= ("\t_vip_=".$this->convert_isvip($isvip)."\t_amt_=".$pay_amount."\t_ccy_=".$currency."\t_paychannel_=".$pay_channel.$op."\n");
        }

        $this->set_basic_info($oss, "_acpay_", $reason, $ts, $acct_id);
        $oss .= ("\t_vip_=".$this->convert_isvip($isvip)."\t_amt_=".$pay_amount."\t_ccy_=".$currency."\t_paychannel_=".$pay_channel.$op."\n");

        $this->write_basic_log($oss, $ts);

        switch ($pay_reason)
        {
        case PayReason::PAY_CHARGE:
            //需要统计金币产出
            $this->do_obtain_golds($acct_id,  "_userbuy_", $outcnt);
            break;
        case PayReason::PAY_VIP:
            //需要统计各个包月时长
            $this->do_buy_vip($acct_id, $pay_amount, $outcnt);
            break;
        case PayReason::PAY_BUY:
            //需要统计道具产出
            $this->do_buy_item($acct_id, $isvip, 0, $pay_amount, "_mibiitem_", $outcome, $outcnt);
            break;
        default:
            break;
        }
    }

    /**
     * @brief unsubscribe 退订VIP服务
     *
     * @param std::acct_id  用户账户
     * @param channel   退订渠道(目前只有米币和短信两个渠道)
     */
    public function unsubscribe($acct_id, $uc)
    {
        if(strlen($acct_id) == 0) {
            $acct_id = -1;
        }

        $ts = time(0);
        $oss = "";
        $op = "\t_op_=item:_uc_";
        $this->set_basic_info($oss, "_unsub_", "_unsub_", $ts, $acct_id);
        $oss .= ("\t_uc_=".$uc.$op."\n");
        $this->write_basic_log($oss, $ts);
    }

    /**
     * @brief cancel_acct 销户
     *
     * @param acct_id   用户帐户
     * @param channel   销户渠道
     */
    public function cancel_acct($acct_id, $channel)
    {
        if(strlen($acct_id) == 0) {
            $acct_id = -1;
        }

        $ts = time(0);
        $oss = "";
        $op = "\t_op_=item:_cac_";
        $this->set_basic_info($oss, "_ccacct_", "_ccacct_", $ts, $acct_id);
        $oss .= ("\t_cac_=".$uc.$op."\n");
        $this->write_basic_log($oss, $ts);
    }

	/**
	 * @brief 玩家每次获得游戏金币时调用 也就是游戏币产出。注意：玩家通过付费充值方式获得金币时，系统会自动调用该函数。
	 * @param reason 获得的金币数量。1~1000000。 系统赠送游戏内一级货币时调用
	 */
    public function obtain_golds($acct_id, $amt) {
        if(!(0 < $amt && $amt <= 1000000000))   return;

        if(strlen($acct_id) == 0) {
            $acct_id = -1;
        }
        $this->do_obtain_golds($acct_id, "_systemsend_", $this->m_need_multi?$amt*100:$amt);
    }

	/**
	 * @brief 玩家每次使用游戏金币时调用 也就是说游戏币消耗。
	 * @param reason 使用游戏内一级货币的原因，如获得特权、复活等等。 购买道具时无需调用。
	 * @param reason 使用的数量。1~100000。
	 */
    public function use_golds($acct_id, $is_vip, $reason, $amt, $lv) {
        StatCommon::stat_trim_underscore($reason);

        if(!( $amt > 0
            && $amt <= 100000)
            && $this->is_valid_common_utf8_parm($reason)
            && $tis->is_valid_lv($lv)
        )
            return;

        if(strlen($acct_id) == 0) {
            $acct_id = -1;
        }

        if(strlen($reason) == 0) {
            $reason = "unknown";
        }

        $ts = time();
        $oss = "";
        $this->set_basic_info($oss, "_usegold_", "_usegold_", $ts, $acct_id);
        $oss .= ("\t_golds_=".($this->m_need_multi?$amt*100:$amt)."\t_op_=sum:_golds_\n");

        $this->set_basic_info($oss, "_usegold_", $reason, $ts, $acct_id);
        $oss .= ("\t_golds_=".($this->m_need_multi?$amt*100:$amt)."\t_isvip_=".$is_vip."\t_lv_=".$lv."\t_op_=item:_isvip_|item_sum:_lv_,_golds_|sum:_golds_\n");

        $this->write_basic_log($oss, $ts);
    }

	/**
	 * @brief 游戏内一级货币购买道具
	 * @param pay_amount 付出的米币/游戏金币数量
	 * @param outcome 购买的道具名称
	 * @param outcnt 道具数量
     * 通过一级游戏币购买道具时调用
	 */
    public function buy_item($acct_id, $isvip, $lv, $pay_amount, $outcome, $outcnt) {
        if(!($this->is_valid_lv($lv)
            && $pay_amount > 0
            && $this->is_valid_common_utf8_parm($outcome)
            && $outcnt > 0)
        )
            return;

        if(strlen($acct_id) == 0) {
            $acct_id = -1;
        }

        if(strlen($outcome) == 0) {
            $outcome = -1;
        }

        $this->do_buy_item($acct_id, $this->convert_isvip($isvip), $lv, $this->m_need_multi?$pay_amount*100:$pay_amount, "_coinsbuyitem_", $outcome, $outcnt);
        $this->use_golds_buyitem($acct_id, $this->convert_isvip($isvip), $this->m_need_multi?$pay_amount*100:$pay_amount, $lv);
    }

	/**
	 * @brief 接受任务。任务预设新手、主线、支线和其他。其他类任务可以通过页面配置具体任务类型。
	 */
    public function accept_task($type, $acct_id, $task_name, $lv) {
        $this->do_task($type, $acct_id, $task_name, $lv, 0);
    }

	/**
	 * @brief 完成任务。
	 */
    public function finish_task($type, $acct_id, $task_name, $lv) {
        $this->do_task($type, $acct_id, $task_name, $lv, 1);
    }

	/**
	 * @brief 放弃任务。
	 */
    public function abort_task($type, $acct_id, $task_name, $lv) {
        $this->do_task($type, $acct_id, $task_name, $lv, 2);
    }

	/**
	 * @brief 新增注册转化
	 */
    public function new_trans($step, $acct_id) {
        if(!$this->is_valid_newtransstep($step))    return;

        if(strlen($acct_id) == 0) {
            $acct_id = "-1";
        }

        $ts = time();
        $oss = "";
        $this->set_basic_info($oss, "_newtrans_", $this->get_new_trans_step($step), $ts, $acct_id);
        $oss .= "\n";
        $this->write_basic_log($oss, $ts);
    }

	/**
	 * @brief 自定义统计项
	 * @param stat_name 主统计名称，不能以_开头或结束，StatLogger会自动把头尾的_去掉。
	 * @param sub_stat_name 子主统计名称，一个主统计名称下可以有多个子统计项。
	 *                      不能以_开头或结束，StatLogger会自动把头尾的_去掉。
	 */
    public function log($stat_name, $sub_stat_name, $acct_id, $player_id = "-1", $info = null) {
        StatCommon::stat_trim_underscore($stat_name);
        StatCommon::stat_trim_underscore($sub_stat_name);

        if(!( $this->is_valid_common_utf8_parm($stat_name)
            && $this->is_valid_common_utf8_parm($sub_stat_name) )
        )
            return;

        if(strlen($acct_id) == 0) {
            $acct_id = "-1";
        }
        if(strlen($stat_name) == 0) {
            $stat_name = "unknown";
        }
        if(strlen($sub_stat_name) == 0) {
            $sub_stat_name = "unknown";
        }

        $ts = time();
        $oss = "";
        $this->set_basic_info($oss, $stat_name, $sub_stat_name, $ts, $acct_id, $player_id);
        if($info instanceof StatInfo) {
            $info->serialize($oss);
        }
        $oss .= "\n";

        $this->write_custom_log($oss, $ts);

    }
    
    //private function
    private function use_golds_buyitem($acct_id, $is_vip, $amt, $lv)  {
        if(strlen($acct_id) == 0) {
            $acct_id = -1;
        }

        $ts = time();
        $oss = "";
        $this->set_basic_info($oss, "_usegold_", "_usegold_", $ts, $acct_id);
        $oss .= ("\t_golds_=".$amt."\t_op_=sum:_golds_\n");

        $this->set_basic_info($oss, "_usegold_", "_buyitem_", $ts, $acct_id);
        $oss .= ("\t_golds_=".$amt."\t_isvip_=".$this->convert_isvip($is_vip)."\t_lv_=".$lv."\t_op_=item:_isvip_|item_sum:_lv_,_golds_|sum:_golds_\n");

        $this->write_basic_log($oss, $ts);
    }

    private function do_obtain_golds($acct_id, $reason, $amt) {
        $ts = time();
        $oss = "";
        // 帐号获得游戏金币
        $this->set_basic_info($oss, "_getgold_", $reason, $ts, $acct_id);
        $oss .= ("\t_golds_=".$amt."\t_op_=sum:_golds_\n");

        $this->write_basic_log($oss, $ts);
    }

    private function do_buy_vip($acct_id, $pay_amount, $amt)
    {
        $ts = time(0);
        $oss;
        $this->set_basic_info($oss, "_buyvip_", "_buyvip_", $ts, $acct_id);
        $oss .= ("\t_payamt_=".$pay_amount."\t_amt_=".$amt."\t_op_=item:_amt_|item_sum:_amt_,_payamt_\n");
        $this->write_basic_log($oss, $ts);
    }
    private function do_buy_item($acct_id, $isvip, $lv, $pay_amount, $pay_type, $outcome, $outcnt)
    {
        $ts = time(0);
        $oss = "";

        $this->set_basic_info($oss, "_buyitem_", $pay_type, $ts, $acct_id);
        $op = "\t_op_=sum:_golds_";
        if($this->is_valid_lv($lv) && $lv != 0) {
            $op .= "|item:_lv_";
        }

        $oss .= ("\t_isvip_=".$isvip."\t_item_=".$outcome."\t_itmcnt_=".$outcnt."\t_golds_=".$pay_amount."\t_lv_=".$lv.$op."\n");

        $this->write_basic_log($oss, $ts);
    }

    private function do_task($type, $acct_id, $task_name, $lv, $step) {
        StatCommon::stat_trim_underscore($task_name);

        if(!($this->is_valid_tasktype($type)
            && $this->is_valid_common_utf8_parm($task_name))
        )
            return;

        if(strlen($acct_id) == 0) {
            $acct_id = "-1";
        }
        if(strlen($task_name) == 0) {
            $task_name = "unknown";
        }

        $ts = time();
        $oss = "";

        $this->set_basic_info($oss, $this->get_task_stid($type, $step), $task_name, $ts, $acct_id);
        if($this->is_valid_lv($lv) && $lv > 0) {
            $oss .= ("\t_lv_=".$lv."\t_op_=item:_lv_\n");
        } else {
            $oss .= "\n";
        }

        $this->write_basic_log($oss, $ts);
    }
    
    private function logout_time_interval($tm) {
        if ($tm <= 0) {
            return "0";
        } else if ($tm < 11) {
            return "0~10";
        } else if ($tm < 61) {
            return "11~60";
        } else if ($tm < 301) {
            return "61~300";
        } else if ($tm < 601) {
            return "301~600";
        } else if ($tm < 1201) {
            return "601~1200";
        } else if ($tm < 1801) {
            return "1201~1800";
        } else if ($tm < 2401) {
            return "1801~2400";
        } else if ($tm < 3001) {
            return "2401~3000";
        } else if ($tm < 3601) {
            return "3001~3600";
        } else if ($tm < 7201) {
            return "3601~7200";
        } else {
            return "大于7200";
        }
    }
    
    private function set_basic_info(&$oss, $stid, $sstid, $ts, $acct, $pid = "-1") {
        if(strlen($acct) == 0)  $acct = "-1";
        if(strlen($pid) == 0)   $pid = "-1";
        $oss .= ("_hip_=".$this->m_hostip."\t_stid_=".$stid."\t_sstid_=".$sstid."\t_gid_=".$this->m_appid."\t_zid_=".$this->m_zoneid."\t_sid_=".$this->m_svrid."\t_pid_=".$this->m_siteid."\t_ts_=".$ts."\t_acid_=".$acct."\t_plid_=".$pid);
    }
    
    private function set_device_info(&$oss, &$op, $ads, $browser, $device, $os, $resolution, $network, $isp) {
        if(strlen($ads)) {
            if(!StatCommon::value_no_invalid_chars($ads))   return;
            $oss .= ("\t_ad_=".$ads);
            //$op .= "item:_ads_,";
        }
        if(strlen($browser)) {
            if(!$this->is_valid_common_utf8_parm($browser)) return;
            $oss .= ("\t_ie_=".$browser);
            $op .= "item:_ie_|";
        }
        if(strlen($device)) {
            if(!$this->is_valid_common_utf8_parm($device)) return;
            $oss .= ("\t_dev_=".$device);
            $op .= "item:_dev_|";
        }
        if(strlen($os)) {
            if(!$this->is_valid_common_parm($os)) return;
            $oss .= ("\t_os_=".$os);
            $op .= "item:_os_|";
        }
        if(strlen($resolution)) {
            if(!$this->is_valid_common_parm($resolution)) return;
            $oss .= ("\t_res_=".$resolution);
            $op .= "item:_res_|";
        }
        if(strlen($network)) {
            if(!$this->is_valid_common_parm($network)) return;
            $oss .= ("\t_net_=".$network);
            $op .= "item:_net_|";
        }
        if(strlen($isp)) {
            if(!$this->is_valid_common_parm($isp)) return;
            $oss .= ("\t_isp_=".$isp);
            $op .= "item:_isp_|";
        }
    }

    private function write_basic_log($s, $ts) {
        if($this->m_inited == 0) {
            $this->errlog("object not inited");
            return;
        }

        if(($ts < $this->m_basic_ts) || (($ts - $this->m_basic_ts) > 19) || ($this->m_basic_fd === false) || ((int)(($this->m_basic_ts + 28800)/86400) != (int)($ts + 28800)/86400)) {
            @fclose($this->m_basic_fd);
            $this->m_basic_ts = $ts - ($ts % 20);
            if($this->m_isgame) {
                $oss = $this->m_path."/inbox/".$this->m_appid."_game_basic_".$this->m_basic_ts;
            } else {
                $oss = $this->m_path."/inbox/".$this->m_appid."_account_basic_".$this->m_basic_ts;
            }
            $this->m_basic_fd = @fopen($oss, "a");
            if($this->m_basic_fd === false) {
                $this->errlog("[write_basic_log]open(): ".$oss);
            }
            @chmod($oss, 0777);
            if(fwrite($this->m_basic_fd, $s) === false) {
                $this->errlog("[write_baic_log]write(): ".$s);
                $this->m_basic_fd = false;
            }
        }
    }

    private function write_custom_log($s, $ts) {
        if($this->m_inited == 0) {
            $this->errlog("object not inited");
            return;
        }

        if(($ts < $this->m_custom_ts) || (($ts - $this->m_custom_ts) > 19) || ($this->m_custom_fd === false) || ((int)(($this->m_custom_ts + 28800)/86400) != (int)($ts + 28800)/86400)) {
            @fclose($this->m_custom_fd);
            $this->m_custom_ts = $ts - ($ts % 20);
            if($this->m_isgame) {
                $oss = $this->m_path."/inbox/".$this->m_appid."_game_custom_".$this->m_custom_ts;
            } else {
                $oss = $this->m_path."/inbox/".$this->m_appid."_account_custom_".$this->m_custom_ts;
            }
            $this->m_custom_fd = @fopen($oss, "a");
            if($this->m_custom_fd === false) {
                $this->errlog("[write_custom_log]open(): ".$oss);
            }
            @chmod($oss, 0777);
            if(@fwrite($this->m_custom_fd, $s) === false) {
                $this->errlog("[write_baic_log]write(): ".$s);
                $this->m_custom_fd = false;
            }
        }
    }

    private function init_errlog() {
        $logpath = $this->m_path."/log";
        if(!StatCommon::stat_makedir($logpath)) return;
        $logpath .= "/error.log";
        $this->m_of = @fopen($logpath, "a");
        if($this->m_of === false)   return;
        $this->errlog("StatLogger Constructed");
    }

    private function errlog($msg) {
        $ts = date("Y-m-d H:i:s", time());
        @fwrite($this->m_of, "[".$ts."]".$msg."\n");
    }

    private function fini_errlog() {
        $this->errlog("StatLogger Destructed.");
    }

    private function is_valid_appid($appid) {
        return is_int($appid);
    }

    private function is_valid_acctid($acctid) {
        return is_int($acctid);
    }

    private function is_valid_playerid($pid) {
        return is_int($pid);
    }

    private function is_valid_race($race) {
        return StatCommon::size_between($race, 1, 256)
            && StatCommon::value_no_invalid_chars($race)
            && StatCommon::stat_is_utf8($race);
    }

    private function is_valid_ip($ip) {
        $dip = split("\.", $ip);
        if(count($dip) != 4)    return false;
        foreach($dip as $sip) {
            if($sip > 255 || $sip < 0) return false;
        }
        return true;
    }

    private function is_valid_lv($lv) {
        return is_int($lv) && ($lv >= 0) && ($lv <= 5000);
    }

    private function is_valid_oltm($oltm) {
        return is_int($oltm) && ($oltm >= 0) && ($oltm <= 864000);
    }

    private function is_valid_currency($ccy) {
        return is_int($ccy) && $ccy > CurrencyType::CCY_BEGIN && $ccy < CurrencyType::CCY_END;
    }

    private function is_valid_payreason($r) {
        return is_int($r) && $r > PayReason::PAY_BEGIN && $r < PayReason::PAY_END;
    }

    private function is_valid_tasktype($type) {
        return is_int($type) && $type > TaskType::TASK_BEGIN && $r < TaskType::TASK_END;
    }

    private function is_valid_newtransstep($step) {
        return is_int($step) && $step > NewTransStep::NW_BEGIN && $step < NewTransStep::NW_END;
    }

    private function is_valid_common_parm($parm, $min = 1, $max = 256) {
        return StatCommon::size_between($parm, $min, $max)
            && StatCommon::value_no_invalid_chars($parm);
    }

    private function is_valid_common_utf8_parm($parm, $min = 1, $max = 256) {
        return StatCommon::size_between($parm, $min, $max)
            && StatCommon::value_no_invalid_chars($parm)
            && StatCommon::stat_is_utf8($parm);
    }

    private function get_task_stid($type, $stage) {
        return $this->task_stid[$type][$stage];
    }

    private function get_new_trans_step($step) {
        return $this->new_trans_stid[$step];
    }

    private function convert_isvip($isvip) {
        if(!is_bool($isvip)) {
            if(!is_int($isvip)) {
                $isvip = 0;
            } else {
                if($isvip != 0) $isvip = 1;
            }
        } else {
            $isvip = $isvip ? 1 : 0;
        }
        return $isvip;
    }

    //const array
    private $task_stid = array(
        array( "", "", "" ),
        array( "_getnbtsk_", "_donenbtsk_", "_abrtnbtsk_" ),
        array( "_getmaintsk_", "_donemaintsk_", "_abrtmaintsk_" ),
        array( "_getauxtsk_", "_doneauxtsk_", "_abrtauxtsk_" ),
        array( "_getetctsk_", "_doneetctsk_", "_abrtetctsk_" )
    );

    private $new_trans_stid = array(
        "",
        "fGetRegSucc", "fLoadRegSucc", "fSendLoginReq",
        "bGetLoginReq", "bSendLoginReq", "bGetLoginSucc",
        "fGetLoginSucc", "fLoadLoginSucc", "fClickStartBtn",
        "bGetNewroleReq", "bSendNewroleSucc", "fStartSrvlistReq",
        "bStartGetSrvlist", "bGetSrvlistSucc", "fGetSrvlistSucc",
        "fSendOnlineReq", "fSend1001Req", "bSendOnlineSucc",
        "fOnlineSucc", "fLoadInfoSucc", "fInterGameSucc"
    );

    //member
    private $m_siteid;
    private $m_appid;
    private $m_isgame;
    private $m_zoneid;
    private $m_svrid;
    private $m_ip;
    private $m_path;
    private $m_basic_fd;
    private $m_custom_fd;
    private $m_basic_ts;
    private $m_custom_ts;
    private $m_of;
    private $m_inited;
    private $m_need_multi;
}

class OpCode {
	const OP_BEGIN = 0;

	const OP_SUM = 1;    // 把某个字段某时间段内所有值相加
	const OP_MAX = 2;    // 求某字段某时间段内最大值
    const OP_SET = 3;    //直接取某字段最新的数值
    const OP_UCOUNT = 4; //对某个字段一段时间的值做去重处理

	const OP_ITEM = 5;      // 求某个大类下的各个ITEM求人数人次
	const OP_ITEM_SUM = 6;  // 对各个ITEM的产出数量/售价等等求和
	const OP_ITEM_MAX = 7;  // 求出各个ITEM的产出数量/售价等等的最大值
    const OP_ITEM_SET = 8;  //求出每个ITEM的最新数值

	const OP_SUM_DISTR = 9; // 对每个人的某字段求和，然后求出前面的“和”在各个区间下的人数
	const OP_MAX_DISTR = 10;// 对每个人的某字段求最大值，然后求出前面的“最大值”在各个区间下的人数
	const OP_MIN_DISTR = 11;//对每个人的某字段求最小值，然后根据前面的最小值在各个区间下做人数分布
    const OP_SET_DISTR = 12;//取某个字段的最新值，做分布

	const OP_IP_DISTR = 13; // 根据IP字段求地区分布的人数人次

	const OP_END = 14;
}

class CurrencyType {
    const CCY_BEGIN = 0;

    const CCY_MIBI  = 1;
    const CCY_CNY   = 2;

    const CCY_END   = 3;
}

class PayReason {
    const PAY_BEGIN = 0;

    const PAY_VIP   = 1;
    const PAY_BUY   = 2;
    const PAY_CHARGE= 3;
    const PAY_FREE  = 4;

    const PAY_END   = 5;
}

class TaskType {
    const TASK_BEGIN    = 0;

    const TASK_NEWBIE   = 1;    //新手任务
    const TASK_STORY    = 2;    //主线任务
    const TASK_SUPPLEMENT = 3;  //支线任务

    const TASK_END      = 4;
}

Class UnsubscribeChannel
{
    const UC_BEGIN = 0;

    const UC_DUANXIN = 1;    //短信
    const UC_MIBI = 2;       //米币

    const UC_END = 3;
}

class NewTransStep
{
    const NW_BEGIN = 0;
    const fGetRegSucc = 1;
    const fLoadRegSucc = 2;
    const fSendLoginReq = 3;
    const bGetLoginReq = 4;
    const bSendLoginReq = 5;
    const bGetLoginSucc = 6;
    const fGetLoginSucc = 7;
    const fLoadLoginSucc = 8;
    const fClickStartBtn = 9;
    const bGetNewroleReq = 10;
    const bSendNewroleSucc = 11;
    const fStartSrvlistReq = 12;
    const bStartGetSrvlist = 13;
    const bGetSrvlistSucc = 14;
    const fGetSrvlistSucc = 15;
    const fSendOnlineReq = 16;
    const fSend1001Req = 17;
    const bSendOnlineSucc = 18;
    const fOnlineSucc = 19;
    const fLoadInfoSucc = 20;
    const fInterGameSucc = 21;
    const NW_END = 22;
}

//$info = new StatInfo();
//$info->add_info("item", "首页点击");
//$info->add_info("count", "1");
//$info->add_op(OpCode::OP_UCOUNT, "item");
//$info->add_op(OpCode::OP_SUM, "count");
//$info->add_op(OpCode::OP_ITEM_SUM, "item", "count");
#$info->serialize($s);
#echo $s;
//StatCommon::stat_mkdir("/opt/taomee/stat/data/spool/dsa/fg/o1h");
$logger = new StatLogger(3);//1是游戏id
//$logger->online_count(18);
//$logger->reg_account(1932, "192.168.11.1", "", "chrome", "PC", "Linux", "1050*1600", "LAN", "tel");
//$logger->reg_role(1932, 1, 1, "192.168.11.1", "", "chrome", "PC", "Linux", "1050*1600", "LAN", "tel");
//$logger->verify_passwd("dkla");
//$logger->login_online("dsakl", "", "1", 0, 10, "hahah", "hdhdh", "dsakl", "d12hjk", "dev");
//$logger->logout("dhaslk", 0, 10, 100);
//$logger->level_up("me", "monkey", 30);
//$logger->obtain_spirit("you", 1, 10, "fhsjk");
//$logger->lose_spirit("you", 0, 90, 7232);
//$logger->pay_item(158456, 1, 10.1465, CurrencyType::CCY_MIBI, "精灵", 10);
//$logger->new_trans(NewTransStep::bSendOnlineSucc, 10);
//$logger->accept_task(TaskType::TASK_STORY, 10, "haha", 90);
//$logger->log("seer", "web", 3215, "", $info);
$logger->obtain_golds("me", 10);
$logger->use_golds(478912, 0, "test", 1, 1);
//$logger->pay(31267843, 0, 100, CurrencyType::CCY_MIBI, PayReason::PAY_BUY, "道具", 1, 1);
?>
