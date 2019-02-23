#include "StdAfx.h"
#include "HttpClient.h"
#pragma comment(lib, "Winhttp")
#include <assert.h>


#define HTTP_READBUF_LEN	1024*1024		//1MµƒΩ” ’ª∫≥Â«¯

CHttpClient::CHttpClient(void)
	:m_hInternet(NULL)
	,m_hConnect(NULL)
	,m_hRequest(NULL)
	,m_pLoadCallback(NULL)
	,m_lpParam(NULL)
	,m_nConnTimeout(5000)
	,m_nSendTimeout(5000)
	,m_nRecvTimeout(5000)
	,m_pStopCallback(NULL)
{
	Init();
}

CHttpClient::~CHttpClient(void)
{
	Release();
}

bool CHttpClient::Init()
{
	m_hInternet = ::WinHttpOpen(
		L"Microsoft Internet Explorer",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		0);
	if ( NULL == m_hInternet )
	{
		SetError(L"WinHttpOpen≥ı ºªØ ß∞‹");
		return false;
	}
	::WinHttpSetTimeouts(m_hInternet, 0, m_nConnTimeout, m_nSendTimeout, m_nRecvTimeout);
	return true;
}

void CHttpClient::Release()
{
	CloseInternetHandle(&m_hRequest);
	CloseInternetHandle(&m_hConnect);
	CloseInternetHandle(&m_hInternet);
}

bool CHttpClient::ConnectHttpServer(LPCTSTR lpIP, WORD wPort)
{
	m_hConnect = ::WinHttpConnect(m_hInternet, lpIP, wPort, 0);
	return m_hConnect != NULL;
}

bool CHttpClient::CreateHttpRequest(LPCTSTR lpPage, RequestType type, DWORD dwFlag/*=0*/)
{
	wchar_t* pVerb = (type == Get)?L"GET":L"POST";
	m_hRequest = ::WinHttpOpenRequest(
		m_hConnect,
		pVerb,
		lpPage,
		NULL,
		WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES, 
		dwFlag);
	return m_hRequest != NULL;
}

void CHttpClient::SetTimeOut( int dwConnectTime, int dwSendTime, int dwRecvTime )
{
	m_nConnTimeout = dwConnectTime;
	m_nSendTimeout = dwSendTime;
	m_nRecvTimeout = dwRecvTime;
}

bool CHttpClient::DownLoadFile( LPCTSTR lpUrl, LPCTSTR lpFilePath )
{
	Release();
	if ( !Init() )
		return false;
	bool bRet = false;
	DWORD dwBytesToRead = 0, dwFileSize = 0, dwReadSize=0, dwRecvSize =0;
	if ( !InitConnect(lpUrl, Get) )
		return false;
	if ( !QueryContentLength(dwFileSize) )
	{
		SetError(L"QueryContentLength≤È—ØHTTP≥§∂» ß∞‹");
		return false; 
	}
	wstring strHeaders;
	bool bQuery = QueryRawHeaders(strHeaders);
	if ( bQuery && (strHeaders.find(L"404")!=wstring::npos) )
	{
		SetError(L"QueryRawHeaders404¥ÌŒÛ");
		return bRet;
	}
	HANDLE hFile = CreateFile(lpFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		OutputDebugString(L"¥¥Ω®œ¬‘ÿŒƒº˛ ß∞‹£°\n");
		return bRet;
	}
	SetFilePointer(hFile, dwFileSize, 0, FILE_BEGIN);
	SetEndOfFile(hFile);
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	if ( !::WinHttpQueryDataAvailable(m_hRequest, &dwBytesToRead) )
	{
		CloseHandle(hFile);
		DeleteFile(lpFilePath);
		return bRet;
	}
	if( m_pStopCallback && m_pStopCallback->IsNeedStop() )
	{
		CloseHandle(hFile);
		DeleteFile(lpFilePath);
		return bRet;
	}
	void* lpBuff = malloc(HTTP_READBUF_LEN);
	while( true )
	{
		if( m_pStopCallback && m_pStopCallback->IsNeedStop() )
			break;
		if ( dwBytesToRead>HTTP_READBUF_LEN )
		{
			free(lpBuff);
			lpBuff = malloc(dwBytesToRead);
		}
		if ( !::WinHttpReadData(m_hRequest, lpBuff, dwBytesToRead, &dwReadSize) )
			break;
		if( m_pStopCallback && m_pStopCallback->IsNeedStop() )
			break;
		DWORD dwWriteByte;
		if ( !WriteFile(hFile, lpBuff, dwReadSize, &dwWriteByte, NULL) || (dwReadSize != dwWriteByte) )
			break;
		dwRecvSize += dwReadSize;
		if( m_pLoadCallback )
			m_pLoadCallback->OnHttpLoading(dwFileSize, dwRecvSize, m_lpParam);
		if ( !::WinHttpQueryDataAvailable(m_hRequest, &dwBytesToRead) )
			break;
		if ( dwBytesToRead<= 0 )
		{
			bRet = true;
			break;
		}
	}
	free(lpBuff);
	CloseHandle(hFile);
	if ( !bRet )
	{//œ¬‘ÿ ß∞‹£¨…æ≥˝Œƒº˛
		DeleteFile(lpFilePath);
	}
	return bRet;
}

