/**
 * @file server.c
 * This program is the server and it reads the registration.txt file and store the
 * hotel reservation data in memory.
 */

#include "common.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

/** Define the number of rooms in each floor*/
#define ROOMCOUNT 24

// Maximum number of rows
#define ROW_MAX 7

/** Defines the string for printing the output or send the output string to client*/
#define LENGTH_OF_OUTPUT 100


/**
 * This struct is used for storing the user commands.
 */
struct Message
{
  char action[10];
  char floor[10];
  int room;
};

/** This is a 3d array for the 7 floor and 24 rooms and the third dimension states that each of these contain 1 character **/
char reservations[ROW_MAX][ROOMCOUNT][1];

/** An array of pointers to maintain the floor names **/
const char *floors[] = {"1stFloor", "2ndFloor", "3rdFloor", "4thFloor", "5thFloor", "6thFloor", "7thFloor"};

/**
 * This function is responsible for printing an error message in the cases
 * of error handling and then exits unsuccessfully.
 * @param message refers to the message to be printed before exiting
 */
static void fail(char const *message)
{
  fprintf(stderr, "%s\n", message);
  exit(1);
}
/** Refers to the initial capacity of string */
#define INITIAL_CAPACITY 5

/** Used to double the capacity to enlarge the array */
#define DOUBLE_CAPACITY 2

char *readLine(FILE *fp)
{
  char ch = EOF;
  int len = 0;
  int capacity = INITIAL_CAPACITY;
  char *str = (char *)malloc(capacity * sizeof(char));

  while ((ch = fgetc(fp)) != '\n')
  {
    if (ch == EOF)
    {
      free(str);
      return NULL;
    }

    if (len >= capacity)
    {
      capacity *= DOUBLE_CAPACITY;
      str = (char *)realloc(str, capacity * sizeof(char));
    }

    str[len] = ch;
    len++;

    if (len >= capacity)
    {
      capacity *= DOUBLE_CAPACITY;
      str = (char *)realloc(str, capacity * sizeof(char));
    }
  }
  str[len] = '\0';
  return str;
}

// Flag for telling the server to stop running because of a sigint.
// This is safer than trying to print in the signal handler.
static int running = 1;

/** Pointer to a file that is later opened */
FILE *fp;

/** Responsible for stopping the while loop once control c is clicked
 * and then prints out the whole text file with floors and their reservation
 * information
 * @param sig refers to the signal it received
 */
void alarmHandler(int sig)
{
  // stops the loop
  running = 0;
  printf("\n");
  // iterate over the floor of the hotel
  for (int i = 0; i < ROW_MAX; i++)
  {
    int length = 0;
    // used for output string
    char eachRowStr[LENGTH_OF_OUTPUT];
    // copy floors into eachRowStr array at that index
    for (int j = 0; floors[i][j] != '\0'; ++j, ++length)
    {
      eachRowStr[length] = floors[i][j];
    }
    // format by adding spaces between each char
    eachRowStr[length++] = ' ';

    // copy the registration data into the eachRowStr array
    for (int j = 0; j < ROOMCOUNT; j++, ++length)
    {
      eachRowStr[length] = reservations[i][j][0];
      eachRowStr[++length] = ' ';
    }
    // null terminate the string
    eachRowStr[length] = '\0';
    // print output string to stdout
    printf("%s", eachRowStr);
    printf("\n");
  }
  printf("\n");
  exit(0);
}

/**
 * Responsible for opening the file, reading file line by line, parsing contents of the line,
 * opening message queues, process client message, handle errors, and close file pointer
 * and message queues.
 * @param argc refers to the number of arguments
 * @param argv refers to the list of command line arguments
 * @return 0 on exit success
 */
