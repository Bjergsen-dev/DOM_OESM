#pragma once
#include"gdal_priv.h"
#include"Transform_btw_Tif_Ima.h"
#include <vector>
#include<iostream>
#include<sstream>
#include<fstream>
#include<algorithm>
#include<vector>
/*

*/
typedef struct ZPoint {
public:
	int ima_x;//ima_x(pixel)
	int ima_y;//ima_y(pixel)
	int tif_x;//dem or dsm tif_x(pixel)
	int tif_y;//dem or dsm tif_y(pixel)
	float geo_x;//geo_x(geo x)
	float geo_y;//geo_y(geo y)
	float _elevation;//geo_z(geo z)

	//overload ==
	bool operator==(const ZPoint b) const
	{
		return (this->ima_x == b.ima_x)&&(this->ima_y == b.ima_y);
	}
	ZPoint(int imax, int imay, int tifx, int tify, float geox, float geoy, float elevation){
	
		ima_x = imax;
		ima_y = imay;
		tif_x = tifx;
		tif_y = tify;
		geo_x = geox;
		geo_y = geoy;
		_elevation = elevation;
	}
}* p_ZPoint;

/*
rect strcut 
*/
typedef struct rect {
public:
	int begin_x;//top-left x
	int begin_y;//top-left y
	int width;//width
	int height;//height
	rect(int x, int y, int wid, int het) {
	
		begin_x = x;
		begin_y = y;
		width = wid;
		height = het;
	}
}*p_rect;

class GDAL_Method
{
private:
	//geo tif trans prameters
	static double _trans[6];
	

public:
	//vector to record the points
	std::vector<p_ZPoint> points_vec;


	//construct
	/*
	@prama: const char* file_path_name -- file name path
	@prama: rect -- rect area to read and write tif
	@prama: trans_btw_tif_ima -- trans_btw_tif_ima instance
	@prama: ima_width -- ima_width(pixel)
	@prama: ima_height -- ima_height(pixel)
	@return: GDAL_Method instance
	*/
	GDAL_Method(const char* file_path_name,p_rect rect,Transform_btw_Tif_Ima * trans_btw_tif_ima, int ima_width, int ima_height);

	// create tif file(dom or oesm)
	/*
	@prama: bandNum -- band of tif you'll create
	@prama: dom_path -- create path
	@prama: tif_width,tif_height -- tif height and width(pixel)
	@prama: trans-- tif trans
	@prama: utm_num -- utm number
	@prama: type -- GDALDataType(float for elevation or uchar for RGB)
	*/
	static int TiffCreate(int bandNum, const char* dom_path, int tif_width, int tif_height, double* trans, int utm_num,GDALDataType type);
	
	// write data to tif
	/*
	@pram: dst -- GDALDataset you ll wirte
	@prama: value -- data you'll write in
	@prama: pafScanblock -- writer of tif(alloc the room then write in)
	@prama:i,j -- pozition you write in tif(pixel)
	@prama:band -- tif band you'll operate
	@prama:type -- data type
	*/
	template<class T>
	static void write_to_tif(GDALDataset* dst, T value, T* pafScanblock, int i, int j, int band, GDALDataType type) {

		//获取图像波段
		GDALRasterBand* poBand1;
		poBand1 = dst->GetRasterBand(band);
		if (poBand1 == NULL)
		{
			printf("poBand is null!");
		}
		pafScanblock[0] = value;

        CPLErr err = poBand1->RasterIO(GF_Write, i, j, 1, 1, pafScanblock, 1, 1, type, 0, 0);
        if(err == CE_Failure){
            
            std::cout<<"write to tif filed"<<std::endl;
        }

	}

	//get the point's geoZ with image x y
	/*
	@prama: poDataset -- GDAL dataset to file
	@prama: inBuf -- GDAL reader
	@prama: x,y -- image x and y(pozition in image pixel)
	@prama: max_elevation -- max_elevation for iteration to find the true geo_XYZ
	@prama: tif_width, tif_height 
	@prama: nine_prameters -- R mitrix
	@prama: ima_width,ima_height 
	*/
	static float get_Ima_geoXYZ_from_xy(GDALDataset* poDataset, float* inBuf, int x, int y, float max_elevation, int tif_width, int tif_height, double* nine_prameters,  int ima_width, int ima_height);

	static double* get_Trans(const char* file_path_name) {
        double* tr = new double[6];
        GDALDataset* poDataset;   //GDAL数据集
	    GDALAllRegister();  //注册所有的驱动
        poDataset = (GDALDataset*)GDALOpen(file_path_name, GA_ReadOnly);
	    if (poDataset == NULL)
	     {
		std::cout << "fail in open files!!!" << std::endl;
	     }

	//获取坐标变换系数
 //double trans[6];
	  CPLErr aaa = poDataset->GetGeoTransform(tr);
      return tr;
	}

	//read the tag file to arry
	/*
	@prama: low -- the row number for reading
	@prama: url -- file path
	@prama: arry -- return arry
	*/
	static void filetoArry(int low, std::string url, double* arry);

	//tif is large and cut the tif to pieces for read and write
	/*
	@prama:tif_width,tif_height 
	@prama:rect_vec -- record the rects
	*/
	static void size_the_tif(int tif_width, int tif_height, std::vector<p_rect>& rect_vec);
};

