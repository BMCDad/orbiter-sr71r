========================================================================
    DYNAMIC LINK LIBRARY : SR71R Project Overview
========================================================================

Updates:
1/19/2019

License:
Copyright(C) 2019  Blake Christensen

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.


Building:
-	Clone local repository, for example: [c:\git\sr71r]
-	Unzip Orbiter 2016 into the 'Orbiter' folder in the repository. [c:\git\sr71r\Orbiter]
-	Run Orbiter so it can go through first type config.
-	In Visual Studio open the SR71R.sln [c:\git\sr71r\Orbiter\Orbitersdk\samples\SR71R\SR71R.sln]
-	Build the solution.
-	To debug you will need to set project properties 'Debugging\Command' to the full orbiter Orbiter.exe path.
	Visual Studio does not keep this in the project file and does not like a relative path, so for our example
	it would be [c:\git\sr71r\Orbiter\Orbiter.exe]
-	Also in project properties set 'Debugging\Working Directory' to $(OrbiterDir)
-	F5 should now run Orbiter in the debugger.  
	!! Make sure the Orbiter installation is functional outside of the debugger before you try and run it under the debugger !!

Blender:
-	The source .blend file it edited and built using the tools found here: https://www.orbiter-forum.com/showthread.php?t=36734

Gimp:
-	Texture base gimp files are in the Art folder.  You will need the DDS plugin to output DDS files.

