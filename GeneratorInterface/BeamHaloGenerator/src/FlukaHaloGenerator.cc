/*
############################################################
#
# FlukaHaloGenerator.cc
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


#include "GeneratorInterface/BeamHaloGenerator/interface/FlukaHaloGenerator.h"

using namespace edm;
using namespace std;



FlukaHaloGenerator::~FlukaHaloGenerator()
{}

//
// Constructor
//

FlukaHaloGenerator::FlukaHaloGenerator(edm::BeamHaloProducer *mainAlg, const edm::EventSetup *setup) :
	BeamHaloGenerator(mainAlg,setup),
	m_sameEvent(true),
	m_firstEvent(true),
	m_fileIndex(0),
	m_flukaParticle(),
	m_lastFlukaParticle()
{
	//debug
	//std::cout << "FlukaHaloGenerator::constructor" << std::endl;
	std::cout << "FlukaHaloGenerator: starting event generation ... " << std::endl;
}



//
// Initialization of the run
//

void FlukaHaloGenerator::initialize()
{
	//debug
	//std::cout << "FlukaHaloGenerator::initialize" << std::endl;
	
	BeamHaloGenerator::initialize();

	// -- initialisation

	//Asciiinput of a vector of strings
	//m_input = new AsciiInput(m_inputFile);
        m_inputs = new AsciiInput(m_inputFiles);

	//Open the first FLUKA text file containing the MIB event info for CMS
	//now m_file is the first file
	if(m_inputs->open(0) != 0)
	{
		cout << "Can't open FLUKA input file " << m_inputFiles[0] << " ... " << endl;
		return;
	}
	
	std::cout << "Reading FLUKA file" << m_inputFiles[0] << " ... " << std::endl;
}



//
// The step we process for each event
//

int FlukaHaloGenerator::fillEvt(edm::Event *event)
{
	//debug
	std::cout << "FlukaHaloGenerator::fillEvt" << std::endl;
	
	evt = new HepMC::GenEvent(); // The event we will produce

	std::vector<BeamHaloParticle> beamHaloEvent;

	// Read one FLUKA event passing the selection cuts.
	if(!readEvent(&beamHaloEvent)) return -1;
	std::cout << "Successfully read one event." << std::endl;
	
	// Convert the particles to GenParticles and attach them to the
	// event.  Flip the event (come from the other beam) if needed.
	if(!BeamHaloGenerator::convertEvent(&beamHaloEvent, evt, event)) return -1;
	std::cout << "Successfully converted event." << std::endl;
	// Set the event number
	evt->set_event_number(m_eventNumber);
	std::cout << "Set event number: " << m_eventNumber << std::endl;

	// Set the weights for this event (1 because the event is unweighted by the buffer)
	evt->weights().push_back(1.0);
	m_eventNumber++;

	//evt->print();

	//std::auto_ptr<HepMCProduct> CMProduct(new HepMCProduct());
	std::unique_ptr<HepMCProduct> CMProduct(new HepMCProduct());

	if (evt) CMProduct->addHepMCData(evt);
	event->put(std::move(CMProduct),"unsmeared");
	//event->put(std::move(CMProduct));
	//auto_ptr<GenEventInfoProduct> genEventInfo(new GenEventInfoProduct(evt));
	std::unique_ptr<GenEventInfoProduct> genEventInfo(new GenEventInfoProduct(evt));
	event->put(std::move(genEventInfo));
        std::cout << "Finished filling event." << std::endl;
	return 1;
}



//
// Finalization of the run
//

void FlukaHaloGenerator::finalize()
{
	//debug
	//std::cout << "FlukaHaloGenerator::finalize" << std::endl;

	BeamHaloGenerator::finalize();
	
	//Close ifstream (currently open file, which is the last one) 
	m_inputs->close();

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


int FlukaHaloGenerator::readEvent(std::vector<BeamHaloParticle> *beamHaloEvent)
{
	//debug
	//std::cout << "FlukaHaloGenerator::readEvent" << std::endl;

	BeamHaloParticle beamHaloParticle;
	const HepPDT::ParticleData* particleData;

	// Clear the event
	beamHaloEvent->clear();

	// If there was a last event.
	if(!m_firstEvent)
	{
		std::cout << "This is not the first event outside while." << std::endl;
		particleData = m_particleTable->particle(m_lastFlukaParticle.pdgId());

		// If the last event caused the same event flag to be set to false
		// copy the last particle into the vector of those in this event.
		if(!m_sameEvent)
		{
			std::cout << "This is also not the same event outside while." << std::endl;
			// Fill the BeamHaloParticle with the data in the FlukaParticle
			if(beamHaloParticle.fill(particleData, &m_lastFlukaParticle))
			{
				std::cout << "Conversion from FlukaParticle to BeamHaloParticle failed." << std::endl;
				return 0;
			}
			
            		//successfully read one particle
                        m_nRead++;
                        m_wRead++;

			// Append the BeamHalo particle to the event if it passes the cuts.
			if(m_BHG_settings->checkParticle(&beamHaloParticle))
			{
				std::cout << "Push particle to beamhaloevent." << std::endl;
				beamHaloEvent->push_back(beamHaloParticle);
			        //+1 particle passed cuts
				m_nAfterCuts++;
        			m_wAfterCuts++;
      			        m_nGenerated++;
        			m_wGenerated++;
			}

			// Set the same event flag to enter the while loop to read the
			// rest of this event.
			std::cout << "Set SE = true" << std::endl;
			m_sameEvent = true;
		}
	}

	// Loop over the ascii input and read each particle until a new
	// event is found or there are no more particles.
	std::vector<std::string> row;
	bool endOfFile = false;
        std::string thisEvent = "0";
	std::string lastEvent = "0";
	std::cout << "before while: SE =" << m_sameEvent << " and FE =" << m_firstEvent << std::endl;
	while(m_sameEvent && !endOfFile)
	{
	
		// Read one line of the currently open ascii file
		tie(row, endOfFile) = m_inputs->readRow();
		
		//read new file
                if(endOfFile)
		{
			
			//Close ifstream (currently open file) 
		        m_inputs->close();
			std::cout << "Closed file index " << m_fileIndex << std::endl;
			
			m_fileIndex += 1;
			//Break loop if processed last file
			if(m_fileIndex == static_cast<int>(m_inputFiles.size()))
			{	
				std::cout << "No more files, break loop." << std::endl;
				//endOfFile = true;
				return -1;
				//continue;
			}
			
			//Return if cannot open new file
			if(m_inputs->open(m_fileIndex) != 0)
      			{
                		std::cout << "Can't open FLUKA input file " << m_inputFiles[m_fileIndex] << " at list index " << m_fileIndex << " ... " << std::endl;
                		return -1;
      			}

       			std::cout << "Reading FLUKA file" << m_inputFiles[m_fileIndex] << " ... " << std::endl;	
		}

		//Check if it is an empty row, usually at the beginning of the file, starting with #
		if(row.size() == 0)
		{
			std::cout << "Invalid row. Sipping to next row..." << std::endl;
			//endOfFile = true
			continue;
		}
		
		//store event number
		thisEvent = row[0];

		//only runs if row not empty
		// Fill the particle from the string vector
		if(m_flukaParticle.read(&row)) continue;

		particleData = m_particleTable->particle(m_flukaParticle.pdgId());

		if(!particleData)
		{
			std::cerr << "PDG code " << m_flukaParticle.pdgId() << " is not in the particle data table." << std::endl;
			continue;
		}
	
		//set flags
		if(!m_firstEvent)
		{
			std::cout << "This is not the first event." << std::endl;
			// Check if the event id of the last particle is the same as this particle.
			if(m_lastFlukaParticle.eventId() == m_flukaParticle.eventId())
			{
				std::cout << "This particle is from the same event - set SE = true" << std::endl;
				m_sameEvent = true;
			}
			else
			{
				std::cout << "This particle is not from the same event: set SE = false, break loop without checks." << std::endl;
				m_sameEvent = false;
			}
		}
		else
		{
			// For the first event.
			std::cout << "This is the first event: set FE = false and SE = true" << std::endl;
			m_firstEvent = false;
			m_sameEvent = true;
		}

		// If this is the same event copy the particle into the vector for
		// this event.
		if(m_sameEvent)
		{
			std::cout << "This is still event " << thisEvent << "." << std::endl;
			// Fill the BeamHaloParticle with the data in the FlukaParticle
			if(beamHaloParticle.fill(particleData, &m_flukaParticle))
			{
				cout << "Conversion from FlukaParticle to BeamHaloParticle failed." << endl;
				return 0;
			}
              		
			//successfully read one particle
                        m_nRead++;
                        m_wRead++;

			// Append the BeamHalo particle to the event if it passes the cuts.
			if(m_BHG_settings->checkParticle(&beamHaloParticle))
			{
				std::cout << "Push particle to beamhaloevent." << std::endl;
				beamHaloEvent->push_back(beamHaloParticle);
			        //+1 particle passed cuts
				m_nAfterCuts++;
   			        m_wAfterCuts++;
        			m_nGenerated++;
        			m_wGenerated++;
			}
			else
			{
				std::cout << "Particle didn't pass cuts." << std::endl;
			}
	
			//save last event number
			lastEvent = thisEvent;
		}
	
		// Copy this particle into the last particle.
		m_lastFlukaParticle = m_flukaParticle;
	}
	
	//After while loop (new event reached)
        std::cout << "Exited while loop with FE =" << m_firstEvent << ", SE = " << m_sameEvent << ", EOF = " << endOfFile << "." << std::endl;	
	cout << "Number of particles in current beam halo event vector so far: " << beamHaloEvent->size() << endl;
	
	//happens when beginning of file with # lines or when nothing passes checks from latest event
	if(beamHaloEvent->size() == 0 && !endOfFile)
	{
		//when truly end of file open new file
	 	std::cout << " No particles in this event: " << lastEvent << ". Recursively read event " << thisEvent << std::endl;
		// Recursively read lines - when first lines start with #
		if(!FlukaHaloGenerator::readEvent(beamHaloEvent)) return 0;
		std::cout << "Event " << thisEvent << " read. Exited recursion." << std::endl;
		//return 0;
	}
	
	//otherwise if there is a new event make checks and call readevent afterwards

	// Check if one of the particles in the event passes the generator settings.
	std::vector<BeamHaloParticle>::iterator itr = beamHaloEvent->begin();
	std::vector<BeamHaloParticle>::iterator itr_end = beamHaloEvent->end();

	bool passed = false;
	std::cout << "Running a check on all " << beamHaloEvent->size() << " particles of current event." << endl;
	for(;itr!=itr_end;++itr)
	{
		// Check the generator settings for this particle.
		if(m_BHG_settings->checkParticle(&(*itr)))
		{
			//std::cout << "this particle passed " << std::endl;
			passed = true;
		}
	}

	// If all of the particles from this event fail read another event.
	// If there are no more events this function will exit with a
	// WARNING.

	if(!passed)
	{
		std::cout << "None passed." << std::endl;
		if(!FlukaHaloGenerator::readEvent(beamHaloEvent)) return 0;
	}


	// Delete the pointer to the generated particle.
	//delete beamHaloParticle;

	return 1;
}


