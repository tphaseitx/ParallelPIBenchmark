// MultiThreadedPICore.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <cmath>
#include <cctype>
#include <powrprof.h>
#include <ppl.h>
#include <thread>

#include "Shellapi.h"

#include "ScreenImage.h"
#include "Base64Encoder.h"
#include "Gdiplusimaging.h"

using namespace concurrency;
using namespace std;

#define WM_UPDATE_CONTROL    WM_APP + 0x10

#define NHX 16 
char chx[NHX]; 

CString strImageBase64;

double series (int m, int id) 
{ 
	int k; 
	double ak, p, s, t; 
	double expm (double x, double y);

	#define eps 1e-17 
	s = 0.; 

	for (k = 0; k < id; k++)
	{ 
		ak = 8 * k + m; 
		p = id - k; 
		t = expm (p, ak); 

		s = s + t / ak; 
		s = s - (int) s; 
	} 

	for (k = id; k <= id + 100; k++)
	{ 
		ak = 8 * k + m; 
		t = pow (16., (double) (id - k)) / ak; 

		if (t < eps) 
		break; 

		s = s + t; 
		s = s - (int) s; 
	} 

	return s; 
} 

double expm (double p, double ak) 
{ 
	int i, j; 
	double p1, pt, r; 

	#define ntp 25 
	static double tp[ntp]; 

	static int tp1 = 0; 

	if (tp1 == 0) 
	{ 
		tp1 = 1; 
		tp[0] = 1.; 

		for (i = 1; i < ntp; i++) 
		tp[i] = 2. * tp[i-1]; 
	} 

	if (ak == 1.) 
	{
		return 0.; 
	}
 
	for (i = 0; i < ntp; i++)
	if (tp[i] > p) break; 

	pt = tp[i-1]; 
	p1 = p; 
	r = 1.; 

	for (j = 1; j <= i; j++)
	{ 
		if (p1 >= pt)
		{ 
			r = 16. * r; 
			r = r - (int) (r / ak) * ak; 
			p1 = p1 - pt; 
		} 

		pt = 0.5 * pt; 

		if (pt >= 1.)
		{ 
			r = r * r; 
			r = r - (int) (r / ak) * ak; 
		} 
	} 
	
	return r; 
}

extern "C" __declspec(dllexport) double __stdcall ParallelPI(int nDecimals, bool bMultiCore)
{
	double pid, s1, s2, s3, s4; 

	try
	{
		double dFrequency = 0.0;
		__int64 CounterStart = 0;

		 LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);

		dFrequency = double(li.QuadPart)/1000.0;
		QueryPerformanceCounter(&li);
		CounterStart = li.QuadPart;

		if (bMultiCore == true)
		{
			concurrency::parallel_for(0, nDecimals, 16, [&](int i)
			{
				s1 = series(1, i);
				s2 = series(4, i);
				s3 = series(5, i);
				s4 = series(6, i);

				pid = 4. * s1 - 2. * s2 - s3 - s4;
				pid = pid - (int)pid + 1.;
			});
		}
		else
		{
			for (int i = 0; i <= nDecimals; i += 16)
			{
				s1 = series(1, i);
				s2 = series(4, i);
				s3 = series(5, i);
				s4 = series(6, i);

				pid = 4. * s1 - 2. * s2 - s3 - s4;
				pid = pid - (int)pid + 1.;
			}
		}

		QueryPerformanceCounter(&li);

		CString strMilliseconds = L"";
		CString strSeconds = L"";
		CString strMinutes = L"";

		double dMilliseconds = double(li.QuadPart - CounterStart) / dFrequency;

		return dMilliseconds;

		/*int nSeconds = (int)dMilliseconds / 1000;
		int nMinutes = (int)nSeconds / 60;
		int nMilliseconds = (int)dMilliseconds - nSeconds * 1000;

		strMilliseconds.Format(_T("%d"), nMilliseconds);

		CString strResult = L"";
		strResult.Format(_T("%d"), nSeconds);
		strResult.Append(L".");

		if (nMilliseconds < 10)
		{
			strResult.Append(_T("00"));
		}

		if ((nMilliseconds >= 10) && (nMilliseconds < 100))
		{
			strResult.Append(_T("0"));
		}

		strResult.Append(strMilliseconds);

		nSeconds = nSeconds - nMinutes * 60;

		strSeconds.Format(_T("%d"), nSeconds);
		strMinutes.Format(_T("%d"), nMinutes);

		CString strElapsedTime = " ";
		strElapsedTime += strMinutes;
		strElapsedTime += " min ";
		strElapsedTime += strSeconds;
		strElapsedTime += " sec ";
		strElapsedTime += strMilliseconds;
		strElapsedTime += " ms";

		CString strReturnString = L"";
		strReturnString.Append(strResult);
		strReturnString += "|";
		strReturnString.Append(strElapsedTime);

		return (LPCTSTR)strReturnString;*/
	}
	catch(...) 
	{
		return 0;
	}
}

