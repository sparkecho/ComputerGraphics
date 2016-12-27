#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


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
    void print()
    {
        std::cout << "[" << x << "," << y << "," << z << "," << s << "]";
    }
};

// typedef Point4f Point4f;

// Global Variables
Mat img;
Point center;                   // 透视中心 x和y
bool endp = false;
vector<vector<Point4f> > face(4, vector<Point4f>(4));

// Function Prototype
void Point4f2Mat(Mat pos, Point4f point);
void Mat2Point4f(Point4f& point, Mat pos);
Point transCordinate(Point4f pt, Point center);
void perspective(Mat& trans, Point p, bool flag);
void points2mats(Mat& xmat, Mat& ymat, Mat& zmat, vector<vector<Point4f> > face);
void drawBezier(Mat img, vector<vector<Point4f> > face, int quantity = 200,
                float min = 0.0f, float max = 1.0f, Scalar color = Scalar(0,0,255));
void onMouse(int event, int x, int y, int flags, void *param);

// Main Function
int main()
{
    face[0][0] = Point4f(0,0,0,1);
    face[0][1] = Point4f(0,120,20,1);
    face[0][2] = Point4f(0,240,20,1);
    face[0][3] = Point4f(0,360,0,1);

    face[1][0] = Point4f(120,0,40,1);
    face[1][1] = Point4f(120,120,60,1);
    face[1][2] = Point4f(120,240,60,1);
    face[1][3] = Point4f(120,360,40,1);

    face[2][0] = Point4f(240,0,40,1);
    face[2][1] = Point4f(240,120,60,1);
    face[2][2] = Point4f(240,240,60,1);
    face[2][3] = Point4f(240,360,40,1);

    face[3][0] = Point4f(360,0,0,1);
    face[3][1] = Point4f(360,120,20,1);
    face[3][2] = Point4f(360,240,20,1);
    face[3][3] = Point4f(360,360,0,1);

    img = Mat(600,600,CV_8UC3,Scalar(255,255,255));
    namedWindow("Curve");
    imshow("Curve", img);
    waitKey(0);
    center = Point(0,0);
    drawBezier(img, face);
    imshow("Curve", img);
    setMouseCallback("Curve", onMouse, NULL);
    waitKey(0);
    return 0;
}



// Function Definitions

void points2mats(Mat& xmat, Mat& ymat, Mat& zmat, vector<vector<Point4f> > face)
{
    // cout << "In points2mats" << endl;
    xmat = Mat(4,4,CV_32FC1);
    ymat = Mat(4,4,CV_32FC1);
    zmat = Mat(4,4,CV_32FC1);
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
        {
            xmat.at<float>(i,j) = face[i][j].x;
            ymat.at<float>(i,j) = face[i][j].y;
            zmat.at<float>(i,j) = face[i][j].z;
        }
    // cout << "points2mats ends successfully" << endl;
}


void drawBezier(Mat img, vector<vector<Point4f> > face, int quantity, float min, float max, Scalar color)
{
    // cout << "In draw Bezier" << endl;
    float coef[4][4] =
        {{-1,  3, -3,  1},
         { 3, -6,  3,  0},
         {-3,  3,  0,  0},
         { 1,  0,  0,  0}};
    Mat bm = Mat(4, 4, CV_32FC1, coef); // Bezier Matrix
    Mat xmat, ymat, zmat;
    points2mats(xmat, ymat, zmat, face);

    Mat x, y, z;
    Point4f pt;
    float delta = (max - min) / quantity;
    for(float u = min; u < max; u += delta)
    {
        float us[4] = {(u*u*u), (u*u), u, 1.0};
        Mat coefu = Mat(1, 4, CV_32FC1, us);
        for(float w = min; w < max; w += delta)
        {
            float ws[4][1] = {{w*w*w}, {w*w}, {w}, {1.0}};
            Mat coefw = Mat(4, 1, CV_32FC1, ws);
            x = coefu * bm * xmat * bm * coefw;
            y = coefu * bm * ymat * bm * coefw;
            z = coefu * bm * zmat * bm * coefw;
            pt = Point4f(x.at<float>(0,0), y.at<float>(0,0), z.at<float>(0,0), 1.0);
            Point p = transCordinate(pt, center);
            line(img, p, p, color);
        }
    }
    imshow("Curve", img);
    // cout << "drawBezier ends here" << endl;
}


Point transCordinate(Point4f pt, Point center)
{
    Mat pmat = Mat(1,4,CV_32FC1);
    Point4f2Mat(pmat, pt);
    Mat trans;
    perspective(trans, center, true);
    Point4f p;
    Mat2Point4f(p, pmat*trans);
    return Point(p.x, p.y);
}


// flag = true  透视投影
// flag = false 平行投影
void perspective(Mat& trans, Point p, bool flag)
{
    Mat persp;
    if(flag)
    {
        float x0 = p.x;
        float y0 = p.y;
        float d = 200;
        float transarr[4][4] =
            {{1, 0, 0, 0},
             {0, 1, 0, 0},
             {0, 0, 0, 0},
             {0, 0, 0, 1}};
        transarr[2][0] = x0 / d;
        transarr[2][1] = y0 / d;
        transarr[2][3] =  1 / d;
        persp = Mat(4,4,CV_32FC1,transarr);
    }
    else
    {
        float transarr[4][4] =
            {{1, 0, 0, 0},
             {0, 1, 0, 0},
             {0, 0, 1, 0},
             {0, 0, 0, 1}};
        persp = Mat(4,4,CV_32FC1,transarr);
    }
    persp.copyTo(trans);
}


void Point4f2Mat(Mat pos, Point4f point)
{
    pos.at<float>(0) = point.x;
    pos.at<float>(1) = point.y;
    pos.at<float>(2) = point.z;
    pos.at<float>(3) = point.s;
}

void Mat2Point4f(Point4f& point, Mat pos)
{
    point.x = pos.at<float>(0);
    point.y = pos.at<float>(1);
    point.z = pos.at<float>(2);
    point.s = pos.at<float>(3);
}


void onMouse(int event, int x, int y, int flags, void *param)
{
    vector<vector<Point> > plain(4, vector<Point>(4));
    switch (event)
    {
        int i, j;
        //按下鼠标左键
        case EVENT_LBUTTONDOWN:
            if(!endp)
            {
                center = Point(x,y);
                img = Mat(600,600,CV_8UC3,Scalar(255,255,255));
                drawBezier(img, face);
                for(i = 0; i < 4; i++)
                    for(j = 0; j < 4; j++)
                    {
                        Point c = transCordinate(face[i][j], center);
                        plain[i][j] = c;
                        circle(img, c, 2, Scalar(0,255,0));
                    }
                // draw grids
                for(i = 0; i < 4; i++)
                    for(j = 1; j < 4; j++)
                        line(img, plain[i][j], plain[i][j-1], Scalar(0,255,0));
                for(j = 0; j < 4; j++)
                    for(i = 1; i < 4; i++)
                        line(img, plain[i][j], plain[i-1][j], Scalar(0,255,0));
                imshow("Curve", img);
            }
            break;
        case EVENT_MBUTTONDOWN:
            endp = true;
            break;
    }
}
