#include <math.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

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


template<typename T>
string ttos(T i)
{
  stringstream s;
  s.precision(16);
  s << i;
  return s.str();
}

template<>
string ttos<char>(char i)
{
  stringstream s;
  s << static_cast<short>(i);
  return s.str();
}

bool TooManyRIndices( index_type val )
{
  return double(val) > pow(2.0, 31.0)-1.0;
}

template<typename CType, typename RType, typename BMAccessorType>
void SetMatrixElements( BigMatrix *pMat, SEXP col, SEXP row, SEXP values,
  double NA_C, double C_MIN, double C_MAX, double NA_R)
{
  BMAccessorType mat( *pMat );
  double *pCols = NUMERIC_DATA(col);
  index_type numCols = GET_LENGTH(col);
  double *pRows = NUMERIC_DATA(row);
  index_type numRows = GET_LENGTH(row);
  VecPtr<RType> vec_ptr;
  RType *pVals = vec_ptr(values);
  index_type valLength = GET_LENGTH(values);
  index_type i=0;
  index_type j=0;
  index_type k=0;
  CType *pColumn;
  index_type kIndex;
  for (i=0; i < numCols; ++i)
  {
    pColumn = mat[static_cast<index_type>(pCols[i])-1];
    for (j=0; j < numRows; ++j)
    {
      kIndex = k++%valLength;
      pColumn[static_cast<index_type>(pRows[j])-1] =
        ((pVals[kIndex] < C_MIN || pVals[kIndex] > C_MAX) ?
         static_cast<CType>(NA_C) : static_cast<CType>(pVals[kIndex]));
    }
  }
}

// Function contributed by Peter Haverty at Genentech.
template<typename CType, typename RType, typename BMAccessorType>
SEXP GetIndivMatrixElements( BigMatrix *pMat, double NA_C, double NA_R,
  SEXP col, SEXP row, SEXPTYPE sxpType)
{
  VecPtr<RType> vec_ptr;
  BMAccessorType mat(*pMat);
  double *pCols = NUMERIC_DATA(col);
  double *pRows = NUMERIC_DATA(row);
  index_type numCols = GET_LENGTH(col);
  int protectCount = 0;
  SEXP retVec = PROTECT( Rf_allocVector(sxpType, numCols) );
  ++protectCount;
  RType *pRet = vec_ptr(retVec);
  CType *pColumn;
  index_type i;
  for (i=0; i < numCols; ++i)
  {
    pColumn = mat[static_cast<index_type>(pCols[i])-1];
    pRet[i] = (pColumn[static_cast<index_type>(pRows[i])-1] ==
      static_cast<CType>(NA_C)) ?
        static_cast<RType>(NA_R) :
        (static_cast<RType>(pColumn[static_cast<index_type>(pRows[i])-1]));
  }
  UNPROTECT(protectCount);
  return(retVec);
}



// Function contributed by Peter Haverty at Genentech.
template<typename CType, typename RType, typename BMAccessorType>
void SetIndivMatrixElements( BigMatrix *pMat, SEXP col, SEXP row, SEXP values,
      double NA_C, double C_MIN, double C_MAX, double NA_R)
{
  BMAccessorType mat( *pMat );
  double *pCols = NUMERIC_DATA(col);
  index_type numCols = GET_LENGTH(col);
  double *pRows = NUMERIC_DATA(row);
  VecPtr<RType> vec_ptr;
  RType *pVals = vec_ptr(values);
  index_type i=0;
  CType *pColumn;
  for (i=0; i < numCols; ++i)
  {
    pColumn = mat[static_cast<index_type>(pCols[i])-1];
    pColumn[static_cast<index_type>(pRows[i])-1] =
      ((pVals[i] < C_MIN || pVals[i] > C_MAX) ?
        static_cast<CType>(NA_C) :
        static_cast<CType>(pVals[i]));
  }
}

template<typename CType, typename RType, typename BMAccessorType>
void SetMatrixAll( BigMatrix *pMat, SEXP values,
  double NA_C, double C_MIN, double C_MAX, double NA_R)
{
  BMAccessorType mat( *pMat );
  index_type numCols = pMat->ncol();
  index_type numRows = pMat->nrow();
  VecPtr<RType> vec_ptr;
  RType *pVals = vec_ptr(values);
  index_type valLength = GET_LENGTH(values);
  index_type i=0;
  index_type j=0;
  index_type k=0;
  CType *pColumn;
  index_type kIndex;
  for (i=0; i < numCols; ++i)
  {
    pColumn = mat[i];
    for (j=0; j < numRows; ++j)
    {
      kIndex = k++%valLength;
      pColumn[j] = ((pVals[kIndex] < C_MIN || pVals[kIndex] > C_MAX) ?
                    static_cast<CType>(NA_C) :
                    static_cast<CType>(pVals[kIndex]));
    }
  }
}

template<typename CType, typename RType, typename BMAccessorType>
void SetMatrixCols( BigMatrix *pMat, SEXP col, SEXP values,
  double NA_C, double C_MIN, double C_MAX, double NA_R)
{
  BMAccessorType mat( *pMat );
  double *pCols = NUMERIC_DATA(col);
  index_type numCols = GET_LENGTH(col);
  index_type numRows = pMat->nrow();
  VecPtr<RType> vec_ptr;
  RType *pVals = vec_ptr(values);
  index_type valLength = GET_LENGTH(values);
  index_type i=0;
  index_type j=0;
  index_type k=0;
  CType *pColumn;
  index_type kIndex;
  for (i=0; i < numCols; ++i)
  {
    pColumn = mat[static_cast<index_type>(pCols[i])-1];
    for (j=0; j < numRows; ++j)
    {  
      kIndex = k++%valLength;
      pColumn[j] = ((pVals[kIndex] < C_MIN || pVals[kIndex] > C_MAX) ?
                    static_cast<CType>(NA_C) :
                    static_cast<CType>(pVals[kIndex]));
    }
  }
}

template<typename CType, typename RType, typename BMAccessorType>
void SetMatrixRows( BigMatrix *pMat, SEXP row, SEXP values,
  double NA_C, double C_MIN, double C_MAX, double NA_R)
{
  BMAccessorType mat( *pMat );
  index_type numCols = pMat->ncol();
  double *pRows = NUMERIC_DATA(row);
  index_type numRows = GET_LENGTH(row);
  VecPtr<RType> vec_ptr;
  RType *pVals = vec_ptr(values);
  index_type valLength = GET_LENGTH(values);
  index_type i=0;
  index_type j=0;
  index_type k=0;
  CType *pColumn;
  index_type kIndex;
  for (i=0; i < numCols; ++i)
  {
    pColumn = mat[i];
    for (j=0; j < numRows; ++j)
    {
      kIndex = k++%valLength;
      pColumn[static_cast<index_type>(pRows[j])-1] =
        ((pVals[kIndex] < C_MIN || pVals[kIndex] > C_MAX) ?
         static_cast<CType>(NA_C) : static_cast<CType>(pVals[kIndex]));
    }
  }
}

template<typename CType, typename BMAccessorType>
void SetAllMatrixElements( BigMatrix *pMat, SEXP value,
  double NA_C, double C_MIN, double C_MAX, double NA_R)
{
  BMAccessorType mat( *pMat );
  double val = NUMERIC_VALUE(value);
  index_type i=0;
  index_type j=0;
  index_type ncol = pMat->ncol();
  index_type nrow = pMat->nrow();

  bool outOfRange=false;
  if (val < C_MIN || val > C_MAX || isna(val))
  { 
    if (!isna(val))
    {
      outOfRange=true;
      warning("The value given is out of range, elements will be set to NA.");
    }
    val = NA_C;
  }
  for (i=0; i < ncol; ++i)
  {
    CType *pColumn = mat[i];
    for (j=0; j < nrow; ++j)
    {
      pColumn[j] = static_cast<CType>(val);
    }
  }
}

template<typename CType, typename RType, typename BMAccessorType>
SEXP GetMatrixElements( BigMatrix *pMat, double NA_C, double NA_R, 
  SEXP col, SEXP row, SEXPTYPE sxpType)
{
  VecPtr<RType> vec_ptr; 
  BMAccessorType mat(*pMat);
  double *pCols = NUMERIC_DATA(col);
  double *pRows = NUMERIC_DATA(row);
  index_type numCols = GET_LENGTH(col);
  index_type numRows = GET_LENGTH(row);
  if (TooManyRIndices(numCols*numRows))
  {
    error("Too many indices (>2^31-1) for extraction.");
    return R_NilValue;
  }
  SEXP ret = PROTECT(NEW_LIST(3));
  int protectCount = 1;
  SET_VECTOR_ELT( ret, 1, NULL_USER_OBJECT );
  SET_VECTOR_ELT( ret, 2, NULL_USER_OBJECT );
  SEXP retMat = PROTECT( Rf_allocMatrix(sxpType, numRows, numCols) );
  ++protectCount;
  SET_VECTOR_ELT(ret, 0, retMat);
  //SEXP ret = PROTECT( new_vec(numCols*numRows) );
  RType *pRet = vec_ptr(retMat);
  CType *pColumn;
  index_type k=0;
  index_type i,j;
  for (i=0; i < numCols; ++i) 
  {
    if (isna(pCols[i]))
    {
      for (j=0; j < numRows; ++j)
      {
        pRet[k] = static_cast<RType>(NA_R);
      }
    }
    else
    {
      pColumn = mat[static_cast<index_type>(pCols[i])-1];
      for (j=0; j < numRows; ++j) 
      {
        if (isna(pRows[j]))
        {
          pRet[k] = static_cast<RType>(NA_R);
        }
        else
        {
          pRet[k] = (pColumn[static_cast<index_type>(pRows[j])-1] == 
            static_cast<CType>(NA_C)) ?  static_cast<RType>(NA_R) : 
            (static_cast<RType>(pColumn[static_cast<index_type>(pRows[j])-1]));
        }
        ++k;
      }
    }
  }
  Names colNames = pMat->column_names();
  if (!colNames.empty())
  {
    ++protectCount;
    SEXP rCNames = PROTECT(allocVector(STRSXP, numCols));
    for (i=0; i < numCols; ++i)
    {
      if (!isna(pCols[i]))
        SET_STRING_ELT( rCNames, i, 
          mkChar(colNames[static_cast<index_type>(pCols[i])-1].c_str()) );
    }
    SET_VECTOR_ELT(ret, 2, rCNames);
  }
  Names rowNames = pMat->row_names();
  if (!rowNames.empty())
  {
    ++protectCount;
    SEXP rRNames = PROTECT(allocVector(STRSXP, numRows));
    for (i=0; i < numRows; ++i)
    {
      if (!isna(pRows[i]))
      {
        SET_STRING_ELT( rRNames, i, 
          mkChar(rowNames[static_cast<index_type>(pRows[i])-1].c_str()) );  
      }
    }
    SET_VECTOR_ELT(ret, 1, rRNames);
  }
  UNPROTECT(protectCount);
  return ret;
}

