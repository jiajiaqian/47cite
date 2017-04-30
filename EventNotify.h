#pragma once


//BOOST系列头文件，线程控制，无锁队列等
#include <boost/thread/thread.hpp>  
#include <boost/lockfree/queue.hpp>  
#include <boost/atomic.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/bind.hpp>
//}}

class EventNotify
{
public:
	EventNotify(void);
	~EventNotify(void);

	boost::mutex mutex;
	boost::condition_variable_any cond;

	void Wait(){
		boost::unique_lock<boost::mutex> lock(mutex);
		cond.wait(mutex);
	}

	void Notify(){
		cond.notify_all();
	}

};

