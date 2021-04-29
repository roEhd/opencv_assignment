#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>       /* exp */
#include <time.h>
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

Mat gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt);
Mat gaussianfilter_separate(const Mat input, int n, float sigmaT, float sigmaS, const char* opt);
clock_t start, end;

int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat output;

	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Original_Grayscale", WINDOW_AUTOSIZE);
	imshow("Original_Grayscale", input);

	//zero-paddle, mirroring, adjustkernel	
	output = gaussianfilter(input, 1, 1, 1, "zero-paddle");

	namedWindow("Gaussian Filter", WINDOW_AUTOSIZE);
	imshow("Gaussian Filter", output);

	output = gaussianfilter_separate(input, 1, 1, 1, "zero-paddle");

	namedWindow("Gaussian Filter_separate", WINDOW_AUTOSIZE);
	imshow("Gaussian Filter_separate", output);


	waitKey(0);

	return 0;
}

Mat gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt) {

	Mat kernel;

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;
	float denom;
	float kernelvalue;

	// Initialiazing Kernel Matrix 
	kernel = Mat::zeros(kernel_size, kernel_size, CV_32F);

	/* calculating Gaussian filter without any separation */
	start = clock();
	denom = 0.0;
	for (int a = -n; a <= n; a++) {
		for (int b = -n; b <= n; b++) {
			float value1 = exp(-(pow(a, 2) / (2 * pow(sigmaS, 2))) - (pow(b, 2) / (2 * pow(sigmaT, 2))));
			kernel.at<float>(a + n, b + n) = value1; // calculate filter except for denominator
			denom += value1; //  calculate denomiator in filter
		}
	}

	for (int a = -n; a <= n; a++) {
		for (int b = -n; b <= n; b++) {
			kernel.at<float>(a + n, b + n) /= denom; // final step of calculating filter
		}
	}

	Mat output = Mat::zeros(row, col, input.type());

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {


			if (!strcmp(opt, "zero-paddle")) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						// Gaussian filter with Zero-paddle boundary process
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							//if the pixel is not a border pixel. if not, simply don't add the value
							sum1_r += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b)[0]);
							sum1_g += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b)[1]);
							sum1_b += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b)[2]);
						}
					}
				}
				output.at<C>(i, j)[0] = (G)sum1_r;
				output.at<C>(i, j)[1] = (G)sum1_g;
				output.at<C>(i, j)[2] = (G)sum1_b;
			}

			else if (!strcmp(opt, "mirroring")) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {

						// Gaussian filter with "mirroring" process
						// decide intensity of original at i+a
						if (i + a > row - 1) {
							tempa = i - a;
						}
						else if (i + a < 0) {
							tempa = -(i + a);
						}
						else {
							tempa = i + a;
						}
						//decide intensity of original at j+b
						if (j + b > col - 1) {
							tempb = j - b;
						}
						else if (j + b < 0) {
							tempb = -(j + b);
						}
						else {
							tempb = j + b;
						}
						sum1_r += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(tempa, tempb)[0]);
						sum1_g += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(tempa, tempb)[1]);
						sum1_b += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(tempa, tempb)[2]);

					}
				}
				output.at<C>(i, j)[0] = (G)sum1_r;
				output.at<C>(i, j)[1] = (G)sum1_g;
				output.at<C>(i, j)[2] = (G)sum1_b;
			}


			else if (!strcmp(opt, "adjustkernel")) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				float sum2 = 0.0;
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {

						// Gaussian filter with "adjustkernel" process
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							//if the pixel is not a border pixel. if not, simply don't add the value
							sum1_r += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b)[0]);
							sum1_g += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b)[1]);
							sum1_b += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b)[2]);
							sum2 += kernel.at<float>(a + n, b + n); //for normalization because summation is not 1
						}

					}
				}
				output.at<C>(i, j)[0] = (G)(sum1_r / sum2);
				output.at<C>(i, j)[1] = (G)(sum1_g / sum2);
				output.at<C>(i, j)[2] = (G)(sum1_b / sum2);
			}
		}
	}
	end = clock();
	printf("Not separable manner: %lf\n", (double)(end - start) / CLOCKS_PER_SEC);
	return output;
}

