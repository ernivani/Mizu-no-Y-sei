@echo off
(
echo Affichage du contenu des fichiers .cpp dans le répertoire src :
for %%f in (.\src\*.cpp) do (
    echo Contenu de %%f :
    type %%f
    echo -----------------------------------
)
echo Affichage du contenu des fichiers .h dans le répertoire lib :
for %%f in (.\lib\*.h) do (
    echo Contenu de %%f :
    type %%f
    echo -----------------------------------
)
) > output.txt
