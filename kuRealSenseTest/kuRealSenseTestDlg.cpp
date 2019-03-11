
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

#define	DistPercentage  0.35

#define FrameCnt		200

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
	DDX_Text(pDX, IDC_GTDistEdit, m_GTDistance);
}

BEGIN_MESSAGE_MAP(CkuRealSenseTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_StartRS, &CkuRealSenseTestDlg::OnBnClickedStartrs)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_StopRS, &CkuRealSenseTestDlg::OnBnClickedStoprs)
	ON_CBN_SELCHANGE(IDC_ModeSelection, &CkuRealSenseTestDlg::OnCbnSelchangeModeselection)
	ON_BN_CLICKED(IDC_SetDistanceValue, &CkuRealSenseTestDlg::OnBnClickedSetdistancevalue)
	ON_BN_CLICKED(IDC_SaveTemporalNoise, &CkuRealSenseTestDlg::OnBnClickedSavetemporalnoise)
	ON_BN_CLICKED(IDC_WriteROI3DPoints, &CkuRealSenseTestDlg::OnBnClickedWriteroi3dpoints)
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
	UpdateData(TRUE);

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

	m_DepthContainerCenter.resize(200);
	m_DepthContainerLT.resize(200);
	m_DepthContainerLB.resize(200);
	m_DepthContainerRT.resize(200);
	m_DepthContainerRB.resize(200);

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
	if (!m_isRSStarted)
	{
		m_RSConfig.enable_stream(RS2_STREAM_COLOR, RS_COLOR_WIDTH, RS_COLOR_HEIGHT, RS2_FORMAT_BGR8, 30);
		m_RSConfig.enable_stream(RS2_STREAM_DEPTH, RS_DEPTH_WIDTH, RS_DEPTH_HEIGHT, RS2_FORMAT_Z16, 30);
		m_RSProfile = m_RSPipeline.start(m_RSConfig);

		int halfWidth = 0.5 * RS_DEPTH_WIDTH;
		int halfHeight = 0.5 * RS_DEPTH_HEIGHT;
		m_CenterPoint = cv::Point(halfWidth, halfHeight);
		m_LTPoint = cv::Point(m_CenterPoint.x - DistPercentage * halfWidth,
			m_CenterPoint.y - DistPercentage * halfHeight);
		m_LBPoint = cv::Point(m_CenterPoint.x - DistPercentage * halfWidth,
			m_CenterPoint.y + DistPercentage * halfHeight);
		m_RTPoint = cv::Point(m_CenterPoint.x + DistPercentage * halfWidth,
			m_CenterPoint.y - DistPercentage * halfHeight);
		m_RBPoint = cv::Point(m_CenterPoint.x + DistPercentage * halfWidth,
			m_CenterPoint.y + DistPercentage * halfHeight);

		m_STDEVROI = cv::Rect(m_LTPoint, m_RBPoint);

		m_DepthScale = GetDepthScale(m_RSProfile.get_device());

		m_isRSStarted = true;

		m_RSColorThread = std::thread(&CkuRealSenseTestDlg::RSThreadFun, this);
	}
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
		//std::lock_guard<std::mutex> gLock(m_Mutex);

		startT = std::chrono::high_resolution_clock::now();

		m_RSFrames	   = m_RSPipeline.wait_for_frames();
		auto processed = align.process(m_RSFrames);

		rs2::frame		 colorFrame		= m_RSFrames.get_color_frame();
		rs2::depth_frame depthFrame		= m_RSFrames.get_depth_frame();
		rs2::frame		 depthFrameShow = m_RSFrames.get_depth_frame().apply_filter(color_map);
		
		//rs2::video_frame colorFrame = processed.first(align_to);
		//rs2::depth_frame aligned_depth_frame = processed.get_depth_frame();

		// Creating OpenCV Matrix from a color image
		cv::Mat colorImg(cv::Size(RS_COLOR_WIDTH, RS_COLOR_HEIGHT), CV_8UC3, (void*)colorFrame.get_data(), cv::Mat::AUTO_STEP);
		cv::Mat depthImg(cv::Size(RS_DEPTH_WIDTH, RS_DEPTH_HEIGHT), CV_8UC3, (void*)depthFrameShow.get_data(), cv::Mat::AUTO_STEP);

		//cv::Mat colorImg(cv::Size(640, 480), CV_8UC3, (void*)colorFrame.get_data(), cv::Mat::AUTO_STEP);
		//cv::Mat alignedDepthImg(cv::Size(640, 480), CV_8UC3, (void*)aligned_depth_frame.get_data(), cv::Mat::AUTO_STEP);

		//cv::imshow("ColorView", colorImg);
		//cv::imshow("DepthView", alignedDepthImg);

		int			 validPixelCnt;
		cv::Rect	 testROI;
		int			 centerPixelIdx = RS_DEPTH_WIDTH * m_CenterPoint.y + m_CenterPoint.x;
		float		 centerDist;
		float		 LTDist, LBDist, RTDist, RBDist;

		int			 baseline;

		std::string	 fillrateString;
		std::string  centerDistString;
		std::string	 LTDistString, LBDistString;
		std::string	 RTDistString, RBDistString;
		std::string  errorDistString;

		cv::Size	 fillrateTextSize;
		cv::Size	 centerDistTextSize;
		cv::Size	 errorTextSize;

		float		 errorValue;

		#pragma region // Testing items //
		switch (m_SelectedMode)
		{
		case TestMode::Accuracy:
			cv::circle(depthImg, m_CenterPoint, 1, CV_RGB(255, 0, 0), 3, CV_AA);
			centerDist = 1000 * depthFrame.get_distance(RS_DEPTH_WIDTH / 2, RS_DEPTH_HEIGHT / 2);
			std::cout << centerDist << std::endl;

			//centerDistString   = std::to_string(centerDist);
			//centerDistTextSize = cv::getTextSize(centerDistString, cv::FONT_HERSHEY_COMPLEX, 1, 2, &baseline);
			//cv::putText(depthImg, centerDistString, cv::Point(colorImg.cols - centerDistTextSize.width, centerDistTextSize.height),
			//			CV_FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 255, 0), 2, CV_AA);

			errorValue = abs(centerDist - m_GTDistance);
			errorDistString = std::to_string(errorValue);
			errorTextSize   = cv::getTextSize(errorDistString, cv::FONT_HERSHEY_COMPLEX, 1, 2, &baseline);
			cv::putText(depthImg, errorDistString, cv::Point(colorImg.cols - errorTextSize.width, errorTextSize.height),
						CV_FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 255, 0), 2, CV_AA);

			break;

		case TestMode::FrameFillRate:
			validPixelCnt = 0;
			for (int i = 0; i < RS_DEPTH_WIDTH; i++)
			{
				for (int j = 0; j < RS_DEPTH_HEIGHT; j++)
				{
					float dist = depthFrame.get_distance(i, j);

					if (dist > 0)
					{
						validPixelCnt++;
					}
				}
			}

			m_FrameFillRate = (float)validPixelCnt / (float)(RS_DEPTH_WIDTH * RS_DEPTH_HEIGHT);
			std::cout << m_FrameFillRate << std::endl;

			fillrateString   = std::to_string(m_FrameFillRate);
			fillrateTextSize = cv::getTextSize(fillrateString, cv::FONT_HERSHEY_COMPLEX, 1, 2, &baseline);
			cv::putText(depthImg, fillrateString, cv::Point(colorImg.cols - fillrateTextSize.width, fillrateTextSize.height),
						CV_FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 255, 0), 2, CV_AA);

			break;

		case TestMode::STDEV:
			cv::rectangle(colorImg, m_STDEVROI, CV_RGB(255, 0, 0), 2, CV_AA);
			ExtractROI3DPoints(depthFrame, m_STDEVROI, m_ROI3DPoints, m_ROI3DPtsNum);

			break;

		case TestMode::TemporalNoise:
			cv::circle(depthImg, m_CenterPoint, 1, CV_RGB(255, 0, 0), 3, CV_AA);
			cv::circle(depthImg, m_LTPoint, 1, CV_RGB(255, 0, 0), 3, CV_AA);
			cv::circle(depthImg, m_LBPoint, 1, CV_RGB(255, 0, 0), 3, CV_AA);
			cv::circle(depthImg, m_RTPoint, 1, CV_RGB(255, 0, 0), 3, CV_AA);
			cv::circle(depthImg, m_RBPoint, 1, CV_RGB(255, 0, 0), 3, CV_AA);

			centerDist = depthFrame.get_distance(m_CenterPoint.x, m_CenterPoint.y);
			LTDist	   = depthFrame.get_distance(m_LTPoint.x, m_LTPoint.y);
			LBDist	   = depthFrame.get_distance(m_LBPoint.x, m_LBPoint.y);
			RTDist	   = depthFrame.get_distance(m_RTPoint.x, m_RTPoint.y);
			RBDist	   = depthFrame.get_distance(m_RBPoint.x, m_RBPoint.y);

			centerDistString = std::to_string(centerDist);
			LTDistString	 = std::to_string(LTDist);
			LBDistString	 = std::to_string(LBDist);
			RTDistString	 = std::to_string(RTDist);
			RBDistString	 = std::to_string(RBDist);

			cv::putText(depthImg, centerDistString, 
						cv::Point(m_CenterPoint.x, m_CenterPoint.y), CV_FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 255, 0), 2, CV_AA);
			cv::putText(depthImg, LTDistString,
						cv::Point(m_LTPoint.x, m_LTPoint.y), CV_FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 255, 0), 2, CV_AA);
			cv::putText(depthImg, LBDistString,
						cv::Point(m_LBPoint.x, m_LBPoint.y), CV_FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 255, 0), 2, CV_AA);
			cv::putText(depthImg, RTDistString,
						cv::Point(m_RTPoint.x, m_RTPoint.y), CV_FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 255, 0), 2, CV_AA);
			cv::putText(depthImg, RBDistString,
						cv::Point(m_RBPoint.x, m_RBPoint.y), CV_FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 255, 0), 2, CV_AA);
		
			m_Mutex.lock();

			if (m_FrameCnt < 200)
			{
				//m_DepthContainer.push_back();
				m_DepthContainerCenter[m_FrameCnt] = 1000 * centerDist;
				m_DepthContainerLT[m_FrameCnt]	   = 1000 * LTDist;
				m_DepthContainerLB[m_FrameCnt]	   = 1000 * LBDist;
				m_DepthContainerRT[m_FrameCnt]	   = 1000 * RTDist;
				m_DepthContainerRB[m_FrameCnt]	   = 1000 * RBDist;

				m_FrameCnt++;
			}
			else if (m_FrameCnt == 200)
			{
				m_DepthContainerCenter.erase(m_DepthContainerCenter.begin());
				m_DepthContainerLT.erase(m_DepthContainerLT.begin());
				m_DepthContainerLB.erase(m_DepthContainerLB.begin());
				m_DepthContainerRT.erase(m_DepthContainerRT.begin());
				m_DepthContainerRB.erase(m_DepthContainerRB.begin());

				m_DepthContainerCenter.push_back(1000 * centerDist);
				m_DepthContainerLT.push_back(1000 * LTDist);
				m_DepthContainerLB.push_back(1000 * LBDist);
				m_DepthContainerRT.push_back(1000 * RTDist);
				m_DepthContainerRB.push_back(1000 * RBDist);
			}

			m_Mutex.unlock();

			std::cout << m_FrameCnt << std::endl;
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

