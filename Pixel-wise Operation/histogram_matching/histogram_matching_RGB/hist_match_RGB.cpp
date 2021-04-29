#include "hist_func.h"
void hist_eq(Mat& input, Mat& equalized, G* trans_func, float* CDF);

int main() {

	//Obtain the transfer function s = T(r)
	//input

	Mat input = imread("street.jpg", IMREAD_COLOR);
	Mat equalized_YUV;

	cvtColor(input, equalized_YUV, COLOR_RGB2YUV);
	Mat channels[3];
	split(equalized_YUV, channels);// split each channel(Y, U, V)
	Mat Y = channels[0];

	float** PDF_RGB = cal_PDF_RGB(input);	//PDF of input
	float* CDF_YUV = cal_CDF(Y);			//CDF of input

	G trans_func_YUV_T[L] = { 0 };			// transfer function T

	hist_eq(channels[0], Y, trans_func_YUV_T, CDF_YUV);	// histogram equalization on Y channel
	channels[0] = Y;

	merge(channels, 3, equalized_YUV);
	cvtColor(equalized_YUV, equalized_YUV, COLOR_YUV2RGB);


	//Obtain the transfer function s = G(z)
	//reference

	Mat input2 = imread("reference.jpg", IMREAD_COLOR);
	Mat equalized_YUV2;

	cvtColor(input2, equalized_YUV2, COLOR_RGB2YUV);
	Mat channels2[3];
	split(equalized_YUV2, channels2);// split each channel(Y, U, V)
	Mat Y2 = channels2[0];

	float** PDF_RGB2 = cal_PDF_RGB(input2);
	float* CDF_YUV2 = cal_CDF(Y2);

	G trans_func_YUV_G[L] = { 0 };			// transfer function G

	hist_eq(channels2[0], Y2, trans_func_YUV_G, CDF_YUV2); // histogram equalization on Y channel
	channels2[0] = Y2;

	// merge Y, U, V channels
	merge(channels2, 3, equalized_YUV2);
	cvtColor(equalized_YUV2, equalized_YUV2, COLOR_YUV2RGB);


	//Apply the intensity mapping from r to z
	//Compute z = G^-1(s)
		//initialize G^-1(s)=-1
	G trans_func_Ginv[L] = { -1 };

	//G^-1(G(z))=z
	for (int i = 0; i < L; i++) {
		trans_func_Ginv[trans_func_YUV_G[i]] = i;
	}
	//if still G^-1(s)=-1, neighbor value
	for (int i = 0; i < L; i++) {
		if (trans_func_Ginv[i] == -1) {
			int a = 0;	//값을 최초로 가지는 i보다 작은 neighbor
			int b = 0;	//값을 최초로 가지는 i보다 큰 neighbor
			for (int j = i; j < L; j++) if (trans_func_Ginv[j] != -1) { b = j; break; }
			for (int j = i; j >= 0; j--) if (trans_func_Ginv[j] != -1) { a = j; break; }
			if (abs(b - i) < abs(a - i)) trans_func_Ginv[i] = trans_func_Ginv[b]; //b랑 더 가까우면
			else trans_func_Ginv[i] = trans_func_Ginv[a]; //a랑 더 가까우면
		}
	}

	//compute trans_func
	//z = G^-1 ( T ( r ) )
	G trans_func[L] = { 0 };
	for (int i = 0; i < L; i++)
		trans_func[i] = trans_func_Ginv[trans_func_YUV_T[i]];

	//apply the trans_func on Y channel of input
	Mat Y3 = Y.clone();
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			Y3.at<G>(i, j) = trans_func[Y.at<G>(i, j)];

	Mat matched;
	Mat channels3[3];
	cvtColor(input, matched, COLOR_RGB2YUV);
	split(matched, channels3);
	channels3[0] = Y3;//applied Y channel
	merge(channels3, 3, matched);
	cvtColor(matched, matched, COLOR_YUV2RGB);

	//Wrint in files
	FILE* f_PDF_RGB;
	fopen_s(&f_PDF_RGB, "PDF_RGB.txt", "w+");
	FILE* f_match_PDF_RGB;	// text file for matced image's PDF
	fopen_s(&f_match_PDF_RGB, "matched_PDF_RGB.txt", "w+");
	float** match_PDF_RGB = cal_PDF_RGB(matched);
	FILE* f_trans_func_RGB; // text file for trans function
	fopen_s(&f_trans_func_RGB, "trans_func_RGB.txt", "w+");


	for (int i = 0; i < L; i++) {
		// write PDFs
		fprintf(f_PDF_RGB, "%d\t%f\t%f\t%f\n", i, PDF_RGB[i][0], PDF_RGB[i][1], PDF_RGB[i][2]); //input's PDF
		fprintf(f_match_PDF_RGB, "%d\t%f\t%f\t%f\n", i, match_PDF_RGB[i][0], match_PDF_RGB[i][1], match_PDF_RGB[i][2]); //matched image's PDF
		// write transfer function
		fprintf(f_trans_func_RGB, "%d\t%d\n", i, trans_func[i]);
	}

	// memory release
	free(PDF_RGB);
	free(CDF_YUV);
	free(PDF_RGB2);
	free(CDF_YUV2);
	fclose(f_PDF_RGB);
	fclose(f_match_PDF_RGB);
	fclose(f_trans_func_RGB);

	////////////////////// Show each image ///////////////////////

	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Equalized", WINDOW_AUTOSIZE);
	imshow("Equalized", equalized_YUV);

	namedWindow("Matched", WINDOW_AUTOSIZE);
	imshow("Matched", matched);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;

}

// histogram equalization
void hist_eq(Mat& input, Mat& equalized, G* trans_func, float* CDF) {

	// compute transfer function
	for (int i = 0; i < L; i++)
		trans_func[i] = (G)((L - 1) * CDF[i]);

	// perform the transfer function
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			equalized.at<G>(i, j) = trans_func[input.at<G>(i, j)];
}