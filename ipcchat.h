#ifndef IPCCHAT_IPCCHAT_H
#define IPCCHAT_IPCCHAT_H

// maximum number of users
#define MAX_USERS 32

// maximum number of groups (user can be connected to all groups at once)
#define MAX_GROUPS 32

// It's possible (even easier) to keep users and groups in a list instead of an array.
// When selecting an array, you must follow the above two constants.

// maximum length of user/channel name
#define MAX_NAME_LENGTH 32

// maximum message length
#define MAX_MESSAGE_LENGTH 2048


/*

# Data exchange

The server creates a queue and prints its ID so that the client knows where to connect.
The client also creates its own queue and immediately after opening the server's queue, it sends the login command with its queue_id there.
After logging out, the client does not turn off itself, but waits for a goodbye message (type 3).

Client -> server communication via server queue, command structure
Server -> client communication via the client queue, message structure


# List of commands

COMMAND                 MTYPE   DESCRIPTION                 NOTES
login [queue_id]        2       login                       should be the first command sent after connecting the client
logout                  1       logout                      must be responded with a message with mtype = 3 to disable the client

join [room_name]        1       join a room
leave [room_name]       1       leave the room
rooms                   1       display rooms               the server should reply with a message (server as sender)
users                   1       display users               as above.
help                    1       display available commands  as above.

@[nickname] [content]   1       private message             logins do not contain whitespace, there are no spaces after @!
#[room_name] [content]  1       message to the room         as above.
* [content]             1       message to everyone

*/


// structure of commands sent by the client to the server
// (we also understand sending a message as a command)
struct command {
    long mtype; // depending on the command type, described above

    char data[2*MAX_MESSAGE_LENGTH]; // command content
    char username[MAX_NAME_LENGTH]; // name of the user sending the command
};

// structure of messages sent from the server to the client
struct message {
    long mtype;

    /*  1 for messages from users
        2 for messages from the server
                (e.g. replies to rooms, users, help; possible welcome message)
        3 special message from the server, upon receipt of which the client should terminate
                (e.g. message about connection rejection due to exceeded number of users,
                used nickname, farewell message after logging out or when the server received a closing signal)
    */

    char from[MAX_NAME_LENGTH]; //name of the user who sent the message
    char to_symbol; // @ - private message, # - message to the channel, * - message to everyone
    char to[MAX_NAME_LENGTH];  // user/channel name or empty if to_symbol = *
    char message[MAX_MESSAGE_LENGTH]; // Message content
};

#endif //IPCCHAT_IPCCHAT_H