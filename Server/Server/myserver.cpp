// myserver.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "myserver.h"


// myserver

myserver::myserver()
{
	full_ACK = 0;
}

myserver::~myserver()
{
}


// myserver 成员函数


//绑定相应的套接字以及端口
void myserver::init(CString &log)
{
	s_socket = socket(AF_INET, SOCK_DGRAM, 0);
	servAddr.sin_family = PF_INET;  //使用IPv4地址
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //自动获取IP地址
    servAddr.sin_port = htons(1234);  //端口
    bind(s_socket, (SOCKADDR*)&servAddr, sizeof(SOCKADDR));
	log+=L"开始监听\r\n";
}


//处理交互过程
void myserver::handleInteraction()
{
	SOCKADDR clntAddr;  //客户端地址信息
    int nSize = sizeof(SOCKADDR);
    mymessage recv_mss;
	mymessage send_mss;
	bool end = false;
	int strLen;
    while(!end)
	{
		//设置超时重传
		int timeout = 2000;      //设置超时时间为2s
		setsockopt(s_socket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(int));
		recv_mss.type = -1;
		strLen = recvfrom(s_socket, (char*)&recv_mss, sizeof(mymessage), 0, &clntAddr, &nSize);
		switch (recv_mss.type)
		{
		//超时情况
		case -1:
		{
			sendto(s_socket, (char*)&send_mss, sizeof(mymessage), 0,(SOCKADDR*)&clntAddr, sizeof(SOCKADDR));
			return;
		}
		//客户端申请访问共享目录
		case APPLY_FILE_DIC:
		{
			CServerDlg* dlg=(CServerDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
			dlg->m_log+=L"收到请求共享目录消息.\r\n";
			send_mss.type = AGREE_FILE_DIC;
			for (int j = 1; j < BUF_SIZE; j++)
			{
				send_mss.mss_buffer[j] = 0;
			}
			char szFilePath[MAX_PATH + 1] = { 0 };
			GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
			(strrchr(szFilePath, '\\'))[0] = 0; // 删除文件名，只获得路径字串  
			string p;
			//文件句柄  
			long   hFile = 0;
			//文件信息  
			struct _finddata_t fileinfo;
			if ((hFile = _findfirst(p.assign(szFilePath).append("\\*").c_str(), &fileinfo)) != -1)
			{
				do
				{
					strncat(send_mss.mss_buffer, strncat(fileinfo.name,"\r\n",5), BUF_SIZE);
				} while (_findnext(hFile, &fileinfo) == 0);
				_findclose(hFile);
			}
			send_mss.mss_buffer[BUF_SIZE-1] = '\n';
			//displayShared(sendBuf);
			sendto(s_socket, (char*)&send_mss, sizeof(mymessage), 0,(SOCKADDR*)&clntAddr, sizeof(SOCKADDR));
			end = true;
			dlg->m_log+=L"发送共享文件目录\r\n";
			break;
		}
		//客户端申请上传文件
		case APPLY_UPLOAD_FILE:
		{
			CServerDlg* dlg=(CServerDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
			dlg->m_log+=L"收到请求上传文件消息.\r\n";
			//设置发送信息的类型为AGREE_UPLOAD_FILE
			mymessage send_mss;
			send_mss.type = AGREE_UPLOAD_FILE;
			send_mss.ACK = 0;
			sendto(s_socket, (char*)&send_mss, strLen, 0,(SOCKADDR*)&clntAddr, sizeof(SOCKADDR));
			char path[MAX_PATH + 1] = { 0 };
			GetModuleFileNameA(NULL, path, MAX_PATH);
			(strrchr(path, '\\'))[0] = 0; // 删除文件名，只获得路径字串
			full_ACK = 0;
			u_file = fopen(strcat(path,recv_mss.file_name),"wb");
			end = true;
			dlg->m_log+=L"发送同意上传文件消息\r\n";
			break;
		}
		//客户端开始上传文件
		case START_UPLOAD_FILE:
		{
			CServerDlg* dlg=(CServerDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
			//发送的ACK与初始seq相等，则继续发送
			if(recv_mss.seq == full_ACK)
			{
				fwrite(recv_mss.mss_buffer,recv_mss.len,1,u_file);
				send_mss.type = AGREE_UPLOAD_FILE;
				full_ACK = send_mss.ACK = full_ACK + recv_mss.len;
				CString str;
				str.Format(_T("seq = %d\r\n"), full_ACK);
				dlg->m_log += L"接收中" + str;
			}
			//如果不相等，则继续发送上一个ACK
			else
			{
				send_mss.ACK = full_ACK;
			}
			if(recv_mss.end_flag)
			{
				fclose(u_file);
				send_mss.type = END_UPLOAD_FILE;
				dlg->m_log += L"接收上传文件完成\r\n";
			}
			else
			{
				send_mss.type = AGREE_UPLOAD_FILE;
			}
			sendto(s_socket, (char*)&send_mss, sizeof(mymessage), 0,(SOCKADDR*)&clntAddr, sizeof(SOCKADDR));
			end = true;
			break;
		}
		//客户端申请下载文件
		case APPLY_DOWNLOAD_FILE:
		{
			CServerDlg* dlg=(CServerDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
			dlg->m_log+=L"收到请求下载文件消息.\r\n";
			mymessage send_mss;
			send_mss.type = AGREE_DOWNLOAD_FILE;
			send_mss.seq = -1;
			sendto(s_socket, (char*)&send_mss, sizeof(mymessage), 0,(SOCKADDR*)&clntAddr, sizeof(SOCKADDR));
			dlg->m_log+=L"发送同意下载文件消息.\r\n";
			//dlg->m_log+=CString(recv_mss.mss_buffer);
			char path[MAX_PATH + 1] = { 0 };
			GetModuleFileNameA(NULL, path, MAX_PATH);
			(strrchr(path, '\\'))[0] = 0; // 删除文件名，只获得路径字串
			full_ACK = 0;
			strcat(path,"\\");
			//用文件流打开文件
			d_file = fopen(strcat(path,recv_mss.file_name),"rb");
			end = true;
			break;

		}
		//客户端开始下载文件
		case START_DOWNLOAD_FILE:
		{
			mymessage send_mss;
			int batch_num = fread(send_mss.mss_buffer,1, BUF_SIZE , d_file);
			CServerDlg* dlg=(CServerDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
			CString str;
			str.Format(_T("ACK = %d\r\n"), recv_mss.ACK);
			dlg->m_log+=L"开始下载"+str;
			send_mss.len = batch_num;
			send_mss.seq = recv_mss.ACK;
			//如果文件流读出来的大小小于BUF_SIZE，则证明下载文件完毕，设置结束标志位。
			if(batch_num < BUF_SIZE)
			{
				dlg->m_log += L"下载文件完成\r\n";
				send_mss.end_flag = true;
			}
			else
			{
				send_mss.end_flag = false;
			}
			send_mss.type = AGREE_DOWNLOAD_FILE;
			sendto(s_socket,(char*)&send_mss, sizeof(mymessage), 0,
						(SOCKADDR*)&clntAddr, sizeof(SOCKADDR));
			end = true;
			break;
		}
		//客户端下载结束
		case END_DOWNLOAD_FILE:
		{
			break;
		}
		}
	
	}
}