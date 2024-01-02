#!/bin/bash

echo "Running BFC on $1"
echo "Nevents=$2"

root4star -b -q -l ''$WORKING_DIR'/star_mount/bfcRunner.C( '$2', "y2017 fzin TpcRS TpxClu TpcHitMover ITTF McEvOut IdTruth McAna tpc_T globT db tpcDB VFMCE Idst event analysis EventQA EvOut StarMagField FieldOn IAna CMuDst vpdSim btofSim btofMixer btofMatch McAss", "'$1'" )'
