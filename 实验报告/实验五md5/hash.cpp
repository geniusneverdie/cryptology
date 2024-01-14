// hash.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include<string>
#include<string.h>
#include <vector>
#include <bitset>
#include <fstream>

using namespace std;

int s[4][4] = { 7,12,17,22,5,9,14,20,4,11,16,23,6,10,15,21 };

int HexToBit[16][4] = { {0,0,0,0},{0,0,0,1},{0,0,1,0},{0,0,1,1},
                        {0,1,0,0},{0,1,0,1},{0,1,1,0},{0,1,1,1},
                        {1,0,0,0},{1,0,0,1},{1,0,1,0},{1,0,1,1},
                        {1,1,0,0},{1,1,0,1},{1,1,1,0},{1,1,1,1}
};

// 将string类型转换为bits类型，方便后续的雪崩效应
void getbit(string a, bitset<128>& temp)
{
    int num = 127;
    // 如果无前缀0x 则这里i需要从2开始
    for (int i = 0; i < a.length(); i++)
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

class MD5 {
public:
    bool finished;
    unsigned int link[4];
    unsigned int count[2];
    unsigned char buffer[64];
    unsigned char digest[16];
    static  unsigned char tianchong[64];
    static  char hex[16];
    MD5(string& message);
    unsigned char* get_digest();
    string getstring();
    void init(unsigned char* input, int len);
    void zip(unsigned char block[64]);
    void encode(unsigned int* input, unsigned char* output, int length);
};

unsigned char MD5::tianchong[64] = { 0x80 };
char MD5::hex[16] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f'
};
unsigned int F(unsigned int& x, unsigned int& y, unsigned int& z)
{
    return (x & y) | (~x & z);
}
unsigned int G(unsigned int& x, unsigned int& y, unsigned int& z)
{
    return (x & z) | (y & ~z);
}
unsigned int H(unsigned int& x, unsigned int& y, unsigned int& z)
{
    return x ^ y ^ z;
}
unsigned int I(unsigned int& x, unsigned int& y, unsigned int& z)
{
    return y ^ (x | ~z);
}
unsigned int leftshift(unsigned int& num, unsigned int& n)
{
    return (num << n) | (num >> (32 - n));
}
void round1(unsigned int& a, unsigned int& b, unsigned int& c, unsigned int& d, unsigned int& x, unsigned int s, unsigned int ac)
{
    a += F(b, c, d) + x + ac;
    a = leftshift(a, s);
    a += b;
}
void round2(unsigned int& a, unsigned int& b, unsigned int& c, unsigned int& d, unsigned int& x, unsigned int s, unsigned int ac)
{
    a += G(b, c, d) + x + ac;
    a = leftshift(a, s);
    a += b;
}
void round3(unsigned int& a, unsigned int& b, unsigned int& c, unsigned int& d, unsigned int& x, unsigned int s, unsigned int ac)
{
    a += H(b, c, d) + x + ac;
    a = leftshift(a, s);
    a += b;
}
void round4(unsigned int& a, unsigned int& b, unsigned int& c, unsigned int& d, unsigned int& x, unsigned int s, unsigned int ac)
{
    a += I(b, c, d) + x + ac;
    a = leftshift(a, s);
    a += b;
}

MD5::MD5(string& message)
{
    finished = false;

    count[0] = count[1] = 0;

    link[0] = 0x67452301;
    link[1] = 0xefcdab89;
    link[2] = 0x98badcfe;
    link[3] = 0x10325476;

    init((unsigned char*)message.c_str(), message.length());
}

unsigned char* MD5::get_digest()
{
    if (!finished)
    {
        finished = true;

        unsigned char bits[8];
        unsigned int oldState[4];
        unsigned int oldCount[2];
        int nowlength, waitlength;

        memcpy(oldState, link, 16);
        memcpy(oldCount, count, 8);

        encode(count, bits, 8);

        nowlength = (unsigned int)((count[0] >> 3) & 0x3f);
        waitlength = (nowlength < 56) ? (56 - nowlength) : (120 - nowlength);
        init(tianchong, waitlength);


        nowlength = (unsigned int)((count[0] >> 3) & 0x3f);

        memcpy(&buffer[nowlength], bits, 8);
        zip(buffer);

        encode(link, digest, 16);
        memcpy(link, oldState, 16);
        memcpy(count, oldCount, 8);
    }
    return digest;
}

void MD5::init(unsigned char* input, int len)
{

    unsigned int i, nowlength, waitlength;

    finished = false;

    nowlength = (unsigned int)((count[0] >> 3) & 0x3f);
    count[0] += (unsigned int)len << 3;
    if ((count[0]) < ((unsigned int)len << 3)) {
        count[1] += 1;
    }
    count[1] += ((unsigned int)len >> 29);

    waitlength = 64 - nowlength;
    if (len >= waitlength) {
        memcpy(&buffer[nowlength], input, waitlength);
        zip(buffer);
        for (i = waitlength; i + 63 < len; i += 64) {
            zip(&input[i]);
        }
        nowlength = 0;
    }
    else {
        i = 0;
    }
    memcpy(&buffer[nowlength], &input[i], len - i);
}

