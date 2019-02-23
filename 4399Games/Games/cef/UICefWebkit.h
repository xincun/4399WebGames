#pragma once
#include "CefBase/util.h"
#include "CefBase/CefHandler.h"
#include "CefBase/CefAppEx.h"
#include "CefBase/CEfV8Handler.h"

class CCefWebkitUI
	: public CControlUI
{
public:
	CCefWebkitUI(LPCTSTR lpUrl = L"");
	virtual ~CCefWebkitUI();
	virtual LPCTSTR	GetClass() const;
	virtual LPVOID	GetInterface(LPCTSTR pstrName);
	virtual void	SetPos(RECT rc);
	virtual void	DoInit();
	virtual void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void	SetInternVisible(bool bVisible = true);
	HWND	GetHostWnd();
	wstring	GetLoadingUrl();
	void	Close();
	void	SetWebloadCallback(CCefWebLoadCallback* pCallback, void* lpParam);
	void	ExecuteJavascript(const wstring& strCode);
	CefRefPtr<CefBrowser>	GetBrowser();
	CefRefPtr<CefFrame>		GetMainFrame();
	CCefHandler* GetCefHandle()			{ return m_cefHandles.get(); }
	bool	CanGoBack()					{ return (m_pWebBrowser.get())?m_pWebBrowser->CanGoBack():false; }
	void	SetTitle(LPCTSTR lpTitle)	{ m_strTitle = lpTitle; }
	LPCTSTR	GetTitle()const				{ return m_strTitle.c_str(); }
	bool	IsTitleEmpty()const			{ return m_strTitle.empty(); }
	void	MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRePaint);
	void	Navigate(LPCTSTR lpUrl);
	void	Refresh();
	void	Reload();
	void	GoBack();
	void	GoForward();
private:
	wstring	m_strHomePage;
	wstring	m_strTitle;
	CefRefPtr<CefBrowser>	m_pWebBrowser;
	CefRefPtr<CCefHandler>	m_cefHandles;
};