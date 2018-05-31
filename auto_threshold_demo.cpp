/*
 * author: LogM
 * date: 2018-05-30
 * information: A demo shows how to make program find best threshold automatically. 
 *              It's an extension program for "multi-seed region grow" algorithm in "main.cpp".
 *              In order to run this program, you need add this cpp file to "CMakeLists.txt".
 */

#include <iostream>
#include <stack>
#include <opencv2/opencv.hpp>

using std::cout;
using std::endl;
using std::stack;

void grow(cv::Mat& src, cv::Mat& dest, cv::Mat& mask, cv::Point seed, int threshold);
double seed_grow_algorithm(cv::Mat& src, cv::Mat& dest, int threshold);
double calculate_mean_and_var(cv::Mat& img, cv::Mat& mask, int* mean, int region_count);

// parameters
//const uchar threshold = 50;
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
    cv::Mat src = cv::imread("./img/11.jpg");
    if(src.empty()) { printf("Invalid input image..."); return -1; }
    cv::namedWindow("src", CV_WINDOW_NORMAL);
    cv::imshow("src", src);

    // 2. convert to grey (now it's unnecessary)
//    cv::Mat src;
//    cv::cvtColor(src, src, CV_BGR2GRAY);
//    cv::namedWindow("grey", CV_WINDOW_NORMAL);
//    cv::imshow("grey", src);

    // 3. a loop to find best threshold
    cv::Mat dest_temp = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
    cv::Mat dest = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

    double min_loss = 1e9;
    int best_threshold = 0;
    for (int threshold = 50; threshold < 800; threshold+=50) {
        printf("processing threshold: %d\n", threshold);

        dest_temp = dest_temp - dest_temp;  // zero "dest_temp"
        double loss = seed_grow_algorithm(src, dest_temp, threshold);
        printf("Score = %f\n", loss);
        if (loss < min_loss) {
            min_loss = loss;
            dest = dest_temp.clone();
            best_threshold = threshold;
        }
    }

    // 4. show result, use different color for different regions
    cv::Mat result_image = cv::Mat::zeros(src.rows, src.cols, CV_8UC3);
    for (int x=0; x<dest.cols; ++x) {
        for (int y = 0; y < dest.rows; ++y) {
            if (dest.at<uchar>(cv::Point(x, y)) == 1) {
                result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 255;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 0;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 0;
            } else if (dest.at<uchar>(cv::Point(x, y)) == 2) {
                result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 0;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 255;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 0;
            } else if (dest.at<uchar>(cv::Point(x, y)) == 3) {
                result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 0;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 0;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 255;
            } else if (dest.at<uchar>(cv::Point(x, y)) == 4) {
                result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 255;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 0;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 255;
            } else if (dest.at<uchar>(cv::Point(x, y)) == 5) {
                result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 0;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 255;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 255;
            } else if (dest.at<uchar>(cv::Point(x, y)) == 6) {
                result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 255;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 255;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 0;
            } else if (dest.at<uchar>(cv::Point(x, y)) == 7) {
                result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 120;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 120;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 0;
            } else if (dest.at<uchar>(cv::Point(x, y)) == 8) {
                result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 0;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 120;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 120;
            } else if (dest.at<uchar>(cv::Point(x, y)) == 9) {
                result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 120;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 0;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 120;
            } else if (dest.at<uchar>(cv::Point(x, y)) == 10) {
                result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 120;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 120;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 120;
            } else if (dest.at<uchar>(cv::Point(x, y)) == 11) {
                result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 120;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 0;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 0;
            } else if (dest.at<uchar>(cv::Point(x, y)) == 12) {
                result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 0;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 120;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 0;
            } else if (dest.at<uchar>(cv::Point(x, y)) == 13) {
                result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 0;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 0;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 120;
            } else if (dest.at<uchar>(cv::Point(x, y)) == 255) {
                result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 20;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 20;
                result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 20;
            }
        }
    }

    printf("Mission complete...\n");
    printf("best threshold: %d\n", best_threshold);
    cv::namedWindow("result", CV_WINDOW_NORMAL);
    cv::imshow("result", result_image);
    cv::waitKey();

    return 0;
}

