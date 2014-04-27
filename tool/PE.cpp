#define DLL_EXPORTS
#include "PE.h"
#include "Toolhelp.h"
#include <iostream>
#include <fstream>
#include <ImageHlp.h>
using namespace std;
#pragma comment(lib, "ImageHlp")
NAMESPACE_PH0LY_BEGIN(tool)

PE::PE(void)
{
	m_bAsImage = false;
	m_hProcess = NULL;
	m_pvBase = NULL;
	m_hFileMapping = NULL;
	m_hFile = NULL;
	m_dwVMRgn = 0;
}

PE::~PE(void)
{
	UnMap();
}

PE::PE(const char * pszFile)
{
	memcpy_s(m_strFile, MAX_PATH, pszFile, strlen(pszFile)+1);
	//m_strFile = strFile;
	m_bAsImage = false;
	m_hProcess = NULL;
	m_pvBase = NULL;
	m_hFileMapping = NULL;
	m_hFile = NULL;
	m_dwVMRgn = 0;
	Map();
}

bool PE::UnMap()
{
	if (m_pvBase == NULL)
		return false;
	bool bValid = false;
	// 内存映射方式，需要释放堆内存
	if (m_bAsImage)
	{
		delete [] m_pvBase;
		bValid = true;
	}
	else
	{
		bValid = UnMapViewOfFile(m_pvBase);
	}
	m_pvBase = NULL;
	CloseHandle(m_hFile);
	CloseHandle(m_hFileMapping);
	m_hFile = NULL;
	m_hFileMapping = NULL;
	return bValid;
}

PVOID PE::MapViewOfFile(std::string strFile, DWORD dwMapAccess)
{
	HANDLE hFile = CreateFileA(m_strFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return NULL;
	}
	HANDLE hFileMapping = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, NULL, NULL, NULL);
	if (hFileMapping == NULL)
	{
		CloseHandle(hFile);
		return NULL;
	}
	PVOID pBase = ::MapViewOfFile(hFileMapping, dwMapAccess, NULL, NULL, NULL);
	if (pBase == NULL)
	{
		CloseHandle(hFileMapping);
		CloseHandle(hFile);
		return NULL;
	}
	m_hFile = hFile;
	m_hFileMapping = hFileMapping;
	return pBase;
}

int PE::GetSections()
{
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)Locate(NT_HEADER);
	return pNtHeader->FileHeader.NumberOfSections;
}

bool PE::GetSections( std::vector<std::string>& vOutputs )
{
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)Locate(SECTIONS_HEADER);
	int nNumber = GetSections();
	for (int i=0; i<nNumber; i++, pSectionHeader++)
	{
		vOutputs.push_back((PCHAR)pSectionHeader->Name);
	}

	return true;
}

bool PE::UnMapViewOfFile(PVOID pv)
{
	return (UnmapViewOfFile(pv) == TRUE);
}

