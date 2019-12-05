/*
 * MeDiPack, a Message Differentiation Package
 *
 * Copyright (C) 2017-2019 Chair for Scientific Computing (SciComp), TU Kaiserslautern
 * Homepage: http://www.scicomp.uni-kl.de
 * Contact:  Prof. Nicolas R. Gauger (codi@scicomp.uni-kl.de)
 *
 * Lead developers: Max Sagebaum (SciComp, TU Kaiserslautern)
 *
 * This file is part of MeDiPack (http://www.scicomp.uni-kl.de/software/codi).
 *
 * MeDiPack is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * MeDiPack is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU
 * General Public License along with MeDiPack.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Max Sagebaum, Tim Albring (SciComp, TU Kaiserslautern)
 */
#include <medi/medi.hpp>

#include <codi.hpp>
#include <codi/externals/codiMediPackTypes.hpp>

#include <iostream>

using namespace medi;

#define TOOL CoDiPackTool<codi::RealReverse>

int main(int nargs, char** args) {
  AMPI_Init(&nargs, &args);

  TOOL::init();

  int rank;
  int size;

  AMPI_Comm_rank(AMPI_COMM_WORLD, &rank);
  AMPI_Comm_size(AMPI_COMM_WORLD, &size);

  if(size != 2) {
    std::cout << "Please start the tutorial with two processes." << std::endl;
  } else {

    codi::RealReverse::TapeType& tape = codi::RealReverse::getGlobalTape();
    tape.setActive();

    codi::RealReverse a = 3.0;
    if( 0 == rank ) {
      tape.registerInput(a);

      AMPI_Send(&a, 1, TOOL::MPI_TYPE, 1, 42, AMPI_COMM_WORLD);
    } else {
      AMPI_Recv(&a, 1, TOOL::MPI_TYPE, 0, 42, AMPI_COMM_WORLD, AMPI_STATUS_IGNORE);

      tape.registerOutput(a);

      a.setGradient(100.0);
    }

    tape.setPassive();

    tape.evaluate();

    if(0 == rank) {
      std::cout << "Adjoint of 'a' on rank 0 is: " << a.getGradient() << std::endl;
    }
  }

  TOOL::finalize();

  AMPI_Finalize();
}

#include <medi/medi.cpp>