double calculate_mean_and_var(cv::Mat& img, cv::Mat& mask, int* mean, int region_count) {
    /* calculate "mask region" mean and var
     * params:
     *      img: source image
     *      mask: a matrix records the region found in current "seed grow", filled with 0 and 1
     *      mean: an array record all regions' mean
     *      region_count: show the mask is belong to which region
     * return:
     *      var: the var of the current region
     * */
    int count = 0;
    int sum = 0;

    for (int x=0; x<mask.cols; ++x) {
        for (int y=0; y<mask.rows; ++y) {
            if (mask.at<uchar>(cv::Point(x, y)) == 1) {
                ++count;
                sum += int(img.at<uchar>(cv::Point(x, y)));
            }
        }
    }

    mean[region_count] = sum / count;

    count = 0;  sum = 0;
    for (int x=0; x<mask.cols; ++x) {
        for (int y=0; y<mask.rows; ++y) {
            if (mask.at<uchar>(cv::Point(x, y)) == 1) {
                ++count;
                sum += pow(int(img.at<uchar>(cv::Point(x, y))) - mean[region_count], 2);
            }
        }
    }

    double var = 1.0 * sum / count;

    return var;
}

double seed_grow_algorithm(cv::Mat& src, cv::Mat& dest, int threshold) {
    /* apply "multi-seed region grow" algorithm in a fixed threshold
     * params:
     *      src: source image
     *      dest: a matrix records which pixels are determined/undtermined/ignored,
     *            0 filled when algorithm begin, and other numbers filled when algorithm end
     *      threshold: a fixed threshold
     * return:
     *      loss: 1.0 / (different-regions' delta-mean) + (same-regions' var)
     * */

    // 1. ready for seed grow
    int min_region_area = int(min_region_area_factor * src.cols * src.rows);  // small region will be ignored
//    cv::namedWindow("mask", CV_WINDOW_NORMAL);

    // "dest" records all regions using different padding number
    // 0 - undetermined, 255 - ignored, other number - determined
    uchar padding = 1;  // use which number to pad in "dest"

    // "mask" records current region, always use "1" for padding
    cv::Mat mask = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

    double var_sum = 0;
    int region_count = 0;
    int mean[100]={0};
    // 2. traversal the whole image, apply "seed grow" in undetermined pixels
    for (int x=0; x<src.cols; ++x) {
        for (int y=0; y<src.rows; ++y) {
            if (dest.at<uchar>(cv::Point(x, y)) == 0) {
                grow(src, dest, mask, cv::Point(x, y), threshold);

                int mask_area = (int)cv::sum(mask).val[0];  // calculate area of the region that we get in "seed grow"
                if (mask_area > min_region_area) {
                    var_sum += calculate_mean_and_var(src, mask, mean, region_count);   // record "same regions' var"
                    ++region_count;

                    dest = dest + mask * padding;   // record new region to "dest"
//                    cv::imshow("mask", mask*255);
//                    cv::waitKey();
                    if(++padding > max_region_num) { printf("run out of max_region_num...\n"); return -1; }
                } else {
                    dest = dest + mask * 255;   // record as "ignored"
                }
                mask = mask - mask;     // zero mask, ready for next "seed grow"
            }
        }
    }

    // 3.calculate loss
    int delta_mean_sum = 0;
    int delta_mean_count = 0;
    for (int i=0; i<region_count; ++i) {
        for (int j=i+1; j<region_count; ++j) {
            ++delta_mean_count;
            delta_mean_sum += abs(mean[i] - mean[j]);   // "different regions' delta-mean"
        }
    }
    printf("average var: %f\n", 1.0 * var_sum / region_count);
    printf("average sum: %f\n", 1.0 * delta_mean_sum / delta_mean_count);
    double loss = 1.0 / (1.0 * delta_mean_sum / delta_mean_count) + 0.0 * (1.0 * var_sum / region_count);
    return loss;
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