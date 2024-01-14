#pragma once
#include<iostream>
#include<math.h>
#include<string>
#include<time.h>
#include<stdlib.h>
#include<fstream>
using namespace std;
const int maxn = 1025;
class bigint
{
private:
	bool num[maxn];
public:
	bool flag;
	// 0是正数，1是负数
	bigint()
	{
		int i;
		for (i = 0; i < maxn; i++)
		{
			num[i] = 0;
		}
		flag = 0;
	}
	bigint(int in)
	{
		if (in >= 0)flag = 0;
		else flag = 1;
		int i = 0;
		while (in)
		{
			num[i] = in % 2;
			in = in / 2;
			i++;
		}
		for (; i < maxn; i++)
		{
			num[i] = 0;
		}
	}
	bigint(string in)
	{
		int i, j = 0;
		int temp;
		for (i = 0; i < maxn; i++) {
			num[i] = 0;
		}
		for (i = in.length() - 1; i >= 0; i--) {
			if (in[i] >= 'a' && in[i] <= 'f') {
				temp = in[i] - 'a' + 10;
			}
			else {
				if (in[i] >= 'A' && in[i] <= 'F') {
					temp = in[i] - 'A' + 10;
				}
				else {
					temp = in[i] - '0';
				}
			}
			if (temp / 8) {
				num[4 * j + 3] = 1;
			}
			if ((temp % 8) / 4) {
				num[4 * j + 2] = 1;
			}
			if ((temp % 4) / 2) {
				num[4 * j + 1] = 1;
			}
			if (temp % 2) {
				num[4 * j] = 1;
			}
			j++;
		}
		flag = 0;
	}
	void random(int n)
	{
		bigint temp;
		srand((int)time(0));
		for (int i = 0; i < n; i++)
		{
			int x = rand() % 2;
			if (x == 1)
				num[i] = true;
			else
				num[i] = false;
		}
		for (int i = maxn - 1; i >= n; i--)
		{
			num[i] = false;
		}
		num[n - 1] = true;
		num[0] = true;
	}
	friend bigint operator+(bigint a, bigint b)
	{
		bigint result;
		int i;
		bool temp = 0;
		if (a.flag == b.flag) {
			for (i = 0; i < maxn; i++) {
				if (a[i] == 0 && b[i] == 0 && temp == 0) {
					result.make(i, 0);
					temp = 0;
				}
				else {
					if (a[i] == 1 && b[i] == 0 && temp == 0 ||
						a[i] == 0 && b[i] == 1 && temp == 0 ||
						a[i] == 0 && b[i] == 0 && temp == 1) {
						temp = 0;
						result.make(i, 1);
					}
					else {
						if (a[i] == 1 && b[i] == 1 && temp == 0 ||
							a[i] == 0 && b[i] == 1 && temp == 1 ||
							a[i] == 1 && b[i] == 0 && temp == 1) {
							temp = 1;
							result.make(i, 0);
						}
						else {
							if (a[i] == 1 && b[i] == 1 && temp == 1) {
								temp = 1;
								result.make(i, 1);
							}
						}
					}
				}
			}
			result.flag = a.flag;
		}
		if (a.flag == 0 && b.flag == 1) {
			b.Num_Not();
			return a - b;
		}
		if (a.flag == 1 && b.flag == 0) {
			a.Num_Not();
			return b - a;
		}
		if (temp) {
			cout << "Overflow" << endl;
		}
		return result;
	}
	friend bigint operator-(bigint a, bigint b)
	{
		bigint result;
		int i;
		bool temp = 0;
		if (a.flag == b.flag) {
			if (!(b.abs() > a.abs())) {
				for (i = 0; i < maxn; i++) {
					if (a[i] == 0 && b[i] == 0 && temp == 0 ||
						a[i] == 1 && b[i] == 0 && temp == 1 ||
						a[i] == 1 && b[i] == 1 && temp == 0) {
						result.make(i, 0);
						temp = 0;
					}
					else {
						if (a[i] == 0 && b[i] == 0 && temp == 1 ||
							a[i] == 0 && b[i] == 1 && temp == 0 ||
							a[i] == 1 && b[i] == 1 && temp == 1) {
							temp = 1;
							result.make(i, 1);
						}
						else {
							if (a[i] == 0 && b[i] == 1 && temp == 1) {
								temp = 1;
								result.make(i, 0);
							}
							else {
								if (a[i] == 1 && b[i] == 0 && temp == 0) {
									temp = 0;
									result.make(i, 1);
								}
							}
						}
					}
				}
				result.flag = a.flag;
			}
			else {
				result = (b - a);
				result.flag = !a.flag;
			}
		}
		if (a.flag == 0 && b.flag == 1) {
			b.Num_Not();
			return a + b;
		}
		if (a.flag == 1 && b.flag == 0) {
			b.Num_Not();
			return a + b;
		}
		return result;
	}
	friend bigint operator*(bigint a, bigint b)
	{
		bigint result;
		int i;
		bool first = true;
		for (i = 0; i < maxn; i++) {
			if (b[i] == 1) {
				result = result + a;
			}
			a.leftMove(1);
		}
		if (a.flag == b.flag) {
			result.flag = 0;
		}
		else {
			result.flag = 1;
		}
		return result;
	}
	friend bigint operator/(bigint a, bigint b)
	{
		bigint count("0");
		if (a.flag == b.flag) {
			count.flag = 0;
		}
		else {
			count.flag = 1;
		}
		int i, j;
		bigint tempA = a, tempB = b, ZERO("0");
		if (ZERO > tempA) {
			tempA.Num_Not();
		}
		if (ZERO > tempB) {
			tempB.Num_Not();
		}
		for (j = maxn - 1; j >= 0; j--) {
			if (b[j]) {
				break;
			}
		}
		bigint result ;
		for (i = maxn - j - 1; i >= 0; i--) {
			result = tempB;
			result.leftMove(i);
			if (!(result > tempA)) {
				count.make(i, 1);
				tempA = tempA - result;
			}
		}
		return count;
	}
	friend bigint operator%(bigint a, bigint b)
	{
		bigint result;
		result = a - (a / b) * b;
		return result;
	}
	friend bool operator>(bigint a, bigint b)
	{
		bool Bigger = 0;
		int i;
		if (a.flag == 0 && b.flag == 1) {
			Bigger = true;
			bool allZERO = true;
			for (i = 0; i < maxn; i++) {
				if (a[i] != 0 || b[i] != 0) {
					allZERO = false;
				}
			}
			if (allZERO) {
				Bigger = false;
			}
		}
		else {
			if (a.flag == 1 && b.flag == 0) {
				Bigger = false;
				bool allZERO = true;
				for (i = 0; i < maxn; i++) {
					if (a[i] != 0 || b[i] != 0) {
						allZERO = false;
					}
				}
				if (allZERO) {
					Bigger = false;
				}
			}
			else {
				if (a.flag == 0 && b.flag == 0) {
					for (i = maxn - 1; i >= 0; i--) {
						if (a[i] == 1 && b[i] == 0) {
							Bigger = true;
							break;
						}
						if (a[i] == 0 && b[i] == 1) {
							Bigger = false;
							break;
						}
					}
				}
				else {
					for (i = maxn - 1; i >= 0; i--) {
						if (a[i] == 1 && b[i] == 0) {
							Bigger = false;
							break;
						}
						if (a[i] == 0 && b[i] == 1) {
							Bigger = true;
							break;
						}
					}
				}
			}
		}
		return Bigger;
	}
	friend bool operator==(bigint a, bigint b)
	{
		int i;
		bool same = true;
		if (a.flag != b.flag) {
			same = false;
		}
		for (i = 0; i < maxn; i++) {
			if (a[i] != b[i]) {
				same = false;
				break;
			}
		}
		return same;
	}
	friend bool operator!=(bigint a, bigint b)
	{
		int i;
		if (a.flag != b.flag) {
			return true;
		}
		for (i = 0; i < maxn; i++) {
			if (a[i] != b[i]) {
				return true;
			}
		}
		return false;
	}
	bigint modMul(bigint a, bigint b, bigint c)
	{
		bigint result;
		int i;
		bool first = true;
		for (i = 0; i < maxn; i++) {
			if (b[i] == 1) {
				result = result + a;
				while (result > c)
					result = result - c;
			}
			a.leftMove(1);
			while (a > c)
				a = a - c;
		}
		if (a.flag == b.flag) {
			result.flag = 0;
		}
		else {
			result.flag = 1;
		}
		return result;
	}
	bigint expmod(bigint e, bigint n)
	{
		bigint c("1");
		int i = maxn - 1;
		while (!e[i]) {
			i--;
		}
		int j;
		for (j = i; j >= 0; j--) {
			c = modMul(c, c, n);
			if (e[j]) {
				c = modMul(c, (*this), n);
			}
		}
		return c;
	}
	bigint inv(bigint x)
	{
		bigint ZERO("0"), ONE("1");
		bigint x1 = ONE, x2 = ZERO, x3 = x;
		bigint y1 = ZERO, y2 = ONE, y3 = (*this);
		bigint t1, t2, t3;
		if (y3 == ONE) {
			return ONE;
		}
		bigint q;
		bigint g;
		do {
			q = x3 / y3;
			t1 = x1 - q * y1;
			t2 = x2 - q * y2;
			t3 = x3 - q * y3;
			x1 = y1;
			x2 = y2;
			x3 = y3;
			y1 = t1;
			y2 = t2;
			y3 = t3;
		} while (!(y3 == ONE));
		g = y2;
		if (!(g > ZERO))
			g = x + g;
		return g;
	}
	void leftMove(int i)
	{
		int k;
		for (k = maxn - 1; k > i - 1; k--) {
			num[k] = num[k - i];
		}
		for (k = 0; k < i; k++) {
			num[k] = 0;
		}
	}
	bool operator[](int i)
	{
		return num[i];
	}
	void make(int i, bool a)
	{
		num[i] = a;
	}
	// 16进制的打印
	void print()
	{
		if (this->flag == 1) {
			cout << '-';
		}
		char result[maxn];
		int i;
		for (i = maxn - 1; i >= 0; i--) {
			if ((*this)[i] == 1) {
				break;
			}
		}
		i++;
		int k;
		int length = 0;
		switch (i % 4) {
		case 1:
			length = i + 3;
			break;
		case 2:
			length = i + 2;
			break;
		case 3:
			length = i + 1;
			break;
		case 0:
			length = i;
			break;
		}
		for (k = 0; k < length; k = k + 4) {
			if ((*this)[k] == 0 && (*this)[k + 1] == 0 && (*this)[k + 2] == 0 && (*this)[k + 3] == 0) {
				result[k / 4] = '0';
			}
			if ((*this)[k] == 1 && (*this)[k + 1] == 0 && (*this)[k + 2] == 0 && (*this)[k + 3] == 0) {
				result[k / 4] = '1';
			}
			if ((*this)[k] == 0 && (*this)[k + 1] == 1 && (*this)[k + 2] == 0 && (*this)[k + 3] == 0) {
				result[k / 4] = '2';
			}
			if ((*this)[k] == 1 && (*this)[k + 1] == 1 && (*this)[k + 2] == 0 && (*this)[k + 3] == 0) {
				result[k / 4] = '3';
			}
			if ((*this)[k] == 0 && (*this)[k + 1] == 0 && (*this)[k + 2] == 1 && (*this)[k + 3] == 0) {
				result[k / 4] = '4';
			}
			if ((*this)[k] == 1 && (*this)[k + 1] == 0 && (*this)[k + 2] == 1 && (*this)[k + 3] == 0) {
				result[k / 4] = '5';
			}
			if ((*this)[k] == 0 && (*this)[k + 1] == 1 && (*this)[k + 2] == 1 && (*this)[k + 3] == 0) {
				result[k / 4] = '6';
			}
			if ((*this)[k] == 1 && (*this)[k + 1] == 1 && (*this)[k + 2] == 1 && (*this)[k + 3] == 0) {
				result[k / 4] = '7';
			}
			if ((*this)[k] == 0 && (*this)[k + 1] == 0 && (*this)[k + 2] == 0 && (*this)[k + 3] == 1) {
				result[k / 4] = '8';
			}
			if ((*this)[k] == 1 && (*this)[k + 1] == 0 && (*this)[k + 2] == 0 && (*this)[k + 3] == 1) {
				result[k / 4] = '9';
			}
			if ((*this)[k] == 0 && (*this)[k + 1] == 1 && (*this)[k + 2] == 0 && (*this)[k + 3] == 1) {
				result[k / 4] = 'A';
			}
			if ((*this)[k] == 1 && (*this)[k + 1] == 1 && (*this)[k + 2] == 0 && (*this)[k + 3] == 1) {
				result[k / 4] = 'B';
			}
			if ((*this)[k] == 0 && (*this)[k + 1] == 0 && (*this)[k + 2] == 1 && (*this)[k + 3] == 1) {
				result[k / 4] = 'C';
			}
			if ((*this)[k] == 1 && (*this)[k + 1] == 0 && (*this)[k + 2] == 1 && (*this)[k + 3] == 1) {
				result[k / 4] = 'D';
			}
			if ((*this)[k] == 0 && (*this)[k + 1] == 1 && (*this)[k + 2] == 1 && (*this)[k + 3] == 1) {
				result[k / 4] = 'E';
			}
			if ((*this)[k] == 1 && (*this)[k + 1] == 1 && (*this)[k + 2] == 1 && (*this)[k + 3] == 1) {
				result[k / 4] = 'F';
			}
		}
		if (i == 0) {
			cout << '0' << endl;
		}
		else {
			for (i = (k / 4) - 1; i >= 0; i--) {
				cout << result[i];
			}
			cout << endl;
		}
	}
	void Num_Not()
	{
		this->flag = !(this->flag);
	}
	bigint abs()
	{
		bigint temp = (*this);
		if (temp.flag == 0) {
			return temp;
		}
		else {
			temp.Num_Not();
			return temp;
		}
	}
};