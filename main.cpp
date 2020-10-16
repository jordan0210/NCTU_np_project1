#include "main.h"

static regex reg("[|!][0-9]+");
vector<Pipe> pipes;
int main(){
	init_shell();

	string cmdLine;
	vector<cmdBlock> cmdBlocks;
	//vector<Pipe> pipes;
	bool is_first_cmd = true;
	int fd_pipe[10000][2];
	int *fd_in, *fd_out;
	//vector<numberPipe> numberPipes;
	
	while (true) {
		cout << "% ";
		getline(cin, cmdLine);
		parsePipe(cmdLine, cmdBlocks);
		is_first_cmd = true;
		
		while (!cmdBlocks.empty()){
			parseCmd(cmdBlocks[0]);
			
			checkPipeType(cmdBlocks[0]);//, pipes);
			
			if (!is_first_cmd){
				cmdBlocks[0].fd_in = pipes[pipes.size()-1].fd[0];
				cmdBlocks[0].has_fd_in = true;
			} else {
				for (int i=0; i<pipes.size(); i++){
					if (pipes[i].count == 0){
						cmdBlocks[0].fd_in = pipes[i].fd[0];
						cmdBlocks[0].has_fd_in = true;
						break;
					}
				}
			}
			
			exec_cmd(cmdBlocks[0]);
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			cout << cmdBlocks[0].cmd << " : " << endl;
			cout << "    [pipeType] = " << cmdBlocks[0].pipeType << endl;
			if (cmdBlocks[0].has_fd_in) cout << "    [fd_in] = " << cmdBlocks[0].fd_in << ";    ";
			else cout << "    [fd_in] = NULL;    ";
			if (cmdBlocks[0].has_fd_out) cout << "[fd_out] = " << cmdBlocks[0].fd_out << ";\n" << endl;
			else cout << "[fd_out] = NULL;\n" << endl;

			for (int i=0;i<pipes.size();i++){
				cout << i << ": [count] = " << pipes[i].count << "; [fd] = " << pipes[i].fd[0] << ", " << pipes[i].fd[1] << endl;
			}
			cout << "------------------------" << endl;
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			cmdBlocks.erase(cmdBlocks.begin());
			is_first_cmd = false;
		}
		
		for (int i=0; i<pipes.size(); i++){
			pipes[i].count--;
			//if (pipes[i].count < 0)
			//	pipes.erase(pipes.begin()+(i--));
		}
	}

	return 0;
}

// Getting start for a shell
void init_shell(){
	system("clear");
	setenv("PATH", "bin:.", 1);
}

//parse input commandLine into commandBlocks
void parsePipe(string cmdLine, vector<cmdBlock> &cmdBlocks){
	int front = 0;
	int end;
	cmdBlock cmdBlock;
	cmdLine += "| ";
	
	while ((end = cmdLine.find(' ', cmdLine.find_first_of("|!", front))) != -1){
		//initial cmdBlock
		cmdBlock.has_fd_in = false;
		cmdBlock.has_fd_out = false;
		cmdBlock.fd_in = 0;
		cmdBlock.fd_out = 0;
		cmdBlock.pipeType = 0;

		//parse cmdLine
		cmdBlock.cmd = cmdLine.substr(front, end-front);
		if (cmdBlock.cmd[0] == ' ') cmdBlock.cmd = (cmdBlock.cmd).substr(1);
		if (end == cmdLine.length()-1) cmdBlock.cmd = (cmdBlock.cmd).substr(0, (cmdBlock.cmd).length()-1);
		if ((cmdBlock.cmd)[(cmdBlock.cmd).length()-1] == ' ') cmdBlock.cmd = (cmdBlock.cmd).substr(0, (cmdBlock.cmd).length()-1);
		cmdBlocks.push_back(cmdBlock);
		
		front = end + 1;
	}
}

//parse inpur commandBlock into command and arguments 
void parseCmd(cmdBlock &cmdBlock){
	int front = 0;
	int end;
	cmdBlock.cmd += " ";
	
	//read arguments
	while ((end = (cmdBlock.cmd).find(" ", front)) != -1){
		(cmdBlock.argv).push_back((cmdBlock.cmd).substr(front, end-front));
		front = end + 1;
	}
}

