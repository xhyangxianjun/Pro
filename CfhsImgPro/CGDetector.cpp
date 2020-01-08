#include "stdafx.h"

#include "CGDetector.h"
#include <numeric>
using namespace std;

CGDetector::CGDetector()
	: m_aa_region_positive(40)
	, m_aa_region_negative(40)
	, m_grey_difference_positive(20)
	, m_grey_difference_negative(20)
	, m_num_for_stop_inspection(5)
	, m_max_area(50000)
	, m_min_area(180)
{
}

CGDetector::~CGDetector()
{
}

int CGDetector::find_thresh_phone(Mat gray)
{
	vector<int> hist; calcHistGray(gray, Mat(), hist);						//namedWindow("hist", 0); imshow("hist", draw_hist(hist)); waitKey();
	vec_smooth(hist, hist, 2);												//namedWindow("hist", 0); imshow("hist", draw_hist(hist)); waitKey();
	vector<int> peaks; vec_findPeaks(hist, peaks, 3);
	vector<int> grays;
	for (int i = 0; i < peaks.size(); i++)
	{
		if (peaks[i] > 50000)
		{
			grays.push_back(i);
		}
	}
	return (grays[0] + grays[1]) / 2;
}

int CGDetector::find_thresh_aa(Mat gray)
{
	vector<int> hist; calcHistGray(gray, Mat(), hist);						//namedWindow("hist", 0); imshow("hist", draw_hist(hist)); waitKey();
	vec_smooth(hist, hist, 2);												//namedWindow("hist", 0); imshow("hist", draw_hist(hist)); waitKey();
	int thresh_aa = max_element(hist.begin(), hist.end()) - hist.begin();
	return thresh_aa;
}

void CGDetector::extract_phone(Mat gray, Mat &dst)
{
	int thresh_phone = find_thresh_phone(gray);
	Mat small; pyrDown(gray, small);//降采样
	Mat bin; threshold(small, bin, thresh_phone, 255, THRESH_BINARY);
	vector<Point> contour_phone; findMaxContour(bin, contour_phone);
	RotatedRect box_phone = minAreaRect(contour_phone);
	vector<Point2f> pts(4); box_phone.points(pts.data());
	Size size_phone(min(box_phone.size.width, box_phone.size.height), max(box_phone.size.width, box_phone.size.height));
	vector<Point2f> pts_new;
	if (pts[0].x > small.cols / 2) pts_new.push_back(Point2f(size_phone.width - 1, size_phone.height - 1));
	pts_new.push_back(Point2f(0, size_phone.height - 1));
	pts_new.push_back(Point2f(0, 0));
	pts_new.push_back(Point2f(size_phone.width - 1, 0));
	if (pts[0].x < small.cols / 2) pts_new.push_back(Point2f(size_phone.width - 1, size_phone.height - 1));

	for (int i = 0; i < pts.size(); i++)
	{
		pts[i] *= 2;
		pts_new[i] *= 2;
	}
	size_phone *= 2;
	Mat H = findHomography(pts, pts_new);
	warpPerspective(gray, dst, H, size_phone);
	//copyMakeBorder(dst, dst, 500, 500, 500, 500, BORDER_CONSTANT);
}

void CGDetector::extract_aa_contour(Mat mat_phone, string file_model, vector<Point>& contour_aa, Mat &mat_aa)
{
	if (!file_model.empty()) extract_aa_contour_model(mat_phone, file_model, contour_aa);
	else extract_aa_contour_auto(mat_phone, contour_aa);
	cvtColor(mat_phone, mat_aa, COLOR_GRAY2BGR);
	vector<vector<Point>> container; container.push_back(contour_aa);
	drawContours(mat_aa, container, 0, Scalar(0, 0, 255), 30);
}

