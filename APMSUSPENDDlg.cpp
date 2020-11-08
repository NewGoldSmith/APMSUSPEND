
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

	pDlg->m_cSL_alock.Lock();//このスレッドが生存中はLockしておく

	WaitForSingleObject(pDlg->m_PI.hProcess, INFINITE);

	pDlg->m_cSL_lock.Lock();
	pDlg->m_ctlImgCtl_Powerlamp = 0;

	CloseHandle(pDlg->m_PI.hThread);
	CloseHandle(pDlg->m_PI.hProcess);
	pDlg->m_PI.hThread = NULL;
	pDlg->m_PI.hProcess = NULL;

	pDlg->m_cSL_lock.Unlock();

	TRACE(_T("\r\nEND Call_Target_end_detect\r\n"));

	pDlg->m_cSL_alock.Unlock();
	pDlg->m_ctlBtn_create_process.EnableWindow(TRUE);
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
	: CDialogEx(IDD_APMSUSPEND_DIALOG, pParent), m_cSem()
, m_b_safe_suspend_support_on(FALSE)
, m_b_restore_prev_target_startup_state(FALSE)
, m_i_show_maindlg_state(0)
, m_b_use_task_tray(FALSE)
,m_cSem_Callback_Alive()
,m_cSL_alock(&m_cSem_Callback_Alive)
,m_cSL_lock(&m_cSem)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_PI.hProcess = NULL;
	m_PI.hThread = NULL;
	mp_cThread_Detect_End = NULL;
	m_ctlImgCtl_Powerlamp = 0;
	m_b_IsHide = FALSE;

	m_is_power_state = 0;
	m_b_Target_had_startup = 0;
	m_i_resume_wait_time = 0;
}
CAPMSUSPENDDlg::~CAPMSUSPENDDlg() {

}

void CAPMSUSPENDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_TARGET_PATH, m_str_Target_path);
	DDX_Check(pDX, IDC_CHECK_TARGET_SUPPORT_ON, m_b_safe_suspend_support_on);
	DDX_Check(pDX, IDC_CHECK_RESTORE_PREVIOUS_STARTUP, m_b_restore_prev_target_startup_state);
	DDX_Radio(pDX, IDC_RADIO_MAIN_DLG_SHOW_STATUS, m_i_show_maindlg_state);
	DDX_Check(pDX, IDC_CHECK_USE_TASK_TRAY, m_b_use_task_tray);
	DDX_Control(pDX, IDC_CHECK_USE_TASK_TRAY, m_ctlChk_use_tasktray);
	DDX_Control(pDX, IDC_CHECK_TARGET_SUPPORT_ON, m_ctlChk_safe_suspend_suport_on);
	DDX_Control(pDX, IDC_STATIC_TARGET_PATH, m_ctlStatic_Target_path);
	DDX_Control(pDX, IDC_BUTTON_CREATE_PROCESS, m_ctlBtn_create_process);
	DDX_Control(pDX, IDC_BUTTON_TEST_RESUME, m_ctlBtn_test_resume);
	DDX_Control(pDX, IDC_BUTTON_TEST_SUSPEND, m_ctlBtn_test_suspend);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_ctlBtn_save);
	DDX_Control(pDX, IDCANCEL, m_ctlBtn_cancel);
	DDX_Control(pDX, IDC_RADIO_SHOW_WINDOW_MAIN_STARTUP, m_ctlRdo_MainDlg_Show);
	DDX_Control(pDX, IDC_CHECK_RESTORE_PREVIOUS_STARTUP, m_ctlChk_restre_prev_startup_state);
}

