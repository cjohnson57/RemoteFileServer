/*
* Name: Johnson, Caleb
* Project: PA-2(Programming)
* Instructor: Feng Chen
* Class: cs7103-au20
* LogonID: cs710310
*/

#include <iostream>
#include <string>
#include <streambuf>
#include <fstream>
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#define PORT 8080 

using namespace std;

/*
* Common Functions
*/

string IP = ""; //IP Address, set at beginning of main from IPAddress.txt

//Make socket connection, send message, get response
string SendAndGetResponse(string message)
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0}; 
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP.c_str(), &serv_addr.sin_addr);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        cout << "Connection to server failed, program aborting." << endl; 
        abort(); 
    } 
    char* charmsg = (char*)malloc(message.length());
    for(int i = 0; i < message.length(); i++)
    {
        charmsg[i] = message[i];      
    }
    send(sock, charmsg, message.length(), 0);
    valread = read(sock, buffer, 1024);
    string response(buffer);
    return response; 
}

//In each function, construct message
//First 32 chars of message is the function name
//Every argument after that each has its own 32 chars
//Therefore args must be padded out to 32 chars, also means usernames and filenames can't exceed 32 chars

bool connect(string username)
{
    string function = "SignIn";
    function.append(32 - function.length(), '\0');
    string arg1 = username;
    arg1.append(32 - arg1.length(), '\0');

    string message = function + arg1;
    return SendAndGetResponse(message) == "true";
}

string GetUserAndFiles(string username)
{
    string function = "UserFiles";
    function.append(32 - function.length(), '\0');
    string arg1 = username;
    arg1.append(32 - arg1.length(), '\0');

    string message = function + arg1;
    return SendAndGetResponse(message);
}

/*
* Admin Functions
*/

string ListAllUsers()
{
    string function = "GetUsers";
    function.append(32 - function.length(), '\0');

    string message = function;
    return SendAndGetResponse(message);
}

bool CreateUser(string username)
{
    string function = "NewUser";
    function.append(32 - function.length(), '\0');
    string arg1 = username;
    arg1.append(32 - arg1.length(), '\0');

    string message = function + arg1;
    return SendAndGetResponse(message) == "true";
}

bool DeleteUser(string username)
{
    string function = "RemoveUser";
    function.append(32 - function.length(), '\0');
    string arg1 = username;
    arg1.append(32 - arg1.length(), '\0');

    string message = function + arg1;
    return SendAndGetResponse(message) == "true";
}

string ListAllFiles()
{
    string function = "ListFiles";
    function.append(32 - function.length(), '\0');

    string message = function;
    string response = SendAndGetResponse(message);
    return response;
}

string PrintFile(string filename)
{
    string function = "ListFile";
    function.append(32 - function.length(), '\0');
    string arg1 = filename;
    arg1.append(32 - arg1.length(), '\0');

    string message = function + arg1;
    return SendAndGetResponse(message);
}

bool DeleteFile(string filename)
{
    string function = "RemoveFile";
    function.append(32 - function.length(), '\0');
    string arg1 = filename;
    arg1.append(32 - arg1.length(), '\0');

    string message = function + arg1;
    return SendAndGetResponse(message) == "true";
}

bool ChangeOwner(string filename, string newowner)
{
    string function = "ChangeFileOwner";
    function.append(32 - function.length(), '\0');
    string arg1 = filename;
    arg1.append(32 - arg1.length(), '\0');
    string arg2 = newowner;
    arg2.append(32 - arg2.length(), '\0');

    string message = function + arg1 + arg2;
    return SendAndGetResponse(message) == "true";
}

/*
* User Functions
*/

bool UploadFile(string filecontents, string filename, string username)
{
    string function = "SendFile";
    function.append(32 - function.length(), '\0');
    //For this one, the first argument will be the length of the file (arg4)
    string arg1 = to_string(filecontents.length());
    arg1.append(32 - arg1.length(), '\0');
    string arg2 = filename;
    arg2.append(32 - arg2.length(), '\0');
    string arg3 = username;
    arg3.append(32 - arg3.length(), '\0');
    string arg4 = filecontents;

    string message = function + arg1 + arg2 + arg3 + arg4;
    return SendAndGetResponse(message) == "true";
}

string DownloadFile(string filename, string username)
{
    string function = "GetFile";
    function.append(32 - function.length(), '\0');
    string arg1 = filename;
    arg1.append(32 - arg1.length(), '\0');
    string arg2 = username;
    arg2.append(32 - arg2.length(), '\0');

    string message = function + arg1 + arg2;
    return SendAndGetResponse(message);
}

