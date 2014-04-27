
#pragma once
#include "../pub/cmn.h"

NAMESPACE_PH0LY_BEGIN(graphics)
/**
 * class Bitmap
 * Copyright (c) ph0ly 2014.04. All rights reserved.
 * notes: it suports no RGB Quad Bitmap better.
 * Licensed under the MIT.
 */
class PH0LY Bitmap 
{
public:
	Bitmap();
	Bitmap(const BYTE * pBuffer, DWORD dwSize);
	Bitmap(DWORD dwWidth, DWORD dwHeight);
	Bitmap(HBITMAP hBitmap);
	Bitmap(const Bitmap& rhs);
	const Bitmap& operator = (const Bitmap& rhs);
	virtual ~Bitmap();

	void FromHBITMAP(HBITMAP hBitmap);
	HBITMAP ToHBITMAP() const;

	void Blt(HDC hdc, DWORD dwLeft = 0, DWORD dwTop = 0);
	void StretchBlt(HDC hdc, DWORD dwWidth, DWORD dwHeight, DWORD dwLeft = 0, DWORD dwTop = 0);

	void Crop(DWORD dwLeft, DWORD dwTop, DWORD dwRight, DWORD dwBottom);

	void Copy(const Bitmap& rhs);
	
	bool Encode(unsigned char *& pOutBuffer, int& count);
	bool Decode(const unsigned char * pBuffer, int count);

	void FreeMemory(void * pBuffer);

	DWORD GetWidth() const { return m_dwWidth; }
	DWORD GetHeight() const { return m_dwHeight; }

	void SetWidth(DWORD dwWidth) { m_dwWidth = dwWidth; }
	void SetHeight(DWORD dwHeight) { m_dwHeight = dwHeight; }

	void Save(const char* path);
	void Load(const char* path);
	
	operator HBITMAP ();

private:
	void Destroy();

	DWORD m_dwWidth;
	DWORD m_dwHeight;
	HBITMAP m_hBitmap;

};

NAMESPACE_PH0LY_END