BEGIN_MESSAGE_MAP(CAPMSUSPENDDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_POWERBROADCAST()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_PATH, &CAPMSUSPENDDlg::OnBnClickedButtonGetPath)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_PROCESS, &CAPMSUSPENDDlg::OnBnClickedButtonCreateProcess)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_PROCESS, &CAPMSUSPENDDlg::OnBnClickedButtonCloseProcess)
	ON_WM_WINDOWPOSCHANGING()
	ON_MESSAGE(WM_TASKTRAY, &CAPMSUSPENDDlg::OnTasktray)
	ON_WM_DESTROY()
	ON_COMMAND(ID_APP_SHOW, &CAPMSUSPENDDlg::OnAppShow)
	ON_COMMAND(ID_APP_ICON, &CAPMSUSPENDDlg::OnAppIcon)
	ON_COMMAND(ID_SET_TARGET_PATH, &CAPMSUSPENDDlg::OnTargetSetPath)
	ON_COMMAND(ID_TARGET_START_UP, &CAPMSUSPENDDlg::OnTargetStartUp)
	ON_COMMAND(ID_TARGET_CLOSE, &CAPMSUSPENDDlg::OnTargetClose)
	ON_BN_CLICKED(IDOK, &CAPMSUSPENDDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_USE_TASK_TRAY, &CAPMSUSPENDDlg::OnClickedCheckUseTaskTray)
	ON_BN_CLICKED(IDC_BUTTON_DO_TASKTRAY, &CAPMSUSPENDDlg::OnBnClickedButtonDoTasktray)
	ON_BN_CLICKED(IDCANCEL, &CAPMSUSPENDDlg::OnBnClickedCancel)
	ON_COMMAND(ID_APP_CANCEL, &CAPMSUSPENDDlg::OnAppCancel)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BUTTON_TEST_SUSPEND, &CAPMSUSPENDDlg::OnBnClickedButtonTestSuspend)
	ON_BN_CLICKED(IDC_BUTTON_TEST_RESUME, &CAPMSUSPENDDlg::OnBnClickedButtonTestResume)
	ON_BN_CLICKED(IDC_CHECK_RESTORE_PREVIOUS_STARTUP, &CAPMSUSPENDDlg::OnBnClickedCheckRestorePreviousStartup)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_ADVANCED_OPTION, &CAPMSUSPENDDlg::OnBnClickedButtonOpenAdvancedSettingDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CAPMSUSPENDDlg::OnBnClickedButtonSave)
	ON_COMMAND(ID_MENU_SAVE, &CAPMSUSPENDDlg::OnMenuSave)
	ON_BN_CLICKED(IDC_RADIO_SHOW_ICON_MAIN_STARTUP, &CAPMSUSPENDDlg::OnBnClickedRadioShowIcon)
	ON_BN_CLICKED(IDC_RADIO_SHOW_WINDOW_MAIN_STARTUP, &CAPMSUSPENDDlg::OnBnClickedRadioShowWindow)
	ON_BN_CLICKED(IDC_RADIO_SHOW_TASKTRAY_MAIN_STARTUP, &CAPMSUSPENDDlg::OnBnClickedRadioShowTasktray)
	ON_BN_CLICKED(IDC_CHECK_TARGET_SUPPORT_ON, &CAPMSUSPENDDlg::OnClickedCheckTargetSupportOn)
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
	m_ctlBtn_test_suspend.ShowWindow(SW_HIDE);
	m_ctlBtn_test_resume.ShowWindow(SW_HIDE);
