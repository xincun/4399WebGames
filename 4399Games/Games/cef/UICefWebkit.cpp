#include "StdAfx.h"
#include "UICefWebkit.h"
// #include "CefBase/util.h"
// #include "CefBase/CefHandler.h"
// #include "CefBase/CefAppEx.h"



//CefRefPtr<CCefHandler> CCefWebkitUI::m_cefHandles = NULL;
CCefWebkitUI::CCefWebkitUI(LPCTSTR lpUrl /*= L""*/)
: m_strHomePage(lpUrl)
, m_cefHandles(new CCefHandler)
{

}

CCefWebkitUI::~CCefWebkitUI()
{
	//Close();
}

LPCTSTR CCefWebkitUI::GetClass() const
{
	return L"CefWebkitUI";
}

LPVOID CCefWebkitUI::GetInterface( LPCTSTR pstrName )
{
	if ( _tcscmp(pstrName, L"CefWebkit") == 0 )
		return static_cast<CCefWebkitUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

void CCefWebkitUI::SetPos( RECT rc )
{
	CControlUI::SetPos(rc);
	if ( m_cefHandles )
	{
		HWND hWnd = GetHostWnd();
		if ( hWnd )
			::MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);
	}
}

void CCefWebkitUI::DoInit()
{
	RECT rc = {0, 0, 0, 0};
	m_cefHandles->CreateBrowser(m_pManager->GetPaintWindow(), rc, m_strHomePage.c_str());
	m_pWebBrowser = m_cefHandles->GetBrowser();
}

void CCefWebkitUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	if ( _tcscmp(pstrName, _T("url")) == 0 )
		m_strHomePage = pstrValue;
	else
		CControlUI::SetAttribute(pstrName, pstrValue);
}

void CCefWebkitUI::SetInternVisible( bool bVisible /*= true*/ )
{
	CControlUI::SetInternVisible(bVisible);
	HWND hWnd = GetHostWnd();
	if ( hWnd )
		::ShowWindow(hWnd, bVisible?SW_SHOW:SW_HIDE);
}

HWND CCefWebkitUI::GetHostWnd()
{
	if ( m_pWebBrowser.get() )
		return m_pWebBrowser->GetHost()->GetWindowHandle();
	return NULL;
}

wstring CCefWebkitUI::GetLoadingUrl()
{
	if ( m_pWebBrowser.get() )
	{
		CefString strUrl = m_pWebBrowser->GetMainFrame()->GetURL();
		if ( strUrl.empty() )
			return m_strHomePage;
		return strUrl.c_str();
	}
	return L"";
}

void CCefWebkitUI::Close()
{
	if ( m_pWebBrowser.get() )
		m_pWebBrowser->GetHost()->CloseBrowser(false);
}

void CCefWebkitUI::SetWebloadCallback( CCefWebLoadCallback* pCallback, void* lpParam )
{
	if ( m_cefHandles.get() )
		m_cefHandles->SetWebloadCallback(pCallback, lpParam);
}

CefRefPtr<CefBrowser> CCefWebkitUI::GetBrowser()
{
	return m_pWebBrowser;
}

void CCefWebkitUI::ExecuteJavascript( const wstring& strCode )
{
	if ( m_pWebBrowser.get() )
	{
		CefRefPtr<CefFrame> frame = m_pWebBrowser->GetMainFrame();
		if ( frame.get() )
		{
			CefString strCode(strCode.c_str()), strUrl(L"");
			frame->ExecuteJavaScript(strCode, strUrl, 0);
		}
	}
}

void CCefWebkitUI::Navigate( LPCTSTR lpUrl )
{
	if ( !m_pWebBrowser.get() || (NULL == lpUrl) )
		return ;
	CefRefPtr<CefFrame> pFrame = m_pWebBrowser->GetMainFrame();
	if ( (_tcslen(lpUrl)>0) && pFrame.get() )
		pFrame->LoadURL(lpUrl);
}

CefRefPtr<CefFrame> CCefWebkitUI::GetMainFrame()
{
	if ( m_pWebBrowser.get() )
	{
		return m_pWebBrowser->GetMainFrame();
	}
	return NULL;
}

void CCefWebkitUI::Reload()
{
	if ( m_pWebBrowser.get() )
	{
		m_pWebBrowser->Reload();
	}
}

void CCefWebkitUI::Refresh()
{
	if ( m_pWebBrowser.get() )
	{
		m_pWebBrowser->ReloadIgnoreCache();
	}
}

void CCefWebkitUI::GoBack()
{
	if ( m_pWebBrowser.get() && m_pWebBrowser->CanGoBack() )
		m_pWebBrowser->GoBack();
}

void CCefWebkitUI::GoForward()
{
	if ( m_pWebBrowser.get() && m_pWebBrowser->CanGoForward() )
		m_pWebBrowser->GoForward();
}

void CCefWebkitUI::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRePaint)
{
	HWND hWnd  = GetHostWnd();
	if ( hWnd )
		::MoveWindow(hWnd, x, y, nWidth, nHeight, bRePaint);
}
