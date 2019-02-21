
// ClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"
#include"myclient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientDlg �Ի���




CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_log = _T("");
	m_file = _T("");
	m_upload = _T("");
	//  m_download_file = _T("");
	m_download = _T("");
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LOG, m_log);
	//DDX_Text(pDX, IDC_FILE, m_file);
	DDX_Text(pDX, IDC_UPLOAD_FILE, m_upload);
	DDX_Control(pDX, IDC_FILE_LIST, m_file_list);
	//DDX_Control(pDX, IDC_FILE_DIC, m_abc);
	//  DDX_Text(pDX, IDC_DOWNLOAD_FILE, m_download_file);
	//  DDX_Control(pDX, IDC_DOWNLOAD_FILE, m_download);
	DDX_Text(pDX, IDC_DOWNLOAD_FILE, m_download);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_FILE_DIC, &CClientDlg::OnBnClickedFileDic)
	ON_BN_CLICKED(IDC_UPLOAD_FILE_BUTTON, &CClientDlg::OnBnClickedUploadFileButton)
	ON_BN_CLICKED(IDC_START_UPLOAD, &CClientDlg::OnBnClickedStartUpload)
	ON_BN_CLICKED(IDC_DOWNLOAD_FILE_BUTTON, &CClientDlg::OnBnClickedDownloadFileButton)
	ON_BN_CLICKED(IDC_START_DOWNLOAD, &CClientDlg::OnBnClickedStartDownload)
END_MESSAGE_MAP()


// CClientDlg ��Ϣ�������

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	client = new myclient();
	client->init(m_log);
	HANDLE hThread = CreateThread(NULL, 0, handleReceive, (LPVOID)client, 0, NULL);//���ö��߳�
	UpdateData(false);

	servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(1234);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//�̵߳��ú���
DWORD WINAPI CClientDlg::handleReceive(LPVOID lpParameter)
{
	myclient* t = (myclient*)lpParameter;
	CString log;
	while(1)
	{
		log.Empty();
		t->handleInteraction();
		CClientDlg* dlg=(CClientDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
		dlg->m_log+=log;
		dlg->GetDlgItem(IDC_LOG)->SetWindowTextW(dlg->m_log);
	}
	return (DWORD)NULL;
}


//�����ȡ����Ŀ¼��ť
void CClientDlg::OnBnClickedFileDic()
{
	mymessage send_mss;
	send_mss.type = 0;
	//sendto(c_socket,(char*)&temp,sizeof(message),0,(SOCKADDR*)&addrFrom,len);
	sendto(client->c_socket,(char*)&send_mss,sizeof(mymessage),0,(SOCKADDR*)&servAddr,sizeof(SOCKADDR));
	CClientDlg* dlg=(CClientDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
	dlg->m_log += L"��ʼ����\r\n";
	dlg->m_log+=L"���Ͳ鿴����Ŀ¼����\r\n";
	dlg->GetDlgItem(IDC_LOG)->SetWindowTextW(dlg->m_log);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

//���ѡ���ļ���ť
void CClientDlg::OnBnClickedUploadFileButton()
{
	//�����ļ�ѡ��Ի���
	CFileDialog openFile(true);
	if (IDOK == openFile.DoModal())
	{
		m_upload = openFile.GetPathName();
		UpdateData(false);
	}
}

//�����ʼ�ϴ���ť
void CClientDlg::OnBnClickedStartUpload()
{
	CClientDlg* dlg=(CClientDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
	dlg->m_log += L"��ʼ����\r\n";
	dlg->m_log+=L"�����ϴ��ļ�����\r\n";
	dlg->GetDlgItem(IDC_LOG)->SetWindowTextW(dlg->m_log);
	//���ļ�������ѡ����ļ�
	CString upload_file = dlg->m_upload;
	CStringA file_stra(upload_file.GetBuffer(0));
	upload_file.ReleaseBuffer();
	string s = file_stra.GetBuffer(0);
	const char* file_con = s.c_str();
	if (!(client->u_file = fopen(file_con, "rb")))
	{
		AfxMessageBox(_T("���������ļ�ʧ�ܣ�"));
	}
	mymessage send_mss;
	int j = 0;
	for(int i = 0;i<upload_file.GetLength();i++)//��ȡ�ļ���
	{
		if(upload_file[i] == '\\')
		{
			j = 0;
		}
		send_mss.file_name[j] = upload_file[i];
		j++;
	}
	send_mss.file_name[j] = 0;
	//������Ϣ����Ϊ�����ϴ��ļ�
	send_mss.type = APPLY_UPLOAD_FILE;
	send_mss.seq = 0;
	sendto(client->c_socket,(char*)&send_mss,sizeof(mymessage),0,(SOCKADDR*)&servAddr,sizeof(SOCKADDR));
	if(dlg->m_upload.GetLength()==0)//����ļ�ѡ����Ϊ�գ��򵯳��Ի���
	{
		MessageBox(_T("�ϴ��ļ�Ϊ�գ�"));
	}
	dlg->GetDlgItem(IDC_LOG)->SetWindowTextW(dlg->m_log);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


//�������ѡ���ļ���ť
void CClientDlg::OnBnClickedDownloadFileButton()
{
	//��ListBox�л�ȡ��ѡ����ļ���
	CString download_file_name;
	m_file_list.GetText(m_file_list.GetCurSel(),download_file_name);
	m_download = download_file_name;
	UpdateData(false);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

//�����ʼ���ذ�ť
void CClientDlg::OnBnClickedStartDownload()
{
	CClientDlg* dlg=(CClientDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
	dlg->m_log += L"��ʼ����\r\n";
	dlg->m_log+=L"���������ļ�����\r\n";
	mymessage send_mss;
	//������Ϣ����Ϊ��������
	send_mss.type = APPLY_DOWNLOAD_FILE;
	//��ȡ�ļ���
	for(int i=0;i<m_download.GetLength();i++)
	{
		send_mss.file_name[i] = m_download[i];
	}
	send_mss.file_name[m_download.GetLength()]=0;
	if(dlg->m_download.GetLength()==0)//����ļ�ѡ����Ϊ�գ��򵯳��Ի���
	{
		MessageBox(_T("�����ļ�Ϊ�գ�"));
	}
	char path[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, path, MAX_PATH);
	(strrchr(path, '\\'))[0] = 0; // ɾ���ļ�����ֻ���·���ִ�
	client->full_ACK = 0;
	strcat(path,"\\");
	client->d_file = fopen(strcat(path,send_mss.file_name),"wb");
	sendto(client->c_socket,(char*)&send_mss,sizeof(mymessage),0,(SOCKADDR*)&servAddr,sizeof(SOCKADDR));
	dlg->GetDlgItem(IDC_LOG)->SetWindowTextW(dlg->m_log);
}
