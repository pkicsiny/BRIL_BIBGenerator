/* 
############################################################
#
# MarsHaloGenerator.cc
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>, inspired from ATLAS BH generator
#         written by W.Bell
#
# May 27th, 2010
#
# Goal: 
# Produce beam halo events based on MARS15 simulation files
#
# Input parameters are:
#
#
# For more info on CMS machine-induced background simulation: http://
#
#############################################################
*/


#include "GeneratorInterface/BeamHaloGenerator/interface/MarsHaloGenerator.h"

using namespace edm;
using namespace std;



MarsHaloGenerator::~MarsHaloGenerator() 
{}

//
// Constructor
//

MarsHaloGenerator::MarsHaloGenerator(edm::BeamHaloProducer *mainAlg, const edm::EventSetup *setup) :
  BeamHaloGenerator(mainAlg,setup),
  m_beamHaloParticleBuffer(0)
{
 
  // -- initialisation

  // Open the MARS15 text file containing the halo event info for CMS

  // Inelastic interaction with beam gas
  m_input_BGI_1 = new AsciiInput("InputFiles/bgi-b2r5.1");
  m_input_BGI_2 = new AsciiInput("InputFiles/bgi-b2r5.2");
  m_input_BGI_3 = new AsciiInput("InputFiles/bgi-b2r5.3");
  m_input_BGI_4 = new AsciiInput("InputFiles/bgi-b2r5.4");
  m_input_BGI_5 = new AsciiInput("InputFiles/bgi-b2r5.5");
  m_input_BGI_6 = new AsciiInput("InputFiles/bgi-b2r5.6");
  m_input_BGI_7 = new AsciiInput("InputFiles/bgi-b2r5.7");
  m_input_BGI_8 = new AsciiInput("InputFiles/bgi-b2r5.8");
  m_input_BGI_9 = new AsciiInput("InputFiles/bgi-b2r5.9");
  m_input_BGI_10= new AsciiInput("InputFiles/bgi-b2r5.10");

  // Elastic interaction with beam gas
  m_input_BGE_1 = new AsciiInput("InputFiles/bge-b2r5.1");
  m_input_BGE_2 = new AsciiInput("InputFiles/bge-b2r5.2");
  m_input_BGE_3 = new AsciiInput("InputFiles/bge-b2r5.3");
  m_input_BGE_4 = new AsciiInput("InputFiles/bge-b2r5.4");
  m_input_BGE_5 = new AsciiInput("InputFiles/bge-b2r5.5");
  m_input_BGE_6 = new AsciiInput("InputFiles/bge-b2r5.6");
  m_input_BGE_7 = new AsciiInput("InputFiles/bge-b2r5.7");
  m_input_BGE_8 = new AsciiInput("InputFiles/bge-b2r5.8");
  m_input_BGE_9 = new AsciiInput("InputFiles/bge-b2r5.9");
  m_input_BGE_10= new AsciiInput("InputFiles/bge-b2r5.10");

  // Beam halo from tertiary collimators
  m_input_BH_1 = new AsciiInput("InputFiles/bh-b2r5.1");
  m_input_BH_2 = new AsciiInput("InputFiles/bh-b2r5.2");
  m_input_BH_3 = new AsciiInput("InputFiles/bh-b2r5.3");
  m_input_BH_4 = new AsciiInput("InputFiles/bh-b2r5.4");
  m_input_BH_5 = new AsciiInput("InputFiles/bh-b2r5.5");
  m_input_BH_6 = new AsciiInput("InputFiles/bh-b2r5.6");
  m_input_BH_7 = new AsciiInput("InputFiles/bh-b2r5.7");
  m_input_BH_8 = new AsciiInput("InputFiles/bh-b2r5.8");
  m_input_BH_9 = new AsciiInput("InputFiles/bh-b2r5.9");
  m_input_BH_10= new AsciiInput("InputFiles/bh-b2r5.10");

  m_input_BGI_1->open();
  m_input_BGI_2->open();
  m_input_BGI_3->open();
  m_input_BGI_4->open();
  m_input_BGI_5->open();
  m_input_BGI_6->open();
  m_input_BGI_7->open();
  m_input_BGI_8->open();
  m_input_BGI_9->open();
  m_input_BGI_10->open();

  m_input_BGE_1->open();
  m_input_BGE_2->open();
  m_input_BGE_3->open();
  m_input_BGE_4->open();
  m_input_BGE_5->open();
  m_input_BGE_6->open();
  m_input_BGE_7->open();
  m_input_BGE_8->open();
  m_input_BGE_9->open();
  m_input_BGE_10->open();

  m_input_BH_1->open();
  m_input_BH_2->open();
  m_input_BH_3->open();
  m_input_BH_4->open();
  m_input_BH_5->open();
  m_input_BH_6->open();
  m_input_BH_7->open();
  m_input_BH_8->open();
  m_input_BH_9->open();
  m_input_BH_10->open();
}



