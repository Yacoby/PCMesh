/*_LICENCE_BLOCK_
------------------------------------------------------------------
Copyright (c) 2007, Jacob Essex (aka Yacoby)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Jacob Essex nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY JACOB ESSEX ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL JACOB ESSEX BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------
_LICENCE_BLOCK_*/
#include "bsa.h"

//#define USE_BINARY_SEARCH

void cBSA::getHash(const std::string& sName, unsigned& hash1, unsigned& hash2){


	const char * name = sName.c_str(); // file name string (in C format)

	unsigned len = strlen(name);
	unsigned l = (len>>1);
	unsigned sum, off, temp, i, n;

	for(sum = off = i = 0; i < l; i++) {
		sum ^= (((unsigned)(name[i]))<<(off&0x1F));
		off += 8;
	}
	hash1 = sum;

	for(sum = off = 0; i < len; i++) {
		temp = (((unsigned)(name[i]))<<(off&0x1F));
		sum ^= temp;
		n = temp & 0x1F;
		sum = (sum << (32-n)) | (sum >> n);  // binary "rotate right"
		off += 8;
	}
	hash2 = sum;
}



void cBSA::getActiveBSAFiles(std::string &iniLoc, std::vector<std::string> &files){
	std::ifstream ini (iniLoc.c_str());
	if (!ini.is_open()){
		return;
	}

	files.push_back("Morrowind.bsa");

	std::string line;
	bool isReadingGameFiles = false;
	while (!ini.eof() ){
		getline (ini,line);
		if ( line.compare("[Archives]") == 0 ){
			isReadingGameFiles = true;
			continue;
		}
		if ( line.length() == 0 )
			continue;

		if ( line.find("[", 0) == 0 ){
			isReadingGameFiles = false;
			continue;
		}
		if ( isReadingGameFiles ){
			int firstIndex = line.find("=", 0);
			line = line.substr( firstIndex + 1, line.length());
			files.push_back(line);
		}
    }
    ini.close();
}


cBSAEntry::cBSAEntry(std::string entryName, int offset, int size){
	this->entryName = entryName;
	this->offset = offset;
	this->size = size;
}

cBSAEntry::cBSAEntry(std::string entryName, int offset, int size, unsigned hash1, unsigned hash2){
	this->entryName = entryName;
	this->offset = offset;
	this->size = size;
	this->hash1 = hash1;
	this->hash2 = hash2;
}


void cBSA::clear(){
	entries.clear();
	bsaName = "";
	numFiles = 0;
	hashPos = 0;
	isGood = false;
}

//Constructer for loading a bsa
cBSA::cBSA(std::string bsaName){
	this->loadBSA(bsaName);
}

cBSA::cBSA(){};

