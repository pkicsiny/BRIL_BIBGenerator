#ifndef BEAMHALOGENERATORSETTINGS_H
#define BEAMHALOGENERATORSETTINGS_H

#include "GeneratorInterface/BeamHaloGenerator/interface/BeamHaloParticle.h"
#include "GeneratorInterface/BeamHaloGenerator/interface/AsciiInput.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cmath>

class BeamHaloParticle;

class BeamHaloGeneratorSettings 
{
 public:
 
  BeamHaloGeneratorSettings(std::vector<std::string> *settings);
 
  // Returns 0 if successful and a status code otherwise.
  int parseSettings(void);
 
  // Check if the supplied beam halo particle passes the generator
  // settings.
  bool checkParticle(BeamHaloParticle *beamHaloParticle);
  
  inline const int getBeam() {return m_beamVal;}
  inline const int getSeed() {return m_seed;}

  void printSettings(void);
  
 private:

  int parseLimitSetting(std::vector<std::string> *commandVector);
  bool checkSetting(std::string key, double value);
  
  
  // A vector of strings to configure the generator settings.
  std::vector<std::string>* m_generatorSettings;
  
  // An allowed set of PDG ids where any empty vector implies all PDG
  // ids are allowed.
  std::vector<long> m_allowedPdgIds;
  
  // Minimum and maximum limits, where all limits are absolute values,
  // |eta| for example, and -1 implies the limit is disabled.
  std::map<std::string, std::pair<float, float> > m_limits;
  

  // From which beam are the events generated.
  int m_beamVal;

  // Seed value (to be multiplied by current time).
  int m_seed;

  // Flag
  bool m_settingsParsed;
};
 
#endif
