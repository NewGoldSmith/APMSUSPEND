#pragma once
#include "pch.h"
//#include <afxwin.h>
#include "APMSUSPEND.h"
// CCtmMenu コマンド ターゲット

class CCtmSubMenu : public CMenu
{
public:
	CCtmSubMenu();
	virtual ~CCtmSubMenu();
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	BOOL Attach(HMENU hMenu);
protected:
	CFont m_smallFont;          // 追加します。
	CFont m_midiumFont;         // 追加します。
	CFont m_largeFont;          // 追加します。
	CSize MaxSize(CSize& sz1, CSize& sz2);
	CSize m_ItemSz;
	void DrawMenuItem(LPDRAWITEMSTRUCT lpDIS, UINT itemState);
};


