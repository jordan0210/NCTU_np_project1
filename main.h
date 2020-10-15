#include <iostream>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

using namespace std;

#define NO_PIPE 1
#define NORMAL_PIPE 2
#define ERROR_PIPE 3

void init_shell();

void parsePipe(string cmdLine, vector<string> &cmdBlocks);

void parseCmd(string cmdBlock, vector<string> &argv);

void exec_cmd(vector<string> &vec, int fd_in[2], int fd_out[2], int pipeType);

void vec2arr(vector<string> &vec, char *arr[], int index);

int findIndex(vector<string> &vec, string str);
