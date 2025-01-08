#pragma once
#include<mysql.h>
#include<string>
#include<ctime>
using namespace std;
/*
ʵ��mysql����ɾ�Ĳ�Ȳ���
��һ����ӣ����ú���Ӧ�Ŀ�·���Ϳ���
������̬�⵽��ǰ�Ĺ���Ŀ¼��
*/
class Connection
{
public:
	//��ʼ�����ݿ�����
	Connection();
	//�ͷ����ݿ�������Դ
	~Connection();
	//�������ݿ�
	bool connect(string ip,
		unsigned short port,
		string user,
		string password,
		string dbname);
	//���²��� update,insert,delete
	bool update(string sql);
	//��ѯ���� select
	void query(string sql);
	void refreshAliveTime()//ˢ��һ����ʼ�Ŀ���ʱ���
	{
		_alivetime = clock();
	}
	clock_t getAliveTime()const//���ش��ʱ��
	{
		return clock() - _alivetime;
	}
private:
	MYSQL* _conn;//��ʾ��MySql Server��һ������
	clock_t _alivetime;//��¼�������״̬���ʱ��
};
