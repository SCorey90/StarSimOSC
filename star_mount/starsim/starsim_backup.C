// macro to instantiate the Geant3 from within
// STAR  C++  framework and get the starsim prompt
// To use it do
//  root4star starsim.C
#include "TParticle.h"

class St_geant_Maker;
St_geant_Maker *geant_maker = 0;

class StarGenEvent;
StarGenEvent *event = 0;

class StarPrimaryMaker;
StarPrimaryMaker *_primary = 0;

class StarKinematics;
StarKinematics *kinematics = 0;

int fNParticles = 2;

// ----------------------------------------------------------------------------
void geometry(TString tag, Bool_t agml = true) {
  TString cmd = "DETP GEOM ";
  cmd += tag;
  if (!geant_maker)
    geant_maker = (St_geant_Maker *)chain->GetMaker("geant");
  geant_maker->LoadGeometry(cmd);
  //  if ( agml ) command("gexec $STAR_LIB/libxgeometry.so");
}
FILE *OpenFile(std::string starlight_file_path) {
  FILE *starlight_file = fopen(starlight_file_path.c_str(), "r");

  if (!starlight_file) {
    perror("Failed to open starlight output. Is the filename correct?");
    exit(1);
  }
  return starlight_file;
}

// move foward to next event, and read how many tracks are in this event
Int_t NextEvent(FILE *filelist) {
  char linelabel[20];
  int i1 = 0;
  int i2 = 0;
  int i3 = 0;
  double x1 = 0.0;
  double x2 = 0.0;
  double x3 = 0.0;
  double x4 = 0.0;
  int ntrk = 0;
  int nvtx = 0;
  // Event line
  fscanf(filelist, "%s %d %d %d ", linelabel, &i1, &ntrk, &i2);
  fNParticles = ntrk;
  // std::cout << linelabel << "  " << i1 << "  " << ntrk << "  " << i2 << "   "
  //          << fNParticles << std::endl;
  // Vertex line
  fscanf(filelist, "%s %lf %lf %lf %lf %d %d %d %d", linelabel, &x1, &x2, &x3,
         &x4, &i1, &i2, &i3, &nvtx);
  // std::cout << linelabel << "  " << x1 << "  " << x2 << "  " << x3 << "  " << x4
  //          << "  " << i1 << "  " << i2 << "  " << i3 << "  " << nvtx
  //          << std::endl;
  if (ntrk != nvtx)
    std::cout << "ERROR: ntrk = " << ntrk << "  nvtx = " << nvtx << std::endl;
  //
  return fNParticles;
}

