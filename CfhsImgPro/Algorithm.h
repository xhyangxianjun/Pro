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
#include "shlwapi.h"

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

#include "Func.h"

#pragma comment(lib,"shlwapi.lib")

using namespace cv::xfeatures2d;
using namespace cv;
using namespace std;
using namespace ml;



#define GONGWEI1	0
#define GONGWEI2	1
#define WUSHI1		2
#define WUSHI2		3
#define APPLE		4

#define TYPE		4

#include "CGDetector.h"
#include "ITODetector.h"


//ITO算子
int AlgorithmInterface_ITO(Mat src, string directory_name, map<int, vector<string>>  &DefectInfoData, int length, int thresh,
							int top, int left, int right, int bottom, int limit, int iter, int defectnum, Mat &img)
{
	ITODetector detector;
	detector.set_length(length);
	detector.set_thresh(thresh);
	detector.set_top(top);
	detector.set_left(left);
	detector.set_right(right);
	detector.set_bottom(bottom);
	detector.set_limit(limit);
	detector.set_iter(iter);

	if (src.empty())
	{
		cout << "输入到算法的图像为空!" << endl;
		return 0;
	}
	//imshow("src", src); waitKey();
	Mat gray = src.clone();
	double time = getTickCount();

	vector<LINE> lines; 
	detector.detect_big(gray, lines);

	time = (getTickCount() - time) / getTickFrequency(); cout << "time=" << time << endl;

	for (auto l : lines)
	{
		line(gray, l.pt_in, l.pt_out, Scalar(0, 0, 255), 2);
	}

	vector<string > AataElem;
	cout << "缺陷个数:" << lines.size() << endl;
	int ilimit = 0;

	ilimit = defectnum<limit ? defectnum : limit;
	ilimit = ilimit < lines.size() ? ilimit : lines.size();

	for (int i = 0; i < ilimit; i++)
	{		
		string buffer;
		Point pt1 = lines[i].pt_out;
		Point pt2 = lines[i].pt_in;
		buffer = to_string(pt1.x) + "," + to_string(pt1.y);
		AataElem.push_back(buffer);			//坐标1		
		buffer = to_string(pt2.x) + "," + to_string(pt2.y);
		AataElem.push_back(buffer);			//坐标2
		AataElem.push_back(to_string(lines[i].length));		//长度
		DefectInfoData.insert(make_pair(i, AataElem));
		AataElem.clear();
	}

	img = gray.clone();
	return 0;
}