// Function contributed by Peter Haverty at Genentech.
SEXP GetIndivMatrixElements(SEXP bigMatAddr, SEXP col, SEXP row)
{
  BigMatrix *pMat =
    reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  if (pMat->separated_columns())
  {
    switch(pMat->matrix_type())
    {
      case 1:
        return GetIndivMatrixElements<char, int, SepMatrixAccessor<char> >(
          pMat, NA_CHAR, NA_INTEGER, col, row, INTSXP);
      case 2:
        return GetIndivMatrixElements<short,int, SepMatrixAccessor<short> >(
          pMat, NA_SHORT, NA_INTEGER, col, row, INTSXP);
      case 4:
        return GetIndivMatrixElements<int, int, SepMatrixAccessor<int> >(
          pMat, NA_INTEGER, NA_INTEGER, col, row, INTSXP);
      case 8:
        return GetIndivMatrixElements<double,double,SepMatrixAccessor<double> >(
          pMat, NA_REAL, NA_REAL, col, row, REALSXP);
    }
  }
  else
  {
    switch(pMat->matrix_type())
    {
      case 1:
        return GetIndivMatrixElements<char, int, MatrixAccessor<char> >(
          pMat, NA_CHAR, NA_INTEGER, col, row, INTSXP);
      case 2:
        return GetIndivMatrixElements<short, int, MatrixAccessor<short> >(
          pMat, NA_SHORT, NA_INTEGER, col, row, INTSXP);
      case 4:
        return GetIndivMatrixElements<int, int, MatrixAccessor<int> >(
          pMat, NA_INTEGER, NA_INTEGER, col, row, INTSXP);
      case 8:
        return GetIndivMatrixElements<double, double, MatrixAccessor<double> >(
          pMat, NA_REAL, NA_REAL, col, row, REALSXP);
    }
  }
  return R_NilValue;
}

template<typename CType, typename RType, typename BMAccessorType>
SEXP GetMatrixRows( BigMatrix *pMat, double NA_C, double NA_R, 
  SEXP row, SEXPTYPE sxpType)
{
  VecPtr<RType> vec_ptr; 
  BMAccessorType mat(*pMat);
  double *pRows=NUMERIC_DATA(row);
  index_type numRows = GET_LENGTH(row);
  index_type numCols = pMat->ncol();
  if (TooManyRIndices(numCols*numRows))
  {
    error("Too many indices (>2^31-1) for extraction.");
    return R_NilValue;
  }
  SEXP ret = PROTECT(NEW_LIST(3));
  int protectCount = 1;
  SET_VECTOR_ELT( ret, 1, NULL_USER_OBJECT );
  SET_VECTOR_ELT( ret, 2, NULL_USER_OBJECT );
  SEXP retMat = PROTECT( Rf_allocMatrix(sxpType, numRows, numCols) );
  ++protectCount;
  SET_VECTOR_ELT(ret, 0, retMat);
  //SEXP ret = PROTECT( new_vec(numCols*numRows) );
  RType *pRet = vec_ptr(retMat);
  CType *pColumn = NULL;
  index_type k=0;
  index_type i,j;
  for (i=0; i < numCols; ++i) 
  {
    pColumn = mat[i];
    for (j=0; j < numRows; ++j) 
    {
      if (isna(pRows[j]))
      {
        pRet[k] = static_cast<RType>(NA_R);
      }
      else
      {
        pRet[k] = (pColumn[static_cast<index_type>(pRows[j])-1] == 
                   static_cast<CType>(NA_C)) ?  static_cast<RType>(NA_R) : 
                   (static_cast<RType>(pColumn[static_cast<index_type>(pRows[j])-1]));
      }
      ++k;
    }
  }
  Names colNames = pMat->column_names();
  if (!colNames.empty())
  {
    ++protectCount;
    SEXP rCNames = PROTECT(allocVector(STRSXP, numCols));
    for (i=0; i < numCols; ++i)
    {
      SET_STRING_ELT( rCNames, i, mkChar(colNames[i].c_str()) );
    }
    SET_VECTOR_ELT(ret, 2, rCNames);
  }
  Names rowNames = pMat->row_names();
  if (!rowNames.empty())
  {
    ++protectCount;
    SEXP rRNames = PROTECT(allocVector(STRSXP, numRows));
    for (i=0; i < numRows; ++i)
    {
      if (!isna(pRows[i]))
      {
        SET_STRING_ELT( rRNames, i, 
          mkChar(rowNames[static_cast<index_type>(pRows[i])-1].c_str()) );  
      }
    }
    SET_VECTOR_ELT(ret, 1, rRNames);
  }
  UNPROTECT(protectCount);
  return ret;
}

template<typename CType, typename RType, typename BMAccessorType>
SEXP GetMatrixCols( BigMatrix *pMat, double NA_C, double NA_R, 
  SEXP col, SEXPTYPE sxpType)
{
  VecPtr<RType> vec_ptr; 
  BMAccessorType mat(*pMat);
  double *pCols=NUMERIC_DATA(col);
  index_type numCols = GET_LENGTH(col);
  index_type numRows = pMat->nrow();
  if (TooManyRIndices(numCols*numRows))
  {
    error("Too many indices (>2^31-1) for extraction.");
    return R_NilValue;
  }
  SEXP ret = PROTECT(NEW_LIST(3));
  int protectCount = 1;
  SET_VECTOR_ELT( ret, 1, NULL_USER_OBJECT );
  SET_VECTOR_ELT( ret, 2, NULL_USER_OBJECT );
  SEXP retMat = PROTECT( Rf_allocMatrix(sxpType, numRows, numCols) );
  ++protectCount;
  SET_VECTOR_ELT(ret, 0, retMat);
  //SEXP ret = PROTECT( new_vec(numCols*numRows) );
  RType *pRet = vec_ptr(retMat);
  CType *pColumn = NULL;
  index_type k=0;
  index_type i,j;
  for (i=0; i < numCols; ++i) 
  {
    if (isna(pCols[i]))
    {
      for (j=0; j < numRows; ++j)
      {
        pRet[k] = static_cast<RType>(NA_R);
      }
    }
    else
    {
      pColumn = mat[static_cast<index_type>(pCols[i])-1];
      for (j=0; j < numRows; ++j) 
      {
        pRet[k] = (pColumn[j] == static_cast<CType>(NA_C)) ?  static_cast<RType>(NA_R) : 
                   (static_cast<RType>(pColumn[j]));
        ++k;
      }
    }
  }
  Names colNames = pMat->column_names();
  if (!colNames.empty())
  {
    ++protectCount;
    SEXP rCNames = PROTECT(allocVector(STRSXP, numCols));
    for (i=0; i < numCols; ++i)
    {
      if (!isna(pCols[i]))
        SET_STRING_ELT( rCNames, i, 
          mkChar(colNames[static_cast<index_type>(pCols[i])-1].c_str()) );
    }
    SET_VECTOR_ELT(ret, 2, rCNames);
  }
  Names rowNames = pMat->row_names();
  if (!rowNames.empty())
  {
    ++protectCount;
    SEXP rRNames = PROTECT(allocVector(STRSXP, numRows));
    for (i=0; i < numRows; ++i)
    {
      SET_STRING_ELT( rRNames, i, mkChar(rowNames[i].c_str()) );  
    }
    SET_VECTOR_ELT(ret, 1, rRNames);
  }
  UNPROTECT(protectCount);
  return ret;
}

template<typename CType, typename RType, typename BMAccessorType>
SEXP GetMatrixAll( BigMatrix *pMat, double NA_C, double NA_R, 
  SEXPTYPE sxpType)
{
  VecPtr<RType> vec_ptr; 
  BMAccessorType mat(*pMat);
  index_type numCols = pMat->ncol();
  index_type numRows = pMat->nrow();
  if (TooManyRIndices(numCols*numRows))
  {
    error("Too many indices (>2^31-1) for extraction.");
    return R_NilValue;
  }
  SEXP ret = PROTECT(NEW_LIST(3));
  int protectCount = 1;
  SET_VECTOR_ELT( ret, 1, NULL_USER_OBJECT );
  SET_VECTOR_ELT( ret, 2, NULL_USER_OBJECT );
  SEXP retMat = PROTECT( Rf_allocMatrix(sxpType, numRows, numCols) );
  ++protectCount;
  SET_VECTOR_ELT(ret, 0, retMat);
  //SEXP ret = PROTECT( new_vec(numCols*numRows) );
  RType *pRet = vec_ptr(retMat);
  CType *pColumn = NULL;
  index_type k=0;
  index_type i,j;
  for (i=0; i < numCols; ++i) 
  {
    pColumn = mat[i];
    for (j=0; j < numRows; ++j) 
    {
      pRet[k] = (pColumn[j] == static_cast<CType>(NA_C)) ?  static_cast<RType>(NA_R) : 
                 (static_cast<RType>(pColumn[j]));
      ++k;
    }
  }
  Names colNames = pMat->column_names();
  if (!colNames.empty())
  {
    ++protectCount;
    SEXP rCNames = PROTECT(allocVector(STRSXP, numCols));
    for (i=0; i < numCols; ++i)
    {
      SET_STRING_ELT( rCNames, i, mkChar(colNames[i].c_str()) );
    }
    SET_VECTOR_ELT(ret, 2, rCNames);
  }
  Names rowNames = pMat->row_names();
  if (!rowNames.empty())
  {
    ++protectCount;
    SEXP rRNames = PROTECT(allocVector(STRSXP, numRows));
    for (i=0; i < numRows; ++i)
    {
      SET_STRING_ELT( rRNames, i, mkChar(rowNames[i].c_str()) );  
    }
    SET_VECTOR_ELT(ret, 1, rRNames);
  }
  UNPROTECT(protectCount);
  return ret;
}

