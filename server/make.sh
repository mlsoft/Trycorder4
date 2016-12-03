gcc -Wno-implicit-function-declaration -o tryclient tryclient.c
gcc -o tryserver tryserver.c
gcc -o tryservers -pthread tryservers.c

