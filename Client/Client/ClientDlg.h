
// ClientDlg.h : ͷ�ļ�
//
#ifndef __CLIENTDLG_H__
#define __CLIENTDLG_H__
extern class myclient;
#pragma once


// CClientDlg �Ի���
class CClientDlg : public CDialogEx
{
// ����
public:
	CClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	myclient* client;
	CString m_log;
	static DWORD WINAPI handleReceive(LPVOID lpParameter);
	afx_msg void OnBnClickedFileDic();
	SOCKADDR_IN servAddr;
	CString m_file;
	CString m_upload;
	afx_msg void OnBnClickedUploadFileButton();
	afx_msg void OnBnClickedStartUpload();
	CListBox m_file_list;
	CButton m_abc;
	afx_msg void OnBnClickedDownloadFileButton();
//	CString m_download_file;
	afx_msg void OnBnClickedStartDownload();
//	CEdit m_download;
	CString m_download;
};
#endif