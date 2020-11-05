// CEditCustom.cpp : 実装ファイル
//

#include "pch.h"
#include "APMSUSPEND.h"
#include "CEditCustom.h"


// CEditCustom

IMPLEMENT_DYNAMIC(CEditCustom, CEdit)

CEditCustom::CEditCustom()
{

}

CEditCustom::~CEditCustom()
{
}


BEGIN_MESSAGE_MAP(CEditCustom, CEdit)
//	ON_WM_PAINT()
END_MESSAGE_MAP()



// CEditCustom メッセージ ハンドラー




//void CEditCustom::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
//					   // TODO: ここにメッセージ ハンドラー コードを追加します。
//					   // 描画メッセージで CEdit::OnPaint() を呼び出さないでください。
///*	CRect rect;
//	CString str;
//	TEXTMETRIC met;
//	GetWindowText(str);
//	dc.GetTextMetrics(&met);
//	GetWindowRect(&rect);
//*/	
///*	CString m_data = _T("1234567890");
//	CDC* pdc = GetDC();
//	CSize size = pdc->GetTextExtent(m_data);
//	CRect rect, rect2;
//	GetClientRect(rect);
//	rect2 = rect;
//	rect2.top = (rect.bottom - size.cy) / 2 + 50;
//	rect2.bottom = rect2.top + size.cy;
//	rect2.bottom = 50;
//	SetRect(rect2);
//	*/
//
//
//}