PVOID PE::Locate(PVOID pvBase, PEType pe)
{
	PBYTE pBase = (PBYTE)m_pvBase;
	PVOID pAddr = pBase;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	ULONG ulSize;
	switch (pe)
	{
	case DOS_HEADER:
		break;
	case NT_HEADER:
		pDosHeader = (PIMAGE_DOS_HEADER)pBase;
		pAddr = (PIMAGE_NT_HEADERS)(pBase+pDosHeader->e_lfanew);
		break;
	case SECTIONS_HEADER:
		pDosHeader = (PIMAGE_DOS_HEADER)pBase;
		pNTHeader = (PIMAGE_NT_HEADERS)(pBase+pDosHeader->e_lfanew);
		pAddr = (PVOID)((PBYTE)pNTHeader+sizeof(pNTHeader->Signature) + sizeof(pNTHeader->FileHeader)+pNTHeader->FileHeader.SizeOfOptionalHeader);
		//pAddr = pBase+sizeof(IMAGE_DOS_HEADER)+sizeof(IMAGE_NT_HEADERS);
		break;
	case IMPORT_TABLE:
		pAddr = ImageDirectoryEntryToData(m_pvBase, m_bAsImage, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
		break;
	case EXPORT_TABLE:
		pAddr = ImageDirectoryEntryToData(m_pvBase, m_bAsImage, IMAGE_DIRECTORY_ENTRY_EXPORT, &ulSize);
		break;
	default:
		pAddr = NULL;
	}

	return pAddr;
}

PVOID PE::Locate(PEType pe)
{
	return Locate(m_pvBase, pe);
}

PE::operator PVOID()
{
	return m_pvBase;
}

bool PE::Map()
{
	m_pvBase = MapViewOfFile(m_strFile);
	return m_pvBase != NULL;
}

DWORD PE::GetVMRgn(HANDLE hProcess, PVOID pvBase)
{
	MEMORY_BASIC_INFORMATION mbi;
	bool bOk = (VirtualQueryEx(hProcess, pvBase, &mbi, sizeof(mbi)) == sizeof(mbi));
	DWORD dwRgn = mbi.RegionSize;
	DWORD dwMem = mbi.RegionSize;
	PVOID lpBaseAddr = mbi.AllocationBase;
	PVOID lpNextAddr = pvBase;
	while (bOk)
	{
		bOk = (VirtualQueryEx(hProcess, lpNextAddr, &mbi, sizeof(mbi)) == sizeof(mbi));
		if (mbi.AllocationBase != lpBaseAddr)
			break;
		dwMem += mbi.RegionSize;
		lpNextAddr = (PBYTE)lpNextAddr+mbi.RegionSize;
	}
	dwMem = dwMem > 0 ? dwMem - dwRgn : 0;
	return dwMem;
}

bool PE::Load(const char * pszFile)
{
	memcpy_s(m_strFile, MAX_PATH, pszFile, strlen(pszFile)+1);
	m_bAsImage = false;
	return Map();
}

bool PE::Load(DWORD dwProcessId)
{
	Toolhelp thlp(TH32CS_SNAPMODULE, dwProcessId);
	MODULEENTRY32 me32 = {sizeof(MODULEENTRY32)};
	bool bOk = false;
	m_pvBase = NULL;
	while (thlp.Next(me32))
	{
		if (strstr(me32.szModule, ".exe") != NULL)
		{
			m_pvBase = me32.hModule;
			m_bAsImage = true;
			bOk = true;
			break;
		}
	}
	if (!bOk)
		return false;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, TRUE, dwProcessId);
	if (hProcess == NULL)
	{
		return false;
	}
	DWORD dwMemCount = GetVMRgn(hProcess, me32.modBaseAddr);
	m_dwVMRgn = dwMemCount;
	m_pvBase = new char[dwMemCount];
	DWORD dwSize = 0;
	// 内存读取失败，返回错误
	if (!ReadProcessMemory(hProcess, me32.modBaseAddr, m_pvBase, dwMemCount, &dwSize))
	{
		CloseHandle(hProcess);
		delete[] m_pvBase;
		m_pvBase = NULL;
		return false;
	}
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)m_pvBase;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		CloseHandle(hProcess);
		delete[] m_pvBase;
		m_pvBase = NULL;
		return false;
	}
	CloseHandle(hProcess);
	// 不是PE文件
	if (!IsPE())
	{
		return false;
	}
// 	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pszBuf+pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
// 	for (; pImportDesc->Name; pImportDesc++)
// 	{
// 		PCHAR pszName = (PCHAR)(pszBuf+pImportDesc->Name);
// 		//cout << "\t" << pszName << endl;
// 		//ofs << "\t" << pszName << endl;
// 	}
	
	return bOk;
}

bool PE::IsPE()
{
	if (m_pvBase == NULL)
	{
		return false;
	}
	PBYTE pBase = (PBYTE)m_pvBase;
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pBase;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return false;
	}
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(pBase+pDosHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		return false;
	}

	return true;
}

PVOID PE::GetEntryPoint()
{	
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)Locate(NT_HEADER);;
	return (PVOID)pNTHeader->OptionalHeader.AddressOfEntryPoint;
}

PVOID PE::RVAToRA(PVOID pvRVA)
{
	PIMAGE_SECTION_HEADER pvSectionBase = (PIMAGE_SECTION_HEADER)Locate(SECTIONS_HEADER);
	int nSections = GetSections();
	for (int i=0; i<nSections; i++, pvSectionBase++)
	{
		if (pvSectionBase->VirtualAddress <= (DWORD)pvRVA && 
			pvSectionBase->VirtualAddress+pvSectionBase->SizeOfRawData >= (DWORD)pvRVA)
		{
			return (PVOID)(pvSectionBase->PointerToRawData+((DWORD)pvRVA-pvSectionBase->VirtualAddress));
		}
	}
	return NULL;
}

