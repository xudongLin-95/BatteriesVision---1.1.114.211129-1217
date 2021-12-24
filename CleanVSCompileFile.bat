del /f /q /s *.ncb
del /f /q /s *.sdf
del /f /q /s *.ipch
del /f /q /s *.tlog
del /f /q /s *.idb
del /f /q /s *.obj
del /f /q /s *.lastbuildstate
del /f /q /s *.manifest
del /f /q /s *manifest.rc
del /f /q /s *.embed.*
del /f /q /s *.log
del /f /q /s *.ilk
del /f /q /s vc100.pdb
del /f /q /s vc90.pdb
del /f /q /s *.exe
del /f /q /s *.pdb
del /f /q /s *.pch
del /f /q /s *.res
del /f /q /s *.aps
del /f /q /s *build
del /f /q /s *.exp
del /f /q /s *.bsc
del /f /q /s *.vc.db

for /f "tokens=* delims=" %%i in ('dir /ad /b /s ^| sort /r') do rd "%%i" 
