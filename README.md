# BRIL_BIBGenerator
>UNDER CONSTRUCTION
### Introduction
This guide gives instructions on how to set up and run Beam Induced Background (BIB, alternatively MIB for Machine Induced Background) simulations in CMSSW with a two step method (BIB particle generation + simulation).
For an official introduction and manual for CMSSW have a look at the offline workbook: 
https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBook

## Setup
Login to _lxplus_.
```sh
ssh -Y username@lxplus.cern.ch
```
If you don't have any CMSSW release already, navigate to an empty directory and then type
```sh
cmsrel CMSSW_11_2_0_pre6
````
which creates a local copy of CMSSW version 11.2.pre6. To find out about a list of currently available CMSSW releases, type
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
This command has to be issued only once but every time you open up a new terminal an start to wotk with CMSSW. In the _/src_ directory you can now clone the current repository that will be used for BIB generation:
```sh
git clone https://github.com/pkicsiny/BRIL_BIBGenerator.git
```
CMSSW can only find and work with the code if it is located in the /src directory. Therefore the subdirectory BRIL_BIBGenerator/GeneratorInterface should be symlinked from the _/src_ directory as shown below:
```sh
ln -s BRIL_BIBGenerator/GeneratorInterface GeneratorInterface
```
In addition, you will need to clone another repository that contains two config files which will be used to run the generation and simulation step respectively. This can be found [here](https://github.com/pkicsiny/BRIL_ITsim/tree/master/BIBGeneration). You can simply clone the whole repository but we will need only the contents of the _BIBGeneration_ directory. Type the following while being in the _/src_ directory:
```sh
git clone https://github.com/pkicsiny/BRIL_ITsim.git
```

#### Getting the input
Some FLUKA simulated BIB files for beam halo and three types on beam gas (H, C and O) can be downloaded from https://bbgen.web.cern.ch/HL-LHC/. It is recommended to place these files to the _/eos_ file system because of their large size. <br>

#### Generation step
Before running the generation step, you might want to have a look at the contencts of the config file.
```sh
cd BRIL_ITsim/BIBGeneration
vi python/BH_generation.py
```
(You might use the tab key for auto-filling the path name when navigating in the terminal.) The config file accepts the following user parameters from the command line: <br>
__nEvents__: number of events to read from the FLUKA input files. Can be set inside the config file through the _nevents_ variable. If set to -1, all events from the inputs will be processed. Note that this is not recommended as CMSSW will throw an error when reaching the end of the last file. Instead, it is recommended to check the number of events input file beforehand and set this parameter accordingly, e.g. to 10000 if the inputs contain let's say 10426 events. <br>
__nThreads__: number of parallel computing threads to use. Default is 1. <br>
__jobId__: relevant when running the generation step on lxbatch, where the simulation is split into smaller chunks each having a unique job ID. Not used if the code is run locally on lxplus. Default is 0. <br>
__tDirectory__: absolute path of the diractory to where the root file will be created. It will contain the same particles as in the FLUKA dump input, but in a different, CMSSW friendly format, called [HEPMC](http://www.t2.ucsd.edu/twiki2/bin/view/HEPProjects/HepMCReference). <br>

In addition, some other parameters can be specified inside the config file: <br>
__inputPath__: absolute path specifying the location of the FLUKA input files. The line _options.inputFiles= [inputPath + "/" + f for f in os.listdir(inputPath) if f[:3] == "run"]_ automatically parses the file names in the __inputPath__ directory and selects files whose name begins with _run_. This parsing can also be adapted or removed depending on the specific usecase. <br>
__nevents__: number of events, alternative hard-coded variant of __nEvents__. <br>
The output file name can be changed under the _#specify output name_ comment. <br>

Although the CMS geometry plays no role in the generation step, CMSSW always expects an input geometry to be specified. You can use the most up to date Phase 2 full CMS geometry, just as a 'placeholder':
```sh
process.load('Configuration.Geometry.GeometryExtended2026D63_cff')
```
You can run the generation step by running the cofig file _BH_generation.py_.
```sh
cmsRun python/BH_generation.py 
```
which invokes code from _BRIL_BIBGenerator/GeneratorInterface/BeamHaloGenerator/python/MIB_generator_cff.py_ which in turn invokes CMSSW through _BRIL_BIBGenerator/GeneratorInterface/BeamHaloGenerator/src/BeamHaloProducer.cc_. <br>

#### Simulation
The second step consists of the transport of paticles and the simulation of particle-matter interactions in the CMSSW geometry model. This step can be launched by executing:
`cmsRun BH_SimTrigRec.py` <br>

#### Running on lxbatch
