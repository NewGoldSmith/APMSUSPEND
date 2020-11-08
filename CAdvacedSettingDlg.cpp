// CAdvacedSetting.cpp : 実装ファイル
//

#include "pch.h"
#include <stdio.h>
#include "APMSUSPEND.h"
#include "CAdvacedSettingDlg.h"
#include "afxdialogex.h"
#include "Resource.h"


// CAdvacedSetting ダイアログ

IMPLEMENT_DYNAMIC(CAdvacedSettingDlg, CDialogEx)

CAdvacedSettingDlg::CAdvacedSettingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ADVANCED_SETTING, pParent)
	, m_resume_wait_time(0)
	, m_newFont(NULL)
{

}

CAdvacedSettingDlg::~CAdvacedSettingDlg()
{
}

void CAdvacedSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RESUME_SLEEP_TIME, m_resume_wait_time);
	DDX_Control(pDX, IDC_EDIT_RESUME_SLEEP_TIME, m_ctlEdit_resume_wait_time);
	DDX_Control(pDX, IDC_STATIC_EXIST_LINK_DESKTOP, m_ctlImgctl_Exist_Link_Desktop);
	DDX_Control(pDX, IDC_STATIC_EXIST_LINK_MENU, m_ctlImgctl_Exist_Link_Menu);
	DDX_Control(pDX, IDC_STATIC_EXIST_LINK_STARTUP, m_ctlImgctl_Exist_Link_Startup);
}


BEGIN_MESSAGE_MAP(CAdvacedSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SLEEP_INCREASE, &CAdvacedSettingDlg::OnClickedButtonSleepIncrease)
	ON_BN_CLICKED(IDC_BUTTON_SLEEP_DECEASE, &CAdvacedSettingDlg::OnClickedButtonSleepDecease)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_CREATE_SHORTCUT_STARTUP, &CAdvacedSettingDlg::OnBnClickedButtonCreateShortcutStartup)
	ON_BN_CLICKED(IDC_BUTTON_DELITE_SHORTCUT_STARTUP, &CAdvacedSettingDlg::OnBnClickedButtonDeleteShortcutStartup)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_SHORTCUT_DESKTOP, &CAdvacedSettingDlg::OnBnClickedButtonCreateShortcutDesktop)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_SHORTCUT_DESKTOP, &CAdvacedSettingDlg::OnBnClickedButtonDeleteShortcutDesktop)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_SHORTCUT_STARTMENU, &CAdvacedSettingDlg::OnBnClickedButtonCreateShortcutStartmenu)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_SHORTCUT_STARTMENU, &CAdvacedSettingDlg::OnBnClickedButtonDeleteShortcutStartmenu)
END_MESSAGE_MAP()


// CAdvacedSetting メッセージ ハンドラー


void CAdvacedSettingDlg::OnClickedButtonSleepIncrease()
{
	CString str;
	m_ctlEdit_resume_wait_time.GetWindowText(str);
	int i = _ttoi(str);
	i += 1000;
	if (i > 9000)
	{
		i = 9000;
	}
	str.Format(_T("%d"), i);
	m_ctlEdit_resume_wait_time.SetWindowText(str);
}


void CAdvacedSettingDlg::OnClickedButtonSleepDecease()
{
	CString str;
	m_ctlEdit_resume_wait_time.GetWindowText(str);
	int i = _ttoi(str);
	i -= 1000;
	if (i < 0)
	{
		i = 0;
	}
	str.Format(_T("%d"),i);
	m_ctlEdit_resume_wait_time.SetWindowText(str);
}


BOOL CAdvacedSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: ここに初期化を追加してください
	UpdateData(FALSE);

	//エディットコントロールのフォントのセッティング。クライアント領域全体を使って表示する。
	CRect rect;
	CSize size;
	CFont* curFont;
	m_ctlEdit_resume_wait_time.GetClientRect(rect);
	size =rect.Size();
	curFont = m_ctlEdit_resume_wait_time.GetFont();
	LOGFONT mylf;
	curFont->GetLogFont(&mylf);
	mylf.lfHeight = size.cy;
	mylf.lfWidth = size.cx/5;
	mylf.lfWeight = FW_HEAVY;
	m_newFont = new CFont;
	m_newFont->CreateFontIndirect(&mylf);
	m_ctlEdit_resume_wait_time.SetFont(m_newFont);

	Update_Lamp();

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CAdvacedSettingDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	m_newFont->DeleteObject();
	delete m_newFont;
}


