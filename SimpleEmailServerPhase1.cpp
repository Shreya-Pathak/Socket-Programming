#include<iostream>
#include<fstream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<regex>
#include<unistd.h>
using namespace std;

int backlog = 5;
const int chunk_size = 1024;

int send_message(int sockfd, string message){
//Prints error message if not sent and returns -10
//Else returns number of bytes sent
	//Convert to char array
	char mess[message.length()+1];
	for (int i=0; i< message.length();i++){
		mess[i] = message[i];
	}
	mess[message.length()] = '\0';
	//Send char array
	int x = send(sockfd,&mess,strlen(mess),0);
	if(x<0){
		cerr<<"Message not sent: "<<message<<endl;
		return -10;
	}
	return x;
}

string rec_message(int sockfd){
//received upto chunk_size bytes and outputs as string.
//Prints appropriate errors
	char buff[chunk_size] = {0};
	int read_bytes = recv(sockfd,(void*)&buff, chunk_size, 0);
	if(read_bytes == 0){
		cerr<<"Connection closed before reading\n";
		exit(0);
	}
	else if(read_bytes == -1){
		cerr<<"Error on receive\n";
		exit(0);
	}
	string buf(buff);
	return buf;
}

int parse_message(string buf,string username, string password){
	regex quit("quit");
	regex message("(User:\\s)(.*)( Pass:\\s)(.*)");
	regex words("[^:\\s]");
//Quit case
	if(regex_match(buf, quit)){
		cout<<"Bye "<<username<<endl;
		return -1;
	}
//Extracting Username and Password
	else if(regex_match(buf, message)){
		int f1 = buf.find(" ");
		//cout<<"f1 is "<<f1<<endl;
		int f2 = buf.find("Pass:",f1+1);
		//cout<<"f2 is "<<f2<<endl;
		string message_username = buf.substr(f1+1,f2-7);
		f1 = buf.find(": ",f2+1);
		string message_password = buf.substr(f1+2);
		if(username!=message_username){
			cout<<"Invalid User\n";
			//cout<<"Extracted username is "<<message_username<<"A"<<endl;
			//cout<<"Extracted password is "<<message_password<<"A"<<endl;
			//cout<<"Username is "<<username<<"A"<<endl;
			//cout<<"Password is "<<password<<"A"<<endl;
			exit(0);
		}
		else if(password!=message_password){
			cout<<"Wrong Passwd\n";
			exit(0);
		}
		else{
			return 0;
		}
	}

	//Invalid message
	else{
		cout<<"Unknown Command\n";
		return -1;
	}
}
int main(int argc, char*argv[]){
//Validates Arguments
	if (argc != 3){
		cerr<<"Invalid number of arguments\nUsage: SimpleEmailServerPhase1 <PortNum> <PasswordFile>\n";
		exit(1);
	}
	char **x;
	int port = strtol(argv[1],x,10);
	if(port == 0){
		cout<<"Invalid port\n";
		exit(2);
	}
	//cout<<"port is"<<" "<<port<<endl;
	char *filename = argv[2];
	//cout<<"filename is"<<" "<<filename<<endl;

// Password file
	ifstream myfile;
	myfile.open(filename);
  	if(!myfile.good()){
  		cerr<<"Unable to open file\n";
  		exit(3);
  	}
  	string username,password;
  	myfile>>username>>password;
  	// cout<<"Username is "<<username<<endl;
  	// cout<<"Password is "<<password<<endl;
  	myfile.close();

//Initialize sockaddr
	sockaddr_in myaddress;
	myaddress.sin_family = AF_INET;
	myaddress.sin_addr.s_addr = INADDR_ANY;//inet_aton("127.0.0.1")
	myaddress.sin_port = htons(port);
    memset(&(myaddress.sin_zero), '\0', 8); // zero the rest of the struct
	int size_addr = sizeof(struct sockaddr);

//Set up socket
	int opt = 1;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));  
	if(sockfd == 0){
		cerr<<"Couldn't form socket\n";
		exit(2);
	}
	else if(bind(sockfd,(sockaddr *) &myaddress, size_addr) < 0
		|| listen(sockfd,backlog) < 0){
		cerr<<"Bind failed\n";
		exit(2);
	}
	else{
		cout<<"BindDone: "<<port<<"\n";
		cout<<"ListenDone: "<<port<<"\n";
	}

//Accept connections
	sockaddr_in client_address;
	int in_socket = accept(sockfd,(struct sockaddr *)&client_address, (socklen_t*)&size_addr);
	if(in_socket < 0){
		cerr<<"Couldn't accept connection\n";
		exit(2);
	}
	else{
		cout<<"Client: "<<inet_ntoa(client_address.sin_addr)<<":"<<ntohs(client_address.sin_port)<<endl;
	}

//Receive message
	string buf = rec_message(in_socket);
	if(parse_message(buf,username,password)<0){
		close(in_socket);
	}
	else{
		string welcome_message = "Welcome "+username+"\n";
		cout<<welcome_message;
		if(send_message(in_socket,welcome_message) < 0){
			cout<<"Message not sent"<<endl;
		}
	}

//Second Message
	buf = rec_message(in_socket);
	if(parse_message(buf,username,password)<0){
		close(in_socket);
	}
}