#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/input.h>


#include <pthread.h>

void usage ( int argc, char *argv[] )
{
    printf("Usage:%s\n" , argv[0]);

    exit(EXIT_FAILURE);
}

// Global variable:
//int i = 2;

void* ksThread(void* p);

int main ( int argc, char *argv[], char *env[] )
{
    if ( argc != 2 )    usage(argc, argv);

    int key;

    FILE *kbd = fopen("/dev/input/kbd", "r");

    char key_map[KEY_MAX/8 + 1];    //  Create a byte array the size of the number of keys
    char buf[30];
    char shName[30];


    //if (access(buf, F_OK) == 0) {
    static const char stateFolder[] = "/dev/input/kstate";
    static const char shortkFolder[] = "/dev/input/kstate-shortcuts";
    mkdir(stateFolder,S_IWUSR | S_IRGRP | S_IROTH);
    mkdir(shortkFolder,S_IWUSR | S_IRGRP | S_IROTH);
    //sprintf(buf, "/dev/input/kbd");
    system("ln -sf /dev/input/by-path/*-kbd /dev/input/kbd");

    //system("ln -s /dev/input/kbd-state /dev/input/kstate");
    //if (access(buf, F_OK) == 0) {
    // check if there is shotcuts folder 
    bool shortCuts = false;  
    sprintf(buf, "%s/",shortkFolder);
    if (access(buf, F_OK) == 0) {
        shortCuts = true;
    }

    char * line = NULL;//global for thread

    bool wasChange = false;
    int j=0;
    char snum[10];
    while (true){
        memset(key_map, 0, sizeof(key_map));    //  Initate the array to zero's
        ioctl(fileno(kbd), EVIOCGKEY(sizeof(key_map)), key_map);    //  Fill the keymap with the current keyboard state
        
        //ins sum =0;
        strcpy(shName,"");
        
        for(int i=0;i<KEY_MAX+1;i++){
            int keyb = key_map[i/8];
            int mask = 1 << (i % 8);
            sprintf(buf, "%s/%i",stateFolder, i);
            bool pressed = (keyb & mask);
            if (shortCuts && pressed){
                //build shortcut name
                sprintf(snum,"%i",i);
                strcat(shName,snum);
                strcat(shName,".");
            }

            if (access(buf, F_OK) == 0) {
                //printf("%i exists\n",i);
                if (!pressed){
                    //wasChange = true;
                    printf("u:%i\n",i);
                    remove(buf);
                }
            } else {
                //printf("%i notExists\n",i);
                if (pressed){
                    wasChange = true;
                    int fd2 = open(buf, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
                    close(fd2);
                    printf("d:%i\n",i);
                }
            }
            //sum+=key_map[i];
        }//for
        //printf("nexrrt %i\n",j++);

        //handle shotcuts
        if (wasChange){
            wasChange = false;
            strcat(shName,"kst");
            sprintf(buf, "%s/%s",shortkFolder, shName);
            printf("shortkFolder/Name:%s\n",buf);
            if (access(buf, F_OK) == 0) {
                FILE * fp;
                if (line)
                    free(line);
                size_t len = 0;
                ssize_t read;
                fp = fopen(buf, "r");
                printf("readLine:\n");
                while ((read = getline(&line, &len, fp)) != -1) {
                    printf("Retrieved line of length %zu:\n", read);
                    printf("%s", line);
                    if (line && line[0]!='#'){
                        //system(line);
                        pthread_t id;
                        pthread_create(&id, NULL, ksThread, (void*)line);
                    }
                }
                fclose(fp);
            }

        }//wasChange

    }//while

    int keyb = key_map[key/8];  //  The key we want (and the seven others arround it)
    int mask = 1 << (key % 8);  //  Put a one in the same column as out key state will be in;

    if (keyb & mask) {
        printf("pressed");    
    }  //  Returns true if pressed otherwise false

}


void* ksThread(void* p){
  char *str = (char*)p;
  if (str){  
    // Print value received as argument:
    //printf("Value recevied as argument in starting routine: %s\n",str);
    //#printf("%i\n", * (int*)p);
    printf("[kstate/short] command: %s\n", str);
    system(str);
  }

  // Return reference to global variable:
  //pthread_exit(&i);
  return 0;
}