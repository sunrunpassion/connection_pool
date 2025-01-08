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
ʵ�����ӳع���ģ��
�������һ�ξͿ����ˣ����Ϊ����ģʽ
�������Ƕ��̣߳����̶߳��������ӳأ��������Ϊ�̰߳�ȫ��
*/
class Connection_Pool
{
public:
	//��ȡ���ӳض���ʵ��
	static Connection_Pool* getConnection_Pool();
	//���ⲿ�ṩ�ӿڣ������ӳػ�ȡһ�����õĿ�������
	shared_ptr<Connection> getConnection();//�����ü���
private:
	Connection_Pool();//���캯��˽�л�
	bool loadConfigFile();//���������ļ�
	//�����ڶ������߳���ר������������������ӣ�������ʳ�Ա����
	void produceConnectionTask();
	void scanConnectionTask();
	string _ip;//MySQLip��ַ
	unsigned short _port;//MySQL�˿ں�
	string _user;//MySQL�û���
	string _password;//MySQL��¼����
	string _dbname;//MySQL����
	int _initSize;//���ӳس�ʼ������
	int _maxSize;//���ӳص����������
	int _maxIdletime;//���ӳ�������ʱ��
	int _connectionTimeout;//���ӳػ�ȡ�ĳ�ʱʱ��
	queue<Connection*> _connectionQue;//��Ҫ������ָ��,�洢MySQL���ӵĶ���
	//���еĲ���һ��Ҫ���̰߳�ȫ��
	mutex _queueMutex;//ά�����Ӷ��е��̰߳�ȫ
	atomic_int _connectionCnt;//��¼���ӵ�����������ͨ���εı��������̰߳�ȫ�ģ�atomic����CAS
	condition_variable cv;//�������������������������������̵߳�ͨ��
};