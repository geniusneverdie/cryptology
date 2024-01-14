#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
#include <cstring>

using namespace std;

// 用于只选取低8bit
#define MOD 256

// s盒
static const int S[16][16] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// 逆s盒
static const int S1[16][16] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

// 轮常数，密钥扩展中用到（AES-128只需要10轮）
static const long long Rcon[10] = {
    0x01000000, 0x02000000,
    0x04000000, 0x08000000,
    0x10000000, 0x20000000,
    0x40000000, 0x80000000,
    0x1b000000, 0x36000000
};

int HexToBit[16][4] = { {0,0,0,0},{0,0,0,1},{0,0,1,0},{0,0,1,1},
                        {0,1,0,0},{0,1,0,1},{0,1,1,0},{0,1,1,1},
                        {1,0,0,0},{1,0,0,1},{1,0,1,0},{1,0,1,1},
                        {1,1,0,0},{1,1,0,1},{1,1,1,0},{1,1,1,1}
};

// 将string类型转换为bits类型，方便后续的雪崩效应
void getbit(string a, bitset<128>& temp)
{
    int num = 127;
    for (int i = 2; i < a.length(); i++)
    {
        if (a[i] <= '9')
        {
            for (int j = 0; j < 4; j++)
            {
                temp[num--] = HexToBit[a[i] - 48][j];
            }
        }
        else
        {
            for (int j = 0; j < 4; j++)
            {
                temp[num--] = HexToBit[a[i] - 65 + 10][j];
            }
        }
    }
}

// 字符 --> 数字
int ch_to_int(char& ch)
{
    int ans = 0;
    // 数字的时候
    if (ch >= 48 && ch <= 57)
    {
        ans = ch - '0';
    }
    // 16进制中a到f
    else if (ch >= 'a' && ch <= 'f')
    {
        ans = ch - 'a' + 10;
    }
    // 16进制中A到F
    else if (ch >= 'A' && ch <= 'F')
    {
        ans = ch - 'A' + 10;
    }
    return ans;
}

// 16进制字符串 --> 数字
long long str_long(string str)
{
    long long ans = 0;
    // 遍历字符串，将字符串的内容变为16进制数字
    for (char ch : str)
    {
        ans = ans * 16 + ch_to_int(ch);
    }
    return ans;
}



// 数字 --> 16进制字符串 (只考虑小写)
string int_to_chs(long long num)
{
    string ans = "";
    while (num)
    {
        // 通过位运算得到低四位
        int x = num & 0xf;
        // 根据数值进行区分
        if (x <= 9)
        {
            char ch = x + '0';
            ans += ch;
        }
        else
        {
            char ch = x - 10 + 'a';
            ans += ch;
        }
        // 移位，其实相当于 / 16
        num >>= 4;
    }
    // 然后反转字符
    int left = 0, right = ans.length() - 1;
    // 双指针实现字符串反转
    while (left < right)
    {
        char  ch = ans[left];
        ans[left] = ans[right];
        ans[right] = ch;
        left++;
        right--;
    }
    return ans;
}

// 由于需要得新增一个函数使得能将128进制的char *类型
// 转换成16进制的字符串，也就是将每8位转换为每4位，方
// 直接使用这个返回的字符串进行操作   --zyl
string charkey_to_strkey(char* char_key)
{
    string str_key;
    char mid_key[32];
    // 先将其转换为32字节（每一个字符是一个字节）的char *类型
    // 匹配之前写的程序里的格式
    for (int w = 0; w < 16; w++)
    {
        long long eight_bits = char_key[w];
        mid_key[w * 2] = eight_bits >> 4;
        mid_key[w * 2 + 1] = eight_bits % 16;
    }
    // 然后将char *转换为string
    str_key = mid_key;
    return str_key;
}

// 得先分组
vector<string> group_key(string& key)
{
    // 四组
    vector<string> groups(4);
    // 初始下标
    int index = 0;
    // 分组
    for (string& g : groups)
    {
        g = key.substr(index, 8);
        index += 8;
    }
    return groups;
}

// 字节循环的实现
string loop_wordbyte(string& wi_1)
{
    string ans = wi_1.substr(2) + wi_1.substr(0, 2);
    return ans;
}

