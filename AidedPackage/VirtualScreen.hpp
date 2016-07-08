#ifndef _VIRTUAL_SCREEN_
#define _VIRTUAL_SCREEN_

#include <iostream>
#include <opencv2/opencv>

using namespace std;
using namespace cv;

class VirtualScreen
{
private:
	int width;
	int height;
	int offset;
	string background;
	void drawRows();
	void drawCols();
	void drawGrid();

public:
	VirtualScreen();
	VirtualScreen(int width, int height);
	VirtualScreen(int width, int height, int offset);
	VirtualScreen(int width, int height, string background);
	VirtualScreen(int width, int height, int offset, string background);
	void setPixel(int x, int y, string color);
	void setPixel(int x, int y, int b, int g, int r);
	string getPixel(int x, int y); // 返回 (x, y) 处像素的颜色值
	void showLine(int x1, int y1, int x2, int y2);
	void showLine(int x1, int y1, int x2, int y2, string color);
	void showLine(int x1, int y1, int x2, int y2, int b, int g, int r);
}

#endif
