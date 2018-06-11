/*
 * author: LogM
 * date: 2018-05-22
 * update: 2018-06-11
 * information: A multi-seed region growing algorithm.
 *              In order to compile, you need to change "OpenCV_DIR", "OpenCV_LIB_DIR", "OpenCV_INCLUDE_DIRS" in "CMakeList.txt".
 * TODO: use more powerful estimating algorithm, instead of "delta and threshold"
 * TODO: optimize the efficiency
 */

#include <iostream>
#include <stack>
#include <cassert>
#include <opencv2/opencv.hpp>

using std::cout;
using std::endl;
using std::stack;

void grow(cv::Mat& src, cv::Mat& dest, cv::Mat& mask, cv::Point seed, int threshold);

// parameters
const int threshold = 200;
const uchar max_region_num = 100;
const double min_region_area_factor = 0.01;
const cv::Point PointShift2D[8] =
{
    cv::Point(1, 0),
    cv::Point(1, -1),
    cv::Point(0, -1),
    cv::Point(-1, -1),
    cv::Point(-1, 0),
    cv::Point(-1, 1),
    cv::Point(0, 1),
    cv::Point(1, 1)
};


int main() {
    // 1. read source image
    cv::Mat src = cv::imread("./img/3.jpg");
    assert(!src.empty());
    if (src.cols > 500 || src.rows > 500) {
        cv::resize(src, src, cv::Size(0, 0), 0.5, 0.5); // resize for speed
    }
    cv::namedWindow("src", CV_WINDOW_NORMAL);
    cv::imshow("src", src);

    // 2. convert to grey (now it's unnecessary)
//    cv::Mat src;
//    cv::cvtColor(src, src_grey, CV_BGR2GRAY);
//    cv::namedWindow("grey", CV_WINDOW_NORMAL);
//    cv::imshow("grey", src_grey);

    // 3. ready for seed grow
    int min_region_area = int(min_region_area_factor * src.cols * src.rows);  // small region will be ignored
    cv::namedWindow("mask", CV_WINDOW_NORMAL);

    // "dest" records all regions using different padding number
    // 0 - undetermined, 255 - ignored, other number - determined
    uchar padding = 1;  // use which number to pad in "dest"
    cv::Mat dest = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

    // "mask" records current region, always use "1" for padding
    cv::Mat mask = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

    // 4. traversal the whole image, apply "seed grow" in undetermined pixels
    for (int x=0; x<src.cols; ++x) {
        for (int y=0; y<src.rows; ++y) {
            if (dest.at<uchar>(cv::Point(x, y)) == 0) {
                grow(src, dest, mask, cv::Point(x, y), threshold);

                int mask_area = (int)cv::sum(mask).val[0];  // calculate area of the region that we get in "seed grow"
                if (mask_area > min_region_area) {
                    dest = dest + mask * padding;   // record new region to "dest"
                    cv::imshow("mask", mask*255);
                    cv::waitKey();
                    if(++padding > max_region_num) { printf("run out of max_region_num..."); return -1; }
                } else {
                    dest = dest + mask * 255;   // record as "ignored"
                }
                mask = mask - mask;     // zero mask, ready for next "seed grow"
            }
        }
    }
    printf("Mission complete...");
    return 0;
}

void grow(cv::Mat& src, cv::Mat& dest, cv::Mat& mask, cv::Point seed, int threshold) {
    /* apply "seed grow" in a given seed
     * Params:
     *   src: source image
     *   dest: a matrix records which pixels are determined/undtermined/ignored
     *   mask: a matrix records the region found in current "seed grow"
     */
    stack<cv::Point> point_stack;
    point_stack.push(seed);

    while(!point_stack.empty()) {
        cv::Point center = point_stack.top();
        mask.at<uchar>(center) = 1;
        point_stack.pop();

        for (int i=0; i<8; ++i) {
            cv::Point estimating_point = center + PointShift2D[i];
            if (estimating_point.x < 0
                || estimating_point.x > src.cols-1
                || estimating_point.y < 0
                || estimating_point.y > src.rows-1) {
                // estimating_point should not out of the range in image
                continue;
            } else {
//                uchar delta = (uchar)abs(src.at<uchar>(center) - src.at<uchar>(estimating_point));
                // delta = (R-R')^2 + (G-G')^2 + (B-B')^2
                int delta = int(pow(src.at<cv::Vec3b>(center)[0] - src.at<cv::Vec3b>(estimating_point)[0], 2)
                                + pow(src.at<cv::Vec3b>(center)[1] - src.at<cv::Vec3b>(estimating_point)[1], 2)
                                + pow(src.at<cv::Vec3b>(center)[2] - src.at<cv::Vec3b>(estimating_point)[2], 2));
                if (dest.at<uchar>(estimating_point) == 0
                    && mask.at<uchar>(estimating_point) == 0
                    && delta < threshold) {
                    mask.at<uchar>(estimating_point) = 1;
                    point_stack.push(estimating_point);
                }
            }
        }
    }
}
