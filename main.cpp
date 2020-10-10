#include <iostream>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void init_shell();

void parsePipe(string cmdLine, vector<string> &cmdBlocks);

void parseCmd(string cmdBlock, vector<string> &argv);

void exec_cmd(vector<string> &vec);

void vec2arr(vector<string> &vec, char *arr[]);

int main(){
	init_shell();
	return 0;
}

// Getting start for a shell
void init_shell(){
	system("clear");
	setenv("PATH", "bin:.", 1);
	
	printf("***********\n");
	printf("**SUCCESS**\n");
	printf("***********\n");
	
	string cmdLine;
	vector<string> cmdBlocks;
	vector<string> argv;
	while (true) {
		cout << "% ";
		getline(cin, cmdLine);
		parsePipe(cmdLine, cmdBlocks);
		
		while (!cmdBlocks.empty()){
			parseCmd(cmdBlocks[0], argv);
			exec_cmd(argv);
			argv.clear();
			cmdBlocks.erase(cmdBlocks.begin());
		}
	}
}

//parse input commandLine into commandBlocks
void parsePipe(string cmdLine, vector<string> &cmdBlocks){
	int front = 0;
	int end;
	string temp_str;
	cmdLine += "|";

	while ((end = cmdLine.find_first_of("|!", front)) != -1){
		temp_str = cmdLine.substr(front, end-front);
		if (temp_str[0] == ' ') temp_str = temp_str.substr(1);
		if (temp_str[temp_str.length()-1] == ' ') temp_str = temp_str.substr(0, temp_str.length()-1);
		cmdBlocks.push_back(temp_str);
		front = end + 1;
	}
}

//parse inpur commandBlock into command and arguments 
void parseCmd(string cmdBlock, vector<string> &argv){
	int front = 0;
	int end;
	cmdBlock += " ";

	//read arguments
	while ((end = cmdBlock.find(" ", front)) != -1){
		argv.push_back(cmdBlock.substr(front, end-front));
		front = end + 1;
	}
}


void exec_cmd(vector<string> &vec){
	char *argv[1000];
	string cmd = vec[0];

	vec2arr(vec, argv);
	if (cmd == "ls"){
		switch (fork()){
			int *status;
			case -1 :
				perror("fork()");
				exit(-1);
			case 0 :
				execv("./bin/ls", argv);
				exit(0);
			default :
				wait(&status);
		}
	} else if (cmd == "cat"){
		switch (fork()){
			int *status;
			case -1:
				perror("fork()");
				exit(-1);
			case 0:
				execv("./bin/cat", argv);
				exit(0);
			default:
				wait(&status);
		}
	} else if (cmd == "noop"){
		switch(fork()){
			int *status;
			case -1:
				perror("fork()");
				exit(-1);
			case 0:
				execv("./bin/noop", argv);
				exit(0);
			default:
				wait(&status);
		}
	} else if (cmd == "number"){
		switch(fork()){
			int *status;
			case -1:
				perror("fork()");
				exit(-1);
			case 0:
				execv("./bin/number", argv);
				exit(0);
			default:
				wait(&status);
		}
	} else if (cmd == "removetag"){
		switch(fork()){
			int *status;
			case -1:
				perror("fork()");
				exit(-1);
			case 0:
				execv("./bin/removetag", argv);
				exit(0);
			default:
				wait(&status);
		}
	} else if (cmd == "removetag0"){
		switch(fork()){
			int *status;
			case -1:
				perror("fork()");
				exit(-1);
			case 0:
				execv("./bin/removetag0", argv);
				exit(0);
			default:
				wait(&status);
		}
	} else if (cmd == "printenv"){
		if (getenv(argv[1]) != NULL)
			cout << getenv(argv[1]) << endl;
	} else if (cmd == "setenv"){
		setenv(argv[1], argv[2], 1);
	} else if (cmd == "exit" || cmd == "EOF") {
		exit(0);
	} else {
		cout << "Unknow cmd: [" << cmd << "]" << endl;
	}
}

void vec2arr(vector<string> &vec, char *arr[]){
	for (int i=0; i<vec.size(); i++){
		arr[i] = (char*)vec[i].data();	
	}
	arr[vec.size()] = NULL;
}
