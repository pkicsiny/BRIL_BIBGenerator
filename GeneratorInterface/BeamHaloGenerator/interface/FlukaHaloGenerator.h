#ifndef FLUKAHALOGENERATOR_H
#define FLUKAHALOGENERATOR_H


#include <sstream>

#include "GeneratorInterface/BeamHaloGenerator/interface/BeamHaloGenerator.h"
#include "GeneratorInterface/BeamHaloGenerator/interface/FlukaParticle.h"

class FlukaHaloGenerator: public BeamHaloGenerator {
 public:
  
  /// Constructor
  FlukaHaloGenerator(edm::BeamHaloProducer *mainAlg, const edm::EventSetup *setup);
  /// Destructor
  virtual ~FlukaHaloGenerator();
  
 
  virtual void initialize();
  virtual int fillEvt(edm::Event *event);
  virtual void finalize();

 protected:

  int  readEvent(std::vector<BeamHaloParticle>* beamHaloEvent);
  int  convertEvent(std::vector<BeamHaloParticle>* beamHaloEvent,HepMC::GenEvent* evt);

 private:
  
  //List of strings
  AsciiInput *m_inputs;
  //AsciiInput *m_input;
  bool m_sameEvent;
  bool m_firstEvent;
  int m_fileIndex;
  FlukaParticle m_flukaParticle;
  FlukaParticle m_lastFlukaParticle;

  
};

#endif