extern "C" __declspec(dllexport) int __stdcall CreatePIXMLFile(int nDecimals, LPCTSTR strVersion, LPCTSTR strResult, LPCTSTR strTime)
{
	if (nDecimals != 80000)
	{
		return -1;
	}

	if (FindWindow(NULL, L"CPU-Z ") == 0)
	{
		return -2;
	}

	CScreenImage m_image;

	try
	{
		m_image.CaptureScreen();
		m_image.Save(L"MultiThreadedPIScreenShot.jpeg", Gdiplus::ImageFormatJPEG);
	}
	catch (...)
	{
		return -3;
	}

	try
	{
		CString strChar = L"";
		CString strFile = L"";
		strImageBase64 = L"";

		FILE * pFile;
		size_t result;
		errno_t err;

		if ((err = fopen_s(&pFile, "MultiThreadedPIScreenShot.jpeg", "rb")) != 0) return -4;

		fseek(pFile, 0, SEEK_END);
		long lSize = ftell(pFile);
		rewind(pFile);

		unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char)*lSize);
		if (data != NULL)
		{
			result = fread(data, 1, lSize, pFile);
			if (result == lSize)
			{
				for (int i = 0; i <= lSize; i++)
				{
					strChar = L"";
					strChar.Format(_T("%c"), data[i]);
					strFile += strChar;
				}
			}
		}

		fclose(pFile);

		aoBase64Encoder encoder;
		encoder.Encode(data, lSize);

		int encodedCopySize = encoder.GetEncodedSize();
		char* encodedCopy = new char[encodedCopySize];
		memcpy(encodedCopy, encoder.GetEncoded(), sizeof(unsigned char)* encodedCopySize);

		for (int i = 0; i <= encodedCopySize; i++)
		{
			strChar = L"";
			strChar.Format(_T("%c"), encodedCopy[i]);
			strImageBase64 += strChar;
		}

		delete[] data;
	}
	catch (...)
	{
		return -7;
	}

	try
	{
		FILE *f;
		errno_t err;

		if ((err = fopen_s(&f, "Shell32CorePIAPI.xml", "w+")) == 0)
		{
			_fputts(L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n", f);
			_fputts(L"<submission xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"  xmlns=\"http://hwbot.org/submit/api\">\n", f);
			_fputts(L"<application>\n", f);
			_fputts(L"<name>", f);
			_fputts(L"MULTI THREADED PI", f);
			_fputts(L"</name>\n", f);
			_fputts(L"<version>", f);
			_fputts(strVersion, f);
			_fputts(L"</version>\n", f);
			_fputts(L"</application>\n", f);
			_fputts(L"<score>\n", f);
			_fputts(L"<points>", f);
			_fputts(strResult, f);
			_fputts(L"</points>\n", f);
			_fputts(L"<subscores>\n", f);
			_fputts(L"<subscore name=\"\">\n", f);
			_fputts(L"</subscore>\n", f);
			_fputts(L"</subscores>\n", f);
			_fputts(L"</score>\n", f);
			_fputts(L"<timestamp>", f);
			_fputts(strTime, f);
			_fputts(L"</timestamp>\n", f);
			_fputts(L"<screenshot contentType=\"image/jpg\">\n", f);
			_fputts(strImageBase64, f);
			_fputts(L"\n</screenshot>\n", f);
			_fputts(L"<verificationUrl></verificationUrl>\n", f);
			_fputts(L"<applicationChecksum></applicationChecksum>\n", f);
			_fputts(L"<hardware>\n", f);
			_fputts(L"<processor>\n", f);
			_fputts(L"<name>", f);
			//_fputts( strCPU, f );
			_fputts(L"</name>\n", f);
			_fputts(L"<amount></amount>\n", f);
			_fputts(L"<idleTemp></idleTemp>\n", f);
			_fputts(L"<loadTemp></loadTemp>\n", f);
			_fputts(L"<coreClock>", f);
			//_fputts( strFrequency, f );
			_fputts(L"</coreClock>\n", f);
			_fputts(L"</processor>\n", f);
			_fputts(L"<videocard>\n", f);
			_fputts(L"<name>", f);
			//_fputts( strGPU, f );
			_fputts(L"</name>\n", f);
			_fputts(L"<vendor></vendor>\n", f);
			_fputts(L"<amount></amount>\n", f);
			_fputts(L"</videocard>\n", f);
			_fputts(L"<motherboard>\n", f);
			_fputts(L"<name></name>\n", f);
			_fputts(L"<vendor></vendor>\n", f);
			_fputts(L"<chipset></chipset>\n", f);
			_fputts(L"</motherboard>\n", f);
			_fputts(L"<memory>\n", f);
			_fputts(L"<vendor></vendor>\n", f);
			_fputts(L"<type></type>\n", f);
			_fputts(L"<rating></rating>\n", f);
			_fputts(L"<totalSize></totalSize>\n", f);
			_fputts(L"</memory>\n", f);
			_fputts(L"<disk>\n", f);
			_fputts(L"<vendor></vendor>\n", f);
			_fputts(L"<amount></amount>\n", f);
			_fputts(L"<diskCapInGB></diskCapInGB>\n", f);
			_fputts(L"<series></series>\n", f);
			_fputts(L"<configuration></configuration>\n", f);
			_fputts(L"</disk>\n", f);
			_fputts(L"</hardware>\n", f);
			_fputts(L"<software>\n", f);
			_fputts(L"<os>\n", f);
			_fputts(L"<family>Windows</family>\n", f);
			_fputts(L"<fullName></fullName>\n", f);
			_fputts(L"</os>\n", f);
			_fputts(L"</software>\n", f);
			_fputts(L"<metadata name=\"\"> </metadata> \n", f);
			_fputts(L"</submission>\n", f);

			fclose(f);
		}
	}
	catch (...)
	{
		return -9;
	}

	try
	{
		TCHAR szDirectory[MAX_PATH] = L"";
		::GetCurrentDirectory(sizeof(szDirectory)-1, szDirectory);

		CString strPath = L"";
		strPath.Append(szDirectory);
		strPath.Append(L"\\Shell32CorePIAPI.xml");

		ShellExecuteW(NULL, L"open", L"openssl.exe", L"aes-128-cbc -K 4228768326CC48357B8734BD061C52DF -iv C7A5EA40CA9465F9175FE490902C43CD -in Shell32CorePIAPI.xml -out MultiThreadedPISubmission.xml", NULL, SW_HIDE);

		return 1;
	}
	catch (...) { }

	return 0;
}

