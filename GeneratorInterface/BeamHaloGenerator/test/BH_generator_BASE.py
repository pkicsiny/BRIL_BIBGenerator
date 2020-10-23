###########################################
#
# BH_generator_BASE.py
#
# Script for MIB batch generation (generate 1000 B1 events
# using either FLUKA or MARS inputs)
#
# WARNING: this is a base script, you're not suppose to modify it
#
# SV: 20/12/2010
#
##########################################

import FWCore.ParameterSet.Config as cms

process = cms.Process('GEN') # Generation only

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryIdeal_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('Configuration.StandardSequences.VtxSmearedNoSmear_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.EventContent.EventContent_cff')

process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.1 $'),
    annotation = cms.untracked.string('Configuration/Generator/python/BeamHalo_cfi.py'),
    name = cms.untracked.string('PyReleaseValidation')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(NEVTS)
)

# Input source
process.source = cms.Source("EmptySource")

# Add this line to generate event with a different seed
#process.RandomNumberGeneratorService.generator.initialSeed = YOURSEED


# Output definition
process.output = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    outputCommands = process.RAWSIMEventContent.outputCommands,
    fileName = cms.untracked.string('BeamHalo_GEN.root'),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM-RAW'),
        filterName = cms.untracked.string('')
    ),
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('generation_step')
    )
)

# Additional output definition

# Other statements
#

process.GlobalTag.globaltag = 'MYGLOBALTAG'

# Here we choose the input, look into MIB_generator_cfi for more infos about the inputs
 
process.load('GeneratorInterface.BeamHaloGenerator.MIB_generator_cff')

process.generator = process.MYGENNAME_generator  # FLUKA OR MARS

process.generator.InputFiles = cms.vstring('MYFNAME')

process.generator.generatorSettings = cms.untracked.vstring(
    #ONLYMU"allowedPdgId 13 -13",  # The PDG IDs of the allowed particles
    "BEAM NBEAM",             # Beam number to generate (DEFAULT is 2)
    "SEED NSEED",             # Input val for the seed (DEFAULT is 1)    
    "pxLimits     -1 -1",   # x momentum (in GeV)
    "pyLimits     -1 -1",   # y momentum (in GeV)
    "pzLimits     -1 -1",   # z momentum (in GeV) 
    "energyLimits MINE -1",   # energy (in GeV)
    "xLimits      -1 -1",   # x position (in mm)
    "yLimits      -1 -1",   # y position (in mm)
    "ptLimits     MINPT -1",
    "phiLimits    -1 -1",
    "etaLimits    -1 -1",
    "rLimits      -1 -1",
    "weightLimits -1 -1"
)


# Path and EndPath definitions
process.generation_step = cms.Path(process.pgen)
process.endjob_step     = cms.Path(process.endOfProcess)
process.out_step        = cms.EndPath(process.output)

# Schedule definition
process.schedule = cms.Schedule(process.generation_step,process.endjob_step,process.out_step)

# special treatment in case of production filter sequence  
for path in process.paths: 
    getattr(process,path)._seq = process.generator*getattr(process,path)._seq
