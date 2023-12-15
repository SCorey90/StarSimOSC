#ifndef FEMTO_TRACK_H
#define FEMTO_TRACK_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#include "FemtoDstBranch.h"
#include <iostream>

class FemtoTrack : public FemtoDstBranch
{
public:
	virtual const char* classname() const { return "FemtoTrack"; }
	FemtoTrack(){
		reset();
	}
	~FemtoTrack(){
	}

	void reset(){
		mPt                 = 0.0;
		mEta                = 0.0;
		mPhi                = 0.0;
		mId                 = 0.0;
		mDedx               = 0.0;
		mNHitsFit           = 0.0;
		mNHitsMax           = 0.0;
		mNHitsDedx          = 0.0;
		mNSigmaPion         = 0.0;
		mNSigmaKaon         = 0.0;
		mNSigmaProton       = 0.0;
		mNSigmaElectron     = 0.0;
		mDCA                = 0.0;
        mDCAXY              = 0.0;
		mChi2               = 99.0;
		
		// pidTraits
		mBTofPidTraitsIndex = -1;
		mMtdPidTraitsIndex  = -1;
		mEmcPidTraitsIndex  = -1;
		mHelixIndex         = -1;
		mMcIndex            = -1;
	}

	void copy( FemtoTrack * that ) {
		this->mPt                 = that->mPt;
		this->mEta                = that->mEta;
		this->mPhi                = that->mPhi;
		this->mId                 = that->mId;
		this->mDedx               = that->mDedx;
		this->mNHitsFit           = that->mNHitsFit;
		this->mNHitsMax           = that->mNHitsMax;
		this->mNHitsDedx          = that->mNHitsDedx;
		this->mNSigmaPion         = that->mNSigmaPion;
		this->mNSigmaKaon         = that->mNSigmaKaon;
		this->mNSigmaProton       = that->mNSigmaProton;
		this->mNSigmaElectron     = that->mNSigmaElectron;
		this->mDCA                = that->mDCA;
		this->mChi2               = that->mChi2;
        this->mDCAXY              = that->mDCAXY;

		this->mBTofPidTraitsIndex = that->mBTofPidTraitsIndex;
		this->mMtdPidTraitsIndex  = that->mMtdPidTraitsIndex;
		this->mEmcPidTraitsIndex  = that->mEmcPidTraitsIndex;
		this->mHelixIndex         = that->mHelixIndex;
		this->mMcIndex            = that->mMcIndex;

		cerr << "nHitsFit = " <<  that->mNHitsFit << endl;
	}

	TVector3 momentum() { TVector3 v; v.SetPtEtaPhi( mPt, mEta, mPhi ); return v; } 
	void momentum( float pt, float eta, float phi ) { 
		this->mPt  = pt;
		this->mEta = eta;
		this->mPhi = phi;
	}
	TLorentzVector lv( double m ) { 
		TLorentzVector lv;
		lv.SetPtEtaPhiM( mPt, mEta, mPhi, m );
		return lv;
	}


	Float_t 	mPt;				
	Float_t 	mEta;				
	Float_t 	mPhi;				
	UShort_t 	mId;				
	
	
	// pidTraits
	Short_t 	mBTofPidTraitsIndex; // index of the BTOF pidTratis in the event
	Short_t 	mMtdPidTraitsIndex;  // index of the MTD pidTratis in the event
	Short_t 	mEmcPidTraitsIndex;  // index of the EMC pidTratis in the event
	Short_t 	mHelixIndex;		 // index of associated Track Helix
	Short_t 	mMcIndex;			 // index of associated Track Helix

	Float_t 	mDedx;				
	Short_t 	mNHitsFit;			
	Short_t 	mNHitsMax;			
	Short_t 	mNHitsDedx;			
	Short_t 	mNSigmaPion;		
	Short_t 	mNSigmaKaon;		
	Short_t 	mNSigmaProton;		
	Short_t 	mNSigmaElectron;	
	Float_t 	mDCA;				
	Float_t 	mChi2;
    Float_t     mDCAXY;
	
	ClassDef( FemtoTrack, 3 )
};

#endif