// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface
// (the "Fluent UI") and is provided only as referential material to supplement the
// Microsoft Foundation Classes Reference and related electronic documentation
// included with the MFC C++ library software.
// License terms to copy, use or distribute the Fluent UI are available separately.
// To learn more about our Fluent UI licensing program, please visit
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// imageDoc.cpp : implementation of the CImageDoc class

// some of the source codes reference to https://www.cnblogs.com/skyfsm/p/7411961.html

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "imageProcessing.h"
#endif

#include "imageDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CImageDoc

IMPLEMENT_DYNCREATE(CImageDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageDoc, CDocument)
END_MESSAGE_MAP()


// CImageDoc construction/destruction
void CImageDoc::calculateCorners ( const Mat& H, const Mat& src )
{
	double	v2[ ]	= { 0, 0, 1 };		// top-left
	double	v1 [ 3 ];					// coordinates after transform
	Mat		matV2	= Mat ( 3, 1, CV_64FC1, v2 );	// column vector
	Mat		matV1	= Mat ( 3, 1, CV_64FC1, v1 );	// column vector

	matV1	= H * matV2;

	// top-left
	corners.left_top.x	= v1 [ 0 ] / v1 [ 2 ];
	corners.left_top.y	= v1 [ 1 ] / v1 [ 2 ];

	// bottom-left
	v2 [ 0 ]	= 0;
	v2 [ 1 ]	= src.rows;
	v2 [ 2 ]	= 1;
	matV2		= Mat ( 3, 1, CV_64FC1, v2 );	// column vector
	matV1		= Mat ( 3, 1, CV_64FC1, v1 );	// column vector

	matV1	= H * matV2;
	corners.left_bottom.x	= v1 [ 0 ] / v1 [ 2 ];
	corners.left_bottom.y	= v1 [ 1 ] / v1 [ 2 ];

	// top-right
	v2 [ 0 ]	= src.cols;
	v2 [ 1 ]	= 0;
	v2 [ 2 ]	= 1;
	matV2		= Mat ( 3, 1, CV_64FC1, v2 );	// column vector
	matV1		= Mat ( 3, 1, CV_64FC1, v1 );	// column vector

	matV1	= H * matV2;
	corners.right_top.x	= v1 [ 0 ] / v1 [ 2 ];
	corners.right_top.y	= v1 [ 1 ] / v1 [ 2 ];

	// bottom-right
	v2 [ 0 ] = src.cols;
	v2 [ 1 ] = src.rows;
	v2 [ 2 ] = 1;
	matV2 = Mat ( 3, 1, CV_64FC1, v2 );	// column vector
	matV1 = Mat ( 3, 1, CV_64FC1, v1 );	// column vector

	matV1 = H * matV2;
	corners.right_bottom.x	= v1 [ 0 ] / v1 [ 2 ];
	corners.right_bottom.y	= v1 [ 1 ] / v1 [ 2 ];
}

CImageDoc::CImageDoc() noexcept
{
	// TODO: add one-time construction code here
	m_pImage = NULL;

	m_bSourceInput	= FALSE;
}

CImageDoc::~CImageDoc()
{
	if ( m_pImage )
		delete m_pImage;
}

BOOL CImageDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CImageDoc serialization

void CImageDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CImageDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CImageDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CImageDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CImageDoc diagnostics

#ifdef _DEBUG
void CImageDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImageDoc commands

BOOL CImageDoc::OnOpenDocument ( LPCTSTR lpszPathName )
{
	//if (!CDocument::OnOpenDocument ( lpszPathName ))
	//	return FALSE;

	// TODO:  Add your specialized creation code here
	HRESULT	hr;

	m_pImage	= new CcvImage;
	hr = m_pImage->Load ( lpszPathName );

	if ( S_OK != hr )
		return	FALSE;

	m_pImage->createMat ( );
	theApp.m_iCnt_pix++;

	m_bSourceInput	= TRUE;

	return TRUE;
}

void CImageDoc::mergePix ( CcvImage *pImage_1, CcvImage *pImage_2 )
{
	int		nWidth, nHeight, nBPP;
	HDC		hDC;

	nBPP = pImage_1->GetBPP ( );
	nWidth = pImage_1->GetWidth ( );
	nHeight = pImage_1->GetHeight ( ) + pImage_2->GetHeight ( );

	if ( nWidth < pImage_2->GetWidth ( ) )
	{
		nWidth = pImage_2->GetWidth ( );
	}

	m_pImage = new CcvImage;

	m_pImage->Create ( nWidth, nHeight, nBPP );

	hDC = m_pImage->GetDC ( );
	pImage_1->BitBlt ( hDC, 0, 0, SRCCOPY );
	pImage_2->BitBlt ( hDC, 0, pImage_1->GetHeight ( ), SRCCOPY );

	m_pImage->ReleaseDC ( );

	UpdateAllViews ( NULL );
}


BOOL CImageDoc::OnSaveDocument ( LPCTSTR lpszPathName )
{
	HRESULT	hr;
	// TODO: Add your specialized code here and/or call the base class
	hr = m_pImage->Save ( lpszPathName, Gdiplus::ImageFormatJPEG );

	if ( S_OK == hr )
		return	TRUE;

	return	FALSE;

	//	return CDocument::OnSaveDocument ( lpszPathName );
}

