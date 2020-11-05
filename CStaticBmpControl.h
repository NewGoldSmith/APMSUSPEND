#pragma once

#include "resource.h"

// CStaticBmpControl

class CStaticBmpControl : public CStatic
{
	DECLARE_DYNAMIC(CStaticBmpControl)

public:
	CStaticBmpControl();
	virtual ~CStaticBmpControl();
private:
	CStaticBmpControl(CStaticBmpControl& c):m_is_lamp(FALSE) {  };
	CStaticBmpControl& operator =(CStaticBmpControl& c) {
		return (*this);}

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	BOOL LampOn(BOOL b);
	BOOL IsLampOn();
	BOOL operator =(BOOL b);
	BOOL operator =(CStaticBmpControl  c);

protected:
	BOOL m_is_lamp;
};


