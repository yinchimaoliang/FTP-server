#ifndef __MYSERVER_H__
#define __MYSERVER_H__
#include"ServerDlg.h"
#include <direct.h>
#include<string.h>

#pragma once
#include<string>
#include<vector>
#include <io.h>
using namespace std;
using namespace std;
#define BUF_SIZE 20000
#define APPLY_FILE_DIC 0
#define AGREE_FILE_DIC 1
#define APPLY_UPLOAD_FILE 2
#define AGREE_UPLOAD_FILE 3
#define START_UPLOAD_FILE 4
#define END_UPLOAD_FILE 5
#define RECEIVE_UPLOAD_FILE 6
#define APPLY_DOWNLOAD_FILE 7
#define AGREE_DOWNLOAD_FILE 8
#define START_DOWNLOAD_FILE 9
#define END_DOWNLOAD_FILE 10

// myserver ����Ŀ��
//�Զ���Ϣ����
struct mymessage
{
	int type;//��Ϣ����
	char mss_buffer[BUF_SIZE];//��������
	char file_name[BUF_SIZE];//�����ļ���
	int len;//�������ݳ���
	int seq;//�����������
	int ACK;//������һ�����ݰ����
	bool end_flag;//������־λ
};
class myserver : public CSocket
{
public:
	myserver();
	virtual ~myserver();
	void init(CString &log);
	sockaddr_in servAddr;
	void handleInteraction();
	SOCKET s_socket;
	bool m_posting;
	FILE* u_file;
	int full_ACK;
	FILE* d_file;
};
#endif

