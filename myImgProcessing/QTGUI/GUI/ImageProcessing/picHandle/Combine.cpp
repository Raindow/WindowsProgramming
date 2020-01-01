#include "Combine.h"



Combine::Combine()
{
}


Combine::~Combine()
{
}

cv::Mat Combine::normal(cv::Mat imgX, cv::Mat imgY)
{
	cv::Mat result;
	cv::Size maxSize = (imgX.size().area() > imgY.size().area()) ? imgX.size() : imgY.size();
	cv::resize(imgX, imgX, cv::Size(maxSize.width / 2, maxSize.height / 2), 0, 0, cv::INTER_LINEAR);
	cv::resize(imgY, imgY, cv::Size(maxSize.width / 2, maxSize.height / 2), 0, 0, cv::INTER_LINEAR);
	cv::hconcat(imgX, imgY, result);
	return result;
}

cv::Mat Combine::orb(cv::Mat imgX, cv::Mat imgY)
{
	cv::Mat imgXs, imgYs;
	cv::cvtColor(imgX, imgXs, cv::COLOR_RGB2GRAY);
	cv::cvtColor(imgY, imgYs, cv::COLOR_RGB2GRAY);

	//��ȡ������ 
	cv::Mat imageDesc1, imageDesc2;
	std::vector<cv::KeyPoint> keyPoint1, keyPoint2;
	cv::Ptr<cv::ORB> orb = cv::ORB::create(3000);
	orb->detectAndCompute(imgXs, cv::Mat(), keyPoint1, imageDesc1);
	orb->detectAndCompute(imgYs, cv::Mat(), keyPoint2, imageDesc2);


	cv::flann::Index flannIndex(imageDesc1, cv::flann::LshIndexParams(12, 20, 2), cvflann::FLANN_DIST_HAMMING);

	std::vector<cv::DMatch> GoodMatchePoints;

	cv::Mat macthIndex(imageDesc2.rows, 2, CV_32SC1), matchDistance(imageDesc2.rows, 2, CV_32FC1);
	flannIndex.knnSearch(imageDesc2, macthIndex, matchDistance, 2, cv::flann::SearchParams());

	// Lowe's algorithm,��ȡ����ƥ���
	for (int i = 0; i < matchDistance.rows; i++)
	{
		if (matchDistance.at<float>(i, 0) < 0.4 * matchDistance.at<float>(i, 1))
		{
			cv::DMatch dmatches(i, macthIndex.at<int>(i, 0), matchDistance.at<float>(i, 0));
			GoodMatchePoints.push_back(dmatches);
		}
	}

	//cv::Mat first_match;
	//drawMatches(imgY, keyPoint2, imgX, keyPoint1, GoodMatchePoints, first_match);
	//imshow("first_match ", first_match);

	std::vector<cv::Point2f> imagePoints1, imagePoints2;

	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		imagePoints2.push_back(keyPoint2[GoodMatchePoints[i].queryIdx].pt);
		imagePoints1.push_back(keyPoint1[GoodMatchePoints[i].trainIdx].pt);
	}



	//��ȡͼ��1��ͼ��2��ͶӰӳ����� �ߴ�Ϊ3*3  
	cv::Mat homo = findHomography(imagePoints1, imagePoints2, cv::RANSAC);
	std::cout << "�任����Ϊ��\n" << homo << std::endl << std::endl; //���ӳ�����      

												//������׼ͼ���ĸ���������
	calcCorners(homo, imgX);
	std::cout << "left_top:" << corners.left_top << std::endl;
	std::cout << "left_bottom:" << corners.left_bottom << std::endl;
	std::cout << "right_top:" << corners.right_top << std::endl;
	std::cout << "right_bottom:" << corners.right_bottom << std::endl;

	//ͼ����׼  
	cv::Mat imageTransform1, imageTransform2;
	warpPerspective(imgX, imageTransform1, homo, cv::Size(MAX(corners.right_top.x, corners.right_bottom.x), imgY.rows));


	//����ƴ�Ӻ��ͼ,����ǰ����ͼ�Ĵ�С
	int dst_width = imageTransform1.cols;  //ȡ���ҵ�ĳ���Ϊƴ��ͼ�ĳ���
	int dst_height = imgY.rows;

	cv::Mat dst(dst_height, dst_width, CV_8UC3);
	dst.setTo(0);

	imageTransform1.copyTo(dst(cv::Rect(0, 0, imageTransform1.cols, imageTransform1.rows)));
	imgY.copyTo(dst(cv::Rect(0, 0, imgY.cols, imgY.rows)));

	optimizeSeam(imgY, imageTransform1, dst);

	return dst;
}

