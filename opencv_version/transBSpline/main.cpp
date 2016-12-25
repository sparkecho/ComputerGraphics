#include <iostream>
#include <opencv2/opencv.hpp>

#define PI 3.1415

using namespace cv;
using namespace std;


// Class Definition
class Point4f
{
public:
    float x;
    float y;
    float z;
    float s;
    Point4f()
    {
        this->x = 0;
        this->y = 0;
        this->z = 0;
        this->s = 1;
    }
    Point4f(float x, float y)
    {
        this->x = x;
        this->y = y;
        this->z = 0;
        this->s = 1;
    }
    Point4f(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->s = 1;
    }
    Point4f(float x, float y, float z, float s)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->s = s;
    }
    Point4f(Mat vec)
    {
        if(vec.size() != Size(1,4))
        {
            cout << "Mat to Point4f, invalid size" << endl;
            return;
        }
        this->x = (int)vec.at<float>(0);
        this->y = (int)vec.at<float>(1);
        this->z = (int)vec.at<float>(2);
        this->s = (int)vec.at<float>(3);
    }
    inline operator Point() { return Point(x, y); }
    void print()
    {
        std::cout << "[" << x << "," << y << "," << z << "," << s << "]";
    }
};




// Global Variables
Mat img;
vector<Point4f> tvp;            // Type value point(型值点)
bool bspline = true;            // if bspline tvp mark is done
Point4f pointer;                  // pointer of mouse

// Function Prototype
Point transCordinate(Mat p);
void Point4f2Mat(Point4f point, Mat& pos);
void Mat2Point4f(Mat pos, Point4f& point);
void translate(Mat& pt, float offset);
void rotate(Mat& pt);
void motion(Mat& pt);
void pvec2mat(vector<Point4f> pvec, Mat& mat);
void onBSpline(int event, int x, int y, int flags, void *param);
void multiBSpline(Mat img, vector<Point4f> tvp, bool projectp, Scalar color);
void multiBSpline2(Mat img, vector<Point4f> tvp, bool projectp);
void drawBSpline(Mat img, vector<Point4f> points, bool projectp, Scalar color = Scalar(0,0,255),
                 int quantity = 200, float min = 0.0, float max = 1.0);

// Main Function
int main(int /*argc*/, char** /*argv*/)
{
    img = Mat(600, 600, CV_8UC3, Scalar(255,255,255));
    namedWindow("Curve");
    imshow("Curve", img);

    setMouseCallback("Curve", onBSpline, NULL);
    waitKey(0);
    // multiBSpline(img, tvp, false, Scalar(0,0,255));
    // imshow("Curve", img);

    char cmd = waitKey(50);
    while(1)
    {
        if(cmd == 'q' || cmd == 'Q' || cmd == 27)
            break;
        if(cmd == '3')
        {
            img = Mat(600, 600, CV_8UC3, Scalar(255,255,255));
            imshow("Curve", img);
            float transarr[4][4] =
                {{1, 0, 0, 0},
                 {0, 1, 0, 0},
                 {0, 0, 1, 0},
                 {0, 0, 0, 1}};
            Mat trans = Mat(4,4,CV_32FC1,transarr);
            Mat pos(1,4,CV_32FC1);
            // rotate(trans);
            for(int i = 0; i < 36; i++)
            {
                for(size_t j = 0; j < tvp.size(); j++)
                {
                    Point4f2Mat(tvp[j], pos);
                    motion(pos);
                    Mat2Point4f(pos, tvp[j]);
                }
                waitKey(0);
                multiBSpline(img, tvp, true, Scalar(0,0,255));
            }
        }
        else if(cmd == '2')
        {
            img = Mat(600, 600, CV_8UC3, Scalar(255,255,255));
            float mx = tvp[0].x;
            vector<Point4f> tmp;
            for(size_t i = 0; i < tvp.size(); i++)
            {
                float x = tvp[i].x;
                x = mx * 2 - x;
                tmp.push_back(Point4f(x,tvp[i].y, tvp[i].z));
            }
            multiBSpline(img, tvp, false, Scalar(0,0,255));
            multiBSpline(img, tmp, false, Scalar(0,0,255));
            imshow("Curve", img);
        }
        else if(cmd == '1')
        {
            multiBSpline2(img, tvp, false);
            imshow("Curve", img);
        }
        cmd = waitKey(0);
    }
    waitKey(0);

    return 0;
}

// Function Definition

