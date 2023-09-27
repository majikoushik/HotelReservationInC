/**
  @file common.h  
  This header file holds the definitions for server queue and client queue
  and holds the declarations of all the magic numbers
**/

// Name for the queue of messages going to the server.
#define SERVER_QUEUE "/hotel-server-queue"

// Name for the queue of messages going to the current client.
#define CLIENT_QUEUE "/hotel-client-queue"

// Maximum length for a message in the queue
// (Long enough to hold any server request or response)
#define MESSAGE_LIMIT 1024



// Maximum length of the floor name.
#define FLOORNAME_MAX 10

// For registration manager indexing at position 2
#define TWO 2

// For registration manager indexing at position 3
#define THREE 3

// For registration manager indexing at position 4
#define FOUR 4

// For registration manager indexing at position 5
#define FIVE 5

// For registration manager indexing at position 6
#define SIX 6

// For registration manager indexing at position 7
#define SEVEN 7

// For registration manager indexing at position 8
#define EIGHT 8

// For registration manager indexing at position 9
#define NINE 9

// For registration manager indexing at position 10
#define TEN 10

// For registration manager indexing at position 11
#define ELEVEN 11

// For registration manager indexing at position 12
#define TWELVE 12

// For registration manager indexing at position 13
#define THIRTEEN 13

// For registration manager indexing at position 14
#define FOURTEEN 14

// For registration manager indexing at position 15
#define FIFTEEN 15

// For registration manager indexing at position 16
#define SIXTEEN 16

// For registration manager indexing at position 17
#define SEVENTEEN 17

// For registration manager indexing at position 18
#define EIGHTEEN 18

// For registration manager indexing at position 19
#define NINETEEN 19

// For registration manager indexing at position 20
#define TWENTY 20

// For registration manager indexing at position 21
#define TWENTYONE 21

// For registration manager indexing at position 22
#define TWENTYTWO 22

// For registration manager indexing at position 23
#define TWENTYTHREE 23

#define RESERVATION_COMMAND "reserve"
#define UNRESERVATION_COMMAND "free"
#define SHOW_STATUS_COMMAND "show"
