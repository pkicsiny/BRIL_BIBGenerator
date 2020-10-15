/* 
############################################################
#
# FlukaParticle.cc
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>, inspired from ATLAS BH generator
#         written by W.Bell
#
# May 27th, 2010
#
# Goal: 
# A class handling the event produced by MARS simulation (by N. Mokhov)
#
# For more info on CMS machine-induced background simulation: http://
#
# For more info on MARS code: http://www-ap.fnal.gov/MARS/
#
#############################################################
*/

#include "GeneratorInterface/BeamHaloGenerator/interface/FlukaParticle.h"



//-------------------------------------------------------------------------------------------------

FlukaParticle::FlukaParticle(): m_eventId(0),
				m_flukaId(0),				
				m_kineticEnergy(0.),
				m_positionAtScoringPlane(),
				m_directionalCosines(),
				m_timeOfFlight (0.),
				m_primaryProtonZ(0.){
}
 
//---------------------------------------------------------------------------

FlukaParticle::FlukaParticle(const FlukaParticle& flukaParticle) 
{
  m_eventId                = flukaParticle.m_eventId;
  m_flukaId                = flukaParticle.m_flukaId;
  m_kineticEnergy          = flukaParticle.m_kineticEnergy;
  m_positionAtScoringPlane = flukaParticle.m_positionAtScoringPlane;
  m_directionalCosines     = flukaParticle.m_directionalCosines;
  m_timeOfFlight           = flukaParticle.m_timeOfFlight;
  m_primaryProtonZ         = flukaParticle.m_primaryProtonZ;
}

//---------------------------------------------------------------------------
 
FlukaParticle& FlukaParticle::operator=(const FlukaParticle flukaParticle) 
{
  m_eventId                = flukaParticle.m_eventId;
  m_flukaId                = flukaParticle.m_flukaId;
  m_kineticEnergy          = flukaParticle.m_kineticEnergy;
  m_positionAtScoringPlane = flukaParticle.m_positionAtScoringPlane;
  m_directionalCosines     = flukaParticle.m_directionalCosines;
  m_timeOfFlight           = flukaParticle.m_timeOfFlight;
  m_primaryProtonZ         = flukaParticle.m_primaryProtonZ;
  return *this;
}

//---------------------------------------------------------------------------
 
void FlukaParticle::clear() 
{
  m_eventId = 0; 
  m_flukaId = 0; 
  m_kineticEnergy = 0.;
  m_positionAtScoringPlane.setX(0.);
  m_positionAtScoringPlane.setY(0.);
  m_positionAtScoringPlane.setZ(0.);
  //m_positionAtScoringPlane.setT(0.);
  m_directionalCosines.setX(0.);
  m_directionalCosines.setY(0.);
  m_directionalCosines.setZ(0.);
  m_timeOfFlight = 0.;
  m_primaryProtonZ = 0.;
}


//-------------------------------------------------------------------------------------------------
// 
// Method reading the text file (AsciiInput) and filling FLUKA particle object
//
//-------------------------------------------------------------------------------------------------

int FlukaParticle::read(std::vector<std::string> *eventAsStringVector) 
{
  //if(eventAsStringVector->size() != 15) 
  //edit GA: in the Ph2 Fluka files the FLUKA run ID is missing,so only 14 columns
  if(eventAsStringVector->size() != 14) 
  {
    std::cerr << "There are only " << eventAsStringVector->size() 
	      << " data words.  This event will be null." << std::endl;
    return 1;
  }
  
  double doubleValue;
  std::vector<std::string>::iterator itr     = eventAsStringVector->begin();
  std::vector<std::string>::iterator itr_end = eventAsStringVector->end();
  int wordNumber = 0;

  for(;itr!=itr_end;++itr,++wordNumber) 
  {
    std::istringstream inStr((*itr));
    switch (wordNumber) 
    {
    //case 0  : inStr >> m_runId; break;
    case 0  : inStr >> m_eventId; break;
    case 1  : inStr >> m_flukaId; break;
    case 4  : inStr >> doubleValue; m_positionAtScoringPlane.setX(doubleValue); break;
    case 5  : inStr >> doubleValue; m_positionAtScoringPlane.setY(doubleValue); break;
    case 6  : inStr >> doubleValue; m_directionalCosines.setX(doubleValue); break;
    case 7  : inStr >> doubleValue; m_directionalCosines.setY(doubleValue); break;
    case 9  : inStr >> m_kineticEnergy; break;
    case 10  : inStr >> m_timeOfFlight; break;
    case 13 : inStr >> m_primaryProtonZ; break;
    default : break;
    }
  }


  m_eventId = 1000*m_runId + m_eventId ;

  return 0;
}

