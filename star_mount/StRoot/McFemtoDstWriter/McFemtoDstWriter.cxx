
#include "FemtoDstFormat/BranchWriter.h"
#include "McFemtoDstWriter.h"

#include "StThreeVectorF.hh"

#include "StEvent.h"
#include "StTrack.h"
#include "StTrackGeometry.h"
#include "StDcaGeometry.h"
#include "StMtdPidTraits.h"
#include "StBTofPidTraits.h"
#include "StPhysicalHelixD.hh"
#include "StEvent/StTrackNode.h"
#include "StEvent/StGlobalTrack.h"
#include "StEvent/StRunInfo.h"
#include "StEvent/StEventInfo.h"
#include "StEvent/StPrimaryVertex.h"
#include "StEvent/StTrackGeometry.h"


#include "StMcEvent/StMcEvent.hh"
#include "StMcEvent/StMcTrack.hh"
#include "StMcEvent/StMcVertex.hh"

#include "StMuDSTMaker/COMMON/StMuTrack.h"

#include "StAssociationMaker/StTrackPairInfo.hh"

#include "StarClassLibrary/StParticleDefinition.hh"



// STL
#include <vector>
#include <map>
#include <algorithm>

ClassImp(McFemtoDstWriter)

int NCommonHits = 10;

McFemtoDstWriter::McFemtoDstWriter( const Char_t *name /*, char* filename*/) : StMaker( name )
{
	this->_outputFilename = /*filename*/ + "FemtoDst.root";

	SetDebug(1);
}

McFemtoDstWriter::~McFemtoDstWriter()
{

}

Int_t McFemtoDstWriter::Init()
{

	LOG_INFO << "INITIALIZE" <<endm;
	this->_rootFile = new TFile( this->_outputFilename.c_str(), "RECREATE" );
	this->_rootFile->cd();
	this->_tree = new TTree( "FemtoDst", "FemtoDst with MC info" );

	// Create Event Branch
	_few.createBranch( this->_tree, "Event" );
	_ftw.createBranch( this->_tree, "Tracks" );
	_fmcw.createBranch( this->_tree, "McTracks" );
	_fmcvertw.createBranch( this->_tree, "McVertices" );
	_fmtdw.createBranch( this->_tree, "MtdPidTraits" );
	_fbtofw.createBranch( this->_tree, "BTofPidTraits" );
	this->_fhw.createBranch( this->_tree, "Helices" );


	return kStOK;
}

Int_t McFemtoDstWriter::Finish()
{
	LOG_INFO << "FINISH" <<endm;
	this->_rootFile->Write();
	this->_rootFile->Close();
	return kStOK;
}