template<typename T, typename BMAccessorType>
SEXP ReadMatrix(SEXP fileName, BigMatrix *pMat,
                SEXP firstLine, SEXP numLines, SEXP numCols, SEXP separator,
                SEXP hasRowNames, SEXP useRowNames, double C_NA, double posInf, 
                double negInf, double notANumber)
{
  BMAccessorType mat(*pMat);
  SEXP ret = PROTECT(NEW_LOGICAL(1));
  LOGICAL_DATA(ret)[0] = (Rboolean)0;
  index_type fl = static_cast<index_type>(NUMERIC_VALUE(firstLine));
  index_type nl = static_cast<index_type>(NUMERIC_VALUE(numLines));
  string sep(CHAR(STRING_ELT(separator,0)));
  index_type i=0,j;
  bool rowSizeReserved = false;
  //double val;

  ifstream file;
  string lc, element;
  file.open(STRING_VALUE(fileName));
  if (!file.is_open())
  {
    UNPROTECT(1);
    return ret;
  }
  for (i=0; i < fl; ++i)
  {
    std::getline(file, lc);
  }
  Names rn;
  index_type offset = static_cast<index_type>(LOGICAL_VALUE(hasRowNames));
  double d;
  int charRead;
  char *pEnd;
  for (i=0; i < nl; ++i)
  {
    // getline may be slow
    std::getline(file, lc);
    string::size_type first=0, last=0;
    j=0;
    while (first < lc.size() && last < lc.size())
    {
      last = lc.find_first_of(sep, first);
      element = lc.substr(first, last-first);
      if (LOGICAL_VALUE(hasRowNames) && 0==j)
      {
        if (LOGICAL_VALUE(useRowNames))
        {
          if (!rowSizeReserved)
          {
            rn.reserve(nl);
            rowSizeReserved = true;
          }
          std::size_t pos;
          while ( (pos = element.find("\"", 0)) != string::npos )
          {
            element = element.replace(pos, 1, "");
          }
          while ( (pos = element.find("'", 0)) != string::npos )
          {
            element = element.replace(pos, 1, "");
          }
          rn.push_back(element);
        }
      }
      else
      {
        if (j-offset < pMat->ncol()+1)
        {
          d = strtod(element.c_str(), &pEnd);
          if (pEnd != element.c_str())
          {
            mat[j-offset][i] = static_cast<T>(d);
          }
          else
          {
            charRead = sscanf(element.c_str(), "%lf", &d);
            if (charRead == static_cast<int>(element.size()))
            {
              mat[j-offset][i] = static_cast<T>(d);
            }
            else if (element == "NA")
            {
              mat[j-offset][i] = static_cast<T>(C_NA);
            }
            else if (element == "inf" || element == "Inf")
            {
              mat[j-offset][i] = static_cast<T>(posInf);
            }
            else if (element == "-inf" || element == "-Inf")
            {
              mat[j-offset][i] = static_cast<T>(negInf);
            }
            else if (element == "NaN")
            {
              mat[j-offset][i] = static_cast<T>(notANumber);
            }
            else if (element =="")
            {
              mat[j-offset][i] = static_cast<T>(C_NA);
            }
            else
            {
              mat[j-offset][i] = static_cast<T>(C_NA);
            }
          }
        }
        else
        {
          warning( 
            (string("Incorrect number of entries in row ")+ttos(j)).c_str());
        }
      }
      first = last+1;
      ++j;
    }
    if (j-offset < pMat->ncol())
    {
//      warning( (string("Incorrect number of entries in row ")+ttos(j)).c_str());
      while (j-offset < pMat->ncol())
      {
        mat[j++ - offset][i] = static_cast<T>(C_NA);
      }
    }
  }
  pMat->row_names( rn );
  file.close();
  LOGICAL_DATA(ret)[0] = (Rboolean)1;
  UNPROTECT(1);
  return ret;
}

template<typename T, typename BMAccessorType>
void WriteMatrix( BigMatrix *pMat, SEXP fileName, SEXP rowNames,
                  SEXP colNames, SEXP sep, double C_NA )
{
  BMAccessorType mat(*pMat);
  FILE *FP = fopen(STRING_VALUE(fileName), "w");
  index_type i,j;
  string  s;
  string sepString = string(CHAR(STRING_ELT(sep, 0)));

  Names cn = pMat->column_names();
  Names rn = pMat->row_names();
  if (LOGICAL_VALUE(colNames) == Rboolean(TRUE) && !cn.empty())
  {
    for (i=0; i < (int) cn.size(); ++i)
      s += "\"" + cn[i] + "\"" + (((int)cn.size()-1 == i) ? "\n" : sepString);
  }
  fprintf(FP, "%s", s.c_str());
  s.clear();
  for (i=0; i < pMat->nrow(); ++i) 
  {
    if ( LOGICAL_VALUE(rowNames) == Rboolean(TRUE) && !rn.empty())
    {
      s += "\"" + rn[i] + "\"" + sepString;
    }
    for (j=0; j < pMat->ncol(); ++j) 
    {
      if ( isna(mat[j][i]) )
      {
        s += "NA";
      }
      else
      {
        s += ttos(mat[j][i]);
      }
      if (j < pMat->ncol()-1)
      { 
        s += sepString;
      }
      else 
      {
        s += "\n";
      }
    }
    fprintf(FP, "%s", s.c_str());
    s.clear();
  }
  fclose(FP);
}

template<typename T>
struct NAMaker;

template<>
struct NAMaker<char>
{char operator()() const {return NA_CHAR;}};

template<>
struct NAMaker<short>
{short operator()() const {return NA_SHORT;}};

template<>
struct NAMaker<int>
{int operator()() const {return NA_INTEGER;}};

template<>
struct NAMaker<double>
{double operator()() const {return NA_REAL;}};
// Note: naLast should be passed as an integer.

template<typename PairType>
struct SecondLess : public std::binary_function<PairType, PairType, bool>
{
  SecondLess( const bool naLast ) : _naLast(naLast) {}

  bool operator()(const PairType &lhs, const PairType &rhs) const
  {
    if (_naLast)
    {
      if (isna(lhs.second) || isna(rhs.second)) return false;
      return lhs.second < rhs.second;
    }
    else
    {
      if (isna(lhs.second)) return true;
      if (isna(rhs.second)) return false;
      return lhs.second < rhs.second;
    }
  }
  
  bool _naLast;

};

template<typename PairType>
struct SecondGreater : public std::binary_function<PairType, PairType, bool>
{
  SecondGreater(const bool naLast ) : _naLast(naLast) {}

  bool operator()(const PairType &lhs, const PairType &rhs) const
  {
    if (_naLast)
    {
      if (isna(lhs.second) || isna(rhs.second)) return false;
      return lhs.second > rhs.second;
    }
    else
    {
      if (isna(lhs.second)) return true;
      if (isna(rhs.second)) return false;
      return lhs.second > rhs.second;
    }
  }

  bool _naLast;

};

template<typename PairType>
struct SecondIsNA : public std::unary_function<PairType, bool>
{
  bool operator()( const PairType &val ) const 
  {
    return isna(val.second);
  }
};

template<typename MatrixAccessorType>
void reorder_matrix( MatrixAccessorType m, SEXP orderVec, 
  index_type numColumns, FileBackedBigMatrix *pfbm )
{
  double *pov = NUMERIC_DATA(orderVec);
  typedef typename MatrixAccessorType::value_type ValueType;
  typedef std::vector<ValueType> Values;
  Values vs(m.nrow());
  index_type i,j;
  for (i=0; i < numColumns; ++i)
  {
    for (j=0; j < m.nrow(); ++j)
    {
      vs[j] = m[i][static_cast<index_type>(pov[j])-1];
    }
    std::copy( vs.begin(), vs.end(), m[i] );
    if (pfbm) pfbm->flush();
  }
}

template<typename RType, typename MatrixAccessorType>
SEXP get_order( MatrixAccessorType m, SEXP columns, SEXP naLast,
  SEXP decreasing )
{
  typedef typename MatrixAccessorType::value_type ValueType;
  typedef typename std::pair<double, ValueType> PairType;
  typedef std::vector<PairType> OrderVecs;
  std::size_t i;
  index_type k;
  index_type col;
  OrderVecs ov;
  ov.reserve(m.nrow());
  typename OrderVecs::iterator begin, end, it, naIt;
  ValueType val;
  for (k=GET_LENGTH(columns)-1; k >= 0; --k)
  {
    col = static_cast<index_type>(NUMERIC_DATA(columns)[k]-1);
    if (k==GET_LENGTH(columns)-1)
    {
      if (isna(INTEGER_VALUE(naLast)))
      {
        for (i=0; i < static_cast<size_t>(m.nrow()); ++i)
        {
          val = m[col][i];
          if (!isna(val)) 
          {
            ov.push_back( std::make_pair( static_cast<double>(i), val) );
          }
        }
      }
      else
      {
        ov.resize(m.nrow());
        for (i=0; i < static_cast<size_t>(m.nrow()); ++i)
        {
          val = m[col][i];
          ov[i].first = i;
          ov[i].second = val;
        }
      }
    }
    else // not the first column we've looked at
    {
      if (isna(INTEGER_VALUE(naLast)))
      {
        i=0;
        while (i < ov.size())
        {
          val = m[col][static_cast<index_type>(ov[i].first)];
          if (!isna(val)) 
          {
            ov[i++].second = val;
          }
          else
          {
            ov.erase(ov.begin()+i);
          }
        }
      }
      else
      {
        for (i=0; i < static_cast<size_t>(m.nrow()); ++i)
        {
          ov[i].second = m[col][static_cast<index_type>(ov[i].first)];
        }
      }
    }
    if (LOGICAL_VALUE(decreasing) == 0)
    {
      std::stable_sort(ov.begin(), ov.end(), 
        SecondLess<PairType>(INTEGER_VALUE(naLast)) );
    }
    else
    {
      std::stable_sort(ov.begin(), ov.end(), 
        SecondGreater<PairType>(INTEGER_VALUE(naLast)));
    }
  }

  SEXP ret = PROTECT(NEW_NUMERIC(ov.size()));
  double *pret = NUMERIC_DATA(ret);
  for (i=0, it=ov.begin(); it < ov.end(); ++it, ++i)
  {
    pret[i] = it->first+1;
  }
  UNPROTECT(1);
  return ret;
}


extern "C"
{

void ReorderRIntMatrix( SEXP matrixVector, SEXP nrow, SEXP ncol, SEXP orderVec )
{
  return reorder_matrix( 
    MatrixAccessor<int>(INTEGER_DATA(matrixVector), 
      static_cast<index_type>(INTEGER_VALUE(nrow))), orderVec,
      static_cast<index_type>(INTEGER_VALUE(ncol)), NULL );
}

void ReorderRNumericMatrix( SEXP matrixVector, SEXP nrow, SEXP ncol, 
  SEXP orderVec )
{
  return reorder_matrix( 
    MatrixAccessor<double>(NUMERIC_DATA(matrixVector), 
      static_cast<index_type>(INTEGER_VALUE(nrow))), orderVec,
      static_cast<index_type>(INTEGER_VALUE(ncol)), NULL );
}

void ReorderBigMatrix( SEXP address, SEXP orderVec )
{
  BigMatrix *pMat = reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(address));
  if (pMat->separated_columns())
  {
    switch (pMat->matrix_type())
    {
      case 1:
        return reorder_matrix( SepMatrixAccessor<char>(*pMat), orderVec,
          pMat->ncol(), dynamic_cast<FileBackedBigMatrix*>(pMat) );
      case 2:
        return reorder_matrix( SepMatrixAccessor<short>(*pMat), orderVec,
          pMat->ncol(), dynamic_cast<FileBackedBigMatrix*>(pMat) );
      case 4:
        return reorder_matrix( SepMatrixAccessor<int>(*pMat),orderVec,
          pMat->ncol(), dynamic_cast<FileBackedBigMatrix*>(pMat) );
      case 8:
        return reorder_matrix( SepMatrixAccessor<double>(*pMat),orderVec,
          pMat->ncol(), dynamic_cast<FileBackedBigMatrix*>(pMat) );
    }
  }
  else
  {
    switch (pMat->matrix_type())
    {
      case 1:
        return reorder_matrix( MatrixAccessor<char>(*pMat),orderVec,
          pMat->ncol(), dynamic_cast<FileBackedBigMatrix*>(pMat) );
      case 2:
        return reorder_matrix( MatrixAccessor<short>(*pMat),orderVec,
          pMat->ncol(), dynamic_cast<FileBackedBigMatrix*>(pMat) );
      case 4:
        return reorder_matrix( MatrixAccessor<int>(*pMat),orderVec,
          pMat->ncol(), dynamic_cast<FileBackedBigMatrix*>(pMat) );
      case 8:
        return reorder_matrix( MatrixAccessor<double>(*pMat),orderVec,
          pMat->ncol(), dynamic_cast<FileBackedBigMatrix*>(pMat) );
    }
  }
}

