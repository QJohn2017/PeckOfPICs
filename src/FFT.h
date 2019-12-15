/*
****************************************************************************
*                                                                          *
*   FFT Header file, based on code from www.codeproject.com, originally    *
*   based on code from Numerical Recipes with refinement in speed.         *
*   Added functionality to load in complex and real arrays          	   *
*   separately. This requires a number of values which is a power of 2.    *
*   This version is also normalized so that the 1/N is taken into account. *
*   AGRT 2010                                                              *
*                                                                          *
****************************************************************************

    data_re -> float array that represent the real array of complex samples
    data_im -> float array that represent the imag array of complex samples
    NVALS -> length of real or imaginary arrays (N^2 order number)
    isign -> 1 to calculate FFT and -1 to calculate Reverse FFT

    The function returns an integer, 1 if FFT ran, 0 otherwise. It will be
    zero if NVALS is not a power of 2
*/

#ifndef FFT_H
#define FFT_H

#include <vector>
#include <cmath>

#include "GridObject.h"

// namespace FFT
// {
    enum FFT_Dir
    {
        FFT = 1,
        iFFT = -1
    };

    double sinc(const double x);

    int FFT_1D(std::vector<double>& data_re, std::vector<double>& data_im,
               FFT_Dir isign);

    int FFT_2D(GridObject& real_part, GridObject& imag_part,
               FFT_Dir transform_dir);

    std::vector<double> get_k_vec(const std::size_t size, const double dx);

    std::vector<double> get_K2_vec(const std::vector<double>& k, const double dx);

    std::vector<double> get_kappa_vec(const std::vector<double>& k, const double dx);

// }

#endif
