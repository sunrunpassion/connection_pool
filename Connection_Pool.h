#pragma once
#include<string>
#include<queue>
#include<mutex>
#include<fstream>
#include<map>
#include<atomic>
#include<thread>
#include<memory>
#include<functional>
#include<condition_variable>
#include"Connection.h"
using namespace std;
/*
实现连接池功能模块
对象出现一次就可以了，设计为单例模式
服务器是多线程，多线程都会来连接池，所以设计为线程安全的
*/
class Connection_Pool
{
public:
	//获取连接池对象实例
	static Connection_Pool* getConnection_Pool();
	//给外部提供接口，从连接池获取一个可用的空闲连接
	shared_ptr<Connection> getConnection();//带引用计数
private:
	Connection_Pool();//构造函数私有化
	bool loadConfigFile();//加载配置文件
	//运行在独立的线程中专门用来负责产生新连接，方便访问成员变量
	void produceConnectionTask();
	void scanConnectionTask();
	string _ip;//MySQLip地址
	unsigned short _port;//MySQL端口号
	string _user;//MySQL用户名
	string _password;//MySQL登录密码
	string _dbname;//MySQL引擎
	int _initSize;//连接池初始连接量
	int _maxSize;//连接池的最大连接量
	int _maxIdletime;//连接池最大空闲时间
	int _connectionTimeout;//连接池获取的超时时间
	queue<Connection*> _connectionQue;//不要拷贝，指针,存储MySQL连接的队列
	//队列的操作一定要是线程安全的
	mutex _queueMutex;//维护连接队列的线程安全
	atomic_int _connectionCnt;//记录连接的总数量，普通整形的变量不是线程安全的，atomic基于CAS
	condition_variable cv;//设置条件变量用于连接生产和消费线程的通信
};