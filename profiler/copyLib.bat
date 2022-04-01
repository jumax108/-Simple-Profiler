<<<<<<< HEAD

set name="profiler"
=======
set name=profiler
>>>>>>> 1155486bec6e0e93eaf49bbbfc14d184b50ab0c1

mkdir %name%

robocopy headers %name%/headers
robocopy release %name% *.pdb
robocopy release %name% *.lib

<<<<<<< HEAD
erase release /S /Q
=======
del release /Q /S

>>>>>>> 1155486bec6e0e93eaf49bbbfc14d184b50ab0c1
rd release /S /Q

pause