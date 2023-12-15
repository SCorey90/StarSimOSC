#!/bin/bash

JOB_DIR=$1
N_EVT=$2
JOB_NO=$3

FZD_FILE="${JOB_DIR}/job${JOB_NO}/slight_${JOB_NO}.fzd"
LOG_FILE="${JOB_DIR}/job${JOB_NO}/log_${JOB_NO}.txt"

export LD_LIBRARY_PATH="/users/PAS2524/corey90/star_mount/.sl79_gcc485/LIB:$LD_LIBRARY_PATH"

echo $N_EVT

echo "Running StarSim" #>> $LOG_FILE
root4star <<EOF
.L /users/PAS2524/corey90/star_mount/starsim/starsim_osc.C
starsim($N_EVT, $JOB_NO, "$JOB_DIR")
.q
EOF

echo "Running Reco Chain" #>> $LOG_FILE
/users/PAS2524/corey90/star_mount/runBFC.sh "$FZD_FILE" "$N_EVT" #>> $LOG_FILE
