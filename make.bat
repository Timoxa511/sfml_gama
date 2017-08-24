set path=C:\Program Files (x86)\CodeBlocks\MinGW\bin;%path%;

g++ test.cpp				   ^ 
 -I X:\sfml\SFML-2.4.2\include ^ 
 -L X:\sfml\SFML-2.4.2\lib     ^
 -l sfml-graphics      		   ^
 -l sfml-system        		   ^
 -l sfml-window         	   ^
 -l sfml-main       	       ^
 -l opengl32                   ^
 -l gdi32                      ^
 -l winmm                      
     