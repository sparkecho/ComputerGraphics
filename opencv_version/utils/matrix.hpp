#ifndef _MATRIX_HPP_
#define _MATRIX_HPP_

#include <iostream>
#include <vector>

using namespace std;


class Matrix
{
public:
    vector<vector<float> > matrix;
    int rows, cols;
public:
    Matrix() { rows = 0; cols = 0; }
    Matrix(int rows, int cols);
    Matrix(int rows, int cols, float value);
    Matrix(int rows, int cols, float* data);
    ~Matrix();
    float& at(int i, int j) { return matrix[i][j]; }
    inline Matrix operator ! ();
    inline Matrix operator - ();
    inline Matrix operator = (Matrix mx);
    inline Matrix operator + (Matrix mx);
    inline Matrix operator - (Matrix mx);
    inline Matrix operator * (Matrix mx);
    inline Matrix operator * (float scale);
    inline Matrix operator / (float scale);
    inline Matrix operator += (Matrix mx);
    inline Matrix operator -= (Matrix mx);
    inline Matrix operator *= (float scale);
    inline Matrix operator /= (float scale);
    inline Matrix operator () (vector<int> mx);
    inline Matrix operator () (vector<float> mx);
    inline Matrix operator () (vector<vector<int> > mx);
    inline Matrix operator () (vector<vector<float> > mx);
    inline vector<float>& operator [] (int i) { return matrix[i]; }
    Matrix eye(int rows, int cols);
    Matrix eye(int rows, int cols, int coloff);
    Matrix eye(int rows, int cols, int coloff, int rowoff);
    void copyTo(Matrix& mx);
    void print();
};

Matrix::Matrix(int rows, int cols)
{
    this->rows = rows;
    this->cols = cols;
    matrix = vector<vector<float> >(rows, vector<float>(cols));
}

Matrix::Matrix(int rows, int cols, float value)
{
    new(this) Matrix(rows, cols);
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            matrix[i][j] = value;
        }
    }
}


Matrix::Matrix(int rows, int cols, float* data)
{
    new(this) Matrix(rows, cols);
    float* p = data;
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            matrix[i][j] = *p;
            p++;
        }
    }
}

Matrix::~Matrix()
{
    matrix.clear();
}

inline Matrix Matrix::operator ! ()
{
    Matrix mat(cols,rows);
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            mat[j][i] = matrix[i][j];
    return (mat);
}

inline Matrix Matrix::operator - ()
{
    Matrix mat(cols,rows);
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            mat[i][j] = -(matrix[i][j]);
    return (mat);
}

inline Matrix Matrix::operator = (Matrix mx)
{
    rows = mx.rows;
    cols = mx.cols;
    matrix = mx.matrix; 
    return (*this);
}


inline Matrix Matrix::operator + (Matrix mx)
{
    if(mx.rows != rows || mx.cols != cols)
    {
        cout << "Dimensions are not match!" << endl;
        return (*this);
    }
    Matrix mat(rows,cols);
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            mat[i][j] = matrix[i][j] + mx[i][j];
    return (mat);
}

inline Matrix Matrix::operator - (Matrix mx)
{
    if(mx.rows != rows || mx.cols != cols)
    {
        cout << "Dimensions are not match!" << endl;
        return (*this);
    }
    Matrix mat(rows,cols);
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            mat[i][j] = matrix[i][j] - mx[i][j];
    return (mat);
}

inline Matrix Matrix::operator * (Matrix mx)
{
    if(mx.rows != cols)
    {
        cout << "Dimensions are not match!" << endl;
        return (*this);
    }
    Matrix mat(rows,mx.cols);
    int m = rows, n = cols, p = mx.cols;
    float sum = 0;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < p; j++)
        {
            sum = 0;
            for (int k = 0; k < n; k++)
            {
                sum += matrix[i][k] * mx[k][j];
            }
            mat[i][j] = sum;
        }
    }
    return (mat);
}

