#ifndef MARSHALOGENERATOR_H
#define MARSHALOGENERATOR_H


#include "GeneratorInterface/BeamHaloGenerator/interface/BeamHaloGenerator.h"
#include "GeneratorInterface/BeamHaloGenerator/interface/BeamHaloParticleBuffer.h"
#include "GeneratorInterface/BeamHaloGenerator/interface/MarsParticle.h"

#include <sstream>

class MarsHaloGenerator: public BeamHaloGenerator {
 public:
  
  /// Constructor
  MarsHaloGenerator(edm::BeamHaloProducer *mainAlg, const edm::EventSetup *setup);
  /// Destructor
  virtual ~MarsHaloGenerator();
  
 
  virtual void initialize();
  virtual int fillEvt(edm::Event *event);
  virtual void finalize();

 protected:


  int  readEvent(std::vector<BeamHaloParticle>* beamHaloEvent);
  int  convertEvent(std::vector<BeamHaloParticle>* beamHaloEvent,HepMC::GenEvent* evt);


  void fillBuffer(AsciiInput* my_input, BeamHaloParticleBuffer* my_buffer, double norm_fac, int type);

 private:
  
  AsciiInput *m_input_BGI_1;
  AsciiInput *m_input_BGI_2;
  AsciiInput *m_input_BGI_3;
  AsciiInput *m_input_BGI_4;
  AsciiInput *m_input_BGI_5;
  AsciiInput *m_input_BGI_6;
  AsciiInput *m_input_BGI_7;
  AsciiInput *m_input_BGI_8;
  AsciiInput *m_input_BGI_9;
  AsciiInput *m_input_BGI_10;
  
  AsciiInput *m_input_BGE_1;
  AsciiInput *m_input_BGE_2;
  AsciiInput *m_input_BGE_3;
  AsciiInput *m_input_BGE_4;
  AsciiInput *m_input_BGE_5;
  AsciiInput *m_input_BGE_6;
  AsciiInput *m_input_BGE_7;
  AsciiInput *m_input_BGE_8;
  AsciiInput *m_input_BGE_9;
  AsciiInput *m_input_BGE_10;
  
  AsciiInput *m_input_BH_1;
  AsciiInput *m_input_BH_2;
  AsciiInput *m_input_BH_3;
  AsciiInput *m_input_BH_4;
  AsciiInput *m_input_BH_5;
  AsciiInput *m_input_BH_6;
  AsciiInput *m_input_BH_7;
  AsciiInput *m_input_BH_8;
  AsciiInput *m_input_BH_9;
  AsciiInput *m_input_BH_10;
  
  
  BeamHaloParticleBuffer *m_beamHaloParticleBuffer;
  
};

#endif
