#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
#define PIXEL pair<const unsigned int, const unsigned int>

bool isMonochrome(double ignoreMonochrome, double subColor1, double subColor2, double subColor3)
{
    if (abs(subColor1 - subColor2) / 255 <= ignoreMonochrome && abs(subColor2 - subColor3) / 255 <= ignoreMonochrome && abs(subColor3 - subColor1) / 255 <= ignoreMonochrome)
    {
        return true;
    }
    return false;
}
void normFunction(Mat &image, double recolorFactor, PIXEL pixel, double ignoreMonochrome)
{
    Vec3b color = image.at<Vec3b>(Point(pixel.first, pixel.second));
    unsigned char red = color.val[2];       //Red
    unsigned char green = color.val[1];     //Green
    unsigned char blue = color.val[0];      //Blue
    if (ignoreMonochrome > 0 && ignoreMonochrome < 1)
    {
        if (isMonochrome(1 - ignoreMonochrome, red, green, blue))
        {
            return;
        }
    }
    unsigned int rValue = round(red * (1 + recolorFactor));
    if (rValue > 255)
    {
        int gValue = green - (rValue - 255);
        image.at<Vec3b>(Point(pixel.first, pixel.second))[2] = 255;
        if (gValue < 0)
        {
            int bValue = blue + gValue;
            image.at<Vec3b>(Point(pixel.first, pixel.second))[1] = 0;
            image.at<Vec3b>(Point(pixel.first, pixel.second))[0] = bValue < 0 ? 0 : bValue;
        }
        else
        {
            image.at<Vec3b>(Point(pixel.first, pixel.second))[1] = gValue;
        }
    }
    else
    {
        image.at<Vec3b>(Point(pixel.first, pixel.second))[2] = rValue;
    }
}
void normalizeColors(Mat &image, double recolorFactor=.5, double ignoreMonochrome=-1)
{
    const unsigned int imageWidth = image.cols;
    const unsigned int imageHeight = image.rows;
    for (unsigned int i = 0; i < imageWidth; i++)
    {
        for (unsigned int j = 0; j < imageHeight; j++)
        {
            normFunction(image, recolorFactor, PIXEL(i, j), ignoreMonochrome);
        }
    }
}
int main() {
    Mat image = imread("../../ImageIn/cbTests.jpg", IMREAD_COLOR);
    // resize(image, image, Size(1260, 700), INTER_LINEAR);
    normalizeColors(image, .35, .9);
    imshow("Output", image);
    // imwrite("../../ImageOut/cbTests.jpg", image);
    int i = waitKey(0);
    return 0;
}

#undef PIXEL