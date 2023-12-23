#include "thread_pool.h"
#include <cstdio>

void CTask::setData(void *data)
{
    this->m_ptrData = data;
}

//静态成员类外初始化
vector<CTask*> CThreadPool::m_vecTaskList;
bool CThreadPool::shutdown = false;
pthread_mutex_t CThreadPool::m_pthreadMutex = PTHREAD_MUTEX_INITIALIZER;   
pthread_cond_t CThreadPool::m_pthreadCond = PTHREAD_COND_INITIALIZER;

//线程管理类的构造函数
CThreadPool::CThreadPool(int threadNum)  
{
    this->m_iThreadNum = threadNum;
    printf("I will create %d threads.\n",threadNum);
    Create();
}

void* CThreadPool::ThreadFunc(void *threadData)
{
    pthread_t tid = pthread_self();
    while (1)
    {
        pthread_mutex_lock(&m_pthreadMutex);
        //如果队列为空，等待新任务进入任务队列
        while (m_vecTaskList.size() == 0 && !shutdown)
        {
            pthread_cond_wait(&m_pthreadCond, &m_pthreadMutex);
        }

        //关闭线程
        if (shutdown)
        {
            pthread_mutex_unlock(&m_pthreadMutex);
            printf("[tid: %lu]\text\n", tid);
            pthread_exit(NULL);
        }

        printf("[tid: %lu]\trun:", tid);
        vector<CTask*>::iterator iter = m_vecTaskList.begin();
        //取出一个任务并处理之
        CTask* task = *iter;
        if (iter != m_vecTaskList.end())
        {
            task = *iter;
            m_vecTaskList.erase(iter);
        }

        pthread_mutex_unlock(&m_pthreadMutex);

        task->Run(); //执行任务
        printf("[tid: %lu]\tidle\n", tid);


    }
    return NULL;
}

int CThreadPool::MoveToIdle(pthread_t tid)
{

}

int CThreadPool::MoveToBusy(pthread_t tid)
{

}

int CThreadPool::Create()
{
    pthread_id = new pthread_t[m_iThreadNum];
    for (int i = 0; i < m_iThreadNum; i++)
    {
        pthread_create(&pthread_id[i], NULL, ThreadFunc, NULL);
    }

    return 0;

}


int CThreadPool::AddTask(CTask *task)
{
    pthread_mutex_lock(&m_pthreadMutex);
    m_vecTaskList.push_back(task);
    pthread_mutex_unlock(&m_pthreadMutex);
    pthread_cond_signal(&m_pthreadCond);

    return 0;
}

//停止所有线程
int CThreadPool::StopAll()
{
    //避免重复调用
    if (shutdown)
    return -1;
    printf("Now I will end all threads!\n\n");

    //唤醒所有等待线程，线程池也要销毁了
    shutdown = true;
    pthread_cond_broadcast(&m_pthreadCond);

    //清除僵尸线程
    for (int i = 0; i < m_iThreadNum; i++)
    {
        pthread_join(pthread_id[i], NULL);
    }
    delete [] pthread_id;
    pthread_id = NULL;

    //销毁互斥锁和条件变量
    pthread_mutex_destroy(&m_pthreadMutex);
    pthread_cond_destroy(&m_pthreadCond);

    return 0;
}

int CThreadPool::getTaskSize()
{
    return this->m_vecTaskList.size();
}