#pragma once
#include "resource.h"
#include "CStaticBmpControl.h"

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
	int m_resume_wait_time;
protected:
	afx_msg void OnBnClickedButtonCreateShortcutDesktop();
	afx_msg void OnBnClickedButtonCreateShortcutStartmenu();
	afx_msg void OnBnClickedButtonCreateShortcutStartup();
	afx_msg void OnBnClickedButtonDeleteShortcutDesktop();
	afx_msg void OnBnClickedButtonDeleteShortcutStartmenu();
	afx_msg void OnBnClickedButtonDeleteShortcutStartup();
	afx_msg void OnClickedButtonSleepDecease();
	afx_msg void OnClickedButtonSleepIncrease();
	afx_msg void OnDestroy();
	bool FindLink(int csidlPath);
	CButton m_ctlBtn_create_shortcut_desktop;
	CButton m_ctlBtn_create_shortcut_menu;
	CButton m_ctlBtn_create_shortcut_startup;
	CButton m_ctlBtn_delete_shortcut_desktop;
	CButton m_ctlBtn_delete_shortcut_menu;
	CButton m_ctlBtn_delete_shortcut_startup;
	CEdit m_ctlEdit_resume_wait_time;
	CFont* m_newFont;
	CStaticBmpControl m_ctlImgctl_Exist_Link_Desktop;
	CStaticBmpControl m_ctlImgctl_Exist_Link_Menu;
	CStaticBmpControl m_ctlImgctl_Exist_Link_Startup;
	virtual BOOL OnInitDialog();
	void CreateShortCut(int csidl);
	void DeleteLink(int csidl);
	void GetLinkPathName(int csidlPath, LPTSTR pszPathName);
	void Update_Lamp();

};
