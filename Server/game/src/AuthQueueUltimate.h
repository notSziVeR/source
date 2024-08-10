/*
	Sherer & Evor 2017 (C)
	Ultimate Biedronka Queue System
	*	Multithread has been implemented for better performance

	Sample:
			count = CLoginQueue::instance().PassQueue();
			CLoginQueue::instance().CleanUp();
			(into main.cpp)
*/

#ifndef __QUEUE_ULTIMATE_H__
#define __QUEUE_ULTIMATE_H__
#include <thread>
#include <memory>
#include "login_sim.h"

#define MAX_CLIENT_NUM 500 // assign some normal value.. Otherwise say bye bye to your processor.

static void Queue_Move(std::queue<CLoginSim *> & queue)
{
	while (!queue.empty())
	{
		CLoginSim * cur = queue.front(); queue.pop();

		if (cur->IsCheck())
		{
			delete cur;
			continue;
		}

		cur->SendLogin();
		delete cur;
	}
}
	
class CLoginQueue : public singleton<CLoginQueue>
{
	public:
		CLoginQueue()
		{
			CleanUp();
		}

		virtual ~CLoginQueue()
		{
			CleanUp();
		}

		void AddNewClient(CLoginSim * ld)
		{
			for (auto && obj : Queue_Container)
			{
				// Checking if queue is empty
				if (obj.size() >= MAX_CLIENT_NUM)
					continue;
				else
				{
					obj.push(ld);
					return;
				}
			}

			std::queue<CLoginSim *> new_queue;
			new_queue.push(ld);
			Queue_Container.push_back(new_queue);
		}

		DWORD PassQueue()
		{
			// Unique ptr for storing threads objects
			std::vector< std::unique_ptr<std::thread> > threads;
			for (BYTE i = 0; i < Queue_Container.size(); ++i)
				threads.push_back(std::unique_ptr<std::thread> (new std::thread(Queue_Move, std::ref(Queue_Container[i]))));

			for (auto && obj : threads)
				// Locking threads
				(obj.get())->join();

			return MAX(all_client, 100);
		}

		void CleanUp()
		{
			// Clear Function
			Queue_Container.clear();
			all_client = 0;
		}

	private:
		std::vector< std::queue<CLoginSim *> > Queue_Container;
		DWORD all_client;
};
#endif

