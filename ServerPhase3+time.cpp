#include<iostream>
#include<fstream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<regex>
#include<unistd.h>
#include<sys/types.h>
#include<dirent.h>
#include<chrono>
#include<algorithm>
#include<cmath>
using namespace std;
using namespace std::chrono;

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

bool match_user(const char* filename, string buf, string& user, int sockfd)
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
        string welcome_message = "Welcome "+username+"\n";
		cout<<welcome_message;
		if(send_message(sockfd,welcome_message) < 0){
			cout<<"Message not sent"<<endl;
			}
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

int send_list(string username, const char* database, int sockfd, std::chrono::microseconds& ab)
{
    high_resolution_clock::time_point start = high_resolution_clock::now();
    high_resolution_clock::time_point en;
    //cout<<username<<endl;
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
        //cout<<"b"<<endl;
            DIR* q;
            string a =string(database) + username;
            cout<<a<<endl;
            int x = a.length();
            char u[x+1];
            strcpy(u, a.c_str());
            if((q=opendir((const char*) u))==NULL)
            {
                cerr<<"username";
                return -2;
            }
            int n = 0;
            struct dirent* dp1;
            while((dp1=readdir(q))!=NULL)
            {
                //cout<<n<<endl;
                n=n+1;
            }
            string mess= username +" Number of messages "+ to_string(n-2);
            cout<<mess<<endl;
            send_message(sockfd, mess);
            en = high_resolution_clock::now();
            ab = duration_cast<microseconds>(en - start);
            return 2;
        }
    }
}

int retrvm(string user, const char* database, string buf, int sockfd, std::chrono::microseconds& b)
{
    high_resolution_clock::time_point start = high_resolution_clock::now();
    high_resolution_clock::time_point en;
    string in = buf.substr(6);
    DIR* p;
    struct dirent* dp;
    if((p=opendir(database))==NULL)
    {
        cerr<<"database";
        return -1;
    }

    while((dp=readdir(p))!= NULL)
    {
        if(string(dp->d_name)==user)
        {
            //cout<<user<<endl;
            DIR* q;
            string a =string(database) + user;
            cout<<a<<endl;
            int x = a.length();
            char u[x+1];
            strcpy(u, a.c_str());
            if((q=opendir((const char*) u))==NULL)
            {
                printf("Error: %s\n", strerror(errno));
                return -2;
            }
            struct dirent* dp1;
            while((dp1=readdir(q))!=NULL)
            {
                string name = string(dp1->d_name);
                cout<<name<<endl;
                int n = name.find(".");
                string index = name.substr(0, n);
                cout<<index<<endl;
                if (index==in){
                    cout<<"c"<<endl;
                           FILE *req_file;
                           int size, read_size, stat, packet_index;
                           char send_buffer[10240]; //read_buffer[256];
                           packet_index = 1;
                            string name1 = a + "/" + name;
                           int x = name1.length();
                           char u[x+1];
                           strcpy(u, name1.c_str());

                           req_file = fopen((const char*)u, "r");
                           printf("Getting req_file Size\n");

                           if(req_file == NULL) {
                                printf("Error Opening File\n");
                                }

                           fseek(req_file, 0, SEEK_END);
                           cout<<"z"<<endl;
                           size = ftell(req_file);
                           cout<<"x"<<endl;
                           fseek(req_file, 0, SEEK_SET);
                           printf("Total req_file size: %i\n",size);

                           //Send req_file Size
                           printf("Sending req_file Size\n");
                           int i = send(sockfd, (void *)&size, sizeof(int), 0);
                           if(i<0){
                            cout<<"Not sent"<<endl;
                            return -1;
                           }

                           cout<<"Sending name of file"<<endl;
                           send_message(sockfd, name);
                           cout<<rec_message(sockfd);

                           //Send req_file as Byte Array
                           printf("Sending req_file as Byte Array\n");

//                           do { //Read while we get errors that are due to signals.
//                              stat=read(socket, &read_buffer , 255);
//                              printf("Bytes read: %i\n",stat);
//                           } while (stat < 0);

                           //printf("Received data in socket\n");
                          // printf("Socket data: %c\n", read_buffer);

                           while(!feof(req_file)) {
                           //while(packet_index = 1){
                              //Read from the file into our send buffer
                              read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, req_file);

                              //Send data through our socket
                              do{
                                stat = send(sockfd, send_buffer, read_size, 0);
                              }while (stat < 0);
                              cout<<rec_message(sockfd);

                              printf("Packet Number: %i\n",packet_index);
                              printf("Packet Size Sent: %i\n",read_size);
                              printf(" \n");
                              printf(" \n");


                              packet_index++;

                              //Zero out our send buffer
                              bzero(send_buffer, sizeof(send_buffer));
                             }
                        en = high_resolution_clock::now();
                        b = duration_cast<microseconds>(en - start);
                        return 1;
                    }
            }
            cout<<"Message Read Fail"<<endl;
            return -2;
        }
    }
}

