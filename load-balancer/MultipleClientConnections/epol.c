#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>

#include "epollinterface.h"


void add_epoll_handler(int epoll_fd, struct epoll_event_handler* handler, uint32_t event_mask){
    struct epoll_event event;
    //some data
    event.data.ptr = handler;
    //which type of event you are interested in
    event.events = event_mask;
    //adds fd to this epoll
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, handler->fd, &event) == -1) {
        exit(-1);
    }
}


void do_reactor_loop(int epoll_fd){
    //this isnt a struct I made
    struct epoll_event current_epoll_event;

    while (1) {
        struct epoll_event_handler* handler;
        /*This wait for all the fds which have been registered.
        1) If its an client connection which is incoming, add the clients fd to the list. Then add the backends fd
        // as well by connecting to the backend
        2) If its data from either backend or client, send it to the appropriate client or backend
        This is a blocking call
        */
        epoll_wait(epoll_fd, &current_epoll_event, 1, -1);//waits for an event
        handler = (struct epoll_event_handler*) current_epoll_event.data.ptr;//when you get that event, you pass in the function you want to call as callback
        handler->handle(handler, current_epoll_event.events);//handle is name of function, you call the function
    }

}