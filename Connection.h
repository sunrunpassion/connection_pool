#pragma once
#include<mysql.h>
#include<string>
#include<ctime>
using namespace std;
/*
实现mysql的增删改查等操作
第一步添加，配置好相应的库路径和库名
拷贝动态库到当前的工程目录下
*/
class Connection
{
public:
	//初始化数据库连接
	Connection();
	//释放数据库连接资源
	~Connection();
	//连接数据库
	bool connect(string ip,
		unsigned short port,
		string user,
		string password,
		string dbname);
	//更新操作 update,insert,delete
	bool update(string sql);
	//查询操作 select
	void query(string sql);
	void refreshAliveTime()//刷新一下起始的空闲时间点
	{
		_alivetime = clock();
	}
	clock_t getAliveTime()const//返回存活时间
	{
		return clock() - _alivetime;
	}
private:
	MYSQL* _conn;//表示和MySql Server的一条连接
	clock_t _alivetime;//记录进入空闲状态后的时间
};
