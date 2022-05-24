#include <opencv2/opencv.hpp>

void normFunction(cv::Mat &image, double &recolorFactor, double &threshold, unsigned int i, unsigned int j, bool ignoreMonochrome)
{
    cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(i, j));
    int red = color.val[2]; //Red
    int green = color.val[1];   //Green
    int blue = color.val[0];    //Blue
    if (ignoreMonochrome == true)
    {
        if (red == green && green == blue)
        {
            return;
        }
    }
    if (abs(red - green) / 255 < threshold)
    {
        double rValue = red * (1 + recolorFactor);
        if (rValue > 255.0)
        {
            image.at<cv::Vec3b>(cv::Point(i, j))[2] = (int)rValue;
            image.at<cv::Vec3b>(cv::Point(i, j))[1] = (int)(green - 255 * pow(((rValue - 255.0) / 255.0), 2.0));
        }
        else
        {
            image.at<cv::Vec3b>(cv::Point(i, j))[2] = (int)rValue;
        }
    }
}
void grayFunction(cv::Mat &image, unsigned int i, unsigned int j)
{
    cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(i, j));
    int red = color.val[2]; //Red
    int green = color.val[1];   //Green
    int blue = color.val[0];    //Blue
    if (red > green)
    {
        image.at<cv::Vec3b>(cv::Point(i, j))[2] = 0;
        image.at<cv::Vec3b>(cv::Point(i, j))[1] = 0;
        image.at<cv::Vec3b>(cv::Point(i, j))[0] = 0;
    }
    else if (green > red)
    {
        image.at<cv::Vec3b>(cv::Point(i, j))[2] = 200;
        image.at<cv::Vec3b>(cv::Point(i, j))[1] = 200;
        image.at<cv::Vec3b>(cv::Point(i, j))[0] = 200;
    }
    else if (!(red == green && green == blue))
    {
        image.at<cv::Vec3b>(cv::Point(i, j))[2] = 100;
        image.at<cv::Vec3b>(cv::Point(i, j))[1] = 100;
        image.at<cv::Vec3b>(cv::Point(i, j))[0] = 100;
    }
}
void normalizeColors(cv::Mat &image, double recolorFactor, double threshold, bool ignoreMonochrome = false)
{
    assert(image.cols > 0 && recolorFactor >= 0 && threshold >= 0);
    int imageWidth = image.cols;
    int imageHeight = image.rows;
    for (int i = 0; i < imageWidth; i++)
    {
        for (int j = 0; j < imageHeight; j++)
        {
            normFunction(image, recolorFactor, threshold, i, j, ignoreMonochrome);
            // grayFunction(image, i, j);
        }
    }
}
int main() {

    cv::Mat image = cv::imread("../../ImageIn/cbTests.jpg", cv::IMREAD_COLOR);
    normalizeColors(image, .50, .5, false);
    cv::imshow("Output", image);
    cv::imwrite("../../ImageOut/cbTests.jpg", image);
    int i = cv::waitKey(0);
    return 0;
}