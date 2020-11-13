// CCtmMenu.cpp : 実装ファイル
//

#include "pch.h"
#include "APMSUSPEND.h"
#include "CCtmMenu.h"


// CCtmMenu

CCtmSubMenu::CCtmSubMenu()
{
	// フォントを生成します。
	m_smallFont.CreatePointFont(80, _T("MS ゴシック"));
	m_midiumFont.CreatePointFont(160, _T("MS ゴシック"));
	m_largeFont.CreatePointFont(640, _T("MS ゴシック"));
}

CCtmSubMenu::~CCtmSubMenu()
{
}


// CCtmMenu のメンバー関数


void CCtmSubMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS/*lpMeasureItemStruct*/)
{
	//lpMIS
	static int c = 0;
	lpMIS->CtlType;//UINT
	  lpMIS->CtlID;//UINT
	  lpMIS->itemID;//UINT
	  lpMIS->itemWidth;//UINT
	  lpMIS->itemHeight;//UINT
	  lpMIS->itemData;//ULONG_PTR

	lpMIS->itemWidth = m_ItemSz.cx;
	lpMIS->itemHeight = m_ItemSz.cy;
	c++;
	TRACE(_T("\r\nCCtmSubMenu::MeasureItem itemID %d count%d\r\n"), lpMIS->itemID,c);
}


void CCtmSubMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS/*lpDrawItemStruct*/)
{
	//UINT        lpDIS->CtlType;
	//UINT        lpDIS->CtlID;
	//UINT        lpDIS->itemID;
	//UINT        lpDIS->itemAction;
	//UINT        lpDIS->itemState;
	//HWND        lpDIS->hwndItem;
	//HDC         lpDIS->hDC;
	//RECT        lpDIS->rcItem;
	//ULONG_PTR   lpDIS->itemData;

	if (!(lpDIS->CtlType == ODT_MENU))
	{
		CMenu::DrawItem(lpDIS);
		return;
	}

	//メニュー項目が全体表示される時の描画。
	if (lpDIS->itemAction & ODA_DRAWENTIRE)
	{
		CString str;

		if (lpDIS->itemState & ODS_DISABLED)
		{
			DrawMenuItem(lpDIS, ODS_DISABLED);
		}
		else
		{
			DrawMenuItem(lpDIS, ODS_NOFOCUSRECT);
		}
		return;
	}

	//メニュー項目が選択された瞬間の処理
	if ((lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & (ODA_SELECT /*| ODA_DRAWENTIRE*/)))
	{
		// item has been selected - hilite frame
		if (lpDIS->itemState & ODS_DISABLED)
		{
			DrawMenuItem(lpDIS, ODS_DISABLED);
		}
		else
		{
			DrawMenuItem(lpDIS, ODS_SELECTED);
		}
		return;
	}
//メニュー項目が選択から外れていった後の処理
	if (!(lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & ODA_SELECT))
	{
		// Item has been de-selected -- remove frame
		if (lpDIS->itemState & ODS_DISABLED)
		{
			DrawMenuItem(lpDIS, ODS_DISABLED);
		}
		else
		{
			DrawMenuItem(lpDIS, ODS_NOFOCUSRECT);
		}
		return;
	}
}