#endif
 

	CWinApp* pApp = AfxGetApp();
	m_str_Target_path = pApp->GetProfileString(_T("SYSTEM"), _T("FILE_PATH"), _T("C:\\Program Files (x86)\\radiko_player_air\\radiko_player_air.exe"));
	m_b_restore_prev_target_startup_state = pApp->GetProfileInt(_T("SYSTEM"), _T("SAVE_STARTUP_STATUS"), TRUE);
	m_b_safe_suspend_support_on = pApp->GetProfileInt(_T("SYSTEM"), _T("SAFE_SUSPEND_SUPPORT_ON"), TRUE);
	m_i_show_maindlg_state = pApp->GetProfileInt(_T("SYSTEM"), _T("SHOW_DLG_STATE"), 0);
	m_b_use_task_tray = pApp->GetProfileInt(_T("SYSTEM"), _T("USE_TASK_TRAY"), TRUE);
	m_ctlImgCtl_Powerlamp = pApp->GetProfileInt(_T("TARGET_APP"), _T("IS_STARTUP"), 0);
	m_i_resume_wait_time = pApp->GetProfileInt(_T("SYSTEM"), _T("RESUME_WAIT_TIME"), 2000);
	if (m_str_Target_path.GetLength()==0)
	{
//		m_str_Target_path = _T("C:\\Program Files (x86)\\radiko_player_air\\radiko_player_air.exe");
	}

	UpdateData(FALSE);//iniファイルの内容をコントロール群に反映

	//ダイアログアイテムのサブクラス化
	m_ctlImgCtl_Powerlamp.SubclassDlgItem(IDC_STATIC_PICTURE_CONTROL_POWER_INDIGATE, this);

	if (m_ctlImgCtl_Powerlamp.IsLampOn() && m_b_safe_suspend_support_on)
	{
		CreateTargetProcess();
	}



	switch (IDC_RADIO_MAIN_DLG_SHOW_STATUS+m_i_show_maindlg_state)
	{
	case IDC_RADIO_MAIN_DLG_SHOW_STATUS:
		m_b_IsHide = FALSE;
		OnAppShow();
		if (m_b_use_task_tray != 0)
		{
			TrayNotifyIcon(NIM_ADD);
		}	
		break;

	case IDC_RADIO_SHOW_ICON_MAIN_STARTUP:
		OnAppIcon();
		if (m_b_use_task_tray != 0)
		{
			TrayNotifyIcon(NIM_ADD);
		}
		break;

	case  IDC_RADIO_SHOW_TASKTRAY_MAIN_STARTUP:
		m_b_IsHide = TRUE;
		m_ctlChk_use_tasktray.SetCheck(TRUE);
		TrayNotifyIcon(NIM_ADD);
		break;
	}

	m_ctlBtn_save.EnableWindow(FALSE);

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
	CSingleLock lock(&m_cSem);
	lock.Lock();
	m_is_power_state = nPowerEvent;
	lock.Unlock();

	switch (nPowerEvent){
	case PBT_APMSUSPEND:
		TRACE(_T("\r\nSTART OnPowerBroadcast PBT_APMSUSPEND\r\n"));
		if (m_ctlChk_safe_suspend_suport_on.GetCheck())
		{
			m_b_Target_had_startup = m_ctlImgCtl_Powerlamp.IsLampOn();
			EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)&m_PI);
		}
		break;

	case PBT_APMRESUMECRITICAL:
		TRACE(_T("\r\nSTART OnPowerBroadcast PBT_APMRESUMECRITICAL\r\n"));
		break;

	case  PBT_APMRESUMESUSPEND:
		TRACE(_T("\r\nSTART OnPowerBroadcast PBT_APMRESUMESUSPEND\r\n"));
		if (m_b_Target_had_startup&& m_ctlChk_safe_suspend_suport_on.GetCheck()) 
		{
			Sleep(m_i_resume_wait_time);
			CreateTargetProcess();
		}
		else
		{
//			m_ctlImgCtl_Powerlamp = 0;
		}
		break;

	}


	return CDialogEx::OnPowerBroadcast(nPowerEvent, nEventData);
}

void CAPMSUSPENDDlg::OnBnClickedButtonGetPath()
{
//ファイルパスはスタティックコントロールに書かれる。変数には反映されない。
	CFileDialog* pDlg = new CFileDialog(TRUE, _T("*.exe"), _T(""), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("実行ファイル（*.exe)|*.exe|全て(*.*)|*.*||"), this);

	CString str,str_old;

	if (pDlg->DoModal() == IDOK)
	{
		str = pDlg->GetPathName(); // return full path and filename
	}
	delete pDlg;
	
	if (m_str_Target_path != str)
	{
		m_ctlBtn_save.EnableWindow(TRUE);
	}
	else
	{
		m_ctlBtn_save.EnableWindow(FALSE);
	}
	m_ctlStatic_Target_path.SetWindowText(str);
}


