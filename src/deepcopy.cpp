#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <boost/lexical_cast.hpp>

#include "bigmemory/BigMatrix.h"
#include "bigmemory/MatrixAccessor.hpp"
#include "bigmemory/util.h"
#include "bigmemory/isna.hpp"

#include <stdio.h>
#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <stdlib.h>
#include <sys/types.h>


template<typename in_CType, typename in_BMAccessorType, 
  typename out_CType, typename out_BMAccessorType>
void DeepCopy(BigMatrix *pInMat, BigMatrix *pOutMat, SEXP rowInds, SEXP colInds)
{
  in_BMAccessorType inMat( *pInMat );
  out_BMAccessorType outMat( *pOutMat );
  
  double *pRows = NUMERIC_DATA(rowInds);
  double *pCols = NUMERIC_DATA(colInds);
  index_type nRows = GET_LENGTH(rowInds);
  index_type nCols = GET_LENGTH(colInds);
  
  if (nRows != pOutMat->nrow())
    Rf_error("length of row indices does not equal # of rows in new matrix");
  if (nCols != pOutMat->ncol())
    Rf_error("length of col indices does not equal # of cols in new matrix");
  
  index_type i = 0;
  index_type j = 0;
  in_CType *pInColumn;
  out_CType *pOutColumn;
  
  for (i = 0; i < nCols; ++i) {
    pInColumn = inMat[static_cast<index_type>(pCols[i])-1];
    pOutColumn = outMat[i];
    for (j = 0; j < nRows; ++j) {
      pOutColumn[j] = static_cast<out_CType>(
        pInColumn[static_cast<index_type>(pRows[j])-1]);
    }
  }
  
  return;
}

extern "C"
{
  #define CALL_DEEP_COPY_2(IN_CTYPE, IN_ACCESSOR, OUT_ACCESSOR) \
    switch(pOutMat->matrix_type()) \
    { \
      case 1: \
        DeepCopy<IN_CTYPE, IN_ACCESSOR<IN_CTYPE>, char, OUT_ACCESSOR<char> >( \
          pInMat, pOutMat, rowInds, colInds); \
        break; \
      case 2: \
        DeepCopy<IN_CTYPE, IN_ACCESSOR<IN_CTYPE>, short, OUT_ACCESSOR<short> >( \
          pInMat, pOutMat, rowInds, colInds); \
        break; \
      case 4: \
        DeepCopy<IN_CTYPE, IN_ACCESSOR<IN_CTYPE>, int, OUT_ACCESSOR<int> >( \
          pInMat, pOutMat, rowInds, colInds); \
        break; \
      case 8: \
        DeepCopy<IN_CTYPE, IN_ACCESSOR<IN_CTYPE>, double, OUT_ACCESSOR<double> >( \
          pInMat, pOutMat, rowInds, colInds); \
        break; \
    }

  #define CALL_DEEP_COPY_1(IN_ACCESSOR, OUT_ACCESSOR) \
    switch(pInMat->matrix_type()) \
    { \
      case 1: \
        CALL_DEEP_COPY_2(char, IN_ACCESSOR, OUT_ACCESSOR) \
        break; \
      case 2: \
        CALL_DEEP_COPY_2(short, IN_ACCESSOR, OUT_ACCESSOR) \
        break; \
      case 4: \
        CALL_DEEP_COPY_2(int, IN_ACCESSOR, OUT_ACCESSOR) \
        break; \
      case 8: \
        CALL_DEEP_COPY_2(double, IN_ACCESSOR, OUT_ACCESSOR) \
        break; \
    }
      
  SEXP CDeepCopy(SEXP inAddr, SEXP outAddr, SEXP rowInds, SEXP colInds, 
    SEXP typecast_warning)
  {
    BigMatrix *pInMat = reinterpret_cast<BigMatrix*>(
      R_ExternalPtrAddr(inAddr));
    BigMatrix *pOutMat = reinterpret_cast<BigMatrix*>(
      R_ExternalPtrAddr(outAddr));
    
    if ((pOutMat->matrix_type() < pInMat->matrix_type()) & 
      (LOGICAL_VALUE(typecast_warning) == (Rboolean)TRUE))
    {
      string type_names[9] = {
        "", "char", "short", "", "integer", "", "", "", "double"};
      
      std::string warnMsg = string("Assignment will down cast from ") + 
        type_names[pInMat->matrix_type()] + string(" to ") + 
        type_names[pOutMat->matrix_type()] + string("\n") + 
        string("Hint: To remove this warning type: ") + 
        string("options(bigmemory.typecast.warning=FALSE)");
      Rf_warning(warnMsg.c_str());
    }
    
    // Not sure if there is a better way to do these function calls
    if (pInMat->separated_columns() && pOutMat->separated_columns()) {
      CALL_DEEP_COPY_1(SepMatrixAccessor, SepMatrixAccessor)
    }
    else if(pInMat->separated_columns() && !(pOutMat->separated_columns()))
    {
      CALL_DEEP_COPY_1(SepMatrixAccessor, MatrixAccessor)
    }
    else if(!(pInMat->separated_columns()) && pOutMat->separated_columns())
    {
      CALL_DEEP_COPY_1(MatrixAccessor, SepMatrixAccessor)
    }
    else
    {
      CALL_DEEP_COPY_1(MatrixAccessor, MatrixAccessor)
    }

    return R_NilValue;
  }
}
