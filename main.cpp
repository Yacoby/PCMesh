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
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

//for the std::time_t
#include <ctime>

//for the pipe stuff
#include <windows.h>

#include "bsa.h"
#include "cESX.h"
#include "race.h"

#include "boost/filesystem.hpp" 
namespace fs = boost::filesystem;




template <typename T>
inline std::string toString ( T x ){
  std::ostringstream oss;
  oss << x;
  return oss.str();
}

//creates all the directorys in a path;
void create_dir_path(const std::string &path){
	std::string curPath = path;
	std::string oldPath = "";
	while ( true ){

		while (( curPath.find("\\") == 0 ) || ( curPath.find("/") == 0 ))
			curPath = curPath.substr(1, curPath.length());
		
		int index = curPath.find("/", 0);
		if ( index == -1 )
			index = curPath.find("\\", 0 );
		if ( index == -1 )
			break;

		std::string thisDir = curPath.substr(0, index);
		curPath = curPath.substr(index, curPath.length());

		std::string trueThisDir = thisDir;
		if ( oldPath != "" )
			trueThisDir = oldPath + "\\" + thisDir;
		if ( !fs::exists(trueThisDir) ){
			fs::create_directory(trueThisDir);
		}
		oldPath = trueThisDir;

	}
}

void setMeshesDone(std::string &pcName){
	std::string name = "Data Files\\MWSE\\" + pcName + "-Mesh.yac";
	create_dir_path(name);

	int doneVal = 1;

	std::fstream fs (name.c_str(), std::ios::out | std::ios::binary);
	fs.write(reinterpret_cast<char *>(&doneVal), sizeof(int));
	fs.close();
}

void copyFile(const std::string &from, const std::string &to){
	if ( !fs::exists(from) )
		return;
	if ( fs::exists ( to ) )
		fs::remove(to);

	create_dir_path(to);
	fs::copy_file(from, to);
}

bool copyModel(const std::string &from, const std::string &to, std::vector<cBSA> &bsaFiles){

	if ( fs::exists(to) ){
		fs::remove(to);
		//return false;
	}

	bool copied = false;

	create_dir_path(to);

	std::string meshPath = "Data Files\\Meshes\\" + from;
	std::string bsaPath = "Meshes\\" + from;

	if ( fs::exists( meshPath ) ){
		fs::copy_file(meshPath, to);
		copied = true;
	}

	if ( !copied ){
		for ( unsigned i = 0; i < bsaFiles.size(); i ++ ){
			
			if ( bsaFiles[i].doesExist(bsaPath) ){
				bsaFiles[i].extractFile(bsaPath, to);
				copied = true;
				break;
			}
		}
	}
	return copied;
}


//gets a list of all the files in the specifed directory
void getFileList(const std::string &dir, std::vector<std::string> &fileList){
	if ( fs::exists( dir ) ){
		fs::directory_iterator end_itr; // default construction yields past-the-end
		for ( fs::directory_iterator itr( dir );itr != end_itr;	++itr ) {
			if ( fs::is_directory(itr->status()) ){
				std::string newDir = dir + "\\" + itr->path().leaf();
				getFileList( newDir, fileList );
			}else{
				std::string file = dir + "\\" + itr->path().leaf();
				fileList.push_back(file);
			}
		}
	}
}

void getActiveMods(std::string &iniLoc, std::vector<std::string> &mods){
	std::ifstream ini (iniLoc.c_str());
	if (!ini.is_open()){
		return;
	}

	std::string line;
	bool isReadingGameFiles = false;
	while (!ini.eof() ){
		getline (ini,line);

		//line.erase(std::remove_if(line.begin(),line.end(),strip),line.end());
		if ( line == "" )
			continue;

		if ( line.compare("[Game Files]") == 0 ){
			isReadingGameFiles = true;
			continue;
		}
		if ( line.find("[", 0) == 0 ){
			isReadingGameFiles = false;
			continue;
		}
		if ( isReadingGameFiles ){
			int firstIndex = line.find("=", 0);
			if ( firstIndex == std::string::npos )
				continue;

			line = line.substr( firstIndex + 1, line.length());
			mods.push_back(line);
		}
    }
    ini.close();
}

