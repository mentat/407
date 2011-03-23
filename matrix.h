#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

using namespace std;


template <class T>
class Matrix
{
public:
    Matrix(unsigned int columns, unsigned int rows)
    {
        resize(columns, rows);
    }

    Matrix(unsigned int columns, unsigned int rows, T[][] array)
    {
        
    }

    Matrix(const Matrix& mat);

    Matrix dot(const Matrix& mat);
    
    Matrix cross(const Matrix& mat);

    /// Matrix addition
    friend Matrix operator +(const Matrix& left, const Matrix& right);

    /// Matrix subtraction
    friend Matrix operator -(const Matrix& left, const Matrix& right);

    /// Scalar Mult
    friend Matrix operator *(T scalar, const Matrix& right);
    friend Matrix operator *(const Matrix& left, T scalar);

    Matrix identity();

    Matrix inverse();

    Matrix& operator=(const Matrix& mat);

private:

    void zero();
    void resize(int y, int x)
    {
        m_matrix.resize(y);

        for (int i = 0; i < x; i++)
            m_matrix[i].resize(rows, 0)
    }

    vector<vector<T> > m_matrix;

};



#endif
