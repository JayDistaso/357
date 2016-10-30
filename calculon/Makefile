Cell: Cell.c
	gcc Cell.c -o Cell -Wall -Werror -O3
LinearSim : LinearSim.c
	gcc LinearSim.c -o LinearSim -Wall -Werror -O3
all : Cell LinearSim
force : Cell LinearSim.c echo
echo : 
	echo "Force recompile"
