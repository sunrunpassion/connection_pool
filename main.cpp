#include<iostream>
#include "Connection.h"
#include"Connection_Pool.h"
using namespace std;
int main()
{
	Connection conn_;
	conn_.connect("127.0.0.1", 3306, "root", "123456", "fengzhaoyang");//û�����ӳص��ڲ���ʱ���ܻᱼ����ͬһ���˺��ȵ�¼һ�£�Ҫ�ǻ��������ǲ����������������
	/*Connection conn;
	char sql[1024] = { 0 };
	sprintf(sql, "select * from student");
	//C++���ñȽϺõ�C���Ժ���ȥ�޸�C++������SDL���
	conn.connect("127.0.0.1", 3306, "root", "123456","itcast");//mysql�˿�Ϊ3306
	conn.query(sql);*/
	/*�������ӳص������ļ�
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
		conn.connect("127.0.0.1", 3306, "root", "123456", "fengzhaoyang");//mysql�˿�Ϊ3306
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
		conn.connect("127.0.0.1", 3306, "root", "123456", "fengzhaoyang");//mysql�˿�Ϊ3306
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
		conn.connect("127.0.0.1", 3306, "root", "123456", "fengzhaoyang");//mysql�˿�Ϊ3306
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
		conn.connect("127.0.0.1", 3306, "root", "123456", "fengzhaoyang");//mysql�˿�Ϊ3306
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
		conn.connect("127.0.0.1", 3306, "root", "123456", "fengzhaoyang");//mysql�˿�Ϊ3306
		conn.update(sql);
		/*shared_ptr<Connection> sp = cp->getConnection();
		string sql = "insert into user value(1,'sun')";
		sp->update(sql);*/
	}
#endif
	clock_t end = clock();
	cout << (end - begin) << "ms" << endl;//3559ms(δʹ��)=>1132ms
	return 0;
}