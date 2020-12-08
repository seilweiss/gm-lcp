bison -o gmParser.cpp -d -l -p gm gmParser.y  
flex -ogmScanner.cpp -Pgm -Sflex.skl gmScanner.l

rem use following for verbose bison
rem bison -o gmParser.cpp -d -l -v -p gm gmParser.y  
