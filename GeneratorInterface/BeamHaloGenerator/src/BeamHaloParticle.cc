/* 
############################################################
#
# BeamHaloParticle.cc
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>, inspired from ATLAS BH generator
#         written by W.Bell
#
# May 27th, 2010
#
# Goal: 
# The basic Beam Halo object, describing a particle coming from 
# machine-induced background. It makes the interface between the
# different MIB simulations (MARS,FLUKA,...) and the HEP event send to
# the CMS simulation
#
# For more info on CMS machine-induced background simulation: http://
#
#############################################################
*/

#include "GeneratorInterface/BeamHaloGenerator/interface/BeamHaloParticle.h"

//---------------------------------------------------------------------

BeamHaloParticle::BeamHaloParticle(): m_pdgId(0), 
				      m_fourVector(), 
				      m_positionAtScoringPlane(), 
				      m_weight(0.),
 				      m_process(0.)
{}
 
//---------------------------------------------------------------------

BeamHaloParticle::BeamHaloParticle(long pdgId, 
				   HepMC::FourVector fourVector, 
				   HepMC::ThreeVector positionAtScoringPlane, 
				   double weight,
				   long process): m_pdgId(pdgId), 
						  m_fourVector(fourVector), 
						  m_positionAtScoringPlane(positionAtScoringPlane), 
						  m_weight(weight),
						  m_process(process) 
{}

//---------------------------------------------------------------------

BeamHaloParticle::BeamHaloParticle(const BeamHaloParticle& beamHaloParticle) 
{
  m_pdgId                  = beamHaloParticle.m_pdgId;
  m_fourVector             = beamHaloParticle.m_fourVector;
  m_positionAtScoringPlane = beamHaloParticle.m_positionAtScoringPlane; 
  m_weight                 = beamHaloParticle.m_weight;
  m_process                = beamHaloParticle.m_process;
}


//---------------------------------------------------------------------
//
// Method taking a MARS object as input, and producing the corresponding BeamHaloParticle
//
//---------------------------------------------------------------------

int BeamHaloParticle::fill(const HepPDT::ParticleData *m_particleData,MarsParticle *marsParticle) 
{
  double p_sq, mod_p, pz, mass, energy;

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // For the moment, MARS input is for 7TeV beam
  // We have 3.5 TeV currently
  //
  // Apply a correction scaling factor (BRUT FORCE, this will have to be changed in the future)
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   
  const double E_scale = 1.; //!!! Don't forget to remove that in the future !!! 
  

  // Read particle mass from pdg table, and PDG ID

  mass    = m_particleData->mass().value();
  m_pdgId = marsParticle->pdgId();
  
  // Mars uses GeV, as in CMS, then calculate |p|
  //   from relativistic kinetic energy (using c=1):
  //     KE = m - m_0            m^2 = p^2 + m_0^2
  
  p_sq  = std::pow(marsParticle->kineticEnergy()*E_scale + mass, 2) - std::pow(mass,2);
  mod_p = std::sqrt(p_sq);
 
  // CMS: the x-axis points towards the centre of the LHC ring, the
  // y-axis points upwards, and the z-axis points towards the Jura
  //
  // MARS simulation: the x-axis points into the ground, the y-axis
  // points out of the ring, and the z-axis points towards the airport
  // and the Saleve.
  //
  // So we have the following correspondance 
  //
  //    MARS   |   CMS
  //           |
  //     X     |   -Y
  //     Y     |   -X
  //     Z     |   -Z
  //

   // Calculate px and py from the directional cosines
   m_fourVector.setX(-1.0 * marsParticle->directionalCosines().y() * mod_p);
   m_fourVector.setY(-1.0 * marsParticle->directionalCosines().x() * mod_p);
   
   // Calculate pz from sqrt(p^2 - p_T^2) (the sign will be set in BeamHaloProducer.cc)
   pz = std::sqrt(p_sq - m_fourVector.perp2());
   m_fourVector.setZ(pz);  // This is always +ve.  Corrected during conversion to HepParticle  
 
   // Calculate the energy
   energy=std::sqrt(std::pow(mass,2)+std::pow(mod_p,2));
   m_fourVector.setE(energy);
   
   // Convert the position from cm to mm.
   m_positionAtScoringPlane.setX(-1.0 * marsParticle->positionAtScoringPlane().y() * 10.0);
   m_positionAtScoringPlane.setY(-1.0 * marsParticle->positionAtScoringPlane().x() * 10.0);
   m_positionAtScoringPlane.setZ(0.0); // The information is not provided in the input file.
   
   // replace this with marsParticle->effectiveWeight();
   m_weight = marsParticle->weight();
   
   MarsShowerPrimary origin = marsParticle->showerPrimary();
   m_process                = origin.processId;

   //MarsBeamGasInteraction bg_comp = marsParticle->beamGasInteraction();
   //   std::cout << bg_comp.nucleusType << " / " << origin.processId << " / " << m_pdgId << std::endl;

   return 0;
}