//利用模板做检测的例子
int AlgorithmInterface_hjh(Mat src, string directory_name, map<int, vector<string>>  &DefectInfoData, string pid, int new_model_hjh, int aa_region_positive_hjh, int aa_region_negative_hjh,
		int grey_difference_positive_hjh, int grey_difference_negative_hjh, int max_area_hjh, int min_area_hjh, int num_for_stop_inspection_hjh, Mat &img)
{
	CGDetector detector;
	if (1 == new_model_hjh)
	{		
		CGDetector detector;
		detector.set_aa_region_positive(40);
		detector.set_aa_region_negative(40);
		//第一步：读取图像并转化成灰度图
		Mat gray = src.clone();		
		//第二步：执行手机提取算子
		Mat mat_phone; detector.extract_phone(gray, mat_phone);//mat_phone保存切割出来的CG片
		//第三步：执行分区算子
		Mat mat_aa;//保存带AA区边框的图
		vector<Point> contour_aa; //保存AA区的轮廓
		detector.extract_aa_contour(mat_phone, "", contour_aa, mat_aa);//执行这个函数时，如果file_model等于""代表不使用模板
		//第四步：检查AA区分区是否合格
		//namedWindow("mat_aa", 0); imshow("mat_aa", mat_aa); waitKey(0);
		//第五步：如果合格，执行以下代码将生成模板文件
		detector.make_model(mat_phone.size(), contour_aa, "D:\\model.txt");		
	}	

	detector.set_aa_region_positive(aa_region_positive_hjh);
	detector.set_aa_region_negative(aa_region_negative_hjh);
	detector.set_grey_difference_positive(grey_difference_positive_hjh);
	detector.set_grey_difference_negative(grey_difference_negative_hjh);
	detector.set_num_for_stop_inspection(num_for_stop_inspection_hjh);
	detector.set_max_area(max_area_hjh);
	detector.set_min_area(min_area_hjh);
	
	//detector.set_aa_region_positive(40);
	//detector.set_aa_region_negative(40);
	//detector.set_grey_difference_positive(20);
	//detector.set_grey_difference_negative(20);
	//detector.set_num_for_stop_inspection(5);
	//detector.set_max_area(50000);
	//detector.set_min_area(180);

	double time = getTickCount();
	string file_model = "D:\\model.txt";//指定模板文件
	//读取图像并转化成灰度图
	Mat gray = src.clone();//读取图像，彩色方式读取
	//第一步：执行手机提取算子
	Mat mat_phone; //保存切割出来的CG片
	detector.extract_phone(gray, mat_phone);
	//第二步：执行分区算子
	Mat mat_aa;//保存带AA区边框的图
	vector<Point> contour_aa; //保存AA区的轮廓
	detector.extract_aa_contour(mat_phone, file_model, contour_aa, mat_aa);//执行这个函数时，如果file_model等于""代表不使用模板，否则到指定路径加载模板
																		   //第三步：执行缺陷检测算子
	vector<defect_info> info_vec;//保存缺陷信息
	detector.defect_detect(mat_phone, contour_aa, info_vec);
	//第四步：打印缺陷信息
	detector.print_defect_message(info_vec);
	cout << "time = " << (getTickCount() - time) / getTickFrequency() << endl;
	Mat mat_red_point;
	cvtColor(mat_phone, mat_red_point, COLOR_GRAY2BGR);	
	vector<string > AataElem;
	cout << "缺陷个数:" << info_vec.size() << endl;
	for (int i = 0; i < info_vec.size(); i++)
	{
		circle(mat_red_point, info_vec[i].defect_location_exibit_to_client, 30, Scalar(0, 0, 255), -1);
		char file[256]; sprintf_s(file, "%s\\Defect\\%s_%d.jpg", directory_name.c_str(), pid.c_str(), i);
		imwrite(file, info_vec[i].defect_sample);
		cout << string(100, '#') << endl;
		cout << "defect_id_exibit_to_client:   " << info_vec[i].defect_id_exibit_to_client << endl;
		
		string buffer;
		Point pt = info_vec[i].defect_location_exibit_to_client;
		cout << "pt.x = " << pt.x << "pt.y = " << pt.y << endl;
		buffer = to_string(pt.x) + "," + to_string(pt.y);
		AataElem.push_back(buffer);			//坐标
		AataElem.push_back(to_string(info_vec[i].defect_area1_exibit_to_client));		//面积1
		AataElem.push_back(to_string(info_vec[i].defect_area2_exibit_to_client));			//面积2
		AataElem.push_back(to_string(info_vec[i].defect_width_exibit_to_client));		//宽
		AataElem.push_back(to_string(info_vec[i].defect_height_exibit_to_client));		//高
		AataElem.push_back(to_string(info_vec[i].defect_mean_grey_value_exibit_to_client));		//灰度
		AataElem.push_back(to_string(info_vec[i].defect_max_min_grey_value_difference_exibit_to_client));	//灰度差
		AataElem.push_back(to_string(info_vec[i].defect_roundness_exibit_to_client));							//圆度
		AataElem.push_back(to_string(info_vec[i].defect_roundness_ratio_exibit_to_client));					//圆周比率
		DefectInfoData.insert(make_pair(info_vec[i].defect_id_exibit_to_client, AataElem));
		AataElem.clear();
	}
	
	img = mat_aa .clone();  //namedWindow("mat_aa", 0); imshow("mat_aa", mat_aa); waitKey(0);
	return 0;
}


