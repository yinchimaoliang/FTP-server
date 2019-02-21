
#ifndef __MYCLIENT_H__
#define __MYCLIENT_H__
#include"ClientDlg.h"
#pragma once
#include<string>
#include<vector>
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
// myclient ����Ŀ��
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
class myclient : public CSocket
{
public:
	myclient();
	virtual ~myclient();
	SOCKET c_socket;
	void init(CString &log);
	int port;
	void handleInteraction();
	virtual void OnSend(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
//	CString file_dic[1000];
	vector<CString> file_dic;
	bool c_posting;
//	FILE* u_upload;
	FILE* u_file;
	int u_full_file_len;
	FILE* d_file;
	int full_ACK;
};
#endif
