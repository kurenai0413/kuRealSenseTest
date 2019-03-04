
// kuRealSenseTestDlg.h : header file
//

#pragma once
#include <thread>
#include <chrono>

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

	rs2::pipeline			m_RSPipeline;
	rs2::pipeline_profile	m_RSProfile;
	rs2::config				m_RSConfig;
	rs2::frameset			m_RSFrames;

	cv::Mat					m_RSColorFrame;

	std::thread				m_RSColorThread;
	std::thread				m_RSDepthThread;

	float					m_DepthScale;

	bool					m_isRSStarted;

	rs2_stream				FindStreamToAlign(const std::vector<rs2::stream_profile> & streams);
	void					RSThreadFun();
	float					GetDepthScale(rs2::device dev);


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
};
