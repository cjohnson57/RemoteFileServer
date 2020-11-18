/*
* Name: Johnson, Caleb
* Project: PA-2(Programming)
* Instructor: Feng Chen
* Class: cs7103-au20
* LogonID: cs710310
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <streambuf>
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#define PORT 8080 

using namespace std;

struct FileInfo
{
public:
	string Name;
	string Owner;
};

/*
* Common Functions
*/

//Gets current file log
vector<FileInfo> GetFileLog()
{
	//Parse log file to find the name and owner of each file in the log
	vector<FileInfo> files;
	ifstream log("FileLog.txt");
	string line;
	while (getline(log, line))
	{
		string name = line.substr(0, line.find(',')); //Get name, before comma
		string owner = line.substr(line.find(',') + 1, line.length()); //Get owner, after comma
		//Use name and owner to make FileInfo object and add to vector
		FileInfo newfile;
		newfile.Name = name;
		string trimmedowner = owner.erase(owner.find_last_not_of(" \n\r\t")+1);
		newfile.Owner = trimmedowner;
		files.push_back(newfile);
	}
	return files; //Return list of files
}

//Saves edited file log
string SaveFileLog(vector<FileInfo> files)
{
	//Construct string representation of new file list
	string log = "";
	for (int i = 0; i < files.size(); i++)
	{
		log += files[i].Name + "," + files[i].Owner + "\n";
	}
	//Save string to file
	try
	{
		ofstream out("FileLog.txt");
		out << log;
		out.close();
		return "true";
	}
	catch (exception ex)
	{
		return "false";
	}
}

//Gets current user log
vector<string> GetUserLog()
{
	//Parse user log file to find each user in the log, one per line
	vector<string> users;
	ifstream log("UserLog.txt");
	string line;
	while (getline(log, line))
	{
		string trimmed = line.erase(line.find_last_not_of(" \n\r\t")+1);
		users.push_back(trimmed);
	}
	return users; //Return list of files
}

//Saves edited user log
string SaveUserLog(vector<string> users)
{
	//Construct string representation of new file list
	string log = "";
	for (int i = 0; i < users.size(); i++)
	{
		log += users[i] + "\n";
	}
	//Save string to file
	try
	{
		ofstream out("UserLog.txt");
		out << log;
		out.close();
		return "true";
	}
	catch (exception ex)
	{
		return "false";
	}
}

//If user exists, return true
string SignIn(string username)
{
	vector<string> users = GetUserLog();
	for (int i = 0; i < users.size(); i++)
	{
		if (users[i] == username) //User exists in list, return true
		{
			return "true";
		}
	}
	return "false"; //User does not exist in list, return false
}

//Get all files owned by user specified in argument
string UserFiles(string user)
{
	//Check each file to see if owned by specified user, if so add to list
	vector<FileInfo> files = GetFileLog();
	string filelist = "";
	for (int i = 0; i < files.size(); i++)
	{
		if (files[i].Owner == user)
		{
			filelist += files[i].Name + "\n";
		}
	}
	return filelist;
}

/*
* Admin Functions
*/

//Simply construct string that has each user on one line (like the UserLog file itself)
string GetUsers()
{
	vector<string> users = GetUserLog();
	string userlist = "";
	for (int i = 0; i < users.size(); i++)
	{
		userlist += users[i] + "\n";
	}
	return userlist;
}

//Add a new user by getting the user list, adding a name to the list, then saving modified list back down
string NewUser(string name)
{
	vector<string> users = GetUserLog();
	//Make sure name doesn't already exist
	for (int i = 0; i < users.size(); i++)
	{
		if (users[i] == name)
		{
			return "false";
		}
	}
	//Doesn't already exist, add to list and save
	users.push_back(name);
	return SaveUserLog(users);
}

//Remove a user by getting the user list, taking them out, then saving it back down
string RemoveUser(string name)
{
	vector<string> users = GetUserLog();
	//Find the index user is located at, if it exists
	int atindex = -1;
	for (int i = 0; i < users.size(); i++)
	{
		//User found, save index and break loop
		if (users[i] == name)
		{
			atindex = i;
			break;
		}
	}
	//If user not found, return false, if they are remove from list and save modified log
	if (atindex > -1)
	{
		users.erase(users.begin() + atindex);
		return SaveUserLog(users);
	}
	else
	{
		return "false";
	}
}

//Get all files and their owners
string ListFiles()
{
	//Check each file and add to list
	vector<FileInfo> files = GetFileLog();
	string filelist = "";
	for (int i = 0; i < files.size(); i++)
	{
		filelist += "Name: " + files[i].Name + ", " + "Owner: " + files[i].Owner + "\n";
	}
	return filelist;
}

//Get a specified file's name and owner
string ListFile(string name)
{
	//Check each file to see if owned by specified user, if so add to list
	vector<FileInfo> files = GetFileLog();
	for (int i = 0; i < files.size(); i++)
	{
		if (files[i].Name == name)
		{
			return "Name: " + name + ", " + "Owner: " + files[i].Owner;
		}
	}
	return "";
}

