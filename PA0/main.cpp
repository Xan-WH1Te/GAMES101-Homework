#include <eigen3/Eigen/Core>
#include <cmath>
#include <iostream>
int main() {
    Eigen::Vector3f p(2.0f, 1.0f, 1.0f);

    float angle = 45.0f / 180.0f * M_PI;

    Eigen::Matrix3f rotate;
    rotate << std::cos(angle), -std::sin(angle), 0,
              std::sin(angle),  std::cos(angle), 0,
               0,                0,               1;

    Eigen::Matrix3f translate;
    translate << 1, 0, 1,
                 0, 1, 2,
                 0, 0, 1;

    Eigen::Vector3f result = translate * rotate * p;

    std::cout << result << std::endl;
    return 0;
}