void CGDetector::extract_aa_contour_auto(Mat mat_phone, vector<Point> &contour_aa)
{
	//计算AA区的阈值
	int thresh_aa = find_thresh_aa(mat_phone);
	//对手机图降采样
	Mat mat_phone_small; pyrDown(mat_phone, mat_phone_small);
	//AA区分割
	int thresh1 = thresh_aa - m_aa_region_negative, thresh2 = thresh_aa + m_aa_region_negative;
	if (thresh1 < 0) thresh1 = 0;
	if (thresh2 > 255) thresh2 = 255;
	Mat bin1; threshold(mat_phone_small, bin1, thresh1, 255, THRESH_BINARY);
	Mat bin2; threshold(mat_phone_small, bin2, thresh2, 255, THRESH_BINARY);
	Mat mask_aa = bin1 - bin2;
	fillHole(mask_aa, mask_aa);		//namedWindow("mask_aa", 0); imshow("mask_aa", mask_aa); waitKey(1);
	//获取aa区轮廓
	vector<Point> contour_aa_org; findMaxContour(mask_aa, contour_aa_org);
	vector<vector<Point>> container; container.push_back(contour_aa_org);
	mask_aa.setTo(0); drawContours(mask_aa, container, 0, Scalar(255), -1);
	vector<vector<Point>> contour_fit(4);
	//定位边界
	vector<int> borders; find_borders(mask_aa, borders);
	//左右边界轮廓拟合
	detect_lr_contour(mat_phone_small, borders, contour_fit[1], contour_fit[3]);
	//上下边界轮廓拟合
	detect_ud_contour(mat_phone_small, borders, contour_fit[0], contour_fit[2]);
	//轮廓混合
	mix_contour_aa(contour_aa_org, contour_fit, contour_aa);
	//还原轮廓到原尺寸
	for (auto &pt : contour_aa) pt *= 2; 

	////寻找轮廓瑕疵点
	//vector<int> index_bad;
	//for (int i = 30; i < contour_aa.size() - 30; i++)
	//{
	//	Point f = contour_aa[i + 30];
	//	Point p = contour_aa[i];
	//	Point b = contour_aa[i - 30];
	//	vector<double> x, y, m;
	//	Point v1 = f - p, v2 = p - b, v3 = f - b;
	//	x.push_back(v1.x); x.push_back(v2.x); x.push_back(v3.x);
	//	y.push_back(v1.y); y.push_back(v2.y); y.push_back(v3.y);
	//	magnitude(x, y, m);
	//	double dx = abs(v3.x), dy = abs(v3.y);
	//	if ((m[2] / (m[0] + m[1]) < 0.98)/*|| min(dx, dy) / max(dx, dy)>0.1*/)
	//	{
	//		index_bad.push_back(i);
	//		//circle(show, p, 10, Scalar(0, 0, 255), -1);
	//	}
	//}
	//Mat show; cvtColor(gray, show, COLOR_GRAY2BGR);
	//for (auto &contour : contour_fit)
	//{
	//	for (Point pt : contour)
	//	{
	//		circle(show, pt, 30, Scalar(255, 0, 0), -1);
	//	}
	//}
	//polylines(show, contour_aa, true, Scalar(0, 255, 0), 10);
	//vector<Point> hull; convexHull(contour_aa, hull);
	//polylines(show, contour_aa, true, Scalar(0, 255, 0), 10);

	//line(show, Point(width, 0), Point(width, show.rows - 1), Scalar(250, 0, 0), 10);
	//line(show, Point(show.cols - width, 0), Point(show.cols - width, show.rows - 1), Scalar(250, 0, 0), 10);
	//line(show, Point(0, height), Point(show.cols - 1, height), Scalar(250, 0, 0), 10);
	//line(show, Point(0, show.rows - height), Point(show.cols - 1, show.rows - height), Scalar(250, 0, 0), 10);
	//namedWindow("hull", 0); imshow("hull", show); waitKey(1);
	//for (int i = 0; i < contour_aa.size(); i += 200)
	//{
	//	circle(show, contour_aa[i], 30, Scalar(0, 0, 255), -1);
	//	namedWindow("hull", 0); imshow("hull", show); waitKey(1);
	//}
	//namedWindow("show", 0); imshow("show", show); waitKey();
}

