#include "./include/stdinclude.h"
#include "./sharemem/context.h"
#include "./systemserver/server.h"
#include "./systemserver/dbcjserver.h"
#include "./systemserver/httxserver.h"
#include "./systemserver/pppserver.h"

int main(int argc, char *argv[])
{
	pid_t result;
	vector<server*> serverList;
	vector<server*>::iterator st;

	/* ���������ļ�*/
	Context::Instance();
	
  	/* ���������ڴ� */
  	int shmid;
	char *shmaddr;

    if ((shmid=shmget(IPC_PRIVATE,sizeof(T_SHARE_MEM),0666)) < 0)
    {
		printf(" share memory create fail\n");
		exit(1);
	}	
	shmaddr = (char *)shmat(shmid,0,0);
	if ( shmaddr== (void*)-1)
	{
		printf(" attached memory fail\n");
	}
	
	/* father pid get share memory*/	
	server::SetMemoryAddress((T_SHARE_MEM*)shmaddr);

	/* sysipc get share memory addr*/
	CSysIpc::SetShareMemAddr((T_SHARE_MEM*)shmaddr);

	/* create new server object */
	serverList.push_back(new DbcjServer((T_SHARE_MEM*)shmaddr));
	serverList.push_back(new HttxServer((T_SHARE_MEM*)shmaddr));
	serverList.push_back(new PPPServer((T_SHARE_MEM*)shmaddr));
	st = serverList.begin();
	
	for (int i=0; i<serverList.size(); i++,st++)
	{
		result = fork();
		if (result == 0)
		{
			while(1)
			{
				(*st)->OntickServer();
			}
		}
	}

	
	/* ������ʱ���ź���*/ /* ���ڴ�������֮ǰ������ÿ�����̶���ALARM�����Է�������ֻ�и���������������û��*/
	server::SigInstall();
	
	while(1)
	{
		printf("daemon %d heart beat! \n",getpid());
		sleep(1);
	}
	return 0;
}




