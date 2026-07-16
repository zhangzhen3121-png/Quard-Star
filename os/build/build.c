#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>

#define TARGET_PATH "../user/bin/"

void insert_app_data(){

    FILE* f = fopen("../src/link_app.S","w");
    if(f == NULL){
        perror("Failed open file");
        exit(EXIT_FAILURE);
    }

    char* app[100];
    int app_num = 0;

    DIR* app_dir = opendir(TARGET_PATH);
    if(app_dir == NULL){
        perror("Failed open DIR");
        exit(EXIT_FAILURE);
    }

    struct dirent* dir_entry;
    while((dir_entry = readdir(app_dir))!=NULL)
    {   
        if(dir_entry->d_name[0] != '.'){
            app[app_num] = dir_entry->d_name;
            printf("file: %s\n",app[app_num]);
            app_num++;
        }
    }
    
    fprintf(f,".align 3\n.section .data\n.global _num_app\n_num_app:");
    fprintf(f,"\n.quad 2");
    for(int i=0;i<app_num;i++){
        fprintf(f,"\n.quad app_%d_start",i);
        if(i==app_num-1)fprintf(f,"\n.quad app_%d_end",i);
    }

    for(int i=0;i<app_num;i++){
        fprintf(f,"\n.align 3\n.global app_%d_start\n.global app_%d_end\napp_%d_start:",i,i,i);
        fprintf(f,"\n.incbin \"%s%s\"",TARGET_PATH,app[i]);
        fprintf(f,"\napp_%d_end:",i);
    }

    
    closedir(app_dir);
    fclose(f);
}

int main(){
    insert_app_data();
    return 0;
}

