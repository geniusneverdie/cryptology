// work1yiweimima.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<iostream>
#include<string>
using namespace std;
void attack()
{
	cin.ignore();
	cout << "请输入需要攻击的字符串" << endl;
	string str;
	getline(cin, str);
	for (int j = 1; j < 26; j++)
	{
		for (int i = 0; i < str.length(); i++)
		{
			if (64 < str[i] && str[i] < 91)
			{

				str[i] = str[i] - 1;
				if (str[i] < 65)
					str[i] += 26;

			}
			else
			{
				if (96 < str[i] && str[i] < 123)
				{
					str[i] = str[i] - 1;
					if (str[i] < 97)
						str[i] += 26;
				}
			}
		}
		cout << j << ":  " << str << endl;
	}
}
void lock()
{
	cout << "请选择你的秘钥" << endl;
	int a;
	cin >> a;
	cin.ignore();
	a = a % 26;
	cout << "请输入需要加密的字符串" << endl;
	string str;
	getline(cin, str);
	for (int i = 0; i < str.length(); i++)
	{
		if (64 < str[i] && str[i] < 91)
		{
			str[i] = (str[i] + a) % 91;
			if (str[i] < 65)
				str[i] += 65;
		}
		else
		{
			if (96 < str[i] && str[i] < 123)
			{
				str[i] = (str[i] + a) % 123;
				if (str[i] < 97)
					str[i] += 97;
			}
		}
	}
	cout << str << endl;
}
void unlock()
{
	cout << "请选择你的秘钥" << endl;
	int a;
	cin >> a;
	cin.ignore();
	a = a % 26;
	cout << "请输入需要解密的字符串" << endl;
	string str;
	getline(cin, str);

	for (int i = 0; i < str.length(); i++)
	{
		if (64 < str[i] && str[i] < 91)
		{
			str[i] = str[i] - a;
			if (str[i] < 65)
				str[i] += 26;
		}
		else
		{
			if (96 < str[i] && str[i] < 123)
			{
				str[i] = str[i] - a;
				if (str[i] < 97)
					str[i] += 26;
			}
		}
	}
	cout << str << endl;
}
int main()
{
	while (true)
	{
		cout << "输入1加密，输入2解密，输入3攻击";
		int a;
		cin >> a;
		switch (a)
		{
		case 1:
			lock();
			break;
		case 2:
			unlock();
			break;
		case 3:
			attack();
		}
	}
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
