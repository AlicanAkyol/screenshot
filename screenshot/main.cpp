
#include <windows.h> 
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <time.h>

using namespace std;

BOOL SaveBitmap(HDC hDC, HBITMAP hBitmap,int count);
char *filename = (char*)malloc(_MAX_PATH);
char *nm = (char*)malloc(_MAX_PATH);
void x(int count);

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	int count = 1;
	while (true)
	{
		x(count);//main func
		Sleep(5000);
		count = count + 1;
	}	
	return 1;
}

void x(int count)
{
	//std::cin.ignore();    //Press enter and the end
	HDC hDC;
	hDC = GetDC(NULL); //Get full screen handle
	HDC hMemDC = CreateCompatibleDC(hDC);
	RECT r;
	GetWindowRect(GetActiveWindow(), &r); 
	SIZE size;
	
	size.cx = GetSystemMetrics(SM_CXSCREEN);
	size.cy = GetSystemMetrics(SM_CYSCREEN);
	
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, size.cx, size.cy);// full screen compatible bitmap handle
	if (hBitmap)
	{
		HBITMAP hOld = (HBITMAP)SelectObject(hMemDC, hBitmap);
		BitBlt(hMemDC, 0, 0, size.cx, size.cy, hDC, 0, 0, SRCCOPY); // hDC copy to hMemDC   //Transformation bitmap, graphical oper.
		SelectObject(hMemDC, hOld);
		bool flag = SaveBitmap(hMemDC, hBitmap,count);
		DeleteDC(hMemDC);
		ReleaseDC(NULL, hDC);	
		//DrawBitmap(hMemDC, 0, 0, hOld, SRCCOPY);
		//pbox->Image = Image::FromHbitmap(hBitmap);
		DeleteObject(hBitmap);		
	}
	
}

BOOL SaveBitmap(HDC hDC, HBITMAP hBitmap,int count)
{
	DWORD error;
	FILE * fp = NULL;	
	strcpy(filename, "image-");
	_itoa(count, nm,10);
	strcat(filename, nm);
	strcat(filename,".bmp");
	fp = fopen(filename, "wb");
	if (fp == NULL)
	{		
		return false;
	}
	BITMAP Bm;
	BITMAPINFO BitInfo;
	ZeroMemory(&BitInfo, sizeof(BITMAPINFO));
	BitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BitInfo.bmiHeader.biBitCount = 0;
	if (!::GetDIBits(hDC, hBitmap, 0, 0, NULL, &BitInfo, DIB_RGB_COLORS))//get compatible bitmap and copy to bufer
	{
		return (false);
	}
	Bm.bmHeight = BitInfo.bmiHeader.biHeight;
	Bm.bmWidth = BitInfo.bmiHeader.biWidth;
	
	BITMAPFILEHEADER    BmHdr;

	BmHdr.bfType = 0x4d42;   // 'BM' WINDOWS_BITMAP_SIGNATURE
	BmHdr.bfSize = (((3 * Bm.bmWidth + 3) & ~3) * Bm.bmHeight)
		+ sizeof(BITMAPFILEHEADER)
		+sizeof(BITMAPINFOHEADER);
	BmHdr.bfReserved1 = BmHdr.bfReserved2 = 0;
	BmHdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER)
		+sizeof(BITMAPINFOHEADER);
	


	BitInfo.bmiHeader.biCompression = 0;
	// Writing Bitmap File Header 
	size_t size = fwrite(&BmHdr, sizeof(BITMAPFILEHEADER), 1, fp);

	if (size < 1)
	{
		error = GetLastError();
	}

	size = fwrite(&BitInfo.bmiHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	if (size < 1)
	{
		error = GetLastError();
	}

	BYTE *pData = new BYTE[BitInfo.bmiHeader.biSizeImage + 5];

	if (!::GetDIBits(hDC, hBitmap, 0, Bm.bmHeight,
		pData, &BitInfo, DIB_RGB_COLORS))
		return (false);

	if (pData != NULL)
		fwrite(pData, 1, BitInfo.bmiHeader.biSizeImage, fp);
		
	fclose(fp);
	delete (pData);
	
	return (true);
}