int parse_message(const char* filename, string buf, const char* database, string& user, int sockfd, std::chrono::microseconds& a, std::chrono::microseconds& b){
	regex quit("quit");
	regex message("(User:\\s)(.*)( Pass:\\s)(.*)");
	regex words("[^:\\s]");
	regex lists("LIST");
	regex retrv ("RETRV \\d");
//Quit case
	if(regex_match(buf, quit)){
		cout<<"Bye "<<endl;
		return -1;
	}
//Extracting Username and Password
	else if(regex_match(buf, message)){

//		int f1 = buf.find(" ");
//		//cout<<"f1 is "<<f1<<endl;
//		int f2 = buf.find("Pass:",f1+1);
////		//cout<<"f2 is "<<f2<<endl;
//		string message_username = buf.substr(f1+1,f2-7);
//		f1 = buf.find(": ",f2+1);
//		string message_password = buf.substr(f1+2);
		bool an = match_user(filename, buf, user, sockfd);
		if(!an)
		{
            //cout<<"User not valid";
            return -2;
		}
		else{ return 1;}
//		if(username!=message_username){
//			cout<<"Invalid User\n";
//			//cout<<"Extracted username is "<<message_username<<"A"<<endl;
//			//cout<<"Extracted password is "<<message_password<<"A"<<endl;
//			//cout<<"Username is "<<username<<"A"<<endl;
//			//cout<<"Password is "<<password<<"A"<<endl;
//			exit(0);
//		}
//		else if(password!=message_password){
//			cout<<"Wrong Passwd\n";
//			exit(0);
//		}
//		else{
//			return 0;
//		}
	}
	else if(regex_match(buf, lists))
	{
	cout<<"a"<<endl;
        if (send_list(user, database, sockfd, a)<0)
        {
            return -3;
        }else{return 2;}
	}
	else if(regex_match(buf, retrv))
	{
        cout<<"b"<<endl;
        if(retrvm(user, database, buf, sockfd, b)<0)
        {
            return -4;
        }
        else{return 3;}
	}
    //else if(regex_match(buf, lists)){
      //  send_list(username, database);
        //return 2;
    //}
	//Invalid message
	else{
		cout<<"Unknown Command\n";
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
    std::chrono::microseconds a;
    std::chrono::microseconds b;

    double av_a= 0; // average time of send_list function
    double av_b=0;  // average time of retrvm function
    double av_a2 = 0;
    double av_b2 = 0;
    double sd_a = 0;  // standard deviation of send_list function
    double sd_b = 0;  // standard deviation of retrvm function
    //high_resolution_clock::time_point c;
    //high_resolution_clock::time_point d;
    //high_resolution_clock::time_point e;
    //high_resolution_clock::time_point f;

// Password file

//  	string username,password;
//  	myfile>>username>>password;
//  	// cout<<"Username is "<<username<<endl;
//  	// cout<<"Password is "<<password<<endl;
//  	myfile.close();
    ofstream infofile;
	infofile.open("info.txt");

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
    int i = 0;
    int j = 0;
//Accept connections
    while(true)
    {
	sockaddr_in client_address;
	int in_socket = accept(sockfd,(struct sockaddr *)&client_address, (socklen_t*)&size_addr);
	high_resolution_clock::time_point start = high_resolution_clock::now();
	high_resolution_clock::time_point en;
	if(in_socket < 0){
		cerr<<"Couldn't accept connection\n";
		exit(2);
	}
	else{
		cout<<"Client: "<<inet_ntoa(client_address.sin_addr)<<":"<<ntohs(client_address.sin_port)<<endl;
	}
	string username;
    //time_t en;
//Receive message
    while(true)
    {
	string buf = rec_message(in_socket);
	int p = parse_message(filename, buf, database, username, in_socket, a, b);
	//time_t en;
	if(p<0)
	{
        close(in_socket);
        //time_t en;
        //time(&en);
        en = high_resolution_clock::now();
        break;
	}
	else if(p==2){
                    cout<<a.count()<<endl;
                    double x = a.count();
                    //double y = b.count();
                    av_a = (av_a*i + x)/(i + 1);
                    av_a2 = (av_a2*i + x*x)/(i+1);

                    cout<<av_a<<" "<<av_a2<<endl;
                    sd_a = sqrt(av_a2 - av_a*av_a);

                    cout<<"sd_a "<<sd_a<<endl;
                    //a = std::chrono::microseconds::zero();
                    //b = std::chrono::microseconds::zero();
                    i++;
	}
	else if (p==3)
	{
        cout<<b.count()<<endl;
        double y = b.count();
        av_b = (av_b*j + y)/(j + 1);
        av_b2 = (av_b2*j + y*y)/(j+1);
        cout<<av_b<<" "<<av_b2<<endl;
        sd_b = sqrt(av_b2 - av_b*av_b);
        cout<<"sd_b "<<sd_b<<endl;
        j++;
	}

//	bool a = match_user(myfile,buf, username);
//	if(!a){
//		close(in_socket);
//	}
//	else{
//		string welcome_message = "Welcome "+username+"\n";
//		cout<<welcome_message;
//		if(send_message(in_socket,welcome_message) < 0){
//			cout<<"Message not sent"<<endl;
//		}
//	}
//
////Second Message
//	buf = rec_message(in_socket);
//	int b = send_list(username, database);
//	if(b<0){
//		close(in_socket);
//	}
//
////Third Message
//	buf = rec_message(in_socket);
//	if(parse_message(myfile, buf, database)<0)
//	{
//        close(in_socket);
//	}
}
    cout<<sd_a<<" "<<sd_b<<endl;
    std::chrono::microseconds time;
	time = duration_cast<microseconds>(en - start);
	cout<<time.count()<<endl;

    //infofile.open("info.txt");
	infofile<<username+ ": "+ to_string(time.count()) + " microseconds"<<endl;
	//infofile.close();
}
infofile.close();
}