//---------------------------------------------------------------------
//
// Method taking a FLUKA object as input, and producing the corresponding BeamHaloParticle
//
//---------------------------------------------------------------------


int BeamHaloParticle::fill(const HepPDT::ParticleData *m_particleData,FlukaParticle *flukaParticle) 
{
  double p_sq, mod_p, mass, energy;
  
  // Needed for debugging
  //flukaParticle->print();
 
  // Read particle mass from pdg table, and PDG ID

  mass    = m_particleData->mass().value();
  m_pdgId = flukaParticle->pdgId();

  if(!m_pdgId) 
  {
    std::cout << "There is no PDG code for FLUKA id " << flukaParticle->flukaId() << std::endl;
    return 1;
  }

  // Mars uses GeV, as in CMS, then calculate |p|
  //   from relativistic kinetic energy (using c=1):
  //     KE = m - m_0            m^2 = p^2 + m_0^2
  
  p_sq  = std::pow(flukaParticle->kineticEnergy() + mass, 2) - std::pow(mass,2);
  mod_p = std::sqrt(p_sq);

 
  // CMS: the x-axis points towards the centre of the LHC ring, the
  // y-axis points upwards, and the z-axis points towards the Jura
  //
  //
  // We used the following correspondance 
  //
  //    FLUKA  |   CMS
  //           |
  //     X     |   X
  //     Y     |   Y
  //     Z     |   Z
  //

   // Calculate px and py from the directional cosines
   m_fourVector.setX(flukaParticle->directionalCosines().x() * mod_p);
   m_fourVector.setY(flukaParticle->directionalCosines().y() * mod_p);
   
   // Calculate pz from sqrt(p^2 - p_T^2) (the sign will be set in BeamHaloGenarator.cc)
   double pz = std::sqrt(p_sq - m_fourVector.perp2());
   m_fourVector.setZ(pz);  // This is always +ve.  Corrected during conversion to HepParticle  
 
   // Calculate the energy
   energy=std::sqrt(std::pow(mass,2)+std::pow(mod_p,2));
   m_fourVector.setE(energy);
   
   // Convert the position from cm to mm.
   m_positionAtScoringPlane.setX(flukaParticle->positionAtScoringPlane().x()*10.0);
   m_positionAtScoringPlane.setY(flukaParticle->positionAtScoringPlane().y()*10.0);
   m_positionAtScoringPlane.setZ(0.0); // The information is not provided in the input file.
    
   // FLUKA does not use weights
   m_weight = 1.0;

   return 0;
}




//---------------------------------------------------------------------
//
// Method to print BeamHaloParticle main properties
//
//---------------------------------------------------------------------
 
void BeamHaloParticle::print() 
{
  std::cout.fill(' ');
  std::cout.precision(6);
  std::cout.width(11); 
  std::cout << m_pdgId << " ";
  std::cout.width(13);
  std::cout << std::scientific << m_fourVector.px() << " "; 
  std::cout << std::scientific << m_fourVector.py() << " ";
  std::cout << std::scientific << m_fourVector.pz() << " ";
  std::cout << std::scientific << m_fourVector.e() << " ";
  std::cout << std::scientific << m_positionAtScoringPlane.x() << " ";
  std::cout << std::scientific << m_positionAtScoringPlane.y() << " ";
  std::cout << std::scientific << m_positionAtScoringPlane.z() << " ";
  std::cout << std::scientific << m_weight << std::endl; 
}

//-------------------------------------------------------------------------------------------------
