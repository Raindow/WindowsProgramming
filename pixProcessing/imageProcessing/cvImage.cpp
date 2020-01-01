#include "stdafx.h"
#include "cvImage.h"

void CcvImage::createMat ( )
{
	m_nWidth		= GetWidth ( );
	m_nHeight		= GetHeight ( );
	m_nChannels		= GetBPP ( ) / 8;
	m_nWidthBytes	= GetPitch ( );

	m_img.release ( );

	// reconstructing mat
	if ( 1 == m_nChannels )
		m_img.create ( m_nHeight, m_nWidth, CV_8UC1 );
	else if ( 3 == m_nChannels )
		m_img.create ( m_nHeight, m_nWidth, CV_8UC3 );

	// copy data
	uchar*	pucRow;
	uchar*	pucImage	= ( uchar* ) GetBits ( );

	for ( int nRow = 0; nRow < m_nHeight; nRow++ )
	{
		pucRow = (m_img.ptr<uchar> ( nRow ));

		for ( int nCol = 0; nCol < m_nWidth; nCol++ )
		{
			if ( 1 == m_nChannels )
				pucRow [ nCol ] = *(pucImage + nRow * m_nWidthBytes + nCol);
			else if ( 3 == m_nChannels )
			{
				for ( int nCha = 0; nCha < 3; nCha++ )
					pucRow [ nCol * 3 + nCha ] = *(pucImage + nRow * m_nWidthBytes + nCol * 3 + nCha);
			}
		}
	}
}

BOOL CcvImage::createFromMat ( )
{
	if ( 0 == m_img.total ( ) )
		return	FALSE;

	m_nWidth		= m_img.cols;
	m_nHeight		= m_img.rows;
	m_nChannels		= m_img.channels( );
//	m_nWidthBytes	= GetPitch ( );

	// rebuild CImage;
	Destroy ( );
	Create ( m_nWidth, m_nHeight, 8 * m_nChannels );

	// copy data
	uchar*	pucRow;									// line pointer to the data
	uchar*	pucImage	= ( uchar* ) GetBits ( );	// data pointer

	m_nWidthBytes	= GetPitch ( );			// 每行字节数, 可能返回负值

	if ( 1 == m_nChannels )	// 单通道
	{
		RGBQUAD*	rgbquadColorTable;

		int	nMaxColors = 256;

		rgbquadColorTable = new RGBQUAD [ nMaxColors ];

		GetColorTable ( 0, nMaxColors, rgbquadColorTable );

		for ( int nColor = 0; nColor < nMaxColors; nColor++ )
		{
			rgbquadColorTable [ nColor ].rgbBlue	= ( uchar ) nColor;
			rgbquadColorTable [ nColor ].rgbGreen	= ( uchar ) nColor;
			rgbquadColorTable [ nColor ].rgbRed		= ( uchar ) nColor;
		}

		SetColorTable ( 0, nMaxColors, rgbquadColorTable );

		delete [ ]rgbquadColorTable;
	}

	for ( int nRow = 0; nRow < m_nHeight; nRow++ )
	{
		pucRow = (m_img.ptr<uchar> ( nRow ));

		for ( int nCol = 0; nCol < m_nWidth; nCol++ )
		{
			if ( 1 == m_nChannels )
			{
				*(pucImage + nRow * m_nWidthBytes + nCol) = pucRow [ nCol ];
			}
			else if ( 3 == m_nChannels )
			{
				for ( int nCha = 0; nCha < 3; nCha++ )
				{
					*(pucImage + nRow * m_nWidthBytes + nCol * 3 + nCha) = pucRow [ nCol * 3 + nCha ];
				}
			}
		}
	}

	return	TRUE;
}


CcvImage::CcvImage ( )
{
}


CcvImage::~CcvImage ( )
{
}

void CcvImage::xfeatures2dExtract ( vector<KeyPoint>& keyPoints, Mat& imageDesc )
{
	int		minHessian	= 1000;

	Mat	imageGray;

	cvtColor ( m_img, imageGray, CV_RGB2GRAY );

	Ptr<SurfFeatureDetector>	surfDetector	= xfeatures2d::SurfFeatureDetector::create ( minHessian );

//	surfDetector->detect ( m_img, keyPoints );
	surfDetector->detect ( imageGray, keyPoints );

	Ptr<SurfDescriptorExtractor>	SurfDescriptor	= xfeatures2d::SurfDescriptorExtractor::create ( );

//	SurfDescriptor->compute ( m_img, keyPoints, imageDesc );
	SurfDescriptor->compute ( imageGray, keyPoints, imageDesc );
}

void CcvImage::xfeatures2dExtract ( )
{
	xfeatures2dExtract ( m_keyPoints, m_imageDesc );
}

