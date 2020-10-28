/*
############################################################
#
# BeamHaloProducer.cc
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>, inspired from ATLAS BH generator
#         written by W.Bell
#
# May 27th, 2010
#
# Goal:
# Produce beam background events based on MARS15 or FLUKA simulation files
#
# Input parameters are:
#
# Link to the original ATLAS code:
#
# http://alxr.usatlas.bnl.gov/lxr/source/atlas/Generators/BeamHaloGenerator/
#
# For more info on CMS machine-induced background simulation:
#
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB
#
#############################################################
*/



#include "GeneratorInterface/BeamHaloGenerator/interface/BeamHaloProducer.h"
#include "GeneratorInterface/BeamHaloGenerator/interface/BeamHaloGenerator.h"
#include "GeneratorInterface/BeamHaloGenerator/interface/MarsHaloGenerator.h"
#include "GeneratorInterface/BeamHaloGenerator/interface/FlukaHaloGenerator.h"

using namespace std;
using namespace edm;

BeamHaloProducer::~BeamHaloProducer()
{}

//
// Constructor
//

BeamHaloProducer::BeamHaloProducer( const ParameterSet & pset) :
	m_inputTypeStr (pset.getParameter<std::string>("InputType")),
	//m_inputFiles    (pset.getParameter<std::string>("InputFile")),
        m_inputFiles    (pset.getParameter<std::vector<std::string>>("FlukaFiles")),
	m_interfacePlane(22600.),
	m_flipProbability(0.5),
	m_flipEventEnabled(false),
	m_generatorSettings(),
	m_binaryBufferFile("BinaryBuffer.bin"),
	m_beamHaloGenerator(0)
{
	
	//debug
	//std::cout << "BeamHaloProducer::constructor" << std::endl;

	// Read the input configuration information
	m_generatorSettings = pset.getUntrackedParameter<std::vector<std::string> >("generatorSettings");

	std::cout << "BeamHaloProducer: starting event generation ... " << std::endl;

	//cout << "We will use " << m_inputTypeStr << " input ... " << endl;
	for(auto fileIt = m_inputFiles.begin(); fileIt != m_inputFiles.end(); ++fileIt){
        	std::cout << "with " << *fileIt << " datafile ... " << std::endl;
	}
	//cout << "with " << m_inputFile << " datafile ... " << endl;
	
	//m_generatorSettings

	produces<HepMCProduct>("unsmeared");
	produces<GenEventInfoProduct>();
	produces<GenRunInfoProduct, edm::Transition::EndRun>();

	std::cout << "BeamHaloProducer constructor finished." << std::endl;
}


//
// Initialization of the run
//

//void BeamHaloProducer::beginRun( Run &run, const EventSetup& es )
//void BeamHaloProducer::beginRun( Run &run, const EventSetup& es )
void BeamHaloProducer::beginLuminosityBlock(LuminosityBlock const& lumi, EventSetup const& es) 
{
	std::cout << "Debug info:BeamHaloProducer::beginLuminosityBlock" << std::endl;
	// Check the input type string we choose which input we will use

	if (m_inputTypeStr == "MARS")
	{
		m_beamHaloGenerator = new MarsHaloGenerator(this,&es);
	}
	else if (m_inputTypeStr == "FLUKA")
	{
		m_beamHaloGenerator = new FlukaHaloGenerator(this,&es);
	}
	else
	{
		std::cout << m_inputTypeStr << " is not known.  Available types are: MARS or FLUKA" << std::endl;
		return;
	}
	std::cout << m_inputTypeStr << " Halo Generator constructed successfully" << std::endl;

	// Initialise the generator.

	m_beamHaloGenerator->initialize();
	std::cout << m_inputTypeStr << " Halo Generator initialized successfully" << std::endl;
}


//
// The step we process for each event
//

void BeamHaloProducer::produce(Event & e, const EventSetup & es)
{
	//debug
	//std::cout << "BeamHaloProducer::produce" << std::endl;
	m_beamHaloGenerator->fillEvt(&e);
}



//
// Finalization of the run
//

void BeamHaloProducer::endRunProduce(edm::Run& run, edm::EventSetup const& es){
	
	//debug
	//std::cout << "BeamHaloProducer::endRunProduce" << std::endl;

	m_beamHaloGenerator->finalize();

	// just create an empty product
	// to keep the EventContent definitions happy
	// later on we might put the info into the run info that this is a PGun
	std::unique_ptr<GenRunInfoProduct> genRunInfo( new GenRunInfoProduct() );
	run.put( std::move(genRunInfo ));
}
