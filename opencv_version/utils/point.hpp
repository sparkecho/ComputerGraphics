#ifndef _POINT_HPP_
#define _POINT_HPP_

#include <iostream>
#include "matrix.hpp"

using namespace std;

///////////////////////// temp /////////////////////////////
class CPoint
{
public:
    float x, y;
    CPoint(float x, float y) { this->x = x; this->y = y; }
};
////////////////////////////////////////////////////////////

class Point
{
public:
    float x, y;
    Point() { x = 0; y = 0; }
    Point(float _x, float _y) { x = _x; y = _y; }
    Point(CPoint p) { x = p.x; y = p.y; }
    inline Point operator = (Point p)  { x = p.x; y = p.y; return (*this); }
    inline Point operator = (CPoint p) { x = p.x; y = p.y; return (*this); }
    inline Point operator += (Point p)  { x += p.x; y += p.y; return (*this); }
    inline Point operator += (CPoint p) { x += p.x; y += p.y; return (*this); }
    inline Point operator -= (Point p)  { x -= p.x; y -= p.y; return (*this); }
    inline Point operator -= (CPoint p) { x -= p.x; y -= p.y; return (*this); }
    inline Point operator *= (float s) { x *= s; y *= s; return (*this); }
    inline Point operator /= (float s) { x /= s; y /= s; return (*this); }
    inline Point operator + (Point p)  { Point t; t.x = x + p.x; t.y = y + p.y; return (t); }
    inline Point operator + (CPoint p) { Point t; t.x = x + p.x; t.y = y + p.y; return (t); }
    inline Point operator - (Point p)  { Point t; t.x = x - p.x; t.y = y - p.y; return (t); }
    inline Point operator - (CPoint p) { Point t; t.x = x - p.x; t.y = y - p.y; return (t); }
    inline Point operator * (float s) { Point t; t.x = x * s; t.y = y * s; return (t); }
    inline Point operator / (float s) { Point t; t.x = x / s; t.y = y / s; return (t); }
    inline operator CPoint() { return CPoint((int)x, (int)y); }
};


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
    Point4f(Matrix mx)          // must be 1*4 matrix
    {
        x = mx[0][0];
        y = mx[0][1];
        z = mx[0][2];
        s = mx[0][3];
    }

    inline Point4f operator = (Point p)  { x = p.x; y = p.y; return (*this); }
    inline Point4f operator = (CPoint p) { x = p.x; y = p.y; return (*this); }
    inline Point4f operator += (Point p)  { x += p.x; y += p.y; return (*this); }
    inline Point4f operator += (CPoint p) { x += p.x; y += p.y; return (*this); }
    inline Point4f operator -= (Point p)  { x -= p.x; y -= p.y; return (*this); }
    inline Point4f operator -= (CPoint p) { x -= p.x; y -= p.y; return (*this); }
    inline Point4f operator + (Point p)  { Point4f t; t.x = x + p.x; t.y = y + p.y; return (t); }
    inline Point4f operator + (CPoint p) { Point4f t; t.x = x + p.x; t.y = y + p.y; return (t); }
    inline Point4f operator - (Point p)  { Point4f t; t.x = x - p.x; t.y = y - p.y; return (t); }
    inline Point4f operator - (CPoint p) { Point4f t; t.x = x - p.x; t.y = y - p.y; return (t); }

    inline Point4f operator =  (Point4f p) { x = p.x; y = p.y; z = p.z; s = p.s; return (*this); }
    inline Point4f operator +  (Point4f p) { return Point4f(x+p.x, y+p.y, z+p.z, 1); }
    inline Point4f operator -  (Point4f p) { return Point4f(x-p.x, y-p.y, z-p.z, 1); }
    inline Point4f operator *  (float scale) { return Point4f(x*scale, y*scale, z*scale, 1); }
    inline Point4f operator /  (float scale) { return Point4f(x/scale, y/scale, z/scale, 1); }
    inline Point4f operator += (Point4f p) { x += p.x; y += p.y; z += p.z; return (*this); }
    inline Point4f operator -= (Point4f p) { x -= p.x; y -= p.y; z -= p.z; return (*this); }
    inline Point4f operator *= (float scale) { x *= scale; y *= scale; z *= scale; return (*this); }
    inline Point4f operator /= (float scale) { x /= scale; y /= scale; z /= scale; return (*this); }
    inline Point4f operator *  (Matrix mx) { return Point4f(Matrix(*this) * mx); }


    inline operator Point()
    {
        return Point(x,y);
    }
    inline operator Matrix()
    {
        float arr[4] = {x, y, z, s};
        return Matrix(1,4,arr);
    }
    void print()
    {
        std::cout << "[" << x << "," << y << "," << z << "," << s << "]" << endl;
    }
};


#endif
