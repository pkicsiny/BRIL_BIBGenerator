/* 
############################################################
#
# BeamHaloGeneratorSettings.cc
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>, inspired from ATLAS BH generator
#         written by W.Bell
#
# May 27th, 2010
#
# Goal: 
# Get the different job options from the python configuration script
#
# For more info on CMS machine-induced background simulation: http://
#
#############################################################
*/

#include "GeneratorInterface/BeamHaloGenerator/interface/BeamHaloGeneratorSettings.h"
 
BeamHaloGeneratorSettings::BeamHaloGeneratorSettings(std::vector<std::string> *settings): m_generatorSettings(settings),
											  m_limits(),
											  m_beamVal(1),
											  m_seed(1),
											  m_settingsParsed(false) 
{}
 
//---------------------------------------------------------------------
//
// The settings appear as a text line in the job options, one has to 
// parse and decode them. This is the role of this method
//
//---------------------------------------------------------------------

int BeamHaloGeneratorSettings::parseSettings() 
{
  
  //debug
  //std::cout << "BeamHaloGeneratorSettings::parseSettings" << std::endl;

  std::vector<std::string> strVector;
  std::vector<std::string>::iterator row_itr;
  std::vector<std::string>::iterator row_itr_end;
  std::vector<std::string>::iterator col_itr;
  std::vector<std::string>::iterator col_itr_end;
  //double lowerLimit;
  //double upperLimit;
  
  // Loop over all settings.
  row_itr     = m_generatorSettings->begin();
  row_itr_end = m_generatorSettings->end();
 
  for(;row_itr!=row_itr_end;++row_itr) 
  {
    strVector.clear();
    strVector = AsciiInput::strToStrVec(*row_itr);
    
    if(strVector.size() == 0) continue;
    if(strVector.size() == 1) 
    {
      std::cerr << "A generator setting must be followed by a value" << std::endl;
      continue;
    }
 
    //lowerLimit = -1.; upperLimit = -1.; // Disable limits by default;
    
    col_itr     = strVector.begin();
    col_itr_end = strVector.end();
        
    if((*col_itr) == "allowedPdgId") // Special case, list of particles to use
    {
      col_itr++;

      for(;col_itr!=col_itr_end;++col_itr)  // Loop over particle PDG IDs
      {
	m_allowedPdgIds.push_back(AsciiInput::strToLong(*col_itr));	
      }
    }
    else if((*col_itr) == "BEAM") // Special case, beam number
    {
      col_itr++;

      m_beamVal = AsciiInput::strToLong(*col_itr);

      if (m_beamVal != 2)
      {
	m_beamVal = 1; // Default beam is beam 1
      }
    }
    else if((*col_itr) == "SEED") // Special case, seed input value
    {
      col_itr++;

      m_seed = AsciiInput::strToLong(*col_itr);

      if (m_seed == -1) m_seed=1; // Default is 1
    }
    else 
    {
      // The basic method: lower and upper limit for the considered value 
      BeamHaloGeneratorSettings::parseLimitSetting(&strVector); 
    }
  }
 
  BeamHaloGeneratorSettings::printSettings();  
  m_settingsParsed = true;
 
  return 0;
}
 
//---------------------------------------------------------------------
//
// Method setting upper and lower limit for a given parameter contained in
// a pre-defined list
//
//---------------------------------------------------------------------

int BeamHaloGeneratorSettings::parseLimitSetting(std::vector<std::string> *commandVector) 
{
  //debug
  //std::cout << "BeamHaloGeneratorSettings::parseLimitSetting" << std::endl;

  double lowerLimit, upperLimit;
 
  std::string availableLimits[11] = {"pxLimits",
				     "pyLimits",
				     "pzLimits",
				     "energyLimits",
				     "xLimits",
				     "yLimits",
				     "ptLimits",
				     "phiLimits",
				     "etaLimits",
				     "rLimits",
				     "weightLimits"};
   
  std::vector<std::string>::iterator itr     = commandVector->begin();
  std::vector<std::string>::iterator itr_end = commandVector->end();
  
  int i = 0;
  while(i<11)
  {
    if((*itr) == availableLimits[i]) break; // We found the name
    i++;
  }
   
  if(i==11) // The text doesn't correspond to any possibility
  {
    std::cerr << "Error: " << (*itr)  << " is an un-known generator setting." << std::endl;
    return 1;
  }
 
  if(m_limits.find(availableLimits[i]) != m_limits.end()) // Limits were already set
  {
    std::cerr << "Error: " << availableLimits[i] << " has already been set." << std::endl;
    return 1;
  }
   
  // Limit values are always positive, -1 means default and no cut.

  lowerLimit = -1.0;
  upperLimit = -1.0;
  itr++; if(itr!=itr_end) lowerLimit = AsciiInput::strToDouble(*itr);
  itr++; if(itr!=itr_end) upperLimit = AsciiInput::strToDouble(*itr);
  m_limits.insert(std::make_pair(availableLimits[i],std::make_pair(lowerLimit,upperLimit)));
  
  return 0;
}
 
  
//---------------------------------------------------------------------
//
// Check if a BeamHaloParticle passes the cuts of not
//
//---------------------------------------------------------------------

