#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// Global Variables
Mat img;
vector<Point> tvp;              // Type value point(型值点)
vector<Point> polygon;          // for bezier2
bool hermite = true;            // if hermite tvp mark is done
bool bezier = true;             // if bezier  tvp mark is done
bool bspline = true;            // if bspline tvp mark is done
bool stop = false;              // for bezier2
Point pointer;                  // pointer of mouse

// Function Prototype
void onHermite(int event, int x, int y, int flags, void *param);
void multiHermite(Mat img, vector<Point> tvp);
void drawHermite(Mat img, vector<Point> points, int quantity = 200,
                 float min = 0.0, float max = 1.0, Scalar color = Scalar(0,0,255));
void pvec2mat(vector<Point> pvec, Mat& mat);

void onBezier(int event, int x, int y, int flags, void *param);
void multiBezier(Mat img, vector<Point> tvp);
void drawBezier(Mat img, vector<Point> points, int quantity = 200,
                 float min = 0.0, float max = 1.0, Scalar color = Scalar(0,0,255));

void onBezier2(int event, int x, int y, int flags, void *param);
void multiBezier2(Mat img, vector<Point> tvp);
void drawBezier2(Mat img, vector<Point> points, int quantity = 200,
                 float min = 0.0, float max = 1.0, Scalar color = Scalar(0,0,255));


void onBSpline(int event, int x, int y, int flags, void *param);
void multiBSpline(Mat img, vector<Point> tvp);
void drawBSpline(Mat img, vector<Point> points, int quantity = 200,
                 float min = 0.0, float max = 1.0, Scalar color = Scalar(0,0,255));

// Main Function
int main(int /*argc*/, char** /*argv*/)
{
    img = Mat(600, 600, CV_8UC3, Scalar(255,255,255));
    namedWindow("Curve");
    imshow("Curve", img);
    char cmd = waitKey(0);

    for(;;)
    {
        if(cmd != 'h' && cmd != 'z' && cmd != 'b' && cmd != '2')
        {
            cmd = waitKey(0);
            continue;
        }
        switch(cmd)
        {
            case 'h':
                setMouseCallback("Curve", onHermite, NULL);
                waitKey(0);
                multiHermite(img, tvp);
                break;
            case 'z':
                setMouseCallback("Curve", onBezier, NULL);
                waitKey(0);
                multiBezier(img, tvp);
                break;
            case 'b':
                setMouseCallback("Curve", onBSpline, NULL);
                waitKey(0);
                multiBSpline(img, tvp);
                break;
            case '2':
                setMouseCallback("Curve", onBezier2, NULL);
                waitKey(0);
                multiBezier2(img, tvp);
                break;
        }
        break;
    }

    imshow("Curve", img);
    waitKey(0);

    return 0;
}

// Function Definition
void onHermite(int event, int x, int y, int flags, void *param)
{
    int cursor;
    Scalar color = Scalar(0,255,0);
    switch(event)
    {
        case EVENT_LBUTTONDOWN:
            if(hermite)
            {
                tvp.push_back(Point(x,y));
                cursor = tvp.size();
                circle(img, Point(x,y), 2, color);
                if(cursor % 2 == 0)
                    arrowedLine(img, tvp[cursor-2], tvp[cursor-1], Scalar(0,128,0));
                imshow("Curve", img);
            }
            break;
        case EVENT_MBUTTONDOWN:
            cursor = tvp.size();
            if(cursor % 2 != 0)
            {
                circle(img, tvp[cursor-1], 2, Scalar(255,255,255));
                tvp.pop_back();
                arrowedLine(img, tvp[cursor-1], pointer, Scalar(255,255,255));
                imshow("Curve", img);
            }
            hermite = false;
            break;
        case EVENT_MOUSEMOVE:
            cursor = tvp.size();
            if(cursor % 2 == 1)
            {
                arrowedLine(img, tvp[cursor-1], pointer, Scalar(255,255,255));
                pointer = Point(x,y);
                arrowedLine(img, tvp[cursor-1], pointer, Scalar(0,128,0));
                imshow("Curve", img);
            }
            break;
    }
}


void pvec2mat(vector<Point> pvec, Mat& mat)
{
    mat = Mat(pvec.size(), 2, CV_32FC1);
    for(size_t i = 0; i < pvec.size(); i++)
    {
        mat.at<float>(i,0) = pvec[i].x;
        mat.at<float>(i,1) = pvec[i].y;
    }
}