Int_t McFemtoDstWriter::Make()
{
	LOG_INFO << "McFemtoDstWriter::Make()" << endm;
	
	// Check that the DS are present
	this->_StEvent = (StEvent*) GetInputDS("StEvent");
	if (nullptr == this->_StEvent) {
		LOG_INFO << "No StEvent Found, cannot proceed" << endm;
		return kStWarn;
	}

	this->_StMcEvent = (StMcEvent*) GetDataSet("StMcEvent");
	if (nullptr == this->_StMcEvent) {
		LOG_INFO << "No StMcEvent Found, cannot proceed" << endm;
		return kStWarn;
	}

	this->_StAssociationMaker = (StAssociationMaker*) GetMaker("StAssociationMaker");
	if ( nullptr == this->_StAssociationMaker ) {
		LOG_INFO << "No StAssociationMaker Found, cannot proceed" << endm;
		return kStWarn;
	}



	this->_rcTrackMap = this->_StAssociationMaker->rcTrackMap();
	this->_mcTrackMap = this->_StAssociationMaker->mcTrackMap();

	// LOG_INFO << "rcTrackMap" << _rcTrackMap << endm << endl;
	// LOG_INFO << *_rcTrackMap << endm << endl;
	// return kStOK;
	// LOG_INFO << "McTrackMap = " << this->_StAssociationMaker->mcTrackMap()->count( nullptr ) << endm << endl;
	// this->_StAssociationMaker->mcTrackMap()->equal_range( nullptr );

	// LOG_INFO << "got equal range" << endm << endl;
	

	// Fill Event Info
	this->_fmtEvent.mRunId   = this->_StEvent->runInfo()->runId();
	this->_fmtEvent.mEventId = this->_StEvent->info()->id();


	StPrimaryVertex *primaryVertex = this->_StEvent->primaryVertex(0);
	if ( nullptr == primaryVertex ){
		LOG_INFO << "No Primary Vertex Found, skipping event" << endm;
		return kStWarn;
	}
	this->_pvPosition = primaryVertex->position();
	this->_fmtEvent.vertex( this->_pvPosition.x(), this->_pvPosition.y(), this->_pvPosition.z() );

	_few.set( this->_fmtEvent );

	// RESET containers
	this->_fmcw.reset();
	// this->_fmcvertw.reset();
	this->_ftw.reset();
	// this->_fhw.reset();
	// this->_fmtdw.reset();
	// this->_fbtofw.reset();

	
	StSPtrVecTrackNode &trackNodes = this->_StEvent->trackNodes();
	Int_t nTracks = this->_StEvent->trackNodes().size();

	/*********************************************************/
	// TRACKS
	
	for ( Int_t iTrack = 0; iTrack < nTracks; iTrack++ ){
		StTrackNode *track = this->_StEvent->trackNodes()[ iTrack ];

		this->_fmtTrack.reset();
		// addMtdPidTraits( track );
		addBTofPidTraits( track );
		addTrackHelix( track );
		addTrack( track );

	}
	// LOG_INFO << "_ftw._n =" << _ftw.N() << endm;

	// TRACKS
	/*********************************************************/


	/*********************************************************/
	// MCTRACKS
	StSPtrVecMcTrack mcTracks = this->_StMcEvent->tracks();
	int nMcTracks = mcTracks.size();
	for ( int iMcTrack = 0; iMcTrack < nMcTracks; iMcTrack++ ){
		StMcTrack * mcTrack = mcTracks[ iMcTrack ];
		if ( nullptr == mcTrack ){
			continue;
		}
		int rcTrackIndex = getMatchedTrackIndex( mcTrack );

		StTrack *rcTrack = nullptr;
		if ( rcTrackIndex >= 0 )
			rcTrack = trackNodes[rcTrackIndex]->track(primary);

		if ( Debug() ) {
			LOG_INFO << "Matched Index : " << rcTrackIndex << ", geantId = " << mcTrack->geantId() << ", pt = " << mcTrack->pt() << endm << endl;

			if ( rcTrack && rcTrack->geometry() )
				LOG_INFO << "pt = " << mcTrack->pt() << " == " << rcTrack->geometry()->momentum().perp() << ", key = " << (rcTrackIndex+1) << " == " << rcTrack->key() << endm << endl;
		}

		addMcTrack( mcTrack, rcTrack );
	}

	// MCTRACKS
	/*********************************************************/

	/*********************************************************/
	// MC Vertices
	StSPtrVecMcVertex McVertices = this->_StMcEvent->vertices();
	int nMcVertices = McVertices.size();
	for ( int iMcVertex = 0; iMcVertex < nMcVertices; iMcVertex++ ){
		StMcVertex *mcVertex = McVertices[ iMcVertex ];
		addMcVertex( mcVertex );
	}

	// MC Vertices
	/*********************************************************/

	this->_tree->Fill();

	return kStOK;

}

