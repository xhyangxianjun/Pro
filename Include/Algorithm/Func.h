#pragma once

#include <windows.h>
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>
#include <stdio.h>
#include "opencv2/core.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/flann.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/ml.hpp"
#include <math.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <opencv2/phase_unwrapping/histogramphaseunwrapping.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <opencv2/phase_unwrapping.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/opencv.hpp>
#include <unordered_map>
#include <math.h>
#include <iomanip>
#include <numeric>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <algorithm>
#include <vector>
#include <iterator>
#include <io.h>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>
#include <direct.h>
#include <conio.h>
#include <ctype.h>
#include <ctime>

#include <math.h>

#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <stdio.h>
#include "opencv2/core.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/flann.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/ml.hpp"
#include <math.h>

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>

#include <io.h>
#include <cstdlib>
#include <string>
#include <ctime>

#include "queue"
#include "Windows.h"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc_c.h"

#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include "device_functions.h"

using namespace cv::xfeatures2d;
using namespace cv;
using namespace std;
using namespace ml;

#ifdef FUNCTION_IN_ALL_API
#else                                                                            
#define FUNCTION_IN_ALL_API __declspec(dllimport) 
#endif   

#ifndef  FUNC_H
#define FUNC_H

//extern "C" void __declspec(dllexport) create_binary_image_in_gpu(
//	const Mat *const aa_region_mask,
//	Mat *black_background_exception_image,
//	const float current_col_mean_grey_value,
//	const float grey_difference,
//	const int current_col);

struct __declspec(dllexport) defect_sample_info
{
	bool defect_judge;
	bool super_defect_judge;

	int defect_sample_id;

	Rect defect_sample_big_rect;
	RotatedRect defect_sample_middle_rotated_rect;
	RotatedRect defect_sample_small_rotated_rect;

	Mat defect_sample;

	Mat defect_sample_big_mask;
	Mat defect_sample_middle_mask;
	Mat defect_sample_small_mask;
	Mat defect_sample_mask;

	Mat defect_sample_with_middle_rotated_rect;
	Mat defect_sample_with_small_rotated_rect;

	Mat defect_sample_with_filter;

	//float defect_sample_with_filter_mean_grey_value_without_defect_area;
	float defect_sample_big_rect_stantard_background_grey_value;

	int defect_id_exibit_to_client;
	Point defect_location_exibit_to_client;
	float defect_area1_exibit_to_client;
	float defect_area2_exibit_to_client;
	float defect_width_exibit_to_client;
	float defect_height_exibit_to_client;
	float defect_mean_grey_value_exibit_to_client;
	float defect_max_min_grey_value_difference_exibit_to_client;
	float defect_roundness_exibit_to_client;
	float defect_roundness_ratio_exibit_to_client;
};

class __declspec(dllexport) LabelAndContour
{
public:
	//LabelAndContour() {};
	//~LabelAndContour() {};

	void set_label(int current_label);
	void set_contour(vector<vector<Point>> current_contour);
	void set_hierarchy(vector<Vec4i> current_hierarchy);
	int get_label();
	vector<vector<Point>> get_contour();
	vector<Vec4i> get_hierarchy();

private:
	int label;
	vector<vector<Point>> contour;
	vector<Vec4i> hierarchy;
};