Mat gaussianfilter_separate(const Mat input, int n, float sigmaT, float sigmaS, const char* opt) {

	Mat kernel_t;
	Mat kernel_s;
	Mat kernel_temp;

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;
	float denom;
	

	// Initialiazing Kernel Matrix 
	kernel_s = Mat::zeros(kernel_size, 1, CV_32F);
	kernel_t = Mat::zeros(1, kernel_size, CV_32F);
	kernel_temp = Mat::zeros(kernel_size, 1, input.type());

	Mat output = Mat::zeros(row, col, input.type());

	/* calculating Gaussian filter in a separable manner */
	
	
	//kernel_s
	denom = 0.0;
	for (int a = -n; a <= n; a++) {
		float value1 = exp(-(pow(a, 2) / (2 * pow(sigmaT, 2))));
		kernel_s.at<float>(a + n, 0) = value1; // calculate filter except for denominator
		denom += value1; //  calculate denomiator in filter
	}
	for (int a = -n; a <= n; a++) kernel_s.at<float>(a + n, 0) /= denom; // final step of calculating filter

	//kernel_t
	denom = 0.0;
	for (int b = -n; b <= n; b++) {
		float value1 = exp(-(pow(b, 2) / (2 * pow(sigmaS, 2))));
		kernel_t.at<float>(0, b + n) = value1; // calculate filter except for denominator
		denom += value1; //  calculate denomiator in filter
	}
	for (int b = -n; b <= n; b++) kernel_t.at<float>(0, b + n) /= denom; // final step of calculating filter

	start = clock();
	
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {


			if (!strcmp(opt, "zero-paddle")) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				for (int a = -n; a <= n; a++) {

					for (int b = -n; b <= n; b++) {

						// Gaussian filter with Zero-paddle boundary process
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
							kernel_temp.at<C>(a + n, 0)[0] += kernel_t.at<float>(0, b + n) * (float)(input.at<C>(i + a, j + b)[0]);
							kernel_temp.at<C>(a + n, 0)[1] += kernel_t.at<float>(0, b + n) * (float)(input.at<C>(i + a, j + b)[1]);
							kernel_temp.at<C>(a + n, 0)[2] += kernel_t.at<float>(0, b + n) * (float)(input.at<C>(i + a, j + b)[2]);
						}

					}
					sum1_r += kernel_s.at<float>(a + n, 0) * kernel_temp.at<C>(a + n, 0)[0];
					sum1_g += kernel_s.at<float>(a + n, 0) * kernel_temp.at<C>(a + n, 0)[1];
					sum1_b += kernel_s.at<float>(a + n, 0) * kernel_temp.at<C>(a + n, 0)[2];
					kernel_temp.at<C>(a + n, 0)[0] = 0.0;
					kernel_temp.at<C>(a + n, 0)[1] = 0.0;
					kernel_temp.at<C>(a + n, 0)[2] = 0.0;
				}


				output.at<C>(i, j)[0] = (G)sum1_r;
				output.at<C>(i, j)[1] = (G)sum1_g;
				output.at<C>(i, j)[2] = (G)sum1_b;
			}

			else if (!strcmp(opt, "mirroring")) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {

						// Gaussian filter with "mirroring" process:
						if (i + a > row - 1) {  //mirroring for the border pixels
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
						kernel_temp.at<C>(a + n, 0)[0] += kernel_t.at<float>(0, b + n) * (float)(input.at<C>(tempa, tempb)[0]);
						kernel_temp.at<C>(a + n, 0)[1] += kernel_t.at<float>(0, b + n) * (float)(input.at<C>(tempa, tempb)[1]);
						kernel_temp.at<C>(a + n, 0)[2] += kernel_t.at<float>(0, b + n) * (float)(input.at<C>(tempa, tempb)[2]);
					}
					sum1_r += kernel_s.at<float>(a + n, 0) * kernel_temp.at<C>(a + n, 0)[0];
					sum1_g += kernel_s.at<float>(a + n, 0) * kernel_temp.at<C>(a + n, 0)[1];
					sum1_b += kernel_s.at<float>(a + n, 0) * kernel_temp.at<C>(a + n, 0)[2];
					kernel_temp.at<C>(a + n, 0)[0] = 0.0;
					kernel_temp.at<C>(a + n, 0)[1] = 0.0;
					kernel_temp.at<C>(a + n, 0)[2] = 0.0;
				}
				output.at<C>(i, j)[0] = (G)sum1_r;
				output.at<C>(i, j)[1] = (G)sum1_g;
				output.at<C>(i, j)[2] = (G)sum1_b;
			}


			else if (!strcmp(opt, "adjustkernel")) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				float sum2 = 0.0;
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {

						// Gaussian filter with "adjustkernel" process:
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							kernel_temp.at<C>(a + n, 0)[0] += kernel_t.at<float>(0, b + n) * (float)(input.at<C>(i + a, j + b)[0]);
							kernel_temp.at<C>(a + n, 0)[1] += kernel_t.at<float>(0, b + n) * (float)(input.at<C>(i + a, j + b)[1]);
							kernel_temp.at<C>(a + n, 0)[2] += kernel_t.at<float>(0, b + n) * (float)(input.at<C>(i + a, j + b)[2]);
							sum2 += kernel_s.at<float>(a + n, 0) * kernel_t.at<float>(0, b + n);
							
						}
					}
					sum1_r += kernel_s.at<float>(a + n, 0) * kernel_temp.at<C>(a + n, 0)[0];
					sum1_g += kernel_s.at<float>(a + n, 0) * kernel_temp.at<C>(a + n, 0)[1];
					sum1_b += kernel_s.at<float>(a + n, 0) * kernel_temp.at<C>(a + n, 0)[2];
					kernel_temp.at<C>(a + n, 0)[0] = 0.0;
					kernel_temp.at<C>(a + n, 0)[1] = 0.0;
					kernel_temp.at<C>(a + n, 0)[2] = 0.0;
				}
				output.at<C>(i, j)[0] = (G)(sum1_r / sum2);
				output.at<C>(i, j)[1] = (G)(sum1_g / sum2);
				output.at<C>(i, j)[2] = (G)(sum1_b / sum2);
			}

		}
	}
	end = clock();
	printf("Separable Manner: %lf\n", (double)(end - start) / CLOCKS_PER_SEC);
	return output;
}