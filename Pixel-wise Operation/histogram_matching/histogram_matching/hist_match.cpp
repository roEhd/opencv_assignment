#include "hist_func.h"
void hist_eq(Mat& input, Mat& equalized, G* trans_func, float* CDF);

int main() {

	//Obtain the transfer function s = T(r)
	//input

	Mat input = imread("street.jpg", IMREAD_COLOR);
	Mat input_gray;
	cvtColor(input, input_gray, COLOR_RGB2GRAY);	// convert RGB to Grayscale
	Mat equalized = input_gray.clone();

	float* PDF = cal_PDF(input_gray);	// PDF of Input image(Grayscale) : [L]
	float* CDF = cal_CDF(input_gray);	// CDF of Input image(Grayscale) : [L]

	G trans_func_T[L] = { 0 };			// transfer function T

	hist_eq(input_gray, equalized, trans_func_T, CDF);			// histogram equalization on grayscale image	
	float* equalized_PDF_gray = cal_PDF(equalized);			// PDF of Input's equalized image

	//Obtain the transfer function s = G(z)
	//reference

	Mat input2 = imread("reference.jpg", IMREAD_COLOR);
	Mat input_gray2;
	cvtColor(input2, input_gray2, COLOR_RGB2GRAY);	// convert RGB to Grayscale
	Mat equalized2 = input_gray2.clone();


	float* PDF2 = cal_PDF(input_gray2);	// PDF of Input2 image(Grayscale) : [L]
	float* CDF2 = cal_CDF(input_gray2);	// CDF of Input2 image(Grayscale) : [L]

	G trans_func_G[L] = { 0 };			// transfer function G

	hist_eq(input_gray2, equalized2, trans_func_G, CDF2);		// histogram equalization on grayscale image



	//Apply the intensity mapping from r to z
	//Compute z = G^-1(s)
	//initialize G^-1(s)=-1
	G trans_func_Ginv[L] = { -1 };

	//G^-1(G(z))=z
	for (int i = 0; i < L; i++) {
		trans_func_Ginv[trans_func_G[i]] = i;
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
		trans_func[i] = trans_func_Ginv[trans_func_T[i]];

	Mat matched = input_gray.clone();
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			matched.at<G>(i, j) = trans_func[input_gray.at<G>(i, j)];	//applying to input

	//Write in files

	FILE* f_match_PDF;	// text file for matced image's PDF
	float* match_PDF = cal_PDF(matched);
	FILE* f_PDF;// PDF txt file
	FILE* f_trans_func; // text file for trans function
	fopen_s(&f_trans_func, "trans_func.txt", "w+");
	fopen_s(&f_match_PDF, "matched_PDF.txt", "w+");
	fopen_s(&f_PDF, "PDF.txt", "w+");

	for (int i = 0; i < L; i++) {
		// write PDFs
		fprintf(f_PDF, "%d\t%f\n", i, PDF[i]); //input's PDF
		fprintf(f_match_PDF, "%d\t%f\n", i, match_PDF[i]); //matched image's PDF
		// write transfer function
		fprintf(f_trans_func, "%d\t%d\n", i, trans_func[i]);
	}

	// memory release
	free(PDF);
	free(CDF);
	free(PDF2);
	free(CDF2);
	fclose(f_PDF);
	fclose(f_trans_func);
	fclose(f_match_PDF);

	////////////////////// Show each image ///////////////////////

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("Equalized", WINDOW_AUTOSIZE);
	imshow("Equalized", equalized);

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