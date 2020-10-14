#ifndef BEAMHALOPARTICLE_H
#define BEAMHALOPARTICLE_H
 
#include "GeneratorInterface/BeamHaloGenerator/interface/MarsParticle.h"
#include "GeneratorInterface/BeamHaloGenerator/interface/FlukaParticle.h"

#include "HepMC/SimpleVector.h"
#include "HepPDT/ParticleData.hh"
#include <iostream>
#include <cmath>

namespace HepPDT 
{
  class ParticleData;
}
 

class BeamHaloParticle
{
 public:
  
  BeamHaloParticle();
  
  BeamHaloParticle(long pdgId, 
		   HepMC::FourVector fourVector, 
		   HepMC::ThreeVector positionAtScoringPlane, 
		   double weight,
		   long process);
  
  BeamHaloParticle(const BeamHaloParticle& beamHaloParticle);

  int fill(const HepPDT::ParticleData *m_particleData,
  	   MarsParticle *marsParticle);

  int fill(const HepPDT::ParticleData *m_particleData,
 	   FlukaParticle *flukaParticle); 

  void print();
  
  long pdgId() const { return m_pdgId; }
  long processOrigin() const { return m_process; }
  HepMC::FourVector fourVector() const { return m_fourVector; }
  HepMC::ThreeVector positionAtScoringPlane() const { return m_positionAtScoringPlane; }
  double weight() const { return m_weight; }
  
 private:
  
  /** The PDG Id of the particle */
  long m_pdgId;
   
  /** A four vector describing this particle at the scoring plane. */
  HepMC::FourVector m_fourVector;
   
  /** Position of the particle at the scoring plane (x,y,z). */
  HepMC::ThreeVector m_positionAtScoringPlane;
  
  /** The resultant particle weight after normalisation and rescaling. */
  double m_weight;

  /** The MARS tag defining the process which induced the particle */
  long m_process;
};



#endif
