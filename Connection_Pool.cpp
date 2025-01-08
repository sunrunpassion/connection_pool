#include "Connection_Pool.h"
#include"public.h"
//�̰߳�ȫ����������ģʽ(����ʹ��ʱ�Ź��죩
Connection_Pool* Connection_Pool::getConnection_Pool()
{
	static Connection_Pool pool;//��ʼ�����ӳٵ���һ��ִ�е���ʱ�򣬻���Ϻ�����̬�ĳ�ʼ���Զ�������
	//static�Զ�lock��unlock
	return &pool;
}

shared_ptr<Connection> Connection_Pool::getConnection()
{
	unique_lock<mutex> lock(_queueMutex);
	while(_connectionQue.empty())
	{//���Ҫ�ǲ������������뿴Ҫ��A,B��������wait�����������Ҫ���ж�����������������ȥ����
		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout)))
		{
			if (_connectionQue.empty())
			{
				LOG("��ȡ�������ӳ�ʱ�ˣ���ȡ����ʧ��");
				return nullptr;
			}
		}
	}
	/*
	shared_ptr����ָ������ʱ�򣬻��connection��Դ�ͷ�
	����connection����������close��
	Ҫ�Զ����ͷ���Դ��ʽ�黹��queue��
	share_ptr����ָ��ɾ��������������
	*/
	shared_ptr<Connection> sp(_connectionQue.front(), [&](Connection* rback) {
		//�����ڷ�����Ӧ���߳��е��õģ�����һ��Ҫ���Ƕ��е��̰߳�ȫ���ܶ��̶߳��������
		unique_lock<mutex> lock(_queueMutex);//���������Ϊ�������������ȥ�������������ʱ��Ҫע���̰߳�ȫ
		rback->refreshAliveTime();
		_connectionQue.push(rback);//�Ͳ����ֶ��黹���ӣ����û�ʹ�ø���
		});
	_connectionQue.pop();
	cv.notify_all();//˭�����˶����е����һ��˭����֪ͨ
	return sp;
}

Connection_Pool::Connection_Pool()
{
	//���������ļ�
	if (!loadConfigFile())
	{
		return;
	}
	//������ʼ����������,���ӳس�ʼ�������ӳ���������û���߳�ʹ�ã����ÿ����̰߳�ȫ
	for (int i = 0; i < _initSize; ++i)
	{
		Connection* p = new Connection();
		p->connect(_ip, _port, _user, _password, _dbname);
		p->refreshAliveTime();//ˢ��һ�¿�ʼ���е�ʱ��
		_connectionQue.push(p);
		_connectionCnt++;
	}
	//����һ���������߳�thread=>�ײ���ǵ���pthread_create,���г�Ա�������̺߳���Ҫ�����󶨶���
	thread produce(std::bind(&Connection_Pool::produceConnectionTask, this));
	produce.detach();//�ػ��߳�
	//����һ���µĶ�ʱ�̣߳�ɨ�����ʱ�䳬��maxIdleTime,�������ӻ���
	thread scanner(std::bind(&Connection_Pool::scanConnectionTask, this));
	scanner.detach();
}

bool Connection_Pool::loadConfigFile()
{
	map<string, string> config;
	ifstream configfile("mysql.ini");
	if (!configfile.is_open())
	{
		LOG("mysql.ini file is not exist!");
		return false;
	}
	string line;
	string key;
	string value;
	while (getline(configfile, line))
	{
		unsigned int dex = line.find("=");//�������޷��ŵ�
		//Ҳ������size_t���ܣ���string::npos�Ƚ�
		if (dex == -1)//�Ҳ������صľ���ʵ�ʵ�-1��1111111111111���޷��ſ������Ǻܴ�һ��ֵ
		{
			continue;
		}
		else
		{
			key = line.substr(0, dex);
			value = line.substr(dex + 1);
			config[key] = value;
		}//switch���������һ����������ö������
		if (key == "ip")
		{
			_ip = config[key];
		}
		else if (key == "port")
		{
			_port = stoi(config[key]);
		}
		else if (key == "user")
		{
			_user = config[key];
		}
		else if (key == "password")
		{
			_password = config[key];
		}
		else if (key == "dbname")
		{
			_dbname = config[key];
		}
		else if (key == "initSize")
		{
			_initSize = stoi(config[key]);
		}
		else if (key == "maxSize")
		{
			_maxSize = stoi(config[key]);
		}
		else if (key == "maxIdletime")
		{
			_maxIdletime = stoi(config[key]);
		}
		else if (key == "connectionTimeout")
		{
			_connectionTimeout = stoi(config[key]);
		}
	}
	configfile.close();
	return true;
}

void Connection_Pool::produceConnectionTask()
{
	while(1)
	{
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionQue.empty())
		{
			cv.wait(lock);//���в��գ��˴������̵߳ȴ�״̬
		}
		//��������û�е������޼��������µ�����
		if (_connectionCnt < _maxSize)
		{
			Connection* p = new Connection();
			p->connect(_ip, _port, _user, _password, _dbname);
			p->refreshAliveTime();
			_connectionQue.push(p);
			_connectionCnt++;
		}
		//֪ͨ�����߿���������
		cv.notify_all();
	}
}

void Connection_Pool::scanConnectionTask()
{
	while (1)
	{
		//ͨ��sleepģ�ⶨʱЧ��
		this_thread::sleep_for(chrono::seconds(_maxIdletime));
		//ɨ���������У��ͷŶ��������
		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize)
		{
			Connection* p = _connectionQue.front();
			if (p->getAliveTime() >= (_maxIdletime * CLOCKS_PER_SEC))
			{
				_connectionQue.pop();
				_connectionCnt--;
				delete p;//����~Connection�����ͷ�����,��ͷһ����ʱ����õ�
			}
			else
			{
				break;
			}
		}
	}
}
