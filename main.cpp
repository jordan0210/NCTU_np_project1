#include "main.h"

int main(){
	init_shell();

	string cmdLine;
	vector<string> cmdBlocks;
	vector<string> argv;
	bool is_first_cmd = true;
	int fd_pipe[100][2];
	int *fd_in, *fd_out;
	int *fd_numberPipe = NULL;
	int pipeType = 0;
	int pipeCount = 0;
	
	while (true) {
		cout << "% ";
		getline(cin, cmdLine);
		parsePipe(cmdLine, cmdBlocks);
		is_first_cmd = true;

		while (!cmdBlocks.empty()){
			parseCmd(cmdBlocks[0], argv);
			
			fd_in = NULL;
			fd_out = NULL;
			if (!is_first_cmd) {
				fd_in = fd_pipe[pipeCount-1];
			} else if (is_first_cmd && (fd_numberPipe != NULL)){
				fd_in = fd_numberPipe;
			}

			if (cmdBlocks.size() != 1){
				fd_out = fd_pipe[pipeCount++];
				pipe(fd_out);
				if (cmdBlocks[0][cmdBlocks[0].length()-1] == '|') pipeType = 1;
				else if (cmdBlocks[0][cmdBlocks[0].length()-1] == '!') pipeType = 2;
			} else{
		
			}

			/*if (fd_in == NULL) cout << "NULL, ";
			else cout << fd_in << ", ";
			if (fd_out == NULL) cout << "NULL" << endl;
			else cout << fd_out << endl;
			cout << is_first_cmd << ", " << (cmdBlocks.size() == 1) << ", " << pipeCount << ", " << pipeType << endl;*/
			exec_cmd(argv, fd_in, fd_out, pipeType);
			argv.clear();
			cmdBlocks.erase(cmdBlocks.begin());
			is_first_cmd = false;
			pipeType = 0;
		}
	}

	return 0;
}

// Getting start for a shell
void init_shell(){
	system("clear");
	setenv("PATH", "bin:.", 1);
	
	printf("***********\n");
	printf("**SUCCESS**\n");
	printf("***********\n");
}

//parse input commandLine into commandBlocks
void parsePipe(string cmdLine, vector<string> &cmdBlocks){
	int front = 0;
	int end;
	string temp_str;
	cmdLine += "| ";
	
	while ((end = cmdLine.find(' ', cmdLine.find_first_of("|!", front))) != -1){
		temp_str = cmdLine.substr(front, end-front);
		if (temp_str[0] == ' ') temp_str = temp_str.substr(1);
		if (end == cmdLine.length()-1) temp_str = temp_str.substr(0, temp_str.length()-1);
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

void exec_cmd(vector<string> &vec, int fd_in[2], int fd_out[2], int pipeType){
	char *argv[1000];
	string cmd = vec[0];
	int child_pid;

	if (fd_out != NULL){
		vec.erase(vec.end());
	}
	
	if (cmd == "printenv"){
		if (getenv(argv[1]) != NULL)
			cout << getenv(argv[1]) << endl;
	} else if (cmd == "setenv"){
		setenv(argv[1], argv[2], 1);
	} else if (cmd == "exit" || cmd == "EOF") {
		exit(0);
	} else {
		switch (child_pid = fork()){
			int status;
			case -1 :
				perror("fork()");
				exit(-1);
			case 0 :
				if (fd_in != NULL){
					dup2(fd_in[0], STDIN_FILENO);
					close(fd_in[0]);
					close(fd_in[1]);
				}
				if (fd_out != NULL){
					if (pipeType == 1){
						dup2(fd_out[1], STDOUT_FILENO);
					} else if (pipeType == 2){
						dup2(fd_out[1], STDOUT_FILENO);
						dup2(fd_out[1], STDERR_FILENO);
					}
					close(fd_out[0]);
					close(fd_out[1]);
				}
				
				int fd_redirection, index;
				if ((index = findIndex(vec, ">")) == -1){
					vec2arr(vec, argv, vec.size());
				} else {
					fd_redirection = open(vec.back().data(), O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0600);
					dup2(fd_redirection, STDOUT_FILENO);
					dup2(fd_redirection, STDERR_FILENO);
					close(fd_redirection);
					vec2arr(vec, argv, index);
				}
				if (execv(("./bin/"+cmd).data(), argv) == -1)
					cerr << "Unknow cmd: [" << cmd << "]" << endl;
				exit(0);
			default :
				if (fd_in != NULL){
					close(fd_in[0]);
					close(fd_in[1]);
				}
				waitpid(child_pid, &status, 0);
		}
	}
}

void vec2arr(vector<string> &vec, char *arr[], int index){
	for (int i=0; i<index; i++){
		arr[i] = (char*)vec[i].data();	
	}
	arr[vec.size()] = NULL;
}

int findIndex(vector<string> &vec, string str){
	int index;
	for (index=0; index<vec.size(); index++){
		if (vec[index] == str){
			break;
		}
	}
	if (index == vec.size()){
		index = -1;
	}
	return index;
}