bool CHttpClient::DownLoadMem( LPCTSTR lpUrl, OUT void** lpBuffer, OUT DWORD& dwSize )
{
	bool bResult = false;
	if ( !InitConnect(lpUrl, Get) )
		return false;
	DWORD dwLength = 0, dwBytesToRead = 0,  dwReadSize = 0, dwRecvSize = 0;
	if ( !QueryContentLength(dwLength) )
	{
		SetError(L"QueryContentLength≤È—ØHTTP≥§∂» ß∞‹");
		return false; 
	}
	wstring strHeaders;
	bool bQuery = QueryRawHeaders(strHeaders);
	if ( bQuery && (strHeaders.find(L"404")!=wstring::npos) )
	{
		SetError(L"QueryRawHeaders404¥ÌŒÛ");
		return false;
	}
	if ( !::WinHttpQueryDataAvailable(m_hRequest, &dwBytesToRead) )
		return false;
	BYTE* lpData = (BYTE*)malloc(dwLength);
	void* lpBuff = malloc(HTTP_READBUF_LEN);
	while( true )
	{
		if ( dwBytesToRead>HTTP_READBUF_LEN )
		{
			free(lpBuff);
			lpBuff = malloc(dwBytesToRead);
		}
		if ( !::WinHttpReadData(m_hRequest, lpBuff, dwBytesToRead, &dwReadSize) )
			break;
		memcpy(lpData+dwRecvSize, lpBuff, dwReadSize);
		dwRecvSize += dwReadSize;
		if ( !::WinHttpQueryDataAvailable(m_hRequest, &dwBytesToRead) )
			break;
		if ( dwBytesToRead<= 0 )
		{
			bResult = true;
			break;
		}
	}
	free(lpBuff);
	if ( bResult )
	{
		*lpBuffer = lpData;
		dwSize = dwRecvSize;
	}
	else
		free(lpData);
	return bResult;
}

string CHttpClient::Request( LPCTSTR lpUrl, RequestType type, LPCSTR lpPostData/* = NULL*/ )
{
	string strRet;
	if ( !InitConnect(lpUrl, type, lpPostData) )
		return strRet;
	DWORD dwBytesToRead, dwReadSize;
	void* lpBuff = malloc(HTTP_READBUF_LEN);
	bool bFinish = false;
	while ( true )
	{
		if ( !::WinHttpQueryDataAvailable(m_hRequest, &dwBytesToRead) )
			break;
		if ( dwBytesToRead<= 0 )
		{
			bFinish = true;
			break;
		}
		if ( dwBytesToRead>HTTP_READBUF_LEN )
		{
			free(lpBuff);
			lpBuff = malloc(dwBytesToRead);
		}
		if ( !::WinHttpReadData(m_hRequest, lpBuff, dwBytesToRead, &dwReadSize) )
			break;
		strRet.append((const char*)lpBuff, dwReadSize);
	}
	free(lpBuff);
	if ( !bFinish )
		strRet.clear();
	return strRet;
}

