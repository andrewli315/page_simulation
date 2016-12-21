#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void fifo(FILE*,FILE*,int,int);
void lru(FILE*,FILE*,int,int);
void rand_cache(FILE*,FILE*,int,int);
int main(void)
{
	FILE *fp,*fout;
	char buf[100];
	char *policy,*vp_num,*phy_num;
	int vp,phy;
	policy = (char*)malloc(20*sizeof(char));
	vp_num = (char*)malloc(20*sizeof(char));	
	phy_num = (char*)malloc(20*sizeof(char));
	fp = fopen("./random/lru_20_10_500.txt","r");
	fout = fopen("./fout_lru.txt","w");
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
	return 0;	
}
void fifo(FILE *fp,FILE *fout,int vp,int phy)
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
		printf("%d\n",ref);
		getchar();
		for(i=0;i<phy;i++)
		{
			if(frame[i] == ref)
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
			fprintf(fout,"Miss, %d, %d>>%d, %d<<%d\n",fifo_count,frame[fifo_count],
					dest,ref,src);
			fault++;
			frame[fifo_count++]=ref;
			fifo_count %= phy;
		}
	}

	printf("%d/%d = %f\n",fault,total,((float)fault/(float)total));
	fprintf(fout,"fault rate = %d/%d = %f\n",fault,total,((float)fault/(float)total));
	
}
void lru(FILE *fp,FILE *fout,int vp,int phy)
{
	int lru[vp],used[vp],lru_cnt[vp],frame[phy];
	int lru_count=0,total=0,fault=0;
	int ref,miss,i,j,src,dest,max;
	for(i=0;i<vp;i++)
	{
		lru[i]=-1;
		lru_cnt[i]=-1;
	}
	for(i=0;i<phy;i++)
		frame[i] = -1;

	while(fscanf(fp,"Reference %d\n",&ref)!=EOF)
	{
		total++;
		miss=1;
		for(i=0;i<vp;i++)
		{
			if(frame[i] == ref)
			{
				printf("Hit, %d=>%d\n",ref,i);
				fprintf(fout,"Hit, %d=>%d\n",ref,i);
				miss=0;
				lru_count=i;
				//increment the lru_cnt[i] every time 
				for(i=0;i<phy;i++)
					lru_cnt[i]++;
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
				else if(lru_cnt[i]==-1)
				{
					lru_count = i;
					break;
				}
			}
			for(i=0;i<vp;i++)
			{
				if(frame[lru_count]!=-1 && used[i]==0)
				{
					dest = i;
					lru[i] = frame[lru_count];
					used[i]=1;
					break;
				}
			}
			for(i=0;i<vp;i++)
			{
				if(lru[i]==ref)
				{
					src=i;
					lru[i]=-1;
					used[i]=0;
					break;
				}
			}
			printf("Miss, %d, %d>>%d, %d<<%d\n",lru_count,frame[lru_count],
													dest,ref,src);
			fprintf(fout,"Miss, %d, %d>>%d, %d<<%d\n",lru_count,frame[lru_count],
													dest,ref,src);
			fault++;
			frame[lru_count]= ref;
			lru[lru_count] = 0;
			max=-1;
		}

	}

}


