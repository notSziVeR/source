SET FLAGS=-A1 -T -j -xM -H -xW -xV -p -O
SET DIR_server=serversrc
SET DIR_client=clientsrc

astyle.exe %FLAGS% CWebBrowser/*.c		CWebBrowser/*.h
astyle.exe %FLAGS% EffectLib/*.cpp		EffectLib/*.h
astyle.exe %FLAGS% EterBase/*.cpp		EterBase/*.h
astyle.exe %FLAGS% EterGrnLib/*.cpp		EterGrnLib/*.h
astyle.exe %FLAGS% EterImageLib/*.cpp	EterImageLib/*.h
astyle.exe %FLAGS% EterLib/*.cpp		EterLib/*.h
astyle.exe %FLAGS% EterLocale/*.cpp		EterLocale/*.h
astyle.exe %FLAGS% EterPack/*.cpp		EterPack/*.h
astyle.exe %FLAGS% EterPythonLib/*.cpp	EterPythonLib/*.h
astyle.exe %FLAGS% GameLib/*.cpp		GameLib/*.h
astyle.exe %FLAGS% MilesLib/*.cpp		MilesLib/*.h
astyle.exe %FLAGS% PRTerrainLib/*.cpp	PRTerrainLib/*.h
astyle.exe %FLAGS% ScriptLib/*.cpp		ScriptLib/*.h
astyle.exe %FLAGS% SpeedTreeLib/*.cpp	SpeedTreeLib/*.h
astyle.exe %FLAGS% SphereLib/*.cpp		SphereLib/*.h
astyle.exe %FLAGS% UserInterface/*.cpp	UserInterface/*.h