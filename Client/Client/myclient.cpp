// myclient.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "myclient.h"


// myclient

myclient::myclient()
{
	port = 8765;
	u_full_file_len = 0;
	full_ACK = 0;
}

myclient::~myclient()
{
}


// myclient 成员函数


//初始化操作
void myclient::init(CString &log)
{
	//绑定套接字及端口
	c_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == c_socket)
	{
		log += L"创建失败!\r\n";

	}
	SOCKADDR_IN addrSock;
	addrSock.sin_family = AF_INET;
	addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	int retval_bind;
	//不同客户绑定不同端口，实现多用户
	while (true) {
		addrSock.sin_port = htons(port++);
		retval_bind = bind(c_socket, (SOCKADDR*)&addrSock, sizeof(SOCKADDR));
		if (retval_bind == 0)break;
		port++;
	}
	log += L"成功!\r\n";

	//产生一个用于接收数据的线程
}

//交互处理程序
void myclient::handleInteraction()
{
	CClientDlg* dlg=(CClientDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
	SOCKADDR_IN addrFrom;
	int len = sizeof(SOCKADDR);
	//dlg=(CClientDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
	mymessage recv;
	int mss_len;
	while(1)
	{
		int timeout = 2000;      //设置超时时间为2s
		setsockopt(c_socket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(int));
		recv.type = -1;
		mss_len = recvfrom(c_socket,(char*)&recv,1000000,0,(SOCKADDR*)&addrFrom, &len);
		switch(recv.type)
		{
		case -1:
		{
			break;
		}
		//服务器同意获取共享目录
		case AGREE_FILE_DIC:
		{
			dlg->m_file_list.ResetContent();
			dlg->m_log += L"服务器同意发送共享目录\r\n";
			dlg->m_file.Empty();
			file_dic.clear();
			CString temp;//临时存放CString字符串
			//将共享目录显示在对话框中
			for(int i=0;i<BUF_SIZE;i++)
			{
				temp+=recv.mss_buffer[i];
				if(recv.mss_buffer[i+1]=='\r'&&recv.mss_buffer[i+2]=='\n')//往向量中添加CString文件名
				{
					file_dic.insert(file_dic.end(),temp);
					i+=2;
					temp.Empty();
				}
				//dlg->m_file+=recv.mss_buffer[i];
			}
			for(int k = 2;k<file_dic.size();k++)
			{
				dlg->m_file_list.AddString(file_dic[k]);
				//dlg->m_file+=file_dic[k]+L"\r\n";
			}
			dlg->GetDlgItem(IDC_LOG)->SetWindowTextW(dlg->m_log);
			
			return;
		}
		//服务器同意上传文件
		case AGREE_UPLOAD_FILE:
		{
			mymessage send_mss;
			//用文件流读文件文件
			int batch_num = fread(send_mss.mss_buffer,1, BUF_SIZE , u_file);
			send_mss.type = START_UPLOAD_FILE;
			int j = 0;
			CString upload_file = dlg->m_upload;
			send_mss.len = batch_num;
			send_mss.seq = recv.ACK;
			CString str;
			str.Format(_T("ACK = %d\r\n"), recv.ACK);
			dlg->m_log += L"上传中"+str;
			//读出内容长度小于BUF_SIZE，则证明上传完成
			if(batch_num < BUF_SIZE)
			{
				dlg->m_log += L"上传文件完成\r\n";
				send_mss.end_flag = true;
			}
			else
			{
				send_mss.end_flag = false;
			}
			sendto(c_socket,(char*)&send_mss, sizeof(mymessage), 0,
						(SOCKADDR*)&addrFrom, sizeof(SOCKADDR));
			dlg->GetDlgItem(IDC_LOG)->SetWindowTextW(dlg->m_log);
			
			return;
		}
		//上传完成，则跳出
		case END_UPLOAD_FILE:
		{

			c_posting = false;
			return;
		}
		//服务器同意下载文件
		case AGREE_DOWNLOAD_FILE:
		{
			if(recv.seq == -1)
			{
				dlg->m_log += L"开始下载文件\r\n";
			}
			mymessage send_mss;
			send_mss.type = START_DOWNLOAD_FILE;
			//ACK与seq相等，则ACK与len相加后继续发送
			if(full_ACK == recv.seq)
			{
				fwrite(recv.mss_buffer,recv.len,1,d_file);
				CString str;
				str.Format(_T("seq = %d\r\n"), recv.seq);
				dlg->m_log += L"下载中" + str;
				send_mss.type = AGREE_UPLOAD_FILE;
				full_ACK = send_mss.ACK = full_ACK + recv.len;
			}
			else
			{
				send_mss.ACK = full_ACK;
			}
			//结束为为true且不处于开始发送状态，则关闭文件，下载完成
			if(recv.end_flag && recv.seq!=-1)
			{
				fclose(d_file);
				send_mss.type = END_DOWNLOAD_FILE;
				dlg->m_log += L"下载文件完成\r\n";
			}
			else
			{
				send_mss.type = START_DOWNLOAD_FILE;
			}
			sendto(c_socket,(char*)&send_mss,sizeof(mymessage),0,(SOCKADDR*)&addrFrom,sizeof(SOCKADDR));
			return;
		}
		}
	}
	mymessage temp;
	temp.type = 0;
	sendto(c_socket,(char*)&temp,sizeof(mymessage),0,(SOCKADDR*)&addrFrom,len);
	return;
}


void myclient::OnSend(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CSocket::OnSend(nErrorCode);
}


void myclient::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CSocket::OnReceive(nErrorCode);
}
