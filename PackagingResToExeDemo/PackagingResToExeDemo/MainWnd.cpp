#include "StdAfx.h"
#include "MainWnd.h"

CMainWnd::CMainWnd()
{
}

CMainWnd::~CMainWnd()
{
}

CControlUI* CMainWnd::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

LRESULT CMainWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;	//取消标题框
	styleValue &= ~WS_SIZEBOX;	//取消自动调整边框的风格,可以放置贴边自动最大化
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_PM.Init(m_hWnd);
	CDialogBuilder builder; 
	CControlUI* pRoot = builder.Create(IDF_XML1, _T("xml"),  this, &m_PM);
	ASSERT(pRoot && "Failed to parse XML");
	m_PM.AttachDialog(pRoot);
	m_PM.AddNotifier(this);

	return 0;
}

void CMainWnd::InitDlg()
{
}

LRESULT CMainWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;

	switch( uMsg ) 
	{
	case WM_CREATE:				lRes = OnCreate(uMsg, wParam, lParam, bHandled);		break;
	case WM_NCHITTEST:			lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled);		break;
	case WM_SYSCOMMAND:			lRes = OnSysCommand(uMsg, wParam, lParam, bHandled);	break;
	case WM_KEYDOWN:
		{
			if (wParam == VK_ESCAPE)
			{
				Close();
			}
			else
				bHandled = FALSE;
		}
	default:
		bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( m_PM.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CMainWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
	{
		InitDlg();
	}
}

LRESULT CMainWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = m_PM.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_PM.FindControl(pt));
			if( pControl && _tcscmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0 && 
				_tcscmp(pControl->GetClass(), DUI_CTR_OPTION) != 0 && 
				_tcscmp(pControl->GetClass(), _T("CButtonUIEx")) != 0)
				return HTCAPTION;
	}
	return HTCLIENT;
}

LRESULT CMainWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == SC_CLOSE ) 
	{
		Close(0);

		bHandled = TRUE;
		return 0;
	}
	else if (wParam == SC_KEYMENU)	//RWH 处理打开系统菜单的响应,屏蔽掉ALT+SPACE调出系统菜单的操作
		return 0;

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);//必须保留，否则消息分发就出问题了。
}