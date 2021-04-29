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

	//bilinear interpolation을 이용해 45도 회전시킨 결과물
	b_rotated = myrotate<Vec3b>(input, 45, "bilinear");
	namedWindow("b_rotated");
	imshow("b_rotated", b_rotated);

	//nearest interpolation을 이용해 45도 회전시킨 결과물
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
	//모든 rotation에 대해서 적용하기 위해 절댓값으로 최종이미지의 크기를 구한다
	float sq_col = ceil(abs(row * sin(radian)) + abs(col * cos(radian)));
	float sq_row = ceil(abs(col * sin(radian)) + abs(row * cos(radian)));

	Mat output = Mat::zeros(sq_row, sq_col, input.type());

	for (int i = 0; i < sq_row; i++) {
		for (int j = 0; j < sq_col; j++) {

			//결과물 픽셀(j,i)를 역회전 시켜 대응되는 원본 픽셀(x,y)을 얻는다
			//이때 사진의 원점이동으로 사진이 잘리지 않도록 해준다
			float x = (j - sq_col / 2) * cos(radian) - (i - sq_row / 2) * sin(radian) + col / 2;
			float y = (j - sq_col / 2) * sin(radian) + (i - sq_row / 2) * cos(radian) + row / 2;

			//계산한 픽셀의 원본사진 범위안에 있을 때
			//보간법을 적용한다
			if ((y >= 0) && (y <= (row - 1)) && (x >= 0) && (x <= (col - 1))) {
				if (!strcmp(opt, "nearest")) {
					//nearest interpolation
					//결과물의 픽셀에 해당하는 값이 실수일때 가장 가까운 픽셀의 intensity를 가져오는 방법
					//가장 가까운 픽셀은 반올림을 통해 구할 수 있다
					output.at<Vec3b>(i, j) = input.at<Vec3b>(round(y), round(x));
				}
				else if (!strcmp(opt, "bilinear")) {
					//bilinear interpolation	

						//x1=x의 내림 픽셀 x2=x의 올림 픽셀
						//y1=y의 내림 픽셀 y2=y의 올림 픽셀

					float x1 = floor(x);
					float x2 = ceil(x);
					float y1 = floor(y);
					float y2 = ceil(y);

					//f(y1,x1)와 f(y1,x2) 의 lambda : 1-lambda 내분점 f(y1,y) = pixel1
					//f(y2,x1)과 f(y2,x2) 의 lambda : 1-lambda 내분점 f(y2,x) = pixel2
					//pixel1과 pixel2 의 mu : 1-mu 내분점 f(y,x)

					double lambda = x - x1;
					double mu = y - y1;

					//	bilinear interpolation을 통해 적절한 intensity를 계산한 후 
					Vec3b pixel1 = (1 - lambda) * input.at<Vec3b>(y1, x1) + lambda * input.at<Vec3b>(y1, x2);
					Vec3b pixel2 = (1 - lambda) * input.at<Vec3b>(y2, x1) + lambda * input.at<Vec3b>(y2, x2);
					Vec3b pixel3 = (1 - mu) * pixel1 + mu * pixel2;

					//결과물에 대입한다
					output.at < Vec3b >(i, j) = pixel3;


				}
			}
		}
	}//end of for

	return output;
}