void CGDetector::extract_aa_contour_model(Mat mat_phone, string file_model, vector<Point>& contour)
{
	vector<Point2f> model_vertex;
	vector<Point> model_contour;
	load_model(file_model, model_vertex, model_contour);

	vector<Point2f> vertex;
	vertex.push_back(Point2f(0, 0));
	vertex.push_back(Point2f(0, mat_phone.rows));
	vertex.push_back(Point2f(mat_phone.cols, mat_phone.rows));
	vertex.push_back(Point2f(mat_phone.cols, 0));
	Mat H = findHomography(model_vertex, vertex);
	Mat mat = Mat::zeros(3, model_contour.size(), H.type());
	for (int i = 0; i < model_contour.size(); i++)
	{
		mat.at<double>(0, i) = model_contour[i].x;
		mat.at<double>(1, i) = model_contour[i].y;
		mat.at<double>(2, i) = 1;
	}
	Mat result = H*mat;
	vector<Point> pts;
	for (int i = 0; i < model_contour.size(); i++)
	{
		Point pt;
		pt.x = result.at<double>(0, i) / result.at<double>(2, i);
		pt.y = result.at<double>(1, i) / result.at<double>(2, i);
		pts.push_back(pt);
	}
	contour.swap(pts);
}

void CGDetector::find_borders(Mat mask_aa, vector<int> &borders)
{
	int border_x = mask_aa.cols / 8;
	int border_y = mask_aa.rows / 14;

	Mat mat_left = mask_aa.colRange(0, border_x);
	Mat mat_right = mask_aa.colRange(mask_aa.cols - border_x, mask_aa.cols - 1);
	Mat mat_up = mask_aa.rowRange(0, border_y);
	Mat mat_down = mask_aa.rowRange(mask_aa.rows - border_y, mask_aa.rows - 1);

	borders.push_back(find_edge_vertical(mat_left));
	borders.push_back(find_edge_vertical(mat_right) + mask_aa.cols - border_x);
	borders.push_back(find_edge_horizon(mat_up));
	borders.push_back(find_edge_horizon(mat_down) + mask_aa.rows - border_y);
}

void CGDetector::detect_lr_contour(Mat gray, vector<int> &borders, vector<Point> &contour_left, vector<Point> &contour_right)
{
	//获取左右边界
	int left_pos = borders[0];
	int right_pos = borders[1];
	//计算上下留白
	int border_y = min(borders[2], gray.rows - borders[2]) + 300; 
	//左右边界拟合
	int step = 100, r = 70, y_c = gray.rows / 2, rect_h = 2 * step, rect_w = 2 * r;
	//计算ROI集合
	vector<Rect> rect_left, rect_right;
	for (int y = y_c - rect_h; y >= border_y; y -= rect_h)
	{
		Mat show; cvtColor(gray, show, COLOR_GRAY2BGR);
		Rect rect_left_up(left_pos - r, y, rect_w, rect_h); if (rect_left_up.x < 0) rect_left_up.x = 0;
		Rect rect_right_up(right_pos - r, y, rect_w, rect_h); if (rect_right_up.x + rect_w > gray.cols) rect_right_up.x = gray.cols - rect_w;
		Rect rect_left_down = rect_left_up; rect_left_down.y = 2 * y_c - rect_left_up.y - rect_h;
		Rect rect_right_down = rect_right_up; rect_right_down.y = 2 * y_c - rect_right_up.y - rect_h;

		rect_left.push_back(rect_left_up);
		rect_left.push_back(rect_left_down);
		rect_right.push_back(rect_right_up);
		rect_right.push_back(rect_right_down);
	}
	//扫描
	for (int i = 0; i < rect_left.size(); i++)
	{
		int x_left = find_edge_vertical(gray(rect_left[i]), true) + rect_left[i].x;
		int y_left = rect_left[i].y + step;
		int x_right = find_edge_vertical(gray(rect_right[i]), true) + rect_right[i].x;
		int y_right = rect_right[i].y + step;
		contour_left.push_back(Point(x_left, y_left));
		contour_right.push_back(Point(x_right, y_right));
		//Mat show; cvtColor(gray, show, COLOR_GRAY2BGR);
		//rectangle(show, rect_left[i], Scalar(0, 0, 255), -1);
		//rectangle(show, rect_right[i], Scalar(0, 0, 255), -1);
		//circle(show, Point(x_left, y_left), 10, Scalar(0, 255, 0), -1);
		//circle(show, Point(x_right, y_right), 10, Scalar(0, 255, 0), -1);
		//namedWindow("hull", 0); imshow("hull", show); waitKey(0);
	}
	sort(contour_left.begin(), contour_left.end(), [](Point pt1, Point pt2) {return pt1.y < pt2.y; });
	sort(contour_right.begin(), contour_right.end(), [](Point pt1, Point pt2) {return pt1.y > pt2.y; });
}

