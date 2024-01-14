#pragma once
#pragma once
#include <iostream>
#include<string>
#include<string.h>
#include <vector>
#include <bitset>
#include <fstream>

using namespace std;

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