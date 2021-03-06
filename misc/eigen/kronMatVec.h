// This file is part of the KRYLSTAT function library
//
// Copyright (C) 2011 Erlend Aune <erlenda@math.ntnu.no>
//
// The KRYLSTAT library is free software; 
// you can redistribute it and/or modify it under the terms 
// of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the 
// License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// The KRYLSTAT library is distributed in the 
// hope that it will be useful, but WITHOUT ANY WARRANTY; without
// even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU Lesser General Public License
// or the GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// the KRYLSTAT library. If not, see 
// <http://www.gnu.org/licenses/>.

#ifndef KRON_MATVEC_H
#define KRON_MATVEC_H


#include <iostream>
#include <Eigen/Eigen>
#include <Eigen/Sparse>
#include <Eigen/StdVector>


using namespace Eigen;

namespace krylstat_misc{

template<class Derived1, class Derived2>
void kronMatVec2(std::vector<Derived1 , aligned_allocator<Derived1> > &Qs, MatrixBase<Derived2> &inVec, 
		MatrixBase<Derived2> &outVec)
{
  
  typedef typename Derived2::Scalar Scalar;
  
  int N = Qs.size();
  
  int totDim=1;
  for(int iii=0;iii<N;++iii)
  {
    totDim=totDim*Qs[iii].cols();
  }
  
  assert(inVec.rows() == totDim);
  
  VectorXi n=VectorXi::Zero(N);
  int nright=1;
  int nleft=1;
  for(int iii=1;iii<=N-1;++iii)
  {
    n[iii-1]=Qs[iii-1].rows();
    nleft=nleft*n[iii-1];
  }
  
  outVec=inVec;
  
  n[N-1]=Qs[N-1].rows();
  
  // std::cout << n << std::endl;

  
  int base;
  int jump;
  int index1;
  int index2;
  int sizeInner;
  int llk;
  
  Matrix<Scalar,Dynamic,1> innerInVec;
  Matrix<Scalar,Dynamic,1> innerOutVec;
  
  
  for(int iii=N;iii>=1;--iii)
  {
    base=0;
    jump=n[iii-1]*nright;
    for(int kkk=1;kkk<=nleft;++kkk)
    {
      for(int jjj=1;jjj<=nright;++jjj)
      {
	index1=base+jjj;
	index2=base+jjj+nright*(n[iii-1]-1);
	
	sizeInner=(index2-index1)/nright+1;
	innerInVec.resize(sizeInner);
	innerOutVec.resize(sizeInner);

	llk=1;
	for(int lll=index1;lll<=index2;lll=lll+nright)
	{
	  innerInVec[llk-1]=outVec[lll-1];
	  llk=llk+1;
	}

	innerOutVec.noalias()=Qs[iii-1]*innerInVec;
	
	llk=1;
	for(int lll=index1;lll<=index2;lll=lll+nright)
	{
	  outVec[lll-1]=innerOutVec[llk-1];
	  llk=llk+1;
	}

      }
      base=base+jump;
      
    }
    
    nleft=nleft/n[std::max(iii-2,1)];
    nright=nright*n[iii-1];
  }
  
  
}



template<class Derived1, class Derived2>
void kronMatVec(const std::vector<Derived1 , aligned_allocator<Derived1> > &Qs, const MatrixBase<Derived2> &inVec, 
		const MatrixBase<Derived2> &outVec)
{
  
  typedef typename Derived2::Scalar Scalar;
  
  int N = Qs.size();
  
  int totDim=1;
  for(int iii=0;iii<N;++iii)
  {
    totDim=totDim*Qs[iii].cols();
  }
  
  assert(inVec.rows() == totDim);
  
  VectorXi n=VectorXi::Zero(N);
  int nright=1;
  int nleft=1;
  for(int iii=1;iii<=N-1;++iii)
  {
    n[iii-1]=Qs[iii-1].rows();
    nleft=nleft*n[iii-1];
  }
  
  const_cast< MatrixBase<Derived2>& >(outVec)=inVec;

  n[N-1]=Qs[N-1].rows();

  int base;
  int jump;
  int index1;
  int index2;
  int sizeInner;
  MatrixBase<Derived2>& outVecNonConst = const_cast< MatrixBase<Derived2>& >(outVec);
  // std::cout << "n: "<< n << "\n" << std::endl;
  
  for(int iii=N;iii>=1;--iii)
  {
    base=0;
    jump=n[iii-1]*nright;
    for(int kkk=1;kkk<=nleft;++kkk)
    {
      for(int jjj=1;jjj<=nright;++jjj)
      {
	index1=base+jjj;
	index2=base+jjj+nright*(n[iii-1]-1);
	
	sizeInner=(index2-index1)/nright+1;
	//sizeInner=n(iii-1);
	
	Map<Matrix<Scalar,Dynamic,1>,0,InnerStride<> > outMap(&outVecNonConst.coeffRef(index1-1),sizeInner, InnerStride<>(nright));
	//std::cout << sizeInner << "\n" << std::endl;
	outMap=Qs[iii-1]*outMap;
	// const_cast< Map<Matrix<Scalar,Dynamic,1>,0,InnerStride<> >& >(outMap)=Qs[iii-1]*outMap;

      }
      //std::cout << "Base-Jump: " << base << "," << jump << std::endl;
      base=base+jump;
      
      
    }
    //std::cout << "nleft,max(iii-2,1)-1: " <<nleft << "," << std::max(iii-2,1)-1 << std::endl;
    nleft=nleft/(n[std::max(iii-2,1)-1]);
    nright=nright*n[iii-1];
    // std::cout <<"nleft-nright-check: " << nleft << "," << nright << "," << n[std::max(iii-2,1)-1]<< std::endl;
  }
  
}
  
  
}



#endif