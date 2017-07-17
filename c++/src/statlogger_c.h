/*
 * =====================================================================================
 *
 *       Filename:  statlogger.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2014年03月17日 16时23分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_STATLOGGER_C_H
#define H_STATLOGGER_C_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief TaskType & OpCode enum需与C++接口保持一致 
 */
typedef enum TaskType {
    task_begin,

    task_newbie,     /* ! 新手任务 */
    task_story,      /* ! 主线任务 */
    task_supplement, /* ! 支线任务 */
    //task_etc,        /* ! 其他任务 */

    task_end
}TaskType;

typedef enum OpCode {
    op_begin,

    op_sum,    // 把某个字段某时间段内所有值相加
    op_max,    // 求某字段某时间段内最大值
    op_set,    //直接取某字段最新的数值
    op_ucount, //对某个字段一段时间的值做去重处理

    op_item,      // 求某个大类下的各个item求人数人次
    op_item_sum,  // 这三个操作符摩尔庄园这里不需要用 
    op_item_max,  // 这三个操作符摩尔庄园这里不需要用
    op_item_set,  //这三个操作符摩尔庄园这里不需要用

    op_sum_distr, // 对每个人的某字段求和，然后求出前面的“和”在各个区间下的人数
    op_max_distr, // 对每个人的某字段求最大值，然后求出前面的“最大值”在各个区间下的人数
    op_min_distr, //对每个人的某字段求最小值，然后根据前面的最小值在各个区间下做人数分布
    op_set_distr, //取某个字段的最新值，做分布

    op_ip_distr,     // 根据ip字段求地区分布的人数人次

    op_end
}OpCode;

typedef enum NewTransStep
{
    nw_begin,
    fGetRegSucc,
    fLoadRegSucc,
    fSendLoginReq,
    bGetLoginReq,
    bSendLoginReq,
    bGetLoginSucc,
    fGetLoginSucc,
    fLoadLoginSucc,
    fClickStartBtn,
    bGetNewroleReq,
    bSendNewroleSucc,
    fStartSrvlistReq,
    bStartGetSrvlist,
    bGetSrvlistSucc,
    fGetSrvlistSucc,
    fSendOnlineReq,
    fSend1001Req,
    bSendOnlineSucc,
    fOnlineSucc,
    fLoadInfoSucc,
    fInterGameSucc,
    nw_end
}NewTransStep;

#ifdef __cplusplus
extern "C"{
#endif
/**
 *  初始化,调用接口前必须先初始化 
 *  game_id:游戏ID
 *  zone_id:区ID,不区分传-1
 *  svr_id:服ID,不区分传-1
 *  site_id:平台ID -1表示该游戏不会拿出去放在不同的平台上运营  1：表示淘米平台
 */
int init(int game_id, int32_t zone_id, int32_t svr_id, int32_t site_id);
/* *
 *    统计当前在线人数 每分钟调用一次
 *    cnt: 当前在线
 *    zoneid: -1总体在线  0电信在线 1网通在线
 */
int online_count(int cnt, int zoneid);

/* *
 * 注册角色
 * uid:用户米米号
 * cli_ip: 用户的IP地址,不知道ID地址填0
 * browser 用户浏览器
 * os 用户flash版本
 * ads_id 广告渠道
 */
int reg_role(int uid, uint32_t cli_ip,char* browser,char* os,char* ads_id);

/* *
 * 验证用户名和密码
 * uid: 用户米米号
 */
int verify_passwd(int uid,uint32_t cli_ip);

/* *
 * 登录online
 * uid: 用户米米号
 * lv: 用户的等级
 * cli_ip: 用户登录时的IP地址,无法获取填0
 */
int login_online(int uid,int lv, uint32_t cli_ip,char* browser,char* os,char* ads_id);

/* *
 * 登出online
 * uid:用户米米号
 * oltime:用户本次在线时长，单位：秒
 */
int logout(int uid , int oltime);

/* *
 * 米币购买道具
 * uid:用户米米号
 * isvip:用户是否vip
 * float:花费的米币*100,花费1米币的话这里传100
 * outcome_id:获得的道具id
 * outcnt:获得的道具数量
 */
int pay(int uid, bool isvip, float pay_amount, int outcome_id, int outcnt);

/* *
 * 升级
 * uid:用户米米号
 * lv:升级后的等级
 */
int level_up(int uid, int lv);


/* *
 * 接收任务
 * uid:用户米米号
 * type:任务类型
 * taskid:任务id
 */
int accept_task(int uid, TaskType type, int taskid);

/* *
 * 完成任务
 * uid:用户米米号
 * type:任务类型
 * taskid:任务id
 */
int finish_task(int uid, TaskType type, int taskid);

/* *
 * 放弃任务
 * uid:用户米米号
 * type: 任务类型
 * taskid:任务 id
 */
int abort_task(int uid, TaskType type, int taskid);

/* *
 * 免费获得游戏币
 * uid:用户米米号
 * amt:获得金币数据的100倍
 */
int obtain_golds(int uid, int amt);

/* *
 * 使用游戏币购买道具
 * uid:用户米米号
 * isvip:是否vip
 * lv:用户购买道具时等级
 * pay_amount:支付游戏币数量的100倍
 * itemid:道具id
 * outcnt:道具数量
 */
int buy_item(int uid, bool isvip, int lv, float pay_amount, int itemid, int outcnt);

/* *
 * 消耗游戏币(购买的道具除外)
 * uid:用户米米号
 * isvip:是否vip
 * reason:购买原因(开启新功能、跳过关卡等)
 * pay_amount:支付游戏币数量的100倍
 * lv:用户购买道具时等级
 */
int use_golds(int uid, bool isvip, const char* reason, float pay_amount, int lv);

/* *
 * 新增注册转化
 * uid:用户米米号
 * step:新增步骤
 */
int new_trans(int uidi, NewTransStep step);

/* *
 * 自定义计算人数人次
 * 对sub_stat_name做人数人次计算 比如要统计 庄园小游戏->抓猪猪 这个游戏的人数人次
 * 这样调用: selflog(471834234, "庄园小游戏", "抓猪猪");
 */
int selflog(int uid, char *stat_name, char *sub_stat_name);

/* *
 * 自定义计算item类型的人数人次
 * 除了对sub_stat_name做人数人次计算外，还对每个itemid做人数人次,
 * 比如　庄园礼包领取情况->超拉礼包领取情况 下面的一星礼包 、二星礼包、三星礼包、四星礼包领取的统计,可以如下调用：
 * selfitemlog(4534636, "庄园礼包领取情况", "超拉礼包领取情况","count", "一星礼包" ,op_item);
 * selfitemlog(4534636, "庄园礼包领取情况", "超拉礼包领取情况","count", "二星礼包" ,op_item);
 * selfitemlog(4534636, "庄园礼包领取情况", "超拉礼包领取情况","count", "三星礼包" ,op_item);
 * selfitemlog(4534636, "庄园礼包领取情况", "超拉礼包领取情况","count", "四星礼包" ,op_item);
 * 就可以统计出超拉礼包领取的人数人次，以及每星的领取人数人次
 */
int selfitemlog(int uid, char *stat_name, char *sub_stat_name, char* key,char* value,OpCode op);

/**
 * @brief 将selfitemlog的key,value,op格式化成字符串后传入,可一次处理多个操作
 * 考虑传入的字符串出错可能性较大,暂不提供
 */
/*int selfitemlogWithFormat(int uid, char *stat_name, char *sub_stat_name, char* format);*/
#ifdef __cplusplus
}//extern "C"
#endif

#endif  //H_STATLOGGER_C_H
