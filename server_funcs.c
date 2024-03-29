#include "server_funcs.h"

bool send(int queue, struct message msg) {
    size_t msg_size = sizeof(msg) - sizeof(msg.mtype);

    if (msgsnd(queue, &msg, msg_size, 0) == -1) {
        perror("Failed to send a message");
        return false;
    }
    return true;
}

int broadcast(struct message msg) {
    return broadcast_excluding(msg, NULL);
}

int broadcast_excluding(struct message msg, char exclude[]) {
    int sent = 0;

    if (msg.to_symbol == '#') {
        struct room *room;
        room = find_room(msg.to, NULL);

        if (room == NULL) {
            return 0;
        }

        struct room_member *member;
        for (member = room->members; member != NULL; member = member->next) {
            if (exclude == NULL || strcmp(member->member->username, exclude) != 0) {
                if (send(member->member->q, msg)) {
                    sent++;
                } else {
                    remove_user(member->member->username);
                }
            }
        }
    } else {
        struct user *user;
        for (user = connected_users; user != NULL; user = user->next) {
            if (exclude == NULL || strcmp(user->username, exclude) != 0) {
                if (match_user(user, msg.to_symbol, msg.to) || strcmp(msg.from, user->username) == 0) {
                    if (send(user->q, msg)) {
                        sent++;
                    } else {
                        remove_user(user->username);
                    }
                }
            }
        }
    }

    return sent;
}

struct message server_message(long mtype, char to_symbol, char *to, char *format, ...) {
    struct message msg;
    msg.mtype = mtype;
    strncpy(msg.from, "server", MAX_NAME_LENGTH);
    msg.to_symbol = to_symbol;
    strncpy(msg.to, to, MAX_NAME_LENGTH);

    va_list argptr;
    va_start(argptr, format);
    vsnprintf(msg.message, MAX_MESSAGE_LENGTH, format, argptr);
    va_end(argptr);

    return msg;
}