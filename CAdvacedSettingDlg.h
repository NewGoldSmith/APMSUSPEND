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
	afx_msg void OnClickedButtonSleepIncrease();
	afx_msg void OnClickedButtonSleepDecease();
	int m_resume_wait_time;
	virtual BOOL OnInitDialog();
protected:
	CFont* m_newFont;
public:
	afx_msg void OnDestroy();
	CEdit m_ctlEdit_resume_wait_time;
	afx_msg void OnBnClickedButtonCreateShortcutStartup();
	afx_msg void OnBnClickedButtonDeleteShortcutStartup();
	afx_msg void OnBnClickedButtonCreateShortcutDesktop();
	afx_msg void OnBnClickedButtonDeleteShortcutDesktop();
	afx_msg void OnBnClickedButtonCreateShortcutStartmenu();
	afx_msg void OnBnClickedButtonDeleteShortcutStartmenu();
protected:
	void DeleteLink(int csidl);
	void MoveLink(int csidlDest, int csidlSource);
	bool FindLink(int csidlPath);
	void GetLinkPathName(int csidlPath, LPTSTR pszPathName);

	void CreateShortCut(int csidl);
	CStaticBmpControl m_ctlImgctl_Exist_Link_Desktop;
	CStaticBmpControl m_ctlImgctl_Exist_Link_Menu;
	CStaticBmpControl m_ctlImgctl_Exist_Link_Startup;
private:
	void Update_Lamp();
protected:
	CButton m_ctlBtn_create_shortcut_desktop;
public:
	CButton m_ctlBtn_create_shortcut_menu;
	CButton m_ctlBtn_create_shortcut_startup;
	CButton m_ctlBtn_delete_shortcut_desktop;
	CButton m_ctlBtn_delete_shortcut_menu;
	CButton m_ctlBtn_delete_shortcut_startup;

};