void MD5::zip(unsigned char block[64])
{

    unsigned int a = link[0], b = link[1], c = link[2], d = link[3], x[16];
    for (int i = 0, j = 0; j < 64; ++i, j += 4)
    {
        x[i] = ((unsigned int)block[j]) | (((unsigned int)block[j + 1]) << 8) | (((unsigned int)block[j + 2]) << 16) | (((unsigned int)block[j + 3]) << 24);
    }
    round1(a, b, c, d, x[0], s[0][0], 0xd76aa478);
    round1(d, a, b, c, x[1], s[0][1], 0xe8c7b756);
    round1(c, d, a, b, x[2], s[0][2], 0x242070db);
    round1(b, c, d, a, x[3], s[0][3], 0xc1bdceee);
    round1(a, b, c, d, x[4], s[0][0], 0xf57c0faf);
    round1(d, a, b, c, x[5], s[0][1], 0x4787c62a);
    round1(c, d, a, b, x[6], s[0][2], 0xa8304613);
    round1(b, c, d, a, x[7], s[0][3], 0xfd469501);
    round1(a, b, c, d, x[8], s[0][0], 0x698098d8);
    round1(d, a, b, c, x[9], s[0][1], 0x8b44f7af);
    round1(c, d, a, b, x[10], s[0][2], 0xffff5bb1);
    round1(b, c, d, a, x[11], s[0][3], 0x895cd7be);
    round1(a, b, c, d, x[12], s[0][0], 0x6b901122);
    round1(d, a, b, c, x[13], s[0][1], 0xfd987193);
    round1(c, d, a, b, x[14], s[0][2], 0xa679438e);
    round1(b, c, d, a, x[15], s[0][3], 0x49b40821);

    round2(a, b, c, d, x[1], s[1][0], 0xf61e2562);
    round2(d, a, b, c, x[6], s[1][1], 0xc040b340);
    round2(c, d, a, b, x[11], s[1][2], 0x265e5a51);
    round2(b, c, d, a, x[0], s[1][3], 0xe9b6c7aa);
    round2(a, b, c, d, x[5], s[1][0], 0xd62f105d);
    round2(d, a, b, c, x[10], s[1][1], 0x2441453);
    round2(c, d, a, b, x[15], s[1][2], 0xd8a1e681);
    round2(b, c, d, a, x[4], s[1][3], 0xe7d3fbc8);
    round2(a, b, c, d, x[9], s[1][0], 0x21e1cde6);
    round2(d, a, b, c, x[14], s[1][1], 0xc33707d6);
    round2(c, d, a, b, x[3], s[1][2], 0xf4d50d87);
    round2(b, c, d, a, x[8], s[1][3], 0x455a14ed);
    round2(a, b, c, d, x[13], s[1][0], 0xa9e3e905);
    round2(d, a, b, c, x[2], s[1][1], 0xfcefa3f8);
    round2(c, d, a, b, x[7], s[1][2], 0x676f02d9);
    round2(b, c, d, a, x[12], s[1][3], 0x8d2a4c8a);

    round3(a, b, c, d, x[5], s[2][0], 0xfffa3942);
    round3(d, a, b, c, x[8], s[2][1], 0x8771f681);
    round3(c, d, a, b, x[11], s[2][2], 0x6d9d6122);
    round3(b, c, d, a, x[14], s[2][3], 0xfde5380c);
    round3(a, b, c, d, x[1], s[2][0], 0xa4beea44);
    round3(d, a, b, c, x[4], s[2][1], 0x4bdecfa9);
    round3(c, d, a, b, x[7], s[2][2], 0xf6bb4b60);
    round3(b, c, d, a, x[10], s[2][3], 0xbebfbc70);
    round3(a, b, c, d, x[13], s[2][0], 0x289b7ec6);
    round3(d, a, b, c, x[0], s[2][1], 0xeaa127fa);
    round3(c, d, a, b, x[3], s[2][2], 0xd4ef3085);
    round3(b, c, d, a, x[6], s[2][3], 0x4881d05);
    round3(a, b, c, d, x[9], s[2][0], 0xd9d4d039);
    round3(d, a, b, c, x[12], s[2][1], 0xe6db99e5);
    round3(c, d, a, b, x[15], s[2][2], 0x1fa27cf8);
    round3(b, c, d, a, x[2], s[2][3], 0xc4ac5665);

    round4(a, b, c, d, x[0], s[3][0], 0xf4292244);
    round4(d, a, b, c, x[7], s[3][1], 0x432aff97);
    round4(c, d, a, b, x[14], s[3][2], 0xab9423a7);
    round4(b, c, d, a, x[5], s[3][3], 0xfc93a039);
    round4(a, b, c, d, x[12], s[3][0], 0x655b59c3);
    round4(d, a, b, c, x[3], s[3][1], 0x8f0ccc92);
    round4(c, d, a, b, x[10], s[3][2], 0xffeff47d);
    round4(b, c, d, a, x[1], s[3][3], 0x85845dd1);
    round4(a, b, c, d, x[8], s[3][0], 0x6fa87e4f);
    round4(d, a, b, c, x[15], s[3][1], 0xfe2ce6e0);
    round4(c, d, a, b, x[6], s[3][2], 0xa3014314);
    round4(b, c, d, a, x[13], s[3][3], 0x4e0811a1);
    round4(a, b, c, d, x[4], s[3][0], 0xf7537e82);
    round4(d, a, b, c, x[11], s[3][1], 0xbd3af235);
    round4(c, d, a, b, x[2], s[3][2], 0x2ad7d2bb);
    round4(b, c, d, a, x[9], s[3][3], 0xeb86d391);

    link[0] += a;
    link[1] += b;
    link[2] += c;
    link[3] += d;
}

