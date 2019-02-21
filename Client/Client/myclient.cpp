// myclient.cpp : ʵ���ļ�
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


// myclient ��Ա����


//��ʼ������
void myclient::init(CString &log)
{
	//���׽��ּ��˿�
	c_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == c_socket)
	{
		log += L"����ʧ��!\r\n";

	}
	SOCKADDR_IN addrSock;
	addrSock.sin_family = AF_INET;
	addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	int retval_bind;
	//��ͬ�ͻ��󶨲�ͬ�˿ڣ�ʵ�ֶ��û�
	while (true) {
		addrSock.sin_port = htons(port++);
		retval_bind = bind(c_socket, (SOCKADDR*)&addrSock, sizeof(SOCKADDR));
		if (retval_bind == 0)break;
		port++;
	}
	log += L"�ɹ�!\r\n";

	//����һ�����ڽ������ݵ��߳�
}

//�����������
void myclient::handleInteraction()
{
	CClientDlg* dlg=(CClientDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
	SOCKADDR_IN addrFrom;
	int len = sizeof(SOCKADDR);
	//dlg=(CClientDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
	mymessage recv;
	int mss_len;
	while(1)
	{
		int timeout = 2000;      //���ó�ʱʱ��Ϊ2s
		setsockopt(c_socket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(int));
		recv.type = -1;
		mss_len = recvfrom(c_socket,(char*)&recv,1000000,0,(SOCKADDR*)&addrFrom, &len);
		switch(recv.type)
		{
		case -1:
		{
			break;
		}
		//������ͬ���ȡ����Ŀ¼
		case AGREE_FILE_DIC:
		{
			dlg->m_file_list.ResetContent();
			dlg->m_log += L"������ͬ�ⷢ�͹���Ŀ¼\r\n";
			dlg->m_file.Empty();
			file_dic.clear();
			CString temp;//��ʱ���CString�ַ���
			//������Ŀ¼��ʾ�ڶԻ�����
			for(int i=0;i<BUF_SIZE;i++)
			{
				temp+=recv.mss_buffer[i];
				if(recv.mss_buffer[i+1]=='\r'&&recv.mss_buffer[i+2]=='\n')//�����������CString�ļ���
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
		//������ͬ���ϴ��ļ�
		case AGREE_UPLOAD_FILE:
		{
			mymessage send_mss;
			//���ļ������ļ��ļ�
			int batch_num = fread(send_mss.mss_buffer,1, BUF_SIZE , u_file);
			send_mss.type = START_UPLOAD_FILE;
			int j = 0;
			CString upload_file = dlg->m_upload;
			send_mss.len = batch_num;
			send_mss.seq = recv.ACK;
			CString str;
			str.Format(_T("ACK = %d\r\n"), recv.ACK);
			dlg->m_log += L"�ϴ���"+str;
			//�������ݳ���С��BUF_SIZE����֤���ϴ����
			if(batch_num < BUF_SIZE)
			{
				dlg->m_log += L"�ϴ��ļ����\r\n";
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
		//�ϴ���ɣ�������
		case END_UPLOAD_FILE:
		{

			c_posting = false;
			return;
		}
		//������ͬ�������ļ�
		case AGREE_DOWNLOAD_FILE:
		{
			if(recv.seq == -1)
			{
				dlg->m_log += L"��ʼ�����ļ�\r\n";
			}
			mymessage send_mss;
			send_mss.type = START_DOWNLOAD_FILE;
			//ACK��seq��ȣ���ACK��len��Ӻ��������
			if(full_ACK == recv.seq)
			{
				fwrite(recv.mss_buffer,recv.len,1,d_file);
				CString str;
				str.Format(_T("seq = %d\r\n"), recv.seq);
				dlg->m_log += L"������" + str;
				send_mss.type = AGREE_UPLOAD_FILE;
				full_ACK = send_mss.ACK = full_ACK + recv.len;
			}
			else
			{
				send_mss.ACK = full_ACK;
			}
			//����ΪΪtrue�Ҳ����ڿ�ʼ����״̬����ر��ļ����������
			if(recv.end_flag && recv.seq!=-1)
			{
				fclose(d_file);
				send_mss.type = END_DOWNLOAD_FILE;
				dlg->m_log += L"�����ļ����\r\n";
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
	// TODO: �ڴ����ר�ô����/����û���

	CSocket::OnSend(nErrorCode);
}


void myclient::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CSocket::OnReceive(nErrorCode);
}
