
// APMSUSPEND.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル
#include "CAdvacedSettingDlg.h"


// CAPMSUSPENDApp:
// このクラスの実装については、APMSUSPEND.cpp を参照してください
//

class CAPMSUSPENDApp : public CWinApp
{
public:
	CAPMSUSPENDApp();
	~CAPMSUSPENDApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
protected:
//	LPTSTR* m_pProfileName;
private:
	HANDLE m_hMutex;
public:
	virtual int ExitInstance();
};

extern CAPMSUSPENDApp theApp;
