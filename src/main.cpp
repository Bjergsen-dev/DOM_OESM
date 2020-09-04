#include<opencv2/opencv.hpp>
#include<opencv2/core.hpp>
#include"GDAL_Method.h"

int main(int argc, char* argv[]) {

	//set the file paths
    std::string str1 = (std::string("../resource/").append(argv[1]));
    std::string str2 = (std::string("../resource/").append(argv[2]));
    std::string str3= (std::string("../resource/").append(argv[3]));
    std::string str4= (std::string("../resource/").append(argv[4]));
	const char* imaFilePath = str1.c_str();
	const char* dtmFilePath = str2.c_str();
	const char* dsmFilePath = str3.c_str();
	const char* oesmFilePath = "../resource/OESM.tif";
	const char* domFilePath = "../resource/DOM.tif";
	const char* tagsFIlepath = str4.c_str();
	const char* domCreatepath = "../resource/DOM.tif";
	const char* oesmCreatepath = "../resource/OESM.tif";

	//chaek the prameters 
	if (argc != 6) {
	
		std::cout << "more or less  prameters" << std::endl;
        return 0;
	}
	float elevation_dsm_for_inservation = atof(argv[5]);

	//read the ima and tif with opencv
	cv::Mat img = cv::imread(imaFilePath);
	cv::Mat tif = cv::imread(dtmFilePath,cv::IMREAD_UNCHANGED);
	int ima_width = img.size().width , ima_height = img.size().height, tif_width = tif.size().width, tif_height = tif.size().height;

	//release the tif after get the size
	tif.release();

	//record the tags of ima and tags of camera
	double tags_arry[9];
	GDAL_Method::filetoArry(1,tagsFIlepath , tags_arry);
    Transform_btw_Tif_Ima* trans_btw_tif_ima = new Transform_btw_Tif_Ima(tags_arry);

	//size the area you will operate
	std::vector<p_rect> p_rec_vec;
	GDAL_Method::size_the_tif(tif_width, tif_height, p_rec_vec);

	//create the DOM and OESM empty
	GDAL_Method::TiffCreate(3, domCreatepath, tif_width, tif_height, GDAL_Method::get_Trans(dsmFilePath), 32, GDT_Byte);
	GDAL_Method::TiffCreate(1, oesmCreatepath, tif_width, tif_height, GDAL_Method::get_Trans(dsmFilePath), 32, GDT_Float32);

	std::cout << "Begin read the dem rasters..." << std::endl;

	//GDAL writer for DOM
	GDALDataset* poDataset;   //GDAL数据集
	GDALAllRegister();  //注册所有的驱动
	poDataset = (GDALDataset*)GDALOpen(domFilePath, GA_Update);
	uchar* paf = new uchar[1];
	//GDAL reader for DSM
	GDALDataset* poDataset1;   //GDAL数据集
	GDALAllRegister();  //注册所有的驱动
	poDataset1 = (GDALDataset*)GDALOpen(dsmFilePath, GA_ReadOnly);
	float* paf1 = new float[1];
	double* nine_prameters = Transform_btw_Tif_Ima::get_nine_prameter();
	//GDAL writer for OESM
	GDALDataset* poDataset2;   //GDAL数据集
	GDALAllRegister();  //注册所有的驱动
	poDataset2 = (GDALDataset*)GDALOpen(oesmFilePath, GA_Update);
	float* paf2 = new float[1];
	for (int k = 0; k < p_rec_vec.size();k++) {
		GDAL_Method* dem_gdal = new GDAL_Method(dtmFilePath, p_rec_vec[k], trans_btw_tif_ima, ima_width, ima_height);
		//std::cout << "End read the dem rasters..." << std::endl;
		/********************************************Create DOM and write***************************************************************/

		if (poDataset == NULL)
		{
			std::cout << "fail in open files!!!" << std::endl;

		}

		//std::cout << "Begin write the DOM..." << std::endl;
		for (int i = 0; i < dem_gdal->points_vec.size(); i++) {

			cv::Vec3b* curr1 = img.ptr<cv::Vec3b>(dem_gdal->points_vec[i]->ima_y);
			cv::Vec3b bgr1 = curr1[dem_gdal->points_vec[i]->ima_x];

			uchar band1_huidu = bgr1[0];
			uchar band2_huidu = bgr1[1];
			uchar band3_huidu = bgr1[2];

			GDAL_Method::write_to_tif(poDataset, band1_huidu, paf, dem_gdal->points_vec[i]->tif_x, dem_gdal->points_vec[i]->tif_y, 3, GDT_Byte);
			GDAL_Method::write_to_tif(poDataset, band2_huidu, paf, dem_gdal->points_vec[i]->tif_x, dem_gdal->points_vec[i]->tif_y, 2, GDT_Byte);
			GDAL_Method::write_to_tif(poDataset, band3_huidu, paf, dem_gdal->points_vec[i]->tif_x, dem_gdal->points_vec[i]->tif_y, 1, GDT_Byte);
		}

		/**********************************************Read the DSM and Create,wirite the OESM*************************************************************************/

		for (int i = 0; i < dem_gdal->points_vec.size(); i++) {

			float elevetion = GDAL_Method::get_Ima_geoXYZ_from_xy(poDataset1, paf1, dem_gdal->points_vec[i]->ima_x, dem_gdal->points_vec[i]->ima_y, elevation_dsm_for_inservation, tif_width, tif_height, nine_prameters, ima_width, ima_height);
			GDAL_Method::write_to_tif(poDataset2, elevetion, paf2, dem_gdal->points_vec[i]->tif_x, dem_gdal->points_vec[i]->tif_y, 1, GDT_Float32);
		}

		//release the room after a read and write
		delete dem_gdal;
		dem_gdal = NULL;

	}

	GDALClose(poDataset);
    GDALClose(poDataset1);
    GDALClose(poDataset2);
    delete[] paf;
    paf = NULL;
    delete[] paf1;
    paf1 = NULL;
    delete[] paf2;
    paf2 = NULL;
    

	return 0;
}
