#pragma once

#include <array>
#include <functional>
#include <mutex>
#include <condition_variable>


template<int SIZE>
struct BufferHolder
{
   std::array<Buffer, SIZE> buffers;
   std::array<int, SIZE> numbers;

   int head;
   int tail;

   std::mutex mtx_buffers;

   std::condition_variable cv;
   std::mutex mtx_cv;

   BufferHolder()
   {
      head = 0;
      tail = SIZE - 1;
      int i;
      for(i = 0; i < SIZE - 1;)
      {
         numbers[i++] = i;
      }
      numbers[i] = -1;
   }

   int pop()
   {
      std::lock_guard<std::mutex> lock(mtx_buffers);
      if(head == -1)
      {
         printf("No more elements to pop\n");
         return head;
      }
      int h = head;
      head = numbers[head];
      if(head == -1) tail = -1;
      numbers[h] = -1;

      return h;
   }

   void push(int n)
   {
      std::lock_guard<std::mutex> lock(mtx_buffers);
      if(numbers[n] != -1 || n == tail)
      {
         printf("Can't push an element which is not loose\n");
         return;
      }
      buffers[n].reset();
      if(head == -1)
      {
         head = n;
         tail = n;
         cv.notify_one();
         return;
      }
      numbers[tail] = n;
      tail = n;
   }

   void use_buffer(std::function<void(Buffer&)> callback)
   {
      int current;
      {
         std::unique_lock<std::mutex> lk(mtx_cv);
         cv.wait(lk, [&]{
            current = pop();
            return current != -1;
         });
      }
      callback(buffers[current]);
      push(current);
   }

   void print()
   {
      for(int i = 0; i < SIZE; i++)
         printf("%d ", numbers[i]);
      printf("\n");
      printf("Head is %d, tail is %d\n\n", head, tail);
   }
};