void multiHermite(Mat img, vector<Point> tvp)
{
    vector<Point> pvec;
    for(size_t i = 3; i < tvp.size(); i+=2)
    {
        pvec.push_back(tvp[i-3]);            // p0
        pvec.push_back(tvp[i-1]);            // p1
        pvec.push_back(tvp[i-2] - tvp[i-3]); // dp0
        pvec.push_back(tvp[i]   - tvp[i-1]); // dp1
            
        drawHermite(img, pvec);
        pvec.clear();
    }
}

// Implementation 2, Matrix Style
void drawHermite(Mat img, vector<Point> points, int quantity, float min, float max, Scalar color)
{
    float coef[4][4] =
        {{ 2, -2,  1,  1},
         {-3,  3, -2, -1},
         { 0,  0,  1,  0},
         { 1,  0,  0,  0}};
    Mat hm = Mat(4, 4, CV_32FC1, coef); // Hermite Matrix
    Mat pts;
    pvec2mat(points, pts);

    Mat p;
    Point pt;
    float delta = (max - min) / quantity;
    for(float u = min; u < max; u += delta)
    {
        float var[4] = {(u*u*u), (u*u), u, 1.0};
        Mat vars = Mat(1, 4, CV_32FC1, var);
        p = vars * hm * pts;
        pt = Point(p.at<float>(0,0), p.at<float>(0,1));
        line(img, pt, pt, color);
    }

    imshow("Curve", img);
}


void onBezier(int event, int x, int y, int flags, void *param)
{
    int cursor;
    Scalar color = Scalar(0,128,0);
    switch(event)
    {
        case EVENT_LBUTTONDOWN:
            cursor = tvp.size();
            if(bezier)
            {
                tvp.push_back(Point(x,y));
                line(img, tvp[cursor-1], pointer, color);
                imshow("Curve", img);
            }
            break;
        case EVENT_MBUTTONDOWN:
            cursor = tvp.size();
            line(img, tvp[cursor-1], pointer, Scalar(255,255,255));
            imshow("Curve", img);
            bezier = false;
            break;
        case EVENT_MOUSEMOVE:
            cursor = tvp.size();
            if(bezier && cursor > 0)
            {
                line(img, tvp[cursor-1], pointer, Scalar(255,255,255));
                pointer = Point(x,y);
                line(img, tvp[cursor-1], pointer, color);
                imshow("Curve", img);
            }
            break;
    }
}

void multiBezier(Mat img, vector<Point> tvp)
{
    vector<Point> pvec;
    for(size_t i = 3; i < tvp.size(); i+=3)
    {
        pvec.push_back(tvp[i-3]);
        pvec.push_back(tvp[i-2]);
        pvec.push_back(tvp[i-1]);
        pvec.push_back(tvp[i]);
        drawBezier(img, pvec);
        pvec.clear();
    }
}


void drawBezier(Mat img, vector<Point> points, int quantity, float min, float max, Scalar color)
{
    float coef[4][4] =
        {{-1,  3, -3,  1},
         { 3, -6,  3,  0},
         {-3,  3,  0,  0},
         { 1,  0,  0,  0}};
    Mat bm = Mat(4, 4, CV_32FC1, coef); // Bezier Matrix
    Mat pts;
    pvec2mat(points, pts);

    Mat p;
    Point pt;
    float delta = (max - min) / quantity;
    for(float u = min; u < max; u += delta)
    {
        float var[4] = {(u*u*u), (u*u), u, 1.0};
        Mat vars = Mat(1, 4, CV_32FC1, var);
        p = vars * bm * pts;
        pt = Point(p.at<float>(0,0), p.at<float>(0,1));
        line(img, pt, pt, color);
    }

    imshow("Curve", img);
}