struct fileData{
	std::string loc;
	std::time_t time;
	long size;
};

int main(){

	std::cout << "Don't start Morrowind yet!\n\n";

	std::string iniLoc = "Morrowind.ini";

	std::cout << "Loading BSA files...\n";

	//load every active BSA file...
	std::vector<std::string> sBSAFiles;
	cBSA::getActiveBSAFiles(iniLoc, sBSAFiles);
	std::vector<cBSA> bsaFiles;
	for ( unsigned i = 0; i < sBSAFiles.size(); i++ ){
		cBSA tBSA;
		tBSA.loadBSA("Data Files\\" + sBSAFiles[i]);
		bsaFiles.push_back(tBSA);
	}

	std::cout << "Sorting active mods...\n";

	//get the active mods.
	std::vector<std::string> actMods;
	getActiveMods(iniLoc, actMods);

	//get the data for all the files.
	std::vector<fileData> fd;
	for ( unsigned i = 0; i < actMods.size(); i++ ){
		fileData tFileData;
		std::time_t ft = fs::last_write_time( "Data Files\\" + actMods[i] );
		tFileData.time = ft;
		tFileData.loc = "Data Files\\" + actMods[i];
		fd.push_back(tFileData);

	}

	//we now need to sort everything into load order.
	bool flag = true;
	while ( flag ){
		flag = false;
		for ( unsigned i = 1; i < fd.size(); i++ ){
			if ( fd[i].time < fd[i - 1].time ){
				flag = true;
				fileData tFileData = fd[i];
				fd[i] = fd[i-1];
				fd[i - 1] = tFileData;
			}
		}
	}


	std::cout << "Loading esmp race data...\n";

	//pull all the body part data from the active esmp files.
	std::vector<std::string> loadTypes;
	loadTypes.push_back("BODY");

	Races races;

	for ( unsigned i = 0 ; i < fd.size(); i++ ){
		cESX esmp;
		esmp.setLoadTypes(loadTypes);
		esmp.loadFile(fd[i].loc);

		esmp.newItr("BODY");
		for ( unsigned b = 0; b < esmp.itrSize(); b++ ){
			cESXRecord rec = esmp.itrGetRecord(b);

			if ( rec.getSubRecordData("BYDT").length() < 4 )
				continue;


			std::string bydt = rec.getSubRecordData("BYDT");
			int p = bydt.at(0);
			int vmp = bydt.at(1);
			int flags = bydt.at(2);
			int pT = bydt.at(3);

			if ( pT != 0 )
				continue;

			bool fem = false;
			if (( flags == 1 ) || ( flags == 3 ))
				fem = true;
 
			races.addBodyPart(rec.getSubRecordData("FNAM"), rec.getSubRecordData("NAME"), p, pT, fem, rec.getSubRecordData("MODL"));
		}


	}
	
	std::cout << "Waiting for input... (start Morrowind now!)\n";


//setup the pipe.
#define BUFSIZE 4096

	HANDLE pipe;
	DWORD size, i;
	CHAR buffer[BUFSIZE+1]; 
	CHAR fileName[BUFSIZE+1]; 
 

	pipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\MWSEYAC_PCMESH"), PIPE_ACCESS_DUPLEX, 
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, BUFSIZE, BUFSIZE, 20000, NULL);
	while ( (pipe != INVALID_HANDLE_VALUE) && 
		( ConnectNamedPipe(pipe, NULL) || (GetLastError() == ERROR_PIPE_CONNECTED) ) )
	{


		while (true) 
		{ 
			if (!ReadFile( pipe, buffer, BUFSIZE, &size, NULL) || (size == 0) ) break;
			//buffer[size] = 0;		// ensure it's null terminated in case it's a string

			std::string reName = buffer;

			std::cout << "Resived name is: " << reName << "\n";


			//clean out the old meshes
			try{
				fs::remove_all ("Data Files\\Meshes\\PCMesh\\");
			}catch(std::exception e){
				std::cout << "Error\n" << e.what() << "\nWhen trying to delete old meshes\n";
				break;
			}


			std::cout << "Loading save data...\n";

			loadTypes.clear();
			loadTypes.push_back("NPC_");

			std::vector<std::string> essLst;
			getFileList("Saves", essLst);
			

			//for every save
			for ( unsigned i = 0; i < essLst.size(); i++){
				cESX ess;
				ess.setLoadTypes(loadTypes);
				ess.loadFile(essLst[i]);

				//for every npc in that save
				ess.newItr("NPC_");
				for ( unsigned n = 0; n < ess.itrSize(); n++ ){
					cESXRecord rec = ess.itrGetRecord(n);

					std::string id = rec.getSubRecordData("NAME");
					id = id.substr(0, id.length() - 1);

					std::string name = rec.getSubRecordData("FNAM");
					name = name.substr(0, name.length() - 1);

					if (( id.compare("player") == 0 ) && ( name.compare(reName) == 0)){


						//get race and race related body parts.
						std::string race = rec.getSubRecordData("RNAM");

						std::vector<BodyPart> parts = races.getBodyParts(race);

						if ( parts.size() < 1 ){
							std::cout << "Cannot find race...\n";
							continue;
						}

						std::string head = rec.getSubRecordData("BNAM");
						std::string hair = rec.getSubRecordData("KNAM");

						for ( unsigned p = 0; p < parts.size(); p++ ){			

							if ( parts[p].id.find(".1st") != std::string::npos ){
								std::cout << "Assuming " << parts[p].id << " is first person so unneeded\n";
								continue;
							}


							if (( parts[p].part == 0 ) && ( parts[p].id.compare(head) != 0 ))
								continue;

							if (( parts[p].part == 1 ) && ( parts[p].id.compare(hair) != 0 ))
								continue;

							std::string copyTo = "Data Files\\Meshes\\PCMesh\\";
							if ( parts[p].isFemale ){
								copyTo += "F_";
							}else{
								copyTo += "M_";
							}

							copyTo += toString(parts[p].part);
							copyTo += ".nif";
								

							//copy the file.
							try{
								std::string modl = parts[p].model.substr(0, parts[p].model.size() - 1);
								if ( !copyModel(modl, copyTo, bsaFiles) ){
									std::cout << "Couldn't copy model from:\n " << parts[p].model << "to:\n" << copyTo << "\n";
								}
							}catch( std::exception e){
								std::cout << e.what();
								std::cin.get();
							}

								
						}
						//copy default files.
						for ( unsigned d = 0; d < 15; d++ ){
							if ( !fs::exists ("Data Files\\Meshes\\PCMesh\\F_" + toString(d) + ".nif") ){
								copyFile("blankNif.dat", "Data Files\\Meshes\\PCMesh\\F_" + toString(d) + ".nif");
							}
							if ( !fs::exists ("Data Files\\Meshes\\PCMesh\\M_" + toString(d) + ".nif") ){
								copyFile("blankNif.dat", "Data Files\\Meshes\\PCMesh\\M_" + toString(d) + ".nif");
							}
						}
						
						//record the fact that we are all good.
						setMeshesDone(reName);
					
						//don't need to look through any more ess files.
						break;
					}
						
				}

			}
		}
			//if ( !WriteFile(pipe, buffer, size, &i, NULL) || (i != size) ) break; //write back name;
		
		FlushFileBuffers(pipe); 
		DisconnectNamedPipe(pipe); 
	}
	CloseHandle(pipe); 


	return 0;

}