// 字节代换的实现
string wordbyte_sub(string& wi_1)
{
    int len = wi_1.length();
    string ans = "";
    for (int i = 0; i < len; i += 2)
    {
        // 先获取当前的下标
        int x = ch_to_int(wi_1[i]), y = ch_to_int(wi_1[i + 1]);
        // 然后获取当前的数字
        int num = S[x][y];
        // 先将数值转化为字符串
        string s = int_to_chs(num);
        // 然后不足的话补0
        while (s.length() < 2)
        {
            s = "0" + s;
        }

        // 加起来
        ans += s;
    }
    return ans;
}

// 轮常量异或的实现
string xor_with_const(string& wi_1, int rounds)
{
    // 先将字符串变为数字
    long long num = 0;
    for (int i = 0; i < 8; ++i)
    {
        char ch = wi_1[i];
        num = num * 16 + ch_to_int(ch);
    }
    // 计算异或结果
    num ^= Rcon[rounds];

    // 将num转化为字符串
    string res = int_to_chs(num);
    while (res.length() < 8)
    {
        res = "0" + res;
    }
    return res;
}


// 拓展的时候，下标为4的倍数时，需要麻烦一些，需要使用一个变换的T函数
string T(string& wi_1, int round)
{
    // T 变换由3部分构成，用的即为上述描述的三个函数
    // 先进行字循环
    string ans = loop_wordbyte(wi_1);
    // 然后字节代换
    ans = wordbyte_sub(ans);
    // 最后是轮异或
    ans = xor_with_const(ans, round);

    return ans;
}


// 写一个字符串对应的16进制数异或的函数
string string_xor(string s1, string s2)
{
    long long num1 = str_long(s1), num2 = str_long(s2);
    long long num = num1 ^ num2;
    // 再把数字转为字符串
    string ans = int_to_chs(num);
    // 不足8位的时候补位
    while (ans.length() < 8)
    {
        ans = "0" + ans;
    }
    return ans;
}

// 拓展密钥
vector<string> extend_key(string& key)
{
    // 先分组
    vector<string> w_key = group_key(key);
    for (int i = 0; i < 40; ++i)
    {
        string w = "";
        int index = 4 + i;
        string temp = w_key[index - 1];
        // 4 的倍数的时候，需要调用T函数
        if (index % 4 == 0)
        {
            temp = T(temp, index / 4 - 1);
        }
        w = string_xor(temp, w_key[index - 4]);

        // 压入数组中
        w_key.push_back(w);
    }

    return w_key;
}

// 行移位函数
vector<string> move_row(vector<string>& s)
{
    vector<string> ans = s;
    // 字符串数组每个对应一列, 所以是对应到列进行移位
    // 一行对应有两个16进制数，所以需要两个一起移动，相当于两列一起移动
    for (int i = 0; i < 4; ++i)
    {
        int k = i * 2;
        // 就原本矩阵对应的行移位，对于字符串数组就是列移位
        for (int j = 0; j < 4; ++j)
        {
            ans[j][k] = s[(j + i) % 4][k];
            ans[j][k + 1] = s[(j + i) % 4][k + 1];
        }
    }
    return ans;
}

// 分割字符串，长度为8变4组
vector<string> split_s(string& s)
{
    vector<string> ans;
    for (int i = 0; i < s.length(); i += 2)
    {
        ans.emplace_back(s.substr(i, 2));
    }
    return ans;
}

string int_ch2(int num)
{
    string ans = int_to_chs(num);
    // 这里是为了确保只有两位字符串
    while (ans.length() < 2)
    {
        ans = "0" + ans;
    }
    return ans;
}

// 移位函数，其实就是在GF（2^8）的范围进行幂次操作
int power(int num)
{
    int ans = (num << 1) % MOD;
    // 如果第七位是1
    if (num & 0x80)
    {
        ans ^= 0x1b;
    }
    return ans;
}

// 接下来就是列混淆
vector<string> col_confuse(vector<string>& s)
{
    vector<string> ans = s;
    // 算法中对应的是列，这边就直接变成了行，即字符
    for (int i = 0; i < 4; ++i)
    {
        // 需要先将字符串拆分成两两一组，共4组
        auto temp = split_s(s[i]);
        // 先转成数字
        int s0 = str_long(temp[0]), s1 = str_long(temp[1]), s2 = str_long(temp[2]),
            s3 = str_long(temp[3]);
        // 计算混淆后的值
        int t0 = power(s0) ^ power(s1) ^ s1 ^ s2 ^ s3;
        int t1 = s0 ^ power(s1) ^ power(s2) ^ s2 ^ s3;
        int t2 = s0 ^ s1 ^ power(s2) ^ s3 ^ power(s3);
        int t3 = s0 ^ power(s0) ^ s1 ^ s2 ^ power(s3);
        // 转换成字符串再相加
        ans[i] = int_ch2(t0) + int_ch2(t1) + int_ch2(t2) + int_ch2(t3);
    }
    return ans;
}

