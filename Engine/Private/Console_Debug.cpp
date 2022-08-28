#include "..\Public\Console_Debug.h"

IMPLEMENT_SINGLETON(CConsole_Debug)

CConsole_Debug::CConsole_Debug()
{
}

unsigned int APIENTRY ThreadEntryFunc_Console_Debug(void* pArg)
{
	CConsole_Debug* pConsole_Debug = (CConsole_Debug*)pArg;
	EnterCriticalSection(&pConsole_Debug->Get_CriticalSection());

	while (true)
	{
		system("cls");
		cout << "���̾������� (wireframe on/off)" << endl;
		cout << "�ְ� (distortion on/off)" << endl;
		cout << "���۵� ����Ÿ�� (deferred on/off)" << endl;
		cout << "������ ����Ÿ�� (lightacc on/off)" << endl;
		cout << "������ ����Ÿ�� (lightdepth on/off)" << endl;
		cout << "HDR ����Ÿ�� (hdr on/off)" << endl;
		cout << "��� ����Ÿ�� (final on/off)" << endl;
		cout << "����Ʈ ����Ÿ�� (effect on/off)" << endl;

		char szBuff[MAX_PATH] = "";
		cin.getline(szBuff, MAX_PATH);

#ifdef _DEBUG
		if (!strcmp("wireframe on\r", szBuff))
		{
			g_bWireFrame = true;
		}
		else if (!strcmp("wireframe off\r", szBuff))
		{
			g_bWireFrame = false;
		}
		else if (!strcmp("distortion on\r", szBuff))
		{
			g_bDistortion = true;
		}
		else if (!strcmp("distortion off\r", szBuff))
		{
			g_bDistortion = false;
		}
		else if (!strcmp("deferred on\r", szBuff))
		{
			g_bDeferred = true;
		}
		else if (!strcmp("deferred off\r", szBuff))
		{
			g_bDeferred = false;
		}
		else if (!strcmp("lightacc on\r", szBuff))
		{
			g_bLightAcc = true;
		}
		else if (!strcmp("lightacc off\r", szBuff))
		{
			g_bLightAcc = false;
		}
		else if (!strcmp("lightdepth on\r", szBuff))
		{
			g_bLightDepth = true;
		}
		else if (!strcmp("lightdepth off\r", szBuff))
		{
			g_bLightDepth = false;
		}
		else if (!strcmp("hdr on\r", szBuff))
		{
			g_bHDR = true;
		}
		else if (!strcmp("hdr off\r", szBuff))
		{
			g_bHDR = false;
		}
		else if (!strcmp("final on\r", szBuff))
		{
			g_bFinal = true;
		}
		else if (!strcmp("final off\r", szBuff))
		{
			g_bFinal = false;
		}
		else if (!strcmp("effect on\r", szBuff))
		{
			g_bEffect = true;
		}
		else if (!strcmp("effect off\r", szBuff))
		{
			g_bEffect = false;
		}
#endif // _DEBUG
	}
	LeaveCriticalSection(&pConsole_Debug->Get_CriticalSection());
	return 0;
}

HRESULT CConsole_Debug::NativeConstruct()
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	InitializeCriticalSection(&m_CriticalSection);

	if (::AllocConsole() == TRUE)
	{
		FILE* nfp[3];
		freopen_s(nfp + 0, "CONIN$", "rb", stdin);
		freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
		freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
		std::ios::sync_with_stdio();
	}

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntryFunc_Console_Debug, this, 0, nullptr);
	if (0 == m_hThread)
	{
		MSG_BOX(L"Failed To CConsole_Debug : NativeConstruct : 0 == m_hThread");
		return E_FAIL;
	}
	m_bInit = true;
	return S_OK;
}

CRITICAL_SECTION CConsole_Debug::Get_CriticalSection()
{
	return m_CriticalSection;
}

void CConsole_Debug::Free()
{
	if (true == m_bInit)
	{
		WaitForSingleObject(m_hThread, IGNORE);
		DeleteCriticalSection(&m_CriticalSection);
		DeleteObject(m_hThread);
		CloseHandle(m_hThread);

		FreeConsole();
	}
}