void CAdvacedSettingDlg::CreateShortCut(int csidl)
{
	TCHAR	szModulePathName[MAX_PATH];
	TCHAR	szLinkPathName[_MAX_PATH];

	::GetModuleFileName(AfxGetInstanceHandle(), szModulePathName, MAX_PATH);	// DLLでも使えるようにAfxGetInstanceHandle()を使っている。

	GetLinkPathName(csidl, szLinkPathName);
	IShellLink* sl;
	IPersistFile* pf;
	CoInitialize(NULL); // COMライブラリを初期化
	if (CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&sl) == S_OK)
	{
		if (sl->QueryInterface(IID_IPersistFile, (void**)&pf) == S_OK)
		{
			_bstr_t bszLinkPathName(szLinkPathName);//文字列変換

			sl->SetPath(szModulePathName); // プログラムパス
			sl->SetDescription(NULL); // 説明（＝ ツールチップス）
			sl->SetArguments(NULL); // コマンドライン引数
			sl->SetWorkingDirectory(NULL); // 作業ディレクトリ
			sl->SetIconLocation(NULL, 0); // Iconのパスまたはインデックス
			sl->SetShowCmd(SW_SHOWNORMAL); // 起動時のウィンドウ表示
			pf->Save(bszLinkPathName, TRUE); // ショートカットを保存
			pf->Release(); // IPersistFileへのポインタを破棄
			pf = NULL;
		}
		sl->Release(); // IShellLinkへのポインタを破棄
		sl = NULL;
	}
	CoUninitialize(); // COMライブラリをクローズ
}


void CAdvacedSettingDlg::GetLinkPathName(int csidlPath, LPTSTR pszPathName)
{
	TCHAR	szModulePathName[MAX_PATH];
	TCHAR	szLinkPath[MAX_PATH]; // ショートカット名はユニコード限定
	TCHAR	drive[_MAX_DRIVE];
	TCHAR	dir[_MAX_DIR];
	TCHAR	fname[_MAX_FNAME];
	TCHAR	ext[_MAX_EXT];

	::GetModuleFileName(AfxGetInstanceHandle(), szModulePathName, MAX_PATH);	// DLLでも使えるようにAfxGetInstanceHandle()を使っている。
	_tsplitpath_s(szModulePathName, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	LPITEMIDLIST pidl;
	SHGetSpecialFolderLocation(NULL, csidlPath, &pidl);
	SHGetPathFromIDList(pidl, szLinkPath);
	_stprintf_s(pszPathName, MAX_PATH, _T("%s\\%s%s"), szLinkPath, fname, _T(".lnk"));
}


void CAdvacedSettingDlg::MoveLink(int csidlSource, int csidlDest)
{
	TCHAR	szSourceLinkPathName[_MAX_PATH];
	TCHAR	szDestLinkPathName[_MAX_PATH];
	//ソース側リンクパスネーム取得
	GetLinkPathName(csidlSource, szSourceLinkPathName);
	//送り先側リンクパスネーム取得
	GetLinkPathName(csidlDest, szDestLinkPathName);
	MoveFile(szSourceLinkPathName, szDestLinkPathName);
}


void CAdvacedSettingDlg::DeleteLink(int csidl)
{
	TCHAR	szLinkPathName[_MAX_PATH];
	GetLinkPathName(csidl, szLinkPathName);
	CFile file;
	file.Remove(szLinkPathName);
}


bool CAdvacedSettingDlg::FindLink(int csidlPath)
{
	CFileFind find;
	CString filePath;

	GetLinkPathName(csidlPath, filePath.GetBufferSetLength(MAX_PATH));
	bool b = find.FindFile(filePath);
	filePath.ReleaseBuffer();
	return b;
}


void CAdvacedSettingDlg::OnBnClickedButtonCreateShortcutStartup()
{
	// デスクトップにショートカット作成（デスクトップかスタートメニューにしか作れない）
	CreateShortCut(CSIDL_DESKTOPDIRECTORY);
	MoveLink(CSIDL_DESKTOPDIRECTORY, CSIDL_STARTUP);
	Update_Lamp();
}


void CAdvacedSettingDlg::OnBnClickedButtonDeleteShortcutStartup()
{
	DeleteLink(CSIDL_STARTUP);
	Update_Lamp();
}


void CAdvacedSettingDlg::OnBnClickedButtonCreateShortcutDesktop()
{
	CreateShortCut(CSIDL_DESKTOPDIRECTORY);
	Update_Lamp();
}


void CAdvacedSettingDlg::OnBnClickedButtonDeleteShortcutDesktop()
{
	DeleteLink(CSIDL_DESKTOPDIRECTORY);
	Update_Lamp();
}

void CAdvacedSettingDlg::OnBnClickedButtonCreateShortcutStartmenu()
{
	CreateShortCut(CSIDL_STARTMENU);
	Update_Lamp();
}


void CAdvacedSettingDlg::OnBnClickedButtonDeleteShortcutStartmenu()
{
	DeleteLink(CSIDL_STARTMENU);
	Update_Lamp();
}

void CAdvacedSettingDlg::Update_Lamp()
{
	if (FindLink(CSIDL_DESKTOPDIRECTORY)) {
		m_ctlImgctl_Exist_Link_Desktop = 1;
	}
	else {
		m_ctlImgctl_Exist_Link_Desktop = 0;
	}

	if (FindLink(CSIDL_STARTUP)) {
		m_ctlImgctl_Exist_Link_Startup = 1;
	}
	else {
		m_ctlImgctl_Exist_Link_Startup = 0;
	}

	if (FindLink(CSIDL_STARTMENU)) {
		m_ctlImgctl_Exist_Link_Menu = 1;
	}
	else {
		m_ctlImgctl_Exist_Link_Menu = 0;
	}
}