//Remove specified file
string RemoveFile(string name)
{
	vector<FileInfo> files = GetFileLog();
	//Find the index user is located at, if it exists
	int atindex = -1;
	for (int i = 0; i < files.size(); i++)
	{
		//File found, save index and break loop
		if (files[i].Name == name)
		{
			atindex = i;
			break;
		}
	}
	//If user not found, return false, if they are remove from list and save modified log, also delete file
	if (atindex > -1)
	{
		files.erase(files.begin() + atindex);
		string filepath = "FileStorage/" + name;
		remove(filepath.c_str()); //Delete file from storage
		return SaveFileLog(files);
	}
	else
	{
		return "false";
	}
}

string ChangeFileOwner(string file, string newowner)
{
	vector<FileInfo> files = GetFileLog();
	//Find the index user is located at, if it exists
	int atindex = -1;
	for (int i = 0; i < files.size(); i++)
	{
		//File found, save index and break loop
		if (files[i].Name == file)
		{
			atindex = i;
			break;
		}
	}
	bool newownerexists = false;
	vector<string> users = GetUserLog();
	//Make sure name doesn't already exist
	for (int i = 0; i < users.size(); i++)
	{
		if (users[i] == newowner)
		{
			newownerexists = true;
			break;
		}
	}
	//If user not found, return false, if they are edit owner in list and save back down
	if (atindex > -1 && newownerexists)
	{
		files[atindex].Owner = newowner;
		return SaveFileLog(files);
	}
	else
	{
		return "false";
	}
}

/*
* User Functions
*/

//Add file to log and save contents, if it doesn't already exist
string SendFile(string filecontents, string filename, string username)
{
	//Check that file doesn't already exist in log
	vector<FileInfo> files = GetFileLog();
	for (int i = 0; i < files.size(); i++)
	{
		//File found, save index and break loop
		if (files[i].Name == filename)
		{
			return "false";
		}
	}
	//Add to log
	FileInfo newfile;
	newfile.Name = filename;
	newfile.Owner = username;
	files.push_back(newfile);
	SaveFileLog(files);
	//Save file contents down as file name
	ofstream out("FileStorage/" + filename);
	out.write(filecontents.c_str(), filecontents.size());
	out.close();
	return "true";
}

//Download specified file name if owned by specified user
string GetFile(string filename, string user)
{
	//Must check that file exists and is owned by user
	vector<FileInfo> files = GetFileLog();
	for (int i = 0; i < files.size(); i++)
	{
		//File found, now return it
		if (files[i].Name == filename && files[i].Owner == user)
		{
			//Read in contents of file as char array
			streampos size;
			char* memblock;
			string filecontents;
			ifstream file("FileStorage/" + filename, ios::in | ios::binary | ios::ate);
			if (file.is_open())
			{
				size = file.tellg();
				memblock = new char[size];
				file.seekg(0, ios::beg);
				file.read(memblock, size);
				file.close();
				string tempcontents(memblock, size);
				delete[] memblock;
				filecontents = tempcontents;
			}
			return filecontents;
		}
	}
	return "";
}

//Remove specified file if it is owned by the specified user
string RemoveOwned(string file, string user)
{
	vector<FileInfo> files = GetFileLog();
	//Find the index user is located at, if it exists
	int atindex = -1;
	for (int i = 0; i < files.size(); i++)
	{
		//File found, save index and break loop
		if (files[i].Name == file && files[i].Owner == user)
		{
			atindex = i;
			break;
		}
	}
	//If user not found, return false, if they are remove from list and save modified log, also delete file
	if (atindex > -1)
	{
		files.erase(files.begin() + atindex);
		string filepath = "FileStorage/" + file;
		remove(filepath.c_str()); //Delete file from storage
		return SaveFileLog(files);
	}
	else
	{
		return "false";
	}
}

//Transfer ownership from one user to another if specified file is owned by specified user
string TransferOwnership(string file, string originalowner, string newowner)
{
	vector<FileInfo> files = GetFileLog();
	//Find the index user is located at, if it exists
	int atindex = -1;
	for (int i = 0; i < files.size(); i++)
	{
		//File found, save index and break loop
		if (files[i].Name == file && files[i].Owner == originalowner)
		{
			atindex = i;
			break;
		}
	}
	bool newownerexists = false;
	vector<string> users = GetUserLog();
	//Make sure name doesn't already exist
	for (int i = 0; i < users.size(); i++)
	{
		if (users[i] == newowner)
		{
			newownerexists = true;
			break;
		}
	}
	//If user not found, return false, if they are edit owner in list and save back down
	if (atindex > -1 && newownerexists)
	{
		files[atindex].Owner = newowner;
		return SaveFileLog(files);
	}
	else
	{
		return "false";
	}
}

