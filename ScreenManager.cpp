#include "ScreenManager.h"
#include "Config.h"
#include "Global.h"
#include "zip.h"

using namespace std;

bool ScreenManager::Shot()
{
	HWND hDesk = GetDesktopWindow();
	HDC hDC = GetDC(hDesk);
	HDC comDC = CreateCompatibleDC(hDC);
	// int width = GetDeviceCaps(hDC, HORZRES);
	// int height = GetDeviceCaps(hDC, VERTRES);
	int width = GetSystemMetrics(SM_CXSCREEN) * 1.25;
	int height = GetSystemMetrics(SM_CYSCREEN) * 1.25;
	/*int width = 1920;
	int height = 1080;*/
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, width, height);
	SelectObject(comDC, hBitmap);
	if (!BitBlt(comDC, 0, 0, width, height, hDC, 0, 0, SRCCOPY))
	{
		return false;
	}
	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = width;
	bmiHeader.biHeight = height;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 16;
	bmiHeader.biCompression = BI_RGB;
	bmiHeader.biSizeImage = 0;
	bmiHeader.biXPelsPerMeter = 0;
	bmiHeader.biYPelsPerMeter = 0;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;
	DWORD dwBmpSize = ((width * bmiHeader.biBitCount + 31) / 32) * 4 * height;
	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	BITMAPFILEHEADER bmfHeader;
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42;
	char* lpbitmap = new char[dwBmpSize];
	ZeroMemory(lpbitmap, dwBmpSize);
	GetDIBits(comDC, hBitmap, 0, (UINT)height, lpbitmap, (BITMAPINFO*)&bmiHeader, DIB_RGB_COLORS);

	ofstream shotBMP;
	shotBMP.open("G:\\shot.bmp", ios::binary);
	shotBMP.write((char*)&bmfHeader, sizeof(BITMAPFILEHEADER));
	shotBMP.write((char*)&bmiHeader, sizeof(BITMAPINFOHEADER));
	shotBMP.write(lpbitmap, dwBmpSize);
	shotBMP.close();
	cout << width << " * " << height << endl;
	return true;
}

void ScreenManager::MakeZip()
{
	HZIP hz = CreateZip("G:\\test.zip", 0);
	ZipAdd(hz, "test.bmp", "G:\\shot.bmp");
	CloseZip(hz);
}