SEXP OrderRIntMatrix( SEXP matrixVector, SEXP nrow, SEXP columns,
  SEXP naLast, SEXP decreasing )
{
  return get_order<int>( 
    MatrixAccessor<int>(INTEGER_DATA(matrixVector), 
      static_cast<index_type>(INTEGER_VALUE(nrow))), 
    columns, naLast, decreasing );
}

SEXP OrderRNumericMatrix( SEXP matrixVector, SEXP nrow, SEXP columns,
  SEXP naLast, SEXP decreasing )
{
  return get_order<double>( 
    MatrixAccessor<double>(NUMERIC_DATA(matrixVector), 
      static_cast<index_type>(INTEGER_VALUE(nrow))), 
    columns, naLast, decreasing );
}

SEXP OrderBigMatrix(SEXP address, SEXP columns, SEXP naLast, SEXP decreasing)
{
  BigMatrix *pMat = reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(address));
  if (pMat->separated_columns())
  {
    switch (pMat->matrix_type())
    {
      case 1:
        return get_order<char>( SepMatrixAccessor<char>(*pMat), 
          columns, naLast, decreasing );
      case 2:
        return get_order<short>( SepMatrixAccessor<short>(*pMat), 
          columns, naLast, decreasing );
      case 4:
        return get_order<int>( SepMatrixAccessor<int>(*pMat),
          columns, naLast, decreasing );
      case 8:
        return get_order<double>( SepMatrixAccessor<double>(*pMat),
          columns, naLast, decreasing );
    }
  }
  else
  {
    switch (pMat->matrix_type())
    {
      case 1:
        return get_order<char>( MatrixAccessor<char>(*pMat),
          columns, naLast, decreasing );
      case 2:
        return get_order<short>( MatrixAccessor<short>(*pMat),
          columns, naLast, decreasing );
      case 4:
        return get_order<int>( MatrixAccessor<int>(*pMat),
          columns, naLast, decreasing );
      case 8:
        return get_order<double>( MatrixAccessor<double>(*pMat),
          columns, naLast, decreasing );
    }
  }
  return R_NilValue;
}

SEXP CCleanIndices(SEXP indices, SEXP rc)
{
  typedef std::vector<index_type> Indices;

  double *pIndices = NUMERIC_DATA(indices);
  index_type numIndices = GET_LENGTH(indices);
  double maxrc = NUMERIC_VALUE(rc);
  int protectCount=1;
  SEXP ret = PROTECT(NEW_LIST(2));
  index_type negIndexCount=0;
  index_type posIndexCount=0;
  index_type zeroIndexCount=0;
  Indices::size_type i,j;
  // See if the indices are within range, negative, positive, zero, or mixed.
  for (i=0; i < static_cast<Indices::size_type>(numIndices); ++i)
  {
    if (static_cast<index_type>(pIndices[i]) == 0)
    {
      ++zeroIndexCount;
    }
    if (static_cast<index_type>(pIndices[i]) < 0)
    {
      ++negIndexCount;
    }
    if (static_cast<index_type>(pIndices[i]) > 0)
    {
      ++posIndexCount;
    }
    if ( labs(static_cast<index_type>(pIndices[i])) > maxrc )
    {
      SET_VECTOR_ELT(ret, 0, NULL_USER_OBJECT);
      SET_VECTOR_ELT(ret, 1, NULL_USER_OBJECT);
      UNPROTECT(protectCount);
      return ret;
    }
  }
  
  if ( (zeroIndexCount == numIndices) && (numIndices > 0) )
  {
    protectCount += 2;
    SEXP returnCond = PROTECT(NEW_LOGICAL(1));
    LOGICAL_DATA(returnCond)[0] = (Rboolean)1;
    SEXP newIndices = PROTECT(NEW_NUMERIC(0));
    SET_VECTOR_ELT(ret, 0, returnCond);
    SET_VECTOR_ELT(ret, 1, newIndices);
    UNPROTECT(protectCount);
    return ret;
  }

  if (posIndexCount > 0 && negIndexCount > 0)
  {
    SET_VECTOR_ELT(ret, 0, NULL_USER_OBJECT);
    SET_VECTOR_ELT(ret, 1, NULL_USER_OBJECT);
    UNPROTECT(protectCount);
    return ret;
  }
  if (zeroIndexCount > 0)
  {  
    protectCount += 2;
    SEXP returnCond = PROTECT(NEW_LOGICAL(1));
    LOGICAL_DATA(returnCond)[0] = (Rboolean)1;
    SEXP newIndices = PROTECT(NEW_NUMERIC(posIndexCount));
    double *newPIndices = NUMERIC_DATA(newIndices);
    j=0;
    for (i=0; i < static_cast<Indices::size_type>(numIndices); ++i)
    {
      if (static_cast<index_type>(pIndices[i]) != 0)
      {
        newPIndices[j++] = pIndices[i]; 
      }
    }
    SET_VECTOR_ELT(ret, 0, returnCond);
    SET_VECTOR_ELT(ret, 1, newIndices);
    UNPROTECT(protectCount);
    return ret;
  }
  else if (negIndexCount > 0)
  {
    // It might be better to use a data-structure other than a vector
    // (sequential ordering).
    Indices ind;
    try
    {
      ind.reserve(static_cast<index_type>(maxrc));
    }
    catch(...)
    {
      SET_VECTOR_ELT(ret, 0, NULL_USER_OBJECT);
      SET_VECTOR_ELT(ret, 1, NULL_USER_OBJECT);
      UNPROTECT(protectCount);
      return ret;
    }
    for (i=1; i <= static_cast<Indices::size_type>(maxrc); ++i)
    {
      ind.push_back(i);
    }
    Indices::iterator it;
    for (i=0; i < static_cast<Indices::size_type>(numIndices); ++i)
    {
      it = std::lower_bound(ind.begin(), ind.end(), 
        static_cast<index_type>(-1*pIndices[i]));
      if ( it != ind.end() && *it == -1*static_cast<index_type>(pIndices[i]) )
      {
        ind.erase(it);
      }
    }
    if (TooManyRIndices(ind.size()))
    {
      SET_VECTOR_ELT(ret, 0, NULL_USER_OBJECT);
      SET_VECTOR_ELT(ret, 1, NULL_USER_OBJECT);
      UNPROTECT(protectCount);
      return ret;
    }
    protectCount +=2;
    SEXP returnCond = PROTECT(NEW_LOGICAL(1));
    LOGICAL_DATA(returnCond)[0] = (Rboolean)1;
    SEXP newIndices = PROTECT(NEW_NUMERIC(ind.size()));
    double *newPIndices = NUMERIC_DATA(newIndices);
    for (i=0; i < ind.size(); ++i)
    {
      newPIndices[i] = static_cast<double>(ind[i]);
    }
    SET_VECTOR_ELT(ret, 0, returnCond);
    SET_VECTOR_ELT(ret, 1, newIndices);
    UNPROTECT(protectCount);
    return ret;
  }
  protectCount += 1;
  SEXP returnCond = PROTECT(NEW_LOGICAL(1));
  LOGICAL_DATA(returnCond)[0] = (Rboolean)0;
  SET_VECTOR_ELT(ret, 0, returnCond);
  SET_VECTOR_ELT(ret, 1, NULL_USER_OBJECT);
  UNPROTECT(protectCount);
  return ret;
}


SEXP HasRowColNames(SEXP address)
{
  BigMatrix *pMat = (BigMatrix*)R_ExternalPtrAddr(address);
  SEXP ret = PROTECT(NEW_LOGICAL(2));
  LOGICAL_DATA(ret)[0] = 
    pMat->row_names().empty() ? Rboolean(0) : Rboolean(1);
  LOGICAL_DATA(ret)[1] = 
    pMat->column_names().empty() ? Rboolean(0) : Rboolean(1);
  UNPROTECT(1);
  return ret;
}

SEXP GetIndexRowNames(SEXP address, SEXP indices)
{
  BigMatrix *pMat = (BigMatrix*)R_ExternalPtrAddr(address);
  Names rn = pMat->row_names();
  return StringVec2RChar(rn, NUMERIC_DATA(indices), GET_LENGTH(indices));
}

SEXP GetIndexColNames(SEXP address, SEXP indices)
{
  BigMatrix *pMat = (BigMatrix*)R_ExternalPtrAddr(address);
  Names cn = pMat->column_names();
  return StringVec2RChar(cn, NUMERIC_DATA(indices), GET_LENGTH(indices));
}

SEXP GetColumnNamesBM(SEXP address)
{
  BigMatrix *pMat = (BigMatrix*)R_ExternalPtrAddr(address);
  Names cn = pMat->column_names();
  return StringVec2RChar(cn);
}

SEXP GetRowNamesBM(SEXP address)
{
  BigMatrix *pMat = (BigMatrix*)R_ExternalPtrAddr(address);
  Names rn = pMat->row_names();
  return StringVec2RChar(rn);
}

void SetColumnNames(SEXP address, SEXP columnNames)
{
  BigMatrix *pMat = (BigMatrix*) R_ExternalPtrAddr(address);
  Names cn;
  index_type i;
  for (i=0; i < GET_LENGTH(columnNames); ++i)
    cn.push_back(string(CHAR(STRING_ELT(columnNames, i))));
  pMat->column_names(cn);
}

void SetRowNames(SEXP address, SEXP rowNames)
{
  BigMatrix *pMat = (BigMatrix*) R_ExternalPtrAddr(address);
  Names rn;
  index_type i;
  for (i=0; i < GET_LENGTH(rowNames); ++i)
    rn.push_back(string(CHAR(STRING_ELT(rowNames, i))));
  pMat->row_names(rn);
}

SEXP IsReadOnly(SEXP bigMatAddr)
{
  BigMatrix *pMat = reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  SEXP ret = PROTECT(NEW_LOGICAL(1));
  LOGICAL_DATA(ret)[0] = (pMat->read_only() ? (Rboolean) 1 : (Rboolean) 0);
  UNPROTECT(1);
  return ret;
}

SEXP CIsSubMatrix(SEXP bigMatAddr)
{
  BigMatrix *pMat = reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  SEXP ret = PROTECT(NEW_LOGICAL(1));
  if ( pMat->col_offset() > 0 || 
       pMat->row_offset() > 0 ||
       pMat->nrow() < pMat->total_rows() || 
       pMat->ncol() < pMat->total_columns() ) 
  {
    LOGICAL_DATA(ret)[0] = (Rboolean) 1;
  } 
  else 
  {
    LOGICAL_DATA(ret)[0] = (Rboolean) 0;
  }
  UNPROTECT(1);
  return ret;
}

SEXP CGetNrow(SEXP bigMatAddr)
{
  BigMatrix *pMat = (BigMatrix*)R_ExternalPtrAddr(bigMatAddr);
  SEXP ret = PROTECT(NEW_NUMERIC(1));
  NUMERIC_DATA(ret)[0] = (double)pMat->nrow();
  UNPROTECT(1);
  return(ret);
}

SEXP CGetNcol(SEXP bigMatAddr)
{
  BigMatrix *pMat = (BigMatrix*)R_ExternalPtrAddr(bigMatAddr);
  SEXP ret = PROTECT(NEW_NUMERIC(1));
  NUMERIC_DATA(ret)[0] = (double)pMat->ncol();
  UNPROTECT(1);
  return(ret);
}

