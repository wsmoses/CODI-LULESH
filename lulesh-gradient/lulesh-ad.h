/*
 * lulesh-ad.h
 *
 *  Created on: Dec 10, 2019
 *      Author: ahueck
 */

#ifndef LULESH_AD_H_
#define LULESH_AD_H_

#define ADJOINT_MODE 1 // TODO set at compile time

#include <mpi.h>
#include <codi.hpp>
#include <medi/medi.hpp>
#include <codi/externals/codiMediPackTypes.hpp>
#include <codi/externals/codiMpiTypes.hpp>

using namespace medi;

using adtool = CoDiPackTool<codi::RealReverse>;
using adreal = codi::RealReverse;
using MeDiTypes = CoDiMpiTypes<codi::RealReverse>;

extern MeDiTypes* medi_types;

// Precision specification
typedef float real4;
typedef double real8;
typedef long double real10;  // 10 bytes on x86

typedef int Index_t; // array subscript and loop index
#if ADJOINT_MODE == 0
typedef real8 Real_t;  // floating point representation
#else
typedef adreal Real_t ;  // floating point representation
#endif
typedef int Int_t;   // integer representation

inline adreal SQRT(adreal arg) {
  return sqrt(arg);
}
inline adreal FABS(adreal arg) {
  return abs(arg);
}
inline adreal CBRT(adreal arg) {
  return cbrt(arg);
}

enum class ADField {
  e, q, p
};

void AD_start();
void AD_end();
void AD_indep(Real_t &v, ADField field);
void AD_dep(Real_t &v, ADField field);
void AD_MPI_start();
void AD_MPI_end();
void AD_reset();
void AD_clear_adjoints();
void AD_driver(Real_t &v, bool print = false);
void AD_print_stats();

namespace detail_mpi {
template<typename T> struct ForSpecialization {
  static auto mpi_datatype() {
    return MPI_DOUBLE;
  }
};
template<> struct ForSpecialization<float> {
  static auto mpi_datatype() {
    return MPI_FLOAT;
  }
};
template<typename Tape> struct ForSpecialization<codi::ActiveReal<Tape>> {
  static auto mpi_datatype() {
    return medi_types->MPI_TYPE;
  }
};
} /* namespace detail */
template<typename T> auto ampi_datatype() {
  return detail_mpi::ForSpecialization<T>::mpi_datatype();
}

namespace detail {
template<typename T> struct ForSpecialization {
  static auto value(const T &v) {
    return v;
  }
};
template<typename Tape> struct ForSpecialization<codi::ActiveReal<Tape>> {
  static auto value(const codi::ActiveReal<Tape> &v) {
    return v.getValue();
  }
};
} /* namespace detail */
template<typename T> auto value(const T &v) {
  return detail::ForSpecialization<T>::value(v);
}
template<typename To, typename From> To recast(const From &v) {
  return static_cast<To>(value<From>(v));
}

template<typename ... Args>
void printf_oo(const char *fmt_string, Args &&... args) {
  printf(fmt_string, value(std::forward<Args>(args))...);
}
template<typename ... Args>
void fprintf_oo(FILE *stream, const char *fmt_string, Args &&... args) {
  fprintf(stream, fmt_string, value(std::forward<Args>(args))...);
}
template<typename ... Args>
void sprintf_oo(char *stream, const char *fmt_string, Args &&... args) {
  sprintf(stream, fmt_string, value(std::forward<Args>(args))...);
}

#endif /* LULESH_AD_H_ */
