
// kuRealSenseTestDlg.h : header file
//

#pragma once
#include <thread>
#include <chrono>
#include <mutex>

#include <opencv2/opencv.hpp>
#include <librealsense2/rs.hpp>

// CkuRealSenseTestDlg dialog
class CkuRealSenseTestDlg : public CDialogEx
{
// Construction
public:
	CkuRealSenseTestDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KUREALSENSETEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	enum						TestMode { Accuracy, FrameFillRate, STDEV, TemporalNoise };
	
	rs2::pipeline				m_RSPipeline;
	rs2::pipeline_profile		m_RSProfile;
	rs2::config					m_RSConfig;
	rs2::frameset				m_RSFrames;

	cv::Mat						m_RSColorFrame;

	std::thread					m_RSColorThread;
	std::thread					m_RSDepthThread;

	std::mutex					m_Mutex;

	float						m_DepthScale;

	float						m_GTDistance	= 0;
	float						m_FrameFillRate = 0;

	bool						m_isRSStarted;

	TestMode					m_SelectedMode;

	CvPoint3D32f			*	m_ROI3DPoints = NULL;
	int							m_ROI3DPtsNum;

	int							m_FrameCnt = 0;

	cv::Point					m_CenterPoint;
	cv::Point					m_LTPoint;
	cv::Point					m_LBPoint;
	cv::Point					m_RTPoint;
	cv::Point					m_RBPoint;

	cv::Rect					m_STDEVROI;

	std::vector<float>			m_DepthContainerCenter;
	std::vector<float>			m_DepthContainerLT;
	std::vector<float>			m_DepthContainerLB;
	std::vector<float>			m_DepthContainerRT;
	std::vector<float>			m_DepthContainerRB;

	rs2_stream					FindStreamToAlign(const std::vector<rs2::stream_profile> & streams);
	void						RSThreadFun();
	float						GetDepthScale(rs2::device dev);
	
	void						ExtractROI3DPoints(rs2::depth_frame depthFrame, cv::Rect ROIRect, CvPoint3D32f * &ROI3DPts, int &ptsNum);
	void						WritePtsToFile(std::string filename, CvPoint3D32f * pts, int ptsNum);
	void						WriteTemporalDistVector(std::string filename, std::vector<float> temporalDist);

	CComboBox					m_ModeSelectionComboBox;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartrs();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedStoprs();
	afx_msg void OnCbnSelchangeModeselection();
	afx_msg void OnBnClickedSetdistancevalue();
	afx_msg void OnBnClickedSavetemporalnoise();
	afx_msg void OnBnClickedWriteroi3dpoints();
};
