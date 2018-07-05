// 被注入的测试程序.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include<windows.h>
int main()
{
	for (int i = 0;i < 10;i++) {
		printf("------\n");
	}
	getchar();
	for (int i = 0;i < 10;i++) {
		printf("------\n");
	}
	system("pause");
    return 0;
}

