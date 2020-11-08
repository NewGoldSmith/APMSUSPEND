#pragma once
#include "resource.h"

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
//	CEditCustom m_ctl_Edit_Resume_sleep_time;
public:
//	CEdit m_ctl_Edit_Resume_sleep_time;
	CEdit m_ctlEdit_resume_sleep_time;
//	afx_msg void OnBnClickedButtonCreateShortcut();
//	virtual HRESULT accDoDefaultAction(VARIANT varChild);
private:
//	void CreateDesktopShortcut();
public:
	afx_msg void OnBnClickedButtonCreateShortcutStartup();
	afx_msg void OnBnClickedButtonDeleteShortcutStartup();
	afx_msg void OnBnClickedButtonCreateShortcutDesktop();
	afx_msg void OnBnClickedButtonDeleteShortcutDesktop();
	afx_msg void OnBnClickedButtonCreateShortcutStartmenu();
	afx_msg void OnBnClickedButtonDeleteShortcutStartmenu();
	void DeleteLink(int csidl);
private:
	void MoveLink(int csidlDest, int csidlSource);
public:
	bool FindLink(int csidlPath);
	void GetLinkPathName(int csidlPath, LPTSTR pszPathName);
private:
	void CreateShortCut(int csidl);
};
