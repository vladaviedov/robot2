/**
 * @file tests/stats.hpp
 * @brief Functions for collecting & analyzing pictures.
 */
#pragma once

#include <string>

namespace test {
	/**
	 * @brief Run a variable sweep on a photograph.
	 *
	 * @param[in] filename - Input image filename.
	 * @param[in] outfile - Output data filename.
	 */
	void var_sweep(std::string &filename, std::string &outfile);

	/**
	 * @brief Save image from camera to file.
	 */
	void save_image();
}
