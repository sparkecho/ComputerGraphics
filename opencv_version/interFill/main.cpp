#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

#define LEN 600

vector<Point> vertex;
Mat img;
Point cursor;
bool stop = false;
int gpattern[16][36] =
{
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    { 0,0,0,1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0},
    { 0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0},
    { 0,1,1,0,0,0,1,1,0,0,0,0,1,1,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1,1,1,1,0,0},
    { 0,1,1,0,0,0,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,1,0,0},
    { 0,1,1,0,0,0,1,1,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,1,1,0,0},
    { 0,1,1,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,1,1,1,0,0,0,1,1,0,0,1,1,0,0},
    { 0,1,1,0,0,0,1,1,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0},
    { 0,1,1,0,0,0,1,1,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0},
    { 0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0},
    { 0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};


void onMouse(int event, int x, int y, int flag, void *param);
void EdgeMarkFill(Mat img, vector<Point> vertex, Scalar color);
void patternFill(Mat img, int x, int y, Scalar color);

int main(int, char**)
{
    img = Mat(400,600,CV_8UC3,Scalar(255,255,255));

    namedWindow("Polygon");
    imshow("Polygon", img);
    setMouseCallback("Polygon", onMouse, NULL);
    waitKey(0);

    EdgeMarkFill(img, vertex, Scalar(0,0,255));
    for(size_t i = 1; i < vertex.size(); i++)
        line(img, vertex[i], vertex[i-1], Scalar(0,0,0));
    imshow("Polygon", img);
    waitKey(0);

    return 0;
}

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
            // line(img, Point(ixs,ys+1), Point(ixs,ys+1), Scalar(0,0,255));
        }
        imshow("Polygon", img);
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
            // if(inside)
            //     line(img, Point(x,y), Point(x,y), color);
            if(inside)
                patternFill(img, x, y, color);
        }
    }

    // for(int y = y1; y <= y2; y++)
    // {
    //     for(int x = x1; x <= x1+140; x++)
    //         if(mask[y][x] == 1)
    //             cout << "1";
    //         else
    //             cout << "0";
    //     cout << endl;
    // }
}


void onMouse(int event, int x, int y, int flags, void *param)
{
    int end;
    switch (event)
    {
        //按下鼠标左键
        case EVENT_LBUTTONDOWN:
            if(stop) break;
            end = vertex.size() - 1;
            if(end >= 0)
            {
                line(img, vertex.at(end), Point(x,y), Scalar(0,0,0));
                imshow("Polygon", img);
            }
            vertex.push_back( Point(x,y) );
            break;

        case EVENT_MBUTTONDOWN:
            end = vertex.size() - 1;
            if(end >= 0)
            {
                cursor = Point(x,y);
                line(img, vertex.at(end), cursor, Scalar(255,255,255));
                stop = true;

                vertex.push_back(vertex.at(0));
                line(img, vertex.at(0), vertex.at(end), Scalar(0,0,0));
                imshow("Polygon", img);
            }
            break;

        case EVENT_MOUSEMOVE:
            if(stop) break;
            end = vertex.size() - 1;
            if(end >= 0)
            {
                line(img, vertex.at(end), cursor, Scalar(255,255,255));
                cursor = Point(x,y);
                line(img, vertex.at(end), cursor, Scalar(0,0,255));
                imshow("Polygon", img);
            }
            break;
    }
}


void patternFill(Mat img, int x, int y, Scalar color)
{
    int row = 16;
    int col = 36;
    if(gpattern[y%row][x%col] == 1)
        line(img, Point(x,y), Point(x,y), color);
    else
        line(img, Point(x,y), Point(x,y), Scalar(255,255,255));
}
