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

// imageProcessing.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "imageProcessing.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "imageDoc.h"
#include "imageView.h"
#include "cvImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	_JICHENG_MEM_LEAK_CHECKING
#ifdef _JICHENG_MEM_LEAK_CHECKING
void EnableMemLeackCheck ( )
{
	int		tmpFlag;

	tmpFlag = _CrtSetDbgFlag ( _CRTDBG_REPORT_FLAG );

	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

	_CrtSetDbgFlag ( tmpFlag );
}
#endif	// _JICHENG_MEM_LEAK_CHECKING


// CImageProcessingApp

BEGIN_MESSAGE_MAP(CImageProcessingApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CImageProcessingApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)

	ON_COMMAND ( ID_BTN_MERGE, &CImageProcessingApp::OnBtnMerge )
	ON_UPDATE_COMMAND_UI ( ID_BTN_MERGE, &CImageProcessingApp::OnUpdateBtnMerge )
	ON_COMMAND ( ID_BTN_JOINT, &CImageProcessingApp::OnBtnJoint )
	ON_UPDATE_COMMAND_UI ( ID_BTN_JOINT, &CImageProcessingApp::OnUpdateBtnJoint )
	ON_COMMAND ( ID_BTN_MATCH, &CImageProcessingApp::OnBtnMatch )
	ON_UPDATE_COMMAND_UI ( ID_BTN_MATCH, &CImageProcessingApp::OnUpdateBtnMatch )

	ON_COMMAND ( ID_COMBO_DETECTOR, OnSelectDetector )
	ON_COMMAND ( ID_COMBO_MATCHER, OnSelectMatcher )
	ON_UPDATE_COMMAND_UI ( ID_COMBO_DETECTOR, &CImageProcessingApp::OnUpdateDetector )
	ON_UPDATE_COMMAND_UI ( ID_COMBO_MATCHER, &CImageProcessingApp::OnUpdateMatcher )
	ON_COMMAND ( ID_BTN_POSE_ESTIMATION, &CImageProcessingApp::OnBtnPoseEstimation )
	ON_UPDATE_COMMAND_UI ( ID_BTN_POSE_ESTIMATION, &CImageProcessingApp::OnUpdateBtnPoseEstimation )
	ON_UPDATE_COMMAND_UI ( ID_EDIT_THRESHOLD, &CImageProcessingApp::OnUpdateEditThreshold )
	ON_COMMAND ( ID_EDIT_THRESHOLD, &CImageProcessingApp::OnEditThreshold )
END_MESSAGE_MAP()


// CImageProcessingApp construction