//
// Initialization of the run
//

void MarsHaloGenerator::initialize()
{
  BeamHaloGenerator::initialize();

  // The buffer into which we will put the particle we want and pick them up randomly
  m_beamHaloParticleBuffer = new BeamHaloParticleBuffer(m_binaryBufferFile,m_engine);
  m_beamHaloParticleBuffer->openForWriting();



  // Sum rules for BEAM1 and BEAM2 cases:
  //
  // BG : Beam gas inelastic/elastic (BGE/BGI)
  // BH : Beam-halo from tertiary collimators (BH)
  //
  // For BEAM 1 : BG+0.085*BH
  // For BEAM 2 : BG+BH

  // Normalization factor is necessary to get the correct rate estimation
  
  double norm_BGI = 2321128./5000000.;
  double norm_BGE = 115400000./30000000.;
  double norm_BH  = 8300000000./30000000.;

  if (m_BHG_settings->getBeam()==1) norm_BH*=0.085;


  std::cout << "Weight for beam halo: " << norm_BH << std::endl;

  


  // es.get<HepPDT::PDTRecord>().get(m_pTable);

  fillBuffer(m_input_BGI_1,m_beamHaloParticleBuffer,norm_BGI,0);
  fillBuffer(m_input_BGI_2,m_beamHaloParticleBuffer,norm_BGI,0);
  fillBuffer(m_input_BGI_3,m_beamHaloParticleBuffer,norm_BGI,0);
  fillBuffer(m_input_BGI_4,m_beamHaloParticleBuffer,norm_BGI,0);
  fillBuffer(m_input_BGI_5,m_beamHaloParticleBuffer,norm_BGI,0);
  fillBuffer(m_input_BGI_6,m_beamHaloParticleBuffer,norm_BGI,0);
  fillBuffer(m_input_BGI_7,m_beamHaloParticleBuffer,norm_BGI,0);
  fillBuffer(m_input_BGI_8,m_beamHaloParticleBuffer,norm_BGI,0);
  fillBuffer(m_input_BGI_9,m_beamHaloParticleBuffer,norm_BGI,0);
  fillBuffer(m_input_BGI_10,m_beamHaloParticleBuffer,norm_BGI,0);

  fillBuffer(m_input_BGE_1,m_beamHaloParticleBuffer,norm_BGE,1);
  fillBuffer(m_input_BGE_2,m_beamHaloParticleBuffer,norm_BGE,1);
  fillBuffer(m_input_BGE_3,m_beamHaloParticleBuffer,norm_BGE,1);
  fillBuffer(m_input_BGE_4,m_beamHaloParticleBuffer,norm_BGE,1);
  fillBuffer(m_input_BGE_5,m_beamHaloParticleBuffer,norm_BGE,1);
  fillBuffer(m_input_BGE_6,m_beamHaloParticleBuffer,norm_BGE,1);
  fillBuffer(m_input_BGE_7,m_beamHaloParticleBuffer,norm_BGE,1);
  fillBuffer(m_input_BGE_8,m_beamHaloParticleBuffer,norm_BGE,1);
  fillBuffer(m_input_BGE_9,m_beamHaloParticleBuffer,norm_BGE,1);
  fillBuffer(m_input_BGE_10,m_beamHaloParticleBuffer,norm_BGE,1);

  fillBuffer(m_input_BH_1,m_beamHaloParticleBuffer,norm_BH,2);
  fillBuffer(m_input_BH_2,m_beamHaloParticleBuffer,norm_BH,2);
  fillBuffer(m_input_BH_3,m_beamHaloParticleBuffer,norm_BH,2);
  fillBuffer(m_input_BH_4,m_beamHaloParticleBuffer,norm_BH,2);
  fillBuffer(m_input_BH_5,m_beamHaloParticleBuffer,norm_BH,2);
  fillBuffer(m_input_BH_6,m_beamHaloParticleBuffer,norm_BH,2);
  fillBuffer(m_input_BH_7,m_beamHaloParticleBuffer,norm_BH,2);
  fillBuffer(m_input_BH_8,m_beamHaloParticleBuffer,norm_BH,2);
  fillBuffer(m_input_BH_9,m_beamHaloParticleBuffer,norm_BH,2);
  fillBuffer(m_input_BH_10,m_beamHaloParticleBuffer,norm_BH,2);

  m_input_BGI_1->close(); 
  m_input_BGI_2->close(); 
  m_input_BGI_3->close(); 
  m_input_BGI_4->close(); 
  m_input_BGI_5->close(); 
  m_input_BGI_6->close(); 
  m_input_BGI_7->close(); 
  m_input_BGI_8->close(); 
  m_input_BGI_9->close(); 
  m_input_BGI_10->close(); 

  m_input_BGE_1->close();
  m_input_BGE_2->close();
  m_input_BGE_3->close();
  m_input_BGE_4->close();
  m_input_BGE_5->close();
  m_input_BGE_6->close();
  m_input_BGE_7->close();
  m_input_BGE_8->close();
  m_input_BGE_9->close();
  m_input_BGE_10->close();

  m_input_BH_1->close();
  m_input_BH_2->close();
  m_input_BH_3->close();
  m_input_BH_4->close();
  m_input_BH_5->close();
  m_input_BH_6->close();
  m_input_BH_7->close();
  m_input_BH_8->close();
  m_input_BH_9->close();
  m_input_BH_10->close();

  // The buffer is full, put it into READING mode
  m_beamHaloParticleBuffer->close();  
  m_beamHaloParticleBuffer->openForReading();
}


