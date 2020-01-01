#pragma once

#include <atlimage.h>


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include "opencv2/core/types_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/calib3d/calib3d_c.h"

#include <iostream>
#include <map>

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

class CcvImage : public CImage
{
public:
	Mat		m_img;	// opencv image data

	vector<KeyPoint>	m_keyPoints;		// key points
	Mat					m_imageDesc;		// description
	vector<DMatch>		m_matchPoints;		// match points

	CcvImage ( );
	~CcvImage ( );

	void	createMat ( );
	BOOL	createFromMat ( );
	void	xfeatures2dExtract ( vector<KeyPoint>& keyPoints, Mat& imageDesc );
	void	xfeatures2dExtract ( );

protected:
	int		m_nWidth;
	int		m_nHeight;
	int		m_nChannels;
	int		m_nWidthBytes;
};

