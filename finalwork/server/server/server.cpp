#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "Server.h"
#include "Aes.h"
#include"Rsa.h"
#include"MD5.h"
#include<math.h>
#pragma comment(lib,"ws2_32.lib")

extern Paraments m_cParament;

int runServer()
{
	//1.加载套接字库
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err)
		printf("S: socket加载失败\n");
	else
		printf("S: socket加载成功\n");
	//2.创建一个套接字供使用
	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	//3.将套接字绑定到本地地址和端口上
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6020);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(ServerSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR));
	//4.将套接字设置为监听模式，以接收客户端请求
	err = listen(ServerSocket, 5);
	if (err)
		printf("S: 绑定端口失败 请重新启动程序\n");
	else
		printf("S: 成功绑定端口 等待客户端连接\n");
	//5.等待并接收客户端请求，返回新的连接套接字
	SOCKADDR_IN addr_out;
	int len = sizeof(SOCKADDR);
	SOCKET ClientSocket = accept(ServerSocket, (SOCKADDR*)&addr_out, &len);



	// 6.计算密钥 主要是RSA的公钥和私钥 并准备发给客户端 需要补充的是从客户端接收
	// 采用服务端先发送 客户端后发送的策略 并准备一个flag 服务端为0 客户端为1
	// 一 服务器发送
	RsaParam server_param = RsaGetParam();
	m_cParament.d = server_param.d;
	m_cParament.e = server_param.e;
	m_cParament.n = server_param.n;
	PublicKey server_public_key = GetPublicKey();
	char send_to_client_public_key[100], send_N[100], send_E[100];
	sprintf(send_N, "%lu", server_public_key.nN);
	itoa(server_public_key.nE, send_E, 10);
	strcpy(send_to_client_public_key, send_N);
	int len_of_N = strlen(send_N), len_of_E = strlen(send_E);
	send_to_client_public_key[len_of_N] = ',';
	strncpy(send_to_client_public_key + len_of_N + 1, send_E, len_of_E);
	send_to_client_public_key[len_of_N + len_of_E + 1] = '\0';
	printf("S: 向客户端发送服务器的公钥对:%s\n", send_to_client_public_key);
	send(ClientSocket, send_to_client_public_key, strlen(send_to_client_public_key), 0);

	// 二 服务器接收
	char recv_from_client_public_key[100] = { 0 };
	recv(ClientSocket, recv_from_client_public_key, 100, 0);
	printf("S: 从客户端接收客户端的公钥对:%s\n", recv_from_client_public_key);
	char recv_N[100], recv_E[100];
	int count_for_N = 0, count_for_E = 0;
	bool divide_for_key = false;
	for (int i = 0; i < strlen(recv_from_client_public_key); i++)
	{
		if (recv_from_client_public_key[i] == ',') { divide_for_key = true; continue; }
		if (!divide_for_key)
			recv_N[count_for_N++] = recv_from_client_public_key[i];
		else
			recv_E[count_for_E++] = recv_from_client_public_key[i];
	}

	PublicKey client_public_key;
	client_public_key.nE = atoi(recv_E);
	client_public_key.nN = atoll(recv_N);
	//char encryKey[300];


	// 接收加密的AES密钥
	char aes_key[20] = { '\0' };
	char en_aes_key[300] = { '\0' };
	recv(ClientSocket, en_aes_key, 300, 0); // 接收密钥
	printf("S: 从客户端接收到加密的AES密钥:%s\n", en_aes_key);
	int len_of_en_aes_key = 0;
	for (int i = 0; i < 8; i++)
	{
		char en_sixteen_bits_to_char[20] = { '\0' };
		int p = 0;
		while (en_aes_key[len_of_en_aes_key++] != ',')
			en_sixteen_bits_to_char[p++] = en_aes_key[len_of_en_aes_key - 1];
		UINT64 en_sixteen_bits = atoll(en_sixteen_bits_to_char);
		UINT64 sixteen_bits = Decry(en_sixteen_bits);
		aes_key[i * 2] = sixteen_bits >> 8;
		aes_key[i * 2 + 1] = sixteen_bits % 256;
	}
	printf("S: 解密的AES密钥为:%s\n", aes_key);



	// 准备收发消息
	char plaintext[500], ciphtext[500] = { 0 };
	while (true)
	{
		string str_aes_key = charkey_to_strkey(aes_key);

		// 一 服务器接收加密信息
		// 1.接收对方发过来的字符串
		recv(ClientSocket, ciphtext, 500, 0);
		// 2.用自己的私钥(server_private_key)解密发过来的字符串
		int len_of_ciphtext = 0;
		for (int i = 0; i < strlen(ciphtext) / 4; i++)
		{
			if (en_aes_key[len_of_ciphtext++] == '.')
			{
				// 判断到加密后的hash值了
				break;
			}
			char en_sixteen_bits_to_char[20] = { '\0' };
			int p = 0;
			while (en_aes_key[len_of_ciphtext++] != ',')
				en_sixteen_bits_to_char[p++] = en_aes_key[len_of_ciphtext - 1];
			UINT64 en_sixteen_bits = atoll(en_sixteen_bits_to_char);
			UINT64 sixteen_bits = Decry(en_sixteen_bits);
			ciphtext[i * 4] = (sixteen_bits >> 12) % 16;
			ciphtext[i * 4 + 1] = (sixteen_bits >> 8) % 16;
			ciphtext[i * 4 + 2] = (sixteen_bits >> 4) % 16;
			ciphtext[i * 4 + 3] = sixteen_bits % 16;
		}
		string mid_ciphtext = ciphtext;
		// 补充：对hash的解密 需要用服务器的公钥解密
		char recv_en_hash[100];
		// 先保存之前的秘钥 防止丢失
		UINT64 mid_n = m_cParament.n;
		UINT64 mid_d = m_cParament.d;
		UINT64 mid_e = m_cParament.e;
		// 赋值为客户端的公钥（client_public_key）
		m_cParament.n = client_public_key.nN;
		m_cParament.d = client_public_key.nE;
		for (int i = 0; i < strlen(ciphtext) / 4; i++)
		{
			if (en_aes_key[len_of_ciphtext++] == '.')
			{
				// 判断到加密后的hash值了
				break;
			}
			char en_sixteen_bits_to_char[20] = { '\0' };
			int p = 0;
			while (en_aes_key[len_of_ciphtext++] != ',')
				en_sixteen_bits_to_char[p++] = en_aes_key[len_of_ciphtext - 1];
			UINT64 en_sixteen_bits = atoll(en_sixteen_bits_to_char);
			UINT64 sixteen_bits = Decry(en_sixteen_bits);
			recv_en_hash[i * 4] = (sixteen_bits >> 12) % 16;
			recv_en_hash[i * 4 + 1] = (sixteen_bits >> 8) % 16;
			recv_en_hash[i * 4 + 2] = (sixteen_bits >> 4) % 16;
			recv_en_hash[i * 4 + 3] = sixteen_bits % 16;
		}
		// 恢复之前的缓冲区
		m_cParament.n = mid_n;
		m_cParament.d = mid_d;
		m_cParament.e = mid_e;
		string recv_hash = recv_en_hash;
		// 3.将其用AES解密(在解密函数中将其分为128位一组的数据)各个组的128位的数据 需要实现CBC分组解密 即需要加上xor
		string de_mid_ciphtext = de_cbc_aes(mid_ciphtext, str_aes_key);
		// 4.输出解密后的明文(如果hash值验证相等的话)
		if (MD5(de_mid_ciphtext).getstring() == recv_hash)
		{
			char* time;
			strcpy(time, getTime());
			printf("S: [%s]经过解密后的明文:", time);
			cout << de_mid_ciphtext;
			if (de_mid_ciphtext == "exit")
			{
				break;
			}
		}
		else
		{
			cout << "本次接收有误，需要重新接收" << endl;
			continue;
		}



		// 二 服务器发送加密信息
		// 补充：先用哈希（MD5）进行加密 这个值再用私钥加密 后与整体进行一起加密
		printf("S: 请输入明文:");
		setbuf(stdin, NULL);
		scanf("%[^\n]s", plaintext); // 使得空行代表读取完毕而不是空格
		bool exit = false;
		if (strcmp(plaintext, "exit") == 0) { exit = true; }
		string mid_plain_text = plaintext;
		// hash也是32个16进制数的字符串
		string hash = MD5(mid_plain_text).getstring();
		PublicKey server_private_key;
		server_private_key.nE = m_cParament.d;
		server_private_key.nN = m_cParament.n;
		// 用自己的私钥去加密
		char en_hash[100] = { '\0' };
		int len_of_en_text = 0;
		for (int i = 0; i < hash.size() / 4; i++)
		{
			int p1 = i * 4, p2 = i * 4 + 1;
			int p3 = i * 4 + 2, p4 = i * 4 + 3;
			int num1 = int(hash[p1]);
			int num2 = int(hash[p2]);
			int num3 = int(hash[p1]);
			int num4 = int(hash[p2]);
			UINT64 sixteen_bits = (num1 << 12) + (num2 << 8) + (num3 << 4) + num4;
			UINT64 en_sixteen_bits = Encry(sixteen_bits, server_private_key);
			char en_sixteen_bits_to_char[20];
			sprintf(en_sixteen_bits_to_char, "%lu", en_sixteen_bits);
			strncpy(en_hash + len_of_en_text, en_sixteen_bits_to_char, strlen(en_sixteen_bits_to_char));
			len_of_en_text += strlen(en_sixteen_bits_to_char);
		}
		en_hash[len_of_en_text] = '\0';
		string wxn_en_hash = en_hash;
		// 1.输入信息 并将其用AES加密为128位的数据 需要实现CBC分组加密 即需要加上xor 并将分组加密完的数据合成一个字符串
		// 为了方便区分最后的hash
		mid_plain_text += ".";
		mid_plain_text += wxn_en_hash;
		string mid_en_text = en_cbc_aes(mid_plain_text, str_aes_key); // 返回的是一个CBC分组加密完合成的字符串
		// 2.用对方的公钥(client_public_key)进行加密 由于合成的字符串每一个字符是4位 所以需要4个为一组
		char en_text[500] = { '\0' };
		int len_of_en_text = 0;
		for (int i = 0; i < mid_en_text.size() / 4; i++)
		{
			int p1 = i * 4, p2 = i * 4 + 1;
			int p3 = i * 4 + 2, p4 = i * 4 + 3;
			int num1 = int(mid_en_text[p1]);
			int num2 = int(mid_en_text[p2]);
			int num3 = int(mid_en_text[p1]);
			int num4 = int(mid_en_text[p2]);
			UINT64 sixteen_bits = (num1 << 12) + (num2 << 8) + (num3 << 4) + num4;
			UINT64 en_sixteen_bits = Encry(sixteen_bits, client_public_key);
			char en_sixteen_bits_to_char[20];
			sprintf(en_sixteen_bits_to_char, "%lu", en_sixteen_bits);
			strncpy(en_text + len_of_en_text, en_sixteen_bits_to_char, strlen(en_sixteen_bits_to_char));
			len_of_en_text += strlen(en_sixteen_bits_to_char);
			char comma = ',';
			en_text[len_of_en_text] = comma;
			len_of_en_text += 1;
		}
		en_text[len_of_en_text] = '\0';
		// 3.将加密完的字符串发送
		send(ClientSocket, en_text, strlen(en_text), 0);
		if (exit)
		{
			break;
		}
	}
	printf("\nS: 退出...");

	//关闭套接字
	closesocket(ServerSocket);
	WSACleanup();
	return 0;
}