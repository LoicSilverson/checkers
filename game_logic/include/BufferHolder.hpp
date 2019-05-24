#pragma once

#include <array>
#include <functional>
#include <mutex>
#include <condition_variable>


template<int SIZE>
struct BufferHolder
{
   std::array<Buffer, SIZE> Buffers;
   std::array<int, SIZE> numbers;

   int head;
   int tail;

   std::mutex mtx;
   std::condition_variable cv;

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
      printf("Poppin. . .");
      std::lock_guard<std::mutex> lock(mtx);
      if(head == -1)
      {
         printf("No more elements to pop\n");
         return head;
      }
      int h = head;
      head = numbers[head];
      if(head == -1) tail = -1;
      numbers[h] = -1;

      printf("%d\n", h);
      return h;
   }
   void push(int n)
   {
      printf("Pushin %d\n", n);
      std::lock_guard<std::mutex> lock(mtx);
      if(numbers[n] != -1 || n == tail)
      {
         printf("Can't push an element which is not loose\n");
         return;
      }
      if(head == -1)
      {
         head = n;
         tail = n;
         return;
      }
      numbers[tail] = n;
      tail = n;
   }

   void fill(std::function<void(Buffer&)> callback)
   {
      int current = pop();
      if(current == -1) return ;
   }
   void print()
   {
      for(int i = 0; i < SIZE; i++)
         printf("%d ", numbers[i]);
      printf("\n");
      printf("Head is %d, tail is %d\n\n", head, tail);
   }
};
