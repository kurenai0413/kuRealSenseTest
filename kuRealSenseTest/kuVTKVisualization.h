#pragma once

#ifdef KUVIZ_EXPORTS
#define KUVIZ_API __declspec(dllexport)
#else
#define KUVIZ_API __declspec(dllimport)
#endif

#include <memory>
#include <opencv2/opencv.hpp>
#include <vector>
#include <windows.h>

#pragma region // Forward declaration for VTK //
template <class T>
class  vtkSmartPointer;

class  vtkActor;
#pragma endregion

struct KUVIZ_API kuVTKColor
{
	kuVTKColor();
	kuVTKColor(float rColor, float gColor, float bColor) { R = rColor; G = gColor; B = bColor; };

	float R;
	float G;
	float B;
};

class KUVIZ_API kuVTKViz2D
{
public:
	enum SliceDirection { AxialView, SaggitalView, CoronalView };

	kuVTKViz2D(void);
	~kuVTKViz2D(void);

	bool Initialize(HWND hWnd, int wndWidth, int wndHeight, SliceDirection sDirection);
	void DrawSlice(vtkSmartPointer<vtkImageActor> imgActor, vtkSmartPointer<vtkDICOMImageReader> &dicomReader, SliceDirection sDirection, int idx);

private:
	class kuVTKViz2DImpl;
	std::unique_ptr<kuVTKViz2DImpl> pimpl;
};

class KUVIZ_API kuVTKViz3D
{
public:
	kuVTKViz3D(void);
	~kuVTKViz3D(void);

	bool Initialize(HWND hWnd, int wndWidth, int wndHeight, float scale, kuVTKColor bgColor);

	// Draw functions
	void DrawAxes(vtkSmartPointer<vtkAxesActor> actor, CvMat * transform);
	void DrawByActor(vtkSmartPointer<vtkActor> actor);	
	void DrawAssembly(vtkSmartPointer<vtkAssembly> assemblyActor);

	template<class T>
	void DrawSpheres(vtkSmartPointer<vtkActor> actor, int ptsNum, T inputPts, double radius, kuVTKColor color);

	void DrawSpheresWithColorMap(vtkSmartPointer<vtkActor> actor, int ptsNum, CvPoint3D32f * inputs, double size, kuVTKColor * colorMap);

	template<class T>
	void DrawPoints(vtkSmartPointer<vtkActor> actor, int ptsNum, T inputPts, double size, kuVTKColor color);
	template<class T>
	void DrawPointsWithColorMap(vtkSmartPointer<vtkActor> actor, int ptsNum, T inputPts, double size, T colorMap);

	void DrawPointsWithColorMap(vtkSmartPointer<vtkActor> actor, int ptsNum, CvPoint3D32f * inputPts, double size, kuVTKColor * colorMap);

	template<class T>
	void DrawNormals(vtkSmartPointer<vtkActor> actor, int ptsNum, T pts, T normals);

	void DrawLine(vtkSmartPointer<vtkActor> actor, CvPoint3D32f pt1, CvPoint3D32f pt2, float size, kuVTKColor color);

	template<class T>
	void DrawPlane(vtkSmartPointer<vtkActor> actor, T normal, T center, float size);

private:
	class kuVTKViz3DImpl;
	std::unique_ptr<kuVTKViz3DImpl> pimpl;
};

#pragma region // template forward declaration //
template KUVIZ_API void kuVTKViz3D::DrawSpheres<CvPoint3D32f *>(vtkSmartPointer<vtkActor> actor, int ptsNum, CvPoint3D32f * inputPts, double radius, kuVTKColor color);
template KUVIZ_API void kuVTKViz3D::DrawSpheres<cv::Point3f *>(vtkSmartPointer<vtkActor> actor, int ptsNum, cv::Point3f * inputPts, double radius, kuVTKColor color);
template KUVIZ_API void kuVTKViz3D::DrawSpheres<std::vector<CvPoint3D32f>>(vtkSmartPointer<vtkActor> actor, int ptsNum, std::vector<CvPoint3D32f> inputPts, double radius, kuVTKColor color);
template KUVIZ_API void kuVTKViz3D::DrawSpheres<std::vector<cv::Point3f>>(vtkSmartPointer<vtkActor> actor, int ptsNum, std::vector<cv::Point3f> inputPts, double radius, kuVTKColor color);

template KUVIZ_API void kuVTKViz3D::DrawPoints<CvPoint3D32f *>(vtkSmartPointer<vtkActor> actor, int ptsNum, CvPoint3D32f * inputPts, double size, kuVTKColor color);
template KUVIZ_API void kuVTKViz3D::DrawPoints<cv::Point3f *>(vtkSmartPointer<vtkActor> actor, int ptsNum, cv::Point3f * inputPts, double size, kuVTKColor color);
template KUVIZ_API void kuVTKViz3D::DrawPoints<std::vector<CvPoint3D32f>>(vtkSmartPointer<vtkActor> actor, int ptsNum, std::vector<CvPoint3D32f> inputPts, double size, kuVTKColor color);
template KUVIZ_API void kuVTKViz3D::DrawPoints<std::vector<cv::Point3f>>(vtkSmartPointer<vtkActor> actor, int ptsNum, std::vector<cv::Point3f> inputPts, double size, kuVTKColor color);

template KUVIZ_API void kuVTKViz3D::DrawNormals<std::vector<cv::Point3f>>(vtkSmartPointer<vtkActor> actor, int ptsNum, std::vector<cv::Point3f> pts, std::vector<cv::Point3f> normals);
template KUVIZ_API void kuVTKViz3D::DrawNormals<CvPoint3D32f *>(vtkSmartPointer<vtkActor> actor, int ptsNum, CvPoint3D32f * pts, CvPoint3D32f * normals);

template KUVIZ_API void kuVTKViz3D::DrawPointsWithColorMap<CvPoint3D32f *>(vtkSmartPointer<vtkActor> actor, int ptsNum, CvPoint3D32f * inputPts,
																		 double size, CvPoint3D32f * colorMap);
template KUVIZ_API void kuVTKViz3D::DrawPointsWithColorMap<cv::Point3f *>(vtkSmartPointer<vtkActor> actor, int ptsNum, cv::Point3f * inputPts,
																		double size, cv::Point3f * colorMap);
template KUVIZ_API void kuVTKViz3D::DrawPointsWithColorMap<std::vector<CvPoint3D32f>>(vtkSmartPointer<vtkActor> actor, int ptsNum, std::vector<CvPoint3D32f> inputPts,
																					double size, std::vector<CvPoint3D32f> colorMap);
template KUVIZ_API void kuVTKViz3D::DrawPointsWithColorMap<std::vector<cv::Point3f>>(vtkSmartPointer<vtkActor> actor, int ptsNum, std::vector<cv::Point3f> inputPts,
																				   double size, std::vector<cv::Point3f> colorMap);

template KUVIZ_API void kuVTKViz3D::DrawPlane<cv::Point3f>(vtkSmartPointer<vtkActor> actor, cv::Point3f normal, cv::Point3f center, float size);
#pragma endregion