#!/bin/csh


###########################################
#
# Main script for MIB batch generation
#
# Usage:
# source launch_GEN.sh p1 p2 p3 p4 p5 p6 
# with:
# p1 : type of input (FLUKA or MARS)
# p2 : global tag for MC production (eg START42_V12)
# p3 : BEAM number (1 or 2)
# p4 : the number of runs (1 is forced for FLUKA)
# p5 : the number of events per run (500000 is forced for FLUKA)
# p6 : BATCH or nothing: launch lxbatch or not
#
# Author: Seb Viret <viret@in2p3.fr>
#
# CMS MIB GEN page: 
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIBGen
# 
###########################################



###################################
#
# The list of parameters you can modify is here
#
###################################

# Filenames if you choose FLUKA (don't have to precise with MARS):
#
# beam-halo_3.5TeV-R5: BEAM1 halo events
# beam-halo_3.5TeV-L5: BEAM2 halo events
# beam-gas_IR5       : BEAM1 beam gas inelastic events
#

set FILENAME     = "beam-gas_IR5"                # Input filename for FLUKA 
set PARTICLES    = ALL                           # Which particles (replace by PDG_ID is necessary)
set MIN_NRJ      = 0.02                          # Minimal energy of generated particles, in GeV
set MIN_PT       = 0.0                           # Minimal transverse mom of gen. particles, in GeV/c
set STORAGEDIR   = $CASTOR_HOME/CMS/MIB/GEN/Prod # Where are you storing the generated ROOTuples

###########################################################
###########################################################
# You are not supposed to touch the rest of the script !!!!
###########################################################
###########################################################


set TYPE         = ${1}           # Input data type (MARS or FLUKA) 
set GTAG         = ${2}"::All"    # Global tag
set BEAM         = ${3}           # Which beam to simulate
set N_RUN        = ${4}           # Number of samples 
set EVTS_PER_RUN = ${5}           # Number of events per sample

# With FLUKA, we have N_RUN = 1 (events are not picked up randomly) 
if ($TYPE == "FLUKA") then
	set N_RUN        = 1 
        set EVTS_PER_RUN = 500000 
endif  


cd  ..
set PACKDIR      = $PWD           # This is where the package is installed 
cd  ../..
set RELEASEDIR   = $PWD           # This is where the release is installed

cd $PACKDIR/batch

if ($TYPE != "MARS" && $TYPE != "FLUKA") then
    echo "Wrong type: precise MARS or FLUKA"
    exit
endif

echo $N_RUN

@ i = 0

# Finally we create the batch scripts and launch the jobs

while ($i != $N_RUN)
    echo $i
    @ i++

    set OUTPUTDIR = $STORAGEDIR/$TYPE
    set OUT_NAME  = "MIB_gen_"$TYPE"_BEAM_"$BEAM"_E_"$MIN_NRJ"_PT_"$MIN_PT"_"$i".root"  

    echo "#\!/bin/bash" > gen_job_${TYPE}_${BEAM}_${i}.sh
    echo "source $PACKDIR/batch/generator.sh $EVTS_PER_RUN $PARTICLES $BEAM $MIN_NRJ $MIN_PT $TYPE $GTAG $FILENAME $i $RELEASEDIR $PACKDIR $OUTPUTDIR" >> gen_job_${TYPE}_${BEAM}_${i}.sh
    chmod 755 gen_job_${TYPE}_${BEAM}_${i}.sh

    if ($TYPE == "FLUKA") then
	stager_get -M  /castor/cern.ch/user/s/sviret/CMS/MIB/Input_FLUKA/${FILENAME}.gz
	if (${4} == "BATCH") then
	    bsub -q 1nw -e /dev/null -o /tmp/${LOGNAME}_out.txt gen_job_${TYPE}_${BEAM}_${i}.sh
	endif
    endif

    if ($TYPE == "MARS") then
	stager_get -M  /castor/cern.ch/user/s/sviret/CMS/MIB/Input_MARS/data_MARS.tar.gz
	if (${4} == "BATCH") then
	    bsub -q 1nd -e /dev/null -o /tmp/${LOGNAME}_out.txt gen_job_${TYPE}_${BEAM}_${i}.sh
	endif
    endif
end
