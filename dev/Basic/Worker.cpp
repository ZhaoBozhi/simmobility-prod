#include "Worker.hpp"

using std::vector;
using boost::barrier;
using boost::function;



Worker::Worker(function<void(Worker*)>* action, barrier* barr) : barr(barr), action(action)
{
	//this.active = false;
}


void Worker::start()
{
	//this.active = true;
	main_thread = boost::thread(boost::bind(&Worker::barrier_mgmt, this));
}


//Worker::Worker(const Worker& copy) {}
//Worker& Worker::operator=(const Worker& rhs) { return *this; }


void Worker::interrupt()
{
	main_thread.interrupt();
}


void Worker::addEntity(void* entity)
{
	entities.push_back(entity);
}

void Worker::remEntity(void* entity)
{
	vector<void*>::iterator it = std::find(entities.begin(), entities.end(), entity);
	if (it!=entities.end())
		entities.erase(it);
}

vector<void*>& Worker::getEntities() {
	return entities;
}


void Worker::barrier_mgmt()
{
	for (;;) {
		main_loop();

		if (barr!=NULL)
			barr->wait();
	}
}


void Worker::main_loop()
{
	if (action!=NULL)
		(*action)(this);
}





