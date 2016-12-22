#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
void fifo(FILE*,FILE*,int,int);
void lru(FILE*,FILE*,int,int);
void rand_cache(FILE*,FILE*,int,int);
int main(int argc,char* argv[])
{
	FILE *fp,*fout;
	char buf[100];
	char *policy,*vp_num,*phy_num;
	int vp,phy;
	policy = (char*)malloc(20*sizeof(char));
	vp_num = (char*)malloc(20*sizeof(char));	
	phy_num = (char*)malloc(20*sizeof(char));

	fp = fopen(argv[1],"r");
	fout = fopen(argv[2],"w");
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
		fifo(fp,fout,vp,phy);
		fclose(fout);
		fclose(fp);
	}
	else if(strcmp("LRU",policy)==0)
	{
		lru(fp,fout,vp,phy);
		fclose(fout);
		fclose(fp);
	}
	else if(strcmp("Random",policy)==0)
	{
		rand_cache(fp,fout,vp,phy);
		fclose(fout);
		fclose(fp);
	}
	else
	{
		printf("wrong policy\n");

	}
	return 0;	
}
void fifo(FILE *fp,FILE *fout,int vp,int phy)
{
	int fifo[vp],used[vp],mem[phy];
	int i,miss,src,dest;
	int ref,fifo_count=0,total=0,fault=0;
	for(i=0;i<vp;i++)
	{
		fifo[i]=-1;
		used[i]=0;
	}
	for(i=0;i<phy;i++)
		mem[i]=-1;
	while(fscanf(fp,"Reference %d\n",&ref)!=EOF)
	{
		total++;
		miss = 1;
		for(i=0;i<phy;i++)
		{
			if(mem[i] == ref)
			{
				printf("Hit, %d=>%d\n",ref,i);
				fprintf(fout,"Hit, %d=>%d\n",ref,i);
				miss =0;
			}
		}
		if(miss==1)
		{
			src=-1;
			dest=-1;
			for(i =0;i<vp;i++)
			{
				if(used[i]==0 && fifo[fifo_count]!=-1)
				{
					dest = i;
					fifo[i] = mem[fifo_count];
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
			printf("Miss, %d, %d>>%d, %d<<%d\n",fifo_count,mem[fifo_count],
					dest,ref,src);
			fprintf(fout,"Miss, %d, %d>>%d, %d<<%d\n",fifo_count,mem[fifo_count],
					dest,ref,src);
			fault++;
			mem[fifo_count++]=ref;
			fifo_count %= phy;
		}
	}

	printf("Page Fault Rate: %f\n",((float)fault/(float)total));
	fprintf(fout,"Page Fault Rate: %f\n",((float)fault/(float)total));
	
}
void lru(FILE *fp,FILE *fout,int vp,int phy)
{
	int lru_disk[vp],used[vp],lru_cnt[vp],mem[phy];
	int lru_count=0,total=0,fault=0,count=0;
	int ref,miss,i,j,src,dest,max;
	for(i=0;i<vp;i++)
	{
		lru_disk[i]=-1;
		lru_cnt[i]=-1;
		used[i] = 0;
	}
	for(i=0;i<phy;i++)
		mem[i] = -1;

	while(fscanf(fp,"Reference %d\n",&ref)!=EOF)
	{
		total++;
		miss=1;
		for(i=0;i<phy;i++)
		{
			if(mem[i] == ref)
			{
				printf("Hit, %d=>%d\n",ref,i);
				fprintf(fout,"Hit, %d=>%d\n",ref,i);
				miss=0;
				lru_count=i;
				//increment the lru_cnt[i] every time 
				for(i=0;i<phy;i++)
				{
					if(mem[i]!=-1)
						lru_cnt[i]++;
				}
				//re-initial the hit lru_cnt[]
				lru_cnt[lru_count]=0;
			}
		}
		if(miss == 1)
		{
			src=-1;
			dest=-1;
			for(i=0;i<phy;i++)
			{
				if(lru_cnt[i] > max)
				{
					max = lru_cnt[i];
					lru_count = i;
				}
				if(mem[i]==-1)
				{
					lru_count = i;
					break;
				}
			}
			for(i=0;i<vp;i++)
			{
				if(used[i] ==0 && mem[lru_count]!=-1)
				{
					dest = i;
					lru_disk[i] = mem[lru_count];
					used[i]=1;
					break;
				}
			}
			for(i=0;i<vp;i++)
			{
				if(lru_disk[i]==ref)
				{
					src=i;
					lru_disk[i]=-1;
					used[i]=0;
					break;
				}
			}
			printf("Miss, %d, %d>>%d, %d<<%d\n",lru_count,mem[lru_count],
													dest,ref,src);
			fprintf(fout,"Miss, %d, %d>>%d, %d<<%d\n",lru_count,mem[lru_count],
													dest,ref,src);
			for(i=0;i<phy;i++)
				if(mem[i]!=-1)
					lru_cnt[i]++;
			fault++;
			mem[lru_count]= ref;
			lru_cnt[lru_count] = 0;
			max=-1;
			count++;
			count%=phy;
		}

	}
	printf("Page Fault Rate: %f\n",((float)fault/(float)total));
	fprintf(fout,"Page Fault Rate: %f\n",((float)fault/(float)total));
}

void rand_cache(FILE*fp,FILE* fout, int vp, int phy)
{
	int rand_disk[vp],used[vp],mem[phy];
	int i,count,total=0,fault=0;
	int miss,src,dest,ref;
	srand(time(NULL));
	for(i=0;i<vp;i++)
	{
		rand_disk[i]=-1;
		used[i]=0;
	}
	for(i=0;i<phy;i++)
		mem[i] = -1;
	while(fscanf(fp,"Reference %d\n",&ref)!=EOF)
	{
		miss=1;
		total++;
		for(i =0;i<phy;i++)
		{
			if(mem[i] == ref)
			{
				printf("Hit, %d=>%d\n",ref,i);
				fprintf(fout,"Hit, %d=>%d\n",ref,i);
				miss=0;
				count=i;
				break;
			}
		}
		if(miss==1)
		{
			src =-1;
			dest = -1;
			for(i=0;i<vp;i++)
			{
				if(used[i]==0 && mem[count]!=-1)
				{
					dest = i;
					count = i;
					rand_disk[i] = mem[count];
					used[i] = 1;
					break;
				}
			}
			for(i=0;i<vp;i++)
			{
				if(rand_disk[i] == ref)
				{
					src = i;
					rand_disk[i] = -1;
					used[i] = 0;
				}
			}
			printf("Miss, %d, %d>>%d, %d<<%d\n",count,mem[count],
													dest,ref,src);
			fprintf(fout,"Miss, %d, %d>>%d, %d<<%d\n",count,mem[count],
													dest,ref,src);
			for(i = 0;i<phy;i++)
			{
				if(mem[i] != -1)
				{
					count = rand()%vp;	
				}
				else if(mem[i] == -1)
				{
					count = i;
					break;
				}
			}
			fault++;
			mem[count]= ref;
			rand_disk[count] = 0;
		}
	}
	printf("Page Fault Rate: %f\n",((float)fault/(float)total));
	fprintf(fout,"Page Fault Rate: %f\n",((float)fault/(float)total));
}
