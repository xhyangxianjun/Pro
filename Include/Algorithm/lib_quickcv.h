#pragma once
#include "opencv.hpp"


#ifdef QUICKCV_EXTERN
#define QUICKCV_API extern "C" _declspec(dllexport) 
#else
#define QUICKCV_API extern "C" _declspec(dllimport) 
#endif

namespace hjh
{
	QUICKCV_API bool init_lib();
	enum Direction { HORIZON, VERTICAL };
	/*********************************��������**************************************/
	//�ҶȻ�
	QUICKCV_API void grayScale(cv::Mat src, cv::Mat& gray);
	//����Ҷ�ֱ��ͼ
	QUICKCV_API void calcHistGray(cv::Mat gray, cv::Mat mask, std::vector<int>& hist);
	//��ֱ��ͼ
	QUICKCV_API void drawHistGray(cv::Mat gray);
	//��ά�ȼ�ಹȫ
	QUICKCV_API void point_edfill(std::vector<cv::Point>&src, std::vector<cv::Point>&dst);
	//���ο�ϲ�
	QUICKCV_API void mergeRects(std::vector<cv::Rect>& rects);
	//��ֵͼͶӰ
	QUICKCV_API void project_bin(cv::Mat bin, std::vector<int>& values, int type = HORIZON);
	//�Ҷ�ͼͶӰ
	QUICKCV_API void project_gray(cv::Mat gray, std::vector<int>& values, int type = HORIZON);
	//��ȡ��ɫ�������ĵ�����
	QUICKCV_API void get_centers(cv::Mat bin, std::vector<cv::Point>& centers);
	//�ն����
	QUICKCV_API void fillHole(cv::Mat srcimage, cv::Mat &dstimage);
	//���Բ�ֵ
	QUICKCV_API void flex_diff(cv::Mat mat1, cv::Mat mat2, cv::Mat &diff, int neighbor);
	/*********************************��ͼ����**************************************/
	//������
	QUICKCV_API void draw_rects(cv::Mat &mat, std::vector<cv::Rect> &rects, cv::Scalar &color, int thickness = 1);
	//������ᴩ��
	QUICKCV_API void draw_line_axis(cv::Mat &mat, int pos, cv::Scalar &color, int direction = HORIZON, int thickness = 1);
	//��ֱ��ͼ
	QUICKCV_API cv::Mat draw_hist(std::vector<int>& values, int direction = HORIZON);
	/*********************************��������**************************************/
	//���㻬��ƽ��ֵ
	QUICKCV_API void vec_smooth(std::vector<int>& src, std::vector<int>& dst, int radius);
	//��
	QUICKCV_API void vec_derivative(std::vector<int>& values, std::vector<int>& derivatives);
	//��ʴ
	QUICKCV_API void vec_erode(std::vector<int>& src, std::vector<int>& dst, int radius);
	//����
	QUICKCV_API void vec_dilate(std::vector<int>& src, std::vector<int>& dst, int radius);
	//��ñ
	QUICKCV_API void vec_tophat(std::vector<int>& src, std::vector<int>& dst, int radius);
	//��ñ
	QUICKCV_API void vec_blackhat(std::vector<int>& src, std::vector<int>& dst, int radius);
	//���ݶ�
	QUICKCV_API void vec_gradient(std::vector<int>& src, std::vector<int>& dst, int radius);
	//����ʽ����
	QUICKCV_API void vec_absdiff_step(std::vector<int>& values, std::vector<int>& diff, int radius);
	//Ѱ�ҷ�ֵ
	QUICKCV_API void vec_findPeaks(std::vector<int>& src, std::vector<int>& dst, int radius);
	//Ѱ�ҹ�ֵ
	QUICKCV_API void vec_findTroughs(std::vector<int>& src, std::vector<int>& dst, int radius);
	//Ѱ�ұ�Ե
	QUICKCV_API void vec_findEdgess(std::vector<int>& src, std::vector<int>& dst, int radius);
	//������ֵ��
	QUICKCV_API void vec_thresh(std::vector<int>&src, std::vector<int>&dst, int thresh, int max);
	//һά�ȼ�ಹȫ
	QUICKCV_API bool vec_edfill(std::vector<int>&src, std::vector<int>&dst);
	//���Ա任
	QUICKCV_API void vec_convert(std::vector<int>&src, std::vector<int>&dst, double alpha, double beta);
	//���ֵ
	QUICKCV_API int vec_max(std::vector<int>&src);
	//���ֵ����
	QUICKCV_API int vec_max_index(std::vector<int>&src);
	//��׼��
	QUICKCV_API double vec_variance(std::vector<double> &values);
	//��������ͼ
	QUICKCV_API void vec_accumulate(std::vector<int> &src, std::vector<int> &dst);
	/*********************************ͼ����ǿ**************************************/
	//ֱ��ͼ����
	QUICKCV_API void enhance_equlize(cv::Mat src, cv::Mat& dst);
	//log�任
	QUICKCV_API void enhance_log(cv::Mat src, cv::Mat& dst, float c);
	//gamma�任
	QUICKCV_API void enhance_gamma(cv::Mat src, cv::Mat& dst, float fGamma);
	//laplace�任
	QUICKCV_API void engance_laplace(cv::Mat src, cv::Mat& dst);
	/*********************************���α任**************************************/
	//���ҷ�ת
	QUICKCV_API void geom_flip_lr(cv::Mat src, cv::Mat& dst);
	//���·�ת
	QUICKCV_API void geom_flip_ud(cv::Mat src, cv::Mat& dst);
	//˳ʱ��90
	QUICKCV_API void geom_rotate_clock(cv::Mat src, cv::Mat& dst);
	//��ʱ��90
	QUICKCV_API void geom_rotate_anticlock(cv::Mat src, cv::Mat& dst);
	//������ת��˳ʱ��
	QUICKCV_API void geom_rotate(cv::Mat src, cv::Mat& dst, double angle, int borderMode = 0, const cv::Scalar &borderValue = cv::Scalar());
	//�ü�
	QUICKCV_API void geom_cut(cv::Mat src, cv::Mat& dst, cv::Rect& rect);
	//ˮƽ����
	QUICKCV_API void geom_correct(cv::Mat src, cv::Mat& dst, cv::Point pt1, cv::Point pt2);
	/**********************************��ֵ��**************************************/
	//�������ֵ��
	QUICKCV_API int thresh_maxEntropy(cv::Mat src, cv::Mat& dst, int type);
	//˫����ֵ��,radius�����ֵ���ǰ뾶,minDis����˫����С���
	QUICKCV_API int thresh_twoPeaks(cv::Mat src, cv::Mat& dst, int type, cv::Mat mask = cv::Mat(), int radius = 10, int minDis = 15, bool showHist = true);
	//�����ֵ
	QUICKCV_API int thresh_otsu(cv::Mat src, cv::Mat& dst, int type = cv::THRESH_BINARY);
	//����Ӧ��ֵ
	QUICKCV_API void thresh_adaptive(cv::Mat src, cv::Mat& dst, int type, int method, int blockSize, double C);
	/*********************************ͼ��ƽ��**************************************/
	//����˫���˲�
	QUICKCV_API void filter_jointBilateral(const cv::Mat &src, cv::Mat &dst, int d, double sigma_color, double sigma_space, cv::Mat &joint = cv::Mat(), int borderType = cv::BORDER_REPLICATE);
	//�����˲�
	QUICKCV_API void filter_guided(cv::Mat src, cv::Mat& dst, int r, double eps, double s);
	/*********************************����ƥ��**************************************/
	//�����������ƶȵķ�ʽ
	enum ScoreType { SCORE_SIMPLE, SCORE_AREA, SCORE_SIZE };
	//Ѱ���������
	QUICKCV_API bool findMaxContour(cv::Mat bin, std::vector<cv::Point>& maxContour, int type = SCORE_SIZE);
	//�����������ƶ�����
	QUICKCV_API double calcScore(std::vector<cv::Point>& c1, std::vector<cv::Point>& c2, int type);
	//�������ƶ�����
	QUICKCV_API void matchShapes(std::vector<std::vector<cv::Point>>& contours, std::vector<double>& scores, std::vector<cv::Point>& contourTemp, int type = SCORE_SIMPLE);
	//�������Ƶ�����
	QUICKCV_API double matchShapesBest(std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Point>& temp, std::vector<cv::Point>& output, int type = SCORE_SIMPLE);
	/********************************��̬ѧ����*************************************/
	QUICKCV_API void morphology(cv::Mat src, cv::Mat& dst, int op, int shape, cv::Size size, int iterations = 1);
	/*********************************ͼ��ƴ��**************************************/
	//��ͼ��ƴ��
	QUICKCV_API cv::Mat stitch_simple(std::vector<cv::Mat> &imgs, int direction = HORIZON);
	/*********************************��״���**************************************/
	//Բ���
	QUICKCV_API void fit_circle(std::vector<cv::Point>& contour, cv::Point2f &center, double &radius, double *sim);
	//�����������ε���С����
	QUICKCV_API int poly_sides_min(std::vector<cv::Point>& contour, std::vector<cv::Point> &poly, double error);
	//����������������ϵĶ���Σ����������
	QUICKCV_API double poly_fit(std::vector<cv::Point>& contour, std::vector<cv::Point> &poly, int size);
	/*********************************�ߴ����**************************************/
	//Ѱ�ҷֽ���
	QUICKCV_API double find_demarcation(cv::Mat bin, int direction = HORIZON);
	//����Ҷ�任
	QUICKCV_API bool dft_shift(cv::Mat src, cv::Mat & dst);

	//ģ��ƥ��
	QUICKCV_API cv::Mat matchTemplate(cv::Mat mat, cv::Mat temp);
	//������ȡ
	QUICKCV_API void BoneExtraction(cv::Mat src, cv::Mat &dst);//��ͨ������ֵ�����ͼ��

}