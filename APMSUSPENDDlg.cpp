
// APMSUSPENDDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "APMSUSPEND.h"
#include "APMSUSPENDDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define WM_TASKTRAY     WM_USER + 1 

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	// CreateProcess()で取得したPROCESS_INFORMATION構造体のポインタを取得
	PROCESS_INFORMATION* pi = (PROCESS_INFORMATION*)lParam;

	// ウインドウを作成したプロセスIDを取得。
	DWORD lpdwProcessId = 0;
	::GetWindowThreadProcessId(hWnd, &lpdwProcessId);

	// CreateProcessで起動したアプリのプロセスIDとメインウィンドウを
	// 作成したプロセスIDが同じ場合、起動したアプリを終了させる。
	if (pi->dwProcessId == lpdwProcessId)
	{
		::PostMessage(hWnd, WM_CLOSE, 0, 0);
		return FALSE;
	}
	return TRUE;
}

//スレッドプロシージャ
UINT  CAPMSUSPENDDlg::Call_Target_end_detect(LPVOID p)
{
	TRACE(_T("\r\nSTART Call_Target_end_detect\r\n"));
	CAPMSUSPENDDlg* pDlg = (CAPMSUSPENDDlg*)p;

//このスレッドが生存中はLockしておく
	CSingleLock alock(&(pDlg->m_sm_Callback_Alive));
	alock.Lock();

	CSingleLock lock(&(pDlg->m_sem));

	WaitForSingleObject(pDlg->m_PI.hProcess, INFINITE);

	lock.Lock();
	pDlg->m_DlgItem_Powerlamp = 0;

	CloseHandle(pDlg->m_PI.hThread);
	CloseHandle(pDlg->m_PI.hProcess);
	pDlg->m_PI.hThread = NULL;
	pDlg->m_PI.hProcess = NULL;

	lock.Unlock();

	TRACE(_T("\r\nEND Call_Target_end_detect\r\n"));

	alock.Unlock();
	pDlg->m_button_create_process.EnableWindow(TRUE);
	return 321;
}

// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg UINT OnPowerBroadcast(UINT nPowerEvent, LPARAM nEventData);
protected:
//	CFileDialog* m_pDlg;
public:
//	afx_msg void OnIdrMenuMain();
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAPMSUSPENDDlg ダイアログ



CAPMSUSPENDDlg::CAPMSUSPENDDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_APMSUSPEND_DIALOG, pParent), m_sem()
, m_radiko_support_on(FALSE)
, m_is_state_save(FALSE)
, m_show_dlg_state(0)
, m_use_task_tray(FALSE)
,m_sm_Callback_Alive()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_PI.hProcess = NULL;
	m_PI.hThread = NULL;
	m_pThread = NULL;
	m_DlgItem_Powerlamp = 0;
	m_IsHide = FALSE;

	m_is_power_state = 0;
	m_Radiko_had_startup = 0;
	m_resume_wait_time = 0;
}
CAPMSUSPENDDlg::~CAPMSUSPENDDlg() {

}

void CAPMSUSPENDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_TARGET_PATH, m_Target_path);
	DDX_Check(pDX, IDC_CHECK_RADIKO_SUPPORT_ON, m_radiko_support_on);
	DDX_Check(pDX, IDC_CHECK_MAINTAIN, m_is_state_save);
	DDX_Radio(pDX, IDC_RADIO_MAIN_DLG_SHOW_STATUS, m_show_dlg_state);
	DDX_Check(pDX, IDC_CHECK_USE_TASK_TRAY, m_use_task_tray);
	DDX_Control(pDX, IDC_CHECK_USE_TASK_TRAY, m_check_use_tasktray);
	DDX_Control(pDX, IDC_CHECK_RADIKO_SUPPORT_ON, m_check_radiko_suport_on);
	DDX_Control(pDX, IDC_STATIC_TARGET_PATH, m_static_target_path);
	DDX_Control(pDX, IDC_BUTTON_CREATE_PROCESS, m_button_create_process);
	DDX_Control(pDX, IDC_BUTTON_TEST_RESUME, m_button_test_resume);
	DDX_Control(pDX, IDC_BUTTON_TEST_SUSPEND, m_button_test_suspend);
}