void CImageDoc::matchSURF ( CcvImage *pImage_1, CcvImage *pImage_2, Ptr<DescriptorMatcher> matcher )
{
	m_pImage = new CcvImage;

	pImage_1->xfeatures2dExtract ( );
	pImage_2->xfeatures2dExtract ( );

//	Ptr<DescriptorMatcher>	matcher	= DescriptorMatcher::create ( "FlannBased" );
//	Ptr<DescriptorMatcher>	matcher	= DescriptorMatcher::create ( "BruteForce" );

	matcher->match ( pImage_1->m_imageDesc, pImage_2->m_imageDesc, m_pImage->m_matchPoints );

	drawMatches ( pImage_1->m_img, pImage_1->m_keyPoints, pImage_2->m_img, pImage_2->m_keyPoints, m_pImage->m_matchPoints, m_pImage->m_img );

	m_pImage->createFromMat ( );

	UpdateAllViews ( NULL );
}

void CImageDoc::jointPix ( CcvImage *pImage_1, CcvImage *pImage_2, Ptr<DescriptorMatcher> matcher )
{
	vector<vector<DMatch>>	rawMatchPoints;

	m_pImage = new CcvImage;

	pImage_1->xfeatures2dExtract ( );
	pImage_2->xfeatures2dExtract ( );

	vector<Mat>		train_desc ( 1, pImage_1->m_imageDesc );
	matcher->add ( train_desc );
	matcher->train ( );

	matcher->knnMatch ( pImage_2->m_imageDesc, rawMatchPoints, 2 );

	// Lowe's algorithm
	int		i;
	for ( i = 0; i < rawMatchPoints.size ( ); i++ )
	{
		if ( rawMatchPoints [ i ] [ 0 ].distance < 0.4 * rawMatchPoints [ i ] [ 1 ].distance )
		{
			m_pImage->m_matchPoints.push_back ( rawMatchPoints [ i ] [ 0 ] );
		}
	}

//	drawMatches ( pImage_2->m_img, pImage_2->m_keyPoints, pImage_1->m_img, pImage_1->m_keyPoints, m_pImage->m_matchPoints, m_pImage->m_img );

//	m_pImage->createFromMat ( );

	vector<Point2f>		imgPt_1, imgPt_2;

	for ( i = 0; i < m_pImage->m_matchPoints.size ( ); i++ )
	{
		imgPt_2.push_back ( pImage_2->m_keyPoints [ m_pImage->m_matchPoints [ i ].queryIdx ].pt );
		imgPt_1.push_back ( pImage_1->m_keyPoints [ m_pImage->m_matchPoints [ i ].trainIdx ].pt );
	}

	// calculating the projection matrix from pImage_1 to pImage_2
	vector<uchar>	m;
	Mat	homography	= findHomography ( imgPt_1, imgPt_2, CV_RANSAC, 3, m );

	calculateCorners ( homography, pImage_1->m_img );

	Mat	imgT_1, imgT_2;

	warpPerspective ( pImage_1->m_img, imgT_1, homography, Size ( MAX ( corners.right_top.x, corners.right_bottom.x ), pImage_2->m_img.rows ) );
	// now that imgT_1 is the perspective transform of pImage_1->m_img

	int		dst_width	= imgT_1.cols;
	int		dst_height	= pImage_2->m_img.rows;

	Mat	imgDst ( dst_height, dst_width, CV_8UC3 );
	imgDst.setTo ( 0 );

	imgT_1.copyTo ( imgDst ( Rect ( 0, 0, imgT_1.cols, imgT_1.rows ) ) );
	pImage_2->m_img.copyTo ( imgDst ( Rect ( 0, 0, pImage_2->m_img.cols, pImage_2->m_img.rows ) ) );

	seamlessJoint ( pImage_2->m_img, imgT_1, imgDst );

	m_pImage->m_img = imgDst;

	m_pImage->createFromMat ( );

	UpdateAllViews ( NULL );
}

void CImageDoc::seamlessJoint ( Mat& img_1, Mat& trans, Mat& dst )
{
	int		iStart, rows, cols;
	double	dWidth, alpha;

	iStart	= MIN ( corners.left_top.x, corners.left_bottom.x );
	dWidth	= img_1.cols - iStart;		// overlap width
	rows	= dst.rows;
	cols	= img_1.cols;				// cols * channels
	alpha	= 1;						// wight of pixel in img_1

	for ( int i = 0; i < rows; i++ )
	{
		uchar*	p	= img_1.ptr<uchar> ( i );
		uchar*	t	= trans.ptr<uchar> ( i );
		uchar*	d	= dst.ptr<uchar> ( i );

		for ( int j = iStart; j < cols; j++ )
		{
			// trans 中无像素的点， 拷贝 img_1
			if ( t [ j * 3 ] == 0 && t [ j * 3 + 1 ] == 0 && t [ j * 3 + 2 ] == 0 )
			{
				alpha	= 1;
			}
			else
			{
				alpha	= (dWidth - (j - iStart)) / dWidth;
			}

			d [ j * 3 ]		= p [ j * 3 ] * alpha + t [ j * 3 ] * (1 - alpha);
			d [ j * 3 + 1 ]	= p [ j * 3 + 1 ] * alpha + t [ j * 3 + 1 ] * (1 - alpha);
			d [ j * 3 + 2 ]	= p [ j * 3 + 2 ] * alpha + t [ j * 3 + 2 ] * (1 - alpha);
		}
	}
}
