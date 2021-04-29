#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>       /* exp */
#define IM_TYPE	CV_8UC3

using namespace cv;

// Image Type
// "G" for GrayScale Image, "C" for Color Image
#if (IM_TYPE == CV_8UC3)
typedef uchar G;
typedef cv::Vec3b C;
#elif (IM_TYPE == CV_16SC3)
typedef short G;
typedef Vec3s C;
#elif (IM_TYPE == CV_32SC3)
typedef int G;
typedef Vec3i C;
#elif (IM_TYPE == CV_32FC3)
typedef float G;
typedef Vec3f C;
#elif (IM_TYPE == CV_64FC3)
typedef double G;
typedef Vec3d C;
#endif

Mat sobelfilter(const Mat input);
Mat laplacianfilter(const Mat input);

int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input_gray;
	Mat output;


	cvtColor(input, input_gray, CV_RGB2GRAY);



	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);
	output = sobelfilter(input_gray);

	namedWindow("Sobel Filter", WINDOW_AUTOSIZE);
	imshow("Sobel Filter", output);

	output = laplacianfilter(input_gray);

	namedWindow("Laplacian Filter", WINDOW_AUTOSIZE);
	imshow("Laplacian Filter", output);

	waitKey(0);

	return 0;
}


Mat sobelfilter(const Mat input) {

	
	int row = input.rows;
	int col = input.cols;
	int n = 1; // Sobel Filter Kernel N

	// Initialiazing 2 Kernel Matrix with 3x3 size for Sx and Sy
	//Fill code to initialize Sobel filter kernel matrix for Sx and Sy (Given in the lecture notes)
	int kernel_x[3][3] = { {-1,0,1},{-2,0,2},{-1,0,1} };
	int kernel_y[3][3] = { {-1,-2,-1},{0,0,0},{1,2,1} };

	Mat output = Mat::zeros(row, col, input.type());
	

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {

			int tempa, tempb;
			float sumx = 0.0, sumy = 0.0;
			float M = 0.0;

			for (int a = -n; a <= n; a++) {
				for (int b = -n; b <= n; b++) {
					// Use mirroring boundary process
					
					if (i + a > row - 1) {  
						tempa = i - a;
					}
					else if (i + a < 0) {
						tempa = -(i + a);
					}
					else {
						tempa = i + a;
					}
					if (j + b > col - 1) {
						tempb = j - b;
					}
					else if (j + b < 0) {
						tempb = -(j + b);
					}
					else {
						tempb = j + b;
					}
					sumx += (float)kernel_x[a + n][b + n] * (float)(input.at<G>(tempa, tempb));
					sumy += (float)kernel_y[a + n][b + n] * (float)(input.at<G>(tempa, tempb));

				}
			}
			// Find output M(x,y) = sqrt( input.at<G>(x, y)*Sx + input.at<G>(x, y)*Sy ) 
			M = sqrt(pow(sumx, 2) + pow(sumy, 2)); 	

			if (M >= 255) M = 255;
			output.at<G>(i, j) = (G)M;
		}
	}
	return output;
}

Mat laplacianfilter(const Mat input) {


	int row = input.rows;
	int col = input.cols;
	int n = 1; // Laplacian Filter Kernel N

	//initialize laplacian kernel filter
	Mat output = Mat::zeros(row, col, input.type());
	int kernel[3][3] = { {0,1,0},{1,-4,1},{0,1,0} };

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {

			float sum1 = 0.0;
			for (int a = -n; a <= n; a++) {
				for (int b = -n; b <= n; b++) {
					// Use mirroring boundary process

					int tempa, tempb;
					
					if (i + a > row - 1) { 
						tempa = i - a;
					}
					else if (i + a < 0) {
						tempa = -(i + a);
					}
					else {
						tempa = i + a;
					}
					if (j + b > col - 1) {
						tempb = j - b;
					}
					else if (j + b < 0) {
						tempb = -(j + b);
					}
					else {
						tempb = j + b;
					}
					sum1 += (float)kernel[n+a][n+b] * (float)(input.at<G>(tempa, tempb));
				}

			}
			//should be absolute value
			if (sum1 < 0)sum1 = -sum1;

			//multiply 8 for visualization
			sum1 = sum1 * 8;
			if (sum1 > 255) sum1 = 255;

			output.at<G>(i, j) = (G)sum1;
		}
	}
	return output;
}