
// APMSUSPEND.cpp : アプリケーションのクラス動作を定義します。
//

#include "pch.h"
#include "framework.h"
#include "APMSUSPEND.h"
#include "APMSUSPENDDlg.h"
#include <libloaderapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAPMSUSPENDApp

BEGIN_MESSAGE_MAP(CAPMSUSPENDApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAPMSUSPENDApp の構築

CAPMSUSPENDApp::CAPMSUSPENDApp()
{
	// TODO: この位置に構築用コードを追加してください。
	m_hMutex = NULL;
}
CAPMSUSPENDApp::~CAPMSUSPENDApp()
{
//	delete[] m_pProfileName;	//MFCの中では解放していないので、自分で解放する必要がある[2006-09-14]
//	CWinApp::~CWinApp();
}

// 唯一の CAPMSUSPENDApp オブジェクト

CAPMSUSPENDApp theApp;


// CAPMSUSPENDApp の初期化

BOOL CAPMSUSPENDApp::InitInstance()
{
	m_hMutex = CreateMutex(FALSE, 0, _T("CAPMSUSPENDApp"));

	// 複数のプロセスの起動を禁止する。起動していたらすぐに終了させる
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(m_hMutex);
		return FALSE;  // FALSEを返すと終了する。
	}
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// ダイアログにシェル ツリー ビューまたはシェル リスト ビュー コントロールが
	// 含まれている場合にシェル マネージャーを作成します。
	CShellManager *pShellManager = new CShellManager;

	// MFC コントロールでテーマを有効にするために、"Windows ネイティブ" のビジュアル マネージャーをアクティブ化
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。

//	SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	if (m_pszProfileName)
	{
		free((void*)m_pszProfileName);
	}

	TCHAR	szModulePathName[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szModulePathName, MAX_PATH);	// DLLでも使えるようにAfxGetInstanceHandle()を使っている。
	TCHAR	szIniPathName[_MAX_PATH];
	TCHAR	drive[_MAX_DRIVE];
	TCHAR	dir[_MAX_DIR];
	TCHAR	fname[_MAX_FNAME];
	TCHAR	ext[_MAX_EXT];
	_tsplitpath_s(szModulePathName, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	_tmakepath_s(szIniPathName, _MAX_PATH, drive, dir, fname, _T("ini"));
	m_pszProfileName = _tcsdup(szIniPathName);
	if (!m_pszProfileName)
	{
		AfxMessageBox(_T("メモリ不足エラー"));
		return FALSE;
	}



	CAPMSUSPENDDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <キャンセル> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: ダイアログの作成に失敗しました。アプリケーションは予期せずに終了します。\n");
		TRACE(traceAppMsg, 0, "警告: ダイアログで MFC コントロールを使用している場合、#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS を指定できません。\n");
	}

	// 上で作成されたシェル マネージャーを削除します。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}



int CAPMSUSPENDApp::ExitInstance()
{
	// Mutexの所有権を解放する。
	ReleaseMutex(m_hMutex);
	CloseHandle(m_hMutex);
	return CWinApp::ExitInstance();
}
