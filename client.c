/**
 * @file client.c
 * This program is the client which is responsible for sending commands to
 * the sever to get reservation information for a given floor, to reserve for
 * a given floor and room, or to cancel a reservation for a particular time.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <mqueue.h>
#include "common.h"
#include <signal.h>

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

/**
 * This struct is used for storing the user commands.
 */
struct Message
{
  char action[10];
  char floor[10];
  int room;
};

/**
 * Responsible for creating message queues, processing command line arguments,
 * sending request and receiving information from server, and closing
 * the queues.
 * @param argc refers to the number of arguments
 * @param argv refers to the list of command line arguments
 * @return 0 on exit success
 */
int main(int argc, char *argv[])
{
  // This is used for creating a structure that specifies the
  // attributes for the message queue.
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 1;
  attr.mq_msgsize = MESSAGE_LIMIT;

  // Creates a new message queue for writing based on the attribute structure
  mqd_t serverQueue = mq_open(SERVER_QUEUE, O_WRONLY | O_CREAT, 0600, &attr);

  // Creates a new message queue for reading based on the attribute structure
  mqd_t clientQueue = mq_open(CLIENT_QUEUE, O_RDONLY | O_CREAT, 0600, &attr);

  // Check if queue cannot be opened
  if (serverQueue == -1 || clientQueue == -1)
    fail("Can't open message queue");

  // Check for the right number of user input or arguments
  if (argc > 4 || argc < 2)
  {
    fprintf(stderr, "usage: server <reservation-file>");
    exit(1);
  }

  // This is responsible for declaring a variable request of the type struct message
  struct Message request;

  // copies the contents of first command line argument into the member called action
  strcpy(request.action, argv[1]);

  // copies the contents of second command line argument into the member called floor
  strcpy(request.floor, argv[2]);

  // If there are additional arguments, consider it as the room index and store into the member
  // called room, if not just set room to 0
  if (argc > 3)
  {
    request.room = atoi(argv[3]);
  }
  else
  {
    request.room = 0;
  }

  // Responsible for sending the request struct to the server queue
  mq_send(serverQueue, (char *)&request, sizeof(request), 0);

  char receive[MESSAGE_LIMIT + 1];
  // Store the contents of the recieved message into receive
  mq_receive(clientQueue, (char *)&receive, MESSAGE_LIMIT, 0);

  printf("%s", receive);
  printf("\n");

  // close copy of the queue
  mq_close(clientQueue);
  mq_close(serverQueue);
  return 0;
}
