//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        auto u_img = std::clamp(int(u * width), 0, width - 1);
        auto v_img = std::clamp(int((1 - v) * height), 0, height - 1);
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

    Eigen::Vector3f getColorBilinear(float u, float v)
    {

        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        int u0 = std::clamp(int(std::floor(u_img)), 0, width - 1);
        int u1 = std::clamp(u0 + 1, 0, width - 1);
        int v0 = std::clamp(int(std::floor(v_img)), 0, height - 1);
        int v1 = std::clamp(v0 + 1, 0, height - 1);
        float s = u_img - std::floor(u_img);
        float t = v_img - std::floor(v_img);

        auto p00 = image_data.at<cv::Vec3b>(v0, u0);
        Eigen::Vector3f c00(p00[0], p00[1], p00[2]);
            
        auto p01 = image_data.at<cv::Vec3b>(v1, u0);
        Eigen::Vector3f c01(p01[0], p01[1], p01[2]);
        auto p10 = image_data.at<cv::Vec3b>(v0, u1);
        Eigen::Vector3f c10(p10[0], p10[1], p10[2]);
        auto p11 = image_data.at<cv::Vec3b>(v1, u1);

        Eigen::Vector3f c11(p11[0], p11[1], p11[2]);
        Eigen::Vector3f c0 = c00 + s * (c10 - c00);
        Eigen::Vector3f c1 = c01 + s * (c11 - c01);
        Eigen::Vector3f c2 = c0 + t * (c1 - c0);

        return Eigen::Vector3f(c2[0], c2[1], c2[2]);
        
    }
};
#endif //RASTERIZER_TEXTURE_H
