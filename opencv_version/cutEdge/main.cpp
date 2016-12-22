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
void SutherlandHodgman(vector<Point> bound, vector<Point> src, vector<Point>& dst);
void withBound(Point bound1, Point bound2, int bindex, vector<Point>& dst);
void EdgeMarkFill(Mat img, vector<Point> vertex, Scalar color);

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
    polygon.push_back(Point(150,400));
    polygon.push_back(Point(200,50));
    polygon.push_back(Point(250,400));
    polygon.push_back(Point(300,100));
    polygon.push_back(Point(100,100));

    setMouseCallback("SignROI", onMouse, NULL);
    // drawPolygon(img, polygon, Scalar(0,255,0));
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

        cmd = waitKey(0);
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
    EdgeMarkFill(img, polygon, color);
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

    if(!dst.empty())
    {
        // for(size_t i = 1; i < dst.size(); i++)
        // {
        //     line(img, dst.at(i-1), dst.at(i), color);
        // }
        EdgeMarkFill(img, dst, color);
    }
    
    dst.clear();
    // cout << endl;               // for test
}


void SutherlandHodgman(vector<Point> bound, vector<Point> src, vector<Point>& dst)
{
    // Init dst
    for(size_t i = 0; i < src.size(); i++)
        dst.push_back(src.at(i));

    for(size_t i = 1; i < bound.size(); i++)
    {
        withBound(bound.at(i-1), bound.at(i), i-1, dst);
    }
    // for test
    // cout << "outside dst: ";
    // if(dst.empty()) return;
    // for(size_t i = 0; i < dst.size(); i++)
    //     cout << dst.at(i);
    // cout << endl;
}

void withBound(Point bound1, Point bound2, int bindex, vector<Point>& dst)
{
    vector<Point> tmp;
    Point s, p;
    int x = bound1.x, y = bound1.y;
    if(bindex == 0)             // edge 0
    {
        for(size_t i = 1; i < dst.size(); i++)
        {
            s = dst.at(i-1);
            p = dst.at(i);
            if(s.y > y && p.y > y)
            {
                // tmp.push_back(s);
                tmp.push_back(p);
            }
            else if(s.y <= y && p.y <= y)
            {
            }
            else
            {
                float deltay = p.y - s.y;
                float deltax = p.x - s.x;
                float targetx = s.x + (y - s.y) / deltay * deltax; // +0.5
                targetx += 1.0;
                Point c = Point((int)targetx, y); // cross point
                if(s.y <= y && p.y > y)
                {
                    tmp.push_back(c);
                    tmp.push_back(p);
                }
                else
                {
                    // tmp.push_back(s);
                    tmp.push_back(c);
                }
            }
        }
    }
    else if(bindex == 2)        // edge 2
    {
        for(size_t i = 1; i < dst.size(); i++)
        {
            s = dst.at(i-1);
            p = dst.at(i);
            if(s.y < y && p.y < y)
            {
                // tmp.push_back(s);
                tmp.push_back(p);
            }
            else if(s.y >= y && p.y >= y)
            {
            }
            else
            {
                float deltay = p.y - s.y;
                float deltax = p.x - s.x;
                float targetx = s.x + (y - s.y) / deltay * deltax; // +0.5
                targetx += 1.0;
                Point c = Point((int)targetx, y); // cross point
                if(s.y >= y && p.y < y)
                {
                    tmp.push_back(c);
                    tmp.push_back(p);
                }
                else
                {
                    // tmp.push_back(s);
                    tmp.push_back(c);
                }
            }
        }
    }
    else if(bindex == 3)        // edge 3
    {
        for(size_t i = 1; i < dst.size(); i++)
        {
            s = dst.at(i-1);
            p = dst.at(i);
            if(s.x > x && p.x > x)
            {
                // tmp.push_back(s);
                tmp.push_back(p);
            }
            else if(s.x <= x && p.x <= x)
            {
            }
            else
            {
                float deltay = p.y - s.y;
                float deltax = p.x - s.x;
                float targety = s.y + (x - s.x) / deltax * deltay; // -0.5
                targety -= 1.0;
                Point c = Point(x, targety); // cross point
                if(s.x <= x && p.x > x)
                {
                    tmp.push_back(c);
                    tmp.push_back(p);
                }
                else
                {
                    // tmp.push_back(s);
                    tmp.push_back(c);
                }
            }
        }
    }
    else                        // edge 1
    {
        for(size_t i = 1; i < dst.size(); i++)
        {
            s = dst.at(i-1);
            p = dst.at(i);
            if(s.x < x && p.x < x)
            {
                // tmp.push_back(s);
                tmp.push_back(p);
            }
            else if(s.x >= x && p.x >= x)
            {
            }
            else
            {
                float deltay = p.y - s.y;
                float deltax = p.x - s.x;
                float targety = s.y + (x - s.x) / deltax * deltay - 0.5; // -0.5
                targety -= 1.0;
                Point c = Point(x, targety); // cross point
                if(s.x >= x && p.x < x)
                {
                    tmp.push_back(c);
                    tmp.push_back(p);
                }
                else
                {
                    // tmp.push_back(s);
                    tmp.push_back(c);
                }
            }
        }
    }
    dst.swap(tmp);
    if(!dst.empty())
        dst.push_back(dst.at(0));
}

#define LEN 600
void EdgeMarkFill(Mat img, vector<Point> vertex, Scalar color)
{
    bool mask[LEN][LEN];
    for(int i = 0; i < LEN; i++)
        for(int j = 0; j < LEN; j++)
            mask[i][j] = false;

    Point pi, pj;               // j = i+1
    for(size_t j = 1; j < vertex.size(); j++)
    {
        pi = vertex[j-1];
        pj = vertex[j];
        if(pi.y == pj.y)
            continue;
        int x1 = pi.y < pj.y ? pi.x : pj.x;
        int x2 = pi.y > pj.y ? pi.x : pj.x;
        int y1 = pi.y < pj.y ? pi.y : pj.y;
        int y2 = pi.y > pj.y ? pi.y : pj.y;

        double xs = x1;
        double dxs = (x2 - x1) * 1.0 / (y2 - y1);
        int ixs;
        for(int ys = y1; ys < y2; ys++)
        {
            xs += dxs;
            ixs = int(xs + 0.5);
            mask[ys+1][ixs] = !mask[ys+1][ixs];
        }
    }

    int x1 = vertex.at(0).x, x2 = vertex.at(0).x,
        y1 = vertex.at(0).y, y2 = vertex.at(0).y;
    for(size_t i = 0; i < vertex.size(); i++)
    {
        Point cur = vertex.at(i);
        if(cur.x < x1)
            x1 = cur.x;
        else if(cur.x > x2)
            x2 = cur.x;
        else
            ;
        if(cur.y < y1)
            y1 = cur.y;
        else if(cur.y > y2)
            y2 = cur.y;
        else
            ;
    }

    bool inside;
    for(int y = y1; y <= y2; y++)
    {
        inside = false;
        for(int x = x1; x <= x2; x++)
        {
            if(mask[y][x])
                inside = !inside;
            if(inside)
                line(img, Point(x,y), Point(x,y), color);
        }
    }
}
