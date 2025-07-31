#include<stdio.h>
int main(){
    FILE * fh_output;
    fh_outpupt =fopen("output.txt","w");
    for(int i = 0;i<10;i++){
        fprintf(fh_output,"%d\n",i);
    }
    fclose(fh_output);
}