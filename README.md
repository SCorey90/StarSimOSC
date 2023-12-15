
Below is an in-progress list of instructions for running STAR simulation using OSC, assuming you can run starlight locally. Note that I encountered issues running  a```split``` command on OSC so I did it locally. These are steps that should work

1. Run starlight locally on however many events you want -- you should get an ```slight.out``` file
2. Make a directory where the job files will go using the following code. This will create a directory of jobfiles, and then split the slight.out files into reasonable size chunks for OSC jobs (4000 is the number of lines, which is 1000 events)
``` bash
mkdir /path_to_docker_mount/JobFiles 
mv slight.out /path_to_docker_mount/JobFiles 
split -l 4000 -d -a 4 slight.out slight_
```
3. Then run the following code snippet inside the JobFiles directory: This will make a directory for each job with part of the slight.out file for a reasonable amount of events
```bash 
for f in slight_* 
do mkdir job${f#${f%%[1-9]*}} 
mv $f job${f#${f%%[1-9]*}}/test_${f#${f%%[1-9]*}}.out 
done
```
4. Once these are created we need to get these files onto OSC. Assuming you already have an OSC account, navigate to the OSC onDemand page. Select the "Clusters" tab on the top and you can use Owens Shell or Pitzer Shell. This will open an interactive shell in your browser. Once in this shell, create a mount directory for star simulation. I called this star_mount. Then in your local shell from your docker_mount directory (or wherever your job files and starlight files are), run the following code snippet.
```bash 
sftp your_username@sftp.osc.edu
```
5. Once connected, run the following, which will transfer over your local files from your docker_mount directory to OSC
```bash
put -r *
```

6. All necessary files are in the shared directory on OSC in
```bash
/fs/scratch/PAS2524
```
7. In your home directory in the OSC shell, use apptainer to pull the container using 
```bash 
apptainer pull docker://ghcr.io/star-bnl/star-sw:SL23f_0-root5-gcc11
```
8. Once built, you will get an output file that ends in  `.sif`. Once you do, running `./file_name.sif` will start the container.
9. Inside the container in the directory with the `bfcRunner.C` file, run `cons`. This builds the necessary items for reconstruction
10. Finally, before you submit jobs, make sure you have `SubmitFile.sh`, `StartShell.sh`, `star_mount/run_starsim+reco.sh`, and `star_mount/runBFC.sh` with the paths changed to reflect your own directory path. you can also specify number of jobs, events per job, and cpu time per job inside `SubmitFile.sh`
11. Once ready to submit, inside your OSC `~` directory run `sbatch SubmitFile.sh` and wait for the output.