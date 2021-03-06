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

// imageDoc.h : interface of the CImageDoc class
//


#pragma once

#include "cvImage.h"

typedef struct
{
	Point2f		left_top;
	Point2f		left_bottom;
	Point2f		right_top;
	Point2f		right_bottom;
} four_corners_t;

//class CcvImage;
class CImageDoc : public CDocument
{
protected: // create from serialization only
	CImageDoc() noexcept;
	DECLARE_DYNCREATE(CImageDoc)

// Attributes
public:
	CcvImage *	m_pImage;

	BOOL	m_bSourceInput;

	four_corners_t	corners;

	void	calculateCorners ( const Mat& H, const Mat& src );

// Operations
public:
	void	mergePix ( CcvImage *pImage_1, CcvImage *pImage_2 );
	void	matchSURF ( CcvImage *pImage_1, CcvImage *pImage_2, Ptr<DescriptorMatcher> matcher );
	void	jointPix ( CcvImage *pImage_1, CcvImage *pImage_2, Ptr<DescriptorMatcher> matcher );

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CImageDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void	seamlessJoint ( Mat& img_1, Mat& trans, Mat& dst );

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

public:
	virtual BOOL OnOpenDocument ( LPCTSTR lpszPathName );
	virtual BOOL OnSaveDocument ( LPCTSTR lpszPathName );
};
