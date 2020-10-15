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
	m_flukaParticle(),
	m_lastFlukaParticle()
{
	cout << "FlukaHaloGenerator: starting event generation ... " << endl;
}



//
// Initialization of the run
//

void FlukaHaloGenerator::initialize()
{
	BeamHaloGenerator::initialize();

	// -- initialisation

	// Open the FLUKA text file containing theMIB event info for CMS

	m_input = new AsciiInput(m_inputFile);

	if(m_input->open() != 0)
	{
		cout << "Can't open FLUKA input file " << m_inputFile << " ... " << endl;
		return;
	}

	cout << "Reading FLUKA file " << m_inputFile << " ... " << endl;
}


//
// The step we process for each event
//

int FlukaHaloGenerator::fillEvt(edm::Event *event)
{
	std::cout << "Fluka Halo Gen::fillEvt" << std::endl;

	evt = new HepMC::GenEvent(); // The event we will produce

	std::vector<BeamHaloParticle> beamHaloEvent;

	// Read one FLUKA event passing the selection cuts.
	if(!readEvent(&beamHaloEvent)) return -1;


	// Convert the particles to GenParticles and attach them to the
	// event.  Flip the event (come from the other beam) if needed.
	if(!BeamHaloGenerator::convertEvent(&beamHaloEvent, evt, event)) return -1;

	// Set the event number
	evt->set_event_number(m_eventNumber);


	// Set the weights for this event (1 because the event is unweighted by the buffer)
	evt->weights().push_back(1.0);

	m_eventNumber++;

	evt->print();

	//std::auto_ptr<HepMCProduct> CMProduct(new HepMCProduct());
	std::unique_ptr<HepMCProduct> CMProduct(new HepMCProduct());

	if (evt) CMProduct->addHepMCData(evt);
	event->put(std::move(CMProduct));

	//auto_ptr<GenEventInfoProduct> genEventInfo(new GenEventInfoProduct(evt));
	std::unique_ptr<GenEventInfoProduct> genEventInfo(new GenEventInfoProduct(evt));
	event->put(std::move(genEventInfo));

	return 1;
}



//
// Finalization of the run
//

void FlukaHaloGenerator::finalize()
{
	BeamHaloGenerator::finalize();

	m_input->close();

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
	BeamHaloParticle beamHaloParticle;
	const HepPDT::ParticleData* particleData;

	// Clear the event
	beamHaloEvent->clear();

	// If there was a last event.
	if(!m_firstEvent)
	{
		particleData = m_particleTable->particle(m_lastFlukaParticle.pdgId());

		// If the last event caused the same event flag to be set to false
		// copy the last particle into the vector of those in this event.
		if(!m_sameEvent)
		{
			// Fill the BeamHaloParticle with the data in the FlukaParticle
			if(beamHaloParticle.fill(particleData, &m_lastFlukaParticle))
			{
				std::cout << "Conversion from FlukaParticle to BeamHaloParticle failed." << std::endl;
				return 0;
			}

			// Append the BeamHalo particle to the event if it passes the cuts.
			if(m_BHG_settings->checkParticle(&beamHaloParticle))
			{
				beamHaloEvent->push_back(beamHaloParticle);
			}

			// Set the same event flag to enter the while loop to read the
			// rest of this event.
			m_sameEvent = true;
		}
	}

	// Loop over the ascii input and read each particle until a new
	// event is found or there are no more particles.
	std::vector<std::string> row;
	bool endOfFile = false;

	while(m_sameEvent && !endOfFile)
	{
		row = m_input->readRow(); // Read one line of the ascii file

		if(row.size() == 0)
		{
			endOfFile = true;
			continue;
		}

		// Fill the particle from the string vector
		if(m_flukaParticle.read(&row)) continue;

		particleData = m_particleTable->particle(m_flukaParticle.pdgId());

		if(!particleData)
		{
			std::cerr << "PDG code " << m_flukaParticle.pdgId() << " is not in the particle data table." << std::endl;
			continue;
		}

		// Check if there was a last particle.
		if(!m_firstEvent)
		{
			// Check if the event id of the last particle is the same as this particle.
			if(m_lastFlukaParticle.eventId() == m_flukaParticle.eventId())
			{
				m_sameEvent = true;
			}
			else
			{
				m_sameEvent = false;
			}
		}
		else
		{
			// For the first event.
			m_firstEvent = false;
			m_sameEvent = true;
		}

		// If this is the same event copy the particle into the vector for
		// this event.
		if(m_sameEvent)
		{
			// Fill the BeamHaloParticle with the data in the FlukaParticle
			if(beamHaloParticle.fill(particleData, &m_flukaParticle))
			{
				cout << "Conversion from FlukaParticle to BeamHaloParticle failed." << endl;
				return 0;
			}

			// Append the BeamHalo particle to the event if it passes the cuts.
			if(m_BHG_settings->checkParticle(&beamHaloParticle))
			{
				beamHaloEvent->push_back(beamHaloParticle);
			}
		}

		// Copy this particle into the last particle.
		m_lastFlukaParticle = m_flukaParticle;
	}

	//  cout << "Number of particles " << beamHaloEvent->size() << endl;


	if(beamHaloEvent->size() == 0)
	{
		//cout << "No particles read from " << m_inputFile << endl;
		if(!FlukaHaloGenerator::readEvent(beamHaloEvent)) return 0;
		//return 0;
	}

	m_nRead++;
	m_wRead++;

	// Check if one of the particles in the event passes the generator settings.
	std::vector<BeamHaloParticle>::iterator itr = beamHaloEvent->begin();
	std::vector<BeamHaloParticle>::iterator itr_end = beamHaloEvent->end();

	bool passed = false;
	for(;itr!=itr_end;++itr)
	{
		// Check the generator settings for this particle.
		if(m_BHG_settings->checkParticle(&(*itr)))
		{
			passed = true;
		}
	}

	m_nAfterCuts++;
	m_wAfterCuts++;
	m_nGenerated++;
	m_wGenerated++;

	// If all of the particles from this event fail read another event.
	// If there are no more events this function will exit with a
	// WARNING.

	if(!passed)
	{
		if(!FlukaHaloGenerator::readEvent(beamHaloEvent)) return 0;
	}


	// Delete the pointer to the generated particle.
	//delete beamHaloParticle;

	return 1;
}


