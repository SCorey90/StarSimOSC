#!/bin/bash

echo "Running BFC on $1"
echo "Nevents=$2"


# version based on P2010a chain

#root4star -b -q -l 'bfcRunner.C( '$2', "DbV20101213 P2010a pmdReco btofSim btof BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D VFMCE TpxClu -VFMinuit -hitfilt -in gen_T,geomT,sim_T,TpcRS,-ittf,-tpc_daq,nodefault", "'$1'")'


#bottom up, working very fast but no MTD info in output
# root4star -b -q -l 'bfcRunner.C( 5, "fzin, y2014a UseXgeom AgML StarMagField FieldOn TpcFastSim VFMinuit ITTF tpcDB MakeEvent vpdSim btofSim btofMixer btofMatch McEvent IdTruth McAss CMuDst", "'$1'" )'



#works with TpcFastSim
#root4star -b -q -l 'bfcRunner.C( '$2', "y2010 fzin TpcFastSim ITTF  McEvOut IdTruth McAna tpc_T globT db tpcDB VFMCE Idst event analysis EventQA EvOut StarMagField FieldOn IAna CMuDst vpdSim btofSim btofMixer btofMatch McAss", "'$1'" )'

 root4star -b -q -l 'bfcRunner.C( '$2', "y2017 fzin TpcRS TpxClu TpcHitMover ITTF McEvOut IdTruth McAna tpc_T globT db tpcDB VFMCE Idst event analysis EventQA EvOut StarMagField FieldOn IAna CMuDst vpdSim btofSim btofMixer btofMatch McAss", "'$1'" )'

#root4star -b -q -l 'bfcRunner.C( '$2', "y2014a fzin TpcFastSim Simu sfs ssdfast McEvOut Sti IdTruth McAna tpc_T globT tls db tpcDB ssdIT ITTF VFMinuit Idst event analysis EventQA tags EvOut StarMagField FieldOn IAna CMuDst vpdSim btofSim btofMixer btofMatch McAss", "'$1'" )'




# workign for MTD Sim
# root4star -b -q -l 'bfcRunner.C( '"$2"', "DbV20150316 y2014a ReverseField fzin mtdSim tpcX AgML tpcDB TpcHitMover Idst ITTF UseXgeom BAna VFMinuit l3onl emcDY2 fpd trgd ZDCvtx btof mtd mtdCalib BEmcChkStat CorrX OSpaceZ2 OGridLeak3D tpcrs TpxRaw VFMCE TpxClu big MakeEvent IdTruth McEvent McAss", "'$1'" )'





# root4star -b -q -l 'bfc.C( 5, "DbV20160418 y2015b fzin mtdSim tpcX AgML tpcDB TpcHitMover Idst Tree ITTF UseXgeom BAna VFMinuit l3onl emcDY2 fpd trgd ZDCvtx  analysis pxlHit istHit btof mtd mtdCalib BEmcChkStat CorrX OSpaceZ2 OGridLeak3D tpcrs TpxRaw  VFMCE TpxClu big MakeEvent evout GeantOut IdTruth", "'$1'" )'

# only working one
# root4star -b -q -l 'bfcRunner.C( '"$2"', "DbV20150316 y2014a fzin mtdSim tpcX AgML tpcDB TpcHitMover Idst Tree ITTF UseXgeom BAna VFMinuit l3onl emcDY2 fpd trgd ZDCvtx analysis pxlHit istHit btof mtd mtdCalib BEmcChkStat CorrX OSpaceZ2 OGridLeak3D tpcrs TpxRaw  VFMCE TpxClu big MakeEvent evout GeantOut IdTruth McEvent McAss btofMixer", "'$1'" )'



# root4star -b -q -l 'bfcRunner.C( 5, " DbV20150316 y2014a fzin TpcRS TpxClu useXgeom MakeEvent NoIstIT NoPxlIT NoSstIT NoSsdIt NoSvtIt Idst BAna VFMinuit tpcDB TpcHitMover bbcSim mtdSim mtd mtdCalib tags emcY2 EEfs evout IdTruth geantout big evout GeantOut McEvent McAss", "'$1'" )'

# fzin TpcRS TpxClu y2014 useXgeom MakeEvent NoSsdIt NoSvtIt Idst BAna VFMinuit tpcDB TpcHitMover bbcSim mtdSim btofsim btofMatch mtd mtdCalib tags emcY2 EEfs evout IdTruth geantout big evout GeantOut McEvent McAss



# very fast chain based on Test.VeryFast.StiVMC -- not working
# root4star -b -q -l 'bfcRunner.C( 5, "fzin y2014a TpcFastSim Simu sfs ssdfast McEvOut GeantOut IdTruth McAna SvtCL tpc_T globT tls db tpcDB svtDb svtIT ssdIT StiVMC Idst event analysis mtdSim mtd mtdCalib EventQA tags EvOut StarMagField FieldOn IAna CMuDst GeantOut McEvent McAss", "'$1'")'


# very fast based on Test.VeryFast.ITTF 
# root4star -b -q -l 'bfcRunner.C( 5, "fzin y2014a mtdSim vpdSim btofSim UseXgeom AgML TpcFastSim Simu sfs ssdfast McEvOut GeantOut IdTruth McAna SvtCL VFMinuit tpc_T globT tls db tpcDB svtDb svtIT ssdIT ITTF Idst event TpcHitMover btof mtd mtdCalib EvOut StarMagField FieldOn IAna GeantOut McEvent McAss", "'$1'")'




# bottom up chain
# root4star -b -q -l 'bfcRunner.C( '"$2"', "DbV20150316 fzin y2014a UseXgeom hitfilt trgd AgML StarMagField FieldOn TpcFastSim VFMinuit ITTF tpcDB MakeEvent McEvent IdTruth McAss", "'$1'" )'


