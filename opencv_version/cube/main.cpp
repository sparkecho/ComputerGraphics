#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

// Global Constants
#define PI 3.14159

// Class Definitions
class Point4f
{
public:
    float x;
    float y;
    float z;
    float s;
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

// Global Variables
Mat img;
vector<Point4f> cube;
Point viewp = Point(0,0);
bool flag = false;


// Function Prototypes
Point transCordinate(Mat img, Point4f& point);
void drawCube(Mat img, vector<Point4f> cube, Scalar color, bool fill = false);
void drawAxes(Mat img, Scalar color);
void Mat2Point4f(Mat pos, Point4f point);
void Point4f2Mat(Point4f& point, Mat pos);
void motion(Mat trans, char key);
void resetTrans(Mat& trans);
void perspective(Mat& trans, Point p, bool flag);
void onMouse(int event, int x, int y, int flag, void *param);
void printMat(Mat mat);         // print 4*4 matrix

// Main Function
int main()
{
    // init
    cube.push_back(Point4f( 50, 50,  0,1));
    cube.push_back(Point4f( 50,100,  0,1));
    cube.push_back(Point4f(100,100,  0,1));
    cube.push_back(Point4f(100, 50,  0,1));
    cube.push_back(Point4f( 50, 50,-50,1));
    cube.push_back(Point4f( 50,100,-50,1));
    cube.push_back(Point4f(100,100,-50,1));
    cube.push_back(Point4f(100, 50,-50,1));

    // The cube to display
    vector<Point4f> dispcube;
    for(int i = 0; i < 8; i++)
    {
        dispcube.push_back(cube.at(i));
    }


    img = Mat(600,600,CV_8UC3,Scalar(255,255,255));
    drawCube(img, cube, Scalar(0,0,255), false);
    namedWindow("cube");
    imshow("cube", img);
    waitKey(0);


    float transarr[4][4] =
        {{1, 0, 0, 0},
         {0, 1, 0, 0},
         {0, 0, 1, 0},
         {0, 0, 0, 1}};
    Mat trans = Mat(4,4,CV_32FC1,transarr);
    Mat pos(1,4,CV_32FC1);

    char cmd = waitKey(30);
    while(cmd != 'q' && cmd != 'Q' && cmd != 27)
    {
        drawCube(img, dispcube, Scalar(255,255,255), false); // clear the former cube
        motion(trans, cmd);

        for(int i = 0; i < 8; i++)
        {
            Mat2Point4f(pos, cube.at(i));
            pos = pos * trans;
            Point4f2Mat(cube.at(i), pos);
        }

        // not sure
        setMouseCallback("cube", onMouse, NULL);
        perspective(trans, viewp, flag); //         透视
        for(int i = 0; i < 8; i++)
        {
            Mat2Point4f(pos, cube.at(i));
            pos = pos * trans;
            Point4f2Mat(dispcube.at(i), pos);
        }

        drawAxes(img, Scalar(100,0,0));
        drawCube(img, dispcube, Scalar(0,255,0), false); // redraw the cube
        // drawCube(img, cube, Scalar(0,255,0), false); // redraw the cube
        imshow("cube", img);
        cmd = waitKey(300);
        resetTrans(trans);
        // Test
        // for(size_t i = 0; i < cube.size(); i++)
        //     cube.at(i).print();
        // cout << endl;
    }
    waitKey(0);
    
    return 0;
}


// Functioin Definitions

void drawCube(Mat img, vector<Point4f> cube, Scalar color, bool fill)
{
    for(int i = 0; i < 3; i++)
    {
        // line(img,
        //      Point(cube[i].x*cube[i].s + offset, cube[i].y*cube[i].s + offset),
        //      Point(cube[i+1].x*cube[i+1].s + offset, cube[i+1].y*cube[i+1].s + offset),
        //      color);
        // line(img,
        //      Point(cube[i+4].x*cube[i+4].s + offset, cube[i+4].y*cube[i+4].s + offset),
        //      Point(cube[i+5].x*cube[i+5].s + offset, cube[i+5].y*cube[i+5].s + offset),
        //      color);
        line(img, transCordinate(img,cube[i]), transCordinate(img,cube[i+1]), color);
        line(img, transCordinate(img,cube[i+4]), transCordinate(img,cube[i+5]), color);
    }
    // line(img,
    //      Point(cube[0].x*cube[0].s + offset, cube[0].y*cube[0].s + offset),
    //      Point(cube[3].x*cube[3].s + offset, cube[3].y*cube[3].s + offset),
    //      color);
    // line(img,
    //      Point(cube[4].x*cube[4].s + offset, cube[4].y*cube[4].s + offset),
    //      Point(cube[7].x*cube[7].s + offset, cube[7].y*cube[7].s + offset),
    //      color);
    line(img, transCordinate(img,cube[0]), transCordinate(img,cube[3]), color);
    line(img, transCordinate(img,cube[4]), transCordinate(img,cube[7]), color);
    for(int i = 0; i < 4; i++)
    {
        // line(img,
        //      Point(cube[i].x*cube[i].s + offset, cube[i].y*cube[i].s + offset),
        //      Point(cube[i+4].x*cube[i+4].s + offset, cube[i+4].y*cube[i+4].s + offset),
        //      color);
        line(img, transCordinate(img,cube[i]), transCordinate(img,cube[i+4]), color);
    }
    // for test
    // for(int i = 0; i < 8; i++)
    // {
    //     cout << "[" << (int)cube[i].x << "," << (int)cube[i].y
    //          << "," << (int)cube[i].z << "]" << endl;
    // }

    if(fill)
    {
    }
}

void drawAxes(Mat img, Scalar color)
{
    int mx = img.cols / 2;
    int my = img.rows / 2;
    line(img, Point(0,my), Point(img.cols,my), color);
    line(img, Point(mx,0), Point(mx,img.rows), color);
}

void Mat2Point4f(Mat pos, Point4f point)
{
    pos.at<float>(0) = point.x;
    pos.at<float>(1) = point.y;
    pos.at<float>(2) = point.z;
    pos.at<float>(3) = point.s;
}

void Point4f2Mat(Point4f& point, Mat pos)
{
    point.x = pos.at<float>(0);
    point.y = pos.at<float>(1);
    point.z = pos.at<float>(2);
    point.s = pos.at<float>(3);
}

void resetTrans(Mat& trans)
{
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            if(i == j)
                trans.at<float>(i,j) = 1;
            else
                trans.at<float>(i,j) = 0;
}

/*    y
 *    ^
 *    |
 *    . ----> x   z 向外
 */
void motion(Mat trans, char key)
{
    float theta = PI / 5.0;
    float sinTheta = sin(theta);
    float cosTheta = cos(theta);
    switch(key)
    {
        case 'w':               // 沿 x 轴平移
            trans.at<float>(3,1) += 10;
            break;
        case 's':
            trans.at<float>(3,1) -= 10;
            break;
        case 'a':               // 沿 y 轴平移
            trans.at<float>(3,0) -= 10;
            break;
        case 'd':
            trans.at<float>(3,0) += 10;
            break;
        case 'z':               // 沿 z 轴平移
            trans.at<float>(3,2) -= 10;
            break;
        case 'e':
            trans.at<float>(3,2) += 10;
            break;
        case 'l':               // 绕 y 轴旋转
            trans.at<float>(0,0) =   cosTheta;
            trans.at<float>(2,0) =   sinTheta;
            trans.at<float>(0,2) = - sinTheta;
            trans.at<float>(2,2) =   cosTheta;
            break;
        case '\'':
            trans.at<float>(0,0) =   cosTheta;
            trans.at<float>(2,0) = - sinTheta;
            trans.at<float>(0,2) =   sinTheta;
            trans.at<float>(2,2) =   cosTheta;
            break;
        case 'p':               // 绕 x 轴旋转
            trans.at<float>(1,1) =   cosTheta;
            trans.at<float>(2,1) =   sinTheta;
            trans.at<float>(1,2) = - sinTheta;
            trans.at<float>(2,2) =   cosTheta;
            break;
        case ';':
            trans.at<float>(1,1) =   cosTheta;
            trans.at<float>(2,1) = - sinTheta;
            trans.at<float>(1,2) =   sinTheta;
            trans.at<float>(2,2) =   cosTheta;
            break;
        case ',':               // 绕 z 轴旋转
            trans.at<float>(0,0) =   cosTheta;
            trans.at<float>(1,0) =   sinTheta;
            trans.at<float>(0,1) = - sinTheta;
            trans.at<float>(1,1) =   cosTheta;
            break;
        case '\[':
            trans.at<float>(0,0) =   cosTheta;
            trans.at<float>(1,0) = - sinTheta;
            trans.at<float>(0,1) =   sinTheta;
            trans.at<float>(1,1) =   cosTheta;
            break;
        case '-':
            trans.at<float>(3,3) /= 1.5;
            break;
        case '=':
            trans.at<float>(3,3) *= 1.5;
            break;
    }
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

void onMouse(int event, int x, int y, int flags, void *param)
{
    int offx, offy;
    switch (event)
    {
        //按下鼠标左键
        case EVENT_LBUTTONDOWN:
            offx = img.cols / 2;
            offy = img.rows / 2;
            viewp = Point(x - offx, offy - y);
            cout << viewp << endl;
            break;
        case EVENT_MBUTTONDOWN:
            flag = !flag;
            break;
    }
}

void printMat(Mat mat)
{
    for(int i = 0; i < 4; i++)
    {
        cout << "|";
        for(int j = 0; j < 4; j++)
            cout << mat.at<float>(i,j) << "\t";
        cout << "|" << endl;
    }
    cout << endl;
}

Point transCordinate(Mat img, Point4f& point)
{
    int height = img.rows;
    int width  = img.cols;
    float x = point.x * point.s;
    float y = point.y * point.s;
    // for test
    // cout << x << "," << y << "/ ";

    int offx = width / 2;
    int offy = height / 2;
    x += offx;
    y = offy - y;
    // for test
    // cout << Point(x,y) << "\t";
    return Point(x,y);
}