int main(int argc, char *argv[])
{
  // This is used for creating a structure that specifies the
  // attributes for the message queue.
  struct sigaction act;
  act.sa_handler = alarmHandler;
  act.sa_flags = 0;
  sigaction(SIGINT, &act, NULL);

  // Check for the right number of user input or arguments
  if (argc > FOUR || argc < TWO)
  {
    fprintf(stderr, "usage: server <reservation-file>\n");
    fprintf(stderr, "\n");
    exit(1);
  }

  // open the file for reading purposes
  fp = fopen(argv[1], "r");
  // if cannot be opened, send to stderr
  if (!fp)
  {
    fprintf(stderr, "Invalid input file: %s\n", argv[1]);
    fprintf(stderr, "\n");
    exit(1);
  }

  // Read from a file line by line and store the result in readFile
  char *readFile;
  readFile = readLine(fp);

  int roomCount = 0;

  // After reading from a file use sscanf to parse the contents
  // Make sure that ther are additional lines to read. Read the first value
  // as floor name, the rest 24 are stored in the reservations array from 0 to 23
  while ((readFile != NULL) && (*readFile != '\0'))
  {
    char floorName[TEN];
    sscanf(readFile, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s", floorName, reservations[roomCount][0], reservations[roomCount][1],
           reservations[roomCount][TWO], reservations[roomCount][THREE], reservations[roomCount][FOUR], reservations[roomCount][FIVE], reservations[roomCount][SIX], reservations[roomCount][SEVEN],
           reservations[roomCount][EIGHT], reservations[roomCount][NINE], reservations[roomCount][TEN], reservations[roomCount][ELEVEN], reservations[roomCount][TWELVE], reservations[roomCount][THIRTEEN], reservations[roomCount][FOURTEEN],
           reservations[roomCount][FIFTEEN], reservations[roomCount][SIXTEEN], reservations[roomCount][SEVENTEEN], reservations[roomCount][EIGHTEEN], reservations[roomCount][NINETEEN], reservations[roomCount][TWENTY],
           reservations[roomCount][TWENTYONE], reservations[roomCount][TWENTYTWO], reservations[roomCount][TWENTYTHREE]);

    roomCount++;

    free(readFile);
    readFile = readLine(fp);
  }
  mq_unlink(SERVER_QUEUE);
  mq_unlink(CLIENT_QUEUE);

  // Prepare structure indicating maximum queue and message sizes.
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 1;
  attr.mq_msgsize = MESSAGE_LIMIT;

  // Make both the server and client message queues.
  mqd_t serverQueue = mq_open(SERVER_QUEUE, O_RDONLY | O_CREAT, 0600, &attr);
  mqd_t clientQueue = mq_open(CLIENT_QUEUE, O_WRONLY | O_CREAT, 0600, &attr);
  if (serverQueue == -1 || clientQueue == -1)
    fail("Can't create the needed message queues");

  // Repeatedly read and process client messages.
  while (running)
  {
    // This is responsible for declaring a variable request of the type struct message
    struct Message request;

    // Receive message for server and store it in request
    mq_receive(serverQueue, (char *)&request, sizeof(struct Message) * MESSAGE_LIMIT, NULL);

    char success[] = "success";
    char failure[] = "error";

    int sizeFloors = sizeof(floors) / sizeof(floors[0]);

    // process when the query is to reserve a room
    if (strncmp(request.action, RESERVATION_COMMAND, strlen(RESERVATION_COMMAND)) == 0)
    {
      // check for valid room number
      if (request.floor != NULL && request.room >= 0 && request.room <= ROOMCOUNT - 1)
      {
        bool invalidFloorName = true;
        int selectedRoomIndex = 0;
        // The floor name had to be one of the floors mentioned in the floors array
        for (int i = 0; i < sizeFloors; i++)
        {
          if (strcmp(request.day, floors[i]) == 0)
          {
            invalidFloorName = false;
            selectedRoomIndex = i;
          }
        }
        // if floor name is not valid send error to client
        if (invalidFloorName)
        {
          int len = mq_send(clientQueue, failure, strlen(failure), 0);
          if (len == -1)
          {
            perror("mq_send");
            exit(1);
          }
        }

        char selectedRoom = reservations[selectedRoomIndex][request.hour][0];
        // convert character 0 to integer
        int currentCount = selectedRoom - '0';
        // if reservation count is one then the room is occupied and cannot be reserve
        if (currentCount == ONE)
        {
          int len = mq_send(clientQueue, failure, strlen(failure), 0);
          if (len == -1)
          {
            perror("mq_send");
            exit(1);
          }
        }
        // else increment count, update the array to store the currentCount
        // and then send information to the client as success
        else
        {
          currentCount++;
          reservations[selectedRoomIndex][request.hour][0] = currentCount + '0';
          int len = mq_send(clientQueue, success, strlen(success), 0);
          if (len == -1)
          {
            perror("mq_send");
            exit(1);
          }
        }
      }
      // if not a valid floor name and room index argument, send to client as failure
      else
      {
        int len = mq_send(clientQueue, failure, strlen(failure), 0);
        if (len == -1)
        {
          perror("mq_send");
          exit(1);
        }
      }
    }

    // process when the query is to cancel
    else if (strncmp(request.action, UNRESERVATION_COMMAND strlen(UNRESERVATION_COMMAND)) == 0)
    {
      // check for valid room number
      if (request.day != NULL && request.hour >= 0 && request.hour <= ROOMCOUNT - 1)
      {
        bool invalidFloorName = true;
        int selectedRoomIndex = 0;
        for (int i = 0; i < sizeFloors; i++)
        {
          if (strcmp(request.day, floors[i]) == 0)
          {
            invalidFloorName = false;
            selectedRoomIndex = i;
          }
        }
        // if floor name is not valid send error to client
        if (invalidFloorName)
        {

          int len = mq_send(clientQueue, failure, strlen(failure), 0);
          if (len == -1)
          {
            perror("mq_send");
            exit(1);
          }
        }

        char selectedRoom = reservations[selectedRoomIndex][request.hour][0];
        // convert character 0 to integer
        int currentCount = selectedRoom - '0';
        // can't cancel because room is not reserved
        if (currentCount == 0)
        {
          int len = mq_send(clientQueue, failure, strlen(failure), 0);
          if (len == -1)
          {
            perror("mq_send");
            exit(1);
          }
        }
        // else decrement count, update the array to store the currentCount
        // and then send information to the client as success
        else
        {
          currentCount--;
          reservations[selectedRoomIndex][request.hour][0] = currentCount + '0'; // int to char          
          int len = mq_send(clientQueue, success, strlen(success), 0);
          if (len == -1)
          {
            perror("mq_send");
            exit(1);
          }
        }
      }
    }

    // process when the query is to query
    else if (strncmp(request.action, SHOW_STATUS_COMMAND, strlen(SHOW_STATUS_COMMAND)) == 0)
    {
      bool invalidFloorName = true;
      for (int i = 0; i < sizeFloors; i++)
      {
        // If the input floor name is found then send the details to client
        if (strncmp(request.day, floors[i], strlen(request.day)) == 0)
        {
          int length = 0;
          invalidFloorName = false;
          char rowStr[LENGTH_OF_OUTPUT];
          for (int j = 0; floors[i][j] != '\0'; ++j, ++length)
          {
            rowStr[length] = floors[i][j];
          }
          rowStr[length++] = ' ';
          for (int j = 0; j < ROOMCOUNT; j++, ++length)
          {
            rowStr[length] = reservations[i][j][0];
            rowStr[++length] = ' ';
          }
          rowStr[length] = '\0';
          
          int len = mq_send(clientQueue, rowStr, sizeof(rowStr), 0);
          if (len == -1)
          {
            perror("mq_send");
            exit(1);
          }
        }
      }
      if (invalidFloorName)
      {       
        int len = mq_send(clientQueue, failure, strlen(failure), 0);
        if (len == -1)
        {
          perror("mq_send");
          exit(1);
        }
      }
    }
    // the action does not match any of the above strings
    else
    {      
      int len = mq_send(clientQueue, failure, strlen(failure), 0);
      if (len == -1)
      {
        perror("mq_send");
        exit(1);
      }
    }

  }

  // close file pointer
  fclose(fp);

  // Close our two message queues (and delete them).
  mq_close(clientQueue);
  mq_close(serverQueue);

  // Remove both queues, in case, last time, this program terminated
  // abnormally with some queued messages still queued.
  mq_unlink(SERVER_QUEUE);
  mq_unlink(CLIENT_QUEUE);

  return 0;
}