extern "C" __declspec(dllexport) LPCTSTR __stdcall GetCPU()
{
	try
	{
		int CPUInfo[4] = { -1 };
		__cpuid(CPUInfo, 0x80000000);
		unsigned int nExIds = CPUInfo[0];

		char CPUBrandString[0x40] = { 0 };
		for (unsigned int i = 0x80000000; i <= nExIds; ++i)
		{
			__cpuid(CPUInfo, i);

			if (i == 0x80000002)
			{
				memcpy(CPUBrandString,
					CPUInfo,
					sizeof(CPUInfo));
			}
			else if (i == 0x80000003)
			{
				memcpy(CPUBrandString + 16,
					CPUInfo,
					sizeof(CPUInfo));
			}
			else if (i == 0x80000004)
			{
				memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
			}
		}

		std::string strCPUFormatat = CPUBrandString;
		strCPUFormatat.erase(strCPUFormatat.begin(), std::find_if(strCPUFormatat.begin(), strCPUFormatat.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));

		CString strCPU = L" ";
		strCPU += strCPUFormatat.c_str();
		
		return (LPCTSTR)strCPU;
	}
	catch (...) {}

	return _T("");
}

extern "C" __declspec(dllexport) float __stdcall GetCPUFrequency()
{
	try
	{
		typedef struct _PROCESSOR_POWER_INFORMATION
		{
			ULONG  Number;
			ULONG  MaxMhz;
			ULONG  CurrentMhz;
			ULONG  MhzLimit;
			ULONG  MaxIdleState;
			ULONG  CurrentIdleState;
		}
		PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;

#pragma comment(lib, "Powrprof.lib")

		// get the number or processors 
		SYSTEM_INFO si = { 0 };
		::GetSystemInfo(&si);

		// allocate buffer to get info for each processor
		const int size = si.dwNumberOfProcessors * sizeof(PROCESSOR_POWER_INFORMATION);
		LPBYTE pBuffer = new BYTE[size];

		NTSTATUS status = ::CallNtPowerInformation(ProcessorInformation, NULL, 0, pBuffer, size);
		try
		{
			// list each processor frequency 
			PPROCESSOR_POWER_INFORMATION ppi = (PPROCESSOR_POWER_INFORMATION)pBuffer;
			for (DWORD nIndex = 0; nIndex < si.dwNumberOfProcessors; nIndex++)
			{
				return ppi->CurrentMhz;

				ppi++;
			}
		}
		catch (...)
		{
			return 0;
		}

		delete[]pBuffer;
	}
	catch (...)
	{
		return 0;
	}
}