void CGDetector::detect_ud_contour(Mat gray, vector<int> &borders, vector<Point>& contour_up, vector<Point>& contour_down)
{
	//获取上下边界
	int up_pos = borders[2];
	int down_pos = borders[3];
	//左右边界留白
	int border_x = min(borders[0], gray.rows - borders[1]) + 300;
	//上下边界拟合
	int step = 100, r = 70, x_c = gray.cols / 2, rect_h = 2 * r, rect_w = 2 * step;
	//计算ROI集合
	vector<Rect> rect_up_left, rect_down_left, rect_up_right, rect_down_right;
	for (int x = x_c - rect_w; x >= 0; x -= rect_w)
	{
		if (x + step < border_x || x + step > gray.cols - border_x) break;
		Mat show; cvtColor(gray, show, COLOR_GRAY2BGR);
		Rect up_left(x, up_pos - r, rect_w, rect_h); if (up_left.y < 0) up_left.y = 0;
		Rect down_left(x, down_pos - r, rect_w, rect_h); if (down_left.y + rect_h > gray.rows) down_left.y = gray.rows - rect_h;
		Rect up_right = up_left; up_right.x = 2 * x_c - up_left.x - rect_w;
		Rect down_right = down_left; down_right.x = 2 * x_c - down_left.x - rect_w;

		rect_up_left.push_back(up_left);
		rect_up_right.push_back(up_right);
		rect_down_left.push_back(down_left);
		rect_down_right.push_back(down_right);
	}
	//扫描上左
	int pos = r;
	for (auto rect : rect_up_left)
	{
		//Mat show; cvtColor(gray, show, COLOR_GRAY2BGR); rectangle(show, rect, Scalar(0, 0, 255), -1); namedWindow("hull", 0); imshow("hull", show); waitKey(1);
		pos = find_edge_horizon(gray(rect), pos);
		if (pos == -1) break;
		contour_up.push_back(Point(rect.x + step, rect.y + pos));
	}
	//扫描上右
	pos = r;
	for (auto rect : rect_up_right)
	{
		//Mat show; cvtColor(gray, show, COLOR_GRAY2BGR); rectangle(show, rect, Scalar(0, 0, 255), -1); namedWindow("hull", 0); imshow("hull", show); waitKey(1);
		pos = find_edge_horizon(gray(rect), pos);
		if (pos == -1) break;
		contour_up.push_back(Point(rect.x + step, rect.y + pos));
	}
	//扫描下左
	pos = r;
	for (auto rect : rect_down_left)
	{
		//Mat show; cvtColor(gray, show, COLOR_GRAY2BGR); rectangle(show, rect, Scalar(0, 0, 255), -1); namedWindow("hull", 0); imshow("hull", show); waitKey(1);
		pos = find_edge_horizon(gray(rect), pos);
		if (pos == -1) break;
		contour_down.push_back(Point(rect.x + step, rect.y + pos));
	}
	//扫描下右
	pos = r;
	for (auto rect : rect_down_right)
	{
		//Mat show; cvtColor(gray, show, COLOR_GRAY2BGR); rectangle(show, rect, Scalar(0, 0, 255), -1); namedWindow("hull", 0); imshow("hull", show); waitKey(1);
		pos = find_edge_horizon(gray(rect), pos);
		if (pos == -1) break;
		contour_down.push_back(Point(rect.x + step, rect.y + pos));
	}
	sort(contour_up.begin(), contour_up.end(), [](Point pt1, Point pt2) {return pt1.x > pt2.x; });
	sort(contour_down.begin(), contour_down.end(), [](Point pt1, Point pt2) {return pt1.x < pt2.x; });
}