// 输出函数，4个16进制数为1组
void show(vector<string>& text)
{
    for (auto t : text)
    {
        for (int i = 0; i < 8; i++)
        {
            cout << t[i];
            if (i == 3)
            {
                cout << " ";
            }
        }
        cout << " ";
    }
    cout << endl;
}

// 先定义一个 aes 加密函数
vector<string> aes(string& plain_text, string& key)
{
    // 先拓展密钥
    vector<string> keys = extend_key(key);

    int index = 0;
    // 然后就是10轮迭代
    // 需要知道明文其实是32位，所以需要搞4下
    // 可以先把明文也分组

    // 一开始的先进行一次轮密钥加
    vector<string> texts = group_key(plain_text);
    for (int i = 0; i < 4; ++i)
    {
        texts[i] = string_xor(texts[i], keys[i]);
    }
    index += 4;

    // 然后十次迭代
    for (int k = 0; k < 10; ++k)
    {
        for (int j = 0; j < 4; ++j)
        {
            // 先是字节代换
            texts[j] = wordbyte_sub(texts[j]);
        }
        // 然后是行移位
        texts = move_row(texts);

        if (k < 9)
        {
            // 再来列混淆
            texts = col_confuse(texts);
        }

        // 轮密钥加
        for (int i = 0; i < 4; ++i)
        {
            texts[i] = string_xor(texts[i], keys[i + index]);
        }

        index += 4;
    }

    return texts;
}

// 定义一个实现CBC分组加密的AES数据
string en_cbc_aes(string& plain_text, string& key)
{
    string en_plain_text;
    int text_len = plain_text.size() / 32 + 1;
    // 先实现分组
    vector<string> groups(text_len);
    // 初始下标
    int index = 0;
    // 分组
    for (string& g : groups)
    {
        g = plain_text.substr(index, 32);
        index += 32;
    }
    // CBC模式进行计算
    // AES的CBC加密模式，默认iv是全零（这个称为初始化向量），由于是分组加密，所以下一组的iv，就用前一组的加密的密文来充当
    // 本次由于字符串用16进制表示，所以就是32个4位，iv初始化为32个零
    string iv = "00000000000000000000000000000000";
    // 中间变量组
    vector<string> groups_wxn(text_len);
    // 最终变量组
    vector<string> groups_zyl(text_len);
    vector<string> texts;
    for (int w = 0; w < text_len; w++)
    {
        // 明文与iv异或
        groups_wxn[w] = string_xor(iv, groups[w]);
        // 进行加密得到密文，同时密文是下次加密的iv
        texts = aes(groups_wxn[w], key);
        groups_zyl[w] = texts[0] + texts[1] + texts[2] + texts[3];
        // 本次的密文是下次加密的iv
        iv = groups_zyl[w];
    }
    // 最后整合
    for (string& z : groups_zyl)
    {
        en_plain_text += z;
    }
    return en_plain_text;
}

// 行移位的逆操作函数
vector<string> in_move_row(vector<string>& s)
{
    vector<string> ans = s;
    // 现在变成了逆操作
    for (int i = 0; i < 4; ++i)
    {
        int k = i * 2;
        // 就原本矩阵对应的行移位，对于字符串数组就是列移位
        for (int j = 0; j < 4; ++j)
        {
            ans[j][k] = s[(j - i + 4) % 4][k];
            ans[j][k + 1] = s[(j - i + 4) % 4][k + 1];
        }
    }
    return ans;
}

// 逆字节代换
string in_wordbyte_sub(string& wi_1)
{
    int len = wi_1.length();
    string ans = "";
    for (int i = 0; i < len; i += 2)
    {
        // 先获取当前的下标
        int x = ch_to_int(wi_1[i]), y = ch_to_int(wi_1[i + 1]);
        // 然后获取当前的数字
        int num = S1[x][y];
        // 先将数值转化为字符串
        string s = int_to_chs(num);
        // 然后不足的话补0
        while (s.length() < 2)
        {
            s = "0" + s;
        }

        // 加起来
        ans += s;
    }
    return ans;
}