int McFemtoDstWriter::getMatchedTrackIndex( StMcTrack * mcTrack ){
	// LOG_INFO << "McFemtoDstWriter::getMatchedTrackIndex(...)" << endm;
	if ( nullptr == this->_mcTrackMap ){
		LOG_INFO << "Invalid McTrackMap" << endm << endl;
		return -1;
	}
	
	pair<mcTrackMapIter,mcTrackMapIter> mcBounds = this->_mcTrackMap->equal_range( mcTrack );

	Int_t maxCommonHits = 0;
	const StGlobalTrack *rcCandTrack = 0;
	// LOG_INFO << "PRELOOP" << endm << endl;
	for(mcTrackMapIter mcMapIter = mcBounds.first; mcMapIter != mcBounds.second; mcMapIter ++) {
		// LOG_INFO << "LOOP" << endm << endl;
		StTrackPairInfo *pair = mcMapIter->second;
		const StGlobalTrack *rcTrack = pair->partnerTrack();
		Int_t commonHits = pair->commonTpcHits();
		if(commonHits > maxCommonHits){
			maxCommonHits = commonHits;
			rcCandTrack = rcTrack;
			// LOG_INFO << "#common Hits = " << commonHits << endm << endl;
		}
	}

	Int_t rcIndex = -1;
	if(maxCommonHits>=10)
	{
		StSPtrVecTrackNode &trackNodes = this->_StEvent->trackNodes();
		Int_t nNodes = trackNodes.size();

		for(Int_t i=0; i<nNodes; i++){
			StTrack *rcTrack = trackNodes[i]->track(primary);	// PRIMARY TRACKS
			if(!rcTrack) continue;
			if(rcTrack->key()==rcCandTrack->key()){
				rcIndex = i;
				break;
			}
		}
	}
	return rcIndex;
}


void McFemtoDstWriter::addMcVertex( StMcVertex *mcVertex ){

	this->_fmtMcVertex.reset();
	if ( nullptr == mcVertex ){
		LOG_INFO << "WARN : null MC Vertex, skipping" << endm;
	}
	this->_fmtMcVertex.mId  = mcVertex->key() - 1;

	this->_fmtMcVertex.mX = mcVertex->position().x();
	this->_fmtMcVertex.mY = mcVertex->position().y();
	this->_fmtMcVertex.mZ = mcVertex->position().z();
	this->_fmtMcVertex.mGeantProcess = mcVertex->geantProcess();

	if ( nullptr != mcVertex->parent() ){
		this->_fmtMcVertex.mParentIndex = mcVertex->parent()->key() - 1;
	}

	this->_fmcvertw.add( this->_fmtMcVertex );
}

void McFemtoDstWriter::addMcTrack( StMcTrack *mcTrack, StTrack *rcTrack )
{
	if ( nullptr == mcTrack ){
		LOG_INFO << "WARN : null MC track, skipping" << endm;
		return;
	}

	this->_fmtMcTrack.reset();

	this->_fmtMcTrack.mId  = mcTrack->key() - 1;
	
	StThreeVectorF p       = mcTrack->momentum();
	this->_fmtMcTrack.mPt  = p.perp();
	this->_fmtMcTrack.mEta = p.pseudoRapidity();
	this->_fmtMcTrack.mPhi = p.phi();
	
	this->_fmtMcTrack.mGeantPID = mcTrack->geantId();
	
	if ( nullptr != mcTrack->particleDefinition() ) 
		this->_fmtMcTrack.mCharge   = mcTrack->particleDefinition()->charge();

	StMcTrack *mcParent = mcTrack->parent();
	if ( nullptr != mcParent ){
		this->_fmtMcTrack.mParentIndex = mcParent->key() - 1; // key is indexed at 1
	}

	StMcVertex *startVertex = mcTrack->startVertex();
	StMcVertex *stopVertex = mcTrack->stopVertex();

	if (nullptr != startVertex)
		this->_fmtMcTrack.mStartVertexIndex = startVertex->key() - 1;
	if (nullptr != stopVertex)
		this->_fmtMcTrack.mStopVertexIndex = stopVertex->key() - 1;


	// If the MC track is matched to a RECO track
	if ( nullptr != rcTrack ){
		
		this->_fmtMcTrack.mAssociatedIndex = rcTrack->key() - 1;
		FemtoTrack * t = this->_ftw.at( this->_fmtMcTrack.mAssociatedIndex );

		if ( nullptr != t ){
			t->mMcIndex = mcTrack->key() - 1;
		}
	}

	int nTpcHits = mcTrack->tpcHits().size();
	this->_fmtMcTrack.mNHits = clamp( nTpcHits, 0, 49 );
	

	this->_fmcw.add( this->_fmtMcTrack );

}



