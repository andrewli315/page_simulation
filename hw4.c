#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void fifo(FILE* fp,int,int);
int main(void)
{
	FILE *fp;
	char buf[100];
	char *policy,*vp_num,*phy_num;
	int vp,phy;
	policy = (char*)malloc(20*sizeof(char));
	vp_num = (char*)malloc(20*sizeof(char));	
	phy_num = (char*)malloc(20*sizeof(char));
	fp = fopen("./random/fifo_20_10_500.txt","r");
	fgets(buf,100,fp);
	sscanf(buf,"Policy: %s",policy);
	printf("Policy: %s\n",policy);
	fgets(buf,100,fp);
	sscanf(buf,"Number of Virtual Page: %s",vp_num);
	fgets(buf,100,fp);
	sscanf(buf,"Number of Physical Frame: %s",phy_num);
	printf("%s\n%s\n",vp_num,phy_num);
	fgets(buf,100,fp);
	vp = atoi(vp_num);
	phy = atoi(phy_num);
	if(strcmp("FIFO",policy)==0)
	{
		fifo(fp,vp,phy);
	}
	return 0;	
}
void fifo(FILE *fp,int vp,int phy)
{
	int fifo[vp],used[vp],frame[phy];
	int i,miss,src,dest;
	int ref,fifo_count=0,total=0,fault=0;
	for(i=0;i<vp;i++)
	{
		fifo[i]=-1;
		used[i]=0;
	}
	for(i=0;i<phy;i++)
		frame[i]=-1;
	while(fscanf(fp,"Reference %d\n",&ref)!=EOF)
	{
		total++;
		miss = 1;
		for(i=0;i<phy;i++)
		{
			if(frame[i] == ref)
			{
				printf("Hit %d=>%d\n",ref,i);
				miss =0;
			}
		}
		if(miss==1)
		{
			src=-1;
			dest=-1;
			for(i =0;i<vp;i++)
			{
				if(used[i]==0&&fifo[i]==-1)
				{
					dest = i;
					fifo[i] = frame[fifo_count];
					used[i]=1;
					break;
				}
			}
			for(i=0;i<vp;i++)
			{
				if(fifo[i] == ref)
				{
					src = i;
					fifo[i]=-1;
					used[i]=0;
					break;
				}
			}
			printf("Miss, %d, %d>>%d, %d<<%d\n",fifo_count,frame[fifo_count],
													dest,ref,src);
			fault++;
		}
		fifo_count++;
		frame[fifo_count++]=ref;
		fifo_count %= phy;
	}

}
