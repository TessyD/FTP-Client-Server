#include "server.h"

int newSocket;
//int count = 0;
char file[1024]; // to store contents from client in server
char file_name[100];
char pwdbuf[256];
char list[1024];
struct stat st = {0};
char renfr[1024];
char pwdbuf1[256];


void split(char *pathaname);
int changeDirectory(char *directory);
int show_currentDirectory();
int ListFilesInDirectory();
int makeDirectory(char *directory);
int write_file(char *filename, int sockfd);
int retr_file(char *filename);



void success()
{ 
    int ret, portNumber;
    struct sockaddr_in serverAddr; // Server Socket Address Structure
    struct sockaddr_in newAddr;    // Client Socket Address Structure
    socklen_t addr_size;           // Value Result Argument.

    int sockfd;
    char buffer[MAXLINE];
    char buflist[MAXLINE];
    pid_t childpid;

            while (1)
            {
                recv(newSocket, buffer, 1024, 0); // receiving input commands from client

                // handle QUIT command
                if (strncmp(buffer, "QUIT", 4) == 0)
                {
                    // printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                    printf("temp disconnect\n");
                    char mssg[256] = "Server disconnected";
                    send(newSocket, mssg, strlen(mssg), 0);
                    break;
                }
                // handle USER command
                // else if (strncmp(buffer, "USER", 4) == 0)
                // {
                //     // char temp[10];
                //     // send(newSocket, temp, 10, 0);
                //     printf(">> Client - %s has Successfully Logged In.\n");
                //     char msg[256] = "User has successfully logged in";
                //     send(newSocket, msg, strlen(msg), 0); // send the user logged in message to client
                // }
                // handle change directory logic
                else if (strncmp(buffer, "CWD", 3) == 0)
                {
                    printf("\nInside CWD logic");
                    int k = 0;
                    char ans[1024];
                    for (int i = 4; i < strlen(buffer); i++)
                    {
                        ans[k] = buffer[i];
                        k++;
                    }
                    strtok(ans, "\n");
                    int status = changeDirectory(ans); // change the dorectory based on argument passed
                    if (status == 0)
                    {
                        // show the current directory
                        status = show_currentDirectory();
                        if (status == 343)
                        {
                            // if current directory status returned successfully
                           // static char buf[256];
                            char mssg[256] = "Reply[200]: Command is Okay.\nReply[343]: Remote Working Directory: ";
                            strcat(mssg,pwdbuf1);
                            send(newSocket, mssg, strlen(mssg), 0);
                           // bzero(pwdbuf1, sizeof(pwdbuf1));
                        }
                    }
                }
                // handle CDUP directory
                else if (strncmp(buffer, "CDUP", 4) == 0)
                {
                    printf("\nInside CDUP logic");
                    char *ans = "..";
                    int status = changeDirectory(ans); // change the dorectory based on argument passed
                    if (status == 0)
                    {
                        // show the current directory
                        status = show_currentDirectory();
                        if (status == 343)
                        {
                            // if current directory status returned successfully
                           // static char buf[256];
                            char mssg[256] = "Reply[200]: Command is Okay.\nReply[343]: Remote Working Directory: ";
                            strcat(mssg, pwdbuf1);
                            send(newSocket, mssg, strlen(mssg), 0);
                        }
                    }
                }
                // handle list
                else if (strncmp(buffer, "LIST", 4) == 0)
                {
                    printf("inside the list func\n");
                    int status = ListFilesInDirectory();
                    if (status == 0)
                    {
                        strcpy(buflist, list);
                        send(newSocket, buflist, strlen(buflist), 0); // send the listed files to client side
                    }
                }
                // handle MKD
                else if (strncmp(buffer, "MKD", 3) == 0)
                {
                    printf("Inside MKD func");
                    int k = 0;
                    char ans[1024];
                    for (int i = 4; i < strlen(buffer); i++)
                    {
                        ans[k] = buffer[i];
                        // printf("Each character: %c\n",ans[k]);
                        k++;
                    }
                    strtok(ans, "\n");
                    int status = makeDirectory(ans); // passing name of the folder to be created as an argument
                    if (status == 336)
                    {
                        // successfully created
                        char mssg[256] = "Directory Successfully created --> ";
                        strcat(mssg, ans);
                        send(newSocket, mssg, strlen(mssg), 0); // send the new directory created message to client
                        bzero(mssg, strlen(mssg));
                    }
                    else if (status == 337)
                    {
                        // already exists
                        char mssg[256] = "Directory Already Exists, so cannot be created again ";
                        send(newSocket, mssg, strlen(mssg), 0);
                    }
                }
                // handle RMD function
                else if (strncmp(buffer, "RMD", 3) == 0)
                {
                    printf("inside RMD func");
                    int k = 0;
                    char ans[1024];
                    for (int i = 4; i < strlen(buffer); i++)
                    {
                        ans[k] = buffer[i];
                        // printf("Each character: %c\n",ans[k]);
                        k++;
                    }
                    strtok(ans, "\n");
                    int status = rmdir(ans);
                    if (status == 0)
                    {
                        // successfully deleted
                        char mssg[256] = "Directory Successfully deleted ";
                        send(newSocket, mssg, strlen(mssg), 0);
                    }
                    else if (status == 1)
                    {
                        // not delted somehow
                        char mssg[256] = "Not successful in deleting ";
                        send(newSocket, mssg, strlen(mssg), 0);
                    }
                }
                // handle present working directory logic
              //  else if (strncmp(buffer, "PWD ", 4) == 0 || strncmp(buffer, "pwd ", 4) == 0 ||
              //           strncmp(buffer, "PWD\t", 4) == 0 || strncmp(buffer, "pwd\t", 4) == 0 ||
                //         strncmp(buffer, "PWD\n", 4) == 0 || strncmp(buffer, "pwd\n", 4) == 0)
                else if (strncmp(buffer, "PWD", 3) == 0)
                {
                    printf("\n Inside PWD logic");
                    int stat = show_currentDirectory();
                    if (stat)
                    {
                        // if current directory status returned successfully
                        //static char bufpwd[256];
                        char mssg1[256] = "Reply[200]: Command is Okay.\nReply[343]: Remote Working Directory: ";
                        // split(pwdbuf);
                        strcat(mssg1, pwdbuf1);
                        send(newSocket, mssg1, strlen(mssg1), 0);
                        //bzero(pwdbuf1, sizeof(pwdbuf1));
                    }
                }
                // handle STOR command
                else if (strncmp(buffer, "STOR ", 5) == 0)
                {
                    // printf("Display the stor byffer value: %s", buffer);
                    printf("Inside the STOR func");
                    int k = 0;
                    char ans[1024];
                    for (int i = 5; i < strlen(buffer); i++)
                    {
                        ans[k] = buffer[i];
                        // printf("Each character: %c\n",ans[k]);
                        k++;
                    }
                    strtok(ans, "\n");
                    printf("\nfilename to be stored from client to server: %s\n", ans);
                    char *answer = ans;
                    // sleep(2);
                    send(newSocket, answer, strlen(answer), 0); // send the filename to client
                    // sleep(2);
                    if(access(answer, F_OK) != -1)
                    {
                        char mssg[256] = "File already exist in Server";
                        send(newSocket, mssg, strlen(mssg), 0);
                    }
                    else
                    { 
                    int status = write_file(answer, newSocket); // passing the filename as an argument
                    // if write file was successful, send message to client
                    if (status == 0)
                    {
                        char mssg[256] = "File uploaded in Server sucessfully";
                        send(newSocket, mssg, strlen(mssg), 0);
                    }
                    else if (status == -1)
                    {
                        char mssg[256] = "Some Error happened in receiving socket";
                        send(newSocket, mssg, strlen(mssg), 0);
                    }
                    }
                    
                }

                // handle APPE command
                else if (strncmp(buffer, "APPE ", 5) == 0)
                {
                    // printf("Display the stor byffer value: %s", buffer);
                    printf("Inside the APPE func");
                    int k = 0;
                    char ans1[1024];
                    for (int i = 5; i < strlen(buffer); i++)
                    {
                        ans1[k] = buffer[i];
                        // printf("Each character: %c\n",ans[k]);
                        k++;
                    }
                    strtok(ans1, "\n");
                    printf("\nfilename to be stored from client to server: %s\n", ans1);
                    char *answer = ans1;
                    // sleep(2);
                    send(newSocket, answer, strlen(answer), 0); // send the filename to client
                    // sleep(2);
                    if(access(answer, F_OK) != -1)
                    {
                        char mssg[256] = "File already exist in Server, appending the file content";
                     /*   FILE *ptr;
                        FILE *ptr1;
                        ptr = fopen("answer","r");
                        ptr1 = fopen("answer","a");
                        fputc(ptr,ptr1);
                        fclose(ptr1);
                        fclose(ptr);*/
                        int status = app_write_file(answer, newSocket);
                        send(newSocket, mssg, strlen(mssg), 0);

                    }
                    else
                    { 
                    int status = app_write_file(answer, newSocket); // passing the filename as an argument
                    // if write file was successful, send message to client
                    if (status == 0)
                    {
                        char mssg[256] = "File uploaded in Server sucessfully";
                        send(newSocket, mssg, strlen(mssg), 0);
                    }
                    else if (status == -1)
                    {
                        char mssg[256] = "Some Error happened in receiving socket";
                        send(newSocket, mssg, strlen(mssg), 0);
                    }
                    }
                    
                }
                // handle RETR logic
                else if (strncmp(buffer, "RETR ", 5) == 0)
                {
                    printf("Inside the RETR func");
                    int k = 0;
                    char ans[1024];
                    char temp[1024];
                    for (int i = 5; i < strlen(buffer); i++)
                    {
                        ans[k] = buffer[i];
                        // printf("Each character: %c\n",ans[k]);
                        k++;
                    }
                    strtok(ans, "\n");
                    printf("\nfilename to be downloaded to client from server: %s\n", ans);
                    strcpy(temp, ans);

                    // get file contents from server
                    int status = retr_file(ans); // passing the filename as an argument
                    if (status == 0)
                    {
                        send(newSocket, temp, MAXLINE, 0); // send the message to client
                        printf("\nWhat are the file contents: %s\n", file);
                        send(newSocket, file, MAXLINE, 0); // send the file contents
                        printf(">> File Sent Successfully.\n");
                    }
                    else
                    {
                        printf("Some error");
                    }
                }
                // give OK respnse to server
                else if (strncmp(buffer, "NOOP", 4) == 0)
                {
                    printf("\nNOOP EXECUTING: \nSending Message to client\n");
                    strcpy(buffer, "OKIEEE\n");
                    send(newSocket, buffer, strlen(buffer), 0);
                   
                }
                // delete file from server
                else if (strncmp(buffer, "DELE ", 5) == 0)
                {
                    int k = 0;
                    char ans[1024];
                    char temp[1024];

                    for (int i = 5; i < strlen(buffer); i++)
                    {
                        ans[k] = buffer[i];
                        // printf("Each character: %c\n",ans[k]);
                        k++;
                    }
                    strtok(ans, "\n");
                    // printf("\nfilename is %s\n", ans);
                    remove(ans);
                    strcpy(buffer, "File deleted Successfully\n");
                    send(newSocket, buffer, strlen(buffer), 0);
                }
                // handle ABOR
                else if (strncmp(buffer, "ABOR", 4) == 0)
                {
                    strcpy(buffer, "user logged out successfully login again!!\n"); // in future we will be using boolean flag to set the login value to true and false
                    send(newSocket, buffer, strlen(buffer), MSG_OOB);
                }
                // handle REIN
                else if (strncmp(buffer, "REIN", 4) == 0)
                {
                    //UserStatus = 0;
                    printf("\nReinitialized...User logged out\n");
                    
                  //  strcpy(buffer, "User logged out, reinitializing server\n");
                   // send(newSocket, buffer, strlen(buffer), 0);
                    strcpy(buffer, "User logged out successfully!!, login again!!\n"); // in future we will be using boolean flag to set the login value to true and false
                    send(newSocket, buffer, strlen(buffer), 0);
                    bzero(buffer, sizeof(buffer));
                    break;
                }
                // handle RNFR and RNTO
                    else if (strncmp(buffer, "RNFR ", 5) == 0)
                    {

                        printf("Display the stor buffer value: %s", buffer);

                        int k = 0;
                        char ans[1024];
                        char temp[1024];
                        strcpy(temp,buffer);
                        bzero(buffer, sizeof(buffer));                        
                        for (int i = 5; i < strlen(temp); i++)
                        {
                            ans[k] = temp[i];
                            // printf("Each character: %c\n",ans[k]);
                            k++;
                        }
                        strtok(ans, "\n");
                        printf("\nfilename is %s\n", ans);
                        strcpy(renfr, ans);
                        strcpy(buffer, " You need to enter RNTO command now!!\n");
                        send(newSocket, buffer, strlen(buffer), 0);
                    }
                    else if (strncmp(buffer, "RNTO ", 5) == 0)
                    {

                        //printf("Display the stor buffer value: %s", buffer);

                        int k = 0;
                        char ans[1024];
                        char temp[1024];
                        strcpy(temp,buffer);
                        bzero(buffer, sizeof(buffer));
                        for (int i = 5; i < strlen(temp); i++)
                        {
                            ans[k] = temp[i];
                            // printf("Each character: %c\n",ans[k]);
                            k++;
                        }
                        strtok(ans, "\n");
                        printf("\nfilename is %s\n", ans);
                        rename(renfr, ans);
                        strcpy(buffer," Filename is changed successfully!!\n");
                        send(newSocket, buffer, strlen(buffer), 0);
                    }

                else
                {
                    // printf("Client: %s\n", buffer);
                    send(newSocket, buffer, strlen(buffer), 0);
                    bzero(buffer, sizeof(buffer));
                }
                bzero(buffer, sizeof(buffer));
                //status = 1;
            }
            return 0;
}