void pvec2mat(vector<Point4f> pvec, Mat& mat)
{
    mat = Mat(pvec.size(), 4, CV_32FC1);
    for(size_t i = 0; i < pvec.size(); i++)
    {
        mat.at<float>(i,0) = pvec[i].x;
        mat.at<float>(i,1) = pvec[i].y;
        mat.at<float>(i,2) = pvec[i].z;
        mat.at<float>(i,3) = pvec[i].s;
    }
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
                if(cursor == 0)
                {
                    tvp.push_back(Point4f(x,y));
                    tvp.push_back(Point4f(x,y));
                }
                tvp.push_back(Point4f(x,y));
                line(img, tvp[cursor-1], pointer, color);
                imshow("Curve", img);
            }
            break;
        case EVENT_MBUTTONDOWN:            
            cursor = tvp.size();
            tvp.push_back(tvp[cursor-1]);
            tvp.push_back(tvp[cursor-1]);
            line(img, tvp[cursor-1], pointer, Scalar(255,255,255));
            imshow("Curve", img);
            bspline = false;
            break;
        case EVENT_MOUSEMOVE:
            cursor = tvp.size();
            if(bspline && cursor > 0)
            {
                line(img, tvp[cursor-1], pointer, Scalar(255,255,255));
                pointer = Point4f(x,y);
                line(img, tvp[cursor-1], pointer, color);
                imshow("Curve", img);
            }
            break;
    }
}

void multiBSpline(Mat img, vector<Point4f> tvp, bool projectp, Scalar color)
{
    vector<Point4f> pvec;
    for(size_t i = 3; i < tvp.size(); i+=1)
    {
        pvec.push_back(tvp[i-3]);
        pvec.push_back(tvp[i-2]);
        pvec.push_back(tvp[i-1]);
        pvec.push_back(tvp[i]);
        drawBSpline(img, pvec, projectp, color);
        pvec.clear();
    }
}


void multiBSpline2(Mat img, vector<Point4f> tvp, bool projectp)
{
    Scalar color;
    vector<Point4f> pvec;
    for(size_t i = 3; i < tvp.size(); i+=1)
    {
        pvec.push_back(tvp[i-3]);
        pvec.push_back(tvp[i-2]);
        pvec.push_back(tvp[i-1]);
        pvec.push_back(tvp[i]);
        if(i % 3 == 0)
            color = Scalar(255,0,0);
        else if(i % 3 == 1)
            color = Scalar(0,255,0);
        else
            color = Scalar(0,0,255);
        drawBSpline(img, pvec, projectp, color);
        pvec.clear();
    }
}


void drawBSpline(Mat img, vector<Point4f> points, bool projectp, Scalar color, int quantity, float min, float max)
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
        
        if(projectp)
            pt = transCordinate(p) / 6.0;
        else
            pt = Point(p.at<float>(0,0), p.at<float>(0,1)) / 6.0;
        line(img, pt, pt, color);
    }

    imshow("Curve", img);
}


void motion(Mat& pt)
{
    float offset = tvp[0].x;
    translate(pt, -offset);
    rotate(pt);
    translate(pt, offset);
}

void translate(Mat& pt, float offset)
{
    float transarr[4][4] =
        {{1, 0, 0, 0},
         {0, 1, 0, 0},
         {0, 0, 1, 0},
         {0, 0, 0, 1}};
    Mat trans = Mat(4,4,CV_32FC1,transarr);
    trans.at<float>(3,0) += offset;
    pt *= trans;
}

void rotate(Mat& pt)
{
    float transarr[4][4] =
        {{1, 0, 0, 0},
         {0, 1, 0, 0},
         {0, 0, 1, 0},
         {0, 0, 0, 1}};
    Mat trans = Mat(4,4,CV_32FC1,transarr);

    float theta = PI / 18;
    float sinTheta = sin(theta);
    float cosTheta = cos(theta);
    // 绕 y 轴旋转
    trans.at<float>(0,0) =   cosTheta;
    trans.at<float>(2,0) =   sinTheta;
    trans.at<float>(0,2) = - sinTheta;
    trans.at<float>(2,2) =   cosTheta;

    pt *= trans;
}

void Point4f2Mat(Point4f point, Mat& pos)
{
    pos.at<float>(0) = point.x;
    pos.at<float>(1) = point.y;
    pos.at<float>(2) = point.z;
    pos.at<float>(3) = point.s;
}

void Mat2Point4f(Mat pos, Point4f& point)
{
    point.x = pos.at<float>(0);
    point.y = pos.at<float>(1);
    point.z = pos.at<float>(2);
    point.s = pos.at<float>(3);
}


Point transCordinate(Mat p)
{
    float transarr[4][4] =
        {{    1,     0, 0, 0},
         {    0,     1, 0, 0},
         {0.707, 0.707, 0, 0},
         {    0,     0, 0, 1}};
    Mat trans = Mat(4,4,CV_32FC1,transarr);
    Mat pt = p * trans;
    return Point(pt.at<float>(0,0), pt.at<float>(0,1));
}
