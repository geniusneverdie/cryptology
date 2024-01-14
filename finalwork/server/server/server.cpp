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
	//1.�����׽��ֿ�
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err)
		printf("S: socket����ʧ��\n");
	else
		printf("S: socket���سɹ�\n");
	//2.����һ���׽��ֹ�ʹ��
	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	//3.���׽��ְ󶨵����ص�ַ�Ͷ˿���
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6020);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(ServerSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR));
	//4.���׽�������Ϊ����ģʽ���Խ��տͻ�������
	err = listen(ServerSocket, 5);
	if (err)
		printf("S: �󶨶˿�ʧ�� ��������������\n");
	else
		printf("S: �ɹ��󶨶˿� �ȴ��ͻ�������\n");
	//5.�ȴ������տͻ������󣬷����µ������׽���
	SOCKADDR_IN addr_out;
	int len = sizeof(SOCKADDR);
	SOCKET ClientSocket = accept(ServerSocket, (SOCKADDR*)&addr_out, &len);



	// 6.������Կ ��Ҫ��RSA�Ĺ�Կ��˽Կ ��׼�������ͻ��� ��Ҫ������Ǵӿͻ��˽���
	// ���÷�����ȷ��� �ͻ��˺��͵Ĳ��� ��׼��һ��flag �����Ϊ0 �ͻ���Ϊ1
	// һ ����������
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
	printf("S: ��ͻ��˷��ͷ������Ĺ�Կ��:%s\n", send_to_client_public_key);
	send(ClientSocket, send_to_client_public_key, strlen(send_to_client_public_key), 0);

	// �� ����������
	char recv_from_client_public_key[100] = { 0 };
	recv(ClientSocket, recv_from_client_public_key, 100, 0);
	printf("S: �ӿͻ��˽��տͻ��˵Ĺ�Կ��:%s\n", recv_from_client_public_key);
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


	// ���ռ��ܵ�AES��Կ
	char aes_key[20] = { '\0' };
	char en_aes_key[300] = { '\0' };
	recv(ClientSocket, en_aes_key, 300, 0); // ������Կ
	printf("S: �ӿͻ��˽��յ����ܵ�AES��Կ:%s\n", en_aes_key);
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
	printf("S: ���ܵ�AES��ԿΪ:%s\n", aes_key);



	// ׼���շ���Ϣ
	char plaintext[500], ciphtext[500] = { 0 };
	while (true)
	{
		string str_aes_key = charkey_to_strkey(aes_key);

		// һ ���������ռ�����Ϣ
		// 1.���նԷ����������ַ���
		recv(ClientSocket, ciphtext, 500, 0);
		// 2.���Լ���˽Կ(server_private_key)���ܷ��������ַ���
		int len_of_ciphtext = 0;
		for (int i = 0; i < strlen(ciphtext) / 4; i++)
		{
			if (en_aes_key[len_of_ciphtext++] == '.')
			{
				// �жϵ����ܺ��hashֵ��
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
		// ���䣺��hash�Ľ��� ��Ҫ�÷������Ĺ�Կ����
		char recv_en_hash[100];
		// �ȱ���֮ǰ����Կ ��ֹ��ʧ
		UINT64 mid_n = m_cParament.n;
		UINT64 mid_d = m_cParament.d;
		UINT64 mid_e = m_cParament.e;
		// ��ֵΪ�ͻ��˵Ĺ�Կ��client_public_key��
		m_cParament.n = client_public_key.nN;
		m_cParament.d = client_public_key.nE;
		for (int i = 0; i < strlen(ciphtext) / 4; i++)
		{
			if (en_aes_key[len_of_ciphtext++] == '.')
			{
				// �жϵ����ܺ��hashֵ��
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
		// �ָ�֮ǰ�Ļ�����
		m_cParament.n = mid_n;
		m_cParament.d = mid_d;
		m_cParament.e = mid_e;
		string recv_hash = recv_en_hash;
		// 3.������AES����(�ڽ��ܺ����н����Ϊ128λһ�������)�������128λ������ ��Ҫʵ��CBC������� ����Ҫ����xor
		string de_mid_ciphtext = de_cbc_aes(mid_ciphtext, str_aes_key);
		// 4.������ܺ������(���hashֵ��֤��ȵĻ�)
		if (MD5(de_mid_ciphtext).getstring() == recv_hash)
		{
			char* time;
			strcpy(time, getTime());
			printf("S: [%s]�������ܺ������:", time);
			cout << de_mid_ciphtext;
			if (de_mid_ciphtext == "exit")
			{
				break;
			}
		}
		else
		{
			cout << "���ν���������Ҫ���½���" << endl;
			continue;
		}



		// �� ���������ͼ�����Ϣ
		// ���䣺���ù�ϣ��MD5�����м��� ���ֵ����˽Կ���� �����������һ�����
		printf("S: ����������:");
		setbuf(stdin, NULL);
		scanf("%[^\n]s", plaintext); // ʹ�ÿ��д����ȡ��϶����ǿո�
		bool exit = false;
		if (strcmp(plaintext, "exit") == 0) { exit = true; }
		string mid_plain_text = plaintext;
		// hashҲ��32��16���������ַ���
		string hash = MD5(mid_plain_text).getstring();
		PublicKey server_private_key;
		server_private_key.nE = m_cParament.d;
		server_private_key.nN = m_cParament.n;
		// ���Լ���˽Կȥ����
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
		// 1.������Ϣ ��������AES����Ϊ128λ������ ��Ҫʵ��CBC������� ����Ҫ����xor �����������������ݺϳ�һ���ַ���
		// Ϊ�˷�����������hash
		mid_plain_text += ".";
		mid_plain_text += wxn_en_hash;
		string mid_en_text = en_cbc_aes(mid_plain_text, str_aes_key); // ���ص���һ��CBC���������ϳɵ��ַ���
		// 2.�öԷ��Ĺ�Կ(client_public_key)���м��� ���ںϳɵ��ַ���ÿһ���ַ���4λ ������Ҫ4��Ϊһ��
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
		// 3.����������ַ�������
		send(ClientSocket, en_text, strlen(en_text), 0);
		if (exit)
		{
			break;
		}
	}
	printf("\nS: �˳�...");

	//�ر��׽���
	closesocket(ServerSocket);
	WSACleanup();
	return 0;
}