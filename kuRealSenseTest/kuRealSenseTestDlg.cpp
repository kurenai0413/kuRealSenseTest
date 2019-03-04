
// kuRealSenseTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kuRealSenseTest.h"
#include "kuRealSenseTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define RS_COLOR_WIDTH  640
#define RS_COLOR_HEIGHT 480
#define RS_DEPTH_WIDTH  640
#define RS_DEPTH_HEIGHT 480

#define testROISize		31

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

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

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CkuRealSenseTestDlg dialog



CkuRealSenseTestDlg::CkuRealSenseTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KUREALSENSETEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CkuRealSenseTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ModeSelection, m_ModeSelectionComboBox);
}

BEGIN_MESSAGE_MAP(CkuRealSenseTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_StartRS, &CkuRealSenseTestDlg::OnBnClickedStartrs)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_StopRS, &CkuRealSenseTestDlg::OnBnClickedStoprs)
	ON_CBN_SELCHANGE(IDC_ExamPercentage, &CkuRealSenseTestDlg::OnCbnSelchangeExampercentage)
END_MESSAGE_MAP()


// CkuRealSenseTestDlg message handlers

BOOL CkuRealSenseTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	AllocConsole();
	FILE * stream;
	errno_t	err = freopen_s(&stream, "CONOUT$", "w+", stdout);

	cv::namedWindow("ColorView", 0);
	cv::resizeWindow("ColorView", RS_COLOR_WIDTH, RS_COLOR_HEIGHT);
	HWND hWndColor = (HWND)cvGetWindowHandle("ColorView");
	HWND hColorParent = ::GetParent(hWndColor);
	::SetParent(hWndColor, GetDlgItem(IDC_ColorView)->m_hWnd);
	::ShowWindow(hColorParent, SW_HIDE);

	cv::namedWindow("DepthView", 0);
	cv::resizeWindow("DepthView", RS_COLOR_WIDTH, RS_COLOR_HEIGHT);
	HWND hWndDepth = (HWND)cvGetWindowHandle("DepthView");
	HWND hDepthParent = ::GetParent(hWndDepth);
	::SetParent(hWndDepth, GetDlgItem(IDC_DepthView)->m_hWnd);
	::ShowWindow(hDepthParent, SW_HIDE);

	m_ModeSelectionComboBox.InsertString(Accuracy, L"Accuracy");
	m_ModeSelectionComboBox.InsertString(FrameFillRate, L"Frame Fill Rate");
	m_ModeSelectionComboBox.InsertString(STDEV, L"STDEV");
	m_ModeSelectionComboBox.InsertString(TemporalNoise, L"Temporal Noise");
	m_ModeSelectionComboBox.SetCurSel(Accuracy);
	m_SelectedMode = (TestMode)m_ModeSelectionComboBox.GetCurSel();

	m_isRSStarted = false;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CkuRealSenseTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CkuRealSenseTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CkuRealSenseTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CkuRealSenseTestDlg::OnBnClickedStartrs()
{
	// TODO: Add your control notification handler code here
	m_RSConfig.enable_stream(RS2_STREAM_COLOR, RS_COLOR_WIDTH, RS_COLOR_HEIGHT, RS2_FORMAT_BGR8, 30);
	m_RSConfig.enable_stream(RS2_STREAM_DEPTH, RS_DEPTH_WIDTH, RS_DEPTH_HEIGHT, RS2_FORMAT_Z16, 30);
	m_RSProfile = m_RSPipeline.start(m_RSConfig);

	m_CenterPoint = cv::Point(0.5 * RS_DEPTH_WIDTH, 0.5 * RS_DEPTH_HEIGHT);

	m_DepthScale = GetDepthScale(m_RSProfile.get_device());
	
	m_isRSStarted = true;

	m_RSColorThread = std::thread(&CkuRealSenseTestDlg::RSThreadFun, this);
}

rs2_stream CkuRealSenseTestDlg::FindStreamToAlign(const std::vector<rs2::stream_profile>& streams)
{
	rs2_stream align_to = RS2_STREAM_ANY;
	bool depth_stream_found = false;
	bool color_stream_found = false;
	for (rs2::stream_profile sp : streams)
	{
		rs2_stream profile_stream = sp.stream_type();
		if (profile_stream != RS2_STREAM_DEPTH)
		{
			if (!color_stream_found)         //Prefer color
				align_to = profile_stream;

			if (profile_stream == RS2_STREAM_COLOR)
			{
				color_stream_found = true;
			}
		}
		else
		{
			depth_stream_found = true;
		}
	}

	if (!depth_stream_found)
		throw std::runtime_error("No Depth stream available");

	if (align_to == RS2_STREAM_ANY)
		throw std::runtime_error("No stream found to align with Depth");

	return align_to;
}