int CGDetector::find_edge_vertical(Mat gray, bool center_first)
{
	vector<int> p_h;										//namedWindow("gray", 0); imshow("gray", gray);
	project_gray(gray, p_h); vec_gradient(p_h, p_h, 10); vec_smooth(p_h, p_h, 5);
	int pos = 0;
	if (center_first)
	{
		vec_findPeaks(p_h, p_h, 10);						//namedWindow("hist", 0); imshow("hist", draw_hist(p_h)); waitKey();
		int min_diff = p_h.size(), x = gray.cols / 2;
		for (int i = 0; i < p_h.size(); i++)
		{
			if (p_h[i])
			{
				int diff = abs(x - i);
				if (diff < min_diff)
				{
					min_diff = diff;
					pos = i;
				}
			}
		}
	}
	else
	{
		pos = max_element(p_h.begin(), p_h.end()) - p_h.begin();
	}
	return pos;
}

int CGDetector::find_edge_horizon(Mat gray, int base_pos)
{
	vector<int> hist;																		//namedWindow("gray_roi", 0); imshow("gray_roi", gray);
	project_gray(gray, hist, VERTICAL);														//imshow("hist", draw_hist(hist, VERTICAL));
	vector<int> peaks; vec_gradient(hist, peaks, 10); vec_smooth(peaks, peaks, 5);
	int pos = 0;
	if (base_pos != -1)
	{
		vec_findPeaks(peaks, peaks, 10);													//imshow("peak", draw_hist(peaks, VERTICAL));
		int min_diff = peaks.size();
		for (int i = 0; i < peaks.size(); i++)
		{
			if (peaks[i])
			{
				int diff = abs(base_pos - i);
				if (diff < min_diff)
				{
					min_diff = diff;
					pos = i;
				}
			}
		}
	}
	else
	{
		return max_element(peaks.begin(), peaks.end()) - peaks.begin();
	}
	double ave_up = accumulate(hist.begin(), hist.begin() + pos, 0) / pos;
	double ave_down = accumulate(hist.begin() + pos, hist.end(), 0) / (hist.size() - pos);
	double rate = min(ave_up, ave_down) / max(ave_up, ave_down);
	//cout << "pos:" << pos << "/" << base_pos << " ave_up:" << ave_up << " ave_down:" << ave_down << " rate:" << rate << endl; waitKey(1);
	if (rate > 0.65) { /*cout << "-------------------" << endl;*/ return -1; }
	return pos;
}

void CGDetector::mix_contour_aa(vector<Point> &contour_aa_org, vector<vector<Point>> &contour_fit, vector<Point> &contour_aa)
{
	//过滤空的点
	vector<vector<Point>> temp;
	for (auto &contour: contour_fit)
	{
		if (contour.size()) temp.push_back(contour);
	}
	//混合轮廓
	for (int i = 0; i < temp.size(); i++)
	{
		contour_aa.insert(contour_aa.end(), temp[i].begin(), temp[i].end());
		Point pt_s = temp[i][temp[i].size() - 1], pt_e = temp[(i + 1) % temp.size()][0];
		int id_s, id_e, min_s = 1000, min_e = 1000;
		for (int i = 0; i < contour_aa_org.size(); i++)
		{
			int x = contour_aa_org[i].x;
			int y = contour_aa_org[i].y;
			int dis = abs(pt_s.x - x) + abs(pt_s.y - y);
			if (dis < min_s) { min_s = dis; id_s = i; }
			dis = abs(pt_e.x - x) + abs(pt_e.y - y);
			if (dis < min_e) { min_e = dis; id_e = i; }
		}
		if (id_s <id_e)
		{
			contour_aa.insert(contour_aa.end(), contour_aa_org.begin() + id_s, contour_aa_org.begin() + id_e);
		}
		else
		{
			contour_aa.insert(contour_aa.end(), contour_aa_org.begin() + id_s, contour_aa_org.end());
			contour_aa.insert(contour_aa.end(), contour_aa_org.begin(), contour_aa_org.begin() + id_e);
		}
	}
}

void CGDetector::filter_detect(vector<defect_info>& defect_info_vec)
{
	sort(defect_info_vec.begin(), defect_info_vec.end(), [](defect_info i1, defect_info i2) {return i1.defect_area1_exibit_to_client < i2.defect_area1_exibit_to_client; });
	vector<defect_info> temp;
	for (int i = 0; i < defect_info_vec.size(); i++)
	{
		Point pt = defect_info_vec[i].defect_location_exibit_to_client;
		bool flag = true;
		for (int j = i + 1; j < defect_info_vec.size(); j++)
		{
			RotatedRect rect = defect_info_vec[j].min_area_rect;
			vector<Point2f> pts(4); rect.points(pts.data());
			if (pointPolygonTest(pts, pt, false) > 0)
			{
				flag = false; break;
			}
		}
		if (flag) temp.push_back(defect_info_vec[i]);
	}
	defect_info_vec.swap(temp);
}

