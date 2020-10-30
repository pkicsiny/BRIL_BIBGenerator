#ifndef ASCIIIOINPUT_H
#define ASCIIIOINPUT_H
 
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream> 
#include <tuple>
 
class AsciiInput 
{
 public:
	  AsciiInput(std::string fileName);
  AsciiInput(std::vector<std::string> fileNames);
  ~AsciiInput();
  int open();
  int open(int idx);
  int close();
  std::tuple<std::vector<std::string>, bool> readRow();
  
  static std::vector<std::string> strToStrVec(std::string inputString);
  static long strToLong(std::string inputString);
  static double strToDouble(std::string inputString);
  
 private:
  /** Input file name */
  std::string m_fileName;
  std::vector<std::string> m_fileNames;
  
  /** Input file stream */
  std::ifstream m_file;
  
  /** Size of the character buffers */
  static const int MAX_LINE_LENGTH = 500;
  
  /** Character buffers used while parsing the input file */
  char m_lineBuffer[MAX_LINE_LENGTH];
};

#endif
