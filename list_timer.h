//
// Created by s on 19-5-13.
//

#ifndef _LIST_TIMER_H_
#define _LIST_TIMER_H_

#include "myutili.h"

/*升序定时器链表*/
#define BUFFER_SIZE 64

class util_timer; /*前向声明*/

/*用户数据结构：客户端socket地址、socket文件描述符、读缓存和定时器*/
struct client_data {
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    util_timer *timer;
};

/*定时器类*/
class util_timer {
 public:
  util_timer () : prev (nullptr), next (nullptr)
  {}
 public:
  time_t expire;  /*任务超时时间，这里使用绝对时间*/
  void (*cb_func (client_data *));  /*任务回调函数*/

  /*回调函数处理的客户数据，由定时器的执行者传递给回调函数*/
  client_data *uer_data;
  util_timer *prev;
  util_timer *next;
};

/*定时器链表。一个升序、双向链表，带有头结点和尾结点*/
class sort_timer_lst {
 public:
  sort_timer_lst () : head (nullptr), tail (nullptr)
  {}
  ~sort_timer_lst ()
  {
    util_timer *temp = head;
    while (temp)
      {
        head = temp->next;
        delete temp;
        temp = head;
      }
  }

  /*将目标定时器timer添加到链表中*/
  void add_timer (util_timer *timer)
  {
    if (timer == nullptr)
      {
        return;
      }

    if (head == nullptr)
      {
        head = tail = timer;
        return;
      }

    /*如果目标定时器的超时时间小于当前链表中所有定时器的超时时间，则把该定时器
     * 插入链表头部，作为链表新的头结点，否则就需要调用重载函数add_timer(util_timer* timer, util_timer* lst_head),
     * 把它插入链表中合适的位置，以保证链表的升序特性。*/
    if (timer->expire < head->expire)
      {
        timer->next = head;
        head->prev = timer;
        head = timer;
        return;
      }
    add_timer (timer, head);
  }

  /*当某个定时任务发生变化时，调整对应的定时器在链表中的位置，这个函数只考虑被调整的定时器的超时时间延长的情况，
   * 即该定时器需要往链表的尾部移动*/
  void adjust_timer (util_timer *timer)
  {
    if (timer == nullptr)
      {
        return;
      }
    util_timer *temp = timer->next;

    /*如果被调整的目标定时器处在链表的尾部，或者该定时器新的超时值仍然小于下一个定时器的超时值，则不用调整*/
    if (temp == nullptr || (timer->expire < temp->expire))
      {
        return;
      }

    /*如果目标定时器是链表的头结点，则将该定时器从链表中取出并重新插入链表*/
    if (timer == head)
      {
        head = head->next;
        head->prev = nullptr;
        timer->next = nullptr;
        add_timer (timer, head);
      }
      /*
       * 如果目标定时器不是链表的头结点，则将该定时器从链表中取出然后插入其原来所在位置之后的
       * 部分链表中
       */
    else
      {
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        add_timer (timer, timer->next);
      }
  }

  /*将目标定时器从链表中删除*/
  void del_timer (util_timer *timer)
  {
    /*如果传入为空*/
    if (timer == nullptr)
      {
        return;
      }

    /*如果链表中只有一个定时器，即目标定时器*/
    if ((timer == head) && (timer == tail))
      {
        delete timer;
        head = nullptr;
        tail = nullptr;
        return;
      }

    if (timer == head)
      {
        head = head->next;
        head->prev = nullptr;
        delete timer;
        return;
      }

    if (timer == tail)
      {
        tail = tail->prev;
        tail->next = nullptr;
        delete timer;
        return;
      }

    /*
     * 如果目标定时器位于链表的中间，则将它前后的定时器串接起来，然后删除目标定时器
     */
    timer->prev->next = timer->next;
    timer->next->prev = timer->prev;
    delete timer;
  }

  /*
   * SIGALRM信号每次被出发就在其信号处理函数中（如果使用统一事件源，则是主函数）中执行一次tick函数
   * 以处理链表上到期的任务
   */
  void tick ()
  {
    if (head == nullptr)
      {
        return;
      }

    printf ("timer tick\n");
    time_t cur = time (nullptr);  /*获取系统当前事件*/
    util_timer *temp = head;

    /*从头结点开始依次处理每个定时器，直到遇到一个尚未到期的定时器，这就是定时器的核心逻辑*/
    while (temp)
      {
        /*
         * 因为每个定时器都是用绝对时间作为超时值，所为我们可以把定时器的超时值和
         * 系统的当前时间，比较以判断定时器是否到期
         */
        if (cur < temp->expire)
          {
            break;
          }

          /*调用定时器的回调函数，以执行定时任务*/
        temp->cb_func (temp->uer_data);
        /*执行完定时器中的定时任务之后，就将它从链表中删除，并重置链表头结点*/
        head = temp->next;
        if (head != nullptr)
          {
            head->prev = nullptr;
          }
        delete temp;
        temp = head;
      }
  }
 private:
  void add_timer (util_timer *timer, util_timer *lst_head)
  {
    util_timer *prev = lst_head;
    util_timer *temp = prev->next;

    while (temp)
      {
        /*遍历lst_head结点之后的部分链表，直到找到一个超时时间大于目标定时器的超时时间的结点，并将目标定时器插入该结点之前*/
        if (timer->expire < temp->expire)
          {
            prev->next = timer;
            timer->next = temp;
            temp->prev = timer;
            timer->prev = prev;
            break;
          }
        prev = temp;
        temp = temp->next;
      }

    /*如果遍历完lst_head结点之后的部分链表，仍未找到超时时间大于目标定时器的超时时间的结点，则将
     * 目标定时器插入链表尾部，并把它设置为链表的尾结点*/
    if (temp == nullptr)
      {
        prev->next = timer;
        timer->prev = prev;
        timer->next = nullptr;
        tail = timer;
      }
  }
  util_timer *head;
  util_timer *tail;
};

#endif //_LIST_TIMER_H_