CImageProcessingApp::CImageProcessingApp() noexcept
{
	m_bHiColorIcons		= TRUE;
	m_bOnEdit_threshold	= FALSE;

	m_iCnt_pix			= 0;

#ifdef _JICHENG_MEM_LEAK_CHECKING
	EnableMemLeackCheck ( );

//		_CrtSetBreakAlloc ( 246 );
//		_CrtSetBreakAlloc ( 12390 );
//		_CrtSetBreakAlloc ( 19375 );
#endif	// _JICHENG_MEM_LEAK_CHECKING

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Wuhan University.CS.programmingWidnows.imageProcessing.betaVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CImageProcessingApp object

CImageProcessingApp theApp;


// CImageProcessingApp initialization

BOOL CImageProcessingApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey ( _T ( "jicheng @ CS of Wuhan University, China" ) );
	LoadStdProfileSettings(16);  // Load standard INI file options (including MRU)

	m_iHessianThreshold = GetProfileInt ( _T ( "SURF" ), _T ( "HessianThreshold" ), 1000 );

	InitContextMenuManager();
	InitShellManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	m_pDocTemplate	= new CMultiDocTemplate ( IDR_imageProcessingTYPE,
											 RUNTIME_CLASS ( CImageDoc ),
											 RUNTIME_CLASS ( CChildFrame ), // custom MDI child frame
											 RUNTIME_CLASS ( CImageView ) );
	if ( !m_pDocTemplate )
		return FALSE;
	AddDocTemplate ( m_pDocTemplate );

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	Ptr<FeatureDetector>		detector;
	Ptr<DescriptorExtractor>	extractor;

	detector				= xfeatures2d::SurfFeatureDetector::create ( m_iHessianThreshold );
	extractor				= xfeatures2d::SurfDescriptorExtractor::create ( );
	m_pDetector_current		= detector;
	m_pExtractor_current	= extractor;
	m_detectors.push_back ( detector );
	m_extractors.push_back ( extractor );

	detector			= xfeatures2d::SiftFeatureDetector::create ( );
	extractor			= xfeatures2d::SiftDescriptorExtractor::create ( );
	m_detectors.push_back ( detector );
	m_extractors.push_back ( extractor );


	Ptr<DescriptorMatcher>	matcher;
	
	matcher				= DescriptorMatcher::create ( "BruteForce" );
	m_pMatcher_current	= matcher;
	m_matchers.push_back ( matcher );
	
	matcher				= DescriptorMatcher::create ( "FlannBased" );
	m_matchers.push_back ( matcher );


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CImageProcessingApp::ExitInstance()
{
	WriteProfileInt ( _T ( "SURF" ), _T ( "HessianThreshold" ), m_iHessianThreshold );

	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CImageProcessingApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CImageProcessingApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CImageProcessingApp customization load/save methods

void CImageProcessingApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CImageProcessingApp::LoadCustomState()
{
}

void CImageProcessingApp::SaveCustomState()
{
}

// CImageProcessingApp message handlers


void CImageProcessingApp::OnBtnMerge ( )
{
	CImageDoc *	pDoc;
	CcvImage *	pImage_1;
	CcvImage *	pImage_2;
	POSITION	pos;

	// first get the 2 pix previously openned
	pos = m_pDocTemplate->GetFirstDocPosition ( );

	pDoc = ( CImageDoc* ) m_pDocTemplate->GetNextDoc ( pos );
	while ( pDoc )
	{
		if ( pDoc->m_bSourceInput )
			break;

		pDoc = ( CImageDoc* ) m_pDocTemplate->GetNextDoc ( pos );
	}
	if ( NULL == pDoc )
		return;

	pImage_1 = pDoc->m_pImage;

	pDoc = ( CImageDoc* ) m_pDocTemplate->GetNextDoc ( pos );
	while ( pDoc )
	{
		if ( pDoc->m_bSourceInput )
			break;

		pDoc = ( CImageDoc* ) m_pDocTemplate->GetNextDoc ( pos );
	}
	if ( NULL == pDoc )
		return;

	pImage_2 = pDoc->m_pImage;

	pDoc = ( CImageDoc* ) m_pDocTemplate->OpenDocumentFile ( NULL );

	pDoc->mergePix ( pImage_1, pImage_2 );

	pDoc->SetModifiedFlag ( );
}

void CImageProcessingApp::OnUpdateBtnMerge ( CCmdUI *pCmdUI )
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable ( m_iCnt_pix > 1 );
}

void CImageProcessingApp::OnBtnMatch ( )
{
	CImageDoc *	pDoc;
	CcvImage *	pImage_1;
	CcvImage *	pImage_2;
	POSITION	pos;

	// first get the 2 pix previously openned
	pos = m_pDocTemplate->GetFirstDocPosition ( );

	pDoc = ( CImageDoc* ) m_pDocTemplate->GetNextDoc ( pos );
	while ( pDoc  )
	{
		if ( pDoc->m_bSourceInput )
			break;

		pDoc = ( CImageDoc* ) m_pDocTemplate->GetNextDoc ( pos );
	}
	if ( NULL == pDoc )
		return;

	pImage_1 = pDoc->m_pImage;

	pDoc = ( CImageDoc* ) m_pDocTemplate->GetNextDoc ( pos );
	while ( pDoc )
	{
		if ( pDoc->m_bSourceInput )
			break;

		pDoc = ( CImageDoc* ) m_pDocTemplate->GetNextDoc ( pos );
	}
	if ( NULL == pDoc )
		return;

	pImage_2 = pDoc->m_pImage;

	pDoc = ( CImageDoc* ) m_pDocTemplate->OpenDocumentFile ( NULL );

	pDoc->matchSURF ( pImage_1, pImage_2, m_pDetector_current, m_pExtractor_current, m_pMatcher_current );

	pDoc->SetModifiedFlag ( );
}

void CImageProcessingApp::OnUpdateBtnMatch ( CCmdUI *pCmdUI )
{
	pCmdUI->Enable ( m_iCnt_pix > 1 );
}

void CImageProcessingApp::OnBtnJoint ( )
{
	CImageDoc *	pDoc;
	CcvImage *	pImage_1;
	CcvImage *	pImage_2;
	POSITION	pos;

	// first get the 2 pix previously openned
	pos = m_pDocTemplate->GetFirstDocPosition ( );

	pDoc = ( CImageDoc* ) m_pDocTemplate->GetNextDoc ( pos );
	while ( pDoc )
	{
		if ( pDoc->m_bSourceInput )
			break;

		pDoc = ( CImageDoc* ) m_pDocTemplate->GetNextDoc ( pos );
	}
	if ( NULL == pDoc )
		return;

	pImage_1 = pDoc->m_pImage;

	pDoc = ( CImageDoc* ) m_pDocTemplate->GetNextDoc ( pos );
	while ( pDoc )
	{
		if ( pDoc->m_bSourceInput )
			break;

		pDoc = ( CImageDoc* ) m_pDocTemplate->GetNextDoc ( pos );
	}
	if ( NULL == pDoc )
		return;

	pImage_2 = pDoc->m_pImage;

	pDoc = ( CImageDoc* ) m_pDocTemplate->OpenDocumentFile ( NULL );

	pDoc->jointPix ( pImage_1, pImage_2, m_pDetector_current, m_pExtractor_current, m_pMatcher_current );

	pDoc->SetModifiedFlag ( );
}

void CImageProcessingApp::OnUpdateBtnJoint ( CCmdUI *pCmdUI )
{
	pCmdUI->Enable ( m_iCnt_pix > 1 );
}

void CImageProcessingApp::OnSelectDetector ( )
{
	CMFCRibbonBar *	pRibbon;

	pRibbon = (( CMainFrame* ) AfxGetMainWnd ( ))->GetRibbonBar ( );
	ASSERT_VALID ( pRibbon );

	CMFCRibbonComboBox *	pComboDetector;
	pComboDetector	= DYNAMIC_DOWNCAST ( CMFCRibbonComboBox, pRibbon->FindByID ( ID_COMBO_DETECTOR ) );

	if ( NULL == pComboDetector )
		return;

	if ( 0 == pComboDetector->GetEditText ( ).Compare ( _T ( "SURF" ) ) )
	{
		m_pDetector_current		= m_detectors.at ( 0 );
		m_pExtractor_current	= m_extractors.at ( 0 );
	}

	if ( 0 == pComboDetector->GetEditText ( ).Compare ( _T ( "SIFT" ) ) )
	{
		m_pDetector_current		= m_detectors.at ( 1 );
		m_pExtractor_current	= m_extractors.at ( 1 );
	}
}

void CImageProcessingApp::OnUpdateDetector ( CCmdUI *pCmdUI )
{
	pCmdUI->Enable ( );
}

void CImageProcessingApp::OnSelectMatcher ( )
{
	CMFCRibbonBar *	pRibbon;

	pRibbon	= (( CMainFrame* ) AfxGetMainWnd ( ))->GetRibbonBar ( );
	ASSERT_VALID ( pRibbon );

	CMFCRibbonComboBox *	pComboMatcher;
	pComboMatcher	= DYNAMIC_DOWNCAST ( CMFCRibbonComboBox, pRibbon->FindByID ( ID_COMBO_MATCHER ) );

	if ( NULL == pComboMatcher )
		return;

	// use feature factory in the future version
	if ( 0 == pComboMatcher->GetEditText ( ).Compare ( _T ( "BruteForce" ) ) )
	{
		m_pMatcher_current	= m_matchers.at ( 0 );
	}

	if ( 0 == pComboMatcher->GetEditText ( ).Compare ( _T ( "FlannBased" ) ) )
	{
		m_pMatcher_current	= m_matchers.at ( 1 );
	}
}

void CImageProcessingApp::OnUpdateMatcher ( CCmdUI *pCmdUI )
{
	pCmdUI->Enable ( );
}




void CImageProcessingApp::OnBtnPoseEstimation ( )
{
	// TODO: Add your command handler code here
}


void CImageProcessingApp::OnUpdateBtnPoseEstimation ( CCmdUI *pCmdUI )
{
	pCmdUI->Enable ( m_iCnt_pix > 1 );
}


void CImageProcessingApp::OnUpdateEditThreshold ( CCmdUI *pCmdUI )
{
	CString				strThreshold;
	CMFCRibbonEdit *	pEdit;

	if ( m_bOnEdit_threshold )
		return;

	pEdit = DYNAMIC_DOWNCAST ( CMFCRibbonEdit, (( CMainFrame* ) m_pMainWnd)->getRibbonBar ( )->FindByID ( ID_EDIT_THRESHOLD ) );

	strThreshold.Format ( _T("%d"), m_iHessianThreshold );

	pEdit->SetEditText ( strThreshold );

	m_bOnEdit_threshold = TRUE;
}


void CImageProcessingApp::OnEditThreshold ( )
{
	CString						strThreshold;
	CMFCRibbonEdit *			pEdit;
	Ptr<SurfFeatureDetector>	pSurf;

	pEdit	= DYNAMIC_DOWNCAST ( CMFCRibbonEdit, ((CMainFrame*)m_pMainWnd)->getRibbonBar()->FindByID( ID_EDIT_THRESHOLD ) );

	strThreshold	= pEdit->GetEditText ( );

	m_iHessianThreshold	= _ttoi ( strThreshold );

	// now set the threshold of the detector
	pSurf	= m_detectors.at ( 0 ).staticCast<SurfFeatureDetector> ( );
	pSurf->setHessianThreshold ( m_iHessianThreshold );
}
