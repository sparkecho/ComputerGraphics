#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Mat img;
vector<Point> vertex;
Point cursor;
bool stop = false;

void onMouse(int event, int x, int y, int flag, void *param);

int main(int, char**)
{
    img = Mat(400,600,CV_8UC3,Scalar(255,255,255));
    namedWindow("Polygon");
    imshow("Polygon", img);
    setMouseCallback("Polygon", onMouse, NULL);
    waitKey(0);
    for(size_t i = 0; i < vertex.size(); i++)
        cout << vertex.at(i) << " ";
    cout << endl;
    return 0;
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
