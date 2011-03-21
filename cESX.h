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
#include <fstream>
#include <vector>
#include <iostream>

#define SUB_USE_STR

#ifdef SUB_USE_STR

//generic subrecord
class cESXSubRecord{
public:
	std::string subName;
	std::string subData;
public:
	void setName(const std::string& name){ subName = name; }
	void setData(const std::string& data){ subData = data; }
};
#else
#define _CRT_SECURE_NO_DEPRECATE 1

class cESXSubRecord{
public:
	//std::string subName;
	//std::string subData;
	char * subName;
	char * subData;
public:
	void setName(const std::string& name){
		subName = new char[name.length()];
		strcpy(subName, name.c_str());

	}
	void setData(const std::string& data){
		subData = new char[data.length()];
		strcpy(subData, data.c_str());
	}
};

#endif


//generic record
class cESXRecord{

protected:
	std::vector<cESXSubRecord> subRecords;

public:
	void addSubRecord(const cESXSubRecord& subRecord){ subRecords.push_back(subRecord); }

private:
	/*Size of the record not including the 16 bytes of header data.*/
	long size;

	/*Unknown value, usually 0 (deleted/ignored flag?)*/
	long hdr1;

	/*Record flags.
			 0x00002000 = Blocked
			 0x00000400 = Persistant*/
	long flags;

	


//the type of the data. e.g. MISC, GMST,
private:
	std::string type;
public:
	virtual std::string getType(){ return type; }
	virtual void setType(const std::string& type){ this->type = type; }

//the id of the data type. every object should have this.
private:
	std::string id;
public:
	virtual std::string getID(){ return id; }
	virtual void setID( const std::string& id){ this->id = id; }


	std::string getSubRecordData(const std::string& recordName){
		for ( unsigned i = 0; i < subRecords.size(); i ++ ){
#ifdef SUB_USE_STR
			if ( subRecords[i].subName.compare(recordName) == 0)
				return subRecords[i].subData;
#else
			if ( strcmp(subRecords[i].subName, recordName.c_str()) )
				return subRecords[i].subData;
#endif

		}
		return "";
	}


};


//top level class for loading and saving esp files.
class cESX{

private:
	std::vector<cESXRecord> records;

	std::vector<int> typeRecords;


	std::vector<std::string> loadTypes;
	bool doLoadAllTypes;


	bool doLoadType(const std::string& type){
		if ( doLoadAllTypes == true ){
			return true;
		}

		for ( unsigned i = 0; i < loadTypes.size(); i++ ){
			if ( loadTypes[i] == type )
				return true;
		}
		return false;
	}

public:

	cESX(){
		doLoadAllTypes = true;
	}

	void setLoadTypes(const std::vector<std::string>& inLoadTypes){ loadTypes = inLoadTypes; doLoadAllTypes = false; }
	void setDoLoadAll(bool b){ doLoadAllTypes = b; }


	void newItr(const std::string& type){
		typeRecords.clear();
		for ( unsigned i = 0; i < records.size(); i++ ){
			if ( records[i].getType().compare(type) == 0)
				typeRecords.push_back(i);
		}
	}

	unsigned itrSize(){ return typeRecords.size(); }

	cESXRecord itrGetRecord(const int i){
		return records[typeRecords[i]];
	}
	cESXRecord itrSetRecord(const int i, cESXRecord& rec){
		return records[typeRecords[i]] = rec;
	}

	void loadFile(const std::string& file);

	
};

void cESX::loadFile(const std::string& file){
	std::ifstream esp (file.c_str(), std::ios::in | std::ios::binary);

	if ( !esp.is_open() )
		return;

	unsigned long espSize;
	esp.seekg (0, std::ios::end);
	espSize = esp.tellg();
	esp.seekg (0, std::ios::beg);

	int sig;
	esp.read ((char *)&sig, 4);

	int hdrSize;
	esp.read ((char *)&hdrSize, sizeof(int));

	esp.seekg(hdrSize + 16);


	unsigned count = 0;
	
	while ( esp.tellg() <= espSize ){
		count++;

		cESXRecord record;



		//read record name
		char tVal[5];
		esp.get(tVal, 5);
		//std::string type = tVal;
		record.setType(tVal);

		//if ( doLoadType(record.getType()) == false)
		//	continue;


		if ( record.getType().length() == 0 )
			continue;


		//record block size block size
		long recordSize;
		esp.read ((char *)&recordSize, 4);

		//hdr1
		long hdr1;
		esp.read ((char *)&hdr1, 4);

		//flags
		long flags;
		esp.read ((char *)&flags, 4);

		unsigned long endPos = recordSize +  esp.tellg();


		if ( !doLoadAllTypes ){
			bool isGoodRec = false;
			for ( unsigned i = 0; i < loadTypes.size(); i++ ){

				if ( record.getType().compare(loadTypes[i]) == 0 ){
					isGoodRec = true;
					break;
				}
			}
			if ( !isGoodRec ){
				esp.seekg(endPos);
				continue;
			}
		}


		
		//std::cout << tVal << ":" << count << std::endl;
		

		while ( esp.tellg() < endPos ){
			//break;


			//get the details

			char tmpSubRecName[5];
			esp.get(tmpSubRecName, 5);
			std::string subRecName = tmpSubRecName;


			long subRecLength;
			esp.read ((char *)&subRecLength, 4);

			long subRecEnd = subRecLength + esp.tellg();



			//USE .READ???????????!??!??!

			//read the data
			/*
			char * tmpSubRecData;
			tmpSubRecData = new char[subRecLength];
			esp.read(tmpSubRecData, subRecLength);
			std::string subRecData = tmpSubRecData;
			delete [] tmpSubRecData;
			*/

			//possibly very slow. but it works. 
			std::string subRecData;
			for ( long i = 0; i < subRecLength; i++ ){
				subRecData += esp.get();
			}

			//add the data
			cESXSubRecord subRecord;
			subRecord.setName(subRecName);
			//subRecord.subName = subRecName;
			subRecord.setData(subRecData);
			//subRecord.subData = subRecData;
			record.addSubRecord(subRecord);

			//std::cout << "\t" <<  subRecName << std::endl;
			
			

		//to here

			esp.seekg(subRecEnd);
		}

		records.push_back(record);

		//saftey. Make sure the pointer is where it should be
		esp.seekg(endPos);

	}
	//std::cout << esp.tellg() << std::endl ;
	esp.close();

}