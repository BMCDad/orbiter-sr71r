REM Deployment batch file.
REM Aug. 31, 2015
REM
REM Copies all files that will be part of the final deployment zip package
REM and places them into a 'Deploy' folder under the current project.  The contents
REM of the deploy folder can then be zipped up (not the Deploy folder itself) and
REM that becomes the release.


@echo off
set projLocation=%1
set orbiterLocation=%2

echo Deployment:
echo Project Location: %projLocation%
echo Orbiter Location: %orbiterLocation%

set targetDir=%projLocation%Deploy
set configDir=\Config\Vessels\
set modulesDir=\Modules\
set texturesDir=\Textures\
set meshDir=\Meshes\
set scenarioDir=\Scenarios\SR71r\
set doc=\Doc\SR71r\

echo Target Dir: %targetDir%

echo Removing target dir.
rmdir /s /q %targetDir%

if not exist %targetDir% (cmd /c mkdir %targetDir%)

echo Config
xcopy %orbiterLocation%%configDir%SR71r.cfg %targetDir%%configDir% /y

echo Module
xcopy %orbiterLocation%%modulesDir%SR71r.dll %targetDir%%modulesDir% /y


REM Hint:	Look at the bottom of the generated .msh file to see which Textures
REM			need to be included here.
echo Textures
xcopy %orbiterLocation%%texturesDir%SR71_VCCockPit.dds %targetDir%%texturesDir% /y
xcopy %orbiterLocation%%texturesDir%SR71rBody.dds %targetDir%%texturesDir% /y
xcopy %orbiterLocation%%texturesDir%SR71R_100_VC1.dds %targetDir%%texturesDir% /y
xcopy %orbiterLocation%%texturesDir%SR71R_100_VC2.dds %targetDir%%texturesDir% /y

echo Meshes
xcopy %orbiterLocation%%meshDir%SR71r.msh %targetDir%%meshDir% /y
xcopy %orbiterLocation%%meshDir%SR71rVC.msh %targetDir%%meshDir% /y

echo Scenarios
xcopy %orbiterLocation%%scenarioDir%*.* %targetDir%%scenarioDir% /y/s

echo Docs
xcopy %projLocation%Docs\*.pdf %targetDir%%doc% /y