void onBezier2(int event, int x, int y, int flags, void *param)
{
    Scalar color = Scalar(0,128,0);
    int end;
    switch (event)
    {
        //按下鼠标左键
        case EVENT_LBUTTONDOWN:
            if(stop) break;
            end = polygon.size() - 1;
            if(end >= 0)
            {
                line(img, polygon.at(end), Point(x,y), color);
                imshow("Curve", img);
            }
            polygon.push_back( Point(x,y) );
            break;

        case EVENT_MBUTTONDOWN:
            end = polygon.size() - 1;
            if(end >= 0)
            {
                pointer = Point(x,y);
                line(img, polygon.at(end), pointer, Scalar(255,255,255));
                stop = true;

                polygon.push_back(polygon.at(0));
                line(img, polygon.at(0), polygon.at(end), color);
                imshow("Curve", img);
            }
            break;

        case EVENT_MOUSEMOVE:
            if(stop) break;
            end = polygon.size() - 1;
            if(end >= 0)
            {
                line(img, polygon.at(end), pointer, Scalar(255,255,255));
                pointer = Point(x,y);
                line(img, polygon.at(end), pointer, color);
                imshow("Curve", img);
            }
            break;
    }
}

void multiBezier2(Mat img, vector<Point> tvp)
{
    size_t i;
    for(i = 1; i < polygon.size(); i++)
    {
        tvp.push_back((polygon[i-1] + polygon[i]) / 2);
        tvp.push_back(polygon[i]);
    }
    tvp.push_back(tvp[0]);
    
    vector<Point> pvec;
    for(size_t i = 2; i < tvp.size(); i += 2)
    {
        pvec.push_back(tvp[i-2]);
        pvec.push_back(tvp[i-1]);
        pvec.push_back(tvp[i]);
        drawBezier2(img, pvec);
        pvec.clear();
    }
}


void drawBezier2(Mat img, vector<Point> points, int quantity, float min, float max, Scalar color)
{
    float coef[3][3] =
        {{ 1, -2,  1},
         {-2,  2,  0},
         { 1,  0,  0}};
    Mat bm = Mat(3, 3, CV_32FC1, coef); // Bezier Matrix
    Mat pts;
    pvec2mat(points, pts);

    Mat p;
    Point pt;
    float delta = (max - min) / quantity;
    for(float u = min; u < max; u += delta)
    {
        float var[3] = {(u*u), u, 1.0};
        Mat vars = Mat(1, 3, CV_32FC1, var);
        p = vars * bm * pts;
        pt = Point(p.at<float>(0,0), p.at<float>(0,1));
        line(img, pt, pt, color);
    }

    imshow("Curve", img);
}




void onBSpline(int event, int x, int y, int flags, void *param)
{
    int cursor;
    Scalar color = Scalar(0,128,0);
    switch(event)
    {
        case EVENT_LBUTTONDOWN:
            cursor = tvp.size();
            if(bspline)
            {
                tvp.push_back(Point(x,y));
                line(img, tvp[cursor-1], pointer, color);
                imshow("Curve", img);
            }
            break;
        case EVENT_MBUTTONDOWN:
            cursor = tvp.size();
            line(img, tvp[cursor-1], pointer, Scalar(255,255,255));
            imshow("Curve", img);
            bezier = false;
            break;
        case EVENT_MOUSEMOVE:
            cursor = tvp.size();
            if(bspline && cursor > 0)
            {
                line(img, tvp[cursor-1], pointer, Scalar(255,255,255));
                pointer = Point(x,y);
                line(img, tvp[cursor-1], pointer, color);
                imshow("Curve", img);
            }
            break;
    }
}

void multiBSpline(Mat img, vector<Point> tvp)
{
    vector<Point> pvec;
    for(size_t i = 3; i < tvp.size(); i+=1)
    {
        pvec.push_back(tvp[i-3]);
        pvec.push_back(tvp[i-2]);
        pvec.push_back(tvp[i-1]);
        pvec.push_back(tvp[i]);
        drawBSpline(img, pvec);
        pvec.clear();
    }
}


void drawBSpline(Mat img, vector<Point> points, int quantity, float min, float max, Scalar color)
{
    float coef[4][4] =
        {{-1,  3, -3,  1},
         { 3, -6,  3,  0},
         {-3,  0,  3,  0},
         { 1,  4,  1,  0}};
    Mat bm = Mat(4, 4, CV_32FC1, coef); // Bezier Matrix
    Mat pts;
    pvec2mat(points, pts);

    Mat p;
    Point pt;
    float delta = (max - min) / quantity;
    for(float u = min; u < max; u += delta)
    {
        float var[4] = {(u*u*u), (u*u), u, 1.0};
        Mat vars = Mat(1, 4, CV_32FC1, var);
        p = vars * bm * pts;
        pt = Point(p.at<float>(0,0), p.at<float>(0,1)) / 6.0;
        line(img, pt, pt, color);
    }

    imshow("Curve", img);
}
