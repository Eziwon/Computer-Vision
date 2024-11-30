#include "opencv2/opencv.hpp"
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

Mat quantization_mat1 = (Mat_<double>(8, 8) <<
16, 11, 10, 16, 24, 40, 51, 61,
12, 12, 14, 19, 26, 58, 60, 55,
14, 13, 16, 24, 40, 57, 69, 56,
14, 17, 22, 29, 51, 87, 80, 62,
18, 22, 37, 56, 68, 109, 103, 77,
24, 35, 55, 64, 81, 104, 113, 92,
49, 64, 78, 87, 103, 121, 120, 101,
72, 92, 95, 98, 112, 100, 103, 99
);

Mat quantization_mat2 = (Mat_<double>(8, 8) <<
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1
);

Mat quantization_mat3 = (Mat_<double>(8, 8) <<
100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100
);

// Function to perform DCT and quantization
Mat processBlock(const Mat& block, const Mat& quant_mat) {
    Mat dct_block, quantized_block, idct_block;

    // Apply DCT
    dct(block, dct_block, 0);

    // Initialize quantized_block
    quantized_block = Mat::zeros(dct_block.size(), CV_64F);

    // Apply Quatization
    for (int j = 0; j < dct_block.rows; j++) {
        for (int i = 0; i < dct_block.cols; i++) {
            quantized_block.at<double>(j, i) = round(dct_block.at<double>(j, i) / quant_mat.at<double>(j, i));
        }
    }

    // Dequantization
    for (int j = 0; j < quantized_block.rows; j++) {
        for (int i = 0; i < quantized_block.cols; i++) {
            quantized_block.at<double>(j, i) = quantized_block.at<double>(j, i) * quant_mat.at<double>(j, i);
        }
    }

    // Inverse DCT
    dct(quantized_block, idct_block, 1);
    return idct_block;
}

// Function to compute PSNR
double calculatePSNR(const Mat& original, const Mat& compressed) {
    Mat diff;
    absdiff(original, compressed, diff);
    diff.convertTo(diff, CV_64F);

    for (int j = 0; j < diff.rows; j++) {
        for (int i = 0; i < diff.cols; i++) {
            diff.at<double>(j, i) = diff.at<double>(j, i) * diff.at<double>(j, i);
        }
    }

    double total_sum = 0.0;
    for (int j = 0; j < diff.rows; j++) {
        for (int i = 0; i < diff.cols; i++) {
            total_sum += diff.at<double>(j, i);
        }
    }

    double mse = total_sum / (double)(diff.rows * diff.cols);
    double psnr = 10.0 * log10(255.0 * 255.0 / mse);
    return psnr;
}

int main () {
    Mat image, image_Ycbcr;
    Mat Ycbcr_channels[3];

    image = imread("lena.png");
    cvtColor(image, image_Ycbcr, COLOR_BGR2YCrCb);
    split(image_Ycbcr, Ycbcr_channels);

    Mat y = Ycbcr_channels[0].clone();

    Mat restored_Y1 = y.clone(), restored_Y2 = y.clone(), restored_Y3 = y.clone();

    for (int j = 0; j < y.rows; j += 8) {
        for (int i = 0; i < y.cols; i += 8) {
            Rect blockRect(i, j, 8, 8);
            Mat block = y(blockRect);
            block.convertTo(block, CV_64F);

            Mat restored_block1 = processBlock(block, quantization_mat1);
            Mat restored_block2 = processBlock(block, quantization_mat2);
            Mat restored_block3 = processBlock(block, quantization_mat3);

            restored_block1.convertTo(restored_block1, CV_8U);
            restored_block2.convertTo(restored_block2, CV_8U);
            restored_block3.convertTo(restored_block3, CV_8U);

            restored_block1.copyTo(restored_Y1(blockRect));
            restored_block2.copyTo(restored_Y2(blockRect));
            restored_block3.copyTo(restored_Y3(blockRect));
        }
    }

    double psnr1 = calculatePSNR(y, restored_Y1);
    double psnr2 = calculatePSNR(y, restored_Y2);
    double psnr3 = calculatePSNR(y, restored_Y3);

    printf("QM1: psnr = %.6f\n", psnr1);
    printf("QM2: psnr = %.6f\n", psnr2);
    printf("QM3: psnr = %.6f\n", psnr3);

    imshow("Original Y", y);
    imshow("QM1", restored_Y1);
    imshow("QM2", restored_Y2);
    imshow("QM3", restored_Y3);

    waitKey(0);
    return 0;
}