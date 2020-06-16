/*
 * lulesh-ad.cc
 *
 *  Created on: Dec 10, 2019
 *      Author: ahueck
 */
#include "lulesh-ad.h"
#include <unordered_map>

#if ADJOINT_MODE == 1
static adreal::TapeType& tape = adreal::getGlobalTape();

static std::unordered_map<ADField, std::vector<adreal::GradientData>> indep_indices;
//static std::unordered_map<ADField, std::vector<adreal::GradientData>> dep_indices;

MeDiTypes* medi_types = nullptr;

void AD_start() {
  tape.setActive();
}

void AD_end() {
  tape.setPassive();
}

void AD_reset() {
  tape.reset();
  indep_indices.clear();
}

void AD_indep(adreal &v, ADField field) {
  if (!tape.isActive()) {
    AD_start();
  }
  tape.registerInput(v);
  indep_indices[field].push_back(v.getGradientData());
}

void AD_dep(adreal &v, ADField field) {
  tape.registerOutput(v);
  //dep_indices[field].push_back(v.getGradientData());
}

void AD_MPI_start() {
#if USE_MPI
  int init;
  AMPI_Initialized(&init);
  if(init){
    medi_types = new MeDiTypes();
  }
#endif
}

void AD_MPI_end() {
#if USE_MPI
  int init;
  AMPI_Initialized(&init);
  if(init){
    delete medi_types;
    medi_types = nullptr;
  }
#endif
}

void AD_clear_adjoints() {
  tape.clearAdjoints();
}

void AD_driver(adreal &v, bool print) {
  int myRank = 0;
#if USE_MPI == 1
  AMPI_Comm_rank(AMPI_COMM_WORLD, &myRank);
#endif
  if (myRank == 0) {
    v.setGradient(1.0);
    tape.evaluate();
    if (print) {
      printf_oo("##### AD sensitivities: #####\n");
      for (auto& vpair : indep_indices) {
        printf_oo("  %d - [ ", vpair.first);
        for (auto& idx : vpair.second) {
          auto grad = tape.getGradient(idx);
          if (grad != 0.0) {
            printf_oo("%f ", grad);
          }
        }
        printf_oo("]\n");
      }
    }
  } else {
    tape.evaluate();
  }
}

void AD_print_stats() {
  std::ostringstream out;
  tape.printStatistics(out);
  printf("%s\n", out.str().c_str());
}

#else
void AD_start() {}
void AD_end() {}
void AD_reset() {}
void AD_indep(Real_t&, ADField) {}
void AD_dep(Real_t&, ADField) {}
void AD_MPI_start() {}
void AD_MPI_end() {}
void AD_clear_adjoints() {}
void AD_driver(Real_t&, bool) {}
void AD_print_stats() {}
void AD_keep(Real_t* v){}
#endif

#include <medi/medi.cpp>

