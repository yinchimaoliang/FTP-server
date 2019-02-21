// myserver.cpp : ʵ���ļ�
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


// myserver ��Ա����


//����Ӧ���׽����Լ��˿�
void myserver::init(CString &log)
{
	s_socket = socket(AF_INET, SOCK_DGRAM, 0);
	servAddr.sin_family = PF_INET;  //ʹ��IPv4��ַ
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //�Զ���ȡIP��ַ
    servAddr.sin_port = htons(1234);  //�˿�
    bind(s_socket, (SOCKADDR*)&servAddr, sizeof(SOCKADDR));
	log+=L"��ʼ����\r\n";
}


//����������
void myserver::handleInteraction()
{
	SOCKADDR clntAddr;  //�ͻ��˵�ַ��Ϣ
    int nSize = sizeof(SOCKADDR);
    mymessage recv_mss;
	mymessage send_mss;
	bool end = false;
	int strLen;
    while(!end)
	{
		//���ó�ʱ�ش�
		int timeout = 2000;      //���ó�ʱʱ��Ϊ2s
		setsockopt(s_socket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(int));
		recv_mss.type = -1;
		strLen = recvfrom(s_socket, (char*)&recv_mss, sizeof(mymessage), 0, &clntAddr, &nSize);
		switch (recv_mss.type)
		{
		//��ʱ���
		case -1:
		{
			sendto(s_socket, (char*)&send_mss, sizeof(mymessage), 0,(SOCKADDR*)&clntAddr, sizeof(SOCKADDR));
			return;
		}
		//�ͻ���������ʹ���Ŀ¼
		case APPLY_FILE_DIC:
		{
			CServerDlg* dlg=(CServerDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
			dlg->m_log+=L"�յ�������Ŀ¼��Ϣ.\r\n";
			send_mss.type = AGREE_FILE_DIC;
			for (int j = 1; j < BUF_SIZE; j++)
			{
				send_mss.mss_buffer[j] = 0;
			}
			char szFilePath[MAX_PATH + 1] = { 0 };
			GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
			(strrchr(szFilePath, '\\'))[0] = 0; // ɾ���ļ�����ֻ���·���ִ�  
			string p;
			//�ļ����  
			long   hFile = 0;
			//�ļ���Ϣ  
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
			dlg->m_log+=L"���͹����ļ�Ŀ¼\r\n";
			break;
		}
		//�ͻ��������ϴ��ļ�
		case APPLY_UPLOAD_FILE:
		{
			CServerDlg* dlg=(CServerDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
			dlg->m_log+=L"�յ������ϴ��ļ���Ϣ.\r\n";
			//���÷�����Ϣ������ΪAGREE_UPLOAD_FILE
			mymessage send_mss;
			send_mss.type = AGREE_UPLOAD_FILE;
			send_mss.ACK = 0;
			sendto(s_socket, (char*)&send_mss, strLen, 0,(SOCKADDR*)&clntAddr, sizeof(SOCKADDR));
			char path[MAX_PATH + 1] = { 0 };
			GetModuleFileNameA(NULL, path, MAX_PATH);
			(strrchr(path, '\\'))[0] = 0; // ɾ���ļ�����ֻ���·���ִ�
			full_ACK = 0;
			u_file = fopen(strcat(path,recv_mss.file_name),"wb");
			end = true;
			dlg->m_log+=L"����ͬ���ϴ��ļ���Ϣ\r\n";
			break;
		}
		//�ͻ��˿�ʼ�ϴ��ļ�
		case START_UPLOAD_FILE:
		{
			CServerDlg* dlg=(CServerDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
			//���͵�ACK���ʼseq��ȣ����������
			if(recv_mss.seq == full_ACK)
			{
				fwrite(recv_mss.mss_buffer,recv_mss.len,1,u_file);
				send_mss.type = AGREE_UPLOAD_FILE;
				full_ACK = send_mss.ACK = full_ACK + recv_mss.len;
				CString str;
				str.Format(_T("seq = %d\r\n"), full_ACK);
				dlg->m_log += L"������" + str;
			}
			//�������ȣ������������һ��ACK
			else
			{
				send_mss.ACK = full_ACK;
			}
			if(recv_mss.end_flag)
			{
				fclose(u_file);
				send_mss.type = END_UPLOAD_FILE;
				dlg->m_log += L"�����ϴ��ļ����\r\n";
			}
			else
			{
				send_mss.type = AGREE_UPLOAD_FILE;
			}
			sendto(s_socket, (char*)&send_mss, sizeof(mymessage), 0,(SOCKADDR*)&clntAddr, sizeof(SOCKADDR));
			end = true;
			break;
		}
		//�ͻ������������ļ�
		case APPLY_DOWNLOAD_FILE:
		{
			CServerDlg* dlg=(CServerDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
			dlg->m_log+=L"�յ����������ļ���Ϣ.\r\n";
			mymessage send_mss;
			send_mss.type = AGREE_DOWNLOAD_FILE;
			send_mss.seq = -1;
			sendto(s_socket, (char*)&send_mss, sizeof(mymessage), 0,(SOCKADDR*)&clntAddr, sizeof(SOCKADDR));
			dlg->m_log+=L"����ͬ�������ļ���Ϣ.\r\n";
			//dlg->m_log+=CString(recv_mss.mss_buffer);
			char path[MAX_PATH + 1] = { 0 };
			GetModuleFileNameA(NULL, path, MAX_PATH);
			(strrchr(path, '\\'))[0] = 0; // ɾ���ļ�����ֻ���·���ִ�
			full_ACK = 0;
			strcat(path,"\\");
			//���ļ������ļ�
			d_file = fopen(strcat(path,recv_mss.file_name),"rb");
			end = true;
			break;

		}
		//�ͻ��˿�ʼ�����ļ�
		case START_DOWNLOAD_FILE:
		{
			mymessage send_mss;
			int batch_num = fread(send_mss.mss_buffer,1, BUF_SIZE , d_file);
			CServerDlg* dlg=(CServerDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
			CString str;
			str.Format(_T("ACK = %d\r\n"), recv_mss.ACK);
			dlg->m_log+=L"��ʼ����"+str;
			send_mss.len = batch_num;
			send_mss.seq = recv_mss.ACK;
			//����ļ����������Ĵ�СС��BUF_SIZE����֤�������ļ���ϣ����ý�����־λ��
			if(batch_num < BUF_SIZE)
			{
				dlg->m_log += L"�����ļ����\r\n";
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
		//�ͻ������ؽ���
		case END_DOWNLOAD_FILE:
		{
			break;
		}
		}
	
	}
}