void CkuRealSenseTestDlg::RSThreadFun()
{
	rs2::colorizer	color_map;

	rs2_stream		align_to = FindStreamToAlign(m_RSProfile.get_streams());
	rs2::align		align(align_to);

	std::chrono::high_resolution_clock::time_point startT, endT;
	
	while (m_isRSStarted)
	{
		startT = std::chrono::high_resolution_clock::now();

		m_RSFrames = m_RSPipeline.wait_for_frames();
		auto processed = align.process(m_RSFrames);

		rs2::frame color_frame = m_RSFrames.get_color_frame();
		rs2::frame depth_frame = m_RSFrames.get_depth_frame().apply_filter(color_map);
		
		//rs2::video_frame colorFrame = processed.first(align_to);
		//rs2::depth_frame aligned_depth_frame = processed.get_depth_frame();

		// Creating OpenCV Matrix from a color image
		cv::Mat colorImg(cv::Size(RS_COLOR_WIDTH, RS_COLOR_HEIGHT), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
		cv::Mat depthImg(cv::Size(RS_DEPTH_WIDTH, RS_DEPTH_HEIGHT), CV_8UC3, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);

		//cv::Mat colorImg(cv::Size(640, 480), CV_8UC3, (void*)colorFrame.get_data(), cv::Mat::AUTO_STEP);
		//cv::Mat alignedDepthImg(cv::Size(640, 480), CV_8UC3, (void*)aligned_depth_frame.get_data(), cv::Mat::AUTO_STEP);

		//cv::imshow("ColorView", colorImg);
		//cv::imshow("DepthView", alignedDepthImg);
		
		cv::Rect testROI;

		#pragma region // Testing items //
		switch (m_SelectedMode)
		{
		case TestMode::Accuracy:
			testROI = cv::Rect(m_CenterPoint.x - 0.5 * testROISize, m_CenterPoint.y - 0.5 * testROISize, testROISize, testROISize);
			cv::rectangle(depthImg, testROI,CV_RGB(0, 255, 0), 2, CV_AA);
			break;
		case TestMode::FrameFillRate:
				
			break;
		case TestMode::STDEV:
			
			break;
		case TestMode::TemporalNoise:
			
			break;
		default:
			break;
		}
		#pragma endregion

		endT = std::chrono::high_resolution_clock::now();
		auto diffT = std::chrono::duration_cast<std::chrono::milliseconds>(endT - startT);
		
		double m_FPS = 1000.0f / (float)diffT.count();
		//std::cout << (float)diffT.count() << std::endl;

		std::string fpsString = std::to_string(m_FPS);
		int baseline;
		cv::Size fpsTextSize = cv::getTextSize(fpsString, cv::FONT_HERSHEY_COMPLEX, 1, 2, &baseline);
		cv::putText(colorImg, fpsString, cv::Point(colorImg.cols - fpsTextSize.width, fpsTextSize.height),
					CV_FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 255, 0), 2, CV_AA);

		cv::imshow("ColorView", colorImg);
		cv::imshow("DepthView", depthImg);
		cv::waitKey(1);
	}
}

float CkuRealSenseTestDlg::GetDepthScale(rs2::device dev)
{
	// Go over the device's sensors
	for (rs2::sensor& sensor : dev.query_sensors())
	{
		// Check if the sensor if a depth sensor
		if (rs2::depth_sensor dpt = sensor.as<rs2::depth_sensor>())
		{
			return dpt.get_depth_scale();
		}
	}
}

void CkuRealSenseTestDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	if (m_isRSStarted)
	{
		m_isRSStarted = false;

		if (m_RSColorThread.joinable())
			m_RSColorThread.join();

		m_RSPipeline.stop();
	}
}

void CkuRealSenseTestDlg::OnBnClickedStoprs()
{
	// TODO: Add your control notification handler code here

}


void CkuRealSenseTestDlg::OnCbnSelchangeExampercentage()
{
	// TODO: Add your control notification handler code here
	m_SelectedMode = (TestMode)m_ModeSelectionComboBox.GetCurSel();
}