cv::Mat Combine::stitch(cv::Mat imgX, cv::Mat imgY)
{
	std::vector<cv::Mat> imgs{ imgX,imgY };
	cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(cv::Stitcher::PANORAMA);
	// ʹ��stitch��������ƴ��
	cv::Mat pano;
	cv::Stitcher::Status status = stitcher->stitch(imgs, pano);
	if (status != cv::Stitcher::OK)
	{
		std::cout << "Can't stitch images, error code = " << int(status) << std::endl;
		return cv::Mat();
	}
	return pano;
}

cv::Mat Combine::surf(cv::Mat imgX, cv::Mat imgY)
{
	//�Ҷ�ͼת��  
	cv::Mat imgXs, imgYs;
	cv::cvtColor(imgX, imgXs, cv::COLOR_RGB2GRAY);
	cv::cvtColor(imgY, imgYs, cv::COLOR_RGB2GRAY);

	cv::Mat imageDesc1, imageDesc2;
	std::vector<cv::KeyPoint> keyPoint1, keyPoint2;
	cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create(2000);
	surf->detectAndCompute(imgXs, cv::Mat(), keyPoint1, imageDesc1);
	surf->detectAndCompute(imgYs, cv::Mat(), keyPoint2, imageDesc2);

	cv::FlannBasedMatcher matcher;
	std::vector<std::vector<cv::DMatch> > matchePoints;
	std::vector<cv::DMatch> GoodMatchePoints;

	std::vector<cv::Mat> train_desc(1, imageDesc1);
	matcher.add(train_desc);
	matcher.train();

	matcher.knnMatch(imageDesc2, matchePoints, 2);
	std::cout << "total match points: " << matchePoints.size() << std::endl;

	// Lowe's algorithm,��ȡ����ƥ���
	for (int i = 0; i < matchePoints.size(); i++)
	{
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
		}
	}

	cv::Mat first_match;
	drawMatches(imgY, keyPoint2, imgX, keyPoint1, GoodMatchePoints, first_match);

	std::vector<cv::Point2f> imagePoints1, imagePoints2;

	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		imagePoints2.push_back(keyPoint2[GoodMatchePoints[i].queryIdx].pt);
		imagePoints1.push_back(keyPoint1[GoodMatchePoints[i].trainIdx].pt);
	}



	//��ȡͼ��1��ͼ��2��ͶӰӳ����� �ߴ�Ϊ3*3  
	cv::Mat homo = findHomography(imagePoints1, imagePoints2, cv::RANSAC);
	std::cout << "�任����Ϊ��\n" << homo << std::endl << std::endl; //���ӳ�����      

   //������׼ͼ���ĸ���������
	calcCorners(homo, imgX);
	std::cout << "left_top:" << corners.left_top << std::endl;
	std::cout << "left_bottom:" << corners.left_bottom << std::endl;
	std::cout << "right_top:" << corners.right_top << std::endl;
	std::cout << "right_bottom:" << corners.right_bottom << std::endl;

	//ͼ����׼  
	cv::Mat imageTransform1, imageTransform2;
	warpPerspective(imgX, imageTransform1, homo, cv::Size(MAX(corners.right_top.x, corners.right_bottom.x), imgY.rows));


	//����ƴ�Ӻ��ͼ,����ǰ����ͼ�Ĵ�С
	int dst_width = imageTransform1.cols;//imgY.cols;  //ȡ���ҵ�ĳ���Ϊƴ��ͼ�ĳ���
	int dst_height = imgY.rows;

	cv::Mat dst(dst_height, dst_width, CV_8UC3);
	dst.setTo(0);

	imageTransform1.copyTo(dst(cv::Rect(0, 0, imageTransform1.cols, imageTransform1.rows)));
	imgY.copyTo(dst(cv::Rect(0, 0, imgY.cols, imgY.rows)));

	optimizeSeam(imgY, imageTransform1, dst);

	return dst;
}