void CCtmSubMenu::DrawMenuItem(LPDRAWITEMSTRUCT lpDIS, UINT itemState)
{
	CBrush	brMenu,			//メニューの背景色
//		brMenuText,			//メニュー内のテキストの色
	brHighLight,		//選択された時の背景色
	brHighLightText,	//選択された時のテキスト色
//		brGrayText,			//無効のテキスト色
//		brActiveBorder,		//アクティブなウインドウの境界色
//		brInActiveBorder,	//非アクティブなウインドウの境界色
	brMenuHighLight;	//ハイライトされたメニューアイテムの色
//		brMenuBar;			//メニューバーの色
	VERIFY(brMenu.CreateSysColorBrush(COLOR_MENU) != 0);
//	VERIFY(brMenuText.CreateSysColorBrush(COLOR_MENUTEXT) != 0);
	VERIFY(brHighLight.CreateSysColorBrush(COLOR_HIGHLIGHT) != 0);
//	VERIFY(brHighLightText.CreateSysColorBrush(COLOR_HIGHLIGHTTEXT) != 0);
//	VERIFY(brGrayText.CreateSysColorBrush(COLOR_GRAYTEXT) != 0);
//	VERIFY(brActiveBorder.CreateSysColorBrush(COLOR_ACTIVEBORDER) != 0);
//	VERIFY(brInActiveBorder.CreateSysColorBrush(COLOR_INACTIVEBORDER) != 0);
	VERIFY(brMenuHighLight.CreateSysColorBrush(COLOR_MENUHILIGHT) != 0);
//	VERIFY(brMenuBar.CreateSysColorBrush(COLOR_MENUBAR) != 0);
	COLORREF ColMenuTxt, ColMenuGray,ColMenuHighLight;

	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	pDC->SelectObject(m_largeFont);
	pDC->SetBkMode(TRANSPARENT);
	ColMenuTxt = GetSysColor(COLOR_MENUTEXT);
	ColMenuGray = GetSysColor(COLOR_GRAYTEXT);
	ColMenuHighLight = GetSysColor(COLOR_HIGHLIGHTTEXT);
	CString str;

	if (itemState & ODS_DISABLED)
	{
		pDC->FillRect(&lpDIS->rcItem, &brMenu);
		CMenu::GetMenuString(lpDIS->itemID, str, MF_BYCOMMAND);
		pDC->SetTextColor(ColMenuGray);
		pDC->TextOutW(lpDIS->rcItem.left, lpDIS->rcItem.top, str);
		return;
	}

	if (itemState & ODS_NOFOCUSRECT)
	{
		pDC->FillRect(&lpDIS->rcItem, &brMenu);
		pDC->FrameRect(&lpDIS->rcItem, &brMenu);
		CMenu::GetMenuString(lpDIS->itemID, str, MF_BYCOMMAND);
		pDC->SetTextColor(ColMenuTxt);
		pDC->TextOutW(lpDIS->rcItem.left, lpDIS->rcItem.top, str);
		return;
	}
 
	if (itemState & ODS_SELECTED)
	{
		pDC->FillRect(&lpDIS->rcItem, &brMenuHighLight);
		pDC->FrameRect(&lpDIS->rcItem, &brMenuHighLight);
		CMenu::GetMenuString(lpDIS->itemID, str, MF_BYCOMMAND);
		pDC->SetTextColor(ColMenuHighLight);
		pDC->TextOutW(lpDIS->rcItem.left, lpDIS->rcItem.top, str);
		return;
	}


}


BOOL CCtmSubMenu::Attach(HMENU hMenu)
{
	TCHAR str[256];
	MENUITEMINFO Mii;
	Mii.cbSize= sizeof(MENUITEMINFO);
	//Mii.fMask;//| MIIM_ID;
	//Mii.fType;  // used if MIIM_TYPE (4.0) or MIIM_FTYPE (>4.0)
	//Mii.fState;        // used if MIIM_STATE
	//Mii.wID;           // used if MIIM_ID
	//Mii.hSubMenu;      // used if MIIM_SUBMENU
	//Mii.hbmpChecked;   // used if MIIM_CHECKMARKS
	//Mii.hbmpUnchecked; // used if MIIM_CHECKMARKS
	//Mii.dwItemData;   // used if MIIM_DATA
	Mii.dwTypeData = str;    // used if MIIM_TYPE (4.0) or MIIM_STRING (>4.0)
	//Mii.cch;           // used if MIIM_TYPE (4.0) or MIIM_STRING (>4.0)
//#if(WINVER >= 0x0500)
	Mii.hbmpItem= 0;      // used if MIIM_BITMAP

	CSize szi,szMax;
	szi.SetSize(0, 0);
	szMax = szi;

	CDC mdc;
	mdc.CreateCompatibleDC(NULL);
	mdc.SelectObject(m_largeFont);
			
	BOOL b=CMenu::Attach(hMenu);
	int j = GetMenuItemCount();
	int i = 0;
	for ( i; i <j ; ++i)
	{
		Mii.fMask = MIIM_STRING|MIIM_FTYPE| MIIM_ID;
		Mii.cch = 256;
		GetMenuItemInfo(i, &Mii, TRUE);

		if (Mii.fType != MFT_STRING)
		{
			TRACE(_T("\r\nCCtmSubMenu::Attach --------------------------------------------\r\n"));
			//Mii.fMask = MIIM_DATA;
			//Mii.dwItemData = -1;
			//SetMenuItemInfo(i, &Mii, TRUE);
			continue;
		}
		TRACE(_T("\r\nCCtmSubMenu::Attach str %s,wID %d\r\n"), str,Mii.wID);

		szi =mdc.GetTextExtent(str, Mii.cch);
		szMax = MaxSize(szi, szMax);

		Mii.fMask = MIIM_TYPE;
		Mii.fType |= MFT_OWNERDRAW ;
		SetMenuItemInfo(i, &Mii, TRUE);
	}
	mdc.DeleteDC();
	m_ItemSz = szMax;
	return b;
}


CSize CCtmSubMenu::MaxSize(CSize& sz1, CSize& sz2)
{
	CSize sz;
	sz.cx = max(sz1.cx, sz2.cx);
	sz.cy = max(sz1.cy, sz2.cy);
	return sz;
}

