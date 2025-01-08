#include<iostream>
#include "Connection.h"
#include"Connection_Pool.h"
using namespace std;
int main()
{
	Connection conn_;
	conn_.connect("127.0.0.1", 3306, "root", "123456", "fengzhaoyang");//没有链接池的在并发时可能会奔溃，同一个账号先登录一下，要是还奔溃就是并发插入的性能问题
	/*Connection conn;
	char sql[1024] = { 0 };
	sprintf(sql, "select * from student");
	//C++想用比较好的C语言函数去修改C++常规中SDL检查
	conn.connect("127.0.0.1", 3306, "root", "123456","itcast");//mysql端口为3306
	conn.query(sql);*/
	/*测试连接池的配置文件
	Connection_Pool* cp = Connection_Pool::getConnection_Pool();
	cp->loadConfigFile();
	cout << cp->_ip<<" "<<cp->_port<<" "<<cp->_user<<" "<<cp->_maxIdletime;*/
	/*Connection_Pool* cp = Connection_Pool::getConnection_Pool();
	shared_ptr<Connection> sp=cp->getConnection();
	string sql = "insert into user value(1,'sun')";
	sp->update(sql);
	shared_ptr<Connection> sp2 = cp->getConnection();
	sql="select * from user";
	sp2->query(sql);
	sql = "delete from user where id=1";
	sp2->update(sql);
	sql = "select * from user";
	sp->query(sql);*/
	clock_t begin = clock();
	//Connection_Pool* cp = Connection_Pool::getConnection_Pool();
	thread t1([]() {for (int i = 0; i < 2500; ++i)//1079
	{
		//Connection_Pool* cp = Connection_Pool::getConnection_Pool();
		Connection conn; //9888ms
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user value(1,'sun')");
		conn.connect("127.0.0.1", 3306, "root", "123456", "fengzhaoyang");//mysql端口为3306
		conn.update(sql);
		/*shared_ptr<Connection> sp = cp->getConnection();
		string sql = "insert into user value(1,'sun')";
		sp->update(sql);*/
	}});
	thread t2([]() {for (int i = 0; i < 2500; ++i)//1079
	{
		//Connection_Pool* cp = Connection_Pool::getConnection_Pool();
		Connection conn; //9888ms
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user value(1,'sun')");
		conn.connect("127.0.0.1", 3306, "root", "123456", "fengzhaoyang");//mysql端口为3306
		conn.update(sql);
		/*shared_ptr<Connection> sp = cp->getConnection();
		string sql = "insert into user value(1,'sun')";
		sp->update(sql);*/
	}});
	thread t3([]() {for (int i = 0; i < 2500; ++i)//1079
	{
		//Connection_Pool* cp = Connection_Pool::getConnection_Pool();
		Connection conn; //9888ms
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user value(1,'sun')");
		conn.connect("127.0.0.1", 3306, "root", "123456", "fengzhaoyang");//mysql端口为3306
		conn.update(sql);
		/*shared_ptr<Connection> sp = cp->getConnection();
		string sql = "insert into user value(1,'sun')";
		sp->update(sql);*/
	}});
	thread t4([]() {for (int i = 0; i < 2500; ++i)//1079
	{
		//Connection_Pool* cp = Connection_Pool::getConnection_Pool();
		Connection conn; //9888ms
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user value(1,'sun')");
		conn.connect("127.0.0.1", 3306, "root", "123456", "fengzhaoyang");//mysql端口为3306
		conn.update(sql);
		/*shared_ptr<Connection> sp = cp->getConnection();
		string sql = "insert into user value(1,'sun')";
		sp->update(sql);*/
	}});
	t1.join();
	t2.join();
	t3.join();
	t4.join();
#if 0
	for (int i = 0; i < 50000; ++i)//1079
	{
		Connection conn; //9888ms
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user value(1,'sun')");
		conn.connect("127.0.0.1", 3306, "root", "123456", "fengzhaoyang");//mysql端口为3306
		conn.update(sql);
		/*shared_ptr<Connection> sp = cp->getConnection();
		string sql = "insert into user value(1,'sun')";
		sp->update(sql);*/
	}
#endif
	clock_t end = clock();
	cout << (end - begin) << "ms" << endl;//3559ms(未使用)=>1132ms
	return 0;
}