bool BeamHaloGeneratorSettings::checkParticle(BeamHaloParticle *beamHaloParticle) 
{
  //debug
  //std::cout << "BeamHaloGeneratorSettings::checkParticle" << std::endl;

  if(!m_settingsParsed) // The parsing hasn't been done yet
  {
    if(parseSettings() != 0){
	std::cout << "Check didn't pass." << std::endl;
	return false;  
     }
  }
 
  // Search the allowed PDG id list if it has been defined.
  if(m_allowedPdgIds.size() != 0) 
  {
    std::vector<long>::iterator itr = m_allowedPdgIds.begin();
    std::vector<long>::iterator itr_end = m_allowedPdgIds.end();
    while(itr!=itr_end) 
    {
      if(beamHaloParticle->pdgId() == (*itr)) break;
      ++itr;
    }

    if(itr==itr_end) 
    {
      std::cout << "Check didn't pass." << std::endl;
      return false;
    }
  }

  if(!checkSetting("pxLimits", fabs(beamHaloParticle->fourVector().px()))) return false;
  if(!checkSetting("pyLimits", fabs(beamHaloParticle->fourVector().py()))) return false;
  if(!checkSetting("pzLimits", fabs(beamHaloParticle->fourVector().pz()))) return false;
  if(!checkSetting("energyLimits", fabs(beamHaloParticle->fourVector().e()))) return false;
  if(!checkSetting("xLimits", fabs(beamHaloParticle->positionAtScoringPlane().x()))) return false;
  if(!checkSetting("yLimits", fabs(beamHaloParticle->positionAtScoringPlane().y()))) return false;
  if(!checkSetting("ptLimits", beamHaloParticle->fourVector().perp())) return false;
  if(!checkSetting("etaLimits", fabs(beamHaloParticle->fourVector().pseudoRapidity()))) return false;
  if(!checkSetting("phiLimits", fabs(beamHaloParticle->fourVector().phi()))) return false;
  if(!checkSetting("rLimits", beamHaloParticle->positionAtScoringPlane().perp())) return false;
  if(!checkSetting("weightLimits", fabs(beamHaloParticle->weight()))) return false;
  std::cout << "Check passed." << std::endl;
  return true;
}
 
//---------------------------------------------------------------------

bool BeamHaloGeneratorSettings::checkSetting(std::string key, double value) 
{
  //debug
  //std::cout << "BeamHaloGeneratorSettings::checkSetting " << key << ": " << value << std::endl;

  std::map<std::string, std::pair<float, float> >::iterator itr = m_limits.find(key);
  if(itr == m_limits.end()) 
  {
    std::cerr << "Error: the limit " << key << " is not defined." << std::endl;
    return false;
  }
  
  if((*itr).second.first >= 0. && value < (*itr).second.first) return false;
  if((*itr).second.second >= 0. && value >= (*itr).second.second) return false;
  
  return true;
}

//---------------------------------------------------------------------

void BeamHaloGeneratorSettings::printSettings() 
{

  //debug
  //std::cout << "BeamHaloGeneratorSettings::printSettings" << std::endl;

  std::cout << "##################################################" << std::endl;

  std::cout << "Particles coming from BEAM " << BeamHaloGeneratorSettings::getBeam() << " will be produced." << std::endl;


  std::cout << "allowedPdfIds: ";
  if(m_allowedPdgIds.size() != 0) 
  {
    std::vector<long>::iterator pdgId_itr = m_allowedPdgIds.begin();
    std::vector<long>::iterator pdgId_itr_end = m_allowedPdgIds.end();

    for(;pdgId_itr != pdgId_itr_end; ++pdgId_itr) 
    {
      std::cout << (*pdgId_itr) << " ";
    }
    std::cout << std::endl;
  }
  else {
    std::cout << "All PDG IDs are allowed." << std::endl;
  }
 
  std::map<std::string, std::pair<float, float> >::iterator limit_itr = m_limits.begin();
  std::map<std::string, std::pair<float, float> >::iterator limit_itr_end = m_limits.end();

  for(;limit_itr!=limit_itr_end;++limit_itr) 
  {
    std::cout << " " << (*limit_itr).first << ": {";
    
    if((*limit_itr).second.first <= 0.) std::cout << "disabled, ";
    else std::cout << (*limit_itr).second.first << ", ";
    
    if((*limit_itr).second.second <= 0.) std::cout << "disabled}";
    else std::cout << (*limit_itr).second.second << "}";
    std::cout << std::endl;
  }
}
