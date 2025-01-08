#include "Connection.h"
#include "Public.h"

Connection::Connection()
{
	_conn = mysql_init(nullptr);//动态库不能转到定义，本质就是开辟内存
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
	MYSQL* p = mysql_real_connect(_conn, ip.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0);//string 转到cosnt cahr*
	return p != nullptr;
}

bool Connection::update(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("更新失败:" + sql);
		return false;
	}
	return true;
}

void Connection::query(string sql)
{//具体演示了怎么对select结果进行输出
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("查询失败:" + sql);
	}
	MYSQL_RES* result=mysql_use_result(_conn);;//结果集对象
	if (result == nullptr)
	{
		cerr << mysql_error(_conn);//mysql_error()	返回最后一次错误的错误信息。
	}
	else
	{
		MYSQL_FIELD * fields=mysql_fetch_fields(result);//fetch是获取的意思
		MYSQL_ROW row;
		int num_fields = mysql_num_fields(result);
		//输出列名
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