void CGDetector::marginal_neglect(vector<Point> &src, vector<Point> &dst, int pix)
{
	vector<Point> temp;
	temp.insert(temp.end(), src.end() - 30, src.end()); 
	temp.insert(temp.end(), src.begin(), src.end());
	temp.insert(temp.end(), src.begin(), src.begin() + 30); 

	vector<Point> dst_temp;
	for (int i = 0; i < src.size(); i++)
	{
		Point p_b = temp[i];
		Point p_f = temp[i + 60];
		Point p_c = src[i];
		double x = p_f.y - p_b.y, y = p_b.x - p_f.x, r = sqrt(x*x + y*y);
		double s = pix / r;
		x *= s;
		y *= s;
		dst_temp.push_back(p_c + Point(x, y));
	}
	dst.swap(dst_temp);
}

void CGDetector::defect_detect(Mat mat_phone, vector<Point> &contour_aa, vector<defect_info> &defect_info_vec)
{
	//边缘忽略
	marginal_neglect(contour_aa, contour_aa, 100);
	//计算AA区mask
	Mat mask_aa = Mat::zeros(mat_phone.size(), mat_phone.type());
	vector<vector<Point>> container; container.push_back(contour_aa);
	drawContours(mask_aa, container, 0, Scalar(255), -1);							//namedWindow("mask_aa", 0); imshow("mask_aa", mask_aa); 
	//计算AA区缺陷图
	int thresh_aa = find_thresh_aa(mat_phone);
	Mat bin1; threshold(mat_phone, bin1, thresh_aa - m_grey_difference_negative, 255, THRESH_BINARY);
	Mat bin2; threshold(mat_phone, bin2, thresh_aa + m_grey_difference_positive, 255, THRESH_BINARY);
	Mat bin = bin1 - bin2;
	bitwise_and(bin, mask_aa, bin);
	absdiff(bin, mask_aa, bin);
	//寻找大瑕疵	
	Mat bin4; pyrDown(bin, bin4); pyrDown(bin4, bin4); 
	threshold(bin4, bin4, 30, 255, THRESH_BINARY);									//namedWindow("bin4", 0); imshow("bin4", bin4); waitKey();
	fillHole(bin4, bin4);															//namedWindow("bin4", 0); imshow("bin4", bin4); waitKey();
	morphology(bin4, bin4, MORPH_CLOSE, MORPH_ELLIPSE, Size(11, 11), 1);			//namedWindow("bin4", 0); imshow("bin4", bin4); waitKey();
	morphology(bin4, bin4, MORPH_OPEN, MORPH_ELLIPSE, Size(3, 3), 1);				//namedWindow("bin4", 0); imshow("bin4", bin4); waitKey();
	fillHole(bin4, bin4);															//namedWindow("bin4", 0); imshow("bin4", bin4); waitKey();
	Mat gray4; pyrDown(mat_phone, gray4); pyrDown(gray4, gray4);
	vector<defect_info> info_temp; calculate_param(gray4, bin4, info_temp);
	if (info_temp.size())
	{
		if (info_temp[0].defect_area2_exibit_to_client > m_max_area / 16)
		{
			info_temp[0].defect_area1_exibit_to_client *= 16;
			info_temp[0].defect_area2_exibit_to_client *= 16;
			info_temp[0].defect_height_exibit_to_client *= 4;
			info_temp[0].defect_width_exibit_to_client *= 4;
			info_temp[0].defect_location_exibit_to_client *= 4;
			defect_info_vec.push_back(info_temp[0]);
			return;
		}
	}
	//寻找小瑕疵 
	calculate_param(mat_phone, bin, defect_info_vec);
}