extern "C" class FUNCTION_IN_ALL_API FunctionInAll
{
public:
	//FunctionInAll() {};
	//~FunctionInAll() {};

	//bool contour_area_comparing(vector<Point> &contour1, vector<Point> &contour2);

	//void current_col_binary_processing_kernal(
	//	float *current_col,
	//	const float current_col_mean_grey_value,
	//	const float grey_difference,
	//	const int current_rows_num);

	void only_for_test_in_gpu(
		float *image_data,
		const int image_rows,
		const int image_cols);

	void create_binary_image_in_gpu(
		const int x_start,
		const int x_end,
		const Mat *const aa_region_mask,
		map<int, float> *standard_grey_value_for_each_col,
		const float grey_difference,
		float *black_background_exception_image_data);

	void set_useless_col_to_zero_in_gpu(
		float *black_background_exception_image_8uc1_data,
		const int current_col,
		const int image_rows,
		const int image_cols);



	void set_enable_gpu(const bool current_enable_gpu);

	bool get_enable_gpu();

	void set_enable_cpu_multi_thread(const bool current_enable_cpu_multi_thread);

	bool get_enable_cpu_multi_thread();

	void set_enable_develop_mode(const bool current_enable_develop_mode);

	bool get_enable_develop_mode();

	void set_aa_region_value_min(const int current_aa_region_value_min);

	int get_aa_region_value_min();

	void set_aa_region_value_max(const int current_aa_region_value_max);

	int get_aa_region_value_max();

	void set_grey_difference_positive(const float current_grey_difference_positive);

	float get_grey_difference_positive();

	void set_grey_difference_negative(const float current_grey_difference_negative);

	float get_grey_difference_negative();

	void set_exception_dilate_element_width(const int current_exception_dilate_element_width);

	int get_exception_dilate_element_width();

	void set_exception_dilate_element_height(const int current_exception_dilate_element_height);

	int get_exception_dilate_element_height();

	void set_loop_stride_to_decide_stantard_grey_value(const int current_loop_stride_to_decide_stantard_grey_value);

	int get_loop_stride_to_decide_stantard_grey_value();

	void set_zoom_distance(const float current_zoom_distance);

	float get_zoom_distance();

	void set_defect_area_for_stop_inspection(const int current_defect_area_for_stop_inspection);

	int get_defect_area_for_stop_inspection();

	void set_defect_num_for_stop_inspection(const int current_defect_num_for_stop_inspection);

	int get_defect_num_for_stop_inspection();

	void set_defect_acceptable_minimum_area(const int current_defect_acceptable_minimum_area);

	int get_defect_acceptable_minimum_area();
	
	void set_aa_region_extraction_judge(const bool current_aa_region_extraction_judge);

	bool get_aa_region_extraction_judge();

	void set_standard_grey_value(const float current_standard_grey_value);

	float get_standard_grey_value();
		
	void from_8uc1_to_8uc3(
		const Mat *const mat_input,
		Mat *mat_output);

	void from_32fc1_to_8uc1(
		const Mat *const mat_input,
		Mat *mat_output);

	void from_8uc3_to_8uc1(
		const Mat *const mat_input,
		Mat *mat_output);

	void from_8uc1_to_32fc1(
		const Mat *const mat_input,
		Mat *mat_output);

	void show_image(
		const Mat *const current_mat,
		const string current_mat_name);

	void show_type_of_mat(
		const Mat *const current_mat);

	void directory_preparation(
		const string root_directory,
		string *root_directory_name,
		string *sub_directory_name_region_separation,
		string *sub_directory_name_defect_in_aa_region,
		string *sub_directory_name_defect_sample);

	void calculate_kmeans_for_color_image(
		const Mat *const original_color_image_8uc3,
		Mat *kmeans_image_8uc3,
		const int cluster_count);

	void target_extraction_for_gray_image(
		const Mat *const original_color_image,
		Mat *hsv_image,
		Mat *pre_target_mask,
		Mat *target_mask,
		Mat *target_image,
		const int hue_min,
		const int hue_max,
		const int saturate_min,
		const int saturate_max,
		const int value_min,
		const int value_max);

	void target_extraction_for_gray_image_with_compression_processing(
		const Mat *const original_color_image,
		const float compression_ratio,
		Mat *hsv_image,
		Mat *pre_target_mask,
		Mat *target_mask,
		Mat *target_image,
		vector<vector<Point>> *white_block_contours,
		const int hue_min,
		const int hue_max,
		const int saturate_min,
		const int saturate_max,
		const int value_min,
		const int value_max);

	void target_extraction_for_gray_image(
		const Mat *const original_color_image,
		Mat *hsv_image,
		Mat *pre_target_mask,
		Mat *target_mask,
		Mat *target_image,
		vector<vector<Point>> *white_block_contours,
		const int hue_min,
		const int hue_max,
		const int saturate_min,
		const int saturate_max,
		const int value_min,
		const int value_max);

	bool map_value_comparing(pair<float, int> &pair1, pair<float, int> &pair2);

	void target_dissection_for_gray_image(
		const Mat *const target_image_8uc3,
		const Mat *const target_mask_8uc1,
		Mat *aa_region_8uc3,
		Mat *aa_region_pre_mask_8uc1,
		Mat *aa_region_mask_8uc1,
		Mat *bm_region_8uc3,
		Mat *bm_region_mask_8uc1,
		const int aa_region_hue_min_for_mode2,
		const int aa_region_hue_max_for_mode2,
		const int aa_region_saturate_min_for_mode2,
		const int aa_region_saturate_max_for_mode2,
		const int aa_region_value_min_for_mode2,
		const int aa_region_value_max_for_mode2,
		const int mode);

	void sample_extraction_for_gray_image(
		const Mat *const original_color_image_8uc3,
		const Point2f center_point,
		const float x_side_length,
		const float y_side_length,
		Mat *original_color_image_with_cube,
		Mat *sample_image);

	void calculate_backproject_for_gray_image(
		Mat *original_color_image,
		Mat *sample_image,
		Mat *sample_hist_image,
		Mat *backproject_image);

	void target_detection(
		Mat *scene_image,
		Mat *target_image,
		Mat *original_matches_image,
		Mat *good_matches_image,
		Mat *ransac_matches_image,
		Mat *target_extracted_in_scene_image,
		Mat *target_extracted_image,
		vector<Point2f> *recorded_corners);

	void bm_region_processing(
		const Mat *const bm_region_8uc3,
		vector<Point2f> *top_component_corners,
		vector<Point2f> *bottom_component_corners,
		const int stop_threshold);

	void aa_region_extraction(
		const Mat *const original_cg_image_8uc3,
		Mat *aa_region_8uc3,
		Mat *aa_region_pre_mask_8uc1,
		Mat *aa_region_mask_8uc1,
		vector<vector<Point>> *white_block_contours,
		const int hue_min,
		const int hue_max,
		const int saturate_min,
		const int saturate_max,
		const int value_min,
		const int value_max,
		const bool aa_region_extration_with_copression_jugde);

	void aa_bm_dissection(
		const bool only_aa,
		const Mat *const original_cg_image_8uc3,
		Mat *aa_region_8uc3,
		Mat *aa_region_pre_mask_8uc1,
		Mat *aa_region_mask_8uc1,
		Mat *bm_region_8uc3,
		Mat *bm_region_mask_8uc1,
		Mat *real_bm_region_8uc3,
		Mat *real_bm_region_mask_8uc1);

	//void calculate_white_area_contour_for_exceptions(
	//	const Mat *const binary_image_8uc1,
	//	vector<vector<Point> > *white_block_contours,
	//	vector<Vec4i> *hierarchy);

	void calculate_white_area_contour_info_for_binary_image(
		const Mat *const binary_image_8uc1,
		Mat *white_block_contour_image,
		Mat *block_contour_image,
		vector<float> *white_block_contour_area_vec,
		vector<float> *block_contour_area_vec,
		vector<vector<Point> > *white_block_contours,
		vector<Vec4i> *hierarchy);

	void calculate_distance_between_two_point(
		const Point2f *const p1,
		const Point2f *const p2,
		float *distance);

	void calculate_warp_affine(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *mask_image_with_corner_points,
		Mat *warp_affine_image,
		Mat *warp_affine_mask_image,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_max_and_min_in_vec(vector<float> *vec, float *max, float *min);

	void standard_deviation_calculation(vector<float> *vec, float *sum, float *mean, float *accum, float *stdev);

	void preprocessing_image(
		Mat *original_color_image,
		Mat *output_image,
		Mat *mask_image);

	void calculate_canny_for_gray_image(Mat *original_color_image, Mat *edge_image, Mat *integral_image1, Mat *integral_image2, Mat *integral_image3, string file_path_and_name);

	void calculate_canny_for_gray_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *edge_image,
		Mat *integral_image1,
		Mat *integral_image2,
		Mat *integral_image3,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_sobel_for_gray_image(Mat *original_color_image, Mat *edge_image, Mat *integral_image1, Mat *integral_image2, Mat *integral_image3, string file_path_and_name);

	void calculate_sobel_for_gray_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *edge_image,
		Mat *integral_image1,
		Mat *integral_image2,
		Mat *integral_image3,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_contour_for_gray_image(Mat *original_color_image, Mat *adaptive_threshold_image, Mat *contour_image, string file_path_and_name_adaptive_threshold, string file_path_and_name_contour);

	void calculate_contour_for_gray_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *adaptive_threshold_image,
		Mat *contour_image,
		string file_path_and_name_contour,
		map<string, float> *parameters);

	void calculate_hist_for_gray_image(Mat *original_gray_image, Mat *hist_image);

	void calculate_hist_for_gray_image(
		Mat *original_gray_image,
		Mat *mask_image,
		Mat *hist_image,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_h_s_hist_for_color_image(Mat *original_color_image, Mat *hist_image);

	void calculate_h_s_v_hist_for_color_image(Mat *original_color_image, Mat *h, Mat *s, Mat *v, Mat *hsv_image, Mat *hist_image);

	void calculate_h_s_v_hist_for_color_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *h,
		Mat *s,
		Mat *v,
		Mat *hsv_image,
		Mat *hist_image,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_dft_for_gray_image(Mat *original_gray_image, Mat *dft_image);

	void calculate_dft_for_gray_image(
		Mat *original_gray_image,
		Mat *mask_image,
		Mat *dft_image,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_dct_for_color_image(Mat *original_color_image, Mat *image_yuv, Mat *image_y, Mat *image_u, Mat *image_v, Mat *dct_image, Mat *dct_image_y, Mat *dct_image_u, Mat *dct_image_v);

	void calculate_y_u_v_hist_for_color_image(Mat *original_color_image, Mat *y, Mat *u, Mat *v, Mat *hist_image);

	void calculate_y_u_v_hist_for_color_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *y,
		Mat *u,
		Mat *v,
		Mat *yuv_image,
		Mat *hist_image,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_fast_for_color_image(Mat *original_color_image, Mat *keyPointImage, string file_path_and_name);

	void calculate_fast_for_color_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *keyPointImage,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_harris_corner_for_color_image(Mat *original_color_image, Mat *keyPointImage, string file_path_and_name);

	void calculate_harris_corner_for_color_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *keyPointImage,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_shi_tomasi_corner_for_color_image(Mat *original_color_image, Mat *keyPointImage, string file_path_and_name);

	void calculate_shi_tomasi_corner_for_color_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *keyPointImage,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_sift_for_color_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *keyPointImage,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_sift_for_color_image(Mat *original_color_image, Mat *keyPointImage, string file_path_and_name);

	void calculate_surf_for_color_image(Mat *original_color_image, Mat *keyPointImage, string file_path_and_name);

	void calculate_surf_for_color_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *keyPointImage,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_orb_for_color_image(Mat *original_color_image, Mat *keyPointImage, string file_path_and_name);

	void calculate_orb_for_color_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *keyPointImage,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_akaze_for_color_image(Mat *original_color_image, Mat *keyPointImage, string file_path_and_name);

	void calculate_akaze_for_color_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *keyPointImage,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_blob_for_color_image(Mat *original_color_image, Mat *keyPointImage, string file_path_and_name);

	void calculate_blob_for_color_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *keyPointImage,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_pca_for_gray_image(
		Mat *original_color_image,
		Mat *pca_image,
		Mat *backproject_image,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_hough_line_for_gray_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *edge_image,
		Mat *edge_image_with_line,
		string file_path_and_name,
		map<string, float> *parameters);


	void calculate_hough_circle_for_gray_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *edge_image,
		Mat *edge_image_with_circle,
		Mat *mask_image_with_center,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_gray_info_for_gray_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *grey_level_statistic_image,
		Mat *white_block_image,
		Mat *black_block_image,
		Mat *white_block_contour_image,
		Mat *black_block_contour_image,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_star_for_color_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *keyPointImage,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_kmeans_for_color_image(
		Mat *original_color_image,
		Mat *kmeans_image,
		int cluster_count,
		vector<LabelAndContour> *label_and_contour_vec,
		string file_path_and_name,
		map<string, float> *parameters);

	void calculate_mser_for_color_image(
		Mat *original_color_image,
		Mat *mask_image,
		Mat *mser_contour_image,
		string file_path_and_name,
		map<string, float> *parameters);

	void image_mask_creation(
		const Point2f *const center_point,
		const Mat *const black_rectangle_with_white_background_grey_image_8uc1,
		Mat *mask_image_8uc1,
		const bool include_rectangle);

	void image_mask_creation_with_multi_thread(
		const Point2f *const center_point,
		const Mat *const black_rectangle_with_white_background_grey_image_8uc1,
		Mat *mask_image_8uc1,
		const bool include_rectangle);

	void exception_tagging_by_cube_tranverse(
		const int mode,
		const Mat *const aa_region_8uc3,
		const Mat *const aa_region_mask_8uc1,
		Mat *aa_region_inverse_mask_8uc1,
		Mat *aa_region_inverse_mask_dilate_8uc1,
		Mat *black_background_exception_image_8uc1,
		const Rect *const bounding_rect_for_aa_region_mask,
		const float cube_side_length,
		const float cube_stride,
		const float grey_difference_for_mode_2_positive,
		const float grey_difference_for_mode_2_negative,
		const int aa_region_dilate_element_width,
		const int aa_region_dilate_element_height,
		const int exception_dilate_element_width,
		const int exception_dilate_element_height,
		const int loop_stride,
		const float zoom_distance,
		Mat *black_background_exception_image_dilate_8uc1,
		map<int, float> *standard_grey_value_for_each_col,
		map<int, int> *exception_pixel_num_for_each_col,
		float *standard_grey_value);

	void calculate_white_block_area_for_binary_image(
		const Mat *const binary_image_8uc1,
		float *white_block_pixel_num,
		float *white_block_area);

	void defect_sample_processing(
		const int mode,
		const Mat *const aa_region,
		const Mat *const binary_image_8uc1,
		vector<vector<Point>> *white_block_contours,
		vector<Vec4i> *hierarchy,
		const Mat *const aa_region_mask_8uc1,
		Mat *original_aa_region_image,
		Mat *defect_tagging_with_rectangle_image,
		Mat *defect_tagging_with_mini_rectangle_image,
		const int bilateral_filter_d,
		const double bilateral_filter_sigma_color,
		const double bilateral_filter_sigma_space,
		const int range_limit_from_mean,
		const float radius_escale_value,
		const int defect_sample_smallest_pixel_num,
		map<int, float> *standard_grey_value_for_each_col,
		const float *const standard_grey_value,
		const int defect_acceptable_minimum_area,
		const int defect_area_for_stop_inspection,
		const int defect_num_for_stop_inspection,
		vector<defect_sample_info> *defect_sample_info_vec);

	void defect_sample_processing_in_mode3_with_multi_thread(
		const int mode,
		const Mat *const aa_region,
		const Mat *const binary_image_8uc1,
		vector<vector<Point>> *white_block_contours,
		vector<Vec4i> *hierarchy,
		const Mat *const aa_region_mask_8uc1,
		Mat *original_aa_region_image,
		Mat *defect_tagging_with_rectangle_image,
		Mat *defect_tagging_with_mini_rectangle_image,
		const int bilateral_filter_d,
		const double bilateral_filter_sigma_color,
		const double bilateral_filter_sigma_space,
		const int range_limit_from_mean,
		const float radius_escale_value,
		const int defect_sample_smallest_pixel_num,
		map<int, float> *standard_grey_value_for_each_col,
		const float *const standard_grey_value,
		const int defect_acceptable_minimum_area,
		const int defect_area_for_stop_inspection,
		const int defect_num_for_stop_inspection,
		vector<defect_sample_info> *defect_sample_info_vec);

	void cg_image_processing(
		Mat *original_cg_image,
		const string root,
		string *root_directory_name,
		string *sub_directory_name_region_separation,
		string *sub_directory_name_defect_in_aa_region,
		string *sub_directory_name_defect_sample,
		vector<vector<Point>> *white_block_contours_for_aa_region_extraction,
		vector<defect_sample_info> *defect_sample_info_vec);

private:
	bool enable_gpu;
	bool enable_cpu_multi_thread;
	bool enable_develop_mode;
	int aa_region_value_min;
	int aa_region_value_max;
	float grey_difference_positive;
	float grey_difference_negative;
	int exception_dilate_element_width;
	int exception_dilate_element_height;
	int loop_stride_to_decide_stantard_grey_value;
	int defect_area_for_stop_inspection;
	int defect_num_for_stop_inspection;
	int defect_acceptable_minimum_area;
	float zoom_distance;
	bool aa_region_extraction_judge;
	float standard_grey_value;
};

#endif
