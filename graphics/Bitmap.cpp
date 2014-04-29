#define DLL_EXPORTS
#include "Bitmap.h"
#include <stdio.h>

NAMESPACE_PH0LY_BEGIN(graphics)

Bitmap::Bitmap() : m_dwWidth(0), m_dwHeight(0), m_hBitmap(NULL)
{

}

Bitmap::Bitmap(DWORD dwWidth, DWORD dwHeight) : m_dwWidth(dwWidth), m_dwHeight(dwHeight), m_hBitmap(NULL)
{

}

Bitmap::Bitmap(const BYTE * pBuffer, DWORD dwSize) 
{
	Decode(pBuffer, dwSize);
}

Bitmap::Bitmap(HBITMAP hBitmap)
{
	FromHBITMAP(hBitmap);
}

Bitmap::Bitmap(const Bitmap& rhs)
{
	if (this == &rhs)
		return;
	FromHBITMAP(rhs.m_hBitmap);
}

const Bitmap& Bitmap::operator = (const Bitmap& rhs)
{
	if (this == &rhs)
		return *this;
	FromHBITMAP(rhs.m_hBitmap);
	return *this;
}

Bitmap::~Bitmap()
{
	if (m_hBitmap)
		Destroy();
}

void Bitmap::Destroy()
{
	DeleteObject(m_hBitmap);
	m_hBitmap = NULL;
}

void Bitmap::FromHBITMAP(HBITMAP hBitmap)
{
	if (m_hBitmap)
		Destroy();
	BITMAP bm;
	GetObjectA(hBitmap, sizeof(BITMAP), &bm);
	m_dwWidth = bm.bmWidth;
	m_dwHeight = bm.bmHeight;

	char * pBuf = new char[bm.bmWidthBytes*bm.bmHeight];
	GetBitmapBits(hBitmap, bm.bmWidthBytes*bm.bmHeight, pBuf);

	HDC hdc = GetDC(NULL);
	m_hBitmap = CreateCompatibleBitmap(hdc, m_dwWidth, m_dwHeight);
	SetBitmapBits(m_hBitmap, bm.bmWidthBytes*bm.bmHeight, pBuf);
	delete [] pBuf;

	ReleaseDC(NULL, hdc);
}

HBITMAP Bitmap::ToHBITMAP() const
{
	return m_hBitmap;
}

Bitmap::operator HBITMAP()
{
	return m_hBitmap;
}

void Bitmap::Blt(HDC hdc, DWORD dwLeft /* = 0 */, DWORD dwTop /* = 0 */)
{
	HDC hDC = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hDC);
	SelectObject(hdcMem, m_hBitmap);
	BitBlt(hdc, dwLeft, dwTop, m_dwWidth, m_dwHeight, hdcMem, 0, 0, SRCCOPY);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hDC);
}

void Bitmap::StretchBlt(HDC hdc, DWORD dwWidth, DWORD dwHeight, DWORD dwLeft /* = 0 */, DWORD dwTop /* = 0 */)
{
	HDC hDC = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hDC);
	SelectObject(hdcMem, m_hBitmap);
	::StretchBlt(hdc, 0, 0, m_dwWidth, m_dwHeight, hdcMem, 0, 0, dwWidth, dwHeight, SRCCOPY);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hDC);
}

void Bitmap::Crop(DWORD dwLeft, DWORD dwTop, DWORD dwRight, DWORD dwBottom)
{
	HDC hDC = GetDC(NULL);
	HDC hdcSrc = CreateCompatibleDC(hDC);
	HDC hdcTarget = CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, dwRight-dwLeft, dwBottom-dwTop);
	m_dwWidth = dwRight-dwLeft;
	m_dwHeight = dwBottom - dwTop;
	SelectObject(hdcSrc, m_hBitmap);
	SelectObject(hdcTarget, hBitmap);
	::StretchBlt(hdcTarget, 0, 0, m_dwWidth, m_dwHeight, hdcSrc, dwLeft, dwTop, dwRight-dwLeft, dwBottom-dwTop, SRCCOPY);
	DeleteObject(m_hBitmap);
	m_hBitmap = hBitmap;
	DeleteDC(hdcSrc);
	DeleteDC(hdcTarget);
	ReleaseDC(NULL, hDC);
}

void Bitmap::Copy(const Bitmap& rhs)
{
	FromHBITMAP(rhs.m_hBitmap);
}

