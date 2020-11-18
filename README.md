# RemoteFileServer

This readme will describe how to use the program as well as some of its workings.
Must be compiled and run on Linux to work.

## Client

### Use
Compile the client using

    g++ RemoteFileClient.cpp -o client
    
Then run the client using

    ./client

To specify the IP you would like to connect to, edit IPAddress.txt. By default this IP is 127.0.0.1, the loopback address, so the program can be tested the same computer.

You can run multiple clients on the same machine with no problem.

Once the client program is started, you must first enter your username. If the username is not found on the server, you will have to try again.
This program has no passwords.

Once you have logged in, you then get the choice to enter 'a' to run in admin mode or 'u' to run in user mode.

In all cases, file names and user names are limited to 32 characters.

The program does error checking involving situations such as trying to transfer a file's ownership, but the file or new owner do not exist.
It also detects situations such as listing files owned by a user but that user owns no files.
In such situations it will print a message describing the issue to the user and keep running.

If the client cannot connect to the server, it will display a message to the user that the connection failed and abort running.

### Admin mode options
Admin mode allows the following options:
* List all users
* List all files owned by given username
* Create a new user by given username
* Delete an existing user by given username
* List all files and their owners
* Print the owner of a given file name
* Delete existing file by given name
* Transfer file to a new owner, given file name and name of new owner
* Exit

### User mode options
In user mode, actions are restricted to files owned by the user who logged in before selecting the mode. User mode allows the following options:
* List all owned files
* Upload a file given file name (file must be stored on user's machine in the same directory as client program)
* Download an owned file given file name (file will be downloaded to the same directory as the client program, if a file with the name already exists it will overwrite)
* Delete an owned file given file name
* Transfer owned file to a new owner, given file name and name of new owner
* Exit

### Functionality

Whenever the user selects an option and provides the needed arguments, the client program will pack the desired function name and arguments into a request string.
The first 32 characters of the request string will contain the name of the function. Every argument after gets another 32 characters. 
This is why file and user names are limited to 32 characters.

Whenever sending a file, the first argument (after the function name) will contain the size of the file. The file itself will then be attached to the end of the request,
being of variable length rather than the strict 32 characters as all other arguments are.

After sending the request, the client program waits for a response, which will always be in the form of a string.
The client program then inspects this string to decide what should be outputted to the user.

## Server

### Use
Compile server using

    g++ RemoteFileServer.cpp -o server
    
Then run the server using

    ./server

The server then runs and no further interaction by the user is required. The server will output in the following cases:
* User signs in (what user and if it was successful)
* User is created (user's name)
* User is deleted (user's name)
* File is uploaded (file's name and user that owns it)
* File is deleted (file's name)
* File ownership is changed (file's name, old owner's name, and new owner's name)

Basically, any time the file log, user log, or files are changed the server will output information about that change.

### Files
The server utilizes two logs:

1. File log (List of files and who owns them)

2. User log (List of users)

Many of the user functions simply request information from these logs, such as wanting to view the complete lists, files owned by a specific user, 
what user owns a specific file, etc.

Of course the server must also have the ability to store files. These are kept in the folder FileStorage. When a user uploads a file, 
the file is placed in FileStorage and the file's name along with who uploaded it is recorded to the log.
When a file is deleted, its contents are deleted from FileStorage and its entry in the log is deleted.

When a user is created, they are added to the user log. When a user is deleted, their entry is deleted from the user log.

### Functionality

The server's main function sets up an open connection which accepts requests from clients.

Requests are in the following form. The first 32 characters of a request always hold the name of the desired function. Then, depending on the function, 
there can be 0-3 arguments which each also have 32 characters of space allocated. For this reason, user and file names are limited to 32 characters.

The special case for this functionality is when uploading a file. In this case, the first argument (after the function name) is the length of the file being sent.
Using this the server reads in the variable length file contents which are attached to the end of the request.

After performing the requested function, an output is constructed. While the response is always a string, in some cases it is in the form of a boolean 
by being either "true" or "false". This is to indicate to the client program whether the operation was successful.

For other functions which would normally return a string, there is a check to see if the string is length 0. If so, the server returns "none."
This indicates to the client program that no results were found, such as not being able to find a specified file name 
or if the client requests files owned by a user and that user owns no files.
