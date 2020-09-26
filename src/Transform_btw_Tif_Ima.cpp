#include "Transform_btw_Tif_Ima.h"
Transform_btw_Tif_Ima::Transform_btw_Tif_Ima(double* arry) {
	_outtags = new outTags( arry);
}

double* Transform_btw_Tif_Ima::get_nine_prameter() {

	double *res = new double[9];
	double phi = _outtags->phi * PI / 180;
	double omiga = _outtags->omiga * PI / 180;
	double kamma = _outtags->kamma * PI / 180;
	res[0] = cos(phi) * cos(kamma) - sin(phi) * sin(omiga) * sin(kamma);
	res[1]= -cos(phi) * sin(kamma) - sin(phi) * sin(omiga) * cos(kamma);
	res[2] = -sin(phi) * cos(omiga);
	res[3] = cos(omiga) * sin(kamma);
	res[4] = cos(omiga) * cos(kamma);
	res[5] = -sin(omiga);
	res[6] = sin(phi) * cos(kamma) + cos(phi) * sin(omiga) * sin(kamma);
	res[7] = -sin(phi) * sin(kamma) + cos(phi) * sin(omiga) * cos(kamma);
	res[8] = cos(phi) * cos(omiga);
	
	return res;
}

std::vector<int> Transform_btw_Tif_Ima::get_Ima_xy_from_geoXYZ(float geo_X, float geo_Y, float elevation, int ima_width, int ima_height,double* nine_prameters) {



	//先计算旋转矩阵a1 a2 a3 b1 b2 b3 c1 c2 c3
	double Xs = _outtags->Xs;
	double Ys = _outtags->Ys;
	double Zs = _outtags->Zs;

	float temp1 = nine_prameters[0] * (geo_X - Xs) + nine_prameters[3] * (geo_Y - Ys) + nine_prameters[6] * (elevation - Zs);
	float temp2 = nine_prameters[1] * (geo_X - Xs) + nine_prameters[4] * (geo_Y - Ys) + nine_prameters[7] * (elevation - Zs);
	float temp3 = nine_prameters[2] * (geo_X - Xs) + nine_prameters[5] * (geo_Y - Ys) + nine_prameters[8] * (elevation - Zs);
	double x = _outtags->_x - _outtags->_f * (temp1 / temp3);
	double y = _outtags->_y - _outtags->_f * (temp2 / temp3);


	//转换框标坐标为像素坐标
	double ima_x = x + ima_width / 2 + 0.5;
	double ima_y = -y + ima_height / 2 + 0.5;

    
	std::vector<int> res;
	res.push_back(ima_x);
	res.push_back(ima_y);
	return res;

	

}


p_outTags Transform_btw_Tif_Ima::getOutTags() {

	return _outtags;
}


p_outTags Transform_btw_Tif_Ima::_outtags = nullptr;
