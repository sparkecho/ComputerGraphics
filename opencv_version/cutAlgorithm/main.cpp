#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

// Global Variables
bool draw;
Mat src;                        // 原始图像
Mat img;                        // 操作图像
Point cursor;                   // 初始坐标
Rect rect;                      // 标记ROI的矩形框
float xl;
float xr;
float yt;
float yb;

vector<Point> polygon;          // 多边形


// Function Prototypes
void onMouse(int event, int x, int y, int flags, void *param);
void motion(char cmd);
void drawPolygon(Mat img, vector<Point> polygon, Scalar color);
void drawInRegion(Mat img, vector<Point> polygon, Scalar color);
// void makeCode(float x, float y, int& c);
// void CohenSutherland(Mat img, Point p0, Point p2);
void SutherlandHodgman(vector<Point> bound, vector<Point> src, vector<Point>& dst);
void withBound(Point bound1, Point bound2, int bindex, vector<Point>& dst);

// Main Function
int main(int /*argc*/, char** /*argv*/)
{
    // src = imread(argv[1]);
    src = Mat(600,600,CV_8UC3,Scalar(255,255,255));
    src.copyTo(img);
    if(src.data == 0)
    {
        cout << "error, the src image is not built!" << endl;
        return -1;
    }
    namedWindow("SignROI");

    polygon.push_back(Point(100,100));
    polygon.push_back(Point(500,500));
    polygon.push_back(Point(100,100));

    setMouseCallback("SignROI", onMouse, NULL);
    drawPolygon(img, polygon, Scalar(0,255,0));
    imshow("SignROI", img);
    char cmd = waitKey(30);
    while(cmd != 'q' && cmd != 'Q' && cmd != 27)
    {
        // setMouseCallback("SignROI", onMouse, NULL);
        if(cmd == 'w' || cmd == 's' || cmd == 'a' || cmd == 'd')
        {
            drawPolygon(img, polygon, Scalar(255,255,255));
            motion(cmd);
            drawPolygon(img, polygon, Scalar(0,255,0));
            rectangle(img, rect, Scalar(0, 0, 255), 1);
            drawInRegion(img, polygon, Scalar(255,0,0));
            imshow("SignROI", img);
        }

        cmd = waitKey(300);
    }
    waitKey();
    return 0;
}


// Function Definitions

void onMouse(int event, int x, int y, int flags, void *param)
{
    img = src.clone();
    switch (event)
    {
        //按下鼠标左键
        case EVENT_LBUTTONDOWN:
            //存放起始坐标
            cursor = Point(x, y);
            //初始化起始矩形框
            rect = Rect(x, y, 0, 0);
            draw = true;
            break;

            //松开鼠标左键
        case EVENT_LBUTTONUP:
            if (rect.height > 0 && rect.width > 0)
            {
                rectangle(img, rect, Scalar(0, 0, 255), 1);
                namedWindow("SignROI");
                imshow("SignROI", img);
                xl = rect.x;
                xr = xl + rect.width;
                yt = rect.y;
                yb = yt + rect.height;
            }
            draw = false;
            break;

            // 复原
        case EVENT_MBUTTONDOWN:
            src.copyTo(img);
            imshow("SignROI", img);
                break;

            //移动光标
        case EVENT_MOUSEMOVE:
            if (draw)
            {
                //将画过矩形框的图像用原图像还原
                src.copyTo(img);
                rectangle(img, rect, Scalar(0, 0, 255), 1);
                imshow("SignROI", img);

                //用MIN得到左上点作为矩形框的起始坐标，如果不加这个，画矩形时只能向一个方向进行
                rect.x = MIN(x, cursor.x);
                rect.y = MIN(y, cursor.y);
                rect.width = abs(cursor.x - x);
                rect.height = abs(cursor.y - y);
                //防止矩形区域超出图像的范围
                rect &= Rect(0, 0, src.cols, src.rows);
            }
            break;
    }
}

// function to move the polygon
void motion(char cmd)
{
    Point p;
    switch(cmd)
    {
        case 'w':
            for(size_t i = 0; i < polygon.size(); i++)
            {
                p = polygon.at(i);
                polygon.at(i) = Point(p.x, p.y-20);
            }
            break;
        case 's':
            for(size_t i = 0; i < polygon.size(); i++)
            {
                p = polygon.at(i);
                polygon.at(i) = Point(p.x, p.y+20);
            }
            break;
        case 'a':
            for(size_t i = 0; i < polygon.size(); i++)
            {
                p = polygon.at(i);
                polygon.at(i) = Point(p.x-20, p.y);
            }
            break;
        case 'd':
            for(size_t i = 0; i < polygon.size(); i++)
            {
                p = polygon.at(i);
                polygon.at(i) = Point(p.x+20, p.y);
            }
            break;
    }
}