BEGIN_MESSAGE_MAP(CAPMSUSPENDDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_POWERBROADCAST()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_PATH, &CAPMSUSPENDDlg::OnBnClickedButtonOpenPath)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_PROCESS, &CAPMSUSPENDDlg::OnBnClickedButtonCreateProcess)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_PROCESS, &CAPMSUSPENDDlg::OnBnClickedButtonCloseProcess)
	ON_WM_WINDOWPOSCHANGING()
ON_MESSAGE(WM_TASKTRAY, &CAPMSUSPENDDlg::OnTasktray)
ON_WM_DESTROY()
ON_COMMAND(ID_APP_SHOW, &CAPMSUSPENDDlg::OnAppShow)
ON_COMMAND(ID_APP_ICON, &CAPMSUSPENDDlg::OnAppIcon)
ON_COMMAND(ID_APP_SAVE_AND_CLOSE, &CAPMSUSPENDDlg::OnAppClose)
ON_COMMAND(ID_RADIKO_SET_PATH, &CAPMSUSPENDDlg::OnRadikoSetPath)
ON_COMMAND(ID_RADIKO_START_UP, &CAPMSUSPENDDlg::OnRadikoStartUp)
ON_COMMAND(ID_RADIKO_CLOSE, &CAPMSUSPENDDlg::OnRadikoClose)
ON_BN_CLICKED(IDOK, &CAPMSUSPENDDlg::OnBnClickedOk)
ON_BN_CLICKED(IDC_CHECK_USE_TASK_TRAY, &CAPMSUSPENDDlg::OnClickedCheckUseTaskTray)
ON_BN_CLICKED(IDC_BUTTON_DO_TASKTRAY, &CAPMSUSPENDDlg::OnBnClickedButtonDoTasktray)
ON_BN_CLICKED(IDCANCEL, &CAPMSUSPENDDlg::OnBnClickedCancel)
ON_COMMAND(ID_APP_CANCEL, &CAPMSUSPENDDlg::OnAppCancel)
ON_STN_CLICKED(IDC_STATIC_PICTURE_CONTROL_POWER_INDIGATE, &CAPMSUSPENDDlg::OnClickedStaticPictureControlPowerIndigate)
ON_WM_DRAWITEM()
ON_BN_CLICKED(IDC_BUTTON_TEST_SUSPEND, &CAPMSUSPENDDlg::OnBnClickedButtonTestSuspend)
ON_BN_CLICKED(IDC_BUTTON_RESUME, &CAPMSUSPENDDlg::OnBnClickedButtonTestResume)
ON_BN_CLICKED(IDC_CHECK_MAINTAIN, &CAPMSUSPENDDlg::OnBnClickedCheckMaintain)
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_BUTTON_ADVANCED_OPTION, &CAPMSUSPENDDlg::OnBnClickedButtonAdvancedOption)
END_MESSAGE_MAP()


// CAPMSUSPENDDlg メッセージ ハンドラー

BOOL CAPMSUSPENDDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
#ifndef _DEBUG
	m_button_test_suspend.ShowWindow(SW_HIDE);
	m_button_test_resume.ShowWindow(SW_HIDE);
