#include <windows.h>
#include <stdio.h>
#include "Client.h"
#include"Rsa.h"
#include "Aes.h"
#include "MD5.h"
#include<math.h>
#pragma comment(lib,"ws2_32.lib")

extern Paraments m_cParament;

int runClient()
{
	// 1.加载套接字库
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err)
		printf("C: socket加载失败\n");
	else
		printf("C: socket加载成功\n");
	// 2.创建一个套接字供使用
	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	// 3.向服务器发出连接请求
	SOCKADDR_IN socksin; // 记录服务器端地址
	socksin.sin_family = AF_INET;
	socksin.sin_port = htons(6020);
	socksin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int rf = connect(ServerSocket, (SOCKADDR*)&socksin, sizeof(socksin));
	if (rf == SOCKET_ERROR)
		printf("C: 与服务器连接失败\n");
	else
	{
		printf("C: 成功与服务器进行连接\n");




		// 接收公钥对 客户端应该也需要自己的公私钥
		// 采用服务端先发送 客户端后发送的策略 并准备一个flag 服务端为0 客户端为1
		// 一 客户端接收
		char recv_from_server_public_key[100] = { 0 };
		recv(ServerSocket, recv_from_server_public_key, 100, 0);
		printf("C: 从服务器接收服务器的公钥对:%s\n", recv_from_server_public_key);
		char recv_N[100], recv_E[100];
		int count_for_N = 0, count_for_E = 0;
		bool divide_for_key = false;
		for (int i = 0; i < strlen(recv_from_server_public_key); i++)
		{
			if (recv_from_server_public_key[i] == ',') { divide_for_key = true; continue; }
			if (!divide_for_key)
				recv_N[count_for_N++] = recv_from_server_public_key[i];
			else
				recv_E[count_for_E++] = recv_from_server_public_key[i];
		}

		PublicKey server_public_key;
		server_public_key.nE = atoi(recv_E);
		server_public_key.nN = atoll(recv_N);


		// 二 客户端发送
		RsaParam client_param = RsaGetParam();
		m_cParament.d = client_param.d;
		m_cParament.e = client_param.e;
		m_cParament.n = client_param.n;
		PublicKey client_public_key = GetPublicKey();
		char send_to_server_public_key[100], send_N[100], send_E[100];
		sprintf(send_N, "%lu", client_public_key.nN);
		itoa(client_public_key.nE, send_E, 10);
		strcpy(send_to_server_public_key, send_N);
		int len_of_N = strlen(send_N), len_of_E = strlen(send_E);
		send_to_server_public_key[len_of_N] = ',';
		strncpy(send_to_server_public_key + len_of_N + 1, send_E, len_of_E);
		send_to_server_public_key[len_of_N + len_of_E + 1] = '\0';
		printf("C: 向服务器发送客户端的公钥对:%s\n", send_to_server_public_key);
		send(ServerSocket, send_to_server_public_key, strlen(send_to_server_public_key), 0);



		// 准备发送AES密钥
		char en_aes_key[300] = { '\0' };
		char aes_key[16] = { 0 };
		GenerateAesKey(aes_key);
		printf("C: 随机生成一个AES密钥:");
		for (int i = 0; i < 16; i++) {
			printf("%c", aes_key[i]);
		}
		printf("\n");
		int len_of_en_aes_key = 0;
		for (int i = 0; i < 8; i++)
		{
			int p1 = i * 2, p2 = i * 2 + 1;
			int num1 = int(aes_key[p1]);
			int num2 = int(aes_key[p2]);
			UINT64 sixteen_bits = (num1 << 8) + num2;
			UINT64 en_sixteen_bits = Encry(sixteen_bits, server_public_key);
			char en_sixteen_bits_to_char[20];
			sprintf(en_sixteen_bits_to_char, "%lu", en_sixteen_bits);
			strncpy(en_aes_key + len_of_en_aes_key, en_sixteen_bits_to_char, strlen(en_sixteen_bits_to_char));
			len_of_en_aes_key += strlen(en_sixteen_bits_to_char);
			char comma = ',';
			en_aes_key[len_of_en_aes_key] = comma;
			len_of_en_aes_key += 1;
		}
		en_aes_key[len_of_en_aes_key] = '\0';
		// 发送加密的AES密钥给服务器
		printf("C: 向服务器发送加密的AES密钥:%s\n", en_aes_key);
		send(ServerSocket, en_aes_key, len_of_en_aes_key, 0);




		// 开始准备收发信息
		char plaintext[500], ciphtext[500] = { 0 };
		while (true)
		{
			string str_aes_key = charkey_to_strkey(aes_key);

			// 一 客户端发送加密信息
			// 补充：先用哈希（MD5）进行加密 这个值再用私钥加密 后与整体进行一起加密
			printf("C: 请输入明文:");
			setbuf(stdin, NULL);
			scanf("%[^\n]s", plaintext); // 使得空行代表读取完毕而不是空格
			bool exit = false;
			if (strcmp(plaintext, "exit") == 0) { exit = true; }
			string mid_plain_text = plaintext;
			// hash也是32个16进制数的字符串
			string hash = MD5(mid_plain_text).getstring();
			PublicKey client_private_key;
			client_private_key.nE = m_cParament.d;
			client_private_key.nN = m_cParament.n;
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
				UINT64 en_sixteen_bits = Encry(sixteen_bits, client_private_key);
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
			// 2.用对方的公钥(server_public_key)进行加密 由于合成的字符串每一个字符是4位 所以需要4个为一组
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
				UINT64 en_sixteen_bits = Encry(sixteen_bits, server_public_key);
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
			send(ServerSocket, en_text, strlen(en_text), 0);
			if (exit)
			{
				break;
			}



			// 二 客户端接收加密信息
			// 1.接收对方发过来的字符串
			recv(ServerSocket, ciphtext, 500, 0);
			// 2.用自己的私钥(client_private_key)解密发过来的字符串
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
			// 赋值为服务器的公钥（server_public_key）
			m_cParament.n = server_public_key.nN;
			m_cParament.d = server_public_key.nE;
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
				printf("C: [%s]经过解密后的明文:", time);
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
		}
		//如果用户选择退出，则向服务器发送退出请求
		printf("\nC: 退出...");
	}
	closesocket(ServerSocket);
	WSACleanup();
	return 0;
}