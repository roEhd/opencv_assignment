#include <iostream>
#include <opencv2/opencv.hpp>

#define IM_TYPE	CV_8UC3

using namespace cv;

int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat output;
	Mat input_gray;
	cvtColor(input, input_gray, CV_RGB2GRAY);

	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Original", WINDOW_AUTOSIZE);
	imshow("Original", input_gray);

	// Note that the intensity value is not normalized here (0~1). You should normalize both intensity and position when using them simultaneously.
	

	Mat normalized;
	input_gray.convertTo(normalized, CV_32F, 1.0 / 255);
	Mat samples(normalized.rows * normalized.cols, 3, CV_32F); //intensity + position
	float sigma = 50;
	for (int y = 0; y < normalized.rows; y++) {
		for (int x = 0; x < normalized.cols; x++) {
			samples.at<float>(y + x * input_gray.rows, 0) = (float)normalized.at<float>(y, x);
			samples.at<float>(y + x * input_gray.rows, 1) = (float)y / (float)normalized.rows / sigma;
			samples.at<float>(y + x * input_gray.rows, 2) = (float)x / (float)normalized.cols / sigma;
		}
	}

	// Clustering is performed

	int clusterCount = 10;
	Mat labels;
	int attempts = 5;
	Mat centers;
	kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);

	Mat new_image(input_gray.size(), input_gray.type());
	for (int y = 0; y < input_gray.rows; y++)
		for (int x = 0; x < input_gray.cols; x++)
		{
			int cluster_idx = labels.at<int>(y + x * normalized.rows, 0);
			new_image.at<uchar>(y, x) = (uchar)(centers.at<float>(cluster_idx, 0) * 255);
					
		}
	imshow("clustered image", new_image);

	waitKey(0);

	return 0;
}