#endif
 

	CWinApp* pApp = AfxGetApp();
	m_Target_path = pApp->GetProfileString(_T("TARGET_APP"), _T("FILE_PATH"), _T("プログラムの場所を指定してください。"));
	m_is_state_save = pApp->GetProfileInt(_T("SYSTEM"), _T("SAVE_STARTUP_STATUS"), TRUE);
	m_radiko_support_on = pApp->GetProfileInt(_T("SYSTEM"), _T("RADIKO_SUPPORT_ON"), TRUE);
	m_show_dlg_state = pApp->GetProfileInt(_T("SYSTEM"), _T("SHOW_DLG_STATE"), 0);
	m_use_task_tray = pApp->GetProfileInt(_T("SYSTEM"), _T("USE_TASK_TRAY"), TRUE);
	m_DlgItem_Powerlamp = pApp->GetProfileInt(_T("TARGET_APP"), _T("IS_STARTUP"), 0);
	m_resume_wait_time = pApp->GetProfileInt(_T("SYSTEM"), _T("RESUME_WAIT_TIME"), 5000);
	if (m_Target_path.GetLength()==0)
	{
		m_Target_path = _T("プログラムの場所を指定してください。");
	}

	UpdateData(FALSE);//iniファイルの内容をコントロール群に反映

	//ダイアログアイテムのサブクラス化
	m_DlgItem_Powerlamp.SubclassDlgItem(IDC_STATIC_PICTURE_CONTROL_POWER_INDIGATE, this);

	if (m_DlgItem_Powerlamp.IsLampOn() && m_radiko_support_on)
	{
		CreateTargetProcess();
	}



	switch (IDC_RADIO_MAIN_DLG_SHOW_STATUS+m_show_dlg_state)
	{
	case IDC_RADIO_MAIN_DLG_SHOW_STATUS:
		m_IsHide = FALSE;
		OnAppShow();
		if (m_use_task_tray != 0)
		{
			TrayNotifyIcon(NIM_ADD);
		}	
		break;

	case IDC_RADIO_SHOW_ICON:
		OnAppIcon();
		if (m_use_task_tray != 0)
		{
			TrayNotifyIcon(NIM_ADD);
		}
		break;

	case  IDC_RADIO_SHOW_TASKTRAY:
		m_IsHide = TRUE;
		m_check_use_tasktray.SetCheck(TRUE);
		TrayNotifyIcon(NIM_ADD);
		break;
	}

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CAPMSUSPENDDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CAPMSUSPENDDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}



}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CAPMSUSPENDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


UINT CAPMSUSPENDDlg::OnPowerBroadcast(UINT nPowerEvent, LPARAM nEventData)
{
	CSingleLock lock(&m_sem);
	lock.Lock();
	m_is_power_state = nPowerEvent;
	lock.Unlock();

	switch (nPowerEvent){
	case PBT_APMSUSPEND:
		TRACE(_T("\r\nSTART OnPowerBroadcast PBT_APMSUSPEND\r\n"));
		m_Radiko_had_startup = m_DlgItem_Powerlamp.IsLampOn();
		EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)&m_PI);
		break;

	case PBT_APMRESUMECRITICAL:
		TRACE(_T("\r\nSTART OnPowerBroadcast PBT_APMRESUMECRITICAL\r\n"));
		break;

	case  PBT_APMRESUMESUSPEND:
		TRACE(_T("\r\nSTART OnPowerBroadcast PBT_APMRESUMESUSPEND\r\n"));
		if (m_Radiko_had_startup&& m_check_radiko_suport_on.GetCheck()) {
			Sleep(m_resume_wait_time);
			CreateTargetProcess();
		}
		else
		{
			m_DlgItem_Powerlamp = 0;
		}
		break;

	}


	return CDialogEx::OnPowerBroadcast(nPowerEvent, nEventData);
}

void CAPMSUSPENDDlg::OnBnClickedButtonOpenPath()
{
//ファイルパスはスタティックコントロールに書かれる。変数には反映されない。
	CFileDialog* pDlg = new CFileDialog(TRUE, _T("*.exe"), _T(""), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("実行ファイル（*.exe)|*.exe|全て(*.*)|*.*||"), this);

	CString str;

	if (pDlg->DoModal() == IDOK)
	{
		str = pDlg->GetPathName(); // return full path and filename
	}
	delete pDlg;

	m_static_target_path.SetWindowText(str);
}


