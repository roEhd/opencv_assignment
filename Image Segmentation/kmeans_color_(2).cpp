#include <iostream>
#include <opencv2/opencv.hpp>

#define IM_TYPE	CV_8UC3

using namespace cv;

int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat output;


	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Original", WINDOW_AUTOSIZE);
	imshow("Original", input);

	// Note that the intensity value is not normalized here (0~1). You should normalize both intensity and position when using them simultaneously.
	

	
	Mat normalized;
	input.convertTo(normalized, CV_32F, 1.0 / 255);
	Mat samples(normalized.rows * normalized.cols, 5, CV_32F); //color + position 5-d
	float sigma = 1;
	for (int y = 0; y < input.rows; y++) {
		for (int x = 0; x < input.cols; x++) {
			for (int z = 0; z < 3; z++)
				samples.at<float>(y + x * input.rows, z) = ((float)input.at<Vec3b>(y, x)[z]) / 255;
			samples.at<float>(y + x * input.rows, 3) = (float)y / (float)input.rows / sigma;
			samples.at<float>(y + x * input.rows, 4) = (float)x / (float)input.cols / sigma;

		}
	}

	int clusterCount = 60;
	Mat labels;
	int attempts = 5;
	Mat centers;

	kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);
	Mat new_image = Mat::zeros(input.size(), input.type());
	for (int y = 0; y < input.rows; y++) {
		for (int x = 0; x < input.cols; x++)
		{
			int cluster_idx = labels.at<int>(y + x * input.rows, 0);
			for (int z = 0; z < 3; z++)
				new_image.at<Vec3b>(y, x)[z] = (uchar)(centers.at<float>(cluster_idx, z) * 255);
		}
	}

	namedWindow("New Image", WINDOW_AUTOSIZE);
	imshow("New Image", new_image);

	waitKey(0);

	return 0;
}

