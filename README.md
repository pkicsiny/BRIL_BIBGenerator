# BRIL_BIBGenerator
## Introduction
This guide gives instructions on how to set up and run Beam Induced Background (BIB, alternatively MIB for Machine Induced Background) simulations in CMSSW with a two step method (BIB particle generation + simulation).
For an official introduction and manual for CMSSW have a look at the offline [workbook](https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBook).

## Setup
### Quickstart
Generation:
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
set output path at line 44: path/to/generator_output
set input files location at line 54: path/to/input
cmsRun BH_generation.py 
```

Simulation:
```sh
mkdir simulation_output
wget https://raw.githubusercontent.com/pkicsiny/BRIL_ITsim/master/BIBGeneration/generatePU.sub
wget https://raw.githubusercontent.com/pkicsiny/BRIL_ITsim/master/BIBGeneration/runSimTkOnly.sh
wget https://raw.githubusercontent.com/pkicsiny/BRIL_ITsim/master/BIBGeneration/python/BH_SimTrigRec.py
mkdir batchlog
vi runSimTkOnly.sh
set input files location at line 30: path/to/generator/output/myoutput.root
set output path at line 36: path/to/simulation_output
vi generatePU.sub
set number of MIB events to simulate at line 2
set relative path of BH_SimTrigRec.py at line 10
set number of parallel splittings at line 18
vi BH_SimTrigRec.py
set number of events processed in each chunk (total number of events/queue) at line 103
change the geometry according to the usecase at lines 76-85
cd ../../
mkdir temp
cd temp
cp ../CMSSW_11_2_0_pre6 .
cd CMSSW_11_2_0_pre6/src
delete everything except the SimG4Core folder
git clone https://github.com/pkicsiny/BRIL_ITsim.git
Delete everything from the above repository except the DataProductionTkOnly folder
cd ../../
tar -czvf sandbox.tar.bz2 CMSSW_11_2_0_pre6
cp sandbox.tar.bz2 ../CMSSW_11_2_0_pre6/src/
cd ../CMSSW_11_2_0_pre6/src/
condor_submit generatePU.sub
```
### In details
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

##### Getting the input
Some FLUKA simulated BIB files for beam halo and three types on beam gas (H, C and O) can be downloaded from [here](https://bbgen.web.cern.ch/HL-LHC/). It is recommended to place these files to the _/eos_ file system because of their large size. Alternatively you can use to experiment with the input files that are used by the config file by default (see at line 54) <br>

##### Generation step
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

#### Simulation step
The second step consists of the transport of paticles and the simulation of particle-matter interactions in the CMSSW geometry model. The code for this step is created for running on lxbatch as simulating a large number of generated MIB particles might take a long time. First let's get the 3 necessary config files:
```sh
wget https://raw.githubusercontent.com/pkicsiny/BRIL_ITsim/master/BIBGeneration/generatePU.sub
wget https://raw.githubusercontent.com/pkicsiny/BRIL_ITsim/master/BIBGeneration/runSimTkOnly.sh
wget https://raw.githubusercontent.com/pkicsiny/BRIL_ITsim/master/BIBGeneration/python/BH_SimTrigRec.py
```
#### generatePU.sub
The first file (_generatePU.sub_) will be used to submit some files to the lxbatch cluster to do the simulation, using [HTCondor](https://batchdocs.web.cern.ch/local/quick.html). Line 1 can be ignored for MIB studies and line 2 specifies the number of MIB events to simulate. The rest of the file can be left as it is, except the last line, where you can define the number of "jobs" to submit and queue on the cluster. In order to run the most efficiently, lxbatch splits up the simulation into smaller chunks or sub-simulations (=jobs) and runs them in parallel. Depending on the number of events you intend to simulate, you can change the queue parameter. For example if at line 2 __NEvents__ is set to 200000, you can set the __queue___ to 40, that tells lxbatch to split up the simulation into 40 jobs each simulating only 200000/40=50000 events. This splitting is performed by selecting a subset of the events at runtime in the _BH_SimTrigRec.py_ config file at lines 103-105 (currently set to 5000 events but it has to be modified accordingly to __NEvents__/__queue__). Lines 7-9 _generatePU.sub_ in define the path where the simulation output, error and log files will be saved. Currently it is set to be saved in a _batchlog_ folder which you can either change or create the _batchlog_ directory in _/src_.
```sh
mkdir batchlog
```
At line 10 you can see that 2 files will be transferred to the cluster if the simulations is launched. The bash script file _runSimTkOnly.sh_ is the highest level file controlling the simulation and invokes the python config file _BH_Rec.py_. Its path is now set to _python/runSimTkOnly.sh_ but it should be changed accordingly, i.e. in this case just remove the _python_ part.

#### BH_SimTrigRec.py
In this config file you can change the CMSSW geometry used for the simulation at lines 76-85. As opposed to the generation step, in the simulation step this config will not be launched directly, but instead it is invoked by _runSimTkOnly.sh_, which is in turn invoked by _generatePU.sub_.

#### runSimTkOnly.sh
In _runSimTkOnly.sh_, the __INFILE__ parameter (currently at line 30) has to be set to point to the output root file from the generation step (e.g. at _/absolute/path/to/cmssw_test/CMSSW_11_2_0_pre6/src/generator_output/myoutput.root_). The parameter __OUTDIR__ at line 36 should be similarly changed to point to the desired simulation output directory (e.g. to _/absolute/path/to/cmssw_test/CMSSW_11_2_0_pre6/src/simulation_output_). In general, nothing else has to be changed in this file. You can notice that it takes some command line arguments (lines 17-20): <br> 
__PU__: refers to pileup but it can be ignored for BIB particle simulations. <br>
__NEVENTS__: number of events to read from the generator step output file and simulate. This parameter is inferred from line 1 of _generatePU.sub_. <br>
__JOBID__: this can also be ignored. It wil be set automatically on lxbatch and can take a value from 0 to the value of __queue__, which is set in the last line of _generatePU.sub_. The __JOBID__ parameter uniquely identifies each split simulation chunk on the cluster. <br>
At line 100 you can also see:
```sh
tar -xf sandbox.tar.bz2
```
which refers to an important step in setting up the simulations on the cluster. 

#### Setting up a CMSSW sandbox
By default, simulations on lxbatch do not have acces to CMSSW code stored locally on lxplus, therefore we need to send them to lxbatch along with the config file, wrapped up in a tar package, which will be automatically unpacked and used on the cluster. To create _sandbox.tar.bz2_, first exit the _CMSSW_11_2_0_pre6_ folder and create a temporary directory
```sh
cd ../../
mkdir temp
cd temp
```
and copy here the full CMSSW_11_2_0_pre6 directory. Now enter the _CMSSW_11_2_0_pre6/src_ and delete everything except _BRIL_BIBGenerator/GeneratorInterface/SimG4Core_ which contains code for handling MIB particles in CMSSW and thus necessary for such simulations. To read more about what is different when simulating MIB particles instead of regular beam collision events, see [here](https://sviret.web.cern.ch/sviret/Images/CMS/MIB/MIB/Welcome.php?n=Work.Prod). Then move _SimG4Core_ back up to the _/src_ directory so that now that is the only content there. Then you will need some code for simulating detector response and digitization, which that can depend on the usecase. In casew of BRIL, simulations were done with having only the tracker geometry. To get the necessary python files for this usecase, get the directory _DataProductionTkOnly_ from [here](https://github.com/pkicsiny/BRIL_ITsim). The easiest (and dirtiest) way is to clone the whole repository and delete everything except the _DataProductionTkOnly_ folder.
```sh
git clone https://github.com/pkicsiny/BRIL_ITsim.git
```
In the end you should have only _SimG4Core_ (MIB specific code) and _DataProductionTkOnly_ (detector specific code) directories in your copy of _CMSSW_11_2_0_pre6/src_. If so, move back up into your _temp_ directory and compress your CMSSW release by
```sh
tar -czvf sandbox.tar.bz2 CMSSW_11_2_0_pre6
```
The resulting _sandbox.tar.bz2_ you can now copy to your original _CMSSW_11_2_0_pre6/src/_ directory.
```sh
cp sandbox.tar.bz2 ../CMSSW_11_2_0_pre6/src/
```
Finally, use
```sh
condor_submit generatePU.sub
```
to submit the simulation to lxbatch. The result root files will be placed to the path which was set in line 36 of _runSimTkOnly.sh_.
