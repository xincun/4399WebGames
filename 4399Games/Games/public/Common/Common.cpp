#include "stdafx.h"
#include "Common.h"




void MyParseUrlW( const wstring& strUrl, wstring& strHostName, wstring& strPage, WORD& sPort )
{
	sPort = 80;
	wstring strTemp=strUrl;
	int nPos=strTemp.find(L"http://");
	if (wstring::npos != nPos )
		strTemp=strTemp.substr(nPos+7, strTemp.size()-nPos-7);
	nPos=strTemp.find('/');
	if ( wstring::npos == nPos )//没有找到 /
		strHostName = strTemp;
	else
		strHostName = strTemp.substr(0, nPos);
	int nPos1 = strHostName.find(':');
	if ( nPos1 != wstring::npos )
	{
		wstring strPort = strTemp.substr(nPos1+1, strHostName.size()-nPos1-1);
		strHostName = strHostName.substr(0, nPos1);
		sPort = (WORD)_wtoi(strPort.c_str());
	}
	if ( wstring::npos == nPos )
		return ;
	strPage = strTemp.substr(nPos, strTemp.size()-nPos);
}

void MyParseUrlA( const string& strUrl, string& strHostName, string& strPage, WORD& sPort )
{
	sPort = 80;
	string strTemp=strUrl;
	int nPos	= strTemp.find("http://");
	if ( string::npos != nPos )
		strTemp	= strTemp.substr(nPos+7, strTemp.size()-nPos-7);
	nPos	= strTemp.find('/');
	if ( string::npos == nPos )//没有找到 /
		strHostName	= strTemp;
	else
		strHostName = strTemp.substr(0, nPos);
	int nPos1 = strHostName.find(':');
	if ( nPos1 != string::npos )
	{
		string strPort = strTemp.substr(nPos1+1, strHostName.size()-nPos1-1);
		strHostName = strHostName.substr(0, nPos1);
		sPort = (WORD)atoi(strPort.c_str());
	}
	if ( string::npos == nPos )
		return ;
	strPage=strTemp.substr(nPos, strTemp.size()-nPos);
}

bool RegWritePca()
{
	HKEY hKey;
	if ( ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, 
		L"Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Persisted", 0, KEY_READ|KEY_WRITE, &hKey) )
	{
		return false;
	}
	wchar_t szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	DWORD dwCode = 1;
	LONG lRet = RegSetValueEx(hKey, szPath,	0, REG_DWORD, (byte*)&dwCode, sizeof(DWORD));
	RegCloseKey(hKey);
	return lRet == ERROR_SUCCESS;
}

string U2A(const wstring& str)
{
	string strDes;
	if ( str.empty() )
		goto __end;
	int nLen=::WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), NULL, 0, NULL, NULL);
	if ( 0==nLen )
		goto __end;
	char* pBuffer=new char[nLen+1];
	memset(pBuffer, 0, nLen+1);
	::WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), pBuffer, nLen, NULL, NULL);
	pBuffer[nLen]='\0';
	strDes.append(pBuffer);
	delete[] pBuffer;
__end:
	return strDes;
}

wstring A2U(const string& str)
{
	wstring strDes;
	if ( str.empty() )
		goto __end;
	int nLen=::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	if ( 0==nLen )
		goto __end;
	wchar_t* pBuffer=new wchar_t[nLen+1];
	memset(pBuffer, 0, nLen+1);
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), pBuffer, nLen);
	pBuffer[nLen]='\0';
	strDes.append(pBuffer);
	delete[] pBuffer;
__end:
	return strDes;
}

string U2Utf(const wstring& wstrUnicode)  
{  
	string strRet;
	if( wstrUnicode.empty() )
		return strRet;
	int nLen = WideCharToMultiByte(CP_UTF8, 0, wstrUnicode.c_str(), -1, NULL, 0, NULL, NULL);  
	char* pBuffer=new char[nLen+1];
	pBuffer[nLen] = '\0';
	nLen = WideCharToMultiByte(CP_UTF8, 0, wstrUnicode.c_str(), -1, pBuffer, nLen, NULL, NULL); 
	strRet.append(pBuffer);
	delete[] pBuffer;
	return strRet;  
}

wstring Utf2U(const string &str)
{
	int u16Len = ::MultiByteToWideChar(CP_UTF8, NULL,str.c_str(),(int)str.size(), NULL, 0);
	wchar_t* wstrBuf = new wchar_t[u16Len + 1];
	::MultiByteToWideChar(CP_UTF8, NULL, str.c_str(),(int)str.size(), wstrBuf, u16Len);
	wstrBuf[u16Len] = L'\0';
	wstring wStr;
	wStr.assign(wstrBuf, u16Len);
	delete [] wstrBuf;
	return wStr;
}

wstring ToUpper(const wstring& str)
{
	wstring strRet;
	for (size_t i=0; i<str.size(); ++i)
	{
		strRet.append(1, (wchar_t)towupper(str[i]));
	}
	return strRet;
}

wstring ToLower(const wstring& str)
{
	wstring strRet;
	for (size_t i=0; i<str.size(); ++i)
	{
		strRet.append(1, (wchar_t)towlower(str[i]));
	}
	return strRet;
}

wstring GetNameByPathW( const wstring& strPath, bool bIsUrl )
{
	wchar_t ch = bIsUrl?'/':'\\';
	int nPos = strPath.find_last_of(ch);
	if ( nPos == wstring::npos )
	{
		return L"";
	}
	return wstring(strPath.begin()+nPos+1, strPath.end());
}

wstring GetRunPathW()
{
	wchar_t szPath[MAX_PATH] = {0};
	GetModuleFileNameW(NULL, szPath, MAX_PATH);
	wstring strPath(szPath);
	int nPos = strPath.find_last_of(L"\\");
	strPath = strPath.substr(0, nPos+1);
	return strPath;
}
