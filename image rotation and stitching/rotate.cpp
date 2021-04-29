#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

template <typename T>
Mat myrotate(const Mat input, float angle, const char* opt);

int main()
{
	Mat input, b_rotated, n_rotated;

	// Read each image
	input = imread("lena.jpg");

	// Check for invalid input
	if (!input.data) {
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	// original image
	namedWindow("image");
	imshow("image", input);

	//bilinear interpolation
	b_rotated = myrotate<Vec3b>(input, 45, "bilinear");
	namedWindow("b_rotated");
	imshow("b_rotated", b_rotated);

	//nearest interpolation
	n_rotated = myrotate<Vec3b>(input, 45, "nearest");
	namedWindow("n_rotated");
	imshow("n_rotated", n_rotated);

	waitKey(0);

	return 0;
}

template <typename T>
Mat myrotate(const Mat input, float angle, const char* opt) {
	int row = input.rows;
	int col = input.cols;

	float radian = angle * CV_PI / 180;
	
	float sq_col = ceil(abs(row * sin(radian)) + abs(col * cos(radian)));
	float sq_row = ceil(abs(col * sin(radian)) + abs(row * cos(radian)));

	Mat output = Mat::zeros(sq_row, sq_col, input.type());

	for (int i = 0; i < sq_row; i++) {
		for (int j = 0; j < sq_col; j++) {

			float x = (j - sq_col / 2) * cos(radian) - (i - sq_row / 2) * sin(radian) + col / 2;
			float y = (j - sq_col / 2) * sin(radian) + (i - sq_row / 2) * cos(radian) + row / 2;

			if ((y >= 0) && (y <= (row - 1)) && (x >= 0) && (x <= (col - 1))) {
				if (!strcmp(opt, "nearest")) {
					output.at<Vec3b>(i, j) = input.at<Vec3b>(round(y), round(x));
				}
				else if (!strcmp(opt, "bilinear")) {
					//bilinear interpolation				

					float x1 = floor(x);
					float x2 = ceil(x);
					float y1 = floor(y);
					float y2 = ceil(y);				

					double lambda = x - x1;
					double mu = y - y1;

					//	bilinear interpolation
					Vec3b pixel1 = (1 - lambda) * input.at<Vec3b>(y1, x1) + lambda * input.at<Vec3b>(y1, x2);
					Vec3b pixel2 = (1 - lambda) * input.at<Vec3b>(y2, x1) + lambda * input.at<Vec3b>(y2, x2);
					Vec3b pixel3 = (1 - mu) * pixel1 + mu * pixel2;
				
					output.at < Vec3b >(i, j) = pixel3;


				}
			}
		}
	}//end of for

	return output;
}