bool CHttpClient::DownloadFileForRetry( LPCTSTR lpUrl, const wstring& strFilePath, int nTryCount /*= 3*/ )
{
	assert( nTryCount>0 );
	bool bRet = false;
	while( nTryCount>0 )
	{
		if ( DownLoadFile(lpUrl, strFilePath.c_str()) )
		{
			bRet = true;
			break;
		}
		//DWORD dwErrCode = GetLastError();
		//if ( ERROR_WINHTTP_TIMEOUT == GetLastError() )
		nTryCount--;
		Sleep(1000);	//–›√ﬂ1√Î÷”‘Ÿ≥¢ ‘
	}
	return bRet;
}

bool CHttpClient::QueryRawHeaders( OUT wstring& strHeaders )
{
	bool bRet = false;
	DWORD dwSize;
	BOOL bResult = ::WinHttpQueryHeaders(m_hRequest, WINHTTP_QUERY_RAW_HEADERS, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &dwSize, WINHTTP_NO_HEADER_INDEX);
	if ( ERROR_INSUFFICIENT_BUFFER == GetLastError() )
	{
		wchar_t* lpData = (wchar_t*)malloc(dwSize);
		bResult = ::WinHttpQueryHeaders(m_hRequest, WINHTTP_QUERY_RAW_HEADERS, WINHTTP_HEADER_NAME_BY_INDEX, lpData, &dwSize, WINHTTP_NO_HEADER_INDEX);
		if ( bResult )
		{
			strHeaders = lpData;
			bRet = true;
		}
		free(lpData);
	}
	return bRet;
}

bool CHttpClient::QueryContentLength( OUT DWORD& dwLength )
{
	bool bRet = false;
	wchar_t szBuffer[30] = {0};
	DWORD dwSize = 30*sizeof(wchar_t);
	if ( ::WinHttpQueryHeaders(m_hRequest, WINHTTP_QUERY_CONTENT_LENGTH, WINHTTP_HEADER_NAME_BY_INDEX, szBuffer, &dwSize, WINHTTP_NO_HEADER_INDEX) )
	{
		TCHAR *p = NULL;
		dwLength = wcstoul(szBuffer, &p, 10);
		bRet = true;
	}
	return bRet;
}

bool CHttpClient::InitConnect( LPCTSTR lpUrl, RequestType type, LPCSTR lpPostData/*=NULL*/ )
{
	Release();
	if ( !Init() )
		return false;
	wstring strHostName, strPage;
	WORD wPort;
	MyParseUrlW(wstring(lpUrl), strHostName, strPage, wPort);
	if ( !ConnectHttpServer(strHostName.c_str(), wPort) )
	{
		SetError(L"ConnectHttpServer ß∞‹");
		return false;
	}
	if ( !CreateHttpRequest(strPage.c_str(), type) )
	{
		SetError(L"CreateHttpRequest ß∞‹");
		return false;
	}
	if ( !SendHttpRequest(lpPostData) )
	{
		SetError(L"SendHttpRequest ß∞‹");
		return false;
	}
	if ( !WinHttpReceiveResponse(m_hRequest, NULL) )
	{
		SetError(L"WinHttpReceiveResponse ß∞‹");
		return false;
	}
	return true;
}

bool CHttpClient::SendHttpRequest( LPCSTR lpPostData/*=NULL*/ )
{
	DWORD dwSize = (NULL==lpPostData)?0:strlen(lpPostData);
	return ::WinHttpSendRequest(m_hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, (LPVOID)lpPostData, dwSize, dwSize, NULL) == TRUE;
}



