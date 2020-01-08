#pragma once
#include <lib_quickcv.h>
using namespace hjh;
using namespace cv;
using namespace std;

struct defect_info
{
	cv::Mat defect_sample;
	cv::RotatedRect min_area_rect;
	cv::Point defect_location_exibit_to_client;

	int defect_id_exibit_to_client;
	float defect_area1_exibit_to_client;
	float defect_area2_exibit_to_client;
	float defect_width_exibit_to_client;
	float defect_height_exibit_to_client;
	float defect_mean_grey_value_exibit_to_client;
	float defect_max_min_grey_value_difference_exibit_to_client;
	float defect_roundness_exibit_to_client;
	float defect_roundness_ratio_exibit_to_client;
};


class CGDetector
{
public:
	CGDetector();
	~CGDetector();
	void make_model(Size size, vector<Point> &contour, string path_model);

	void cg_image_processing(Mat src, string file_model, Mat &mat_phone, Mat &mat_aa, std::vector<defect_info>& defect_info_vec);

	void extract_phone(Mat gray, Mat &dst);
	void extract_aa_contour(Mat mat_phone, string file_model, vector<Point> &contour_aa, Mat &mat_aa);
	void defect_detect(Mat mat_phone, vector<Point> &contour_aa, vector<defect_info> &defect_info_vec);

	void print_defect_message(std::vector<defect_info>& defect_sample_info_vec);
	void output_defect_message(std::ofstream & os, defect_info& info);

	void set_aa_region_positive(int n);
	void set_aa_region_negative(int n);
	void set_grey_difference_positive(int n);
	void set_grey_difference_negative(int n);
	void set_num_for_stop_inspection(int n);
	void set_max_area(double area);
	void set_min_area(double area);
private:
	int find_thresh_phone(cv::Mat gray);
	int find_thresh_aa(cv::Mat gray);
	void extract_aa_contour_auto(Mat mat_phone, vector<Point> &contour_aa);
	void extract_aa_contour_model(Mat mat_phone, string file_model, vector<Point>& contour);
	void marginal_neglect(vector<Point> &src, vector<Point> &dst, int pix);
	void find_borders(Mat mask_aa, vector<int> &borders);
	int find_edge_vertical(Mat gray, bool center_first = false);
	int find_edge_horizon(Mat gray, int base_pos = -1);
	void detect_lr_contour(Mat gray, vector<int> &borders, vector<Point> &contour_left, vector<Point> &contour_right);
	void detect_ud_contour(Mat gray, vector<int> &borders, vector<Point> &contour_up, vector<Point> &contour_down);
	void mix_contour_aa(vector<Point> &contour_aa_org, vector<vector<Point>> &contour_fit, vector<Point> &contour_aa);
	void filter_detect(vector<defect_info>& defect_info_vec);
	void load_model(string dile_model, vector<Point2f>& vertex, vector<Point>& contour);
	void calculate_param(Mat gray, Mat bin, vector<defect_info> &defect_info_vec);
private:
	int m_aa_region_positive;
	int m_aa_region_negative;
	int m_grey_difference_positive;
	int m_grey_difference_negative;
	int m_num_for_stop_inspection;
	double m_max_area;
	double m_min_area;
};

