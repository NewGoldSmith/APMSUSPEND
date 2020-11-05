
// APMSUSPENDDlg.h : ヘッダー ファイル
//

#pragma once

#include "CStaticBmpControl.h"
#include "CAdvacedSettingDlg.h"

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
	afx_msg void OnBnClickedButtonOpenPath();
	afx_msg void OnBnClickedButtonCreateProcess();
	PROCESS_INFORMATION m_PI;
public:
	afx_msg void OnBnClickedButtonCloseProcess();
protected:
	static UINT Call_Target_end_detect(LPVOID p);
	CWinThread * m_pThread;
public:
	CSemaphore m_sem;
	CSemaphore m_sm_Callback_Alive;
protected:
	CString m_Target_path;
public:
	virtual BOOL DestroyWindow();
	BOOL m_radiko_support_on;
	int m_Radiko_is_startup;
	BOOL m_is_state_save;
private:
	void CreateTargetProcess();
protected:
	bool m_IsHide;
public:
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
protected:
	afx_msg LRESULT OnTasktray(WPARAM wParam, LPARAM lParam);
public:
	bool TrayNotifyIcon(DWORD dwMessage);
	afx_msg void OnDestroy();
	int m_show_dlg_state;
	afx_msg void OnAppShow();
	afx_msg void OnAppIcon();
	afx_msg void OnAppClose();
	afx_msg void OnRadikoSetPath();
	afx_msg void OnRadikoStartUp();
	afx_msg void OnRadikoClose();
	afx_msg void OnBnClickedOk();
	BOOL m_use_task_tray;
	afx_msg void OnClickedCheckUseTaskTray();
	afx_msg void OnBnClickedButtonDoTasktray();
	CButton m_check_use_tasktray;
	CButton m_check_radiko_suport_on;
private:
	CStatic m_static_target_path;
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnAppCancel();
	void PowerLampON();
	afx_msg void OnClickedStaticPictureControlPowerIndigate();
	void PowerLampOff();
	int m_is_power_state;
	CStaticBmpControl m_DlgItem_Powerlamp;
public:
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void Dump(CDumpContext& dc) const;
	BOOL m_Radiko_had_startup;
//	BOOL m_thread_is_alive;
	afx_msg void OnBnClickedButtonTestSuspend();
	afx_msg void OnBnClickedButtonTestResume();
	afx_msg void OnBnClickedCheckMaintain();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonAdvancedOption();
protected:
	int m_resume_wait_time;
private:
	CButton m_button_create_process;
public:
	CButton m_button_test_resume;
	CButton m_button_test_suspend;
};
