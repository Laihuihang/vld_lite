/*
 * @Author: Hh Lai 
 * @Date: 2021-03-30 23:54:44 
 * @Last Modified by:   Hh Lai 
 * @Last Modified time: 2021-03-30 23:54:44 
 */
#ifndef VLD_LITE_H_
#define VLD_LITE_H_

#pragma once

#include<stdlib.h>
#include<iostream>
#include<stdio.h>
#include<assert.h>

/*内存块*/
struct Mem_Node
{
    const char* file_;          /*泄露文件名*/
    const char* func_;          /*泄露函数名*/
    unsigned long long line_;      /*泄露行号*/
    unsigned long long size_;      /*泄露大小*/
    Mem_Node* next_;            /*申请的内存以链表形式连接，连接下一块申请的内存*/
};

/*Hash[0]为new申请出来的内存，Hash[1]为new[]申请出来的内存，Hash[2]为malloc申请出来的内存*/
Mem_Node* Hash[3];

void Init()
{
    /*初始化hash表中的指针为空*/
    for(int i=0; i<3; i++)
    {
        Hash[i] = NULL;
    }
}
void Check()
{
    /*检查是否有内存泄露*/
    if(Hash[0] == NULL && Hash[1] == NULL && Hash[2] == NULL)
    {
        std::cout << "No memery leak.";
    }
    for(int i=0; i<3; i++)
    {
        if(Hash[i] == NULL)
        {
            continue;
        }
        else
        {
            Mem_Node* cur = Hash[i];
            while(cur != NULL)
            {
                std::cout << "Memery leak accours!" << std::endl;
                std::cout << "file:" << cur->file_ << " func:" << cur->func_ << " line:" << cur->line_ << "has memery leak. size:" << cur->size_ << "Bytes" << std::endl;
                cur = cur->next_;
            }
        }
    }
}

/*申请的内存为实际需要的内存+MemNode头，之后插入到链表中*/
void* operator new(size_t size, const char* file, const char* func, const unsigned long long line, int flag = 0)
{
    void* result;
    unsigned long long new_size = size + sizeof(Mem_Node);
    Mem_Node* cur = (Mem_Node*)malloc(new_size);
    assert(cur != NULL);
    cur->file_ = file;
    cur->func_ = func;
    cur->line_ = line;
    cur->next_ = NULL;
    cur->size_ = size;

    cur->next_ = Hash[flag];
    Hash[flag] = cur;

    result = cur + 1;
    return result;

}
void* operator new[](size_t size, const char* file, const char* func, const long long line)
{
    return operator new(size, file, func, line, 1);
}

void operator delete(void* ptr, int flag = 0)
{
    if(ptr == NULL || Hash[flag] == NULL)
        return;
    Mem_Node* cur = Hash[flag];
    while(cur->next_ != NULL)
    {
        if(cur->next_ + 1 == ptr)
        {
            Mem_Node* del = cur->next_;
            cur->next_ = del->next_;
            free(del);
            del = NULL;
        }
        else
        {
            cur = cur->next_;
        }
    }

}

void operator delete[](void* ptr)
{
    operator delete(ptr, 1);
}

void* my_malloc(size_t size, const char* file, const char* func, const long long line, int flag = 2)
{
    void* result;
    unsigned long long new_size = size + sizeof(Mem_Node);
    Mem_Node* cur = (Mem_Node*)malloc(new_size);
    assert(cur != NULL);
    cur->file_ = file;
    cur->func_ = func;
    cur->line_ = line;
    cur->next_ = NULL;
    cur->size_ = size;

    cur->next_ = Hash[flag];
    Hash[flag] = cur;

    result = cur + 1;
    return result;
}
void my_free(void* ptr, int flag = 2)
{
    if(ptr == NULL || Hash[flag] == NULL)
        return;
    Mem_Node* cur = Hash[flag];
    while(cur->next_ != NULL)
    {
        if(cur->next_ + 1 == ptr)
        {
            Mem_Node* del = cur->next_;
            cur->next_ = del->next_;
            free(del);
            del = NULL;
        }
        else
        {
            cur = cur->next_;
        }
    }
}

class vld_lite
{
public:
    vld_lite(){Init();}
    ~vld_lite(){Check();}
};


vld_lite vld;

#define new new(__FILE__,__func__,__LINE__)
#define malloc(size) my_malloc(size,__FILE__,__func__,__LINE__)
#define free(ptr) my_free(ptr)









#endif 