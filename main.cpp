#include <iostream>
#include <string>
using namespace std;

void init_shell();

int main(int argc, char *argv[]){
	init_shell();
	return 0;
}

// Getting start for a shell
void init_shell(){
	system("clear");
	printf("***********\n");
	printf("**SUCCESS**\n");
	printf("***********\n");
	
	string commandLine;
	while (true) {
		cout << "%% ";
		cin >> commandLine;
		if (commandLine == "exit"){
			exit(0);
		} else {
			cout << commandLine << endl;
		}
	}
}
