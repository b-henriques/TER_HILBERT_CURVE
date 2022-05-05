# CMakeList.txt : CMake project for ttproj, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

project ("TER_HilbertCurve")

#_________________________________________________
# Add source to this project's executable.
file(GLOB SOURCES
	src/HilbertCurve/*.h
	src/HilbertCurve/*.cpp 
	src/Point2D/*.h
	src/Point2D/*.cpp
	src/PointsGenerators/*.h
	src/PointsGenerators/*.cpp
	src/main.cpp
)

add_executable (TER_HilbertCurve  ${SOURCES})

#_________________________________________________
#Pybind
add_subdirectory(pybind11)
pybind11_add_module(pyHilbertCurve SHARED 
	src/HilbertCurveWrapper.cpp 
	src/Point2D/Point2D.cpp 
	src/HilbertCurve/HilbertCurve2D.cpp
	src/HilbertCurve/HilbertCurve2D_Fixed.cpp
	src/HilbertCurve/HilbertCurve2D_Adaptive.cpp
	src/PointsGenerators/PointsGenerator.h
	src/PointsGenerators/UniformPointsGenerator.cpp
	src/PointsGenerators/GammaPointsGenerator.cpp
	src/PointsGenerators/PiecewiseConstantPointsGenerator.cpp
	src/PointsGenerators/PointsGeneratorBuilder.cpp
)

#___________________________________________________________
#include dir
include_directories(src/Point2D)
include_directories(src/HilbertCurve)
include_directories(src/PointsGenerators)
