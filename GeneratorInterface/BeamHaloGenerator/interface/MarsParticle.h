#ifndef MARSPARTICLE_H
#define MARSPARTICLE_H
 
#include "HepMC/SimpleVector.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
 
class MarsShowerPrimary {
public:
  MarsShowerPrimary();
   
  int particleId;              // IORIG
  int processId;               // KORIG
  double weight;               // WORIG
  HepMC::ThreeVector position; // XORG, YORG, ZORG
  double kineticEnergy;        // EORIG
};

class MarsBeamGasInteraction {
 public:
  MarsBeamGasInteraction();
  
  int nucleusType;             // INUC
  int nevve;                   // ? NEVVE
  double timeOfFlight;         // ZBGASHIT
};

class MarsParticle {
 public:
  MarsParticle();
  
  int read(std::vector<std::string> *eventAsStringVector, double norm_factor, int type);
  void print(bool beamGas);
  
  long eventNumber() const { return m_eventNumber; }
  int particleId() const { return m_particleId; }
  double kineticEnergy() const { return m_kineticEnergy; }
  double weight() const { return m_weight; }
  HepMC::ThreeVector positionAtScoringPlane() const { return m_positionAtScoringPlane; }
  HepMC::ThreeVector directionalCosines() const { return m_directionalCosines; }
  double timeOfFlight() const { return m_timeOfFlight; }
  double primaryProtonZ() const { return m_primaryProtonZ; }
  MarsShowerPrimary showerPrimary() const { return m_showerPrimary; }
  MarsBeamGasInteraction beamGasInteraction() const { return m_beamGasInteraction; }
 
  int pdgId();
  
 private:
  long m_eventNumber;
  int m_particleId;
  double m_kineticEnergy;
  double m_weight;
  HepMC::ThreeVector m_positionAtScoringPlane;
  HepMC::ThreeVector m_directionalCosines;
  double m_timeOfFlight;
  double m_primaryProtonZ;
  MarsShowerPrimary m_showerPrimary;
  MarsBeamGasInteraction m_beamGasInteraction;
};

#endif
