#pragma once
#include<math.h>
#include<vector>
#define PI 3.1415926535897//pi

/*
define the struct of TAGS
*/
typedef struct outTags {
public:
	double Xs, Ys, Zs, omiga, phi, kamma;
	double _x, _y, _f;
	outTags(double* arry) {
	
		Xs = arry[0];
		Ys = arry[1];
		Zs = arry[2];
		omiga = arry[3];
		phi = arry[4];
		kamma = arry[5];
		_x = arry[6];
		_y = arry[7];
		_f = arry[8];
	}
}* p_outTags;

class Transform_btw_Tif_Ima
{
public:
	/*
	@prama:double* arry -- input arry to initial the Transform_btw_Tif_Ima instance
	@return: a instance of Transform_btw_Tif_Ima 
	*/
	Transform_btw_Tif_Ima(double* arry);

	/*
	@return: R mitrix
	*/
	static double* get_nine_prameter();

	/*
	@prama:float geo_X -- geo_X coordinate of input point
	@prama:float geo_Y -- geo_Y coordinate of input point
	@prama:float elevation -- geo_Z coordinate of input point
	@prama:ima_width -- image width (pixel)
	@prama:ima_height -- image height (pixel)
	@prama:nine_prameters -- R mitrix
	@return:ima_x and ima_y
	*/
	std::vector<int> get_Ima_xy_from_geoXYZ(float geo_X, float geo_Y, float elevation, int ima_width, int ima_height, double* nine_prameters);

	/*
	return: Tags 
	*/
	 static p_outTags getOutTags();
private:
	 static p_outTags _outtags;
};

