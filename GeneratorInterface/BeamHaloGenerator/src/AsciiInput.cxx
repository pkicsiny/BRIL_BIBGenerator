/* 
############################################################
#
# AsciiInput.cc
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>, inspired from ATLAS BH generator
#         written by W.Bell
#
# May 27th, 2010
#
# Goal: 
# Handling the text files produced by MIB simulations (MARS/FLUKA)
#
# For more info on CMS machine-induced background simulation: http://
#
#############################################################
*/

#include "GeneratorInterface/BeamHaloGenerator/interface/AsciiInput.h"


AsciiInput::AsciiInput(std::string fileName): m_fileName(fileName),
                                               m_file() 
{}

AsciiInput::AsciiInput(std::vector<std::string> fileNames): m_fileNames(fileNames),
						m_file()
{} 
//-------------------------------------------------------------------------
 
AsciiInput::~AsciiInput() 
{}
 
//-------------------------------------------------------------------------

// Open single file

int AsciiInput::open() 
{
  //debug
  //std::cout << "AsciiInput::open()" << std::endl;

  m_file.open(m_fileName.c_str());
  if(!m_file) 
  {
    std::cerr << "Error: could not open " << m_fileName << std::endl;
    return 1;
  }  
  return 0;
}



// Open one file in the list
int AsciiInput::open(int idx)
{
  //debug
  //std::cout << "AsciiInput::open(" << idx << ")" << std::endl;

  // Sanity check
  if(!(idx >= 0 and idx < static_cast<int>(m_fileNames.size()))){
  	std::cerr << "Error: invalid index" << std::endl;
	return 1;
  }
  m_file.open(m_fileNames[idx].c_str());
  if(!m_file)
  {
    std::cerr << "Error: could not open " << m_fileNames[idx] << std::endl;
    return 1;
  }
  return 0;
}

int AsciiInput::close()
{
  //debug
  //std::cout << "AsciiInput::close" << std::endl;

  if(m_file.is_open())
  {
    m_file.close();
  }
  return 0;
}

//-------------------------------------------------------------------------

std::tuple<std::vector<std::string>, bool> AsciiInput::readRow() 
{
  
  //debug
  //std::cout << "AsciiInput::readrow" << std::endl;
 
  std::vector<std::string> row;

  // Check if the end of file has been reached.
  if(m_file.eof()) 
  {
    std::cout << "End of file reached"  << std::endl;
    return std::make_tuple(row, true);
  }
 
  // Read one line from the input file.  (Need one additional character
  // for string termination of a word.)
  
  m_file.getline(m_lineBuffer,MAX_LINE_LENGTH-1);
  if(m_file.eof()) { // True when normally reaching the end of the file.
    std::cout << "EOF after getline" << std::endl;
    return std::make_tuple(row, true); 
  }
   
  // Convert the cstring to a string.
  std::string tmpString(m_lineBuffer);
  std::cout << "row: " << tmpString << std::endl;
  //std::cout << "length of row: " << tmpString.size() << std::endl;
  // Split the string into substrings.
  return std::make_tuple(strToStrVec(tmpString), false);
}
 
//-------------------------------------------------------------------------

std::vector<std::string> AsciiInput::strToStrVec(std::string inputString) 
{
  //debug
  //std::cout << "AsciiInput::strToStrVec" << std::endl;

  std::vector<std::string> strVec;
  std::string tmpString;
  size_t stringLength, i;
  
  stringLength = inputString.length();
  //std::cout << "length of string: " << stringLength << std::endl; 
  // Loop over all characters in the string.
  i=0;
  while(i<stringLength) 
  {
  
    //std::cout << "index in current line: " << i << std::endl;
    //Skip over any white spaces at the start of the string.
    while(inputString[i] == ' ' || 
          inputString[i] == '\t') {
      i++;
      if(i >= stringLength) break;
    }
    
    if(i>=stringLength) continue;
    
    // Copy all non-white space characters until a white space, end of string or comment character.
    tmpString.clear();

    //std::cout << "reading word or number" << std::endl;
    //Comment lines starting with # are not read
    while(inputString[i] != ' ' &&
	  inputString[i] != '\t' && 
          inputString[i] != '#') 
      {
	//std::cout << "index: " << i << ", character: " << inputString[i] << ", tmp: " << tmpString << std::endl;
	tmpString.push_back(inputString[i]);
	i++;
	if(i >= stringLength) break;
      }
    
    // Push the string back if its length is greater than zero.
    if(tmpString.length() > 0) 
    { 
      //std::cout << "pushing word or number to vec" << std::endl;
      strVec.push_back(tmpString);
    }

    // Check if a comment was found.
    if(i < stringLength) 
    {
      if(inputString[i] == '#') 
	{
	  std::cout << "comment, breaking loop" << std::endl;
	  break;
	}
    }
  }
  
  //std::cout << "length of vector: " << strVec.size() << std::endl;  
  return strVec;
}


//-------------------------------------------------------------------------

long AsciiInput::strToLong(std::string inputString) 
{
  //debug
  //std::cout << "AsciiInput::strToLong" << std::endl;


  long longValue;
  std::istringstream inStr(inputString);
  inStr >> longValue;
  return longValue;
}
 
//-------------------------------------------------------------------------
 
double AsciiInput::strToDouble(std::string inputString) 
{
  //debug
  //std::cout << "AsciiInput::strToDouble" << std::endl;

  double doubleValue;
  std::istringstream inStr(inputString);
  inStr >> doubleValue;
  return doubleValue;
}