// read particle data from a event.
TParticle *NextParticle(FILE *filelist) {
  char tracklabel[20];
  int i1 = 0;
  int i2 = 0;
  int i3 = 0;
  int i4 = 0;
  Int_t idpart = 0;
  Double_t px = 0.0;
  Double_t py = 0.0;
  Double_t pz = 0.0;
  Double_t ep = 0.0;

  // std::cout << "In NextParticle: fNparticles = " << fNParticles << std::endl;

  // for ( int itk=0; itk < fNParticles; itk++){

  fscanf(filelist, "%s %d %le %le %le %d %d %d %d", tracklabel, &i1, &px, &py,
         &pz, &i2, &i3, &i4, &idpart);
  // std::cout << "   " << tracklabel << "  " << i1 << "  " << px << "  " << py
  //          << "  " << pz << "  " << i2 << "  " << i3 << "  " << i4 << "  "
  //          << idpart << std::endl;

  TParticle *particle =
      new TParticle(idpart, 0, -1, -1, -1, -1, px, py, pz, ep, 0., 0., 0., 0.);
  if (idpart == 11 || idpart == -11) {
    particle->SetCalcMass(0.00051099907);
  } else if (idpart == 13 || idpart == -13) {
    particle->SetCalcMass(0.105658389);
  } else if (idpart == 211 || idpart == -211) {
    particle->SetCalcMass(0.13956995);
  } else if (idpart == 321 || idpart == -321) {
    particle->SetCalcMass(0.493677);
  } else {
    std::cout << "unknown daughter!  please edit the code to accomodate"
              << std::endl;
    exit(0);
  }

  return particle;
}
// ----------------------------------------------------------------------------
void command(TString cmd) {
  if (!geant_maker)
    geant_maker = (St_geant_Maker *)chain->GetMaker("geant");
  geant_maker->Do(cmd);
}
// ----------------------------------------------------------------------------
void trig(FILE* my_file, Int_t n = 1) {
  double mass;
  // loop through slight events 
  for (Int_t i = 0; i < n; i++) {
    // for each event, return how many tracks
    const int ntracks = NextEvent(my_file);
    // create particle vector for all tracks
    TLorentzVector *vecArr[ntracks];
    Int_t idpart = 0;
    // Clear the chain from the previous event
    chain->Clear();
    // Looping over the tracks of the event
    for (Int_t tr = 0; tr < ntracks; tr++) {
      // get particle data for all tracks
      TParticle *part = NextParticle(my_file);
      // note mass are hard coded based on pdg code
      mass = part->GetCalcMass();
      idpart = part->GetPdgCode();
      // simple energy calculation
      Double_t energy =
          TMath::Sqrt(mass * mass + part->Px() * part->Px() +
                      part->Py() * part->Py() + part->Pz() * part->Pz());
      // convert current track index to a lorentz vector
      vecArr[tr] = new TLorentzVector;
      vecArr[tr]->SetPxPyPzE(part->Px(), part->Py(), part->Pz(), energy);

      // case for K+
      if (idpart == 321){
        StarGenParticle *kaon_plus = kinematics->AddParticle("K+");
        double px  = vecArr[tr]->Px();
        double py  = vecArr[tr]->Py();
        double pz  = vecArr[tr]->Pz();
        kaon_plus->SetPx(px);
        kaon_plus->SetPy(py);
        kaon_plus->SetPz(pz);
        kaon_plus->SetEnergy(energy);
      }
      // case for K-
      else if (idpart == -321){ 
        StarGenParticle *kaon_minus = kinematics->AddParticle("K-");
        double px  = vecArr[tr]->Px();
        double py  = vecArr[tr]->Py();
        double pz  = vecArr[tr]->Pz();
        kaon_minus->SetPx(px);
        kaon_minus->SetPy(py);
        kaon_minus->SetPz(pz);
        kaon_minus->SetEnergy(energy);
      }
      // case for pi+
      else if (idpart == 211){
        StarGenParticle *pion_plus = kinematics->AddParticle("pi+");
        double px  = vecArr[tr]->Px();
        double py  = vecArr[tr]->Py();
        double pz  = vecArr[tr]->Pz();
        pion_plus->SetPx(px);
        pion_plus->SetPy(py);
        pion_plus->SetPz(pz);
        pion_plus->SetEnergy(energy);
      }
      // case for pi-
      else if (idpart == -211){
        StarGenParticle *pion_minus = kinematics->AddParticle("pi-");
        double px  = vecArr[tr]->Px();
        double py  = vecArr[tr]->Py();
        double pz  = vecArr[tr]->Pz();
        pion_minus->SetPx(px);
        pion_minus->SetPy(py);
        pion_minus->SetPz(pz);
        pion_minus->SetEnergy(energy);
      }

      // Generate the event

      // Print the event
      // _primary->event()->Print();
    }
    chain->Make();
  }
}
  // ----------------------------------------------------------------------------
  // ----------------------------------------------------------------------------
  // ----------------------------------------------------------------------------
  void Kinematics() {

    //  gSystem->Load( "libStarGeneratorPoolPythia6_4_23.so" );
    gSystem->Load("libKinematics.so");
    kinematics = new StarKinematics();

    _primary->AddGenerator(kinematics);
  }
  // ----------------------------------------------------------------------------
  // ----------------------------------------------------------------------------
  // ----------------------------------------------------------------------------
  void starsim(Int_t nevents = 1, Int_t rngSeed = 1234) {
    FILE *my_file = OpenFile("slight.out");
    gROOT->SetMacroPath(
        ".:/star-sw/StRoot/macros:./StRoot/macros:./StRoot/macros/graphics:./"
        "StRoot/macros/analysis:./StRoot/macros/test:./StRoot/macros/"
        "examples:./StRoot/macros/html:./StRoot/macros/qa:./StRoot/macros/"
        "calib:./StRoot/macros/mudst:/afs/rhic.bnl.gov/star/packages/DEV/"
        "StRoot/macros:/afs/rhic.bnl.gov/star/packages/DEV/StRoot/macros/"
        "graphics:/afs/rhic.bnl.gov/star/packages/DEV/StRoot/macros/analysis:/"
        "afs/rhic.bnl.gov/star/packages/DEV/StRoot/macros/test:/afs/"
        "rhic.bnl.gov/star/packages/DEV/StRoot/macros/examples:/afs/"
        "rhic.bnl.gov/star/packages/DEV/StRoot/macros/html:/afs/rhic.bnl.gov/"
        "star/packages/DEV/StRoot/macros/qa:/afs/rhic.bnl.gov/star/packages/"
        "DEV/StRoot/macros/calib:/afs/rhic.bnl.gov/star/packages/DEV/StRoot/"
        "macros/mudst:/afs/rhic.bnl.gov/star/ROOT/36/5.34.38/"
        ".sl73_x8664_gcc485/rootdeb/macros:/afs/rhic.bnl.gov/star/ROOT/36/"
        "5.34.38/.sl73_x8664_gcc485/rootdeb/tutorials");
    gSystem->Load("libStarRoot.so");
    gROOT->ProcessLine(".L bfc.C");
    {
      TString simple = "y2018a geant gstar usexgeom agml ";
      bfc(0, simple);
    }

    gSystem->Load("libVMC.so");

    gSystem->Load("StarGeneratorUtil.so");
    gSystem->Load("StarGeneratorEvent.so");
    gSystem->Load("StarGeneratorBase.so");

    gSystem->Load("libMathMore.so");
    gSystem->Load("xgeometry.so");

    // Setup RNG seed and map all ROOT TRandom here
    StarRandom::seed(rngSeed);
    StarRandom::capture();

    //
    // Create the primary event generator and insert it
    // before the geant maker
    //
    //  StarPrimaryMaker *
    _primary = new StarPrimaryMaker();
    {
      _primary->SetFileName("kinematics.starsim.root");
      chain->AddBefore("geant", _primary);
    }

    Kinematics();

    //
    // Initialize primary event generator and all sub makers
    //
    _primary->Init();
  _primary->SetSigma( 0.1, 0.1, 0.1 ); // 1mm x 1mm x 1mm smearing at the vertex
  _primary->SetVertex(0.1, 0.1, 0.0 );
    //
    // Setup geometry and set starsim to use agusread for input
    //
    // geometry("y2012");
    command("gkine -4 0");
    command("gfile o kinematics.starsim.fzd");

    //
    // Setup PT and ETA distributions
    //

    //
    // Trigger on nevents
    //
    trig(my_file, nevents);

    command("call agexit"); // Make sure that STARSIM exits properly
  }
  // ----------------------------------------------------------------------------
