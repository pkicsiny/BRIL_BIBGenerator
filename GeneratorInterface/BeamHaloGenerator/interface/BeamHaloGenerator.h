#ifndef BEAMHALOGENERATOR_H
#define BEAMHALOGENERATOR_H


#include <vector>
#include <string>
#include <iostream>

#include "HepMC/GenEvent.h"
#include "HepMC/IO_HEPEVT.h"
#include "CLHEP/Random/RandFlat.h"

#include "TFile.h"
#include "TTree.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/RandomNumberGenerator.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "HepPDT/ParticleDataTable.hh"
#include "SimGeneral/HepPDTRecord/interface/PDTRecord.h" 

#include "GeneratorInterface/BeamHaloGenerator/interface/BeamHaloGeneratorSettings.h"
#include "GeneratorInterface/BeamHaloGenerator/interface/BeamHaloParticle.h"
#include "GeneratorInterface/BeamHaloGenerator/interface/AsciiInput.h"
#include "GeneratorInterface/BeamHaloGenerator/interface/BeamHaloProducer.h"


class BeamHaloGenerator {
 public:

  /// Constructor
  BeamHaloGenerator(edm::BeamHaloProducer *mainAlg, const edm::EventSetup *setup);
  /// Destructor
  virtual ~BeamHaloGenerator();
  virtual void initialize();
  virtual void finalize();
  virtual int fillEvt(edm::Event *event) = 0;

 protected:

  virtual int readEvent(std::vector<BeamHaloParticle>* beamHaloEvent) = 0;
    
  int  convertEvent(std::vector<BeamHaloParticle>* beamHaloEvent,HepMC::GenEvent* evt, edm::Event* event);
  bool flipEvent();
  bool bookNtuple(); 
  
  /** A pointer to the base algorithm to get services. */
  edm::BeamHaloProducer *m_mainAlg;
  
  
  
  /** A pointer to the particle data table. */
  edm::ESHandle < HepPDT::ParticleDataTable > m_particleTable;
  
  //   const HepPDT::ParticleDataTable *fPDGTable = &(*fTable);
  
  HepMC::GenEvent  *evt;
  
  /** The position of the interface plane in mm. */
  double m_interfacePlane;

  /** Flip probability */
  float m_flipProbability;  
  
  /** Flag for flipping event */
  bool m_flipEventEnabled;
  
  /** Input file name */
  //std::string m_inputFiles;
  std::vector<std::string> m_inputFiles;
  
  /** The file name used to store the binary buffer if required. */
  std::string m_binaryBufferFile; 
  
  /** Random number engine */
  CLHEP::HepRandomEngine *m_engine; 
  
  /** Ascii input */
  //AsciiInput *m_asciiInput;
  AsciiInput *m_asciiInputs;

  /** Generator settings */
  BeamHaloGeneratorSettings *m_BHG_settings;
  
  
  /** The event number */
  long m_eventNumber;
  
  /** Number of particles or events read from the ASCII input file.
      The MARS generator reads particles and the FLUKA generator reads
      events. */
  long m_nRead;
  
  /** Number of particles or events available from those read after
      the generator settings have been applied. */
  long m_nAfterCuts;
  
  /** Number of particles or events generated. */
  long m_nGenerated;
  
  /** Total weight of particles or events read from input file. */
  double m_wRead;
  
  /** Total weight of particles or events available from those read
      after the generator settings have been applied. (this corresponds to the particle rate)*/
  double m_wAfterCuts;
  
  /** Total weight of particles or events generated. */
  double m_wGenerated;
  
  long m_seed;
  
  
  TFile *m_rootFile;
  
  TTree *m_tree;
  
  double m_weight;
  double m_energy;
  double m_pos[3];
  double m_mom[3];
  double m_cos_x;
  double m_cos_y;
  double m_mass;
  int    m_PDG_ID;
  int    m_process;
  
  
};


#endif