void drawPolygon(Mat img, vector<Point> polygon, Scalar color)
{
    for(size_t i = 1; i < polygon.size(); i++)
        line(img, polygon.at(i), polygon.at(i-1), color);
}

void drawInRegion(Mat img, vector<Point> polygon, Scalar color)
{
    vector<Point> bound;
    bound.push_back(Point(rect.x, rect.y));
    bound.push_back(Point(rect.x+rect.width, rect.y));
    bound.push_back(Point(rect.x+rect.width, rect.y+rect.height));
    bound.push_back(Point(rect.x, rect.y+rect.height));
    bound.push_back(Point(rect.x, rect.y));
    vector<Point> dst;
    SutherlandHodgman(bound, polygon, dst);
    for(size_t i = 1; i < dst.size(); i++)
    {
        line(img, dst.at(i-1), dst.at(i), color);
        // cout << dst.at(i-1) << "\t"; // for test
    }
    dst.clear();
    cout << endl;               // for test
}


void SutherlandHodgman(vector<Point> bound, vector<Point> src, vector<Point>& dst)
{
    // Init dst
    for(size_t i = 0; i < src.size(); i++)
        dst.push_back(src.at(i));
    // for test
    // cout << "bound: ";
    // for(size_t i = 1; i < bound.size(); i++)
    //     cout << bound.at(i-1) << "\t";
    // cout << endl;
    // cout << "src:   ";
    // for(size_t i = 1; i < src.size(); i++)
    //     cout << src.at(i-1) << "\t";
    // cout << endl;

    for(size_t i = 1; i < bound.size(); i++)
    {
        withBound(bound.at(i-1), bound.at(i), i, dst);
    }
    // for test
    cout << "outside dst: ";
    if(dst.empty()) return;
    for(size_t i = 1; i < dst.size(); i++)
        cout << dst.at(i-1);
    cout << endl;
}

void withBound(Point bound1, Point bound2, int bindex, vector<Point>& dst)
{
    vector<Point> tmp;
    for(size_t i = 1; i < dst.size(); i++)
    {
        Point p = dst.at(i);
        Point s = dst.at(i-1);
        if(bound1.x == bound2.x)
        {
            if(bindex == 0 || bindex == 3)            // when > bound is outside
            {
                if(s.x <= bound1.x && p.x <= bound1.x) // case 1
                {
                    tmp.push_back(s);
                    tmp.push_back(p);
                }
                else if(s.x > bound1.x && p.x > bound1.x) // case 2
                {
                }
                else if(s.x <= bound1.x && p.x > bound1.x) // case3
                {
                    tmp.push_back(s);
                    float deltax = p.x - s.x;
                    float deltay = p.y - s.y;
                    deltay *= ((bound1.x - s.x) / deltax);
                    deltay += s.y;
                    tmp.push_back(Point(bound1.x, (int)deltay));
                }
                else                // case4
                {
                    float deltax = p.x - s.x;
                    float deltay = p.y - s.y;
                    deltay *= ((bound1.x - s.x) / deltax);
                    deltay += s.y;
                    tmp.push_back(Point(bound1.x, (int)deltay));
                    tmp.push_back(p);
                }
            }
            else                    // when <= bound is outside
            {
                if(s.x > bound1.x && p.x > bound1.x) // case 1
                {
                    tmp.push_back(s);
                    tmp.push_back(p);
                }
                else if(s.x <= bound1.x && p.x <= bound1.x) // case 2
                {
                }
                else if(s.x > bound1.x && p.x <= bound1.x) // case3
                {
                    tmp.push_back(s);
                    float deltax = p.x - s.x;
                    float deltay = p.y - s.y;
                    deltay *= ((bound1.x - s.x) / deltax);
                    deltay += s.y;
                    tmp.push_back(Point(bound1.x, (int)deltay));
                }
                else                // case4
                {
                    float deltax = p.x - s.x;
                    float deltay = p.y - s.y;
                    deltay *= ((bound1.x - s.x) / deltax);
                    deltay += s.y;
                    tmp.push_back(Point(bound1.x, (int)deltay));
                    tmp.push_back(s);
                }
            }
        }
        else if(bound1.y == bound2.y)
        {
            if(bindex == 0 || bindex == 3)            // when > bound is outside
            {
                if(s.x <= bound1.x && p.x <= bound1.x) // case 1
                {
                    tmp.push_back(s);
                    tmp.push_back(p);
                }
                else if(s.x > bound1.x && p.x > bound1.x) // case 2
                {
                }
                else if(s.x <= bound1.x && p.x > bound1.x) // case3
                {
                    tmp.push_back(s);
                    float deltax = p.x - s.x;
                    float deltay = p.y - s.y;
                    deltay *= ((bound1.x - s.x) / deltax);
                    deltay += s.y;
                    tmp.push_back(Point(bound1.x, (int)deltay));
                }
                else                // case4
                {
                    float deltax = p.x - s.x;
                    float deltay = p.y - s.y;
                    deltay *= ((bound1.x - s.x) / deltax);
                    deltay += s.y;
                    tmp.push_back(Point(bound1.x, (int)deltay));
                    tmp.push_back(s);
                }
            }
            else                    // when <= bound is outside
            {
                if(s.y > bound1.y && p.y > bound1.y) // case 1
                {
                    tmp.push_back(s);
                    tmp.push_back(p);
                }
                else if(s.y <= bound1.y && p.y <= bound1.y) // case 2
                {
                }
                else if(s.y > bound1.y && p.y <= bound1.y) // case3
                {
                    tmp.push_back(s);
                    float deltax = p.x - s.x;
                    float deltay = p.y - s.y;
                    deltax *= ((bound1.y - s.y) / deltay);
                    deltax += s.x;
                    tmp.push_back(Point(bound1.x, (int)deltax));
                }
                else                // case4
                {
                    float deltax = p.x - s.x;
                    float deltay = p.y - s.y;
                    deltax *= ((bound1.y - s.y) / deltay);
                    deltax += s.x;
                    tmp.push_back(Point(bound1.x, (int)deltax));
                    tmp.push_back(s);
                }
            }
        }
        else
        {
            cout << "Exception!!!" << endl;
        }
    }
    // for test
    cout << "dst:   ";
    if(tmp.empty())
        return;
    for(size_t i = 0; i < tmp.size(); i++)
        cout << tmp.at(i) << "\t";
    cout << endl;
    
    dst.swap(tmp);
}



