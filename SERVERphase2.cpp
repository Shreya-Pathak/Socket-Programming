#include<iostream>
#include<fstream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<regex>
#include<unistd.h>
#include<sys/types.h>
#include<dirent.h>
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
	//cout<<"a"<<endl;
	return buf;
}

bool match_user(const char* filename, string buf, string& user)
{
    ifstream file;
	file.open(filename);
  	if(!file.good()){
  		cerr<<"Unable to open file\n";
  		exit(3);
  	}

	regex message("(User:\\s)(.*)( Pass:\\s)(.*)");
    string username;
    string password;
	if(regex_match(buf, message)){

		int f1 = buf.find(" ");
		//cout<<"f1 is "<<f1<<endl;
		int f2 = buf.find("Pass:",f1+1);
//		//cout<<"f2 is "<<f2<<endl;
		username = buf.substr(f1+1,f2-7);
		f1 = buf.find(": ",f2+1);
		password = buf.substr(f1+2);
		//bool an = match_user(passfile, message_username, message_password);
		/*if(!an)
		{
            //cout<<"User not valid";
            return -2;
		}
		else{ return 1;}*/
		}
    while(!file.eof())
    {
        string line;
        getline(file, line);
        int f1 = line.find(" ");
		//cout<<"f1 is "<<f1<<endl;
		int f2 = line.find("Pass:",f1+1);
//		//cout<<"f2 is "<<f2<<endl;
		string file_username = line.substr(f1+1,f2-7);
		f1 = line.find(": ",f2+1);
		string file_password = line.substr(f1+2);
        if(line==username+" "+ password)
        {
        user = username;
        return true;}
        else if (file_username==username)
        {
            cout<<"Wrong Password";
            return false;
        }

    }
    cout<<"Wrong username";
    return false;
}

int send_list(string username, const char* database, string& mess)
{
    DIR* p;
    struct dirent* dp;
    if((p=opendir(database))==NULL)
    {
        cerr<<"database";
        return -1;
    }

    while((dp=readdir(p))!= NULL)
    {
        if(string(dp->d_name)==username)
        {
            DIR* q;
            string a =string(database) + username;
            int x = a.length();
            char u[x+1];
            strcpy(u, a.c_str());
            if((q=opendir((const char*) u))==NULL)
            {
                cerr<<"username";
                return -2;
            }
            int n;
            struct dirent* dp1;
            while((dp1=readdir(q))!=NULL)
            {
                n=n+1;
            }
            mess = username+" Number of messages "+to_string(n)+"\n";
            cout<<mess;
            return 2;
        }
    }
}

//int parse_message(const char* filename, string buf, const char* database){
//
////    ifstream passfile;
////	passfile.open(filename);
////  	if(!passfile.good()){
////  		cerr<<"Unable to open file\n";
////  		exit(3);
////  	}
//
//	regex quit("quit");
//	regex message("(User:\\s)(.*)( Pass:\\s)(.*)");
//	regex words("[^:\\s]");
//	regex lists("LIST");
////Quit case
//	if(regex_match(buf, quit)){
//		cout<<"Bye "<<endl;
//		return -1;
//	}
////Extracting Username and Password
//	else if(regex_match(buf, message)){
//
//		int f1 = buf.find(" ");
//		//cout<<"f1 is "<<f1<<endl;
//		int f2 = buf.find("Pass:",f1+1);
////		//cout<<"f2 is "<<f2<<endl;
//		string message_username = buf.substr(f1+1,f2-7);
//		f1 = buf.find(": ",f2+1);
//		string message_password = buf.substr(f1+2);
//		bool an = match_user(passfile, message_username, message_password);
//		if(!an)
//		{
//            //cout<<"User not valid";
//            return -2;
//		}
//		else{ return 1;}
////		if(username!=message_username){
////			cout<<"Invalid User\n";
////			//cout<<"Extracted username is "<<message_username<<"A"<<endl;
////			//cout<<"Extracted password is "<<message_password<<"A"<<endl;
////			//cout<<"Username is "<<username<<"A"<<endl;
////			//cout<<"Password is "<<password<<"A"<<endl;
////			exit(0);
////		}
////		else if(password!=message_password){
////			cout<<"Wrong Passwd\n";
////			exit(0);
////		}
////		else{
////			return 0;
////		}
//	}
//    //else if(regex_match(buf, lists)){
//      //  send_list(username, database);
//        //return 2;
//    //}
//	//Invalid message
//	else{
//		cout<<"Unknown Command\n";
//		return -1;
//	}
//}

int quit(string buf)
{
    regex quit("quit");
    if (regex_match(buf, quit))
    {
        cout<<"Bye"<<endl;
        return -1;
    }

}


int main(int argc, char*argv[]){
//Validates Arguments
	if (argc != 4){
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
    char* database = argv[3];

// Password file

//  	string username,password;
//  	myfile>>username>>password;
//  	// cout<<"Username is "<<username<<endl;
//  	// cout<<"Password is "<<password<<endl;
//  	myfile.close();

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
    while(true)
    {
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
	string username;
	bool a = match_user(filename,buf, username);
	if(!a){
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
	string buf1 = rec_message(in_socket);
	string mess;
	int b = send_list(username, database, mess);
	if(b<0){
		close(in_socket);
	}else{
        send_message(in_socket, mess);
	}


//Third Message
	string buf2 = rec_message(in_socket);
	if(quit(buf2)<0)
	{
        close(in_socket);
	}
}
}