PVOID PE::RVAToRA(PVOID pvBase, PVOID pvRVA)
{
	m_pvBase = pvBase;
	return RVAToRA(pvRVA);
}

DWORD PE::RVAToRA(DWORD pvRVA)
{
	return (DWORD)RVAToRA((PVOID)pvRVA);
}

bool PE::GetImportFunctions(std::vector<std::string>& vOutputFoos)
{
	if (!IsPE())
	{
		return false;
	}
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)Locate(IMPORT_TABLE);
	for (; pImportDesc->Name; pImportDesc++)
	{
		PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((PBYTE)m_pvBase + (m_bAsImage ? pImportDesc->OriginalFirstThunk : RVAToRA(pImportDesc->OriginalFirstThunk)));
		PSTR pszDllName = (PSTR)((PBYTE)m_pvBase + (m_bAsImage ? pImportDesc->Name : RVAToRA(pImportDesc->Name)));
		for (; pThunk->u1.Function; pThunk++)
		{
			//以名字导入
			if ((pThunk->u1.Function & 0x80000000) == 0)
			{
				PIMAGE_IMPORT_BY_NAME pImortByName = (PIMAGE_IMPORT_BY_NAME)((PBYTE)m_pvBase + (m_bAsImage ? pThunk->u1.Function : RVAToRA(pThunk->u1.Function)));
				PCHAR pszName = (PCHAR)pImortByName->Name;
				vOutputFoos.push_back(pszName);
			}
		}
	}
	return true;
}

bool PE::GetImportFunctions(std::map<std::string, std::vector<std::string>>& vOutputs)
{
	if (!IsPE())
	{
		return false;
	}
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)Locate(IMPORT_TABLE);
	for (; pImportDesc->Name; pImportDesc++)
	{
		PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((PBYTE)m_pvBase + (m_bAsImage ? pImportDesc->OriginalFirstThunk : RVAToRA(pImportDesc->OriginalFirstThunk)));
		PSTR pszDllName = (PSTR)((PBYTE)m_pvBase + (m_bAsImage ? pImportDesc->Name : RVAToRA(pImportDesc->Name)));
		std::vector<std::string> vDlls;
		for (; pThunk->u1.Function; pThunk++)
		{
			//以名字导入
			if ((pThunk->u1.Function & 0x80000000) == 0)
			{
				PIMAGE_IMPORT_BY_NAME pImortByName = (PIMAGE_IMPORT_BY_NAME)((PBYTE)m_pvBase + (m_bAsImage ? pThunk->u1.Function : RVAToRA(pThunk->u1.Function)));
				PCHAR pszName = (PCHAR)pImortByName->Name;
				vDlls.push_back(pszName);
			}
		}
		vOutputs[pszDllName] = vDlls;
	}
	return true;
}

bool PE::GetExportFunctions( std::vector<std::string>& vOutputs )
{
	if (!IsPE())
	{
		return false;
	}
	PIMAGE_EXPORT_DIRECTORY pExportDirec = (PIMAGE_EXPORT_DIRECTORY)Locate(EXPORT_TABLE);
	PCHAR* pName = (PCHAR*)((PBYTE)m_pvBase+(m_bAsImage ? pExportDirec->AddressOfNames : RVAToRA(pExportDirec->AddressOfNames)));
	DWORD dwCount = pExportDirec->NumberOfFunctions;
	for (size_t i=0; i<dwCount; i++)
	{
		vOutputs.push_back((PCHAR)m_pvBase+(DWORD)(m_bAsImage ? pName[i] : RVAToRA(pName[i])));
	}

	return true;
}

bool PE::Dump( const char * pszFilePath )
{
	// 不是进程，不合法
	if (!m_bAsImage || m_pvBase == NULL)
	{
		return false;
	}
	ofstream ofs(pszFilePath, ios::binary);
	if (!ofs.good())
	{
		return false;
	}
	ofs.write((PCHAR)m_pvBase, m_dwVMRgn);
	return true;
}

NAMESPACE_PH0LY_END