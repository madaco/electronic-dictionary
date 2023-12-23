#include "thread_pool.h"
#include <unistd.h>

class CMyTask : public CTask{
public:
    CMyTask() = default;
    int Run()
    {
        printf("%s\n", (char *)m_ptrData);
        int x = rand() % 4 + 1;  //1-4
        sleep(x);
        return 0;
    }

    ~CMyTask() {}
};

int main(int argc, char *argv[])
{
    CMyTask taskObj;
    char szTmp[] = "hello!";
    taskObj.setData((void *)szTmp);
    CThreadPool threadpool(5);

    for (int i = 0; i < 10; i++)
    {
        threadpool.AddTask(&taskObj);
    }

    while (1)
    {
        printf("There are still %d tasks need to handle \n", threadpool.getTaskSize());

        //如果任务队列没有任务了
        if(threadpool.getTaskSize() == 0)
        {
            //清除线程池
            if(threadpool.StopAll() == -1)
            {
                printf("Thread pool clear, exit.\n");
                exit(0);
            }
        }

        sleep(2);
        printf("2 seconds later...\n");
    }
    return 0;
}