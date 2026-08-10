#pragma once
#include<queue> 
#include<mutex>
#include<semaphore>
using namespace  std;
template<typename T>
class SharedQueue{
  public:
    explicit SharedQueue(ptrdiff_t capacity):
        free(capacity), filled(0) {}
    void push(T item){
        free.acquire();
        {
            lock_guard<mutex> lock(mtx);
            buffer.push(move(item));
        }
        filled.release();
    }
    T pop(){
        filled.acquire();
        T item;
        {
            lock_guard<mutex> lock(mtx);
            item = move(buffer.front());
            buffer.pop();
        }
        free.release();
        return item;
    }


  private:
    queue<T> buffer;
    mutex mtx;
    counting_semaphore<> free;
    counting_semaphore<> filled; 

};