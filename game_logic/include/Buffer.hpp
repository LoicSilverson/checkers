#pragma once
//#define BUFLEN 512

#include <string.h>

#include <stdio.h>

template<int SIZE>
struct Buffer
{
   char m_data[SIZE];
   int m_size;

   void reset()
   {
      memset(m_data, '\0', SIZE);
      m_size = 0;
   }

   template<typename T>
   bool write(T data)
   {
      int new_size = m_size + sizeof(T);
      if(new_size > SIZE) return false;
      memcpy(&m_data[m_size], &data, sizeof(T));
      m_size = new_size;
      return true;
   }

   template<typename T>
   T read()
   {
      int new_size = m_size + sizeof(T);
      if(new_size > SIZE) return -1;
      T data;
      memcpy(&data, &m_data[m_size], sizeof(T));
      m_size = new_size;
      return data;
   }

   bool write_char(char data)
   {
      int new_size = m_size + sizeof(char);
      if(new_size > SIZE) return false;
      memcpy(&m_data[m_size], &data, sizeof(char));
      m_size = new_size;
      return true;
   }

   bool write_int(int data)
   {
      int new_size = m_size + sizeof(int);
      if(new_size > SIZE) return false;
      memcpy(&m_data[m_size], &data, sizeof(int));
      m_size = new_size;
      return true;
   }

   bool write_ulong(unsigned long data)
   {
      int new_size = m_size + sizeof(unsigned long);
      if(new_size > SIZE) return false;
      memcpy(&m_data[m_size], &data, sizeof(unsigned long));
      m_size = new_size;
      return true;
   }

   bool write_c_string(const char * data)
   {
      size_t len = strlen(data);
      int new_size = m_size + len;
      if(new_size > SIZE) return false;
      strcpy(&m_data[m_size], data);
      m_size = new_size;
      memset(&m_data[m_size++], '\0', 1);
      return true;
   }

   int read_int()
   {
      int new_size = m_size + sizeof(int);
      if(new_size > SIZE) return -1;
      int data;
      memcpy(&data, &m_data[m_size], sizeof(int));
      m_size = new_size;
      return data;
   }

   unsigned long read_ulong()
   {
      int new_size = m_size + sizeof(unsigned long);
      if(new_size > SIZE) return -1;
      unsigned long data;
      memcpy(&data, &m_data[m_size], sizeof(unsigned long));
      m_size = new_size;
      return data;
   }

   char * read_c_string()
   {
      size_t len = strlen(&m_data[m_size])+1;
      int new_size = m_size + len;
      if(new_size > SIZE) return nullptr;
      char * data = &m_data[m_size];
      m_size = new_size;
      return data;
   }
};
