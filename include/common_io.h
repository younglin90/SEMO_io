#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <functional>
#include <regex>
#include <set>
#include <cassert>

#ifdef SEMO_USE_EIGEN_LIB
#include <Eigen/Dense>
#else

#endif

namespace semo::hidden_io {

#ifdef SEMO_USE_EIGEN_LIB
	using Point3 = Eigen::Vector3d;
#else
	using Point3 = std::array<double, 3>;
#endif



}
#pragma pop_macro("max")
#pragma pop_macro("min")