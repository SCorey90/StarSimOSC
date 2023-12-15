#ifndef FEMTO_BTOF_PID_TRAITS_H
#define FEMTO_BTOF_PID_TRAITS_H

#include "TObject.h"
#include "FemtoDstBranch.h"

class FemtoBTofPidTraits : public FemtoDstBranch
{
public:
	virtual const char* classname() const { return "FemtoBTofPidTraits"; }
	FemtoBTofPidTraits(){ reset(); }
	~FemtoBTofPidTraits(){ }

	void reset(){
		this->mBTofBeta      = 0.0;
		this->mBTofYLocal    = -10.0;
		this->mBTofZLocal    = -10.0;
		this->mLength        = 0.0;
		this->mBTofMatchFlag = 0.0;
		this->mIdTruth       = -1;
	}
	void copy( FemtoBTofPidTraits * that){
		this->mLength        = that->mLength;
		this->mBTofBeta      = that->mBTofBeta;
		this->mBTofYLocal    = that->mBTofYLocal;
		this->mBTofZLocal    = that->mBTofZLocal;
		this->mBTofMatchFlag = that->mBTofMatchFlag;
		this->mIdTruth       = that->mIdTruth;
	}
	void matchFlag( int tmp ){ this->mBTofMatchFlag = tmp; }
	int matchFlag() { return this->mBTofMatchFlag; }

	Float_t  mBTofBeta;			// beta
	Float_t  mBTofYLocal;			// ylocal 
	Float_t  mBTofZLocal;			// zlocal 
	UChar_t  mBTofMatchFlag;	// 0 - no match, 1 - one-to-one, 2 - one-to-multiple
	Short_t  mIdTruth;			// the idTruth used to id which MCTrack caused this info, only used in MC
	Float_t  mLength;

	ClassDef( FemtoBTofPidTraits, 4 )
};

#endif