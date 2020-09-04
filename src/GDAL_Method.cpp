#include "GDAL_Method.h"
GDAL_Method::GDAL_Method(const char* file_path_name, p_rect rect, Transform_btw_Tif_Ima* trans_btw_tif_ima, int ima_width, int ima_height) {

	GDALDataset* poDataset;   //GDAL数据集
	GDALAllRegister();  //注册所有的驱动
	poDataset = (GDALDataset*)GDALOpen(file_path_name, GA_ReadOnly);
	if (poDataset == NULL)
	{
		std::cout << "fail in open files!!!" << std::endl;
		return;
	}

	//获取坐标变换系数
 //double trans[6];
	CPLErr aaa = poDataset->GetGeoTransform(_trans);

	if (aaa == CE_Failure)
	{
		std::cout<<"读取输入图像数据失败！";
		return;
	}

	GDALRasterBand* pInRasterBand = poDataset->GetRasterBand(1);
	float* inBuf;
	inBuf = new float[rect->width * rect->height];
	CPLErr err;
	err = pInRasterBand->RasterIO(GF_Read, rect->begin_x, rect->begin_y, rect->width, rect->height, inBuf, rect->width, rect->height, GDT_Float32, 0, 0);
	if (err == CE_Failure)
	{
		std::cout << "读取输入图像数据失败！";
		return;
	}

	double *nine_pras;
	nine_pras = Transform_btw_Tif_Ima::get_nine_prameter();
	for (int i = 0; i < rect->width * rect->height; i++) {

		int dx = i % rect->width + rect->begin_x;
		int dy = i / rect->width + rect->begin_y;

		float geo_X = _trans[0] + dx * _trans[1] + dy * _trans[2];
		float geo_Y = _trans[3] + dx * _trans[4] + dy * _trans[5];
		float elevation = inBuf[i];

		std::vector<int> tem_vec = trans_btw_tif_ima->get_Ima_xy_from_geoXYZ(geo_X, geo_Y, elevation, ima_width, ima_height,nine_pras);
		if ((tem_vec[0] >= 0 && tem_vec[0] < ima_width) && (tem_vec[1] >= 0 && tem_vec[1] < ima_height)&&elevation!=-9999) {
			p_ZPoint p_point = new ZPoint(tem_vec[0], tem_vec[1], dx, dy, geo_X, geo_Y, elevation);
			this->points_vec.push_back(p_point);
		}
	}


	delete []inBuf;
	inBuf = NULL;
	delete poDataset;
	poDataset = NULL;
}


 int GDAL_Method::TiffCreate(int bandNum, const char* dom_path, int tif_width, int tif_height, double* trans, int utm_num,GDALDataType type)
{
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");//支持中文路径
	GDALAllRegister();  //注册所有的驱动
	//写tiff测试
	GDALDriver* pDriver = GetGDALDriverManager()->GetDriverByName("GTIFF"); //图像驱动
	char** ppszOptions = NULL;
	ppszOptions = CSLSetNameValue(ppszOptions, "BIGTIFF", "IF_NEEDED"); //配置图像信息

		//创建图像
	int bufWidth = tif_width;  //图像X尺寸
	int bufHeight = tif_height;  //图像Y尺寸

	GDALDataset* dst = pDriver->Create(dom_path, bufWidth, bufHeight, bandNum, type, ppszOptions);
	if (dst == nullptr)
	{
		printf("Can't Create Image!\n");
		return 0;
	}
	OGRSpatialReference oSRS;//该类需要#include "ogr_spatialref.h"头文件
	char* pszSRS_WKT = NULL;
	oSRS.SetProjCS("UTM /WGS84");
	oSRS.SetUTM(utm_num, TRUE);
	oSRS.SetWellKnownGeogCS("WGS84");
	oSRS.exportToWkt(&pszSRS_WKT);
	dst->SetProjection(pszSRS_WKT);
	CPLFree(pszSRS_WKT);
	dst->SetGeoTransform(trans);
	GDALClose(dst);

	printf("End Create Image!\n");
}


 float GDAL_Method::get_Ima_geoXYZ_from_xy(GDALDataset* poDataset, float* inBuf,int x, int y, float max_elevation, int tif_width, int tif_height, double* nine_prameters,int ima_width,int ima_height) {


	 x = x - ima_width / 2;
	 y = -y + ima_height / 2;

	 double Xs = Transform_btw_Tif_Ima::getOutTags()->Xs;
	 double Ys = Transform_btw_Tif_Ima::getOutTags()->Ys;
	 double Zs = Transform_btw_Tif_Ima::getOutTags()->Zs;

	 double temp1 = (nine_prameters[0] * (x - Transform_btw_Tif_Ima::getOutTags()->_x) + nine_prameters[1] * (y - Transform_btw_Tif_Ima::getOutTags()->_y) - nine_prameters[2] * Transform_btw_Tif_Ima::getOutTags()->_f);
	 double temp2 = (nine_prameters[3] * (x - Transform_btw_Tif_Ima::getOutTags()->_x) + nine_prameters[4] * (y - Transform_btw_Tif_Ima::getOutTags()->_y) - nine_prameters[5] * Transform_btw_Tif_Ima::getOutTags()->_f);
	 double temp3 = (nine_prameters[6] * (x - Transform_btw_Tif_Ima::getOutTags()->_x) + nine_prameters[7] * (y - Transform_btw_Tif_Ima::getOutTags()->_y) - nine_prameters[8] * Transform_btw_Tif_Ima::getOutTags()->_f);
	 double Xgeo = (max_elevation - Zs) * temp1 / temp3 + Xs;
	 double Ygeo = (max_elevation - Zs) * temp2 / temp3 + Ys;

	 double dTemp = _trans[1] * _trans[5] - _trans[2] * _trans[4];

	 int dx = (_trans[5] * (Xgeo - _trans[0]) - _trans[2] * (Ygeo - _trans[3])) / dTemp + 0.5;//移到像素中心
	 int dy = (_trans[1] * (Ygeo - _trans[3]) - _trans[4] * (Xgeo - _trans[0])) / dTemp + 0.5;
	 dx = dx < 0 ? 0 : dx;
	 dx = dx > tif_width ? tif_width - 1 : dx;
	 dy = dy < 0 ? 0 : dy;
	 dy = dy > tif_height ? tif_height - 1 : dy;

	 float elevetion = max_elevation;
	 GDALRasterBand* pInRasterBand = poDataset->GetRasterBand(1);
	 CPLErr err;
	 err = pInRasterBand->RasterIO(GF_Read, dx, dy, 1, 1, inBuf, 1, 1, GDT_Float32, 0, 0);
	 


	 //迭代点刚好在DEM边界时 应该防止死循环
	 //低点指高点 高点指低点 死循环 1 -> 0 -> 1
	 int count = 0;
	 while (fabs(elevetion - *inBuf) > 0.05 && count < 5000)
	 {
		 elevetion = *inBuf;
		 Xgeo = (elevetion - Zs) * temp1 / temp3 + Xs;
		 Ygeo = (elevetion - Zs) * temp2 / temp3 + Ys;
		  dx = (_trans[5] * (Xgeo - _trans[0]) - _trans[2] * (Ygeo - _trans[3])) / dTemp + 0.5;//移到像素中心
		  dy = (_trans[1] * (Ygeo - _trans[3]) - _trans[4] * (Xgeo - _trans[0])) / dTemp + 0.5;
		 dx = dx < 0 ? 0 : dx;
		 dx = dx > tif_width ? tif_width - 1 : dx;
		 dy = dy < 0 ? 0 : dy;
		 dy = dy > tif_height ? tif_height - 1 : dy;
		 err = pInRasterBand->RasterIO(GF_Read, dx, dy, 1, 1, inBuf, 1, 1, GDT_Float32, 0, 0);
		 count++;
	 }

	 return elevetion;
 }

 
 void GDAL_Method::filetoArry(int low, std::string url,  double* arry) {
	 int head = 0;//定位现在需要读哪一行
	 std::string line;//每一行
	 std::ifstream fin(url);
	 if (!fin)
	 {
		 std::cout << "读取tags失败" << std::endl;
		 return ;
	 }

	 while (std::getline(fin, line))
	 {
		 std::istringstream is(line);

		 if (head != low)
		 {
			 head += 1;
			 continue;

		 }
			 is >>arry[0] >> arry[1] >> arry[2] >> arry[3] >> arry[4] >> arry[5] >> arry[6] >> arry[7] >> arry[8];

		 break;
	 }

	 return ;
 }


 void GDAL_Method::size_the_tif(int tif_width, int tif_height, std::vector<p_rect>& rect_vec) {
 
	 int temp = 1000;
	 int w_num = tif_width / temp;
     int w_left = tif_width% temp;
	 int h_num = tif_height / temp;
     int h_left = tif_height % temp;

	 for (int i = 0; i < w_num + 1; i++) {
	 
		 for (int j = 0; j < h_num + 1; j++) {
		 
			 
			 int width = i == w_num ? w_left : temp;
			 int height = j == h_num ? h_left : temp;
			 p_rect rec = new rect(temp * i, temp * j, width, height);
			 rect_vec.push_back(rec);
		 }
	 }
 }

 double GDAL_Method::_trans[6] = {0,0,0,0,0,0};