void MD5::encode(unsigned int* input, unsigned char* output, int length)
{
    for (int i = 0, j = 0; j < length; ++i, j += 4)
    {
        output[j] = (unsigned char)(input[i] & 0xff);
        output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
        output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
        output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
    }
}

string MD5::getstring()
{
    unsigned char* digest_zyl = get_digest();
    string str;
    for (int i = 0; i < 16; ++i)
    {
        int t = digest_zyl[i];
        int a = t / 16;
        int b = t % 16;
        str.append(1, hex[a]);
        str.append(1, hex[b]);
    }
    return str;
}

void print(string& message)
{
    string result = MD5(message).getstring();
    cout << "MD5的加密结果如下（采用16进制的方法输出）：" << endl;
    cout << "0x";
    for (int i = 0; i < 32; i++)
    {
        cout << result[i];
    }
    cout << endl;
    //    cout << result;
}

int avalanche(string& zyl, string& wxn)
{
    bitset<128> zyl_bit, wxn_bit;
    int num = 0;
    string zyl_MD5 = MD5(zyl).getstring();
    string wxn_MD5 = MD5(wxn).getstring();

    cout << "原始字符串：" << zyl << endl << "改后字符串：" << wxn << endl;
    cout << "原始结果：0x" << zyl_MD5 << endl << "改后结果：0x" << wxn_MD5 << endl;

    getbit(zyl_MD5, zyl_bit);
    getbit(wxn_MD5, wxn_bit);

    for (int i = 0; i < 128; i++)
    {
        if (zyl_bit[i] != wxn_bit[i])
        {
            num++;
        }
    }

    //    cout << endl;
    return num;
}

int main()
{
    //    cout << 1;

    while (true)
    {
        // 进行简介
        cout << "====================================================================================" << endl;
        cout << "这是我的`MD5`加密器，你可以输入以下数字进行相应操作：" << endl;
        cout << "0.`MD5`加密" << endl;
        cout << "1.检测雪崩" << endl;
        cout << "2.退出程序" << endl;
        cout << "====================================================================================" << endl;

        int op;
        // 输入想要进行的操作
        cin >> op;

        if (op == 0)
        {
            string str;
            cout << "请输入你想要进行`MD5`加密的字符串：" << endl;
            cin >> str;
            print(str);
        }
        else if (op == 1)
        {
            // 雪崩
            // 原始明文
            string zyl = "12345678901234567890123456789012345678901234567890123456789012345678901234567890";
            string zyl_MD5 = "57edf4a22be3c955ac49da2e2107b67a";
            // 每次改变逗号的位置
            // 8次不同的密文
            string wxn[8] = {
                "1234567890123456789012345678901234567890，1234567890123456789012345678901234567890",
                "123456789012345678901234567890123456789012，34567890123456789012345678901234567890",
                "12345678901234567890123456789012345678901234，567890123456789012345678901234567890",
                "1234567890123456789012345678901234567890123456，7890123456789012345678901234567890",
                "123456789012345678901234567890123456789012345678，90123456789012345678901234567890",
                "12345678901234567890123456789012345678901234567890，123456789012345678901234567890",
                "1234567890123456789012345678901234567890123456789012，3456789012345678901234567890",
                "123456789012345678901234567890123456789012345678901234，56789012345678901234567890"
            };
            int num[8];
            cout << "------------------------------------------------------------------------------------" << endl;
            for (int i = 0; i < 8; i++)
            {
                // 每次得到的不同位数
                num[i] = avalanche(zyl, wxn[i]);
                cout << "不同的数字位数：" << num[i] << endl;
            }
            int num_pre = 0;
            for (int w = 0; w < 8; w++)
            {
                num_pre += num[w];
            }
            cout << "改变字符串获得不同的数字位数的平均数为：" << double(num_pre) / 8 << endl;
            cout << "------------------------------------------------------------------------------------" << endl;
        }
        else
        {
            break;
        }
    }

    system("pause");
    return 0;
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
