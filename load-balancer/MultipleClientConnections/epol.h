struct epoll_event_handler {
    int fd;//fd associated with the handler
    void (*handle)(struct epoll_event_handler*, uint32_t);// the function which is called in an epoll event
    void* closure;//any data needed for the callback
};

extern void add_epoll_handler(int epoll_fd, struct epoll_event_handler* handler, uint32_t event_mask);

extern void do_reactor_loop(int epoll_fd);