void CAPMSUSPENDDlg::OnBnClickedButtonCreateProcess()
{
	CSingleLock lock(&m_cSem);
	lock.Lock();

	if(m_ctlImgCtl_Powerlamp.IsLampOn())
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
	m_ctlStatic_Target_path.GetWindowText(command);
	CPathT<CString> path(command.GetBuffer());
	if (path.FileExists() == FALSE)
	{
		AfxMessageBox(_T("ターゲットのパスが正しくありません。設定し直してください。"), MB_OK);
		m_ctlImgCtl_Powerlamp = 0;
		return;
	}
	command.ReleaseBuffer();

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

		//ターゲットの終了を検知する為のスレッドを作成。
		mp_cThread_Detect_End = AfxBeginThread(CAPMSUSPENDDlg::Call_Target_end_detect, (LPVOID)this);
		CSingleLock lock(&m_cSem);
		lock.Lock();
		mp_cThread_Detect_End->m_bAutoDelete = TRUE;
		
		m_ctlImgCtl_Powerlamp = 1;
		lock.Unlock();
		m_ctlBtn_create_process.EnableWindow(FALSE);
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
	m_ctlImgCtl_Powerlamp = 0;
}



BOOL CAPMSUSPENDDlg::DestroyWindow()
{
//ターゲット起動状態を保存するかどうか、ランプで保存する。
	if (!m_b_restore_prev_target_startup_state)
	{
		m_ctlImgCtl_Powerlamp = 0;
	}

	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileString(_T("SYSTEM"), _T("FILE_PATH"), m_str_Target_path);
	pApp->WriteProfileInt(_T("SYSTEM"), _T("SAVE_STARTUP_STATUS"), m_b_restore_prev_target_startup_state);
	pApp->WriteProfileInt(_T("SYSTEM"), _T("SAFE_SUSPEND_SUPPORT_ON"), m_b_safe_suspend_support_on);
	pApp->WriteProfileInt(_T("TARGET_APP"), _T("IS_STARTUP"), m_ctlImgCtl_Powerlamp.IsLampOn());
	pApp->WriteProfileInt(_T("SYSTEM"), _T("SHOW_DLG_STATE"), m_i_show_maindlg_state );
	pApp->WriteProfileInt(_T("SYSTEM"), _T("USE_TASK_TRAY"), m_b_use_task_tray);
	pApp->WriteProfileInt(_T("SYSTEM"), _T("RESUME_WAIT_TIME"), m_i_resume_wait_time);

//ターゲットプロセスを探し出し、閉じるメッセージを送る
	EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)&m_PI);

	CSingleLock alock(&m_cSem_Callback_Alive);

//スレッドが終了するまで待つ。通常こちらのスレッドの方が圧倒的に早いので
//	あらかじめコールバック関数でm_cSem_Callback_AliveをLockしておく。
	alock.Lock();
	alock.Unlock();
	TRACE(_T("\r\nEND CAPMSUSPENDDlg::DestroyWindow()\r\n"));

	return CDialogEx::DestroyWindow();
}


//ここから下のほとんどがUIの振る舞いの記述である。
void CAPMSUSPENDDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanging(lpwndpos);

	if (m_b_IsHide)
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
}




