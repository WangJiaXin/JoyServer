/*
 * Bussines.h
 *
 */

#ifndef BUSINESS_H_
#define BUSINESS_H_
#define BOOST_DATE_TIME_SOURCE
//#define BOOST_DATE_TIME_NO_LIB
#include <mysql.h>
#include <string.h>
#include <glog/logging.h>
#include "../src/Code.h"
#include "../src/profile_config.h"
#include "../src/Json.hpp"


#include <boost/date_time/posix_time/posix_time.hpp>

using namespace bas;

namespace bus {


enum RET_CODE { //定义各种返回值
	RET_NORMAL				= 0,	//正常
	RET_DB_QUERY_ERR		= 10,	//数据库查询异常
	RET_DB_UPDATE_ERR,				//数据库更新异常
	RET_DB_INSERT_ERR,				//数据库插入异常
	RET_DB_DELETE_ERR,				//数据库删除异常
	RET_DB_EXEC_PROC_ERR,			//执行存储过程异常
	RET_DB_QUERY_PROC_ERR,			//执行存储过程异常
	RET_DB_NULL_ROW,				//数据库查询返回空行
	RET_DB_MULTI_ROW,				//数据库查询返回多行
	RET_EXISTS				= 20,	//存在
	RET_NOT_EXISTS,					//不存在
	RET_EXISTS_MULTI,				//存在多个
	RET_PARAM_ERR			= 30,	//发送包参数错误
	RET_CSERVER_ERR,				//客户端通信异常
	RET_PROTOCOL_ERR,				//协议操作异常
	RET_UNKNOWN_REALM,              //未知的分区
	RET_CHECK_ERR			= 40,	//服务器端数据与客户端数据校验失败

	RET_LOGIN_OTHER         = 60,   //别处登录
	RET_RECONN_PROTOCOL     = 61,   //同一个协议同时登录
	RET_CHECK_ERR_RELOGIN   = 62,   //资源校验失败,客户端要求重新启动
	
	RET_CDKEY_ERR           = 70,   //CDKEY不合法
	RET_CDKEY_ACTIVATED     = 71,   //CDKEY已激活
	RET_CDKEY_ACTIVATED_PRE = 72,   //已激活过同类型的CDKEY
	
	RET_BOSS_TIME_INVALID   = 80,   //世界boss时间未到


	RET_MAX_CODE
};

enum EVENT_TYPE
{
	
	TEST_EVENT          = 1333, 
};
       
class Process;
class BusInf
{
    public:
        virtual void* getQry() = 0;
        virtual void* getAck() = 0;
        virtual int process(Process*) = 0;
        virtual void db_done(const int, void *);
        virtual ~BusInf(){};
        BusInf():_tools(0){};
        void peer_addr(const char *ip, const int port){
            if (ip == 0) return ;
            strncpy(_peer_ip, ip, sizeof(_peer_ip));
            _peer_port = port;
        }
        void cmd(int command){
            _cmd = command;
        }

        Process *_tools;

    protected:
        char _peer_ip[16];
        int  _peer_port;
        int  _cmd;
};

template<typename Qry, typename Ack>
class Business : public BusInf
{
    public:
        virtual void* getQry() {return &_qry;}
        virtual void* getAck() {return &_ack;}
        virtual int process(Process*){return 0;}

        virtual ~Business(){
            std::string strTime = boost::posix_time::to_iso_string(
                    boost::posix_time::second_clock::local_time());
            _js << Pair("log_time", strTime) <<
                    Pair("ip", _peer_ip) <<
                    Pair("port", _peer_port) <<
                    Pair("cmd", _cmd) << 
                    Pair("sub_cmd_id", 0); //default sub_cmd_id
            VLOG(L_RP) << _js.Str();
        }
        Business(){}
        void printQry() { VLOG(L_DT) << "Qry Body >>>>> " << _qry.Str(); }
        void printAck() { VLOG(L_DT) << "Ack Body >>>>> " << _ack.Str(); }

    protected:
        Qry _qry;
        Ack _ack;
        Json _js;
};

} /* namespace BUS */
#endif /* BUSINESS_H_ */