SEXP CGetType(SEXP bigMatAddr)
{
  BigMatrix *pMat = (BigMatrix*)R_ExternalPtrAddr(bigMatAddr);
  SEXP ret = PROTECT(NEW_INTEGER(1));
  INTEGER_DATA(ret)[0] = pMat->matrix_type();
  UNPROTECT(1);
  return(ret);
}

SEXP IsSharedMemoryBigMatrix(SEXP bigMatAddr)
{
  BigMatrix *pMat = (BigMatrix*)R_ExternalPtrAddr(bigMatAddr);
  SEXP ret = PROTECT(NEW_LOGICAL(1));
  LOGICAL_DATA(ret)[0] = 
    dynamic_cast<SharedMemoryBigMatrix*>(pMat) == NULL ? 
      static_cast<Rboolean>(0) :
      static_cast<Rboolean>(1);
  UNPROTECT(1);
  return ret;
}

SEXP IsFileBackedBigMatrix(SEXP bigMatAddr)
{
  BigMatrix *pMat = (BigMatrix*)R_ExternalPtrAddr(bigMatAddr);
  SEXP ret = PROTECT(NEW_LOGICAL(1));
  LOGICAL_DATA(ret)[0] = 
    dynamic_cast<FileBackedBigMatrix*>(pMat) == NULL ? 
      static_cast<Rboolean>(0) :
      static_cast<Rboolean>(1);
  UNPROTECT(1);
  return ret;
}

SEXP IsSeparated(SEXP bigMatAddr)
{
  BigMatrix *pMat = (BigMatrix*)R_ExternalPtrAddr(bigMatAddr);
  SEXP ret = PROTECT(NEW_LOGICAL(1));
  LOGICAL_DATA(ret)[0] = pMat->separated_columns() ? (Rboolean)1 : (Rboolean)0;
  UNPROTECT(1);
  return(ret);
}

void CDestroyBigMatrix(SEXP bigMatrixAddr)
{
  BigMatrix *pm=(BigMatrix*)(R_ExternalPtrAddr(bigMatrixAddr));
  delete pm;
  R_ClearExternalPtr(bigMatrixAddr);
}

inline bool Lcomp(double a, double b, int op) {
  return(op==0 ? a<=b : a<b);
}
inline bool Gcomp(double a, double b, int op) {
  return(op==0 ? a>=b : a>b);
}

} // close extern C, because the next function isn't an extern.

template<typename T, typename MatrixType>
SEXP MWhichMatrix( MatrixType mat, index_type nrow, SEXP selectColumn, 
  SEXP minVal, SEXP maxVal, SEXP chkMin, SEXP chkMax, SEXP opVal, double C_NA )
{
  index_type numSc = GET_LENGTH(selectColumn);
  double *sc = NUMERIC_DATA(selectColumn);
  double *min = NUMERIC_DATA(minVal);
  double *max = NUMERIC_DATA(maxVal);
  int *chkmin = INTEGER_DATA(chkMin);
  int *chkmax = INTEGER_DATA(chkMax);

  double minV, maxV;
  int ov = INTEGER_VALUE(opVal);
  index_type count = 0;
  index_type i,j;
  double val;
  for (i=0; i < nrow; ++i) {
    for (j=0; j < numSc; ++j)  {
      minV = min[j];
      maxV = max[j];
      if (isna(minV)) {
        minV = static_cast<T>(C_NA);
        maxV = static_cast<T>(C_NA);
      }
      val = (double) mat[(index_type)sc[j]-1][i];
      if (chkmin[j]==-1) { // this is an 'neq'
        if (ov==1) { 
          // OR with 'neq'
          if  ( (minV!=val) ||
                ( (isna(val) && !isna(minV)) ||
                  (!isna(val) && isna(minV)) ) ) {
            ++count;
            break;
          }
        } else {
          // AND with 'neq'   // if they are equal, then break out.
          if ( (minV==val) || (isna(val) && isna(minV)) ) break;
        }
      } else { // not a 'neq'     

        // If it's an OR operation and it's true for one, it's true for the
        // whole row.
        if ( ( (Gcomp(val, minV, chkmin[j]) && Lcomp(val, maxV, chkmax[j])) ||
               (isna(val) && isna(minV))) && ov==1 ) { 
          ++count;
          break;
        }
        // If it's an AND operation and it's false for one, it's false for
        // the whole row.
        if ( ( (Lcomp(val, minV, 1-chkmin[j]) || Gcomp(val, maxV, 1-chkmax[j])) 
             ||
               (isna(val) && !isna(minV)) || (!isna(val) && isna(minV)) ) &&
             ov == 0 ) break;
      }
    }
    // If it's an AND operation and it's true for each column, it's true
    // for the entire row.
    if (j==numSc && ov == 0) ++count;
  }

  if (count==0) return NEW_INTEGER(0);

  SEXP ret = PROTECT(NEW_NUMERIC(count));
  double *retVals = NUMERIC_DATA(ret);
  index_type k = 0;
  for (i=0; i < nrow; ++i) {
    for (j=0; j < numSc; ++j) {
      minV = min[j];
      maxV = max[j];
      if (isna(minV)) {
        minV = static_cast<T>(C_NA);
        maxV = static_cast<T>(C_NA);
      }
      val = (double) mat[(index_type)sc[j]-1][i];

      if (chkmin[j]==-1) { // this is an 'neq'
        if (ov==1) {
          // OR with 'neq'
          if  ( (minV!=val) ||
                ( (isna(val) && !isna(minV)) || 
                  (!isna(val) && isna(minV)) ) ) {
            retVals[k++] = i+1;
            break;
          }
        } else {
          // AND with 'neq'   // if they are equal, then break out.
          if ( (minV==val) || (isna(val) && isna(minV)) ) break;
        }
      } else { // not a 'neq'

        if ( ( (Gcomp(val, minV, chkmin[j]) && Lcomp(val, maxV, chkmax[j])) ||
               (isna(val) && isna(minV))) && ov==1 ) {
          retVals[k++] = i+1;
          break;
        }
        if (((Lcomp(val, minV, 1-chkmin[j]) || Gcomp(val, maxV, 1-chkmax[j])) ||
               (isna(val) && !isna(minV)) || (!isna(val) && isna(minV)) ) &&
             ov == 0 ) break;

      }
    } // end j loop
    if (j==numSc && ov == 0) retVals[k++] = i+1;
  } // end i loop
  UNPROTECT(1);
  return(ret);
}

