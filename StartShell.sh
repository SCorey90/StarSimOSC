#!/bin/bash

JOB_DIR=$1
N_EVT=$2
JOB_NUM=$3

export WORKING_DIR=/users/PAS2524/corey90

$WORKING_DIR/star_root5-gcc485.sif <<EOF
$WORKING_DIR/star_mount/run_starsim+reco.sh $JOB_DIR $N_EVT $JOB_NUM
EOF
