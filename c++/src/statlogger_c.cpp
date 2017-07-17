#include "statlogger_c.h"
#include "statlogger.h"
#include <string>
#include <sstream>
#include <istream>
#include <vector>
#include <cstdlib>

StatLogger logger;
uint8_t isInit = 0;

template<typename _Ty>
std::string type2string(_Ty val,bool isSetEmpty=false){
    if(isSetEmpty){
        return "";
    }
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

struct key_val_op{
    std::string key;
    std::string val;
    int op;
    void clear(){
        this->key.clear();
        this->val.clear();
        this->op = -1;
    }
};

/**
 * @brief  分割字符串,默认按逗号分割
 */
std::vector<key_val_op> split(std::string str,char pattern=','){
    std::vector<key_val_op> tmp;
    std::string item,trim_str(str);
    stat_trim(trim_str,type2string(pattern));//去除字符串开头结尾的pattern
    std::istringstream iss(trim_str);

    int index = 0;
    key_val_op block;
    while(getline(iss,item,pattern)){
        ++index;
        if(index % 3 == 0){
            block.op = atoi(item.c_str());
            tmp.push_back(block);//每3个为构成结构体
        }else if(index % 3 == 1){
            block.clear();//读取key表示开始一个新结构体,初始化
            block.key = item;
        }else{
            block.val = item;
        }
    }
    return tmp;
}

int init(int game_id, int32_t zone_id, int32_t svr_id, int32_t site_id){
    logger.init(game_id,zone_id,svr_id,site_id,1);
    isInit = 1;
    return 0;
}

int online_count(int cnt, int zoneid){
    if(isInit != 1){
        return -1;
    }
    bool isEmpty = false;
    if(zoneid == -1){
        isEmpty = true;
    }
    logger.online_count(cnt,type2string(zoneid,isEmpty));
    return 0;
}

int reg_role(int uid, uint32_t cli_ip,char* browser,char* os,char* ads_id){
    if(isInit != 1){
        return -1;
    }
    logger.reg_role(type2string(uid),"","",cli_ip,ads_id,browser,"",os,"","","");
    return 0;
}

int verify_passwd(int uid,uint32_t cli_ip){
    if(isInit != 1){
        return -1;
    }
    logger.verify_passwd(type2string(uid),cli_ip,"","","","","","","");
    return 0;
}

int login_online(int uid,int lv, uint32_t cli_ip,char* browser,char* os,char* ads_id){
    if(isInit != 1){
        return -1;
    }
    logger.login_online(type2string(uid),"","",false,lv,cli_ip,ads_id,"",browser,"",os,"","","");
    return 0;
}

int logout(int uid , int oltime){
    if(isInit != 1){
        return -1;
    }
    logger.logout(type2string(uid),false,0,oltime);
    return 0;
}

int level_up(int uid, int lv){
    if(isInit != 1){
        return -1;
    }
    logger.level_up(type2string(uid),"",lv);
    return 0;
}

int pay(int uid, bool isvip, float pay_amount, int outcome_id, int outcnt){
    if(isInit != 1){
        return -1;
    }
    logger.pay(type2string(uid), isvip, pay_amount, StatLogger::ccy_mibi, StatLogger::pay_buy, type2string(outcome_id), outcnt, "1");
    return 0;
}

int accept_task(int uid, TaskType type, int taskid){
    if(isInit != 1){
        return -1;
    }
    logger.accept_task(static_cast<StatLogger::TaskType>(type),type2string(uid),type2string(taskid),0);
    return 0;
}

int finish_task(int uid, TaskType type, int taskid){
    if(isInit != 1){
        return -1;
    }
    logger.finish_task(static_cast<StatLogger::TaskType>(type),type2string(uid),type2string(taskid),0);
    return 0;
}

int abort_task(int uid, TaskType type, int taskid){
    if(isInit != 1){
        return -1;
    }
    logger.abort_task(static_cast<StatLogger::TaskType>(type),type2string(uid),type2string(taskid),0);
    return 0;
}

int obtain_golds(int uid, int amt) {
    if(isInit != 1){
        return -1;
    }
    logger.obtain_golds(type2string(uid), amt);
    return 0;
}

int buy_item(int uid, bool isvip, int lv, float pay_amount, int itemid, int outcnt) {
    //printf("isInit=%u uid=%d isvip=%u lv=%d pay_amout=%f itemid=%d outcnt=%d\n",
    //        isInit, uid, isvip, lv, pay_amount, itemid, outcnt);
    if(isInit != 1){
    //    printf("not inited\n");
        return -1;
    }
    logger.buy_item(type2string(uid), isvip, lv, pay_amount, type2string(itemid), outcnt);
    return 0;
}

int use_golds(int uid, bool isvip, const char* reason, float pay_amount, int lv) {
    if(isInit != 1){
        return -1;
    }
    logger.use_golds(type2string(uid), isvip, reason, pay_amount, lv);
    return 0;
}

int new_trans(int uid, NewTransStep step){
    if(isInit != 1){
        return -1;
    }
    logger.new_trans(static_cast<StatLogger::NewTransStep>(step),type2string(uid));
    return 0;
}

int selflog(int uid,char *stat_name, char *sub_stat_name){
    if(isInit != 1){
        return -1;
    }
    logger.log(stat_name,sub_stat_name,type2string(uid),"");
    return 0;
}

int selfitemlog(int uid, char *stat_name, char *sub_stat_name, char* key,char* value,OpCode op){
    if(isInit != 1){
        return -1;
    }
    if(op == op_item_sum || op == op_item_max || op == op_item_set){
        return -1;
    }
    StatInfo info;
    info.add_info(key,value);
    info.add_op(static_cast<StatInfo::OpCode>(op),key);
    logger.log(stat_name,sub_stat_name,type2string(uid),"",info);
    return 0;
}

//int selfitemlogWithFormat(int uid, char *stat_name, char *sub_stat_name, char* format){
    //if(isInit != 1){
        //return -1;
    //}
    //std::vector<key_val_op> result = split(format);
    //StatInfo info;
    //std::vector<key_val_op>::iterator it = result.begin();
    //for(; it != result.end(); ++it) {
        //info.add_info(it->key,it->val);
        //info.add_op(static_cast<StatInfo::OpCode>(it->op),it->key);
    //}
    //logger.log(stat_name,sub_stat_name,type2string(uid),"",info);
    //return 0;
//}
