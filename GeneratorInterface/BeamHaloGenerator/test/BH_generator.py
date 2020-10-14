###########################################
#
# BH_generator.py
#
# Test script for MIB generation (generate 1000 B1 events
# using either FLUKA or MARS inputs)
#
# To use is just do
#
# cmsRun BH_generator.py
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
    version = cms.untracked.string('$Revision: 1.168.2.1 $'),
    annotation = cms.untracked.string('Test script for MIB production'),
    name = cms.untracked.string('PyReleaseValidation')
)

# Number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)

# Input source
process.source = cms.Source("EmptySource")


# Output definition
process.output = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    outputCommands = process.RAWSIMEventContent.outputCommands,
    fileName = cms.untracked.string('/tmp/sviret/BeamHalo_GEN.root'),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN'),
        filterName = cms.untracked.string('')
    ),
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('generation_step')
    )
)

# Additional output definition

# Global tal clearly depends on the release you are working on
# To get the correct GT, look at this page:
#
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions?redirectedfrom=CMS.SWGuideFrontierConditions#Global_Tags_for_Monte_Carlo_Prod
#

process.GlobalTag.globaltag = 'START42_V11::All'


# Here we choose the input, look into MIB_generator_cfi for more infos about the inputs
 
process.load('GeneratorInterface.BeamHaloGenerator.MIB_generator_cff')

process.generator       = process.FLUKA_generator  # FLUKA
#process.generator       = process.MARS_generator   # MARS

# Path and EndPath definitions

process.generation_step = cms.Path(process.pgen)
process.endjob_step     = cms.Path(process.endOfProcess)
process.out_step        = cms.EndPath(process.output)

# Schedule definition
process.schedule = cms.Schedule(process.generation_step,process.endjob_step,process.out_step)

# special treatment in case of production filter sequence  
for path in process.paths:     
    getattr(process,path)._seq = process.generator*getattr(process,path)._seq