void CkuRealSenseTestDlg::ExtractROI3DPoints(rs2::depth_frame depthFrame, cv::Rect ROIRect, CvPoint3D32f * &ROI3DPts, int &ptsNum)
{
	//std::lock_guard<std::mutex> gLock(m_Mutex);
	m_Mutex.lock();

	if (ROI3DPts != NULL)
	{
		delete[] ROI3DPts;
	}

	rs2::pointcloud				pc;
	rs2::points					points;
	const rs2::vertex		*	vertices;

	std::vector<cv::Point3f>	ptsTemp;

	points   = pc.calculate(depthFrame);
	vertices = points.get_vertices();

	for (int i=ROIRect.x; i< ROIRect.x + ROIRect.width; i++)
	{
		for (int j = ROIRect.y; j < ROIRect.y + ROIRect.height; j++)
		{
			//float dist = depthFrame.get_distance(i, j);

			ptsTemp.push_back(cv::Point3f(vertices[RS_DEPTH_WIDTH*j + i].x,
										  vertices[RS_DEPTH_WIDTH*j + i].y,
										  vertices[RS_DEPTH_WIDTH*j + i].z));
		}
	}

	ptsNum   = (int)ptsTemp.size();
	ROI3DPts = new CvPoint3D32f[ptsNum];

	for (int i = 0; i < ptsNum; i++)
	{
		ROI3DPts[i] = ptsTemp[i];
	}

	m_Mutex.unlock();
}

