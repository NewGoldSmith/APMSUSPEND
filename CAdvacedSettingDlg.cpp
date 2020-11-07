// CAdvacedSetting.cpp : 実装ファイル
//

#include "pch.h"
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
	DDX_Control(pDX, IDC_EDIT_RESUME_SLEEP_TIME, m_ctlEdit_resume_sleep_time);
}


BEGIN_MESSAGE_MAP(CAdvacedSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SLEEP_INCREASE, &CAdvacedSettingDlg::OnClickedButtonSleepIncrease)
	ON_BN_CLICKED(IDC_BUTTON_SLEEP_DECEASE, &CAdvacedSettingDlg::OnClickedButtonSleepDecease)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_CREATE_SHORTCUT, &CAdvacedSettingDlg::OnBnClickedButtonCreateShortcut)
END_MESSAGE_MAP()


// CAdvacedSetting メッセージ ハンドラー


void CAdvacedSettingDlg::OnClickedButtonSleepIncrease()
{
	CString str;
	m_ctlEdit_resume_sleep_time.GetWindowText(str);
	int i = _ttoi(str);
	i += 1000;
	if (i > 9000)
	{
		i = 9000;
	}
	str.Format(_T("%d"), i);
	m_ctlEdit_resume_sleep_time.SetWindowText(str);
}


void CAdvacedSettingDlg::OnClickedButtonSleepDecease()
{
	CString str;
	m_ctlEdit_resume_sleep_time.GetWindowText(str);
	int i = _ttoi(str);
	i -= 1000;
	if (i < 0)
	{
		i = 0;
	}
	str.Format(_T("%d"),i);
	m_ctlEdit_resume_sleep_time.SetWindowText(str);
}


BOOL CAdvacedSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: ここに初期化を追加してください
	UpdateData(FALSE);


	CRect rect, rect2;
	CSize size;
	CFont* curFont;
	m_ctlEdit_resume_sleep_time.GetClientRect(rect);
	size =rect.Size();
	curFont = m_ctlEdit_resume_sleep_time.GetFont();
	LOGFONTW mylf;
	curFont->GetLogFont(&mylf);
	mylf.lfHeight = size.cy;
	mylf.lfWidth = size.cx/5;
	mylf.lfWeight = FW_HEAVY;
	m_newFont = new CFont;
	m_newFont->CreateFontIndirectW(&mylf);
	m_ctlEdit_resume_sleep_time.SetFont(m_newFont);

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


void CAdvacedSettingDlg::OnBnClickedButtonCreateShortcut()
{
	CreateShortcut(CSIDL_DESKTOPDIRECTORY);
}

void CAdvacedSettingDlg::CreateShortcut(int csidl)
{
	//	CWinApp* pApp = AfxGetApp();
	//	CString programPath,linkPath;
	TCHAR	szModulePathName[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szModulePathName, MAX_PATH);	// DLLでも使えるようにAfxGetInstanceHandle()を使っている。
	TCHAR	szLinkPathName[_MAX_PATH];
	TCHAR	drive[_MAX_DRIVE];
	TCHAR	dir[_MAX_DIR];
	TCHAR	fname[_MAX_FNAME];
	TCHAR	ext[_MAX_EXT];
	_wsplitpath_s(szModulePathName, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	wchar_t linkPath[MAX_PATH]; // ショートカット名はユニコード限定

	// デスクトップフォルダ取得（デスクトップかスタートメニューにしか作れない）
	LPITEMIDLIST pidl;
	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY, &pidl);
	SHGetPathFromIDListW(pidl, linkPath);
	wsprintf(linkPath, L"%s\\%s%s", linkPath, fname, L".lnk");

	IShellLink* sl;
	IPersistFile* pf;
	CoInitialize(NULL); // COMライブラリを初期化
	if (CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&sl) == S_OK)
	{
		if (sl->QueryInterface(IID_IPersistFile, (void**)&pf) == S_OK)
		{
			sl->SetPath(szModulePathName); // プログラムパス
			sl->SetDescription(NULL); // 説明（＝ ツールチップス）
			sl->SetArguments(NULL); // コマンドライン引数
			sl->SetWorkingDirectory(NULL); // 作業ディレクトリ
			sl->SetIconLocation(NULL, 0); // Iconのパスまたはインデックス
			sl->SetShowCmd(SW_SHOWNORMAL); // 起動時のウィンドウ表示
			pf->Save(linkPath, TRUE); // ショートカットを保存
			pf->Release(); // IPersistFileへのポインタを破棄
			pf = NULL;


		}
		sl->Release(); // IShellLinkへのポインタを破棄
		sl = NULL;
	}
	CoUninitialize(); // COMライブラリをクローズ
/*
	wchar_t lpNewFileName[MAX_PATH];
	SHGetSpecialFolderLocation(NULL, CSIDL_STARTUP, &pidl);
	SHGetPathFromIDListW(pidl, lpNewFileName);
	wsprintf(lpNewFileName, L"%s\\%s%s", lpNewFileName, fname, L".lnk");

	MoveFile(
		linkPath, // ファイル名
		lpNewFileName // 新しいファイル名
	);
	*/

	return;
}