void CAPMSUSPENDDlg::OnBnClickedButtonCreateProcess()
{
	CSingleLock lock(&m_sem);
	lock.Lock();

	if(m_DlgItem_Powerlamp.IsLampOn())
	{
		lock.Unlock();
		return;
	}
	lock.Unlock();
	CreateTargetProcess();
}
void CAPMSUSPENDDlg::CreateTargetProcess()
{
	STARTUPINFO s;
	ZeroMemory(&s, sizeof(s));
	s.cb = sizeof(s);
	s.dwFlags = STARTF_USESHOWWINDOW;
	s.wShowWindow = SW_SHOWNORMAL;

	CString command;
	m_static_target_path.GetWindowTextW(command);
	if (command.GetLength() == 0)
	{
		AfxMessageBox(_T("ラジコのパスを指定してください。"), MB_OK);
	}

	int ret = CreateProcess(
		NULL, // 実行可能モジュールの名
		command.GetBuffer(), // コマンドラインの文字列
		NULL, // セキュリティ記述子
		NULL,// セキュリティ記述子
		FALSE, // ハンドルの継承オプション
		NULL, // 作成のフラグ
		NULL,// 新しい環境ブロック
		NULL, // カレントディレクトリの名前
		&s, // スタートアップ情報
		&m_PI // プロセス情報
	);
	command.ReleaseBuffer();

	if (ret)
	{

		//ラジコの終了を検知する為のスレッドを作成。
		m_pThread = AfxBeginThread(CAPMSUSPENDDlg::Call_Target_end_detect, (LPVOID)this);
		CSingleLock lock(&m_sem);
		lock.Lock();
		m_pThread->m_bAutoDelete = TRUE;
		lock.Unlock();
		
		m_DlgItem_Powerlamp = 1;
		m_button_create_process.EnableWindow(FALSE);
		
/*		CMenu menu,*p_submenu;
		menu.LoadMenu(IDR_MENU_MAIN);
		p_submenu = menu.GetSubMenu(0);
		p_submenu->EnableMenuItem(ID_RADIKO_START_UP, FALSE);
*/
	}
	else
	{
		return;
	}
	return ;
}


void CAPMSUSPENDDlg::OnBnClickedButtonCloseProcess()
{

	// プロセスを探し出しWM_CLOSEメッセージをポストするコールバック関数の呼び出し。
	EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)&m_PI);
	m_DlgItem_Powerlamp = 0;
}



BOOL CAPMSUSPENDDlg::DestroyWindow()
{
//起動状態を保存するかどうか
	if (!m_is_state_save)
	{
		m_DlgItem_Powerlamp = 0;
	}

	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileString(_T("TARGET_APP"), _T("FILE_PATH"), m_Target_path);
	pApp->WriteProfileInt(_T("SYSTEM"), _T("SAVE_STARTUP_STATUS"), m_is_state_save);
	pApp->WriteProfileInt(_T("SYSTEM"), _T("RADIKO_SUPPORT_ON"), m_radiko_support_on);
	pApp->WriteProfileInt(_T("TARGET_APP"), _T("IS_STARTUP"), m_DlgItem_Powerlamp.IsLampOn());
	pApp->WriteProfileInt(_T("SYSTEM"), _T("SHOW_DLG_STATE"), m_show_dlg_state );
	pApp->WriteProfileInt(_T("SYSTEM"), _T("USE_TASK_TRAY"), m_use_task_tray);
	pApp->WriteProfileInt(_T("SYSTEM"), _T("RESUME_WAIT_TIME"), m_resume_wait_time);

//ラジコプロセスを探し出し、閉じるメッセージを送る
	EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)&m_PI);


//スレッドが終了するまで待つ。こっちのスレッド方が圧倒的に早いのであらかじめm_sm_Callback_AliveをLockしておく。

	CSingleLock alock(&m_sm_Callback_Alive);
	alock.Lock();
	alock.Unlock();
	TRACE(_T("\r\nEND CAPMSUSPENDDlg::DestroyWindow()\r\n"));

	return CDialogEx::DestroyWindow();
}



void CAPMSUSPENDDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanging(lpwndpos);

	if (m_IsHide)
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
}




afx_msg LRESULT CAPMSUSPENDDlg::OnTasktray(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case WM_LBUTTONDOWN:
		if (wParam == IDR_MAINFRAME) {    // アイコンID
			if (m_IsHide)
			{
				OnAppShow();
			}
			else
			{
				m_IsHide = TRUE;
				m_check_use_tasktray.SetCheck(TRUE);
				ShowWindow(SW_HIDE);


			}
		}
		break;
	case WM_RBUTTONDOWN:
		if (wParam == IDR_MAINFRAME) {
			CMenu menu;
			menu.LoadMenu(IDR_MENU_MAIN);
			CMenu* popup = menu.GetSubMenu(0);
			UINT is_enable = 0;
			if (m_DlgItem_Powerlamp.IsLampOn())
			{
				is_enable = MF_BYCOMMAND | MF_DISABLED | MF_GRAYED;
			}
			else
			{
				is_enable = MF_BYCOMMAND | MF_ENABLED;
			}
			popup->EnableMenuItem(ID_RADIKO_START_UP, is_enable);
			SetForegroundWindow();
			POINT pt;
			GetCursorPos(&pt);
			popup->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
			menu.DestroyMenu();
		}
	default:
		break;
	}
	return 0;
}


