#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include <cmath>
#include <iostream>
#include <vector>
#include <iomanip>
#include <stdexcept>

using namespace std;

namespace Matrices
{
    class Matrix
    {
    public:
        /// Construct a matrix of the specified size. Initialize each element to 0.
        Matrix(int _rows, int _cols) : rows(_rows), cols(_cols), a(_rows, vector<double>(_cols, 0.0)) {}

        /// Read element at row i, column j
        const double& operator()(int i, int j) const
        {
            return a.at(i).at(j);
        }

        /// Assign element at row i, column j
        double& operator()(int i, int j)
        {
            return a.at(i).at(j);
        }

        int getRows() const { return rows; }
        int getCols() const { return cols; }

    protected:
        vector<vector<double>> a;

    private:
        int rows;
        int cols;
    };

    /// Add each corresponding element.
    inline Matrix operator+(const Matrix& a, const Matrix& b)
    {
        if (a.getRows() != b.getRows() || a.getCols() != b.getCols()) {
            throw runtime_error("Matrix dimensions must match for addition.");
        }
        Matrix result(a.getRows(), a.getCols());
        for (int i = 0; i < a.getRows(); ++i) {
            for (int j = 0; j < a.getCols(); ++j) {
                result(i, j) = a(i, j) + b(i, j);
            }
        }
        return result;
    }

    /// Matrix multiply.
    inline Matrix operator*(const Matrix& a, const Matrix& b)
    {
        if (a.getCols() != b.getRows()) {
            throw runtime_error("Matrix dimensions mismatched for multiplication.");
        }
        Matrix result(a.getRows(), b.getCols());
        for (int i = 0; i < a.getRows(); ++i) {
            for (int j = 0; j < b.getCols(); ++j) {
                for (int k = 0; k < a.getCols(); ++k) {
                    result(i, j) += a(i, k) * b(k, j);
                }
            }
        }
        return result;
    }

    /// Matrix comparison.
    inline bool operator==(const Matrix& a, const Matrix& b)
    {
        if (a.getRows() != b.getRows() || a.getCols() != b.getCols()) return false;
        for (int i = 0; i < a.getRows(); ++i) {
            for (int j = 0; j < a.getCols(); ++j) {
                if (abs(a(i, j) - b(i, j)) > 0.0001) return false;
            }
        }
        return true;
    }

    /// Matrix comparison.
    inline bool operator!=(const Matrix& a, const Matrix& b)
    {
        return !(a == b);
    }

    /// Output matrix.
    inline ostream& operator<<(ostream& os, const Matrix& a)
    {
        for (int i = 0; i < a.getRows(); ++i) {
            for (int j = 0; j < a.getCols(); ++j) {
                os << a(i, j) << (j == a.getCols() - 1 ? "" : " ");
            }
            os << "\n";
        }
        return os;
    }

    /// 2D rotation matrix
    class RotationMatrix : public Matrix
    {
    public:
        RotationMatrix(double theta) : Matrix(2, 2)
        {
            a[0][0] = cos(theta);
            a[0][1] = -sin(theta);
            a[1][0] = sin(theta);
            a[1][1] = cos(theta);
        }
    };

    /// 2D scaling matrix
    class ScalingMatrix : public Matrix
    {
    public:
        ScalingMatrix(double scale) : Matrix(2, 2)
        {
            a[0][0] = scale;
            a[0][1] = 0.0;
            a[1][0] = 0.0;
            a[1][1] = scale;
        }
    };

    /// 2D Translation matrix
    class TranslationMatrix : public Matrix
    {
    public:
        TranslationMatrix(double xShift, double yShift, int nCols) : Matrix(2, nCols)
        {
            for (int j = 0; j < nCols; ++j) {
                a[0][j] = xShift;
                a[1][j] = yShift;
            }
        }
    };
}

#endif // MATRIX_H_INCLUDED