A multi-thread httpserver framework implemented through libevent2.0.22-stable.
基于libevent2.0.22的http服务简单框架。

Build:
	cmake .
	make

说明:
	libevent http 多线程有许多的坑，文档也少。
	重新阅读 libevent2.0.22-stable.tar.gz 源码后，写了这个多线程event_base + evhttp + threadpool 框架.
	自己设计的线程池，任务均衡分配，当然也可以用无锁队列的方法做，后续再添加无所队列的方法。



