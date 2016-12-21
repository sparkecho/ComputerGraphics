#include <iostream>
#include <opencv2/opencv.hpp>

#define LEN 20

using namespace cv;
using namespace std;

// Global Variables
Mat img;
Mat blank;
vector<Point> tvp;              // Type value point(型值点)
vector<Point> polygon;          // for bezier2
bool stop = false;              // for bezier2
Point pointer;                  // pointer of mouse
bool directionx[LEN];           // true : enlarge the x cordinate
bool directiony[LEN];           // true : enlarge the y cordinate

void animation();
void pvec2mat(vector<Point> pvec, Mat& mat);
void onBezier2(int event, int x, int y, int flags, void *param);
void multiBezier2(Mat img, vector<Point> tvp);
void drawBezier2(Mat img, vector<Point> points, int quantity = 200,
                 float min = 0.0, float max = 1.0, Scalar color = Scalar(0,0,255));

// Main Function
int main(int /*argc*/, char** /*argv*/)
{
    img = Mat(600, 600, CV_8UC3, Scalar(255,255,255));
    blank = Mat(600, 600, CV_8UC3, Scalar(255,255,255));
    
    namedWindow("Curve");
    imshow("Curve", img);
    char cmd = waitKey(0);

    for(;;)
    {
        if(cmd != 's' && cmd != 'e')
        {
            cmd = waitKey(0);
            continue;
        }
        switch(cmd)
        {
            case 's':
                setMouseCallback("Curve", onBezier2, NULL);
                waitKey(0);
                animation();
                break;
            case 'e':
                break;
        }
        break;
    }

    imshow("Curve", img);
    waitKey(0);

    return 0;
}

// Function Definition
void animation()
{
    for(int i = 0; i < LEN; i++)
    {
        directionx[i] = true;
        directiony[i] = true;
    }
    multiBezier2(img, tvp);
    char cmd = waitKey(0);
    int width = img.cols;
    int height = img.rows;
    int step;
    while(cmd != 'q' && cmd != 'Q' && cmd != 27)
    {
        cmd = waitKey(50);
        if(cmd == 'p')
            while(waitKey(50) != 'p');
        blank.copyTo(img);
        for(size_t i = 1; i < polygon.size(); i++)
        {
            step = i * 3;
            Point p = polygon[i];
            if(i % 2 == 0)
            {
                if(height - p.y < step || p.y < step)
                    directiony[i] = !directiony[i];
                if(directiony[i])
                    polygon[i].y += step;
                else
                    polygon[i].y -= step;
            }
            else
            {
                if(width - p.x < step || p.x < step)
                    directionx[i] = !directionx[i];
                if(directionx[i])
                    polygon[i].x += step;
                else
                    polygon[i].x -= step;
            }
        }
        size_t end = polygon.size() - 1;
        polygon[0] = polygon[end];
        multiBezier2(img, tvp);
    }
}


void multiBezier2(Mat img, vector<Point> tvp)
{
    tvp.clear();
    for(size_t i = 1; i < polygon.size(); i++)
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



void pvec2mat(vector<Point> pvec, Mat& mat)
{
    mat = Mat(pvec.size(), 2, CV_32FC1);
    for(size_t i = 0; i < pvec.size(); i++)
    {
        mat.at<float>(i,0) = pvec[i].x;
        mat.at<float>(i,1) = pvec[i].y;
    }
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
