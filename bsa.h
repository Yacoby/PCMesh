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
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

//holds bsa entries;
class cBSAEntry{
public:
	std::string entryName;
    int offset;
    int size;

	//maitin compatibility;
	cBSAEntry(std::string entryName, int offset, int size);

	cBSAEntry(std::string entryName, int offset, int size, unsigned hash1, unsigned hash2);

	//Path to the entry to add.
	std::string addEntryPath;

	unsigned hash1; // first hash value
	unsigned hash2; // second hash value
};

class cBSA{
private:
	std::vector<cBSAEntry> entries;

	//path to the bsa.
	std::string bsaName;

	unsigned numFiles;
	unsigned hashPos; //position of the start of the file hash section.

	bool doDoesExist(std::string file, int &index);

public:

	void getHash(const std::string& sName, unsigned& hash1, unsigned& hash2);

	//Specifes if the file has been read correctly
	bool isGood;

	//Extracts the specifed file from the loaded BSA
	bool extractFile(std::string file, std::string extractTo);

	//Loads a BSA
	cBSA(std::string bsaName);

	cBSA();

	//Checks if a file exists.
	bool doesExist(std::string file);

	//Checks if a file exists and returns its position in the vector.
	bool doesExist(std::string file, int &index);

	//clears all data from the bsa.
	void clear();

	//Clears all data from a bsa and loads a new bsa.
	void loadBSA(std::string bsaName);

	static void getActiveBSAFiles(std::string &iniLoc, std::vector<std::string> &files);

	//below are not implemented.
	bool addEntry(); //adds an entry to the bsa
	bool saveBSA(); //saves the bsa



};