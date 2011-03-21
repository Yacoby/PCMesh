====================================================================
PC Mesh Maker
====================================================================

Name:		PC Mesh Maker
Author(s):	Jacob Essex (aka Yacoby)
Version:	0.0.2
Last Updated:	28 April 2008

====================================================================
LICENSE AND COPYRIGHT (Legal Junk)
====================================================================

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


====================================================================
LICENSE AND COPYRIGHT (Basic)
====================================================================

The license basically means that you can do what you want, as long as
you keep the above notice with the program. In other words, feel
free to distribute the program, but keep this document with it.


====================================================================
INTRODUCTION 
====================================================================

This program is intended as a modders resource. When you pass the
players name to the program via MWSE, it will copy the meshes that
the player is using into a set position.
This alows you to have an NPC that is a clone of the player. (If you
copy the items with MWSE ;¬) )

====================================================================
KNOWN LIMITATIONS
====================================================================

The program works by reading saves, so for this to work, the player
must have saved and loaded the game with their current charachter 
at least once.

You can check if meshes have been created by reading an int (short?)
from a file with the name [playersName]-Mesh.yac (replace [playersName]
with the players name);
The bytes read should be equal to 1.

====================================================================
INSTALLATION INSTRUCTIONS
====================================================================

Place the exe and the .dat file in the Morrowind directory.

The program needs to be run along side Morrowind at least once to 
generate the meshes.
After the meshes have been generated, you don't need to run
it again untill you start using a diffrent charachter.

FYI the .dat file is the blank nif file, just with a diffrent file
extension.

====================================================================
USING THE SOFTWARE
====================================================================

You need to mention in your mods readme that this program should be run
along side Morrowind. You should not start Morrowind untill the program tells
you to. Else it won't work.

You would want to use a script something like this as a startscript:
--------------------------
begin SCRNam

long pcRef
long pcName

;get the player ref. it is slow, but as we only do it once per load, it
;should be fine.
setx pcRef to xGetRef, "player"

;don't run before the player has set up his/her stats
if ( CharGenState != -1 )
    return
endif

setx pcName to pcRef->xGetName

;write the players name to a pipe. If the program is running it will
;look through all the saves untill it finds a player with the same name as
;the one you sent down the pipe. It gets all the information from that save
;and copies the meshes.

xFileWriteText, "|YAC_PCMESH", "%s", pcName

stopscript, SCRNam

end
--------------------------


The program generates the meshes in the Data Files\Meshes\PCMesh\ Directory

The male meshes start with a M_
The female meshes start with a F_

Then a number for the bodypart

	0 = Head
	1 = Hair
	2 = Neck
	3 = Chest
	4 = Groin
	5 = Hand
	6 = Wrist
	7 = Forearm
	8 = Upperarm
	9 = Foot
	10 = Ankle
	11 = Knee
	12 = Upperleg
	13 = Clavicle
	14 = Tail

Whatever happens there will be a file for every number, even if the
race doesn't have that body part, a blank nif file is used.

If someone complains about the numbers, I will change it. I am just
lazy.


====================================================================
CONTRIBUTORS 
====================================================================  

Timeslip, for the C# BSA code I converted to C++ and used.
Fliggerty, for the request.
Mr Cellophane, an idea/prompt.

====================================================================
CONTACT INFORMATION
====================================================================
 
I welcome your feedback, suggestions and contributions. Contact me
via the Offical Elder Scrolls forum (username: Yacoby) if you have
questions, feedback, code submissions,and bug reports.


====================================================================