void Combine::calcCorners(const cv::Mat & H, const cv::Mat & src)
{
	double v2[] = { 0, 0, 1 };//���Ͻ�
	double v1[3];//�任�������ֵ
	cv::Mat V2 = cv::Mat(3, 1, CV_64FC1, v2);  //������
	cv::Mat V1 = cv::Mat(3, 1, CV_64FC1, v1);  //������

	V1 = H * V2;
	//���Ͻ�(0,0,1)
	std::cout << "V2: " << V2 << std::endl;
	std::cout << "V1: " << V1 << std::endl;
	corners.left_top.x = v1[0] / v1[2];
	corners.left_top.y = v1[1] / v1[2];

	//���½�(0,src.rows,1)
	v2[0] = 0;
	v2[1] = src.rows;
	v2[2] = 1;
	V2 = cv::Mat(3, 1, CV_64FC1, v2);  //������
	V1 = cv::Mat(3, 1, CV_64FC1, v1);  //������
	V1 = H * V2;
	corners.left_bottom.x = v1[0] / v1[2];
	corners.left_bottom.y = v1[1] / v1[2];

	//���Ͻ�(src.cols,0,1)
	v2[0] = src.cols;
	v2[1] = 0;
	v2[2] = 1;
	V2 = cv::Mat(3, 1, CV_64FC1, v2);  //������
	V1 = cv::Mat(3, 1, CV_64FC1, v1);  //������
	V1 = H * V2;
	corners.right_top.x = v1[0] / v1[2];
	corners.right_top.y = v1[1] / v1[2];

	//���½�(src.cols,src.rows,1)
	v2[0] = src.cols;
	v2[1] = src.rows;
	v2[2] = 1;
	V2 = cv::Mat(3, 1, CV_64FC1, v2);  //������
	V1 = cv::Mat(3, 1, CV_64FC1, v1);  //������
	V1 = H * V2;
	corners.right_bottom.x = v1[0] / v1[2];
	corners.right_bottom.y = v1[1] / v1[2];
}

void Combine::optimizeSeam(cv::Mat & img1, cv::Mat & trans, cv::Mat & dst)
{
	int start = MIN(corners.left_top.x, corners.left_bottom.x);//��ʼλ�ã����ص��������߽�  

	double processWidth = img1.cols - start;//�ص�����Ŀ��  
	int rows = dst.rows;
	int cols = img1.cols; //ע�⣬������*ͨ����
	double alpha = 1;//img1�����ص�Ȩ��  
	for (int i = 0; i < rows; i++)
	{
		uchar* p = img1.ptr<uchar>(i);  //��ȡ��i�е��׵�ַ
		uchar* t = trans.ptr<uchar>(i);
		uchar* d = dst.ptr<uchar>(i);
		for (int j = start; j < cols; j++)
		{
			//�������ͼ��trans�������صĺڵ㣬����ȫ����img1�е�����
			if (t[j * 3] == 0 && t[j * 3 + 1] == 0 && t[j * 3 + 2] == 0)
			{
				alpha = 1;
			}
			else
			{
				//img1�����ص�Ȩ�أ��뵱ǰ�������ص�������߽�ľ�������ȣ�ʵ��֤�������ַ���ȷʵ��  
				alpha = (processWidth - (j - start)) / processWidth;
			}

			d[j * 3] = p[j * 3] * alpha + t[j * 3] * (1 - alpha);
			d[j * 3 + 1] = p[j * 3 + 1] * alpha + t[j * 3 + 1] * (1 - alpha);
			d[j * 3 + 2] = p[j * 3 + 2] * alpha + t[j * 3 + 2] * (1 - alpha);

		}
	}

}

cv::Mat Combine::runState(cv::Mat imgX, cv::Mat imgY, int state)
{
	try
	{
		switch (state)
		{
		case 1:
			return this->normal(imgX, imgY);
		case 2:
			return this->orb(imgX, imgY);
		case 3:
			return this->surf(imgX, imgY);
		case 4:
			return this->stitch(imgX, imgY);
		}
	}
	catch (const std::exception&)
	{
		std::cerr << "defeat" << std::endl;
		std::cout << "defeat" << std::endl;
		std::cout << "Please choose other images" << std::endl;
		return cv::Mat();
	}
	
}


