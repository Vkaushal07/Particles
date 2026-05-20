#ifndef MATRICES_H
#define MATRICES_H

#include <vector>
#include <iostream>
#include <stdexcept>
#include <cmath>

class Matrix {
public:
    Matrix(int r, int c) : rows(r), cols(c), data(r, std::vector<double>(c, 0.0)) {}
    virtual ~Matrix() = default;

    double& operator()(int r, int c) { return data.at(r).at(c); }
    const double& operator()(int r, int c) const { return data.at(r).at(c); }

    int getRows() const { return rows; }
    int getCols() const { return cols; }

    friend Matrix operator+(const Matrix& lhs, const Matrix& rhs) {
        if (lhs.rows != rhs.rows || lhs.cols != rhs.cols) {
            throw std::runtime_error("Matrix dimensions must match for addition.");
        }
        Matrix result(lhs.rows, lhs.cols);
        for (int i = 0; i < lhs.rows; ++i) {
            for (int j = 0; j < lhs.cols; ++j) {
                result.data[i][j] = lhs.data[i][j] + rhs.data[i][j];
            }
        }
        return result;
    }

    friend Matrix operator*(const Matrix& lhs, const Matrix& rhs) {
        if (lhs.cols != rhs.rows) {
            throw std::runtime_error("Matrix dimensions mismatched for multiplication.");
        }
        Matrix result(lhs.rows, rhs.cols);
        for (int i = 0; i < lhs.rows; ++i) {
            for (int j = 0; j < rhs.cols; ++j) {
                for (int k = 0; k < lhs.cols; ++k) {
                    result.data[i][j] += lhs.data[i][k] * rhs.data[k][j];
                }
            }
        }
        return result;
    }

protected:
    int rows;
    int cols;
    std::vector<std::vector<double>> data;
};

class RotationMatrix : public Matrix {
public:
    RotationMatrix(double theta) : Matrix(2, 2) {
        data[0][0] = std::cos(theta);
        data[0][1] = -std::sin(theta);
        data[1][0] = std::sin(theta);
        data[1][1] = std::cos(theta);
    }
};

class ScalingMatrix : public Matrix {
public:
    ScalingMatrix(double c) : Matrix(2, 2) {
        data[0][0] = c;
        data[0][1] = 0.0;
        data[1][0] = 0.0;
        data[1][1] = c;
    }
};

class TranslationMatrix : public Matrix {
public:
    TranslationMatrix(double xShift, double yShift, int nCols) : Matrix(2, nCols) {
        for (int j = 0; j < nCols; ++j) {
            data[0][j] = xShift;
            data[1][j] = yShift;
        }
    }
};

#endif