//
// The step we process for each event
//

int MarsHaloGenerator::fillEvt(edm::Event *event) 
{
  
  evt = new HepMC::GenEvent(); // The event we will produce

  // Read one random event from the buffer.

  std::vector<BeamHaloParticle> beamHaloEvent;
  if(!readEvent(&beamHaloEvent)) return -1;
  

  // Convert the particles to GenParticles and attach them to the
  // event.  Flip the event (come from the other beam) if needed.
  
  if(!BeamHaloGenerator::convertEvent(&beamHaloEvent, evt, event)) return -1;
  
  // Set the event number
  evt->set_event_number(m_eventNumber);
  
  
  // Set the weights for this event (1 because the event is unweighted by the buffer)
  evt->weights().push_back(1.0);
  
  m_eventNumber++;

  //evt->print();
  
  std::unique_ptr<HepMCProduct> CMProduct(new HepMCProduct());
  
  if (evt) CMProduct->addHepMCData(evt);
  event->put(std::move(CMProduct), "unsmeared");

  std::unique_ptr<GenEventInfoProduct> genEventInfo(new GenEventInfoProduct(evt));
  event->put(std::move(genEventInfo));

  return 1;
}



//
// Finalization of the run 
//

void MarsHaloGenerator::finalize()
{
  BeamHaloGenerator::finalize();

  m_beamHaloParticleBuffer->close();

  std::cout << "=================================================================" << std::endl;
  std::cout << "|                                 | Total Number | Total Weight |" << std::endl;
  std::cout << "|---------------------------------------------------------------|" << std::endl;
  std::cout << "| Particles read from input file  | ";
  std::setw(12);
  std::cout << m_nRead << " | ";
  std::setw(12);
  std::setprecision(8);
  std::cout << m_wRead << " |" << std::endl;
  std::cout << "| Particles after cuts            | ";
  std::setw(12);
  std::cout << m_nAfterCuts << " | ";
  std::setw(12);
  std::setprecision(8);
  std::cout << m_wAfterCuts << " |" << std::endl;
  std::cout << "| Particles generated             | ";
  std::setw(12);
  std::cout << m_nGenerated << " | ";
  std::setw(12);
  std::setprecision(8);
  std::cout << m_wGenerated << " |" << std::endl;
  std::cout << "| MIB period generated (in seconds)     | ";
  std::setw(12);
  std::setprecision(3);
  std::cout << static_cast<double>(m_nGenerated)/m_wAfterCuts << " | ";
  std::cout << "=================================================================" << std::endl;
  
  std::setprecision(6);
}