void McFemtoDstWriter::addTrackHelix( StTrackNode *node )
{
	StGlobalTrack * globalTrack = dynamic_cast<StGlobalTrack*>(node->track(global));
	if ( nullptr == globalTrack )
		return;
	StDcaGeometry	*dcaGeom = globalTrack->dcaGeometry();
	if ( dcaGeom == nullptr )
		return;
	const float* params = dcaGeom->params();
	for (int i = 0; i < 6; i++) this->_fmtHelix.mPar[i] = params[i];

	this->_fmtHelix.mMap0 = (UInt_t)(globalTrack->topologyMap().data(0));
	this->_fmtHelix.mMap1 = (UInt_t)(globalTrack->topologyMap().data(1));

	this->_fmtTrack.mHelixIndex = this->_fhw.N();
	this->_fhw.add( this->_fmtHelix );
}

void McFemtoDstWriter::addTrack( StTrackNode *node )
{

	// NOTE:
	// Mtd, BTof Pid traits index may already be set, dont reset

	// create a MuTrack since it is simpler to use
	StGlobalTrack * globalTrack = dynamic_cast<StGlobalTrack*>(node->track(global));
	StTrack * primaryTrack = node->track( primary );

	LOG_INFO << "StTrack globalPt " << globalTrack->geometry()->momentum().perp() << endm;
	if ( primaryTrack ){
		LOG_INFO << "StTrack primaryPt " << primaryTrack->geometry()->momentum().perp() << endm;
	}

	const StVertex *vtx = nullptr;
	if ( primaryTrack )
		vtx = primaryTrack->vertex();
	if ( nullptr == vtx )
		vtx = this->_StEvent->primaryVertex();

	StMuTrack *muTrack = new StMuTrack( this->_StEvent, globalTrack, 0 );

	if ( nullptr == muTrack ){
		LOG_INFO << "WARN, null StMuTrack" << endm;
		return;
	}
	LOG_INFO << "nHitsPoss = " << globalTrack->numberOfPossiblePoints() << " == " << muTrack->nHitsPoss() << endm << endl;
	LOG_INFO << "nHitsFit = " << globalTrack->fitTraits().numberOfFitPoints() << " == " << muTrack->nHitsFit() << endm << endl;
	LOG_INFO << "dEdx = " << muTrack->dEdx() << endm;

	this->_fmtTrack.mNHitsMax = muTrack->nHitsPoss();
	this->_fmtTrack.mNHitsFit = muTrack->nHitsFit() * muTrack->charge();
	this->_fmtTrack.mNHitsDedx = muTrack->nHitsDedx();

	this->_fmtTrack.mDedx = ( muTrack->dEdx() * 1.e6 );
	//this->_fmtTrack.nSigmaElectron( muTrack->nSigmaElectron() );
	//this->_fmtTrack.nSigmaPion( muTrack->nSigmaPion() );
	//this->_fmtTrack.nSigmaKaon( muTrack->nSigmaKaon() );
	//this->_fmtTrack.nSigmaProton( muTrack->nSigmaProton() );

	StThreeVectorF p = muTrack->p();
	LOG_INFO << "MuTrack Pt = " << p.perp() << endm;
	this->_fmtTrack.mPt  = p.perp();
	this->_fmtTrack.mEta = p.pseudoRapidity();
	this->_fmtTrack.mPhi = p.phi();

	this->_fmtTrack.mId = muTrack->id() - 1; // id starts at 1
	this->_fmtTrack.mDCA = calculateDCA( globalTrack, this->_pvPosition );
	this->_fmtTrack.mChi2 = globalTrack->fitTraits().chi2();

    //LOG_INFO << "gDCA = " << globalTrack->dca().mag() << endm;
    //LOG_INFO << "gDCAD = " << globalTrack->dcaD() << endm;

	this->_ftw.add( this->_fmtTrack );


	LOG_INFO << "nHitsFit = " << this->_fmtTrack.mNHitsFit << endm;
}


double McFemtoDstWriter::calculateDCA(StGlobalTrack *globalTrack, StThreeVectorF vtxPos) const
{
	if( nullptr == globalTrack ) 
		return 999;
	
	StDcaGeometry	*dcaGeom = globalTrack->dcaGeometry();
	
	if( nullptr == dcaGeom ) 
		return 999;
	
	StPhysicalHelixD dcaHelix = dcaGeom->helix();
	return dcaHelix.distance(vtxPos,kFALSE);
}

