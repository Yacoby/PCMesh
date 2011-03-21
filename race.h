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

struct BodyPart{

	std::string id;

	int part;

	/*
	0 = Skin
	1 = Clothing
	2 = Armor
	*/
	int partType;


	bool isFemale;


	std::string model;

};


class Race{
	std::string raceName;
	//bool isFemale;

	std::vector<BodyPart> bodyParts;
public:

	//void setIsFemale(bool f){ isFemale = f; }
	//bool getIsFemale(){ return isFemale; }

	void setRaceName(const std::string &n){ raceName = n; }
	std::string getRaceName(){ return raceName; }

	std::vector<BodyPart> getBodyParts(){ return bodyParts;	}

	void addBodyPart(const std::string &id, int p, int pT, bool isFem, const std::string &mod){
		bool found = false;
		for ( unsigned i = 0; i < bodyParts.size(); i++ ){
			//if (( bodyParts[i].part == p ) && ( bodyParts[i].partType == pT ) && ( bodyParts[i].isFemale == isFem)){
			if ( bodyParts[i].id == id ){
				bodyParts[i].id = id;
				bodyParts[i].part = p;
				bodyParts[i].partType = pT;
				bodyParts[i].isFemale = isFem;
				bodyParts[i].model = mod;

				found = true;
				break;
			}
		}
		if ( !found ){
			BodyPart bodyPart;
			bodyPart.id = id;
			bodyPart.part = p;
			bodyPart.partType = pT;
			bodyPart.isFemale = isFem;
			bodyPart.model = mod;
			bodyParts.push_back(bodyPart);
		}

	}



};

class Races{
	std::vector<Race> races;
public:
	void addBodyPart(const std::string &raceName,
		const std::string &id, int p, int pT, bool isFem, const std::string &model){

				bool found = false;

				for ( unsigned i = 0; i < races.size(); i ++ ){
					if ( races[i].getRaceName().compare(raceName) == 0 ){
						races[i].addBodyPart(id, p, pT, isFem, model);
						found = true;
						break;
					}
				}
				if ( !found ){
					Race race;
					race.setRaceName(raceName);
					//race.setIsFemale(isFem);
					race.addBodyPart(id, p, pT, isFem, model);
					races.push_back(race);
				}
	}
	std::vector<BodyPart> getBodyParts(const std::string &race){
		for ( unsigned i = 0; i < races.size(); i ++ ){
			if ( races[i].getRaceName().compare(race) == 0 ){
				return races[i].getBodyParts();
			}
		}
		std::vector<BodyPart> b;
		return b;
	}

};