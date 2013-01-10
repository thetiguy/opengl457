#########################################################
#  CMPSC 457                                            #
#  Simple make file for PS 3                            #
#  March 2, 2010                                        #
#  Charlie Crandall                                     #
#  cdc5064@psu.edu                                      #
#                                                       #
#                                                       #
#  Your source codes should be named as                 #
#                                                       #
#               template.cxx                            #
#                                                       #
#                                                       #
#  To compile your program, type                        #
#                                                       #
#               make template                           #
#                                                       #
#  or, simply type                                      #
#                                                       #
#               make                                    #
#                                                       #
#  on your shell.                                       #
#                                                       #
#                                                       #
#  To run your program, type                            #
#                                                       #
#               make run                                #
#                                                       #
#                                                       #
#  To remove the executable 'template' and other        #
#  backup files (e.g., template.cxx~, etc), type        #
#                                                       #
#               make clean                              #
#                                                       #
#########################################################


# compiler to be used
CC = /usr/sfw/bin/g++

# flags to the compiler
CXX_FLAGS = -Wall -ansi

# path to directories containing header files
INC_DIR = -I. -I/opt/csw/include

# GL related libraries
GL_LIBS = -L/opt/csw/lib -lglut -lGLU -lGL

# X related libraries
X_LIBS = -lXmu -lXext -lXi -lm






#### TARGETS ####

template: template.cxx matrices.cxx
	$(CC) -o template template.cxx matrices.cxx $(CXX_FLAGS) $(INC_DIR) $(GL_LIBS) $(X_LIBS)

run: template
	./template
clean:
	rm -f template  *.o  *~
