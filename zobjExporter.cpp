#include <cstdio>
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <vector>
#include <algorithm>

using namespace std;

// Function to convert hexadecimal to decimal 
int hexadecimalToDecimal(string hexVal) 
{    
    int len = hexVal.length(); 
      
    // Initializing base value to 1, i.e 16^0 
    int base = 1; 
      
    int dec_val = 0; 
      
    // Extracting characters as digits from last character 
    for (int i=len-1; i>=0; i--) 
    {    
        // if character lies in '0'-'9', converting  
        // it to integral 0-9 by subtracting 48 from 
        // ASCII value. 
        if (hexVal[i]>='0' && hexVal[i]<='9') 
        { 
            dec_val += (hexVal[i] - 48)*base; 
                  
            // incrementing base by power 
            base = base * 16; 
        } 
  
        // if character lies in 'A'-'F' , converting  
        // it to integral 10 - 15 by subtracting 55  
        // from ASCII value 
        else if (hexVal[i]>='A' && hexVal[i]<='F') 
        { 
            dec_val += (hexVal[i] - 55)*base; 
          
            // incrementing base by power 
            base = base*16; 
        } 
    } 
      
    return dec_val; 
}

string toUpper(string str)
{
	std::transform(str.begin(), str.end(),str.begin(),::toupper);
	return str;
}

template< typename T >
std::string int_to_hex( T i )
{
  std::stringstream stream;
  stream << std::setfill ('0') << std::setw(sizeof(T)*2) 
         << std::hex << i;
  return stream.str();
}

void findAndReplaceAll(std::string & data, std::string toSearch, std::string replaceStr)
{
	// Get the first occurrence
	size_t pos = data.find(toSearch);
 
	// Repeat till end is reached
	while( pos != std::string::npos)
	{
		// Replace this occurrence of Sub String
		data.replace(pos, toSearch.size(), replaceStr);
		// Get the next occurrence from the current position
		pos =data.find(toSearch, pos + replaceStr.size());
	}
}

int main()
{
	string fileName;
	string offset;
	size_t offsetInt;
	size_t lengthInt;
	string contents;
	string displayList;
	ostringstream buffer;
	vector<string> vectorVector; //so funny
	vector<string> textureVector;
	vector<string> newVectorVector;
	vector<string> newTextureVector;
	
	cout << "Enter the zobj filename and path" << endl;
	cin >> fileName;
	
	ifstream input(fileName, ios::binary);
	
	if (input.is_open())
	{
	unsigned char a;

	input >> noskipws;
	while(input >> hex >> a)
		{
		buffer << hex << std::setw(2) << std::setfill('0') << (int)a;
		}
	}
	else 
	{
		cout << "Unable to open file"; 
		return 0;
	}
	
	
	contents = buffer.str();

	cout << "Enter the display list offset (hex)" << endl;
	cin >> hex >> offset;
	offsetInt = hexadecimalToDecimal(offset) * 2; //times 2 because each position in hex has 2 bits
	//find the next "df00000" after the display list start to get length
	lengthInt = (contents.find("df00000000000000",offsetInt)+16)-offsetInt;
	displayList = contents.substr(offsetInt,lengthInt);

	size_t pos = displayList.find("01");

	while(pos != string::npos)
		{
			if((pos%16==0)||(pos%32==0))
			{
				vectorVector.push_back(displayList.substr(pos,16));
			}
			pos=displayList.find("01",pos+2);
		}
	
	//start of vector data = first offset
	//end of vector data = last offset + length of last one - 1
	
	int vectorStart = hexadecimalToDecimal(toUpper(vectorVector.front().substr(12,4)))*2;
	int vectorEnd = (hexadecimalToDecimal(toUpper(vectorVector.back().substr(12,4)))+hexadecimalToDecimal(toUpper(vectorVector.back().substr(2,4))))*2;

	string vectorData = contents.substr(vectorStart,(vectorEnd-vectorStart));

	int newVectorStartInt = displayList.length()/2;
	string newVectorStart = int_to_hex(newVectorStartInt);
	
	int newOffset;
	string tempString;

	tempString = vectorVector.front();
	tempString.replace(12,4,newVectorStart.substr(4,4));
	newVectorVector.push_back(tempString);
	newOffset = newVectorStartInt + hexadecimalToDecimal(toUpper(vectorVector.front().substr(2,4)));
	
	string replaceString;
	
	findAndReplaceAll(displayList,vectorVector.front(),newVectorVector.front());


	for(int i = 1; i < (vectorVector.size()); i++)
	{		
		tempString = vectorVector.at(i);
		replaceString=int_to_hex(newOffset).substr(4,4);
		
		tempString.replace(12,4,int_to_hex(newOffset).substr(4,4));
		newVectorVector.push_back(tempString);
		newOffset = newOffset + hexadecimalToDecimal(toUpper(vectorVector.at(i).substr(2,4)));
		findAndReplaceAll(displayList,vectorVector.at(i),newVectorVector.at(i));
	}
	
	pos=displayList.find("fd");
	while(pos!=string::npos)
		{
			if((pos%16==0)||(pos%32==0))
			{
				textureVector.push_back(displayList.substr(pos,16));
			}
			pos= displayList.find("fd",pos + 2);
		}
/*
	for(int i =0; i < textureVector.size(); i++)
	{
		cout << endl << textureVector.at(i) << endl;
	}
*/

//when we get to textures can save the fd command with duplicates but have to be careful to replace all instances of the same one

	string output;
	output = displayList + vectorData;

	// input: std::string hex; (e.g. = "180f00005e2c3415" or longer)
	std::basic_string<uint8_t> bytes;

	for (size_t i = 0; i < output.length(); i += 2) {
	    uint16_t byte;
	    std::string nextbyte = output.substr(i, 2);
	    std::istringstream(nextbyte) >> std::hex >> byte;
	    bytes.push_back(static_cast<uint8_t>(byte));
	}

	std::string result(begin(bytes), end(bytes));


	ofstream myFile;
	myFile.open("newZobj.zobj");
	

	myFile << result;
	myFile.close();


	return 0;
}
