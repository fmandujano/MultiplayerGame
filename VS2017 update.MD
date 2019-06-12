# Problemas encontrados al actualizar a VS 2017 (v141)

* Los siguientes archivos no se agregaron por lo que causaron 44 errores LNK2019
 * ofxInputField.h/.cpp
 * ofxColorPicker.h/.cpp
 * ofxNetworkUtils.cpp

* No copia las DLLs necesarias por lo que se agregó el comando como custom post build event:
`for /d %%f in ($(OF_ROOT)\libs\*) do (if exist %%f\lib\vs\$(Platform_Actual)\*.dll ( robocopy "%%f\lib\vs\$(Platform_Actual)" "$(ProjectDir)bin" "*.dll" /njs /njh /np /fp /bytes ))
if errorlevel 1 exit 0 else exit %errorlevel% `


