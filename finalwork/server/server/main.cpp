// server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "iostream"
#include "Server.h"

using namespace std;

#include <iostream>
#include <string>
#include <regex> // 正则表达式

using namespace std;

// 定义用户结构体
struct User {
    string name;
    string idCardNo;
    string phoneNo;
};

// 核对用户信息函数
bool verifyUser(User u) {
    bool isValid = true; // 核对结果
    regex idCardPattern(R"(^\d{18}$)"); // 身份证号正则表达式：18位数字
    regex phonePattern(R"(^1[0-9]{10}$)"); // 手机号正则表达式：11位数字，以1开头

    // 核对身份证号
    if (!regex_match(u.idCardNo, idCardPattern)) {
        cout << "身份证号输入有误！" << endl;
        isValid = false;
    }

    // 核对手机号码
    if (!regex_match(u.phoneNo, phonePattern)) {
        cout << "手机号输入有误！" << endl;
        isValid = false;
    }

    // 如果核对通过，输出用户信息
    if (isValid) {
        cout << "用户信息核对通过。姓名：" << u.name << "，身份证号：" << u.idCardNo << "，手机号码：" << u.phoneNo << endl;
    }

    return isValid;
}

// 注册新用户函数
void registerUser() {
    User u;
    cout << "请输入姓名：";
    cin >> u.name;
    cout << "请输入身份证号码：";
    cin >> u.idCardNo;
    cout << "请输入手机号码：";
    cin >> u.phoneNo;

    // 核对用户信息
    verifyUser(u);
}



int main() {
    char mode[10] = { 0 };
    printf("Server:\n");
    while (true) {
        registerUser(); // 注册新用户
        runServer();
        break;
    }
    system("pause");
    return 1;
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
