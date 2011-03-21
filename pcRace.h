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

struct PCRaceData{
	
	std::string name;
	std::string race;
};

class PCRace{
	std::vector<PCRaceData> raceData;

public:
	void set(std::string name, std::string race){
		bool found = false;
		for ( unsigned i = 0; i < raceData.size() i++ ){
			if ( raceData[i].name == name ){
				found = true;
				raceData[i].race = race;
			}
		}
		if ( !found ){
			PCRaceData tData;
			tData.name = name;
			tData.race = race;
			raceData.push_back(tData);
		}
	}

	std::string getRace(std::string pcName){
		for ( unsigned i = 0; i < raceData.size() i++ ){
			if ( raceData[i].name == pcName ){
				return raceData[i].race;
			}
		}
		return "";
	}

};