//------------------------------------------------------------------
// 
// A method to read a random event in the buffer
// 
//------------------------------------------------------------------


int MarsHaloGenerator::readEvent(std::vector<BeamHaloParticle> *beamHaloEvent) 
{
  BeamHaloParticle *beamHaloParticle;
  
  // Clear the event
  beamHaloEvent->clear();
 
  // Read one particle at random from the binary buffer.  This uses
  // the particle weights to produce a flat distribution rather than a
  // weighted one, but may generate the same particle twice.
  beamHaloParticle = m_beamHaloParticleBuffer->readRandomParticle();

  if(!beamHaloParticle) return 0;
  
  // Increment generated particles information.
  m_nGenerated++;
  m_wGenerated+=beamHaloParticle->weight();

 
  // Copy the BeamHaloParticle into this event
  beamHaloEvent->push_back(*beamHaloParticle);
 
  // Delete the pointer to the generated particle.
  delete beamHaloParticle; 
  
  return 1;
}




//------------------------------------------------------------------

void MarsHaloGenerator::fillBuffer(AsciiInput* my_input, BeamHaloParticleBuffer* my_buffer, double norm_fac, int type)
{
  MarsParticle     marsParticle;
  BeamHaloParticle beamHaloParticle;


  // Loop over all lines in the text file.
  std::vector<std::string> row;
  
  do 
  {
    bool eof = false;
    // Read one row of the ASCII file.
    tie(row, eof) = my_input->readRow();
    if(row.size() == 0) continue;
 
    // Fill a MarsParticle with values from the string vector.
    if(marsParticle.read(&row,norm_fac,type)) continue;

    const HepPDT::ParticleData* particleData = m_particleTable->particle(marsParticle.pdgId());

    if(!particleData)
    {
      std::cerr << "PDG code " << marsParticle.pdgId() << " is not in the particle data table." << std::endl;
      continue;
    }

    // Fill the BeamHaloParticle with the data in the MarsParticle
    if(beamHaloParticle.fill(particleData, &marsParticle)) 
    {
      std::cerr << "Conversion from MarsParticle to BeamHaloParticle failed." << std::endl;
      continue;
    }

    // Increment the particles read from file information.
    m_nRead++;
    m_wRead+=beamHaloParticle.weight();
    
    // Check the generator settings.  If this particle fails the selection, skip it.
    if(!m_BHG_settings->checkParticle(&beamHaloParticle)) continue;

    // Increment the particles after cuts information.
    m_nAfterCuts++;
    m_wAfterCuts+=beamHaloParticle.weight();
    
    // Write the BeamHaloParticle into the binary buffer.
    if(!my_buffer->writeParticle(&beamHaloParticle)) 
    {
      std::cerr << "writeParticle failed." << std::endl;
    }
  } while(row.size() != 0);

}