void cBSA::loadBSA(std::string bsaName){

	//clear the bsa of all the old data.
	this->clear();

	this->isGood = false;
	
	//store the bsa name
	this->bsaName = bsaName;

	//open the file
	std::ifstream bsaFile (bsaName.c_str(), std::ios::in | std::ios::binary);
	if ( !bsaFile.is_open() ){
#ifdef BSA_ERROR_DOTHROW
		throw "BSA file: " + bsaName + "couldn't be opened for reading";
#endif
		return;
	}



	//check the version
	int version;
	bsaFile.read ((char *)&version, sizeof (int));
	if ( version != 0x00000100 ){
#ifdef BSA_ERROR_DOTHROW
		throw "BSA version is not correct in " + bsaName;
#endif
		return;
	}

	//get the number of files and hash offset
	int hashoffset;
	int numfiles;
	bsaFile.read ((char *)&hashoffset, 4);
	bsaFile.read ((char *)&numfiles, 4);


	this->numFiles = numfiles;

	unsigned long hashSize = hashoffset - 12 * numfiles;


	//store the position of the hash table
	this->hashPos = hashoffset + 12;
	
	for ( int i = 0; i < numfiles; i++ ){
		bsaFile.seekg(12 + i * 8);

		//get file size
		int size;
		bsaFile.read ((char*)&size, sizeof(int));

		//get file offset
		int offset;
		bsaFile.read ((char*)&offset, sizeof(int));

		//read the location
		int nameLoc;
		bsaFile.seekg(12+numfiles*8+i*4);
		bsaFile.read ((char*)&nameLoc, sizeof(int));

		//gets the name
		bsaFile.seekg(nameLoc + 12 + numfiles *  12);
		std::string name = "";
		while(true){
			char b;
			bsaFile.read ((char *)&b, 1);
			if ( b == 0)
				break;
			name += (char) b;
		}

		unsigned tHash1;
		unsigned tHash2;

		bsaFile.seekg(hashPos + i* 8);
		bsaFile.read ((char*)&tHash1, 4);
		bsaFile.read ((char*)&tHash2, 4);

		//unsigned t2Hash1;
		//unsigned t2Hash2;
		//this->getHash(name, t2Hash1, t2Hash2);

		//if (( tHash1 != t2Hash1 ) || ( tHash2 != t2Hash2 ))
		//	std::cout "invlid hash\n";
			


		//add the entry to the data storage.
		//make sure it is in lower case;
		std::transform (name.begin(),name.end(), name.begin(), tolower); 
		cBSAEntry entry(name, offset, size, tHash1, tHash2);
		this->entries.push_back(entry);
	}
	bsaFile.close();

	this->isGood = true;
}



bool cBSA::doDoesExist(std::string file, int &index){
	index = -1; //default setting, not found.

	//everything lower case.
	std::transform (file.begin(),file.end(), file.begin(), tolower); 

	unsigned hash1;
	unsigned hash2;
	this->getHash(file, hash1, hash2);

	unsigned first = 0;
	unsigned last = this->entries.size();


#ifdef USE_BINARY_SEARCH
	while (first <= last) {
		int mid = (first + last) / 2; 
		if (hash1 > this->entries[mid].hash1) 
			first = mid + 1;
		else if (hash1 < this->entries[mid].hash1) 
			last = mid - 1;
		else if (hash2 > this->entries[mid].hash2) 
				first = mid + 1; 
		else if (hash2 < this->entries[mid].hash2) 
				last = mid - 1; 
		else{
           index = mid;  
		   break;
		}
	}
#else
	//loop through vector
	unsigned long size = this->entries.size();
	for ( unsigned i = 0; i < size; i++){
		std::string entry = this->entries[i].entryName;
		if ( file == entry){
			index = i;
			break;
		}
	}
#endif
	


	if ( index == -1 )
		return false;

	return true;
}

bool cBSA::doesExist(std::string file, int &index){
	bool val = this->doDoesExist(file, index);
	return val;
}
bool cBSA::doesExist(std::string file){
	int junk;
	bool val = this->doDoesExist(file, junk);
	return val;
}

bool cBSA::extractFile(std::string file, std::string extractTo){

	//this is the pos of the end of the hash table.
	//hash table size is number of files * ( 2 * 4 bytes)
	unsigned dataStart = this->hashPos + this->numFiles * 8;

	int index;
	if ( !this->doesExist(file, index) )
		return false;

	#ifdef cBSA_DO_CREATE_FOLDERS
		createDirs(extractTo);
	#endif	



	//read the file into memory
	char * texture;
	texture = new char[this->entries[index].size];

	
	std::ifstream bsaFile (this->bsaName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
		if ( !bsaFile.is_open() ){
			throw "Couldn't open BSA file: " + bsaName + " for file extraction";
			//return false;
		}
		bsaFile.seekg(this->entries[index].offset + dataStart);
		bsaFile.read((char *)texture, this->entries[index].size);
	bsaFile.close();

	//write the data to the file
	std::ofstream outFile (extractTo.c_str(), std::ios::out | std::ios::binary);
		if ( !outFile.is_open() ){
			throw "Cannot open " + extractTo + " for writing";
			//return false;
		}
		outFile.write (texture, this->entries[index].size);
	outFile.close();

	//dispose of used mem
	delete [] texture;

	return true;
}