template<typename T>
SEXP CreateRAMMatrix(SEXP row, SEXP col, SEXP colnames, SEXP rownames,
  SEXP typeLength, SEXP ini, SEXP separated)
{
  T *pMat=NULL;
  try
  {
    pMat = new T();
    if (!pMat->create( static_cast<index_type>(NUMERIC_VALUE(row)),
      static_cast<index_type>(NUMERIC_VALUE(col)),
      INTEGER_VALUE(typeLength),
      static_cast<bool>(LOGICAL_VALUE(separated))))
    {
      delete pMat;
      return NULL_USER_OBJECT;
    }
    if (colnames != NULL_USER_OBJECT)
    {
      pMat->column_names(RChar2StringVec(colnames));
    }
    if (rownames != NULL_USER_OBJECT)
    {
      pMat->row_names(RChar2StringVec(rownames));
    }
    if (GET_LENGTH(ini) != 0)
    {
      if (pMat->separated_columns())
      {
        switch (pMat->matrix_type())
        {
          case 1:
            SetAllMatrixElements<char, SepMatrixAccessor<char> >(
              pMat, ini, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_REAL);
            break;
          case 2:
            SetAllMatrixElements<short, SepMatrixAccessor<short> >(
              pMat, ini, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, NA_REAL);
            break;
          case 4:
            SetAllMatrixElements<int, SepMatrixAccessor<int> >(
              pMat, ini, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_REAL);
            break;
          case 8:
            SetAllMatrixElements<double, SepMatrixAccessor<double> >(
              pMat, ini, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
        }
      }
      else
      {
        switch (pMat->matrix_type())
        {
          case 1:
            SetAllMatrixElements<char, MatrixAccessor<char> >(
              pMat, ini, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_REAL );
            break;
          case 2:
            SetAllMatrixElements<short, MatrixAccessor<short> >(
              pMat, ini, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, NA_REAL );
            break;
          case 4:
            SetAllMatrixElements<int, MatrixAccessor<int> >(
              pMat, ini, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_REAL );
            break;
          case 8:
            SetAllMatrixElements<double, MatrixAccessor<double> >(
              pMat, ini, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
        }
      }
    }
    SEXP address = R_MakeExternalPtr( dynamic_cast<BigMatrix*>(pMat),
      R_NilValue, R_NilValue);
    R_RegisterCFinalizerEx(address, (R_CFinalizer_t) CDestroyBigMatrix, 
      (Rboolean) TRUE);
    return address;
  }
  catch(std::exception &e)
  {
    Rprintf("%s\n", e.what());
  }
  catch(...)
  {
    Rprintf("Exception caught while trying to create shared matrix.");
  }
  delete(pMat); 
  error("The shared matrix could not be created\n");
  return(R_NilValue);
}

extern "C"{

void SetRowOffsetInfo( SEXP bigMatAddr, SEXP rowOffset, SEXP numRows )
{
  BigMatrix *pMat = 
    reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  pMat->row_offset(static_cast<index_type>(NUMERIC_VALUE(rowOffset)));
  pMat->nrow(static_cast<index_type>(NUMERIC_VALUE(numRows)));
  
}

void SetColumnOffsetInfo( SEXP bigMatAddr, SEXP colOffset, SEXP numCols )
{
  BigMatrix *pMat = 
    reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  pMat->col_offset(static_cast<index_type>(NUMERIC_VALUE(colOffset)));
  pMat->ncol(static_cast<index_type>(NUMERIC_VALUE(numCols)));
}

SEXP GetRowOffset( SEXP bigMatAddr )
{
  BigMatrix *pMat = 
    reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  SEXP ret = PROTECT(NEW_NUMERIC(2));
  NUMERIC_DATA(ret)[0] = pMat->row_offset();
  NUMERIC_DATA(ret)[1] = pMat->nrow();
  UNPROTECT(1);
  return ret;
}

SEXP GetColOffset( SEXP bigMatAddr )
{
  BigMatrix *pMat = 
    reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  SEXP ret = PROTECT(NEW_NUMERIC(2));
  NUMERIC_DATA(ret)[0] = pMat->col_offset();
  NUMERIC_DATA(ret)[1] = pMat->ncol();
  UNPROTECT(1);
  return ret;
}

SEXP GetTotalColumns( SEXP bigMatAddr )
{
  BigMatrix *pMat = 
    reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  SEXP ret = PROTECT(NEW_NUMERIC(1));
  NUMERIC_DATA(ret)[0] = pMat->total_columns();
  UNPROTECT(1);
  return ret;
}

SEXP GetTotalRows( SEXP bigMatAddr )
{
  BigMatrix *pMat = 
    reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  SEXP ret = PROTECT(NEW_NUMERIC(1));
  NUMERIC_DATA(ret)[0] = pMat->total_rows();
  UNPROTECT(1);
  return ret;
}

SEXP GetTypeString( SEXP bigMatAddr )
{
  BigMatrix *pMat = 
    reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  SEXP ret = PROTECT(allocVector(STRSXP, 1));
  switch (pMat->matrix_type())
  {
    case 1:
      SET_STRING_ELT(ret, 0, mkChar("char"));
      break;
    case 2:
      SET_STRING_ELT(ret, 0, mkChar("short"));
      break;
    case 4:
      SET_STRING_ELT(ret, 0, mkChar("integer"));
      break;
    case 8:
      SET_STRING_ELT(ret, 0, mkChar("double"));
  }
  UNPROTECT(1);
  return ret;
}

SEXP MWhichBigMatrix( SEXP bigMatAddr, SEXP selectColumn, SEXP minVal,
                     SEXP maxVal, SEXP chkMin, SEXP chkMax, SEXP opVal )
{
  BigMatrix *pMat = 
    reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  if (pMat->separated_columns())
  {
    switch (pMat->matrix_type())
    {
      case 1:
        return MWhichMatrix<char>( SepMatrixAccessor<char>(*pMat),
          pMat->nrow(), selectColumn, minVal, maxVal, chkMin, chkMax, 
          opVal, NA_CHAR);
      case 2:
        return MWhichMatrix<short>( SepMatrixAccessor<short>(*pMat),
          pMat->nrow(), selectColumn, minVal, maxVal, chkMin, chkMax, 
          opVal, NA_SHORT);
      case 4:
        return MWhichMatrix<int>( SepMatrixAccessor<int>(*pMat),
          pMat->nrow(), selectColumn, minVal, maxVal, chkMin, chkMax, 
          opVal, NA_INTEGER);
      case 8:
        return MWhichMatrix<double>( SepMatrixAccessor<double>(*pMat),
          pMat->nrow(), selectColumn, minVal, maxVal, chkMin, chkMax, 
          opVal, NA_REAL);
    }
  }
  else
  {
    switch (pMat->matrix_type())
    {
      case 1:
        return MWhichMatrix<char>( MatrixAccessor<char>(*pMat),
          pMat->nrow(), selectColumn, minVal, maxVal, chkMin, chkMax, 
          opVal, NA_CHAR);
      case 2:
        return MWhichMatrix<short>( MatrixAccessor<short>(*pMat),
          pMat->nrow(), selectColumn, minVal, maxVal, chkMin, chkMax, 
          opVal, NA_SHORT);
      case 4:
        return MWhichMatrix<int>( MatrixAccessor<int>(*pMat),
          pMat->nrow(), selectColumn, minVal, maxVal, chkMin, chkMax, 
          opVal, NA_INTEGER);
      case 8:
        return MWhichMatrix<double>( MatrixAccessor<double>(*pMat),
          pMat->nrow(), selectColumn, minVal, maxVal, chkMin, chkMax, 
          opVal, NA_REAL);
    }
  }
  return R_NilValue;
}

SEXP MWhichRIntMatrix( SEXP matrixVector, SEXP nrow, SEXP selectColumn,
  SEXP minVal, SEXP maxVal, SEXP chkMin, SEXP chkMax, SEXP opVal )
{
  index_type numRows = static_cast<index_type>(INTEGER_VALUE(nrow));
  MatrixAccessor<int> mat(INTEGER_DATA(matrixVector), numRows);
  return MWhichMatrix<int, MatrixAccessor<int> >(mat, numRows, 
    selectColumn, minVal, maxVal, chkMin, chkMax, opVal, NA_INTEGER);
}

SEXP MWhichRNumericMatrix( SEXP matrixVector, SEXP nrow, SEXP selectColumn,
  SEXP minVal, SEXP maxVal, SEXP chkMin, SEXP chkMax, SEXP opVal )
{
  index_type numRows = static_cast<index_type>(INTEGER_VALUE(nrow));
  MatrixAccessor<double> mat(NUMERIC_DATA(matrixVector), numRows);
  return MWhichMatrix<double, MatrixAccessor<double> >(mat, numRows,
    selectColumn, minVal, maxVal, chkMin, chkMax, opVal, NA_REAL);
}

SEXP CCountLines(SEXP fileName)
{ 
  FILE *FP;
  double lineCount = 0;
  char readChar;
  FP = fopen(STRING_VALUE(fileName), "r");
  SEXP ret = PROTECT(NEW_NUMERIC(1));
  NUMERIC_DATA(ret)[0] = -1;                   
  if (FP == NULL) return(ret);
  do {
    readChar = fgetc(FP);
    if ('\n' == readChar) ++lineCount;
  } while( readChar != EOF );
  fclose(FP);
  NUMERIC_DATA(ret)[0] = lineCount; 
  UNPROTECT(1);                  
  return(ret);
}

SEXP ReadMatrix(SEXP fileName, SEXP bigMatAddr,
                SEXP firstLine, SEXP numLines, SEXP numCols, SEXP separator,
                SEXP hasRowNames, SEXP useRowNames)
{
  BigMatrix *pMat = reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  if (pMat->separated_columns())
  {
    switch (pMat->matrix_type())
    {
      case 1:
        return ReadMatrix<char, SepMatrixAccessor<char> >(
          fileName, pMat, firstLine, numLines, numCols, 
          separator, hasRowNames, useRowNames, NA_CHAR, NA_CHAR, NA_CHAR, 
          NA_CHAR);
      case 2:
        return ReadMatrix<short, SepMatrixAccessor<short> >(
          fileName, pMat, firstLine, numLines, numCols, 
          separator, hasRowNames, useRowNames, NA_SHORT, NA_SHORT, NA_SHORT, 
          NA_SHORT);
      case 4:
        return ReadMatrix<int, SepMatrixAccessor<int> >(
          fileName, pMat, firstLine, numLines, numCols, 
          separator, hasRowNames, useRowNames, NA_INTEGER, NA_INTEGER, 
          NA_INTEGER, NA_INTEGER);
      case 8:
        return ReadMatrix<double, SepMatrixAccessor<double> >(
          fileName, pMat, firstLine, numLines, numCols, 
          separator, hasRowNames, useRowNames, NA_REAL, R_PosInf, R_NegInf, 
          R_NaN);
    }
  }
  else
  {
    switch (pMat->matrix_type())
    {
      case 1:
        return ReadMatrix<char, MatrixAccessor<char> >(
          fileName, pMat, firstLine, numLines, numCols, 
          separator, hasRowNames, useRowNames, NA_CHAR, NA_CHAR, NA_CHAR, 
          NA_CHAR);
      case 2:
        return ReadMatrix<short, MatrixAccessor<short> >(
          fileName, pMat, firstLine, numLines, numCols, 
          separator, hasRowNames, useRowNames, NA_SHORT, NA_SHORT, NA_SHORT, 
          NA_SHORT);
      case 4:
        return ReadMatrix<int, MatrixAccessor<int> >(
          fileName, pMat, firstLine, numLines, numCols, 
          separator, hasRowNames, useRowNames, NA_INTEGER, NA_INTEGER, 
          NA_INTEGER, NA_INTEGER);
      case 8:
        return ReadMatrix<double, MatrixAccessor<double> >(
          fileName, pMat, firstLine, numLines, numCols, 
          separator, hasRowNames, useRowNames, NA_REAL, R_PosInf, R_NegInf, 
          R_NaN);
    }
  }
  return R_NilValue;
}

void WriteMatrix( SEXP bigMatAddr, SEXP fileName, SEXP rowNames,
  SEXP colNames, SEXP sep )
{
  BigMatrix *pMat = reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  if (pMat->separated_columns())
  {
    switch (pMat->matrix_type())
    {
      case 1:
        WriteMatrix<char, SepMatrixAccessor<char> >(
          pMat, fileName, rowNames, colNames, sep, NA_CHAR);
        break;
      case 2:
        WriteMatrix<short, SepMatrixAccessor<short> >(
          pMat, fileName, rowNames, colNames, sep, NA_SHORT);
        break;
      case 4:
        WriteMatrix<int, SepMatrixAccessor<int> >(
          pMat, fileName, rowNames, colNames, sep, NA_INTEGER);
        break;
      case 8:
        WriteMatrix<double, SepMatrixAccessor<double> >(
          pMat, fileName, rowNames, colNames, sep, NA_REAL);
    }
  }
  else
  {
    switch (pMat->matrix_type())
    {
      case 1:
        WriteMatrix<char, MatrixAccessor<char> >(
          pMat, fileName, rowNames, colNames, sep, NA_CHAR);
        break;
      case 2:
        WriteMatrix<short, MatrixAccessor<short> >(
          pMat, fileName, rowNames, colNames, sep, NA_SHORT);
        break;
      case 4:
        WriteMatrix<int, MatrixAccessor<int> >(
          pMat, fileName, rowNames, colNames, sep, NA_INTEGER);
        break;
      case 8:
        WriteMatrix<double, MatrixAccessor<double> >(
          pMat, fileName, rowNames, colNames, sep, NA_REAL);
    }
  }
}

SEXP GetMatrixElements(SEXP bigMatAddr, SEXP col, SEXP row)
{
  BigMatrix *pMat = 
    reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  if (pMat->separated_columns())
  {
    switch(pMat->matrix_type())
    {
      case 1:
        return GetMatrixElements<char, int, SepMatrixAccessor<char> >
          (pMat, NA_CHAR, NA_INTEGER, col, row, INTSXP);
      case 2:
        return GetMatrixElements<short,int, SepMatrixAccessor<short> >
          (pMat, NA_SHORT, NA_INTEGER, col, row, INTSXP);
      case 4:
        return GetMatrixElements<int, int, SepMatrixAccessor<int> >
          (pMat, NA_INTEGER, NA_INTEGER, col, row, INTSXP);
      case 8:
        return GetMatrixElements<double, double, SepMatrixAccessor<double> >(
          pMat, NA_REAL, NA_REAL, col, row, REALSXP);
    }
  }
  else
  {
    switch(pMat->matrix_type())
    {
      case 1:
        return GetMatrixElements<char, int, MatrixAccessor<char> >(
          pMat, NA_CHAR, NA_INTEGER, col, row, INTSXP);
      case 2:
        return GetMatrixElements<short, int, MatrixAccessor<short> >(
          pMat, NA_SHORT, NA_INTEGER, col, row, INTSXP);
      case 4:
        return GetMatrixElements<int, int, MatrixAccessor<int> >(
          pMat, NA_INTEGER, NA_INTEGER, col, row, INTSXP);
      case 8:
        return GetMatrixElements<double, double, MatrixAccessor<double> >
          (pMat, NA_REAL, NA_REAL, col, row, REALSXP);
    }
  }
  return R_NilValue;
}

SEXP GetMatrixRows(SEXP bigMatAddr, SEXP row)
{
  BigMatrix *pMat = 
    reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  if (pMat->separated_columns())
  {
    switch(pMat->matrix_type())
    {
      case 1:
        return GetMatrixRows<char, int, SepMatrixAccessor<char> >
          (pMat, NA_CHAR, NA_INTEGER, row, INTSXP);
      case 2:
        return GetMatrixRows<short,int, SepMatrixAccessor<short> >
          (pMat, NA_SHORT, NA_INTEGER, row, INTSXP);
      case 4:
        return GetMatrixRows<int, int, SepMatrixAccessor<int> >
          (pMat, NA_INTEGER, NA_INTEGER, row, INTSXP);
      case 8:
        return GetMatrixRows<double, double, SepMatrixAccessor<double> >(
          pMat, NA_REAL, NA_REAL, row, REALSXP);
    }
  }
  else
  {
    switch(pMat->matrix_type())
    {
      case 1:
        return GetMatrixRows<char, int, MatrixAccessor<char> >(
          pMat, NA_CHAR, NA_INTEGER, row, INTSXP);
      case 2:
        return GetMatrixRows<short, int, MatrixAccessor<short> >(
          pMat, NA_SHORT, NA_INTEGER, row, INTSXP);
      case 4:
        return GetMatrixRows<int, int, MatrixAccessor<int> >(
          pMat, NA_INTEGER, NA_INTEGER, row, INTSXP);
      case 8:
        return GetMatrixRows<double, double, MatrixAccessor<double> >
          (pMat, NA_REAL, NA_REAL, row, REALSXP);
    }
  }
  return R_NilValue;
}

SEXP GetMatrixCols(SEXP bigMatAddr, SEXP col)
{
  BigMatrix *pMat = 
    reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  if (pMat->separated_columns())
  {
    switch(pMat->matrix_type())
    {
      case 1:
        return GetMatrixCols<char, int, SepMatrixAccessor<char> >
          (pMat, NA_CHAR, NA_INTEGER, col, INTSXP);
      case 2:
        return GetMatrixCols<short,int, SepMatrixAccessor<short> >
          (pMat, NA_SHORT, NA_INTEGER, col, INTSXP);
      case 4:
        return GetMatrixCols<int, int, SepMatrixAccessor<int> >
          (pMat, NA_INTEGER, NA_INTEGER, col, INTSXP);
      case 8:
        return GetMatrixCols<double, double, SepMatrixAccessor<double> >(
          pMat, NA_REAL, NA_REAL, col, REALSXP);
    }
  }
  else
  {
    switch(pMat->matrix_type())
    {
      case 1:
        return GetMatrixCols<char, int, MatrixAccessor<char> >(
          pMat, NA_CHAR, NA_INTEGER, col, INTSXP);
      case 2:
        return GetMatrixCols<short, int, MatrixAccessor<short> >(
          pMat, NA_SHORT, NA_INTEGER, col, INTSXP);
      case 4:
        return GetMatrixCols<int, int, MatrixAccessor<int> >(
          pMat, NA_INTEGER, NA_INTEGER, col, INTSXP);
      case 8:
        return GetMatrixCols<double, double, MatrixAccessor<double> >
          (pMat, NA_REAL, NA_REAL, col, REALSXP);
    }
  }
  return R_NilValue;
}

SEXP GetMatrixAll(SEXP bigMatAddr, SEXP col)
{
  BigMatrix *pMat = 
    reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  if (pMat->separated_columns())
  {
    switch(pMat->matrix_type())
    {
      case 1:
        return GetMatrixAll<char, int, SepMatrixAccessor<char> >
          (pMat, NA_CHAR, NA_INTEGER, INTSXP);
      case 2:
        return GetMatrixAll<short,int, SepMatrixAccessor<short> >
          (pMat, NA_SHORT, NA_INTEGER, INTSXP);
      case 4:
        return GetMatrixAll<int, int, SepMatrixAccessor<int> >
          (pMat, NA_INTEGER, NA_INTEGER, INTSXP);
      case 8:
        return GetMatrixAll<double, double, SepMatrixAccessor<double> >(
          pMat, NA_REAL, NA_REAL, REALSXP);
    }
  }
  else
  {
    switch(pMat->matrix_type())
    {
      case 1:
        return GetMatrixAll<char, int, MatrixAccessor<char> >(
          pMat, NA_CHAR, NA_INTEGER, INTSXP);
      case 2:
        return GetMatrixAll<short, int, MatrixAccessor<short> >(
          pMat, NA_SHORT, NA_INTEGER, INTSXP);
      case 4:
        return GetMatrixAll<int, int, MatrixAccessor<int> >(
          pMat, NA_INTEGER, NA_INTEGER, INTSXP);
      case 8:
        return GetMatrixAll<double, double, MatrixAccessor<double> >
          (pMat, NA_REAL, NA_REAL, REALSXP);
    }
  }
  return R_NilValue;
}

void SetMatrixElements(SEXP bigMatAddr, SEXP col, SEXP row, SEXP values)
{
  BigMatrix *pMat = reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  if (pMat->separated_columns())
  {
    switch (pMat->matrix_type())
    {
      case 1:
        SetMatrixElements<char, int, SepMatrixAccessor<char> >( 
          pMat, col, row, values, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_INTEGER);
        break;
      case 2:
        SetMatrixElements<short, int, SepMatrixAccessor<short> >( 
          pMat, col, row, values, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, 
          NA_INTEGER);
        break;
      case 4:
        SetMatrixElements<int, int, SepMatrixAccessor<int> >( 
          pMat, col, row, values, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_INTEGER);
        break;
      case 8:
        SetMatrixElements<double, double, SepMatrixAccessor<double> >( 
          pMat, col, row, values, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
    }
  }
  else
  {
    switch (pMat->matrix_type())
    {
      case 1:
        SetMatrixElements<char, int, MatrixAccessor<char> >( 
          pMat, col, row, values, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_INTEGER);
        break;
      case 2:
        SetMatrixElements<short, int, MatrixAccessor<short> >( 
          pMat, col, row, values, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, 
          NA_INTEGER);
        break;
      case 4:
        SetMatrixElements<int, int, MatrixAccessor<int> >( 
          pMat, col, row, values, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_INTEGER);
        break;
      case 8:
        SetMatrixElements<double, double, MatrixAccessor<double> >( 
          pMat, col, row, values, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
    }
  }
}

// Function contributed by Peter Haverty at Genentech.
void SetIndivMatrixElements(SEXP bigMatAddr, SEXP col, SEXP row, SEXP values)
{
  BigMatrix *pMat = reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  if (pMat->separated_columns())
  {
    switch (pMat->matrix_type())
    {
    case 1:
      SetIndivMatrixElements<char, int, SepMatrixAccessor<char> >(
        pMat, col, row, values, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_INTEGER);
      break;
    case 2:
      SetIndivMatrixElements<short, int, SepMatrixAccessor<short> >(
        pMat, col, row, values, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, NA_INTEGER);
      break;
    case 4:
      SetIndivMatrixElements<int, int, SepMatrixAccessor<int> >(
        pMat, col, row, values, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_INTEGER);
      break;
    case 8:
      SetIndivMatrixElements<double, double, SepMatrixAccessor<double> >(
        pMat, col, row, values, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
    }
  }
  else
  {
    switch (pMat->matrix_type())
    {
    case 1:
      SetIndivMatrixElements<char, int, MatrixAccessor<char> >(
        pMat, col, row, values, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_INTEGER);
      break;
    case 2:
      SetIndivMatrixElements<short, int, MatrixAccessor<short> >(
        pMat, col, row, values, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, NA_INTEGER);
      break;
    case 4:
      SetIndivMatrixElements<int, int, MatrixAccessor<int> >(
        pMat, col, row, values, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_INTEGER);
      break;
    case 8:
      SetIndivMatrixElements<double, double, MatrixAccessor<double> >(
        pMat, col, row, values, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
    }
  }
}


void SetMatrixAll(SEXP bigMatAddr, SEXP values)
{
  BigMatrix *pMat = reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  if (pMat->separated_columns())
  {
    switch (pMat->matrix_type())
    {
      case 1:
        SetMatrixAll<char, int, SepMatrixAccessor<char> >( 
          pMat, values, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_INTEGER);
        break;
      case 2:
        SetMatrixAll<short, int, SepMatrixAccessor<short> >( 
          pMat, values, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, 
          NA_INTEGER);
        break;
      case 4:
        SetMatrixAll<int, int, SepMatrixAccessor<int> >( 
          pMat, values, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_INTEGER);
        break;
      case 8:
        SetMatrixAll<double, double, SepMatrixAccessor<double> >( 
          pMat, values, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
    }
  }
  else
  {
    switch (pMat->matrix_type())
    {
      case 1:
        SetMatrixAll<char, int, MatrixAccessor<char> >( 
          pMat, values, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_INTEGER);
        break;
      case 2:
        SetMatrixAll<short, int, MatrixAccessor<short> >( 
          pMat, values, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, 
          NA_INTEGER);
        break;
      case 4:
        SetMatrixAll<int, int, MatrixAccessor<int> >( 
          pMat, values, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_INTEGER);
        break;
      case 8:
        SetMatrixAll<double, double, MatrixAccessor<double> >( 
          pMat, values, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
    }
  }
}

void SetMatrixCols(SEXP bigMatAddr, SEXP col, SEXP values)
{
  BigMatrix *pMat = reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  if (pMat->separated_columns())
  {
    switch (pMat->matrix_type())
    {
      case 1:
        SetMatrixCols<char, int, SepMatrixAccessor<char> >( 
          pMat, col, values, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_INTEGER);
        break;
      case 2:
        SetMatrixCols<short, int, SepMatrixAccessor<short> >( 
          pMat, col, values, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, 
          NA_INTEGER);
        break;
      case 4:
        SetMatrixCols<int, int, SepMatrixAccessor<int> >( 
          pMat, col, values, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_INTEGER);
        break;
      case 8:
        SetMatrixCols<double, double, SepMatrixAccessor<double> >( 
          pMat, col, values, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
    }
  }
  else
  {
    switch (pMat->matrix_type())
    {
      case 1:
        SetMatrixCols<char, int, MatrixAccessor<char> >( 
          pMat, col, values, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_INTEGER);
        break;
      case 2:
        SetMatrixCols<short, int, MatrixAccessor<short> >( 
          pMat, col, values, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, 
          NA_INTEGER);
        break;
      case 4:
        SetMatrixCols<int, int, MatrixAccessor<int> >( 
          pMat, col, values, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_INTEGER);
        break;
      case 8:
        SetMatrixCols<double, double, MatrixAccessor<double> >( 
          pMat, col, values, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
    }
  }
}

void SetMatrixRows(SEXP bigMatAddr, SEXP row, SEXP values)
{
  BigMatrix *pMat = reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  if (pMat->separated_columns())
  {
    switch (pMat->matrix_type())
    {
      case 1:
        SetMatrixRows<char, int, SepMatrixAccessor<char> >( 
          pMat, row, values, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_INTEGER);
        break;
      case 2:
        SetMatrixRows<short, int, SepMatrixAccessor<short> >( 
          pMat, row, values, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, 
          NA_INTEGER);
        break;
      case 4:
        SetMatrixRows<int, int, SepMatrixAccessor<int> >( 
          pMat, row, values, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_INTEGER);
        break;
      case 8:
        SetMatrixRows<double, double, SepMatrixAccessor<double> >( 
          pMat, row, values, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
    }
  }
  else
  {
    switch (pMat->matrix_type())
    {
      case 1:
        SetMatrixRows<char, int, MatrixAccessor<char> >( 
          pMat, row, values, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_INTEGER);
        break;
      case 2:
        SetMatrixRows<short, int, MatrixAccessor<short> >( 
          pMat, row, values, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, 
          NA_INTEGER);
        break;
      case 4:
        SetMatrixRows<int, int, MatrixAccessor<int> >( 
          pMat, row, values, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_INTEGER);
        break;
      case 8:
        SetMatrixRows<double, double, MatrixAccessor<double> >( 
          pMat, row, values, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
    }
  }
}

// WHERE IS THIS CALLED FROM?  Maybe only from C, not from R?  
// We might like to be able to do this recycling efficiently in other
// cases?  I thought we did.
void SetAllMatrixElements(SEXP bigMatAddr, SEXP value)
{
  BigMatrix *pMat = reinterpret_cast<BigMatrix*>(R_ExternalPtrAddr(bigMatAddr));
  if (pMat->separated_columns())
  {
    switch (pMat->matrix_type())
    {
      case 1:
        SetAllMatrixElements<char, SepMatrixAccessor<char> >( 
          pMat, value, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_REAL);
        break;
      case 2:
        SetAllMatrixElements<short, SepMatrixAccessor<short> >( 
          pMat, value, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, NA_REAL);
        break;
      case 4:
        SetAllMatrixElements<int, SepMatrixAccessor<int> >( 
          pMat, value, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_REAL);
        break;
      case 8:
        SetAllMatrixElements<double, SepMatrixAccessor<double> >( 
          pMat, value, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
    }
  }
  else
  {
    switch (pMat->matrix_type())
    {
      case 1:
        SetAllMatrixElements<char, MatrixAccessor<char> >( 
          pMat, value, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_REAL);
        break;
      case 2:
        SetAllMatrixElements<short, MatrixAccessor<short> >( 
          pMat, value, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, NA_REAL);
        break;
      case 4:
        SetAllMatrixElements<int, MatrixAccessor<int> >( 
          pMat, value, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_REAL);
        break;
      case 8:
        SetAllMatrixElements<double, MatrixAccessor<double> >( 
          pMat, value, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
    }
  }
}

SEXP CreateSharedMatrix(SEXP row, SEXP col, SEXP colnames, SEXP rownames,
  SEXP typeLength, SEXP ini, SEXP separated)
{
  return CreateRAMMatrix<SharedMemoryBigMatrix>(row, col, colnames,
    rownames, typeLength, ini, separated);
}

SEXP CreateLocalMatrix(SEXP row, SEXP col, SEXP colnames, SEXP rownames,
  SEXP typeLength, SEXP ini, SEXP separated)
{
  return CreateRAMMatrix<LocalBigMatrix>(row, col, colnames,
    rownames, typeLength, ini, separated);
}

void* GetDataPtr(SEXP address)
{
  SharedBigMatrix *pMat = 
    reinterpret_cast<SharedBigMatrix*>(R_ExternalPtrAddr(address));
  return pMat->data_ptr();
}

SEXP CreateFileBackedBigMatrix(SEXP fileName, SEXP filePath, SEXP row, 
  SEXP col, SEXP colnames, SEXP rownames, SEXP typeLength, SEXP ini, 
  SEXP separated)
{
  try
  {
    FileBackedBigMatrix *pMat = new FileBackedBigMatrix();
    string fn;
    string path = ((filePath == NULL_USER_OBJECT) ? 
      "" : 
      RChar2String(filePath));
    if (isNull(fileName))
    {
      fn=pMat->uuid()+".bin";
    }
    else
    {
      fn = RChar2String(fileName);
    }
    if (!pMat->create( fn, RChar2String(filePath),
      static_cast<index_type>(NUMERIC_VALUE(row)),
      static_cast<index_type>(NUMERIC_VALUE(col)),
      INTEGER_VALUE(typeLength),
      static_cast<bool>(LOGICAL_VALUE(separated))))
    {
      delete pMat;
      error("Problem creating filebacked matrix.");
      return NULL_USER_OBJECT;
    }
    if (colnames != NULL_USER_OBJECT)
    {
      pMat->column_names(RChar2StringVec(colnames));
    }
    if (rownames != NULL_USER_OBJECT)
    {
      pMat->row_names(RChar2StringVec(rownames));
    }
    if (GET_LENGTH(ini) != 0)
    {
      if (pMat->separated_columns())
      {
        switch (pMat->matrix_type())
        {
          case 1:
            SetAllMatrixElements<char, SepMatrixAccessor<char> >(
              pMat, ini, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_REAL);
            break;
          case 2:
            SetAllMatrixElements<short, SepMatrixAccessor<short> >(
              pMat, ini, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, NA_REAL);
            break;
          case 4:
            SetAllMatrixElements<int, SepMatrixAccessor<int> >(
              pMat, ini, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_REAL);
            break;
          case 8:
            SetAllMatrixElements<double, SepMatrixAccessor<double> >(
              pMat, ini, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
        }
      }
      else
      {
        switch (pMat->matrix_type())
        {
          case 1:
            SetAllMatrixElements<char, MatrixAccessor<char> >(
              pMat, ini, NA_CHAR, R_CHAR_MIN, R_CHAR_MAX, NA_REAL);
            break;
          case 2:
            SetAllMatrixElements<short, MatrixAccessor<short> >(
              pMat, ini, NA_SHORT, R_SHORT_MIN, R_SHORT_MAX, NA_REAL);
            break;
          case 4:
            SetAllMatrixElements<int, MatrixAccessor<int> >(
              pMat, ini, NA_INTEGER, R_INT_MIN, R_INT_MAX, NA_REAL);
            break;
          case 8:
            SetAllMatrixElements<double, MatrixAccessor<double> >(
              pMat, ini, NA_REAL, R_DOUBLE_MIN, R_DOUBLE_MAX, NA_REAL);
        }
      }
    }
    SEXP address = R_MakeExternalPtr( dynamic_cast<BigMatrix*>(pMat),
      R_NilValue, R_NilValue);
    R_RegisterCFinalizerEx(address, (R_CFinalizer_t) CDestroyBigMatrix, 
        (Rboolean) TRUE);
    return address;
  }
  catch(std::exception &e)
  {
    Rprintf("%s\n", e.what());
  }
  catch(...)
  {
    Rprintf("Unspecified problem trying to create big.matrix\n");
  }
  return R_NilValue;
}

SEXP CAttachSharedBigMatrix(SEXP sharedName, SEXP rows, SEXP cols, 
  SEXP rowNames, SEXP colNames, SEXP typeLength, SEXP separated,
  SEXP readOnly)
{
  SharedMemoryBigMatrix *pMat = new SharedMemoryBigMatrix();
  bool connected = pMat->connect( 
    string(CHAR(STRING_ELT(sharedName,0))),
    static_cast<index_type>(NUMERIC_VALUE(rows)),
    static_cast<index_type>(NUMERIC_VALUE(cols)),
    INTEGER_VALUE(typeLength),
    static_cast<bool>(LOGICAL_VALUE(separated)),
    static_cast<bool>(LOGICAL_VALUE(readOnly)));
  if (!connected)
  {
    delete pMat;
    return NULL_USER_OBJECT;
  }
  if (GET_LENGTH(colNames) > 0)
  {
    pMat->column_names(RChar2StringVec(colNames));
  }
  if (GET_LENGTH(rowNames) > 0)
  {
    pMat->row_names(RChar2StringVec(rowNames));
  }
  SEXP address = R_MakeExternalPtr( dynamic_cast<BigMatrix*>(pMat),
    R_NilValue, R_NilValue);
  R_RegisterCFinalizerEx(address, (R_CFinalizer_t) CDestroyBigMatrix, 
      (Rboolean) TRUE);
  return address;
}

SEXP CAttachFileBackedBigMatrix(SEXP fileName, 
  SEXP filePath, SEXP rows, SEXP cols, SEXP rowNames, SEXP colNames, 
  SEXP typeLength, SEXP separated, SEXP readOnly)
{
  FileBackedBigMatrix *pMat = new FileBackedBigMatrix();
  bool connected = pMat->connect( 
    string(CHAR(STRING_ELT(fileName,0))),
    string(CHAR(STRING_ELT(filePath,0))),
    static_cast<index_type>(NUMERIC_VALUE(rows)),
    static_cast<index_type>(NUMERIC_VALUE(cols)),
    INTEGER_VALUE(typeLength),
    static_cast<bool>(LOGICAL_VALUE(separated)),
    static_cast<bool>(LOGICAL_VALUE(readOnly)));
  if (!connected)
  {
    delete pMat;
    return NULL_USER_OBJECT;
  }
  if (GET_LENGTH(colNames) > 0)
  {
    pMat->column_names(RChar2StringVec(colNames));
  }
  if (GET_LENGTH(rowNames) > 0)
  {
    pMat->row_names(RChar2StringVec(rowNames));
  }
  SEXP address = R_MakeExternalPtr( dynamic_cast<BigMatrix*>(pMat),
    R_NilValue, R_NilValue);
  R_RegisterCFinalizerEx(address, (R_CFinalizer_t) CDestroyBigMatrix, 
      (Rboolean) TRUE);
  return address;
}

SEXP SharedName( SEXP address )
{
  BigMatrix *pMat = (BigMatrix*)R_ExternalPtrAddr(address);
  SharedMemoryBigMatrix *psmbm = dynamic_cast<SharedMemoryBigMatrix*>(pMat);
  if (psmbm) return String2RChar(psmbm->shared_name());
  error("Object is not a shared memory big.matrix.");
  return R_NilValue;
  
}

SEXP FileName( SEXP address )  
{   
  BigMatrix *pMat = (BigMatrix*)R_ExternalPtrAddr(address);
  FileBackedBigMatrix *pfbbm = dynamic_cast<FileBackedBigMatrix*>(pMat);
  if (pfbbm) return String2RChar(pfbbm->file_name());
  error("Object is not a filebacked big.matrix.");
  return R_NilValue;
}

SEXP Flush( SEXP address )  
{   
  FileBackedBigMatrix *pMat =   
    reinterpret_cast<FileBackedBigMatrix*>(R_ExternalPtrAddr(address));   
  FileBackedBigMatrix *pfbbm = dynamic_cast<FileBackedBigMatrix*>(pMat);
  SEXP ret = PROTECT(NEW_LOGICAL(1));
  if (pfbbm)
  { 
    LOGICAL_DATA(ret)[0] = pfbbm->flush() ? (Rboolean)TRUE : Rboolean(FALSE);
  }
  else
  {
    LOGICAL_DATA(ret)[0] = (Rboolean)FALSE;
    error("Object is not a filebacked big.matrix");
  }
  UNPROTECT(1);
  return ret;
}

SEXP IsShared( SEXP address )
{
  FileBackedBigMatrix *pMat =   
    reinterpret_cast<FileBackedBigMatrix*>(R_ExternalPtrAddr(address));   
  SEXP ret = PROTECT(NEW_LOGICAL(1));
  LOGICAL_DATA(ret)[0] = pMat->shared() ? (Rboolean)TRUE : Rboolean(FALSE);
  UNPROTECT(1);
  return ret;
}

SEXP isnil(SEXP address)
{
  void *ptr = R_ExternalPtrAddr(address);
  SEXP ret = PROTECT(NEW_LOGICAL(1));
  LOGICAL_DATA(ret)[0] = (ptr==NULL) ? (Rboolean)TRUE : Rboolean(FALSE);
  UNPROTECT(1);
  return(ret);
}

} // extern "C"