void McFemtoDstWriter::addMtdPidTraits( StTrackNode *node )
{
	LOG_INFO << "addMtdPidTraits(...)" << endm;
	StTrack *track = node->track( primary );
	if ( nullptr == track ) 
		return;
	StPtrVecTrackPidTraits traits = track->pidTraits(kMtdId);
	LOG_INFO << "MtdPidTraits size " << traits.size() << endm;
	if ( traits.size() <= 0 )
		return;

	StMtdPidTraits *mtdPid = dynamic_cast<StMtdPidTraits*>(traits[0]);
	if ( nullptr == mtdPid ){
		LOG_INFO << "WARN, null MtdPidTraits" << endm;
	}


	
	LOG_INFO << "MC MtdHit? " << mtdPid->mtdHit()->idTruth() << endm;
	LOG_INFO << "MC MtdHit? QA = " << mtdPid->mtdHit()->qaTruth() << endm;
	
	if ( mtdPid->mtdHit()->associatedTrack() ){
		LOG_INFO << "MC MtdHit Track " << mtdPid->mtdHit()->associatedTrack()->key() << endm;
		LOG_INFO << "this track keey = " << track->key() << endm;
	}
	

	StMtdHit *hit = mtdPid->mtdHit();

	this->_fmtMtdPid.reset();
	this->_fmtMtdPid.mDeltaY            = mtdPid->deltaY();
	this->_fmtMtdPid.mDeltaZ            = mtdPid->deltaZ();
	this->_fmtMtdPid.mMatchFlag         = mtdPid->matchFlag();
	this->_fmtMtdPid.mDeltaTimeOfFlight = 0;
	this->_fmtMtdPid.mMtdHitChan        = (hit->backleg() - 1) * 60 + (hit->module() - 1) * 12 + hit->cell();
	this->_fmtMtdPid.mIdTruth           = hit->idTruth() - 1;	// minus one to index at 0

	this->_fmtTrack.mMtdPidTraitsIndex = this->_fmtdw.N();

	this->_fmtdw.add( this->_fmtMtdPid );

}


void McFemtoDstWriter::addBTofPidTraits( StTrackNode *node )
{
	
	LOG_INFO << "addBTofPidTraits(...)" << endm;
	StTrack *track = node->track( primary );
	if ( nullptr == track ) 
		return;
	StPtrVecTrackPidTraits traits = track->pidTraits(kTofId);
	LOG_INFO << "BTofPidTraits size " << traits.size() << endm;
	
	if ( traits.size() <= 0 )
		return;

	StBTofPidTraits *btofPid = dynamic_cast<StBTofPidTraits*>(traits[0]);
	if ( nullptr == btofPid ){
		LOG_INFO << "WARN, null BTofPidTraits" << endm;
		return;
	}

	if ( Debug() ) {
		LOG_INFO << "MC BTofHit? " << btofPid->tofHit()->idTruth() << endm;
	 	LOG_INFO << "MC BTofHit? QA = " << btofPid->tofHit()->qaTruth() << endm;
	
		if ( btofPid->tofHit()->associatedTrack() ){
			LOG_INFO << "MC BTofHit Track " << btofPid->tofHit()->associatedTrack()->key() << endm;
			LOG_INFO << "this track keey = " << track->key() << endm;
		}
	}

	StBTofHit *hit = btofPid->tofHit();

	this->_fmtBTofPid.reset();
	this->_fmtBTofPid.mBTofYLocal = (btofPid->yLocal() );
	this->_fmtBTofPid.mBTofZLocal = ( btofPid->zLocal() );
	this->_fmtBTofPid.matchFlag( btofPid->matchFlag() );
	this->_fmtBTofPid.mIdTruth  =  btofPid->tofHit()->idTruth() - 1; // minus 1 to index at 0
	this->_fmtBTofPid.mLength   = btofPid->pathLength();

	double b = btofPid->beta();
	if ( b < 0 )
		b = 0;
	this->_fmtBTofPid.mBTofBeta = ( btofPid->timeOfFlight() );

	LOG_INFO << "beta = " << btofPid->timeOfFlight() << endm;


	this->_fmtTrack.mBTofPidTraitsIndex = this->_fbtofw.N();

	this->_fbtofw.add( this->_fmtBTofPid );

}
