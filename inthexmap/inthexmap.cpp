/* Opens an Intel Hex file and reads it in line by line */
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream> // for string streaming
#include <string>
#include <cstdlib>
#include <vector>
#include <map>

char *program_name;

// Usage for this program
void usage()
{
    std::cerr << "Usage is " << program_name <<
                 " [file]" << std::endl;
    exit (8);
}

unsigned char charToHex(char value)
{
    //std::cout << "In charToHex()" << std::endl;
    
    if (value >= '0' && value <= '9')
    {
        //std::cout << "A:Found " << value << " returning " << static_cast<unsigned int>(value - '0') << std::endl;
        return static_cast<unsigned char>(value - '0');
    }
    else if (value >= 'A' && value <= 'F')
    {
        //std::cout << "B:Found " << value << " returning " << static_cast<unsigned int>(value - 'A' + 10) << std::endl;
        return static_cast<unsigned char>(value - 'A' + 10);
    }
    else if (value >= 'a' && value <= 'f')
    {
        //std::cout << "C:Found " << value << " returning " << static_cast<unsigned int>(value - 'a' + 10) << std::endl;
        return static_cast<unsigned char>(value - 'a' + 10);
    }
    else
    {
        std::cerr << "Error: In charToHex()" << std::endl;
        return 0xFF;
    }
}

int main(int argc, char *argv[])
{	
	// Create an input stream
	std::ifstream intelHexInput;
	// Create a string to store lines of Intel Hex info
	std::string intelHexLine;
	
	// The program name is the first argument - save for later use
	program_name = argv[0];

	// Make sure there is only <command> and <file> arguments	
	if(argc != 2) {
    	usage();
    }
    	
	intelHexInput.open(argv[1], std::ifstream::in);
	
	if(intelHexInput.bad())
	{
    	std::cerr << "Error: couldn't open " << argv[1] << std::endl;
    	usage();
	}

    int totalLines = 0;
    int hexAddress = 0;
    std::vector<unsigned char> lineData;
    std::map<int, unsigned char> mymap;
    std::map<int, unsigned char>::iterator it;
    std::pair<std::map<int, unsigned char>::iterator,bool> ret;
    
    do {
        std::string::iterator it;
        
        intelHexInput >> intelHexLine;
        ++totalLines;
        
        // Set string iterator to start of string
        it = intelHexLine.begin();
        
        // Test first position for the start code ':'
        if (*it != ':') {
            std::cout << "Error: Start code \':\' missing on line " << std::dec << totalLines << std::endl;
            break;
        }
        
        // Remove the start code from the string as we don't need it anymore
        intelHexLine.erase(it);
        
        unsigned char intelHexChecksum = 0;
        
        std::cout << std::dec << std::setw(6) << totalLines << ":" << intelHexLine << std::endl << "      :" ;
        
        for (it = intelHexLine.begin(); it != intelHexLine.end(); /* nothing */ ) {
            unsigned char upperNibble = 0xFF;
            unsigned char lowerNibble = 0xFF;
            std::string byteReadTemp;
            unsigned char byteRead = 0;
            
            upperNibble = charToHex(static_cast<char>(*it));
            byteReadTemp = *it;
            
            ++it;
            
            lowerNibble = charToHex(*it);
            byteReadTemp += *it;
            
            ++it;
            
            if (upperNibble > 0x0F) {
                std::cout << "Error: Upper nibble in line " << std::dec <<  totalLines << " of byte " << std::hex << byteReadTemp << " not hexedecimal value." << std::endl;
            } else if (lowerNibble > 0x0F) {
                std::cout << "Error: Lower nibble in line " << std::dec << totalLines << " of byte " << std::hex << byteReadTemp << " not hexedecimal value." << std::endl;
            } else {
                byteRead = (upperNibble << 4) + lowerNibble;
            }
            intelHexChecksum += byteRead;
            //std::cout << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(byteRead) << " ";
            std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(byteRead);
            byteReadTemp.erase();
        }
        
#if 0
        std::istringstream myStream(intelHexLine, std::istringstream::in);
        
        unsigned char intelHexChecksum = 0;
        
        while (myStream.good()) {
            unsigned char tempData = 0;
            unsigned char tempData2 = 0;
            
            //myStream.width(4);
            
            //myStream >> tempData;
            //myStream >> std::cout;
            
            tempData = myStream.get();
            if (tempData >='0' && tempData <='9') {
                tempData -= '0';
            }
            else if (tempData >='a' && tempData <= 'f') {
                tempData -= ('a' + 0x0A);
            }
            else if (tempData >='A' && tempData <= 'F') {
                tempData -= ('A' + 0x0A);
            }
            else {
                std::cout << "Error: Unrecognised character 1 " << std::hex << tempData << " at line " << totalLines << std::endl;
            }
            
            tempData <<= 4;
            
            tempData2 += myStream.get();
            
            if (tempData2 >='0' && tempData2 <='9') {
                tempData2 -= '0';
            }
            else if (tempData2 >='a' && tempData2 <= 'f') {
                tempData2 -= ('a' + 0x0A);
            }
            else if (tempData2 >='A' && tempData2 <= 'F') {
                tempData2 -= ('A' + 0x0A);
            }
            else {
                std::cout << "Error: Unrecognised character 2 " << std::hex << tempData2 << " at line " << totalLines << std::endl;
            }
            
            tempData += tempData2;
            
            std::cout << std::hex << static_cast<unsigned int>(tempData) << " ";
            
            intelHexChecksum += tempData;
        }
#endif
        
        std::cout << std::endl;
        
//        std::cout << myStream.good() << " " << myStream.eof() << " " << myStream.fail() << " " << myStream.bad() << std::endl;
        
        if(intelHexChecksum != 0) {
            std::cout << "Error: Checksum error on line " << std::dec << totalLines << " of " << std::hex << static_cast<unsigned int>(intelHexChecksum) << std::endl;
        }
        
        intelHexLine.erase();
                
    } while (!intelHexInput.eof());
	
	std::cout << "Total lines = " << std::dec << totalLines << std::endl;
	
	return(0);
}