void CGDetector::make_model(Size size, vector<Point> &contour, string path_model)
{
	ofstream os(path_model);
	os << 0 << " " << 0 << endl;
	os << 0 << " " << size.height << endl;
	os << size.width << " " << size.height << endl;
	os << size.width << " " << 0 << endl;
	for (auto pt : contour)
	{
		os << pt.x << " " << pt.y << endl;
	}
}

void CGDetector::set_aa_region_positive(int n)
{
	m_aa_region_positive = n;
}

void CGDetector::set_aa_region_negative(int n)
{
	m_aa_region_negative = n;
}

void CGDetector::set_grey_difference_positive(int n)
{
	m_grey_difference_positive = n;
}

void CGDetector::set_grey_difference_negative(int n)
{
	m_grey_difference_negative = n;
}

void CGDetector::set_num_for_stop_inspection(int n)
{
	m_num_for_stop_inspection = n;
}

void CGDetector::set_max_area(double area)
{
	m_max_area = area;
}

void CGDetector::set_min_area(double area)
{
	m_min_area = area;
}

void CGDetector::load_model(string dile_model, vector<Point2f> &vertex, vector<Point> &contour)
{
	ifstream is(dile_model);
	for (int i = 0; i < 4; i++)
	{
		Point2f pt;
		is >> pt.x >> pt.y;
		vertex.push_back(pt);
	}
	while (!is.eof())
	{
		Point pt;
		is >> pt.x >> pt.y;
		if (pt.x == 0 && pt.y == 0) break;
		contour.push_back(pt);
	}
}

void CGDetector::calculate_param(Mat gray, Mat bin, vector<defect_info> &defect_info_vec)
{
	vector<defect_info> defect_sample_temp;
	vector<vector<Point>> defects; findContours(bin, defects, RETR_LIST, CHAIN_APPROX_NONE);
	Mat color; cvtColor(gray, color, COLOR_GRAY2BGR);
	for (int i = 0; i < defects.size(); i++)
	{
		double arc_area = contourArea(defects[i]);//缺陷面积
		if (arc_area < m_min_area) continue;
		double arc_length = arcLength(defects[i], true);//缺陷周长
		Rect roi = boundingRect(defects[i]);//外切矩形
		Mat mask_roi = bin(roi), gray_roi = gray(roi);
		double min, max; minMaxLoc(gray_roi, &min, &max, 0, 0, mask_roi);//缺陷灰度极值
		RotatedRect rotated_rect = minAreaRect(defects[i]);//最小矩形
		vector<Point2f> pts(4); rotated_rect.points(pts.data());
		//获取缺陷样本图片
		Rect rect_show = boundingRect(pts);
		if (rect_show.width < rect_show.height)
		{
			rect_show.x = rect_show.x + rect_show.width / 2 - rect_show.height / 2;
			rect_show.width = rect_show.height;
		}
		else
		{
			rect_show.y = rect_show.y + rect_show.height / 2 - rect_show.width / 2;
			rect_show.height = rect_show.width;
		}
		rect_show.x -= rect_show.width / 10;
		rect_show.y -= rect_show.height / 10;
		rect_show.width += rect_show.width / 5;
		rect_show.height += rect_show.height / 5;
		if (rect_show.x < 0) rect_show.x = 0;
		if (rect_show.x + rect_show.width > gray.cols) rect_show.width = gray.cols - rect_show.x;
		if (rect_show.y < 0) rect_show.y = 0;
		if (rect_show.y + rect_show.height > gray.rows) rect_show.height = gray.rows - rect_show.y;
		double rate = 200.0 / MAX(rect_show.width, rect_show.height);
		Mat mat_sample; resize(color(rect_show), mat_sample, Size(rect_show.width*rate, rect_show.height*rate));
		for (auto &pt : pts)
		{
			pt.x -= rect_show.tl().x; pt.x *= rate;
			pt.y -= rect_show.tl().y; pt.y *= rate;
		}
		for (int i = 0; i < pts.size(); i++)
		{
			line(mat_sample, pts[i], pts[(i + 1) % 4], Scalar(0, 0, 255));
		}
		//缺陷各维度信息汇总
		defect_info info;
		info.min_area_rect = rotated_rect;
		info.defect_sample = mat_sample;
		info.defect_area1_exibit_to_client = rotated_rect.size.area();
		info.defect_area2_exibit_to_client = arc_area;
		info.defect_width_exibit_to_client = rotated_rect.size.width;
		info.defect_height_exibit_to_client = rotated_rect.size.height;
		info.defect_location_exibit_to_client = rotated_rect.center;
		info.defect_mean_grey_value_exibit_to_client = mean(gray_roi, mask_roi)[0];//平均灰度值
		info.defect_max_min_grey_value_difference_exibit_to_client = max - min;
		info.defect_roundness_exibit_to_client = arc_length * arc_length / arc_area;
		info.defect_roundness_ratio_exibit_to_client = info.defect_area1_exibit_to_client / arc_area * info.defect_roundness_exibit_to_client;
		defect_sample_temp.push_back(info);
	}
	//过滤掉内部缺陷
	filter_detect(defect_sample_temp);
	//缺陷排序
	sort(
		defect_sample_temp.begin(),
		defect_sample_temp.end(),
		[](defect_info i1, defect_info i2) {return i1.defect_area2_exibit_to_client > i2.defect_area2_exibit_to_client; });
	defect_info_vec.clear();
	for (int i = 0; i < MIN(m_num_for_stop_inspection, defect_sample_temp.size()); i++)
	{
		defect_info info = defect_sample_temp[i];
		info.defect_id_exibit_to_client = i;
		defect_info_vec.push_back(info);
	}
}

