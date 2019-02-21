
// ClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"
#include"myclient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CClientDlg 对话框




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


// CClientDlg 消息处理程序

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	client = new myclient();
	client->init(m_log);
	HANDLE hThread = CreateThread(NULL, 0, handleReceive, (LPVOID)client, 0, NULL);//利用多线程
	UpdateData(false);

	servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(1234);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//线程调用函数
DWORD WINAPI CClientDlg::handleReceive(LPVOID lpParameter)
{
	myclient* t = (myclient*)lpParameter;
	CString log;
	while(1)
	{
		log.Empty();
		t->handleInteraction();
		CClientDlg* dlg=(CClientDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
		dlg->m_log+=log;
		dlg->GetDlgItem(IDC_LOG)->SetWindowTextW(dlg->m_log);
	}
	return (DWORD)NULL;
}


//点击获取共享目录按钮
void CClientDlg::OnBnClickedFileDic()
{
	mymessage send_mss;
	send_mss.type = 0;
	//sendto(c_socket,(char*)&temp,sizeof(message),0,(SOCKADDR*)&addrFrom,len);
	sendto(client->c_socket,(char*)&send_mss,sizeof(mymessage),0,(SOCKADDR*)&servAddr,sizeof(SOCKADDR));
	CClientDlg* dlg=(CClientDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
	dlg->m_log += L"开始交互\r\n";
	dlg->m_log+=L"发送查看共享目录请求\r\n";
	dlg->GetDlgItem(IDC_LOG)->SetWindowTextW(dlg->m_log);
	// TODO: 在此添加控件通知处理程序代码
}

//点击选择文件按钮
void CClientDlg::OnBnClickedUploadFileButton()
{
	//代开文件选择对话框
	CFileDialog openFile(true);
	if (IDOK == openFile.DoModal())
	{
		m_upload = openFile.GetPathName();
		UpdateData(false);
	}
}

//点击开始上传按钮
void CClientDlg::OnBnClickedStartUpload()
{
	CClientDlg* dlg=(CClientDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
	dlg->m_log += L"开始交互\r\n";
	dlg->m_log+=L"发送上传文件命令\r\n";
	dlg->GetDlgItem(IDC_LOG)->SetWindowTextW(dlg->m_log);
	//用文件流打开所选择的文件
	CString upload_file = dlg->m_upload;
	CStringA file_stra(upload_file.GetBuffer(0));
	upload_file.ReleaseBuffer();
	string s = file_stra.GetBuffer(0);
	const char* file_con = s.c_str();
	if (!(client->u_file = fopen(file_con, "rb")))
	{
		AfxMessageBox(_T("创建本地文件失败！"));
	}
	mymessage send_mss;
	int j = 0;
	for(int i = 0;i<upload_file.GetLength();i++)//获取文件名
	{
		if(upload_file[i] == '\\')
		{
			j = 0;
		}
		send_mss.file_name[j] = upload_file[i];
		j++;
	}
	send_mss.file_name[j] = 0;
	//设置消息类型为申请上传文件
	send_mss.type = APPLY_UPLOAD_FILE;
	send_mss.seq = 0;
	sendto(client->c_socket,(char*)&send_mss,sizeof(mymessage),0,(SOCKADDR*)&servAddr,sizeof(SOCKADDR));
	if(dlg->m_upload.GetLength()==0)//如果文件选择栏为空，则弹出对话框
	{
		MessageBox(_T("上传文件为空！"));
	}
	dlg->GetDlgItem(IDC_LOG)->SetWindowTextW(dlg->m_log);
	// TODO: 在此添加控件通知处理程序代码
}


//点击下载选择文件按钮
void CClientDlg::OnBnClickedDownloadFileButton()
{
	//从ListBox中获取所选择的文件名
	CString download_file_name;
	m_file_list.GetText(m_file_list.GetCurSel(),download_file_name);
	m_download = download_file_name;
	UpdateData(false);
	// TODO: 在此添加控件通知处理程序代码
}

//点击开始下载按钮
void CClientDlg::OnBnClickedStartDownload()
{
	CClientDlg* dlg=(CClientDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
	dlg->m_log += L"开始交互\r\n";
	dlg->m_log+=L"发送下载文件命令\r\n";
	mymessage send_mss;
	//设置消息类型为申请下载
	send_mss.type = APPLY_DOWNLOAD_FILE;
	//获取文件名
	for(int i=0;i<m_download.GetLength();i++)
	{
		send_mss.file_name[i] = m_download[i];
	}
	send_mss.file_name[m_download.GetLength()]=0;
	if(dlg->m_download.GetLength()==0)//如果文件选择栏为空，则弹出对话框
	{
		MessageBox(_T("下载文件为空！"));
	}
	char path[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, path, MAX_PATH);
	(strrchr(path, '\\'))[0] = 0; // 删除文件名，只获得路径字串
	client->full_ACK = 0;
	strcat(path,"\\");
	client->d_file = fopen(strcat(path,send_mss.file_name),"wb");
	sendto(client->c_socket,(char*)&send_mss,sizeof(mymessage),0,(SOCKADDR*)&servAddr,sizeof(SOCKADDR));
	dlg->GetDlgItem(IDC_LOG)->SetWindowTextW(dlg->m_log);
}