void CkuRealSenseTestDlg::WritePtsToFile(std::string filename, CvPoint3D32f * pts, int ptsNum)
{
	// Write to file
	std::fstream file;
	file.open(filename, std::ios::out);
	for (int i = 0; i < ptsNum; i++)
	{
		file << pts[i].x << " "
			 << pts[i].y << " "
			 << pts[i].z << std::endl;
	}
	file.close();
}

void CkuRealSenseTestDlg::WriteTemporalDistVector(std::string filename, std::vector<float> temporalDist)
{
	std::fstream file;
	file.open(filename, std::ios::out);
	for (int i=0;i< temporalDist.size();i++)
	{
		file << temporalDist[i] << std::endl;
	}
	file.close();
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

void CkuRealSenseTestDlg::OnCbnSelchangeModeselection()
{
	// TODO: Add your control notification handler code here
	m_SelectedMode = (TestMode)m_ModeSelectionComboBox.GetCurSel();

	switch (m_SelectedMode)
	{
	case TemporalNoise:
		m_FrameCnt = 0;
		break;
	default:
		break;
	}
}

void CkuRealSenseTestDlg::OnBnClickedSetdistancevalue()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CkuRealSenseTestDlg::OnBnClickedSavetemporalnoise()
{
	// TODO: Add your control notification handler code here
	std::lock_guard<std::mutex> gLock(m_Mutex);
	std::string centerFilename = "TemporalDistCenter_" + std::to_string(m_GTDistance) + "mm.txt";
	std::string LTFilename = "TemporalDistLT_" + std::to_string(m_GTDistance) + "mm.txt";
	std::string LBFilename = "TemporalDistLB_" + std::to_string(m_GTDistance) + "mm.txt";
	std::string RTFilename = "TemporalDistRT_" + std::to_string(m_GTDistance) + "mm.txt";
	std::string RBFilename = "TemporalDistRB_" + std::to_string(m_GTDistance) + "mm.txt";

	WriteTemporalDistVector(centerFilename, m_DepthContainerCenter);
	WriteTemporalDistVector(LTFilename, m_DepthContainerLT);
	WriteTemporalDistVector(LBFilename, m_DepthContainerLB);
	WriteTemporalDistVector(RTFilename, m_DepthContainerRT);
	WriteTemporalDistVector(RBFilename, m_DepthContainerRB);
}

void CkuRealSenseTestDlg::OnBnClickedWriteroi3dpoints()
{
	// TODO: Add your control notification handler code here
	std::lock_guard<std::mutex> gLock(m_Mutex);
	std::string roi3DPtsName = "ROI3DPoints_" + std::to_string(m_GTDistance) +"mm.txt";
	std::cout << roi3DPtsName << std::endl;
	std::cout << "ROI 3D Pts Num: " << m_ROI3DPtsNum << std::endl;
	WritePtsToFile(roi3DPtsName, m_ROI3DPoints, m_ROI3DPtsNum);
}