bool DeleteOwnedFile(string filename, string username)
{
    string function = "RemoveOwned";
    function.append(32 - function.length(), '\0');
    string arg1 = filename;
    arg1.append(32 - arg1.length(), '\0');
    string arg2 = username;
    arg2.append(32 - arg2.length(), '\0');

    string message = function + arg1 + arg2;
    return SendAndGetResponse(message) == "true";
}

bool TransferOwner(string filename, string originalowner, string newowner)
{
    string function = "TransferOwnership";
    function.append(32 - function.length(), '\0');
    string arg1 = filename;
    arg1.append(32 - arg1.length(), '\0');
    string arg2 = originalowner;
    arg2.append(32 - arg2.length(), '\0');
    string arg3 = newowner;
    arg3.append(32 - arg3.length(), '\0');

    string message = function + arg1 + arg2 + arg3;
    return SendAndGetResponse(message) == "true";
}

/*
* Frontend Functions
*/

int AdminMenu()
{
    int choice;
    while (true) //Repeat until a choice is returned
    {
        cout << endl;
        cout << "1: List All Users" << endl;
        cout << "2: List Files Owned by Specified User" << endl;
        cout << "3: Create User" << endl;
        cout << "4: Delete User" << endl;
        cout << "5: List All Files" << endl;
        cout << "6: Show Specified File and Owner " << endl;
        cout << "7: Delete Specified File " << endl;
        cout << "8: Transfer File Ownership " << endl;
        cout << "9: Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        if (choice >= 1 && choice <= 9)
            return choice;
        cout << "\"" << choice << "\" is not a valid option" << endl;

    }
}

int UserMenu()
{
    int choice;
    while (true) //Repeat until a choice is returned
    {
        cout << endl;
        cout << "1: List Owned Files" << endl;
        cout << "2: Upload File" << endl;
        cout << "3: Download Owned File" << endl;
        cout << "4: Delete Owned File" << endl;
        cout << "5: Transfer File Ownership " << endl;
        cout << "6: Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        if (choice >= 1 && choice <= 6)
            return choice;
        cout << "\"" << choice << "\" is not a valid option" << endl;

    }
}

string EnterUser()
{
    string user;
    while (user.length() == 0 && user.length() < 33)
    {
        cout << "Enter specified user name (32 character max): " << endl;
        cin >> user;
    }
    return user;
}

string EnterUserTransfer()
{
    string user;
    while (user.length() == 0 && user.length() < 33)
    {
        cout << "Enter specified user name for new owner (32 character max): " << endl;
        cin >> user;
    }
    return user;
}

string EnterFile()
{
    string filename;
    while (filename.length() == 0 && filename.length() < 33)
    {
        cout << "Enter specified file name (including extension) (32 characters max): " << endl;
        cin >> filename;
    }
    return filename;
}

