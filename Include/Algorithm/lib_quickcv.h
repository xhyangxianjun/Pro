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
	/*********************************辅助函数**************************************/
	//灰度化
	QUICKCV_API void grayScale(cv::Mat src, cv::Mat& gray);
	//计算灰度直方图
	QUICKCV_API void calcHistGray(cv::Mat gray, cv::Mat mask, std::vector<int>& hist);
	//画直方图
	QUICKCV_API void drawHistGray(cv::Mat gray);
	//二维等间距补全
	QUICKCV_API void point_edfill(std::vector<cv::Point>&src, std::vector<cv::Point>&dst);
	//矩形框合并
	QUICKCV_API void mergeRects(std::vector<cv::Rect>& rects);
	//二值图投影
	QUICKCV_API void project_bin(cv::Mat bin, std::vector<int>& values, int type = HORIZON);
	//灰度图投影
	QUICKCV_API void project_gray(cv::Mat gray, std::vector<int>& values, int type = HORIZON);
	//获取白色区域中心点坐标
	QUICKCV_API void get_centers(cv::Mat bin, std::vector<cv::Point>& centers);
	//空洞填充
	QUICKCV_API void fillHole(cv::Mat srcimage, cv::Mat &dstimage);
	//弹性差值
	QUICKCV_API void flex_diff(cv::Mat mat1, cv::Mat mat2, cv::Mat &diff, int neighbor);
	/*********************************绘图函数**************************************/
	//画矩形
	QUICKCV_API void draw_rects(cv::Mat &mat, std::vector<cv::Rect> &rects, cv::Scalar &color, int thickness = 1);
	//画轴向贯穿线
	QUICKCV_API void draw_line_axis(cv::Mat &mat, int pos, cv::Scalar &color, int direction = HORIZON, int thickness = 1);
	//画直方图
	QUICKCV_API cv::Mat draw_hist(std::vector<int>& values, int direction = HORIZON);
	/*********************************向量操作**************************************/
	//计算滑动平均值
	QUICKCV_API void vec_smooth(std::vector<int>& src, std::vector<int>& dst, int radius);
	//求导
	QUICKCV_API void vec_derivative(std::vector<int>& values, std::vector<int>& derivatives);
	//腐蚀
	QUICKCV_API void vec_erode(std::vector<int>& src, std::vector<int>& dst, int radius);
	//膨胀
	QUICKCV_API void vec_dilate(std::vector<int>& src, std::vector<int>& dst, int radius);
	//顶帽
	QUICKCV_API void vec_tophat(std::vector<int>& src, std::vector<int>& dst, int radius);
	//底帽
	QUICKCV_API void vec_blackhat(std::vector<int>& src, std::vector<int>& dst, int radius);
	//求梯度
	QUICKCV_API void vec_gradient(std::vector<int>& src, std::vector<int>& dst, int radius);
	//步进式做差
	QUICKCV_API void vec_absdiff_step(std::vector<int>& values, std::vector<int>& diff, int radius);
	//寻找峰值
	QUICKCV_API void vec_findPeaks(std::vector<int>& src, std::vector<int>& dst, int radius);
	//寻找谷值
	QUICKCV_API void vec_findTroughs(std::vector<int>& src, std::vector<int>& dst, int radius);
	//寻找边缘
	QUICKCV_API void vec_findEdgess(std::vector<int>& src, std::vector<int>& dst, int radius);
	//数组阈值化
	QUICKCV_API void vec_thresh(std::vector<int>&src, std::vector<int>&dst, int thresh, int max);
	//一维等间距补全
	QUICKCV_API bool vec_edfill(std::vector<int>&src, std::vector<int>&dst);
	//线性变换
	QUICKCV_API void vec_convert(std::vector<int>&src, std::vector<int>&dst, double alpha, double beta);
	//最大值
	QUICKCV_API int vec_max(std::vector<int>&src);
	//最大值索引
	QUICKCV_API int vec_max_index(std::vector<int>&src);
	//标准差
	QUICKCV_API double vec_variance(std::vector<double> &values);
	//向量积分图
	QUICKCV_API void vec_accumulate(std::vector<int> &src, std::vector<int> &dst);
	/*********************************图像增强**************************************/
	//直方图均衡
	QUICKCV_API void enhance_equlize(cv::Mat src, cv::Mat& dst);
	//log变换
	QUICKCV_API void enhance_log(cv::Mat src, cv::Mat& dst, float c);
	//gamma变换
	QUICKCV_API void enhance_gamma(cv::Mat src, cv::Mat& dst, float fGamma);
	//laplace变换
	QUICKCV_API void engance_laplace(cv::Mat src, cv::Mat& dst);
	/*********************************几何变换**************************************/
	//左右翻转
	QUICKCV_API void geom_flip_lr(cv::Mat src, cv::Mat& dst);
	//上下翻转
	QUICKCV_API void geom_flip_ud(cv::Mat src, cv::Mat& dst);
	//顺时针90
	QUICKCV_API void geom_rotate_clock(cv::Mat src, cv::Mat& dst);
	//逆时针90
	QUICKCV_API void geom_rotate_anticlock(cv::Mat src, cv::Mat& dst);
	//自由旋转，顺时针
	QUICKCV_API void geom_rotate(cv::Mat src, cv::Mat& dst, double angle, int borderMode = 0, const cv::Scalar &borderValue = cv::Scalar());
	//裁剪
	QUICKCV_API void geom_cut(cv::Mat src, cv::Mat& dst, cv::Rect& rect);
	//水平矫正
	QUICKCV_API void geom_correct(cv::Mat src, cv::Mat& dst, cv::Point pt1, cv::Point pt2);
	/**********************************二值化**************************************/
	//最大熵阈值化
	QUICKCV_API int thresh_maxEntropy(cv::Mat src, cv::Mat& dst, int type);
	//双峰阈值化,radius代表峰值覆盖半径,minDis代表双峰最小间隔
	QUICKCV_API int thresh_twoPeaks(cv::Mat src, cv::Mat& dst, int type, cv::Mat mask = cv::Mat(), int radius = 10, int minDis = 15, bool showHist = true);
	//大津阈值
	QUICKCV_API int thresh_otsu(cv::Mat src, cv::Mat& dst, int type = cv::THRESH_BINARY);
	//自适应阈值
	QUICKCV_API void thresh_adaptive(cv::Mat src, cv::Mat& dst, int type, int method, int blockSize, double C);
	/*********************************图像平滑**************************************/
	//联合双边滤波
	QUICKCV_API void filter_jointBilateral(const cv::Mat &src, cv::Mat &dst, int d, double sigma_color, double sigma_space, cv::Mat &joint = cv::Mat(), int borderType = cv::BORDER_REPLICATE);
	//导向滤波
	QUICKCV_API void filter_guided(cv::Mat src, cv::Mat& dst, int r, double eps, double s);
	/*********************************轮廓匹配**************************************/
	//计算轮廓相似度的方式
	enum ScoreType { SCORE_SIMPLE, SCORE_AREA, SCORE_SIZE };
	//寻找最大轮廓
	QUICKCV_API bool findMaxContour(cv::Mat bin, std::vector<cv::Point>& maxContour, int type = SCORE_SIZE);
	//计算轮廓相似度评分
	QUICKCV_API double calcScore(std::vector<cv::Point>& c1, std::vector<cv::Point>& c2, int type);
	//轮廓相似度排序
	QUICKCV_API void matchShapes(std::vector<std::vector<cv::Point>>& contours, std::vector<double>& scores, std::vector<cv::Point>& contourTemp, int type = SCORE_SIMPLE);
	//求最相似的轮廓
	QUICKCV_API double matchShapesBest(std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Point>& temp, std::vector<cv::Point>& output, int type = SCORE_SIMPLE);
	/********************************形态学操作*************************************/
	QUICKCV_API void morphology(cv::Mat src, cv::Mat& dst, int op, int shape, cv::Size size, int iterations = 1);
	/*********************************图像拼接**************************************/
	//简单图像拼接
	QUICKCV_API cv::Mat stitch_simple(std::vector<cv::Mat> &imgs, int direction = HORIZON);
	/*********************************形状拟合**************************************/
	//圆拟合
	QUICKCV_API void fit_circle(std::vector<cv::Point>& contour, cv::Point2f &center, double &radius, double *sim);
	//给定误差，求多边形的最小边数
	QUICKCV_API int poly_sides_min(std::vector<cv::Point>& contour, std::vector<cv::Point> &poly, double error);
	//给定边数，求最拟合的多边形，并返回误差
	QUICKCV_API double poly_fit(std::vector<cv::Point>& contour, std::vector<cv::Point> &poly, int size);
	/*********************************尺寸测量**************************************/
	//寻找分界线
	QUICKCV_API double find_demarcation(cv::Mat bin, int direction = HORIZON);
	//傅里叶变换
	QUICKCV_API bool dft_shift(cv::Mat src, cv::Mat & dst);

	//模版匹配
	QUICKCV_API cv::Mat matchTemplate(cv::Mat mat, cv::Mat temp);
	//骨骼提取
	QUICKCV_API void BoneExtraction(cv::Mat src, cv::Mat &dst);//单通道、二值化后的图像

}