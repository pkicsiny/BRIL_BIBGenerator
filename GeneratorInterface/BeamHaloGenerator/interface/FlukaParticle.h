#ifndef FLUKAPARTICLE_H
#define FLUKAPARTICLE_H
 
#include "HepMC/SimpleVector.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>


class FlukaParticle {
 public:
  FlukaParticle();
  FlukaParticle(const FlukaParticle& flukaParticle);
  FlukaParticle& operator=(const FlukaParticle flukaParticle);

  int read(std::vector<std::string> *eventAsStringVector);
  void print();
  void clear();

  long eventId() const { return m_eventId; }
  int flukaId() const { return m_flukaId; }
  double kineticEnergy() const { return m_kineticEnergy; }
  HepMC::ThreeVector positionAtScoringPlane() const { return m_positionAtScoringPlane; }
  HepMC::ThreeVector directionalCosines() const { return m_directionalCosines; }
  double timeOfFlight() const { return m_timeOfFlight; }
  double primaryProtonZ() const { return m_primaryProtonZ; }
 
  int pdgId();
  
 private:
  long m_eventId;
  long m_runId;
  int m_flukaId;
  double m_kineticEnergy;
  HepMC::ThreeVector m_positionAtScoringPlane;
  HepMC::ThreeVector m_directionalCosines;
  double m_timeOfFlight;
  double m_primaryProtonZ;
};

#endif