inline Matrix Matrix::operator * (float scale)
{
    Matrix mat(rows,cols);
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            mat[i][j] = matrix[i][j] * scale;
    return (mat);
}

inline Matrix Matrix::operator / (float scale)
{
    if(scale == 0)
    {
        cout << "ERROR: Divide zero!" << endl;
        return (*this);
    }
    Matrix mat(rows,cols);
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            mat[i][j] = matrix[i][j] / scale;
    return (mat);
}


inline Matrix Matrix::operator += (Matrix mx)
{
    if(mx.rows != rows || mx.cols != cols)
    {
        cout << "Dimensions are not match!" << endl;
        return (*this);
    }
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            matrix[i][j] += mx[i][j]; // mx.at(i,j);
    return (*this);
}

inline Matrix Matrix::operator -= (Matrix mx)
{
    if(mx.rows != rows || mx.cols != cols)
    {
        cout << "Dimensions are not match!" << endl;
        return (*this);
    }
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            matrix[i][j] -= mx[i][j];
    return (*this);
}

inline Matrix Matrix::operator *= (float scale)
{
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            matrix[i][j] *= scale;
    return (*this);
}

inline Matrix Matrix::operator /= (float scale)
{
    if(scale == 0)
    {
        cout << "ERROR: Divide zero!" << endl;
        return (*this);
    }
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            matrix[i][j] /= scale;
    return (*this);
}

inline Matrix Matrix::operator () (vector<int> mx)
{
    rows = 1;
    cols = mx.size();
    Matrix mat(rows,cols);
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            mat[i][j] = mx[j];
    return (mat);
}


inline Matrix Matrix::operator () (vector<float> mx)
{
    rows = 1;
    cols = mx.size();
    Matrix mat(rows,cols);
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            mat[i][j] = mx[j];
    return (mat);
}

inline Matrix Matrix::operator () (vector<vector<int> > mx)
{
    rows = mx.size();
    cols = mx[0].size();
    Matrix mat(rows,cols);
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            mat[i][j] = mx[i][j];
    return (mat);
}

inline Matrix Matrix::operator () (vector<vector<float> > mx)
{
    rows = mx.size();
    cols = mx[0].size();
    Matrix mat(rows,cols);
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            mat[i][j] = mx[i][j];
    return (mat);
}

Matrix Matrix::eye(int rows, int cols)
{
    if(rows > this->rows || cols > this->cols)
    {
        cout << "ERROR: eye matrix out of bound!" << endl;
    }
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            if(i == j)
                matrix[i][j] = 1;
            else
                matrix[i][j] = 0;
        }
    }
    return (*this);
}

Matrix Matrix::eye(int rows, int cols, int coloff)
{
    if(rows > this->rows || cols > this->cols)
    {
        cout << "ERROR: eye matrix out of bound!" << endl;
    }
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            if(i == j)
                matrix[i][j + coloff] = 1;
            else
                matrix[i][j + coloff] = 0;
        }
    }
    return (*this);
}

Matrix Matrix::eye(int rows, int cols, int coloff, int rowoff)
{
    if(rows > this->rows || cols > this->cols)
    {
        cout << "ERROR: eye matrix out of bound!" << endl;
    }
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            if(i == j)
                matrix[i + rowoff][j + coloff] = 1;
            else
                matrix[i + rowoff][j + coloff] = 0;
        }
    }
    return (*this);
}


void Matrix::copyTo(Matrix& mx)
{
    if(mx.rows != rows || mx.cols != cols)
    {
        cout << "Dimensions are not match!" << endl;
        return;
    }
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            mx[i][j] = matrix[i][j];
    return;
}


void Matrix::print()
{
    cout << endl;
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
            cout << matrix[i][j] << "\t";
        cout << endl;
    }
    cout << endl;
}


// Functions
Matrix eye(int rows, int cols)
{
    Matrix mat(rows,cols);
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            if(i == j)
                mat[i][j] = 1;
            else
                mat[i][j] = 0;
        }
    }
    return (mat);
}

#endif