/*
void CohenSutherland(Mat img, Point p0, Point p2)
{
    // float xl = region.x;
    // float xr = xl + region.width;
    // float yt = region.y;
    // float yb = yt + region.height;
    float x0 = p0.x;
    float y0 = p0.y;
    float x2 = p2.x;
    float y2 = p2.y;
    int c, c1 = 0, c2 = 0;
    float x, y;
    makeCode(x0, y0, c1);
    makeCode(x2, y2, c2);
    // for test 
    cout << "In CohenSutherland, entering while loop" << endl;
    cout << "c1 = " << c1 << ", c2 = " << c2 << endl;
    while(c1 != 0 || c2 != 0)
    {
        if((c1 & c2) == 1)
            return;
        c = c1;
        if(c == 0)
            c = c2;
        if((c & 1) == 1)
        {
            y = y0 + (y2 - y0) * (xl - x0) / (x2 - x0);
            x = xl;
        }
        else if(c & 2)
        {
            y = y0 + (y2 - y0) * (xr - x0) / (x2 - x0);
            x = xl;
        }
        else if(c & 4)
        {
            x = x0 + (x2 - x0) * (yb - y0) / (y2 - y0);
            y = yb;
        }
        else if(c & 8)
        {
            x = x0 + (x2 - x0) * (yb - y0) / (y2 - y0);
            y = yb;
        }
        if(c == c1)
        {
            x0 = x;
            y0 = y;
            makeCode(x, y, c1);
        }
        else
        {
            x2 = x;
            y2 = y;
            makeCode(x, y, c2);
        }
        // line(img, Point(x0,y0), Point(x2,y2), Scalar(128,0,0));
        // for test
        // cout << Point(x0,y0) << "\t" << Point(x2,y2) << endl; // ;;;;
    }
    line(img, Point(x0,y0), Point(x2,y2), Scalar(128,0,0));
}

void makeCode(float x, float y, int& c)
{
    // for test
    // cout << "In makeCode" << endl;
    // cout << xl << ", " << xr << ", "
    //      << yt << ", " << yb << "\t";
    // cout << "x = " << x << ", y = " << y << endl;

    c = 0;
    if(x < xl)
    {
        c = 1;
    }
    else if(x > xr)
    {
        c = 2;
    }
    if(y < yb)
    {
        c += 4;
    }
    else if(y > yt)
    {
        c += 8;
    }
    // for test
    // cout << "c = " << c << endl;
}
*/
