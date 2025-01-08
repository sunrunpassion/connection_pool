#include "Connection.h"
#include "Public.h"

Connection::Connection()
{
	_conn = mysql_init(nullptr);//��̬�ⲻ��ת�����壬���ʾ��ǿ����ڴ�
}

Connection::~Connection()
{
	if (_conn != nullptr)
	{
		mysql_close(_conn);
	}
}

bool Connection::connect(string ip, unsigned short port, string user, string password, string dbname)
{
	MYSQL* p = mysql_real_connect(_conn, ip.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0);//string ת��cosnt cahr*
	return p != nullptr;
}

bool Connection::update(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("����ʧ��:" + sql);
		return false;
	}
	return true;
}

void Connection::query(string sql)
{//������ʾ����ô��select����������
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("��ѯʧ��:" + sql);
	}
	MYSQL_RES* result=mysql_use_result(_conn);;//���������
	if (result == nullptr)
	{
		cerr << mysql_error(_conn);//mysql_error()	�������һ�δ���Ĵ�����Ϣ��
	}
	else
	{
		MYSQL_FIELD * fields=mysql_fetch_fields(result);//fetch�ǻ�ȡ����˼
		MYSQL_ROW row;
		int num_fields = mysql_num_fields(result);
		//�������
		for (int i = 0; i < num_fields; i++)
		{
			cout << fields[i].name;
			cout << '\t';
		}
		cout << endl;
		while ((row = mysql_fetch_row(result)))
		{
			for (int i = 0; i < num_fields; i++)
			{
				cout << (row[i] ? row[i] : "NULL");
				cout << '\t';
			}
			cout << endl;
		}
		mysql_free_result(result);
	}
}