int app_write_file(char *filename, int sockfd)
{
    int n;
    FILE *fp;
    char buffer[MAXLINE];
    char clientContent[MAXLINE];
    printf("file name %s\n", filename);
    fp = fopen(filename, "a");
    printf("hello there- file opening \n");
    if (fp == NULL)
    {
        perror("[-]Error in creating file");
        exit(1);
    }
    while (1)
    {
        n = recv(sockfd, buffer, MAXLINE, 0);
        if (n <= 0)
        {
            break;
            return -1;
        }
        //printf("\nDisplay the clientContent: %s\n", buffer);
        strcat(clientContent, buffer);
        bzero(buffer, sizeof(buffer));
        bzero(buffer, sizeof(buffer));
        printf("\nDisplay the clientContent: %s\n", clientContent);
        //fwrite(fp,clientContent);
        fprintf(fp, "%s", clientContent);
        fclose(fp);
        bzero(clientContent, sizeof(clientContent));
        // sleep(1);

        printf("\n>> File Name: %s", filename);
        printf("\n>> File Size: %ld bytes\n", strlen(clientContent));
        // close(sockfd);
        return 0;
    }
    return 0;
}



// write the contents in the file
int write_file(char *filename, int sockfd)
{
    int n;
    FILE *fp;
    char buffer[MAXLINE];
    char clientContent[MAXLINE];
    printf("file name %s\n", filename);
    fp = fopen(filename, "w");
    printf("hello there- file opening \n");
    if (fp == NULL)
    {
        perror("[-]Error in creating file");
        exit(1);
    }
    while (1)
    {
        n = recv(sockfd, buffer, MAXLINE, 0);
        if (n <= 0)
        {
            break;
            return -1;
        }
        strcpy(clientContent, buffer);
        bzero(buffer, sizeof(buffer));
        printf("\nDisplay the clientContent: %s\n", clientContent);
        fprintf(fp, "%s", clientContent);
        fclose(fp);
        // sleep(1);

        printf("\n>> File Name: %s", filename);
        printf("\n>> File Size: %ld bytes\n", strlen(clientContent));
        // close(sockfd);
        return 0;
    }
    return 0;
}

