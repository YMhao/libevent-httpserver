
#include "httpServer.h"

int main() {
    HttpServer s;
    s.servInit("192.168.1.13",8888, 10, 3);
}