// 列混淆的逆变换
vector<string> in_col_confuse(vector<string>& s)
{
    // 逆变换其实原来的变换矩阵的逆矩阵，对应0xe, 0xb, 0xd, 0x9
    // 4 列

    vector<string> ans = s;
    for (int i = 0; i < 4; ++i)
    {
        // 先分割成4个两位数字
        auto temp = split_s(s[i]);
        // 转换成数字
        vector<int> nums(4);
        for (int j = 0; j < 4; ++j)
        {
            nums[j] = str_long(temp[j]);
        }
        vector<int> t4(4, 0);
        for (int j = 0; j < 4; ++j)
        {
            for (int t = 0; t < 4; ++t)
            {
                int k = (t - j + 4) % 4;
                t4[j] ^= power(power(power(nums[t])));        // 表示8
                switch (k)
                {
                case 0:        // 0xe = 8 + 4 + 2
                {
                    t4[j] ^= power(power(nums[t])) ^ power(nums[t]);
                    break;
                }
                case 1:        // 0xb = 8 + 2 + 1
                {
                    t4[j] ^= power(nums[t]) ^ nums[t];
                    break;
                }
                case 2:        // 0xd = 8 + 4 + 1
                {
                    t4[j] ^= power(power(nums[t])) ^ nums[t];
                    break;
                }
                default:    // 0x9 = 8 + 1
                    t4[j] ^= nums[t];
                    break;
                }

            }
        }
        // 将数字转换成字符串存储
        ans[i] = int_ch2(t4[0]) + int_ch2(t4[1]) + int_ch2(t4[2]) + int_ch2(t4[3]);
    }

    return ans;
}

// aes解密函数
vector<string> in_aes(string& text, string& key)
{
    // 先拓展密钥
    auto keys = extend_key(key);

    // 初始下标
    int index = 40;

    // 对密文分组
    vector<string> texts = group_key(text);

    // 一开始先进行依次轮密钥加
    for (int i = 0; i < 4; ++i)
    {
        texts[i] = string_xor(texts[i], keys[index + i]);
    }
    index -= 4;

    // 然后十次迭代
    for (int i = 0; i < 10; ++i)
    {
        // 先逆行移位
        texts = in_move_row(texts);

        // 然后是字节代换逆操作
        for (int j = 0; j < 4; ++j)
        {
            texts[j] = in_wordbyte_sub(texts[j]);
        }

        // 轮密钥加
        for (int j = 0; j < 4; ++j)
        {
            texts[j] = string_xor(texts[j], keys[index + j]);
        }

        // 除了最后一轮，都要列混淆逆变换
        if (i < 9)
        {
            texts = in_col_confuse(texts);
        }
        index -= 4;
    }

    return texts;
}

// 定义一个实现CBC分组解密的AES数据
string de_cbc_aes(string& en_text, string& key)
{
    string de_text;
    int text_len = en_text.size() / 32 + 1;
    // 先实现分组
    vector<string> groups(text_len);
    // 初始下标
    int index = 0;
    // 分组
    for (string& x : groups)
    {
        x = en_text.substr(index, 32);
        index += 32;
    }
    // CBC模式进行计算
    // AES的CBC解密模式，默认iv是全零（这个称为初始化向量），由于是分组解密，所以下一组的iv，用前一组密文来充当
    // 本次由于字符串用16进制表示，所以就是32个4位，iv初始化为32个零，与加密的时候相同
    string iv = "00000000000000000000000000000000";
    // 中间变量组
    vector<string> groups_wxn(text_len);
    // 最终变量组
    vector<string> groups_zyl(text_len);
    vector<string> texts;
    for (int w = 0; w < text_len; w++)
    {
        // 密文块解密
        texts = in_aes(groups[w], key);
        groups_wxn[w] = texts[0] + texts[1] + texts[2] + texts[3];
        // 与iv异或得到明文
        groups_zyl[w] = string_xor(iv, groups_wxn[w]);
        // 设置下次解密用到的iv
        iv = groups_wxn[w];
    }
    // 最后整合
    for (string& z : groups_zyl)
    {
        de_text += z;
    }
    return de_text;
}

char* getTime()
{
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
    return tmp;
}

// 需要补充的是
// 1.加密和解密的CBC模式（完成）
// 2.发送以及接收AES（完成）
// 3.最后给AES加上RSA（就用之前密钥的）（完成）
// 4.看能不能添加类似数字签名、时间戳或者是消息认证机制