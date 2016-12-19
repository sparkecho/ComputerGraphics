#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

#define LEN 600

vector<Point> vertex;
Mat img;

void EdgeMarkFill(Mat img, vector<Point> vertex, Scalar color);

int main(int, char**)
{
    img = Mat(400,600,CV_8UC3,Scalar(255,255,255));
    // [224, 92] [131, 210] [309, 249] [363, 147] [224, 92] 
    vertex.push_back(Point(224,92));
    vertex.push_back(Point(131,210));
    vertex.push_back(Point(309,249));
    vertex.push_back(Point(363,147));
    vertex.push_back(Point(224,92));
    namedWindow("Polygon");
    for(size_t i = 1; i < vertex.size(); i++)
    {
        // cout << vertex.at(i) << "\t" << vertex.at(i-1) << endl;
        line(img, vertex.at(i), vertex.at(i-1), Scalar(0,0,0));
    }
    imshow("Polygon", img);
    waitKey(0);

    EdgeMarkFill(img, vertex, Scalar(0,0,255));
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
            if(dxs < 0)
                ixs ++;
            mask[ys][ixs] = !mask[ys][ixs];
            line(img, Point(ixs,ys), Point(ixs,ys), Scalar(0,0,255));
        }
        imshow("Polygon", img);
        waitKey(0);
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
