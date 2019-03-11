#pragma once


#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkAssembly.h>
#include <vtkAutoInit.h>
#include <vtkAxes.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkConeSource.h>  
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkDICOMImageReader.h>
#include <vtkFollower.h>
#include <vtkGlyph3D.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkLine.h>
#include <vtkLandmarkTransform.h>
#include <vtkPlyReader.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h> 
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkSTLReader.h>
#include <vtkTransform.h>
#include <vtkVectorText.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkWindowLevelLookupTable.h>

VTK_MODULE_INIT(vtkRenderingOpenGL2);				// VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkRenderingContextOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);