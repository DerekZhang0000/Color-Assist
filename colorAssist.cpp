#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Color Assist Methods
bool isMonochrome(double ignoreMonochrome, double subColor1, double subColor2, double subColor3)
{
    if(abs(subColor1 - subColor2) / 255 <= ignoreMonochrome && abs(subColor2 - subColor3) / 255 <= ignoreMonochrome && abs(subColor3 - subColor1) / 255 <= ignoreMonochrome)
    {
        return true;
    }
    return false;
}
#define PIXEL pair<const uint, const uint>
void normFunction(Mat &image, double recolorFactor, PIXEL pixel, double ignoreMonochrome)
{
    Vec3b color = image.at<Vec3b>(Point(pixel.first, pixel.second));
    uchar red = color.val[2];       //Red
    uchar green = color.val[1];     //Green
    uchar blue = color.val[0];      //Blue
    if(ignoreMonochrome > 0 && ignoreMonochrome < 1)
    {
        if(isMonochrome(1 - ignoreMonochrome, red, green, blue))
        {
            return;
        }
    }
    uint rValue = round(red * (1 + recolorFactor));
    if(rValue > 255)
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
    const uint imageWidth = image.cols;
    const uint imageHeight = image.rows;
    for(uint i = 0; i < imageWidth; i++)
    {
        for(uint j = 0; j < imageHeight; j++)
        {
            normFunction(image, recolorFactor, PIXEL(i, j), ignoreMonochrome);
        }
    }
}
#undef PIXEL
//Data parsing methods
void split(string const &str, vector<string> &out)
{
    stringstream ss(str);
    string s;
    while(getline(ss, s, '?'))
    {
        replace(s.begin(), s.end(), '*', ' ');
        out.push_back(s);
    }
}
vector<string> parseData(vector<string> &data)
{   //download path, image path, boost factor, monochrome factor, resize?, width, height
    string str;
    cin >> str;
    split(str, data);
    return data;
}
int main()
{
    vector<string> data;
    parseData(data);
    try
    {
        Mat image = imread(data.at(1), IMREAD_COLOR);
        if(data.at(4) == "True")
        {
            resize(image, image, Size(stoi(data.at(5)), stoi(data.at(6))), INTER_LINEAR);
        }
        normalizeColors(image, stod(data.at(2)) / 100, stod(data.at(3)) / 100);
        imwrite(data.at(0), image);
    }
    catch (...) {}
    return 0;
}

