#include "Connection_Pool.h"
#include"public.h"
//线程安全的懒汉单例模式(懒汉使用时才构造）
Connection_Pool* Connection_Pool::getConnection_Pool()
{
	static Connection_Pool pool;//初始化会延迟到第一次执行到的时候，汇编上函数静态的初始化自动加锁了
	//static自动lock和unlock
	return &pool;
}

shared_ptr<Connection> Connection_Pool::getConnection()
{
	unique_lock<mutex> lock(_queueMutex);
	while(_connectionQue.empty())
	{//这块要是不这样处理，想想看要是A,B都阻塞在wait的情况，出来要先判断条件变量，而不是去拿锁
		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout)))
		{
			if (_connectionQue.empty())
			{
				LOG("获取空闲连接超时了，获取连接失败");
				return nullptr;
			}
		}
	}
	/*
	shared_ptr智能指针析构时候，会把connection资源释放
	调用connection析构函数，close掉
	要自定义释放资源方式归还到queue中
	share_ptr可以指定删除器（函数对象）
	*/
	shared_ptr<Connection> sp(_connectionQue.front(), [&](Connection* rback) {
		//这里在服务器应用线程中调用的，所以一定要考虑队列的线程安全，很多线程都会往里加
		unique_lock<mutex> lock(_queueMutex);//这里加是因为你在外面用这个去操作，最后析构时后要注意线程安全
		rback->refreshAliveTime();
		_connectionQue.push(rback);//就不用手动归还连接，让用户使用更简单
		});
	_connectionQue.pop();
	cv.notify_all();//谁消费了队列中的最后一个谁负责通知
	return sp;
}

Connection_Pool::Connection_Pool()
{
	//加载配置文件
	if (!loadConfigFile())
	{
		return;
	}
	//创建初始数量的连接,连接池初始化，连接池启动，还没有线程使用，不用考虑线程安全
	for (int i = 0; i < _initSize; ++i)
	{
		Connection* p = new Connection();
		p->connect(_ip, _port, _user, _password, _dbname);
		p->refreshAliveTime();//刷新一下开始空闲的时间
		_connectionQue.push(p);
		_connectionCnt++;
	}
	//启动一个生产者线程thread=>底层就是调用pthread_create,类中成员函数做线程函数要绑定器绑定对象
	thread produce(std::bind(&Connection_Pool::produceConnectionTask, this));
	produce.detach();//守护线程
	//启动一个新的定时线程，扫描空余时间超过maxIdleTime,进行连接回收
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
		unsigned int dex = line.find("=");//返回是无符号的
		//也可以用size_t接受，和string::npos比较
		if (dex == -1)//找不到返回的就是实际的-1，1111111111111，无符号看来就是很大一个值
		{
			continue;
		}
		else
		{
			key = line.substr(0, dex);
			value = line.substr(dex + 1);
			config[key] = value;
		}//switch后面必须是一个整数或者枚举类型
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
			cv.wait(lock);//队列不空，此处生产线程等待状态
		}
		//连接数量没有到达上限继续创建新的连接
		if (_connectionCnt < _maxSize)
		{
			Connection* p = new Connection();
			p->connect(_ip, _port, _user, _password, _dbname);
			p->refreshAliveTime();
			_connectionQue.push(p);
			_connectionCnt++;
		}
		//通知消费者可以连接了
		cv.notify_all();
	}
}

void Connection_Pool::scanConnectionTask()
{
	while (1)
	{
		//通过sleep模拟定时效果
		this_thread::sleep_for(chrono::seconds(_maxIdletime));
		//扫描整个队列，释放多余的连接
		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize)
		{
			Connection* p = _connectionQue.front();
			if (p->getAliveTime() >= (_maxIdletime * CLOCKS_PER_SEC))
			{
				_connectionQue.pop();
				_connectionCnt--;
				delete p;//调用~Connection（）释放连接,对头一定是时间最久的
			}
			else
			{
				break;
			}
		}
	}
}
