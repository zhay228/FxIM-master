// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <Commdlg.h>
#include <shlobj.h>
#include <Dbghelp.h>					// For MakeSureDirectoryPathExists
#include <Wincrypt.h>					// �ӽ���
#include <atlbase.h>
#include <atlwin.h>
#include <shellapi.h>
#include <winsock2.h>
#include <iphlpapi.h>					// for SendARP��GetAdaptersInfo

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// STL
#include <map>


#include "Utils/md5.h"
#include "Utils/Utils.h"
#include "Utils/BaseDef.h"
#include "Utils/singleton.h"
// Xml parser
//[5/8/2014 ybt] 'Markup.cpp' contains 'windows.h' caused this error : 
//fatal error C1189: #error :  WINDOWS.H already included.  MFC apps must not #include <windows.h>
//#define MARKUP_STL
//#include "Markup.h"


// Duilib
#include "ThirdParty/DuiLib/UIlib.h"
#include "ThirdParty/DuiLib/Utils/WinImplBase.h"
using namespace DuiLib;

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "../ThirdParty/Duilib/lib/DuiLib_ud.lib")
#   else
#       pragma comment(lib, "../ThirdParty/Duilib/lib/DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "../ThirdParty/Duilib/lib/DuiLib_u.lib")
#   else
#       pragma comment(lib, "../ThirdParty/Duilib/lib/DuiLib.lib")
#   endif
#endif

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Dbghelp.lib")

#pragma warning(disable: 4996)					// ����VS2008��Ϊstrcat��strdup��strncpy�Ⱥ�������ȫ�ľ���
// #pragma warning(disable: 4832)

// http://blog.csdn.net/bagboy_taobao_com/article/details/6415637
// http://msdn.microsoft.com/en-us/library/abx4dbyh(v=vs.80).aspx
// DuiLib_d.lib(UILabel.obj) : error LNK2005: "public: __thiscall std::_Container_base_secure::
// ~_Container_base_secure(void)" (??1_Container_base_secure@std@@QAE@XZ) already defined in msvcprtd.lib(MSVCP90D.dll) 
// �˴���������Project->C/C++->Code Generation->Runtime Library ���ѡ�LIB��������EXE����ѡ��Ĳ�һ������ġ���Ҫ�ĳ�һ��
// [7/23/2014 ybt]
