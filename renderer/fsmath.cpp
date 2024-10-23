#include "fsmath.h"


Matrix::Matrix() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            cols[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

Matrix Matrix::operator*(const Matrix &p_other) const {
    Matrix m;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m.cols[i][j] = cols[i][0] * p_other.cols[0][j] +
                            cols[i][1] * p_other.cols[1][j] +
                            cols[i][2] * p_other.cols[2][j] +
                            cols[i][3] * p_other.cols[3][j];
        }
    }

    return m;
}


Matrix Matrix::ortho(float p_left, float p_right, float p_bottom, float p_top) {
    Matrix m;
    m.cols[0][0] = 2.0f / (p_right - p_left);
    m.cols[1][1] = 2.0f / (p_top - p_bottom);
    m.cols[3][0] = -(p_right + p_left) / (p_right - p_left);
    m.cols[3][1] = -(p_top + p_bottom) / (p_top - p_bottom);
    
    return m;
}

Matrix Matrix::translate(float p_x, float p_y) {
    Matrix m;
    m.cols[3][0] = p_x;
    m.cols[3][1] = p_y;
    
    return m;
}