bool Bitmap::Encode(unsigned char *& pOutBuffer, int& count)
{
	BITMAP bm;
	GetObjectA(m_hBitmap, sizeof(BITMAP), &bm);

	pOutBuffer = new unsigned char[sizeof(BITMAPINFOHEADER)+bm.bmWidthBytes*bm.bmHeight];
	PBITMAPINFOHEADER pInfoHeader = reinterpret_cast<PBITMAPINFOHEADER>(pOutBuffer);
	pInfoHeader->biWidth = m_dwWidth;
	pInfoHeader->biHeight = m_dwHeight;
	pInfoHeader->biBitCount = 24;
	GetBitmapBits(m_hBitmap, bm.bmWidthBytes*bm.bmHeight, (char*)pOutBuffer+sizeof(BITMAPINFOHEADER));
	count = bm.bmWidthBytes * bm.bmHeight+sizeof(BITMAPINFOHEADER);

	return true;
}

bool Bitmap::Decode(const unsigned char * pBuffer, int count)
{
	if (m_hBitmap)
		Destroy();
	HDC hdc = GetDC(NULL);

	PBITMAPINFOHEADER pInfoHeader = (PBITMAPINFOHEADER)(pBuffer);
	m_dwWidth = pInfoHeader->biWidth;
	m_dwHeight = pInfoHeader->biHeight;
	m_hBitmap = CreateCompatibleBitmap(hdc, m_dwWidth, m_dwHeight);
	SetBitmapBits(m_hBitmap, count, (char*)pBuffer+sizeof(BITMAPINFOHEADER));
	ReleaseDC(NULL, hdc);

	return true;
}

void Bitmap::FreeMemory(void * pBuffer)
{
	if (pBuffer)
		delete [] pBuffer;
}

void Bitmap::Save(const char* path)
{
	// Convert To DIB
	HDC hdc = GetDC(NULL);
	DWORD dwRowLength = (m_dwWidth*24+31) / 32 * 4;
	BITMAPINFOHEADER bitmapInfoHeader = {0};
	bitmapInfoHeader.biWidth = m_dwWidth;
	bitmapInfoHeader.biHeight = m_dwHeight;
	bitmapInfoHeader.biBitCount = 24;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biSizeImage = dwRowLength * m_dwHeight;

	void * pBuffer = NULL;
	HDC hdcTarget = CreateCompatibleDC(hdc);
	HDC hdcSrc = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bitmapInfoHeader, DIB_RGB_COLORS, &pBuffer, NULL, 0);
	SelectObject(hdcTarget, hBitmap);
	SelectObject(hdcSrc, m_hBitmap);
	BitBlt(hdcTarget, 0, 0, m_dwWidth, m_dwHeight, hdcSrc, 0, 0, SRCCOPY);
	BITMAPFILEHEADER bitmapFileHeader = {0};
	bitmapFileHeader.bfType = 0x4d42;
	bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bitmapInfoHeader.biSizeImage;
	bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	FILE * fp = NULL;
	fopen_s(&fp, path, "wb");
	if (fp == NULL) {
		DeleteDC(hdcTarget);
		DeleteDC(hdcSrc);
		DeleteObject(hBitmap);
		ReleaseDC(NULL, hdc);
		return;
	}
	fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(pBuffer, bitmapInfoHeader.biSizeImage, 1, fp);

	fclose(fp);
	//delete [] pBuffer;
	DeleteDC(hdcTarget);
	DeleteDC(hdcSrc);
	DeleteObject(hBitmap);
	ReleaseDC(NULL, hdc);
}

void Bitmap::Load(const char* path)
{
	if (m_hBitmap)
		Destroy();

	HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (INVALID_HANDLE_VALUE == hFile) {
		return;
	}
	DWORD dwSize = GetFileSize(hFile, NULL);
	char * pBuffer = new char[dwSize];
	DWORD dwBytesRead = 0;
	if (!ReadFile(hFile, pBuffer, dwSize, &dwBytesRead, NULL)) {
		CloseHandle(hFile);
		delete [] pBuffer;
		return;
	}
	CloseHandle(hFile);
	BITMAPFILEHEADER * pBitmapFileHeader = reinterpret_cast<PBITMAPFILEHEADER>(pBuffer);
	if (pBitmapFileHeader->bfType != 0x4d42 || pBitmapFileHeader->bfSize != dwSize) {
		delete [] pBuffer;
		return;
	}
	HDC hdc = GetDC(NULL);
	HBITMAP hBitmap = CreateDIBitmap(hdc, (PBITMAPINFOHEADER)(pBuffer+sizeof(BITMAPFILEHEADER)), CBM_INIT, (pBuffer+pBitmapFileHeader->bfOffBits), (BITMAPINFO*)(pBitmapFileHeader+1), DIB_RGB_COLORS);
	FromHBITMAP(hBitmap);
	delete [] pBuffer;

	ReleaseDC(NULL, hdc);
}

NAMESPACE_PH0LY_END