void CGDetector::cg_image_processing(Mat src, string file_model,Mat &mat_phone, Mat &mat_aa, std::vector<defect_info>& defect_info_vec)
{
	Mat gray; grayScale(src, gray);
	extract_phone(gray, mat_phone);
	vector<Point> contour_aa; extract_aa_contour(mat_phone, file_model, contour_aa, mat_aa);
	defect_detect(mat_phone, contour_aa, defect_info_vec);
}

void CGDetector::print_defect_message(std::vector<defect_info>& defect_sample_info_vec)
{
	for (auto info : defect_sample_info_vec)
	{
		cout << string(100, '#') << endl;
		cout << "defect_id_exibit_to_client:   " << info.defect_id_exibit_to_client << endl;
		cout << "defect_location_exibit_to_client:   " << info.defect_location_exibit_to_client << endl;
		cout << "defect_area1_exibit_to_client:   " << info.defect_area1_exibit_to_client << endl;
		cout << "defect_area2_exibit_to_client:   " << info.defect_area2_exibit_to_client << endl;
		cout << "defect_width_exibit_to_client:   " << info.defect_width_exibit_to_client << endl;
		cout << "defect_height_exibit_to_client:   " << info.defect_height_exibit_to_client << endl;
		cout << "defect_mean_grey_value_exibit_to_client:   " << info.defect_mean_grey_value_exibit_to_client << endl;
		cout << "defect_max_min_grey_value_difference_exibit_to_client:   " << info.defect_max_min_grey_value_difference_exibit_to_client << endl;
		cout << "defect_roundness_exibit_to_client:   " << info.defect_roundness_exibit_to_client << endl;
		cout << "defect_roundness_ratio_exibit_to_client:   " << info.defect_roundness_ratio_exibit_to_client << endl;
	}
}

void CGDetector::output_defect_message(std::ofstream & os, defect_info& info)
{
	os << "defect_location_exibit_to_client:   " << info.defect_location_exibit_to_client << endl;
	os << "defect_area1_exibit_to_client:   " << info.defect_area1_exibit_to_client << endl;
	os << "defect_area2_exibit_to_client:   " << info.defect_area2_exibit_to_client << endl;
	os << "defect_width_exibit_to_client:   " << info.defect_width_exibit_to_client << endl;
	os << "defect_height_exibit_to_client:   " << info.defect_height_exibit_to_client << endl;
	os << "defect_mean_grey_value_exibit_to_client:   " << info.defect_mean_grey_value_exibit_to_client << endl;
	os << "defect_max_min_grey_value_difference_exibit_to_client:   " << info.defect_max_min_grey_value_difference_exibit_to_client << endl;
	os << "defect_roundness_exibit_to_client:   " << info.defect_roundness_exibit_to_client << endl;
	os << "defect_roundness_ratio_exibit_to_client:   " << info.defect_roundness_ratio_exibit_to_client << endl;
}
