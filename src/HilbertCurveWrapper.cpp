#include <pybind11/pybind11.h>
#include <Point2D.h>
#include <PointsGeneratorBuilder.h>
#include <HilbertCurve2D_Adaptive.h>
#include <HilbertCurve2D_Fixed.h>
#include <HilbertCurve2D.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>


namespace py = pybind11;


PYBIND11_MODULE(pyHilbertCurve, m) {

	/*POINT*/
	py::class_<Point2D>(m, "Point2D")
		.def(py::init<double, double>())
		.def("getX", &Point2D::getX)
		.def("getY", &Point2D::getY)
		.def("dist", &Point2D::dist);

	//==================================================================================
	/*POINT GENERATOR*/
	py::class_ <PointsGenerator>(m, "PointsGenerator");

	py::class_ <UniformPointsGenerator, PointsGenerator>(m, "UniformPointsGenerator")
		.def("generatePoints", &UniformPointsGenerator::generatePoints);

	py::class_ <GammaPointsGenerator, PointsGenerator>(m, "GammaPointsGenerator")
		.def("generatePoints", &GammaPointsGenerator::generatePoints);

	py::class_ <PiecewiseConstantPointsGenerator, PointsGenerator>(m, "PiecewiseConstantPointsGenerator")
		.def("generatePoints", &PiecewiseConstantPointsGenerator::generatePoints);

	py::class_ <PointsGeneratorBuilder>(m, "PointsGeneratorBuilder")
		.def(py::init<>())
		.def("mk_uniform", &PointsGeneratorBuilder::mk_uniform)
		.def("mk_gamma", &PointsGeneratorBuilder::mk_gamma)
		.def("mk_piecewise_constant", &PointsGeneratorBuilder::mk_piecewise_constant);

	//====================================================================================
	/*HILBERTCURVE*/
	py::class_ <HilbertCurve2D>(m, "HilbertCurve2D")
		.def("coords_to_hilbertindex", &HilbertCurve2D::coords_to_hilbertindex)
		.def("hilbertindex_to_coord", &HilbertCurve2D::hilbertindex_to_coord)
		.def("coords_to_mortonindex", &HilbertCurve2D::coords_to_mortonindex)
		.def("mortonindex_to_coord", &HilbertCurve2D::mortonindex_to_coord)
		.def("mortonToHilbert", &HilbertCurve2D::mortonToHilbert)
		.def("get_points_from_hilbertindex", &HilbertCurve2D::get_points_from_hilbertindex);

	py::class_ <HilbertCurve2D_Fixed, HilbertCurve2D>(m, "HilbertCurve2D_Fixed")
		.def(py::init<std::vector<Point2D>&, uint32_t, double, double, uint32_t >())
		.def("get_mappedPoint", &HilbertCurve2D::get_mappedPoint)
		.def("get_MortonIndex", &HilbertCurve2D::get_MortonIndex)
		.def("get_HilbertIndex", &HilbertCurve2D::get_HilbertIndex)
		.def("get_n_closest", &HilbertCurve2D::get_n_closest)
		.def("get_points_in_range", &HilbertCurve2D::get_points_in_range);

	py::class_ <HilbertCurve2D_Adaptive, HilbertCurve2D>(m, "HilbertCurve2D_Adaptive")
		.def(py::init<std::vector<Point2D>&, uint32_t, double, double, uint32_t >())
		.def("get_mappedPoint", &HilbertCurve2D::get_mappedPoint)
		.def("get_MortonIndex", &HilbertCurve2D::get_MortonIndex)
		.def("get_HilbertIndex", &HilbertCurve2D::get_HilbertIndex)
		.def("get_n_closest", &HilbertCurve2D::get_n_closest)
		.def("get_points_in_range", &HilbertCurve2D::get_points_in_range);

}