int main()
{
    //Get IP address from file
    ifstream IPFile;
    IPFile.open("IPAddress.txt");
    IPFile >> IP;
    IPFile.close();
    //Sign in
    cout << "Please enter your username: ";
    string username;
    cin >> username;
    while (!connect(username))
    {
        cout << "User does not exist, please try again." << endl;
        cin >> username;
    }
    cout << "Log in successful. Please choose to run in admin or user mode (type a for admin or u for user)." << endl;
    string choice;
    cin >> choice;
    while(choice != "a" && choice != "u")
    {
        cout << "Please type a for admin mode or u for user mode." << endl;
        cin >> choice;
    }
    if (choice == "a")
    {
        //Admin CMD
        int choice = AdminMenu();
        while (choice != 9)
        {
            if (choice == 1) //List all users
            {
                string users = ListAllUsers();
                if(users == "none")
                {
                    cout << "No users found on server." << endl;
                }
                else
                {
                    cout << users << endl;
                }
            }
            else if (choice == 2) //List all owned files for user
            {
                string user = EnterUser();
                cout << "Owned files for user " + user + ": " << endl;
                string ownedfiles = GetUserAndFiles(user);
                if (ownedfiles == "none") //Nothing returned
                {
                    cout << "No files found, either user does not exist or user owns no files." << endl;
                }
                else
                {
                    cout << ownedfiles;
                }
            }
            else if (choice == 3) //Create new user
            {
                string user = EnterUser();
                bool created = CreateUser(user);
                if (created)
                {
                    cout << "User " + user + " successfully created." << endl;
                }
                else
                {
                    cout << "User " + user + " already exists." << endl;
                }
            }
            else if (choice == 4) //Delete user
            {
                string user = EnterUser();
                bool deleted = DeleteUser(user);
                if (deleted)
                {
                    cout << "User " + user + " successfully deleted." << endl;
                }
                else
                {
                    cout << "User " + user + " does not exist so can't be deleted." << endl;
                }
            }
            else if (choice == 5) //List all files
            {
                string files = ListAllFiles();
                if(files == "none")
                {
                    cout << "No files found on server." << endl;
                }
                else
                {
                    cout << files << endl;
                }          
            }
            else if (choice == 6) //List file and its owner
            {
                string filename = EnterFile();
                string fileandowner = PrintFile(filename);
                if (fileandowner == "none") //Nothing returned
                {
                    cout << "File name " + filename + " not found." << endl;
                }
                else
                {
                    cout << fileandowner << endl;
                }
            }
            else if (choice == 7) //Delete a file
            {
                string filename = EnterFile();
                bool deleted = DeleteFile(filename);
                if (deleted)
                {
                    cout << "File " + filename + " successfully deleted." << endl;
                }
                else
                {
                    cout << "File " + filename + " does not exist so can't be deleted." << endl;
                }
            }
            else if (choice == 8) //Change ownership of a file
            {
                string filename = EnterFile();
                string user = EnterUserTransfer();
                bool changed = ChangeOwner(filename, user);
                if (changed)
                {
                    cout << "File " + filename + " successfully transferred ownership to " + user + "." << endl;
                }
                else
                {
                    cout << "Either file " + filename + " or user " + user + " does not exist so can't transfer ownership." << endl;
                }
            }
            //Show menu and get next choice
            choice = AdminMenu();
        }
    }
    else
    {
        //User CMD
        int choice = UserMenu();
        while (choice != 6)
        {
            if (choice == 1) //List all owned files for user
            {
                cout << "Owned files for user " + username + ": " << endl;
                string ownedfiles = GetUserAndFiles(username);
                if (ownedfiles == "none") //Nothing returned
                {
                    cout << "No owned files found." << endl;
                }
                else
                {
                    cout << ownedfiles << endl;
                }
            }
            else if (choice == 2) //Upload File
            {
                string filename = EnterFile();
                //Get file from pc as char array
                streampos size;
                char* memblock;
                string filecontents;
                ifstream file(filename, ios::in | ios::binary | ios::ate);
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
                //Upload to server
                if (filecontents.length() == 0)
                {
                    cout << "File " + filename + " either does not exist or is empty." << endl;
                }
                else
                {
                    bool uploaded = UploadFile(filecontents, filename, username);
                    if (uploaded)
                    {
                        cout << "File " + filename + " successfully uploaded." << endl;
                    }
                    else
                    {
                        cout << "File " + filename + " already exists on server." << endl;
                    }
                }
            }
            else if (choice == 3) //Download File
            {
                string filename = EnterFile();
                string filecontents = DownloadFile(filename, username);
                if (filecontents == "none") //Nothing returned
                {
                    cout << "Either file does not exist or it is not owned by user " + username + "." << endl;
                }
                else
                {
                    //Save file contents down as file name
                    ofstream out(filename);
                    out.write(filecontents.c_str(), filecontents.size());
                    out.close();
                    cout << "File " << filename << " downloaded successfully." << endl;
                }
            }
            else if (choice == 4) //Delete a file
            {
                string filename = EnterFile();
                bool deleted = DeleteOwnedFile(filename, username);
                if (deleted)
                {
                    cout << "File " + filename + " successfully deleted." << endl;
                }
                else
                {
                    cout << "File " + filename + " either does not exist or is not owned by " + username + " so can't be deleted." << endl;
                }
            }
            else if (choice == 5) //Change ownership of a file
            {
                string filename = EnterFile();
                string user = EnterUserTransfer();
                bool changed = TransferOwner(filename, username, user);
                if (changed)
                {
                    cout << "File " + filename + " successfully transferred ownership to " + user + "." << endl;
                }
                else
                {
                    cout << "File " + filename + " either does not exist or is not owned by " + username + " or new owner " + user + " does not exist, so can't transfer ownership." << endl;
                }
            }
            //Show menu and get next choice
            choice = UserMenu();
        }
    }
}

