#pragma once
#include<iostream>
using namespace std;
/*
�򵥷�װ��־���ļ����кţ�ʱ�䣬�����Ѿ�д�õĺõĺ�*/
//\���ڽ�����չ������
#define LOG(str) \
cout<<__FILE__<<":"<<__LINE__<<" "<<\
__TIMESTAMP__<<":"<<str<<endl;