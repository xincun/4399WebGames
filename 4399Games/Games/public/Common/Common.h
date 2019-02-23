#pragma once
#include <string>
using std::string;
using std::wstring;




void MyParseUrlW( const wstring& strUrl, wstring& strHostName, wstring& strPage, WORD& sPort );
void MyParseUrlA( const string& strUrl, string& strHostName, string& strPage, WORD& sPort );
bool RegWritePca();

string	U2A(const wstring& str);
wstring A2U(const string& str);
string	U2Utf(const wstring& wstrUnicode);
wstring Utf2U(const string &str);

wstring ToUpper(const wstring& str);
wstring ToLower(const wstring& str);

wstring GetNameByPathW( const wstring& strPath, bool bIsUrl );
wstring GetRunPathW();