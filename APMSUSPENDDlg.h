// APMSUSPENDDlg.h : ヘッダー ファイル
//

#pragma once

#include "CStaticBmpControl.h"
#include "CAdvacedSettingDlg.h"
#include <atlpath.h>
#include "CCtmMenu.h"

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
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTasktray(WPARAM wParam, LPARAM lParam);
	afx_msg UINT OnPowerBroadcast(UINT nPowerEvent, LPARAM nEventData);
	afx_msg void OnAppCancel();
	afx_msg void OnAppIcon();
	afx_msg void OnAppShow();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCheckRestorePreviousStartup();
	afx_msg void OnBnClickedCheckUseTaskTray();
	afx_msg void OnBnClickedCloseProcess();
	afx_msg void OnBnClickedCreateProcess();
	afx_msg void OnBnClickedGetPath();
	afx_msg void OnBnClickedGoTasktray();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedOpenAdvancedSettingDlg();
	afx_msg void OnBnClickedRadioShowIcon();
	afx_msg void OnBnClickedRadioShowTasktray();
	afx_msg void OnBnClickedRadioShowWindow();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedTestResume();
	afx_msg void OnBnClickedTestSuspend();
	afx_msg void OnClickedCheckTargetSupportOn();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnGoTasktray();
	afx_msg void OnMenuSave();
	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnTargetClose();
	afx_msg void OnTargetSetPath();
	afx_msg void OnTargetStartUp();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	BOOL m_b_is_hide;
	BOOL m_b_restore_Prev_target_startup_state;
	BOOL m_b_safe_suspend_support_on;
	BOOL m_b_Target_had_startup;
	BOOL m_b_use_task_tray;
	bool TrayNotifyIcon(DWORD dwMessage);
	CButton m_cRdn_MaindlgIcon;
	CButton m_cRdn_MaindlgTasktray;
	CButton m_ctlBn_do_tasktray;
	CButton m_ctlBn_save;
	CButton m_ctlBtn_cancel;
	CButton m_ctlBtn_create_process;
	CButton m_ctlBtn_test_resume;
	CButton m_ctlBtn_test_suspend;
	CButton m_ctlChk_restre_Prev_startup_state;
	CButton m_ctlChk_safe_suspend_suport_on;
	CButton m_ctlChk_use_tasktray;
	CButton m_ctlRdb_mainDlg_showis;
	CButton m_ctlRdb_show_icon_maindlg;
	CButton m_ctlRdb_show_tasktray_mainDlg;
	CSemaphore m_cSem;
	CSemaphore m_cSem_callback_alive;
	CSingleLock m_cSL_alive_lock;
	CSingleLock m_cSL_lock;
	CStatic m_ctlStc_target_path;
	CStaticBmpControl m_ctlImgCtl_lamp;
	CString m_str_target_path;
	CWinThread* mp_cThread_detect_end;
	DECLARE_MESSAGE_MAP()
	int DoChangeSaveBtn();
	int m_i_resume_wait_time;
	int m_i_show_mainDlg_state;
	int m_i_target_is_startup;
	int m_is_power_state;
	int MainDlgRdbGetCheck();
	int MainDlgRdbSetCheck(int bID);
	int UpdateRdbData(BOOL True_down);
	PROCESS_INFORMATION m_PI;
	static UINT Call_Target_end_detect(LPVOID p);
	virtual BOOL DestroyWindow();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void Dump(CDumpContext& dc) const;
	void CreateTargetProcess();
};
