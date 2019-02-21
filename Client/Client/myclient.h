
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
// myclient 命令目标
//自定信息类型
struct mymessage
{
	int type;//消息类型
	char mss_buffer[BUF_SIZE];//发送数据
	char file_name[BUF_SIZE];//发送文件名
	int len;//发送数据长度
	int seq;//发送数据序号
	int ACK;//申请下一个数据包序号
	bool end_flag;//结束标志位
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
