#pragma once
#include "resource.h"
#include "CEditCustom.h"

// CAdvacedSetting ダイアログ

class CAdvacedSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAdvacedSettingDlg)

public:
	CAdvacedSettingDlg(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CAdvacedSettingDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADVANCED_SETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()

public:
//	CEdit m_ctl_Edit_custom;
	afx_msg void OnClickedButtonSleepIncrease();
	afx_msg void OnClickedButtonSleepDecease();
	int m_resume_wait_time;
	virtual BOOL OnInitDialog();
protected:
	CFont* m_newFont;
public:
	afx_msg void OnDestroy();
protected:
	CEditCustom m_ctl_Edit_custom;
};