//check the pipeType of the cmdBlock, and generate a new pipe. !! The fd_in in cmdBlock is not recorded.
void checkPipeType(cmdBlock &cmdBlock){//, vector<Pipe> &pipes){
	string last_argv = cmdBlock.argv.back();
	Pipe newPipe;
	newPipe.count = 0;
	if (regex_match(last_argv, reg)){    //number pipe case
		if (last_argv[0] == '|'){
			cmdBlock.pipeType = 1;
		} else if (last_argv[0] == '!'){
			cmdBlock.pipeType = 2;
		}
		newPipe.count = stoi(last_argv.substr(1));
		cout << newPipe.count << endl;
		pipe(newPipe.fd);
		cmdBlock.fd_out = newPipe.fd[1];
		pipes.push_back(newPipe);
	} else {
		if (last_argv == "|"){		     //normal pipe case
			cmdBlock.pipeType = 1;
		} else if (last_argv == "!"){    //error pipe case
			cmdBlock.pipeType = 2;
		} else {						 //no pipe case
			cmdBlock.pipeType = 0;
		}
		if (cmdBlock.pipeType != 0){
			newPipe.count = -1;
			pipe(newPipe.fd);
			cmdBlock.fd_out = newPipe.fd[1];
			pipes.push_back(newPipe);
		}
	}
	if (cmdBlock.pipeType != 0){
		cmdBlock.has_fd_out = true;
	}
}

void exec_cmd(cmdBlock &cmdBlock){
	char *argv[1000];
	string cmd = cmdBlock.argv[0];
	int child_pid;
	
	if (cmd == "printenv"){
		if (getenv(cmdBlock.argv[1].data()) != NULL)
			cout << getenv(cmdBlock.argv[1].data()) << endl;
	} else if (cmd == "setenv"){
		setenv(cmdBlock.argv[1].data(), cmdBlock.argv[2].data(), 1);
	} else if (cmd == "exit" || cmd == "EOF") {
		exit(0);
	} else {
		switch (child_pid = fork()){
			int status;
			case -1 :
				perror("fork()");
				exit(-1);
			case 0 :
				if (cmdBlock.has_fd_in){
					dup2(cmdBlock.fd_in, STDIN_FILENO);
				}
				if (cmdBlock.has_fd_out){
					if (cmdBlock.pipeType == 1){
						dup2(cmdBlock.fd_out, STDOUT_FILENO);
					} else if (cmdBlock.pipeType == 2){
						dup2(cmdBlock.fd_out, STDOUT_FILENO);
						dup2(cmdBlock.fd_out, STDERR_FILENO);
					}
				}
				//child closes all pipe fds
				for (int i=0; i<pipes.size(); i++){
					close(pipes[i].fd[0]);
					close(pipes[i].fd[1]);
				}

				// file redirection
				int fd_redirection, index;
				if ((index = findIndex(cmdBlock.argv, ">")) == -1){
					if (cmdBlock.pipeType == 0)
						vec2arr(cmdBlock.argv, argv, (cmdBlock.argv).size());
					else
						vec2arr(cmdBlock.argv, argv, (cmdBlock.argv).size()-1);
				} else {
					fd_redirection = open((cmdBlock.argv).back().data(), O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0600);
					dup2(fd_redirection, STDOUT_FILENO);
					dup2(fd_redirection, STDERR_FILENO);
					close(fd_redirection);
					vec2arr(cmdBlock.argv, argv, index);
				}
				if (execv(("./bin/"+cmd).data(), argv) == -1)
					cerr << "Unknow cmd: [" << cmd << "]" << endl;
				exit(0);
			default :
				//parent close useless pipe fd
				if (cmdBlock.has_fd_in){
					for (int i=0; i<pipes.size(); i++){
						if (pipes[i].fd[0] == cmdBlock.fd_in){
							close(pipes[i].fd[0]);
							close(pipes[i].fd[1]);
							pipes.erase(pipes.begin()+i);
							break;
						}
					}
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
