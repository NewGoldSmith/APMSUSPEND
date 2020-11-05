// CStaticBmpControl.cpp : 実装ファイル
//

#include "pch.h"
#include "APMSUSPEND.h"
#include "CStaticBmpControl.h"


// CStaticBmpControl

IMPLEMENT_DYNAMIC(CStaticBmpControl, CStatic)

CStaticBmpControl::CStaticBmpControl()
{
	m_is_lamp = FALSE;
}

CStaticBmpControl::~CStaticBmpControl()
{
}


BEGIN_MESSAGE_MAP(CStaticBmpControl, CStatic)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CStaticBmpControl メッセージ ハンドラー




void CStaticBmpControl::OnDestroy()
{
	CStatic::OnDestroy();

	// TODO: ここにメッセージ ハンドラー コードを追加します。
}

void CStaticBmpControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。

	CStatic::OnLButtonDown(nFlags, point);
}


void CStaticBmpControl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ここにメッセージ ハンドラー コードを追加します。
					   // 描画メッセージで CStatic::OnPaint() を呼び出さないでください。
 
	// PictureCotrolの領域を取得します。
	 CRect rect;
	 GetClientRect(&rect);

	 // Bitmapファイルをロードします。
	 CBitmap bmp_on,bmp_off,bmp_mask;
	 bmp_on.LoadBitmap(IDB_BITMAP_POWER_ON_LAMP);
	 bmp_off.LoadBitmap(IDB_BITMAP_POWER_OFF_LAMP);
	 bmp_mask.LoadBitmap(IDB_BITMAP_MASK);
	 // ビットマップファイルを表示します。
	 CDC mdc_on,mdc_off,mdc_mask;
	 mdc_on.CreateCompatibleDC(&dc);
	 mdc_on.SelectObject(bmp_on);
	 mdc_off.CreateCompatibleDC(&dc);
	 mdc_off.SelectObject(bmp_off);
	 mdc_mask.CreateCompatibleDC(&dc);
	 mdc_mask.SelectObject(bmp_mask);
	 dc.BitBlt(0, 0, rect.Width(), rect.Height(), &mdc_mask, 0, 0, SRCAND);
	 if (m_is_lamp == FALSE)
	 {
		 dc.BitBlt(0, 0, rect.Width(), rect.Height(), &mdc_off, 0, 0, SRCPAINT);
	 }
	 else
	 {
		 dc.BitBlt(0, 0, rect.Width(), rect.Height(), &mdc_on, 0, 0, SRCPAINT);
	 }
 }

BOOL CStaticBmpControl::LampOn(BOOL b=1)
{
	m_is_lamp = b;
	Invalidate();
	UpdateWindow();
	return b;
}


BOOL CStaticBmpControl::IsLampOn()
{
	return m_is_lamp;
}

BOOL CStaticBmpControl::operator =(BOOL b)
{
	m_is_lamp = b;
	if (IsWindow(*this))
	{
		Invalidate();
		UpdateWindow();
	}
	return m_is_lamp;
}

BOOL CStaticBmpControl::operator =(CStaticBmpControl c)
{
	m_is_lamp = c.m_is_lamp;
	if (IsWindow(*this))
	{
		Invalidate();
		UpdateWindow();
	}
	return m_is_lamp;
}