//-------------------------------------------------------------------------------------------------
 
void FlukaParticle::print() 
{
  std::cout.fill(' ');
  std::cout.width(11); std::cout << m_eventId << " ";
  std::cout.width(6);  std::cout << m_flukaId << " ";
  std::cout.width(12); std::cout.precision(5); std::cout << std::scientific << m_kineticEnergy << " "; 
  std::cout.width(11); std::cout.precision(4); std::cout << std::scientific << m_positionAtScoringPlane.x() << " ";
  std::cout.width(11); std::cout.precision(4); std::cout << std::scientific << m_positionAtScoringPlane.y() << " ";
  std::cout.width(14); std::cout.precision(7); std::cout << std::scientific << m_directionalCosines.x() << " ";
  std::cout.width(14); std::cout.precision(7); std::cout << std::scientific << m_directionalCosines.y() << " ";
  std::cout.width(11); std::cout.precision(4); std::cout << std::scientific << m_timeOfFlight << " "; 
  std::cout.width(12); std::cout.precision(5); std::cout << std::scientific << m_primaryProtonZ << " ";
  std::cout.fill(' ');
  std::cout << std::endl;
  
  std::cout.precision(6);
}
 
//-------------------------------------------------------------------------------------------------

int FlukaParticle::pdgId() {
  int pdgID = 0;
  
  switch (m_flukaId) {
  case -6 : pdgID = 1000020040; break;
  case -5 : pdgID = 1000020030; break;
  case -4 : pdgID = 1000010030; break;
  case -3 : pdgID = 1000010020; break;
  case 1 : pdgID = 2212; break;
  case 2 : pdgID = -2212; break;
  case 3 : pdgID = 11; break;
  case 4 : pdgID = -11; break;
  case 5 : pdgID = 12; break;
  case 6 : pdgID = -12; break;
  case 7 : pdgID = 22; break;
  case 8 : pdgID = 2112; break;
  case 9 : pdgID = -2112; break;
  case 10 : pdgID = -13; break;
  case 11 : pdgID = 13; break;
  case 12 : pdgID = 130; break;
  case 13 : pdgID = 211; break;
  case 14 : pdgID = -211; break;
  case 15 : pdgID = 321; break;
  case 16 : pdgID = -321; break;
  case 17 : pdgID = 3122; break;
  case 18 : pdgID = -3122; break;
  case 19 : pdgID = 310; break;
  case 20 : pdgID = 3112; break;
  case 21 : pdgID = 3222; break;
  case 22 : pdgID = 3212; break;
  case 23 : pdgID = 111; break;
  case 24 : pdgID = 311; break;
  case 25 : pdgID = -311; break;
  case 27 : pdgID = 14; break;
  case 28 : pdgID = -14; break;
  case 31 : pdgID = -3222; break;
  case 32 : pdgID = -3212; break;
  case 33 : pdgID = -3112; break;
  case 34 : pdgID = 3322; break;
  case 35 : pdgID = -3322; break;
  case 36 : pdgID = 3312; break;
  case 37 : pdgID = -3312; break;
  case 38 : pdgID = 3334; break;
  case 39 : pdgID = -3334; break;
  case 41 : pdgID = -15; break;
  case 42 : pdgID = 15; break;
  case 43 : pdgID = 16; break;
  case 44 : pdgID = -16; break;
  case 45 : pdgID = 411; break;
  case 46 : pdgID = -411; break;
  case 47 : pdgID = 421; break;
  case 48 : pdgID = -421; break;
  case 49 : pdgID = 431; break;
  case 50 : pdgID = -431; break;
  case 51 : pdgID = 4122; break;
  case 52 : pdgID = 4232; break;
  case 53 : pdgID = 4112; break;
  case 54 : pdgID = 4322; break;
  case 55 : pdgID = 4312; break;
  case 56 : pdgID = 4332; break;
  case 57 : pdgID = -4122; break;
  case 58 : pdgID = -4232; break;
  case 59 : pdgID = -4132; break;
  case 60 : pdgID = -4322; break;
  case 61 : pdgID = -4312; break;
  case 62 : pdgID = -4332; break;
  default : pdgID = 0; break;
  }
  
  return pdgID;
}
 
