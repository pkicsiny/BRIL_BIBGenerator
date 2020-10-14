#ifndef BEAMHALOPRODUCER_H
#define BEAMHALOPRODUCER_H

#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

class BeamHaloGenerator;

namespace edm
{
  class BeamHaloProducer : public EDProducer {
  public:

    /// Constructor
    BeamHaloProducer(const ParameterSet &);
    /// Destructor
    virtual ~BeamHaloProducer();

    virtual void beginRun(Run & r, const EventSetup & es);
    virtual void produce(Event & e, const EventSetup & es);
    virtual void endRun(Run & r, const EventSetup & es);

    std::string inputFile() const { return m_inputFile; }    
    double interfacePlane() const { return m_interfacePlane; }
    float flipProbability() const { return m_flipProbability; }
    bool flipEventEnabled() const { return m_flipEventEnabled; }
    std::vector<std::string>* generatorSettings() { return &m_generatorSettings; }
    std::string binaryBufferFile() const { return m_binaryBufferFile; }

  private:

    /** Input file type and therefore associated beam halo generator
	that should be used. */
    std::string m_inputTypeStr;

    /** Input file name */
    std::string m_inputFile;

    /** The position of the interface plane in mm. */
    double m_interfacePlane;

    /** Flip probability */
    float m_flipProbability;  
    
    /** Flag for flipping event */
    bool m_flipEventEnabled;

    /** A vector of strings defining generator settings. */
    std::vector<std::string> m_generatorSettings;
     
    /** The file name used to store the binary buffer if required. */
    std::string m_binaryBufferFile; 

    /** A pointer to the beam halo generator */
    BeamHaloGenerator *m_beamHaloGenerator;

  };

}

#endif
