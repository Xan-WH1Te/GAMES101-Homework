#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
    //平移到原点
    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    model << cos(rotation_angle / 180 * MY_PI), -sin(rotation_angle / 180 * MY_PI), 0, 0,
        sin(rotation_angle / 180 * MY_PI), cos(rotation_angle / 180 * MY_PI), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1;
    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.

    return model;
}

//TODO: Implement a function
//Rotate by any axis
//在 main.cpp 中构造一个函数，该函数的作用是得到绕任意
//过原点的轴的旋转变换矩阵。
Eigen::Matrix4f get_rotation(Eigen::Vector3f axis, float angle)
{
    angle = angle / 180.0 * MY_PI;
    axis.normalize();

    Eigen::Matrix3f N_star;
    N_star << 0,       -axis.z(),  axis.y(),
              axis.z(),  0,        -axis.x(),
             -axis.y(),  axis.x(),  0;

    Eigen::Matrix3f I = Eigen::Matrix3f::Identity();
    Eigen::Matrix3f R3 = std::cos(angle) * I
                       + (1 - std::cos(angle)) * axis * axis.transpose()
                       + std::sin(angle) * N_star;

    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    model.block<3, 3>(0, 0) = R3;
    return model;
}
Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    float t = zNear * tan(eye_fov / 2 / 180 * MY_PI);
    float r = t * aspect_ratio;

    projection << zNear / r, 0, 0, 0,
                    0, zNear / t, 0, 0,
                    0, 0, -(zNear + zFar) / (zFar - zNear), -2 * zNear * zFar / (zFar - zNear),
                    0, 0, -1, 0;
    
    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    return projection;
}

int main(int argc, const char** argv)
{
    // [5 分 提高项] 测试绕任意轴旋转
    // 测试1: 绕 Z 轴应该等于 get_model_matrix
    Eigen::Matrix4f Rz = get_rotation(Eigen::Vector3f(0, 0, 1), 45);
    Eigen::Matrix4f Mz = get_model_matrix(45);
    std::cout << "绕 Z 轴 45°  vs model: " << (Rz.isApprox(Mz, 1e-5f) ? "PASS" : "FAIL") << std::endl;
    // 测试2: 绕 X 轴旋转 90°，点 (0,1,0) → (0,0,1)
    Eigen::Matrix4f Rx = get_rotation(Eigen::Vector3f(1, 0, 0), 90);
    Eigen::Vector3f p = Rx.block<3,3>(0,0) * Eigen::Vector3f(0, 1, 0);
    std::cout << "绕 X 轴 90° (0,1,0)→(0,0,1): "
              << (p.isApprox(Eigen::Vector3f(0, 0, 1), 1e-5f) ? "PASS" : "FAIL")
              << "  got (" << p.transpose() << ")" << std::endl;

    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    // 用倾斜轴 (1, 1, 0) 演示绕任意轴旋转
    Eigen::Vector3f rot_axis(1, 1, 0);

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_rotation(rot_axis, angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    std::cout << "Rotation axis: (1, 1, 0), A/D to change angle, Esc to quit" << std::endl;

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_rotation(rot_axis, angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
