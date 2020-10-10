#include <iostream>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void init_shell();

void parsePipe();

void parseCmd(string cmdLine, vector<string> &argv);

void exec_cmd(vector<string> &vec);

void vec2arr(vector<string> &vec, char *arr[]);

int main(){
	init_shell();
	return 0;
}

// Getting start for a shell
void init_shell(){
	system("clear");
	printf("***********\n");
	printf("**SUCCESS**\n");
	printf("***********\n");
	
	string cmdLine;
	vector<string> argv;
	while (true) {
		cout << "% ";
		getline(cin, cmdLine);
		parseCmd(cmdLine, argv);
		
		exec_cmd(argv);
		argv.clear();
	}
}

//parse input commandLine into commandBlocks
void parsePipe(){

}

//parse inpur commandBlock into command and arguments 
void parseCmd(string cmdBlock, vector<string> &argv){
	int front = 0;
	int end;
	int counter = 0;
	cmdBlock += " ";

	//read arguments
	while((end = cmdBlock.find(" ", front)) != -1){
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
	
	} else if (cmd == "noop"){
		
	} else if (cmd == "number"){
	
	} else if (cmd == "removetag"){
	
	} else if (cmd == "removetag0"){
	
	} else if (cmd == "printenv"){
		switch(fork()){
			int *status;
			case -1 :
				perror("fork()");
				exit(-1);
			case 0 :
				cout << getenv("PATH") << endl;
				exit(0);
			default :
				wait(&status);
		}
	} else if (cmd == "setenv"){
	
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
