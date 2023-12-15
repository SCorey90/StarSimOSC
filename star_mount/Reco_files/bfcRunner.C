

class StMaker;

void bfcRunner( int nevt, char* chain_opts, char* filename ){

	// needed since I use the StMuTrack
	gSystem->Load( "St_base" );
	gSystem->Load("StarClassLibrary");
	// gSystem->Load("StStrangeMuDstMaker");
	// gSystem->Load("StMuDSTMaker");

        gSystem->Load( "libStarRoot.so" );

        gROOT->SetMacroPath(".:/star-sw/StRoot/macros/:./StRoot/macros:./StRoot/macros/graphics:./StRoot/macros/analysis:./StRoot/macros/test:./StRoot/macros/examples:./StRoot/macros/html:./StRoot/macros/qa:./StRoot/macros/calib:./StRoot/macros/mudst:/afs/rhic.bnl.gov/star/packages/DEV/StRoot/macros:/afs/rhic.bnl.gov/star/packages/DEV/StRoot/macros/graphics:/afs/rhic.bnl.gov/star/packages/DEV/StRoot/macros/analysis:/afs/rhic.bnl.gov/star/packages/DEV/StRoot/macros/test:/afs/rhic.bnl.gov/star/packages/DEV/StRoot/macros/examples:/afs/rhic.bnl.gov/star/packages/DEV/StRoot/macros/html:/afs/rhic.bnl.gov/star/packages/DEV/StRoot/macros/qa:/afs/rhic.bnl.gov/star/packages/DEV/StRoot/macros/calib:/afs/rhic.bnl.gov/star/packages/DEV/StRoot/macros/mudst:/afs/rhic.bnl.gov/star/ROOT/36/5.34.38/.sl73_x8664_gcc485/rootdeb/macros:/afs/rhic.bnl.gov/star/ROOT/36/5.34.38/.sl73_x8664_gcc485/rootdeb/tutorials");

	gROOT->LoadMacro("./bfc.C");

	bfc( -1, chain_opts, filename );

	gSystem->Load("FemtoDstFormat");
	gSystem->Load("McFemtoDstWriter");

	

	// StMcEventMaker *mcEvent = new StMcEventMaker();
	// StAssociationMaker *association = new StAssociationMaker( );       // TPC association maker
	// association->SetDebug(0);

	McFemtoDstWriter *fmtWriter = new McFemtoDstWriter( "MtdMc");
	fmtWriter->SetDebug(0);
	fmtWriter->setOutputFileName( "FemtoDst.root" );

	
	// chain->AddAfter("MuDst", mcEvent );
	// chain->AddAfter("StMcEvent", association );
	// cout << "ADDING fmtWriter = " << chain->AddAfter("StMcEventMaker", fmtWriter ) << endl;
	cout << "ADDING fmtWriter = " << chain->AddAfter("StAssociationMaker", fmtWriter ) << endl;

	StBTofMatchMaker * btofMM = (StBTofMatchMaker*) chain->GetMaker( "btofMatch" );
	btofMM->setCalculateAlign(kTRUE);

	StMaker::lsMakers(chain);
	chain->PrintInfo();
	chain->Init();
	chain->EventLoop(nevt);
	chain->Finish();

	//delete chain;
}