int changeDirectory(char *directory)
{
    strtok(directory, "\n");
    if (chdir(directory) == -1)
    {
        if (errno == EACCES)
            return 339;
        else if (errno == ENOTDIR || errno == ENOENT)
            return 340;

        else
            return 341;
    }
    return 0; // if successful
}


int show_currentDirectory()
{
    
    printf("\ninside current direct logic\n");
    // display logic for the current directory
    if (getcwd(pwdbuf1, 256) == NULL)
    {
        printf("Some issue");
        return 342;
    }
    //printf(pwdbuf1);
    strtok(pwdbuf1, "\n");
   // printf(pathValue);
    printf("\nCurrent Directory >> %s\n", pwdbuf1);
  //  return pwdbuf1;
    return 343;
}
// logic to handle the listing of files in a directory
int ListFilesInDirectory()
{

    DIR *dp;
    struct dirent *DIRP;
    *list = '\0';

    if ((dp = opendir("./")) == NULL)
    {
        return 346;
    }

    while ((DIRP = readdir(dp)) != NULL)
    {
        if ((strcmp(DIRP->d_name, ".") != 0) && (strcmp(DIRP->d_name, "..") != 0))
        {
            strcat(list, "--> ");
            strcat(list, DIRP->d_name);
            strcat(list, "\n");
        }
    }

    closedir(dp);

    return 0;
}

// handing logic to make directory
int makeDirectory(char *directory)
{

    /*  Return Type:    Integer
                          return 336: Directory Created
                          return 337: Directory already exists
                          return 338: Directory can't be created
*/

    strtok(directory, "\n"); // to remove trailing '\n'
    int status = stat(directory, &st);

    if (status == -1)
    {
        mkdir(directory, 0700);
        return 336;
    }

    if (status == 0)
        return 337;

    return 338;
}

// retrieve the file from server to client
int retr_file(char *filename)
{

    strtok(filename, "\n");

    char ch;
    FILE *f;

    if ((f = fopen(filename, "r")) == NULL)
    {
        if (errno == ENOENT)
        {
            return 347;
        }
        return 348;
    }

    int k = 0;
    ch = fgetc(f);
    while (ch != EOF)
    {
        file[k] = ch;
        k++;
        ch = fgetc(f);
    }
    file[k] = '\0';

    return 0;
}