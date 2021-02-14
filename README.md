# BRIL_BIBGenerator
>UNDER CONSTRUCTION
## Introduction
This guide gives instructions on how to set up and run Beam Induced Background (BIB, alternatively MIB for Machine Induced Background) simulations in CMSSW with a two step method (BIB particle generation + simulation).
For an official introduction and manual for CMSSW have a look at the offline workbook: 
https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBook

## Setup
# Quickstart
BIB generation:
```sh
ssh -Y username@lxplus.cern.ch
mkdir cmssw_test
cd cmssw_test
cmsrel CMSSW_11_2_0_pre6
cd CMSSW_11_2_0_pre6/src
git clone https://github.com/pkicsiny/BRIL_BIBGenerator.git
ln -s BRIL_BIBGenerator/GeneratorInterface GeneratorInterface
wget https://raw.githubusercontent.com/pkicsiny/BRIL_ITsim/master/BIBGeneration/python/BH_generator.py
scram b
mkdir generator_output
vi BH_generator.py
line 44: path/to/generator_output
line 54: path/to/input
cmsRun BH_generation.py 
```

Simulation:
```sh
mkdir simulation_output
wget https://raw.githubusercontent.com/pkicsiny/BRIL_ITsim/master/BIBGeneration/python/BH_SimTrigRec.py
wget https://raw.githubusercontent.com/pkicsiny/BRIL_ITsim/master/BIBGeneration/runSimTkOnly_local.sh
line 28: path/to/generator/output/myoutput.root
line 29: path/to/simulation_output

```
## In details
Login to _lxplus_.
```sh
ssh -Y username@lxplus.cern.ch
```
If you don't have any CMSSW release already, create and navigate to an empty directory
```sh
mkdir cmssw_test
cd cmssw_test
```
and clone a CMSSW release by
```sh
cmsrel CMSSW_11_2_0_pre6
```
which creates a local copy of CMSSW version 11.2.pre6. You can alternatively work with another version, but it is always recommended to use one of the newest ones (currently as of version 11). To find out about a list of currently available CMSSW releases, type
```sh
scram list -a
```
or visit the official [CMSSW github](https://github.com/cms-sw/cmssw) page, where you can also browse the simulation source code. Now you can navigate to the source directory within the created CMSSW release
```sh
cd CMSSW_11_2_0_pre6/src
```
and activate a CMSSW working environment, paths and compiler (while being in the /src directory) by typing
```sh
cmsenv
```
This command has to be issued only once but every time you open up a new terminal and start to work with CMSSW. In the _/src_ directory you can now clone this repository that will be used for BIB generation:
```sh
git clone https://github.com/pkicsiny/BRIL_BIBGenerator.git
```
CMSSW can only find and work with the code if it is located in the /src directory. Therefore the subdirectory BRIL_BIBGenerator/GeneratorInterface should be symlinked from the _/src_ directory as shown below:
```sh
ln -s BRIL_BIBGenerator/GeneratorInterface GeneratorInterface
```
In addition, you will need to clone a config file for the generation and simulation steps respectively. These config files will be used to launch CMSSW and can be found [here](https://github.com/pkicsiny/BRIL_ITsim/tree/master/BIBGeneration/python). You can simply use [wget](https://www.gnu.org/software/wget/manual/wget.html) to clone the 2 config files into your _/src_ directory. Type the following while being in _/src_
```sh
 wget https://raw.githubusercontent.com/pkicsiny/BRIL_ITsim/master/BIBGeneration/python/BH_generator.py
```
to get the config file for the generation step.

#### Getting the input
Some FLUKA simulated BIB files for beam halo and three types on beam gas (H, C and O) can be downloaded from [here](https://bbgen.web.cern.ch/HL-LHC/). It is recommended to place these files to the _/eos_ file system because of their large size. Alternatively you can use to experiment with the input files that are used by the config file by default (see at line 54) <br>

#### Generation step
Before running the generation step, you need to compile and build your C++ files in the _BRIL_BIBGenerator_. This is done by
```sh
scram b
```
and it is also recommended t ostore the output of the generatio nstep in a dedicated folder.
```sh
mkdir generator_output
```
Before launching the BIB generation, you might want to have a look at the contencts of the config file.
```sh
vi BH_generation.py
```
(You might use the tab key for auto-filling the path name when navigating in the terminal.) The config file accepts the following user parameters from the command line: <br>
__nEvents__: number of events to read from the FLUKA input files. Can be set inside the config file through the _nevents_ variable. If set to -1, all events from the inputs will be processed. Note that this is not recommended as CMSSW will throw an error when reaching the end of the last file. Instead, it is recommended to check the number of events input file beforehand and set this parameter accordingly, e.g. to 10000 if the inputs contain let's say 10426 events. <br>
__nThreads__: number of parallel computing threads to use. Default is 1. <br>
__jobId__: relevant when running the generation step on lxbatch, where the simulation is split into smaller chunks each having a unique job ID. Not used if the code is run locally on lxplus. Default is 0. <br>
__tDirectory__: absolute path of the diractory to where the root file will be created. It will contain the same particles as in the FLUKA dump input, but in a different, CMSSW friendly format, called [HEPMC](http://www.t2.ucsd.edu/twiki2/bin/view/HEPProjects/HepMCReference). <br>

In addition, some other parameters can be specified inside the config file: <br>
__inputPath__: absolute path specifying the location of the FLUKA input files. Also do not forget the _file:_ prefix before the path! The line _options.inputFiles= [inputPath + "/" + f for f in os.listdir(inputPath) if f[:3] == "run"]_ automatically parses the file names in the __inputPath__ directory and selects files whose name begins with _run_. This parsing can also be adapted or removed depending on the specific usecase. <br>
__nevents__: number of events, alternative hard-coded variant of __nEvents__. <br>
The output file name can be changed under the _#specify output name_ comment. <br>

Although the CMS geometry plays no role in the generation step, CMSSW always expects an input geometry to be specified. You can use the most up to date Phase 2 full CMS geometry, just as a 'placeholder':
```sh
process.load('Configuration.Geometry.GeometryExtended2026D63_cff')
```
You can run the generation step by running the cofig file _BH_generation.py_.
```sh
cmsRun BH_generation.py 
```
which invokes code from _BRIL_BIBGenerator/GeneratorInterface/BeamHaloGenerator/python/MIB_generator_cff.py_ which in turn invokes CMSSW through _BRIL_BIBGenerator/GeneratorInterface/BeamHaloGenerator/src/BeamHaloProducer.cc_. <br>

#### Simulation
The second step consists of the transport of paticles and the simulation of particle-matter interactions in the CMSSW geometry model. The code for this step is created for running on lxbatch as simulating a large number of generated MIB particles might take a long time. First let's get the 3 necessary config files:
```sh
wget https://raw.githubusercontent.com/pkicsiny/BRIL_ITsim/master/BIBGeneration/generatePU.sub
wget https://raw.githubusercontent.com/pkicsiny/BRIL_ITsim/master/BIBGeneration/runSimTkOnly.sh
wget https://raw.githubusercontent.com/pkicsiny/BRIL_ITsim/master/BIBGeneration/python/BH_SimTrigRec.py
```
The first file (_generatePU.sub_) will be used to submit some files to the lxbatch cluster to do the simulation. Line 1 can be ignored for MIB studies and line 2 specifies the number of MIB events to simulate. The rest of the file can be left as it is, except the last line, where you can define the number of "jobs" to submit and queue on the cluster. In order to run the most efficiently, lxbatch splits up the simulation into smaller chunks or sub-simulations (=jobs) and runs them in parallel. Depending on the number of events you intend to simulate, you can change the queue parameter. For example if at line 2 __NEvents__ is set to 200000, you can set the __queue___ to 40, that tells lxbatch to split up the simulation into 40 jobs each simulating only 200000/40=50000 events. <br>
The bash script file _runSimTkOnly.sh_ is the highest level file controlling the simulation and invokes the python config file _BH_SimTrigRec.py_. In _runSimTkOnly.sh_, the __INFILE__ parameter (currently at line 30) has to be set to point to the output root file from the generation step (e.g. at _/absolute/path/to/cmssw_test/CMSSW_11_2_0_pre6/src/generator_output/myoutput.root_). Line 36 should be similarly changed to point to the desired simulation output directory (e.g. to _/absolute/path/to/cmssw_test/CMSSW_11_2_0_pre6/src/simulation_output_). In general, nothing else has to be changed in this file. You can notice that it takes some command line arguments (lines 17-20): <br> 
__PU__: refers to pileup but it can be ignored for BIB particle simulations. <br>
__NEVENTS__: number of events to read from the generator step output file and simulate. This parameter is inferred from line 1 of _generatePU.sub_. <br>
__JOBID__: this can also be ignored. It wil be set automatically on lxbatch and can take a value from 0 to the value of __queue__, which is set in the last line of _generatePU.sub_. The __JOBID__ parameter uniquely identifies each split simulation chunk on the cluster. <br>
At line 100 you can also see:
```sh
tar -xf sandbox.tar.bz2
```
which refers to an important step in setting up the simulations on the cluster. By default, simulations on lxbatch do not have acces to CMSSW code stored locally on lxplus, therefore we need to send them to lxbatch along with the config files, wrapped up in a tar file, which will be automatically unpacked and used on the cluster. To create _sandbox.tar.bz2_, first exit the _CMSSW_11_2_0_pre6_ folder and create a temporary directory
```sh
cd ../../
mkdir sandbox
cd sandbox
```
and copy here the full CMSSW_11_2_0_pre6 directory. Now enter the _CMSSW_11_2_0_pre6/src_ and delete everything except _BRIL_BIBGenerator/GeneratorInterface/SimG4Core_ which contains code for handling MIB particles in CMSSW and thus necessary for such simulations. To read more about what is different when simulating MIB particles instead of regular beam collision events, see [here](https://sviret.web.cern.ch/sviret/Images/CMS/MIB/MIB/Welcome.php?n=Work.Prod).
In this file you can specify the input at line 28, pointing to the output root file of the generation step, and the output at line 29, for which it is again recommended to create a separate directory:
```sh
mkdir simulation_output
```

#### Running on lxbatch
