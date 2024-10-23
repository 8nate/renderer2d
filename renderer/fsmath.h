#ifndef FS_MATH_H
#define FS_MATH_H

struct Matrix {
  float cols[4][4];
  
  Matrix();
  Matrix operator*(const Matrix &p_other) const;

public:
  static Matrix ortho(float p_left, float p_right, float p_bottom, float p_top);
  static Matrix translate(float p_x, float p_y);
};

#endif // FS_MATH_H