int AlgorithmInterface(Mat src, string directory_name, map<int, vector<string>>  &DefectInfoData, string pid, int aa_reg_min, int aa_reg_max, float grey_dif_pos, float grey_dif_neg,
						int excep_dilate_width, int excep_dilate_height, int loop_stride_to_decide, int defect_area, int defect_num, int defect_accep_min_area, Mat &img)
{
	clock_t startTime;
	clock_t endTime;

	startTime = clock();

	FunctionInAll function_in_all;
	function_in_all.set_enable_gpu(false);
	//function_in_all.set_enable_cpu_multi_thread(false);
	function_in_all.set_enable_develop_mode(false);

	function_in_all.set_aa_region_value_min(aa_reg_min);
	function_in_all.set_aa_region_value_max(aa_reg_max);
	function_in_all.set_grey_difference_positive(grey_dif_pos);
	function_in_all.set_grey_difference_negative(grey_dif_neg);
	function_in_all.set_exception_dilate_element_width(excep_dilate_width);
	function_in_all.set_exception_dilate_element_height(excep_dilate_height);
	function_in_all.set_loop_stride_to_decide_stantard_grey_value(loop_stride_to_decide);
	function_in_all.set_defect_area_for_stop_inspection(defect_area);
	function_in_all.set_defect_num_for_stop_inspection(defect_num);
	function_in_all.set_defect_acceptable_minimum_area(defect_accep_min_area);

	//function_in_all.set_aa_region_value_min(130);
	//function_in_all.set_aa_region_value_max(255);
	//function_in_all.set_grey_difference_positive(30.0f);
	//function_in_all.set_grey_difference_negative(30.0f);
	//function_in_all.set_exception_dilate_element_width(30);
	//function_in_all.set_exception_dilate_element_height(30);
	//function_in_all.set_loop_stride_to_decide_stantard_grey_value(100);
	//function_in_all.set_defect_area_for_stop_inspection(50000);
	//function_in_all.set_defect_num_for_stop_inspection(5);
	//function_in_all.set_defect_acceptable_minimum_area(180);
	function_in_all.set_zoom_distance(300.0f);//parameter for controlling the aa region zooming scale
	function_in_all.set_aa_region_extraction_judge(true);//parameter for judging the extraction of aa region success or failure
	function_in_all.set_standard_grey_value(191.0f);
	string root = directory_name;
	string root_directory_name;
	string sub_directory_name_region_separation;
	string sub_directory_name_defect_in_aa_region;
	string sub_directory_name_defect_sample;

	Mat original_cg_image = src.clone();

	vector<vector<Point>> white_block_contours_for_aa_region_extraction;
	vector<defect_sample_info> defect_sample_info_vec;

	function_in_all.cg_image_processing(
		&original_cg_image,
		root,
		&root_directory_name,
		&sub_directory_name_region_separation,
		&sub_directory_name_defect_in_aa_region,
		&sub_directory_name_defect_sample,
		&white_block_contours_for_aa_region_extraction,
		&defect_sample_info_vec);

	if (function_in_all.get_aa_region_extraction_judge() == false)
	{
		cout << "aa_region extraction failed" << endl;
		//system("pause");
		return -1;
	}

	//access the edge of aa region for current cg 
	img = original_cg_image.clone();
	drawContours(img, white_block_contours_for_aa_region_extraction, 0, Scalar(0, 255, 0), 30, 8);
	//namedWindow("aa_region_edge", 0);
	//imshow("aa_region_edge", img);
	//waitKey(1);

	endTime = clock();
	cout << "total run time is:   " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;


	vector<string > AataElem;
	cout << "缺陷个数:" << defect_sample_info_vec.size() << endl;
	for (int i = 0; i < defect_sample_info_vec.size(); i++)
	{
		if (defect_sample_info_vec[i].defect_judge == true)
		{
			char file[256]; sprintf_s(file, "%s\\Defect\\%s_%d.jpg", directory_name.c_str(), pid.c_str(), i);
			imwrite(file, defect_sample_info_vec[i].defect_sample_with_small_rotated_rect);
			cout << string(100, '#') << endl;
			cout << "defect_id_exibit_to_client:   " << defect_sample_info_vec[i].defect_id_exibit_to_client << endl;
			//cout << "defect_location_exibit_to_client:   " << defect_sample_info_vec[i].defect_location_exibit_to_client << endl;
			//cout << "defect_area1_exibit_to_client:   " << defect_sample_info_vec[i].defect_area1_exibit_to_client << endl;
			//cout << "defect_area2_exibit_to_client:   " << defect_sample_info_vec[i].defect_area2_exibit_to_client << endl;
			//cout << "defect_width_exibit_to_client:   " << defect_sample_info_vec[i].defect_width_exibit_to_client << endl;
			//cout << "defect_height_exibit_to_client:   " << defect_sample_info_vec[i].defect_height_exibit_to_client << endl;
			//cout << "defect_mean_grey_value_exibit_to_client:   " << defect_sample_info_vec[i].defect_mean_grey_value_exibit_to_client << endl;
			//cout << "defect_max_min_grey_value_difference_exibit_to_client:   " << defect_sample_info_vec[i].defect_max_min_grey_value_difference_exibit_to_client << endl;
			//cout << "defect_roundness_exibit_to_client:   " << defect_sample_info_vec[i].defect_roundness_exibit_to_client << endl;
			//cout << "defect_roundness_ratio_exibit_to_client:   " << defect_sample_info_vec[i].defect_roundness_ratio_exibit_to_client << endl;
		}
		string buffer;
		Point pt = defect_sample_info_vec[i].defect_location_exibit_to_client;
		//buffer = "(" + to_string(pt.x) + "," + to_string(pt.y) + ")";
		buffer = to_string(pt.x) + "," + to_string(pt.y) ;
		AataElem.push_back(buffer);			//坐标
		AataElem.push_back(to_string(defect_sample_info_vec[i].defect_area1_exibit_to_client));		//面积1
		AataElem.push_back(to_string(defect_sample_info_vec[i].defect_area2_exibit_to_client));			//面积2
		AataElem.push_back(to_string(defect_sample_info_vec[i].defect_width_exibit_to_client));		//宽
		AataElem.push_back(to_string(defect_sample_info_vec[i].defect_height_exibit_to_client));		//高
		AataElem.push_back(to_string(defect_sample_info_vec[i].defect_mean_grey_value_exibit_to_client));		//灰度
		AataElem.push_back(to_string(defect_sample_info_vec[i].defect_max_min_grey_value_difference_exibit_to_client));	//灰度差
		AataElem.push_back(to_string(defect_sample_info_vec[i].defect_roundness_exibit_to_client));							//圆度
		AataElem.push_back(to_string(defect_sample_info_vec[i].defect_roundness_ratio_exibit_to_client));					//圆周比率
		DefectInfoData.insert(make_pair(defect_sample_info_vec[i].defect_id_exibit_to_client, AataElem));
		AataElem.clear();
	}
	return 0;
}
