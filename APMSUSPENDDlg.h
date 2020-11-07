
// APMSUSPENDDlg.h : ヘッダー ファイル
//

#pragma once

#include "CStaticBmpControl.h"
#include "CAdvacedSettingDlg.h"
#include <atlpath.h>

#define WM_TASKTRAY     WM_USER + 1 

// CAPMSUSPENDDlg ダイアログ
class CAPMSUSPENDDlg : public CDialogEx
{
// コンストラクション
public:
	CAPMSUSPENDDlg(CWnd* pParent = nullptr);	// 標準コンストラクター
	~CAPMSUSPENDDlg();
// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APMSUSPEND_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg UINT OnPowerBroadcast(UINT nPowerEvent, LPARAM nEventData);
	afx_msg void OnBnClickedButtonGetPath();
	afx_msg void OnBnClickedButtonCreateProcess();
	PROCESS_INFORMATION m_PI;
public:
	afx_msg void OnBnClickedButtonCloseProcess();
protected:
	static UINT Call_Target_end_detect(LPVOID p);
	CWinThread * mp_cThread_Detect_End;
public:
	CSemaphore m_cSem;
	CSemaphore m_cSem_Callback_Alive;
protected:
	CString m_str_Target_path;
public:
	virtual BOOL DestroyWindow();
	BOOL m_b_safe_suspend_support_on;
	int m_i_Radiko_is_startup;
	BOOL m_b_restore_prev_target_startup_state;
private:
	void CreateTargetProcess();
protected:
	BOOL m_b_IsHide;
public:
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
protected:
	afx_msg LRESULT OnTasktray(WPARAM wParam, LPARAM lParam);
public:
	bool TrayNotifyIcon(DWORD dwMessage);
	afx_msg void OnDestroy();
	int m_i_show_maindlg_state;
	afx_msg void OnAppShow();
	afx_msg void OnAppIcon();
	afx_msg void OnTargetSetPath();
	afx_msg void OnTargetStartUp();
	afx_msg void OnTargetClose();
	afx_msg void OnBnClickedOk();
	BOOL m_b_use_task_tray;
	afx_msg void OnClickedCheckUseTaskTray();
	afx_msg void OnBnClickedButtonDoTasktray();
	CButton m_ctlChk_use_tasktray;
	CButton m_ctlChk_safe_suspend_suport_on;
private:
	CStatic m_ctlStatic_Target_path;
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnAppCancel();
	int m_is_power_state;
	CStaticBmpControl m_ctlImgCtl_Powerlamp;
public:
	virtual void Dump(CDumpContext& dc) const;
	BOOL m_b_Target_had_startup;
	afx_msg void OnBnClickedButtonTestSuspend();
	afx_msg void OnBnClickedButtonTestResume();
	afx_msg void OnBnClickedCheckRestorePreviousStartup();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonOpenAdvancedSettingDlg();
protected:
	int m_i_resume_wait_time;
private:
	CButton m_ctlBtn_create_process;
public:
	CButton m_ctlBtn_test_resume;
	CButton m_ctlBtn_test_suspend;
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnMenuSave();
	CButton m_ctlBtn_save;
	CButton m_ctlBtn_cancel;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	CButton m_ctlRdo_MainDlg_Show;
	afx_msg void OnBnClickedRadioShowIcon();
	afx_msg void OnBnClickedRadioShowWindow();
	afx_msg void OnBnClickedRadioShowTasktray();
	afx_msg void OnClickedCheckTargetSupportOn();
	CButton m_ctlChk_restre_prev_startup_state;
protected:
	CSingleLock m_cSL_alock;
	CSingleLock m_cSL_lock;
};
