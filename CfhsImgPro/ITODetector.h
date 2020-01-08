#pragma once
#include <vector>
#include <lib_quickcv.h>
using namespace std;
using namespace cv;
using namespace hjh;

#ifdef ITODETECTOR_EXTERN
#define ITODETECTOR _declspec(dllexport)
#else
#define ITODETECTOR _declspec(dllimport)
#endif


struct LINE
{
	Point pt_in;
	Point pt_out;
	double length;
	double theta;
	double diff;
};

class ITODETECTOR ITODetector
{
public:
	ITODetector();
	~ITODetector();
	void set_length(int length);
	void set_thresh(int thresh);
	void set_top(int top);
	void set_left(int left);
	void set_right(int right);
	void set_bottom(int bottom);
	void set_limit(int limit);
	void set_iter(int iter);

	void detect_big(Mat gray, vector<LINE> &info);
	void detect_small_bin(Mat bin, double theta1, double theta2, vector<LINE> &info);
private:
	void filter_lines(Mat gray, double theta1, double theta2, vector<LINE>& info_temp, vector<LINE>& info);
	void merge(vector<LINE> &lines, Size size);
	void calc_theta(Mat gray, double & theta1, double & theta2);
	void calc_theta_ave(Mat gray, int w, int h, double & theta1, double & theta2);
	void adjustPoints(vector<Point2f>& pts, double theta, Size sizeMat);
	void adjustPoints(vector<Point>& pts, double theta, Size sizeMat);
	void scanning_bin(Mat bin_big, Rect rect_mask, vector<Point2f>& pt_in, vector<Point2f>& pt_out, double theta);
	void cut(Size mat_size, int w, int h, vector<Rect>& rects);
	void extract_nodes(Mat mat_hline, Mat mask_temp, int step, int roi_width, vector<int> &row_index, vector<vector<Point>> &nodes);

	int m_thresh;
	int m_length;
	int m_border_top;
	int m_border_left;
	int m_border_right;
	int m_border_bottom;
	int m_limit;
	int m_iter;
};