//Get argument from buffer
string GetArg(char* buffer, int start)
{
	char* value = (char*)malloc(32);
	for(int i = 0; i < 32; i++)
	{
		value[i] = buffer[start + i];
	}
	string output(value);
	return value;
}

//Get file argument (specified length) from buffer
string GetFileArg(char* buffer, int start, int length)
{
	char* value = (char*)malloc(length + 1);
	for(int i = 0; i < length; i++)
	{
		value[i] = buffer[start + i];
	}
	value[length] = '\0';
	string output(value);
	return output;
}

int main()
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	bind(server_fd, (struct sockaddr*)&address, sizeof(address));
	listen(server_fd, 3);
	cout << "Startup successful, waiting for requests." << endl;
	while (true)
	{
		new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		valread = read(new_socket, buffer, 1024);
		cout << "Message detected." << endl;
		string function = GetArg(buffer, 0); //First 32 characters will be the function name
		cout << "Function called: " << function << endl;
		//Now must consider the function to parse the arguments
		//Each argument given 32 characters
		if (function == "SignIn")
		{
			string username = GetArg(buffer, 32);
			cout << "User signing in: " << username << endl;
			string response = SignIn(username);
			cout << "Successful? " << response << endl;
			send(new_socket, response.c_str(), response.length(), 0);
		}
		else if (function == "UserFiles")
		{
			string username = GetArg(buffer, 32);
			string response = UserFiles(username);
			if(response.length() == 0)
			{
				response = "none";
			}
			send(new_socket, response.c_str(), response.length(), 0);
		}
		else if (function == "GetUsers")
		{
			string response = GetUsers();
			if(response.length() == 0)
			{
				response = "none";
			}
			send(new_socket, response.c_str(), response.length(), 0);

		}
		else if (function == "NewUser")
		{
			string username = GetArg(buffer, 32);
			string response = NewUser(username);
			if(response == "true")
			{
				cout << "User created: " << username << endl;
			}
			send(new_socket, response.c_str(), response.length(), 0);

		}
		else if (function == "RemoveUser")
		{
			string username = GetArg(buffer, 32);
			string response = RemoveUser(username);
			if(response == "true")
			{
				cout << "User deleted: " << username << endl;
			}
			send(new_socket, response.c_str(), response.length(), 0);

		}
		else if (function == "ListFiles")
		{
			string response = ListFiles();
			if(response.length() == 0)
			{
				response = "none";
			}
			send(new_socket, response.c_str(), response.length(), 0);

		}
		else if (function == "ListFile")
		{
			string filename = GetArg(buffer, 32);
			string response = ListFile(filename);
			if(response.length() == 0)
			{
				response = "none";
			}
			send(new_socket, response.c_str(), response.length(), 0);

		}
		else if (function == "RemoveFile")
		{
			string filename = GetArg(buffer, 32);
			string response = RemoveFile(filename);
			if(response == "true")
			{
				cout << "Deleted file " << filename << endl;
			}
			send(new_socket, response.c_str(), response.length(), 0);
		}
		else if (function == "ChangeFileOwner")
		{
			string filename = GetArg(buffer, 32);
			string username = GetArg(buffer, 64);
			string response = ChangeFileOwner(filename, username);
			if(response == "true")
			{
				cout << "File " << filename << " changed owner to " << username << endl;
			}
			send(new_socket, response.c_str(), response.length(), 0);
		}
		else if (function == "SendFile")
		{
			//This one is special as the first 32 bits must contain the length of the file
			int length = stoi(GetArg(buffer, 32));
			string filename = GetArg(buffer, 64);
			string username = GetArg(buffer, 96);
			string filecontents = GetFileArg(buffer, 128, length); //Get file of arbitrary length
			string response = SendFile(filecontents, filename, username);
			if(response == "true")
			{
				cout << "File " << filename << " created, owned by " << username << endl;
			}
			send(new_socket, response.c_str(), response.length(), 0);
		}
		else if (function == "GetFile")
		{
			string filename = GetArg(buffer, 32);
			string username = GetArg(buffer, 64);
			string response = GetFile(filename, username);
			if(response.length() == 0)
			{
				response = "none";
			}
			send(new_socket, response.c_str(), response.length(), 0);

		}
		else if (function == "RemoveOwned")
		{
			string filename = GetArg(buffer, 32);
			string username = GetArg(buffer, 64);
			string response = RemoveOwned(filename, username);
			if(response == "true")
			{
				cout << "Deleted file " << filename << endl;
			}
			send(new_socket, response.c_str(), response.length(), 0);
		}
		else if (function == "TransferOwnership")
		{
			string filename = GetArg(buffer, 32);
			string oldowner = GetArg(buffer, 64);
			string newowner = GetArg(buffer, 96);
			string response = TransferOwnership(filename, oldowner, newowner);
			if(response == "true")
			{
				cout << "File " << filename << " changed owner from " << oldowner << " to " << newowner << endl;
			}
			send(new_socket, response.c_str(), response.length(), 0);
		}			
	}
}