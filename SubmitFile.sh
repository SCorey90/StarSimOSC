#!/bin/bash
#SBATCH --job-name=StarSimArray
#SBATCH --account=PAS2524
#SBATCH --time=00:10:00
#SBATCH --array=5-6
#SBATCH --output=/users/PAS2524/corey90/LogFiles/log.txt

/users/PAS2524/corey90/StartShell.sh /users/PAS2524/corey90/star_mount/JobFiles 1 $SLURM_ARRAY_TASK_ID

