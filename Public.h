#pragma once
#include<iostream>
using namespace std;
/*
简单封装日志：文件，行号，时间，都是已经写好的好的宏*/
//\用于将宏拓展到多行
#define LOG(str) \
cout<<__FILE__<<":"<<__LINE__<<" "<<\
__TIMESTAMP__<<":"<<str<<endl;