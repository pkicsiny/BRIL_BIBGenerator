import FWCore.ParameterSet.Config as cms


#
# This config file provides different sequences for machine induced background simulation
#

#
# 1. MARS input (7 TeV per beam)
#
# Data files are all read and used to produce a binary buffer
# So you don't need to precise the file names, but you need to have them available where
# CMSSW is running (into a InputFile directory)
#
# Easiest way to do that (in the directory where you run):
# 
# mkdir InputFiles
# cd InputFiles
# xrdcp root://castorcms//castor/cern.ch/user/s/sviret/CMS/MIB/Input_MARS/data_MARS.tar.gz .
# tar -zxf data_MARS.tar.gz
# cmsRun YOURJOB.py
#


MARS_generator = cms.EDProducer("BeamHaloProducer",

    InputType = cms.string('MARS'),
    InputFile = cms.vstring(''),

    # Settings for the beam halo generator
    #
    # Only positive value should be given
    #
    # Example: you want to keep |x| values between 5mm and 100mm, do:
    #  
    # "xLimits      5 100"
    #
    # This will select events with 5<x<100, but also -100<x<-5

    #
    # For MARS these settings will be used to create the buffer
    #

    generatorSettings = cms.untracked.vstring(
    #    "allowedPdgId 13 -13",  # The PDG IDs of the allowed particles
    "BEAM 1",                    # Beam number to generate (DEFAULT is 1)
    "SEED 1",                    # Input val for the seed  (DEFAULT is 1)    
    "pxLimits     -1 -1",        # x momentum (in GeV)
    "pyLimits     -1 -1",        # y momentum (in GeV)
    "pzLimits     -1 -1",        # z momentum (in GeV) 
    "energyLimits 0.02 -1",      # energy (in GeV)
    "xLimits      -1 -1",        # x position (in mm)
    "yLimits      -1 -1",        # y position (in mm)
    "ptLimits     -1 -1",
    "phiLimits    -1 -1",
    "etaLimits    -1 -1",
    "rLimits      -1 -1",
    "weightLimits -1 -1")

)



#
# 2. FLUKA input (3.5 TeV per beam)
#
# Data files are just read out here, no buffer needed
# You should precise the file name you want and store it where
# CMSSW is running (into a InputFile directory)
#
# Easiest way to do that (in the directory where you run):
# 
# xrdcp root://castorcms//castor/cern.ch/user/s/sviret/CMS/MIB/Input_FLUKA/filename.gz .
# gunzip filename.gz
# cmsRun YOURJOB.py
#
# Where filename can be:
#
# beam-halo_3.5TeV-R5: BEAM1 halo events
# beam-halo_3.5TeV-L5: BEAM2 halo events
# beam-gas_IR5       : BEAM1 beam gas inelastic events


print("Creating FLUKA generator...")
FLUKA_generator = cms.EDProducer("BeamHaloProducer",

    InputType = cms.string('FLUKA'),
    #InputFile = cms.string('beam-halo_3.5TeV-R5'),
    InputFile = cms.string('beam-gas_IR7'),
    FlukaFiles = cms.vstring("test"),
    #InputFile = cms.vstring('beam-gas_IR5'), # default values, instantiated in BH_generator


    generatorSettings = cms.untracked.vstring(
    #    "allowedPdgId 13 -13",  # The PDG IDs of the allowed particles
    "BEAM 2",               # Beam number to generate (DEFAULT is 1)
    "SEED 1",               # Input val for the seed (DEFAULT is 1)    
    "pxLimits     -1 -1",   # x momentum (in GeV)
    "pyLimits     -1 -1",   # y momentum (in GeV)
    "pzLimits     -1 -1",   # z momentum (in GeV) 
    "energyLimits 0.2 -1",   # energy (in GeV)
    "xLimits      -1 -1",   # x position (in mm)
    "yLimits      -1 -1",   # y position (in mm)
    "ptLimits     -1 -1",
    "phiLimits    -1 -1",
    "etaLimits    -1 -1",
    "rLimits      -1 -1",
    "weightLimits -1 -1")
)
print("FLUKA generator created.")
