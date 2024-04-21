#pragma once
#include "Header_Files.h"

using namespace std;

namespace olc
{
    namespace net
    {
        template<typename T>
        class TSQueue{
            protected:
            mutex muxQueue;
            queue<T> TSQ;

            public:
            TSQueue()=default;
            TSQueue(const TSQueue<T>&)=delete;
            virtual ~TSQueue(){clear();}

            public:
            //Return and maintains item at fornt of Queue
            const T& get(){
                scoped_lock lock(muxQueue);
                return TSQ.front();
            }

            // Adds an item to back of Queue
            void push(const T& item){
                scoped_lock lock(muxQueue);
                TSQ.emplace(move(item));
            }

            // Return and remove item from front of queue
            void pop(){
                scoped_lock lock(muxQueue);
                auto t = move(TSQ.front());
                TSQ.pop();
            }

            // Return if Queue has no item
            bool empty(){
                scoped_lock lock(muxQueue);
                return TSQ.empty();
            }

            // Return number of items in Queue
            size_t count(){
                scoped_lock lock(muxQueue);
                return TSQ.size();
            }

            // Clear Queue
            void clear(){
                scoped_lock lock(muxQueue);
                while(!TSQ.empty()){
                    TSQ.pop();
                }
            }
        };
    }
}