bool CAPMSUSPENDDlg::TrayNotifyIcon(DWORD dwMessage)
{
	CString sTip = _T("ラジコランチャー");
	NOTIFYICONDATA nid;

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = GetSafeHwnd();
	nid.uID = IDR_MAINFRAME;        // <-- アイコンIDを指定してます
	nid.uCallbackMessage = WM_TASKTRAY;    // <-- WM_USER + 1 とかでメッセージ定義
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	_tcscpy_s(nid.szTip, _countof(nid.szTip), (LPCTSTR)sTip);

	return Shell_NotifyIcon(dwMessage, &nid);
}


void CAPMSUSPENDDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	TrayNotifyIcon(NIM_DELETE);
}


void CAPMSUSPENDDlg::OnAppShow()
{
	m_IsHide = false;
	ShowWindow(SW_NORMAL);
	SetForegroundWindow();
	SetFocus();
}


void CAPMSUSPENDDlg::OnAppIcon()
{
	m_IsHide = false;
	ShowWindow(SW_MINIMIZE);
//	SetForegroundWindow();
//	SetFocus();
}


void CAPMSUSPENDDlg::OnAppClose()
{
	OnBnClickedOk();
}


void CAPMSUSPENDDlg::OnRadikoSetPath()
{
	OnBnClickedButtonOpenPath();
}


void CAPMSUSPENDDlg::OnRadikoStartUp()
{
	OnBnClickedButtonCreateProcess();
}


void CAPMSUSPENDDlg::OnRadikoClose()
{
	OnBnClickedButtonCloseProcess();
}


void CAPMSUSPENDDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}


void CAPMSUSPENDDlg::OnClickedCheckUseTaskTray()
{
//	UpdateData(TRUE); OnCancelが選ばれた場合保存しないようにm_use_task_trayは使わない。
	bool is_checked;
	is_checked = m_check_use_tasktray.GetCheck();
	if (is_checked)
	{
		TrayNotifyIcon(NIM_ADD);
	}
	else
	{
		TrayNotifyIcon(NIM_DELETE);
	}
}


void CAPMSUSPENDDlg::OnBnClickedButtonDoTasktray()
{
	TrayNotifyIcon(NIM_ADD);
	m_check_use_tasktray.SetCheck(TRUE);
	m_IsHide = 1;
	ShowWindow(SW_HIDE);
}


void CAPMSUSPENDDlg::OnBnClickedCancel()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnCancel();
}


void CAPMSUSPENDDlg::OnAppCancel()
{
	OnBnClickedCancel();
}


void CAPMSUSPENDDlg::PowerLampON()
{

}


void CAPMSUSPENDDlg::OnClickedStaticPictureControlPowerIndigate()
{

}


void CAPMSUSPENDDlg::PowerLampOff()
{

}



void CAPMSUSPENDDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。

	CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


void CAPMSUSPENDDlg::Dump(CDumpContext& dc) const
{
//	CDialogEx::Dump(dc);
	
	dc << _T("maindlg");
}


void CAPMSUSPENDDlg::OnBnClickedButtonTestSuspend()
{
	PostMessage(WM_POWERBROADCAST, PBT_APMSUSPEND, NULL);
//	OnPowerBroadcast(PBT_APMSUSPEND, NULL);
}


void CAPMSUSPENDDlg::OnBnClickedButtonTestResume()
{
	PostMessage(WM_POWERBROADCAST, PBT_APMRESUMESUSPEND, NULL);
//	OnPowerBroadcast(PBT_APMRESUMESUSPEND, NULL);
}


void CAPMSUSPENDDlg::OnBnClickedCheckMaintain()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}



void CAPMSUSPENDDlg::OnClose()
{
	CDialogEx::OnOK();

//	CDialogEx::OnClose();
}


void CAPMSUSPENDDlg::OnBnClickedButtonAdvancedOption()
{
	CString str;
	CAdvacedSettingDlg* pDlg = new CAdvacedSettingDlg(this);
	pDlg->m_resume_wait_time = m_resume_wait_time;
	pDlg->DoModal();
	m_resume_wait_time = pDlg->m_resume_wait_time;
	delete pDlg;
	return;
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: ここに初期化を追加してください
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
