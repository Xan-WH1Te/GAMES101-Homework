#include <algorithm>
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

std::vector<cv::Point2f> control_points;

void mouse_handler(int event, int x, int y, int flags, void *userdata) 
{
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < 4) 
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
        << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }     
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) 
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t) 
{
    // TODO: Implement de Casteljau's algorithm
    if (t > 1 || t < 0)
    {
        std::cout << "t should be [0,1]" << std::endl;
        return cv::Point2f(0,0);
    }
    int size = control_points.size();
    if (size == 1)
    {
        return control_points[0];
    }
    std::vector<cv::Point2f> p;
    for (int i = 0; i < size - 1; i++) {
        p.push_back((1 - t) * control_points[i] + t * control_points[i + 1]);
    }
    return recursive_bezier(p, t);
}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window)
{
    for (double t = 0.0; t <= 1.0; t += 0.001) {
        cv::Point2f pt = recursive_bezier(control_points, t);
        int x0 = std::max(0, std::min(int(pt.x), window.cols - 1));
        int y0 = std::max(0, std::min(int(pt.y), window.rows - 1));
        int x1 = std::max(0, std::min(x0 + 1, window.cols - 1));
        int y1 = std::max(0, std::min(y0 + 1, window.rows - 1));
        float px = pt.x - x0;
        float py = pt.y - y0;

        float d00 = sqrt(px * px + py * py);
        float d10 = sqrt((1 - px) * (1 - px) + py * py);
        float d01 = sqrt(px * px + (1 - py) * (1 - py));
        float d11 = sqrt((1 - px) * (1 - px) + (1 - py) * (1 - py));

        auto add_color = [&](int y, int x, float d) {
            float c = window.at<cv::Vec3b>(y, x)[1] + 255.0f * std::max(0.0f, 1.0f - d);
            window.at<cv::Vec3b>(y, x)[1] = cv::saturate_cast<uchar>(c);
        };

        add_color(y0, x0, d00);
        add_color(y0, x1, d10);
        add_color(y1, x0, d01);
        add_color(y1, x1, d11);
    }
}

int main() 
{
    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    
    while (key != 27) 
    {
        for (auto &point : control_points) 
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }

        if (control_points.size() == 4) 
        {
            //naive_bezier(control_points, window);
            bezier(control_points, window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

return 0;
}
