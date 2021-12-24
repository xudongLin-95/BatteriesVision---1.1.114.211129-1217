#pragma once
class DHInterface
{
public:
	virtual int DoSomething(void* message,void* wParam,void* lParam) = 0;
	virtual ~DHInterface(){};// 抽象类的虚析构函数
};


