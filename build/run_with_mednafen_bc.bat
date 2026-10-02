@ECHO Off
SET EMULATOR_DIR=d:\joengine\Emulators
SET MEDNAFEN_EXECUTABLE_PATH=%EMULATOR_DIR%\mednafen\mednafen.exe

if not exist %MEDNAFEN_EXECUTABLE_PATH% (
	echo ---
	echo Please install Mednafen here %EMULATOR_DIR%
	echo ---
	pause
	exit
)

if exist "jo engine.cue" (
"%MEDNAFEN_EXECUTABLE_PATH%" "%cd%\jo engine.cue" -sound.volume "150"
) else (
echo Please compile first !
)