afx_msg LRESULT CAPMSUSPENDDlg::OnTasktray(WPARAM wParam, LPARAM lParam)
{
//タスクトレイの振る舞いを記述。
	switch (lParam)
	{
	case WM_LBUTTONDOWN:
		if (wParam == IDR_MAINFRAME) {    // アイコンID
			if (m_b_IsHide)
			{
				OnAppShow();
			}
			else if(IsIconic())
			{
				OnAppShow();
			}
			else
			{
				m_b_IsHide = TRUE;
				m_ctlChk_use_tasktray.SetCheck(TRUE);
				ShowWindow(SW_HIDE);


			}
		}
		break;
	case WM_RBUTTONDOWN:
		if (wParam == IDR_MAINFRAME) {
			CMenu menu;
			menu.LoadMenu(IDR_MENU_MAIN);
			CMenu* popup = menu.GetSubMenu(0);
			UINT is_enable = MF_BYCOMMAND | MF_ENABLED;
			UINT is_disable= MF_BYCOMMAND | MF_DISABLED | MF_GRAYED;
			
			//”このアプリを表示する”項目を有効にするかどうか判別。以前に表示になっていた場合０以外。
			//以前に非表示になっていた場合は０。
			if (ShowWindow(SW_SHOW))
			{
				if (IsIconic())
				{
					popup->EnableMenuItem(ID_APP_SHOW, is_enable);
				}
				else
				{
					popup->EnableMenuItem(ID_APP_SHOW, is_disable);
				}
			}
			else
			{
				popup->EnableMenuItem(ID_APP_SHOW, is_enable);
			}

			//”アイコンで表示する”項目の有効・無効判別。以前にアイコン表示になっていたかどうか判別。
			if (IsIconic())
			{
				popup->EnableMenuItem(ID_APP_ICON, is_disable);
			}
			else
			{
				popup->EnableMenuItem(ID_APP_ICON, is_enable);
			}

			//”起動”を有効にするかどうか判別
			if (m_ctlImgCtl_Powerlamp.IsLampOn())
			{
				popup->EnableMenuItem(ID_TARGET_START_UP, is_disable);
			}
			else
			{
				popup->EnableMenuItem(ID_TARGET_START_UP, is_enable);
			}
			
			//”セーブ”項目を有効にするかどうか判別
			if (!m_ctlBtn_save.EnableWindow())//直前有効なら０。無効なら０以外。
			{
				popup->EnableMenuItem(ID_MENU_SAVE, is_enable);
				m_ctlBtn_save.EnableWindow(TRUE);
			}
			else
			{
				popup->EnableMenuItem(ID_MENU_SAVE, is_disable);
				m_ctlBtn_save.EnableWindow(FALSE);
			}

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


//タスクトレイにアイコンを表示し、クリックしたときに送られてくるメッセージを指定する。
//WM_TASKTRAYはユーザー定義のメッセージIDである。OnTasktrayにマッピングしてある。
bool CAPMSUSPENDDlg::TrayNotifyIcon(DWORD dwMessage)
{
	CString sTip = _T("ターゲットランチャー");
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
	m_b_IsHide = false;
	ShowWindow(SW_NORMAL);
	SetForegroundWindow();
	SetFocus();
}


void CAPMSUSPENDDlg::OnAppIcon()
{
	m_b_IsHide = false;
	ShowWindow(SW_MINIMIZE);
}


void CAPMSUSPENDDlg::OnTargetSetPath()
{
	OnBnClickedButtonGetPath();
}


void CAPMSUSPENDDlg::OnTargetStartUp()
{
	OnBnClickedButtonCreateProcess();
}


void CAPMSUSPENDDlg::OnTargetClose()
{
	OnBnClickedButtonCloseProcess();
}


void CAPMSUSPENDDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}


void CAPMSUSPENDDlg::OnClickedCheckUseTaskTray()
{
	if (m_ctlChk_use_tasktray.GetCheck())
	{
		TrayNotifyIcon(NIM_ADD);
	}
	else
	{
		TrayNotifyIcon(NIM_DELETE);
	}
	if (m_b_use_task_tray != m_ctlChk_use_tasktray.GetCheck())
	{
		m_ctlBtn_save.EnableWindow(TRUE);
	}
	else
	{
		m_ctlBtn_save.EnableWindow(FALSE);
	}
}


void CAPMSUSPENDDlg::OnBnClickedButtonDoTasktray()
{
//タスクトレイを使用するときは、”タスクトレイを使用する”のチェックが入った状態にする。
//疑似的にチェックボタンが押されてクリックメッセージが届いた様にする。
	if (!m_ctlChk_use_tasktray.GetCheck())
	{
		m_ctlChk_use_tasktray.SetCheck(TRUE);
		OnClickedCheckUseTaskTray();
	}
	
	m_b_IsHide = 1;
	ShowWindow(SW_HIDE);
}


void CAPMSUSPENDDlg::OnBnClickedCancel()
{
	// ”終了する”のボタンが押されるとここが呼ばれる
	CDialogEx::OnCancel();
}


void CAPMSUSPENDDlg::OnAppCancel()
{
	OnBnClickedCancel();
}

void CAPMSUSPENDDlg::Dump(CDumpContext& dc) const
{
//	CDialogEx::Dump(dc);
	
	dc << _T("maindlg");
}


void CAPMSUSPENDDlg::OnBnClickedButtonTestSuspend()
{
	PostMessage(WM_POWERBROADCAST, PBT_APMSUSPEND, NULL);
}


void CAPMSUSPENDDlg::OnBnClickedButtonTestResume()
{
	PostMessage(WM_POWERBROADCAST, PBT_APMRESUMESUSPEND, NULL);
}


void CAPMSUSPENDDlg::OnBnClickedCheckRestorePreviousStartup()
{
	if (m_ctlChk_restre_prev_startup_state.GetCheck() != m_b_restore_prev_target_startup_state)
	{
		m_ctlBtn_save.EnableWindow(TRUE);
	}
	else
	{
		m_ctlBtn_save.EnableWindow(FALSE);
	}
}



void CAPMSUSPENDDlg::OnClose()
{
	CDialogEx::OnClose();
}


void CAPMSUSPENDDlg::OnBnClickedButtonOpenAdvancedSettingDlg()
{
	CAdvacedSettingDlg* pDlg = new CAdvacedSettingDlg(this);
	pDlg->m_resume_wait_time = m_i_resume_wait_time;
	BOOL b= pDlg->DoModal();
	if (b == 1)
	{
		m_i_resume_wait_time = pDlg->m_resume_wait_time;
	}
	delete pDlg;
	return;
}

void CAPMSUSPENDDlg::OnBnClickedButtonSave()
{
	m_ctlBtn_save.EnableWindow(FALSE);
	UpdateData(TRUE);
}


void CAPMSUSPENDDlg::OnMenuSave()
{
	OnBnClickedButtonSave();
}


BOOL CAPMSUSPENDDlg::PreTranslateMessage(MSG* pMsg)
{

	if (WM_KEYDOWN == pMsg->message)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			PostMessage(WM_COMMAND, IDOK);
			return FALSE;
		case VK_ESCAPE:
			PostMessage(WM_COMMAND, IDCANCEL);
			return FALSE;
		default:
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CAPMSUSPENDDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if (wParam == IDOK)
	{
		OnOK();
		return TRUE;
	}
	return CDialogEx::OnCommand(wParam, lParam);
}

void CAPMSUSPENDDlg::OnBnClickedRadioShowWindow()
{
	if(m_i_show_maindlg_state != 0)
	{
		m_ctlBtn_save.EnableWindow(TRUE);
	}
	else
	{
		m_ctlBtn_save.EnableWindow(FALSE);
	}
}

void CAPMSUSPENDDlg::OnBnClickedRadioShowIcon()
{
	if (m_i_show_maindlg_state != 1)
	{
		m_ctlBtn_save.EnableWindow(TRUE);
	}
	else
	{
		m_ctlBtn_save.EnableWindow(FALSE);
	}
}

void CAPMSUSPENDDlg::OnBnClickedRadioShowTasktray()
{
	if (m_i_show_maindlg_state != 2)
	{
		m_ctlBtn_save.EnableWindow(TRUE);
	}
	else
	{
		m_ctlBtn_save.EnableWindow(FALSE);
	}
}


void CAPMSUSPENDDlg::OnClickedCheckTargetSupportOn()
{
	if (m_b_safe_suspend_support_on != m_ctlChk_safe_suspend_suport_on.GetCheck())
	{
		m_ctlBtn_save.EnableWindow(TRUE);
	}
	else
	{
		m_ctlBtn_save.EnableWindow(FALSE);
	}
}

