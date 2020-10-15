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
	m_inputFile    (pset.getParameter<std::string>("InputFile")),
	m_interfacePlane(22600.),
	m_flipProbability(0.5),
	m_flipEventEnabled(false),
	m_generatorSettings(),
	m_binaryBufferFile("BinaryBuffer.bin"),
	m_beamHaloGenerator(0)
{

	// Read the input configuration information
	m_generatorSettings = pset.getUntrackedParameter<std::vector<std::string> >("generatorSettings");

	cout << "BeamHaloProducer: starting event generation ... " << endl;

	cout << "We will use " << m_inputTypeStr << " input ... " << endl;
	cout << "with " << m_inputFile << " datafile ... " << endl;
	cout << "and settings:" <<std::endl;
	for(auto setting : m_generatorSettings)
		std::cout <<setting << std::endl;

	produces<HepMCProduct>();
	produces<GenEventInfoProduct>();
	//produces<GenRunInfoProduct, InRun>();
	produces<GenRunInfoProduct, edm::Transition::EndRun>();

	cout << "BeamHaloProducer constructor finished" << endl;
}


//
// Initialization of the run
//

void BeamHaloProducer::beginRun( Run &run, const EventSetup& es )
{
	std::cout << "Beam Halo Producer::beginRun" << std::endl;
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
	m_beamHaloGenerator->fillEvt(&e);
}



//
// Finalization of the run
//

void BeamHaloProducer::endRun( Run &run, const EventSetup& es )
{
	m_beamHaloGenerator->finalize();

	// just create an empty product
	// to keep the EventContent definitions happy
	// later on we might put the info into the run info that this is a PGun
	std::unique_ptr<GenRunInfoProduct> genRunInfo( new GenRunInfoProduct() );
	run.put( std::move(genRunInfo ));
}

void BeamHaloProducer::endRunProduce(edm::Run&, edm::EventSetup const&){

}
