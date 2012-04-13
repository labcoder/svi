/*SVI Editor by Oscar Sanchez*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char String[256];

//enum for type of range specification
typedef enum {none,text,range} lineSpecType;

//union for range specification-------------------------------------------------------
typedef union {
    //none is treated as a boolean. 0 if there is no special specifier
    int none;
    //this stores what the text will be that must be contained
    String containedText;
    //range[0] and range[1] are the first and last line numbers, respectively
    int range[2];
} lineSpecUnion;

//the stuct for the operators---------------------------------------------------------
typedef struct {
    lineSpecType specType;
    lineSpecUnion specifier;

    //will be one of {A,I,O,d,s}
    char operation;
    String data;
} Edit;

//method to get the first two integers; the line range specifications-----------------
void getRange(int nums[],String original) {
    char *separators=",/";
    String line; //this is so I don't mess with the original command string
    strncpy(line,original,strlen(original));
    int i=0;
    char *token;
    //I'm using tokenizers to make it more efficient. i<2 because I only want the two ints
    token=strtok(line,separators);
    while(i<2 && token!=NULL) {
        nums[i]=atoi(token);
        token=strtok(NULL,separators);
        i++;
    }

}

//returns a struct for a particular edit command--------------------------------------
Edit getEditStruct(String command) {
    Edit ret;
    //check what type of line specs this command uses
    if (command[0]=='/') {        //uses text specifier
        ret.specType=text;
        strncpy(ret.specifier.containedText,command+1,strlen(command)-strlen(strchr(command+1,'/')));
        ret.specifier.containedText[strlen(command)-strlen(strchr(command+1,'/'))-1]='\0';
        //update String to now point to the actual operator
        char *temp=strstr(command+1,"/")+1;
        strncpy(command,temp,strlen(temp)+1);
        //keep track of what operator it is
        ret.operation=command[0];
        //if the operator is 'd', then ret.data will just be "\0"
        strncpy(ret.data,command+1,strlen(command));
    }
    else if(isdigit(command[0])) { //uses line range specifier
        //to-Do
        ret.specType=range;
        //get the line ranges
        getRange(ret.specifier.range,command);
        //update String to now point to the actual operator
        char *tmp=strstr(command,"/")+1;
        strncpy(command,strstr(command,"/")+1,strlen(command));
        //keep track of what operator it is
        ret.operation=command[0];
        //if the operator is 'd', then ret.data will just be "\0"
        strncpy(ret.data,command+1,strlen(command));
    }
    else {                         //uses no specifier: apply to all!
        //to-Do
        ret.specType=none;
//        printf("You've specified none.\n");
        ret.specifier.none=1;
        ret.operation=command[0];
        strncpy(ret.data,command+1,strlen(command));
    }

    return ret;
}


//This method reads and stores the edit commands from the file------------------
void readInput(FILE *commandFile, Edit edits[],int * numOfEdits) {
    String input;
    int i=0;
    /*  read lines from input, using fgets until either more than 100 edits
        are encountered or you reach the end of file                     */
    while (i<100 && fgets(input,256, commandFile)!=NULL) {
        edits[i]=getEditStruct(input);
        i++;
    }
    //keeping track of how many edit commands there Are.
    *numOfEdits=i;
}
//Checks whether a line should be edited by that Edit struct--------------------------

int isCompatible(Edit edit,String input,int lineNumber) {
    if(edit.specType==none){ //if there is no specifier, then it edits ALL lines
        return 1;
    }
    if(edit.specType==range) {
        if (lineNumber>=edit.specifier.range[0] && lineNumber<=edit.specifier.range[1])
        return 1;
    }
    if(edit.specType==text) {
        if(strstr(input,edit.specifier.containedText)!=NULL)
            return 1;
    }
    return 0;

}

//THIS Method replaces an occurance or orig in st with repl-------------
char *replace(char *st, char *orig, char *repl) {
    static char buffer[512];
    char *ch;
    if (!(ch = strstr(st, orig)))
        return st;
    strncpy(buffer, st, ch-st);  
    buffer[ch-st] = 0;
    sprintf(buffer+(ch-st), "%s%s", repl, ch+strlen(orig));
    return buffer;
  }

//This is the important function, it edits a line according to some command-----------
void editThis(Edit edit,String input,String OLines,String ILines,String ALines,int *p1) {
    //if the edit is to delete, then just stop and delete the string
    if (edit.operation=='d') {
        *p1=1;
        return;
    }
    if (edit.operation=='A') {
        strncat(ALines,edit.data,strlen(edit.data));
        ALines[strlen(ALines)]='\0';
        return;
    }
    if (edit.operation=='O') {
        //strcat(OLines,"\n");
        strncat(OLines,edit.data,strlen(edit.data));
        OLines[strlen(OLines)]='\0';
        return;
    }
    if (edit.operation=='I') {
        String temp;
        strcpy(temp,edit.data);
        strcat(temp,ILines);
        //since each I should be added to the beginning of the line, I had to do it this way.
        strcpy(ILines,temp);
        ILines[strlen(ILines)-1]='\0';
        while(strstr(ILines,"\n")!=NULL) {
            strcpy(ILines,replace(ILines,"\n"," "));
        }
    }
    if (edit.operation=='s') {
        String textToReplace, replacement;
        char * token;
        //printf("data is:%s-\n",edit.data);
        token=strtok(edit.data,"/");
        if(token!=NULL) {
            strcpy(textToReplace,token);
            token=strtok(NULL,"/");
        }
        if(token!=NULL) {
            strcpy(replacement,token);
        }
        replacement[strlen(replacement)]='\0';
        textToReplace[strlen(textToReplace)]='\0';
        //printf("first is: %s,second is %s...",textToReplace,replacement);
        while(strstr(input,textToReplace)) {
            strcpy(input,replace(input,textToReplace,replacement));
        }

    }
    

}




//MAIN FUNCTION!!! AAAAAAAAAAAHHHHHH--------------------------------------------------
int main(int argc, char*argv[]) {
//----------------------------READ/STORE EDIT COMMANDS FROM FILE----------------------
    FILE *commands;  //the file we'll read the commands from
    if (argc<2) {
        printf("usage is: svi <command file>\n");
        return 0;
    }
    //create pointer to read file of commands
    commands=fopen(argv[1],"r");
    //catch error if file doesn't exist
    if (commands==NULL) {
        printf("File pointer could not be made... does the file \"%s\" exist?\n",argv[1]);
        return 1;
    }
    
    Edit editArray[100]; //max number of edit lines = 100, stored in an array of structs
    //this will keep track of how many edit commands there actually are. optimization
    int numOfEdits=0; int *p1;p1=&numOfEdits;
    //read commands from file. Reference to int is a loophole to avoid global variables
    readInput(commands,editArray,p1);

//----------------------------READ USER INPUT AND DO ALL THE STUFF---------------------

    //will contain the user input String
    String input;
    //will keep track of what line it's on.
    int lineNumber=1;
    

    while(fgets(input,256,stdin)!=NULL) {
        input[strlen(input)-1]='\0';
        String OLines,ILines,ALines;
        OLines[0]='\0';ILines[0]='\0';ALines[0]='\0';
        int i;
        //isDeleted=1 when the line should be deleted, this is to keep from running other edits.
        int isDeleted=0;
        p1=&isDeleted;
        //go through all the structs, see which ones are compatible, and if so, edit.
        for (i=1;i<=numOfEdits;i++) {
            if(!isDeleted && isCompatible(editArray[i-1],input,lineNumber)) {
                editThis(editArray[i-1],input,OLines,ILines,ALines,p1);
            }
        }
        //this String will contain what I'll output. I'll run it through a cleaner method to avoid continuous new lines.
        String finalOutput;

        /*input will be the user-input line modified by all the edits.
          OLines will have any O-edits (lines added BEFORE the modified input)
          ILines will have any I-edits (things added to the beginning of the input)
          ALines will have any A-edits (things appended to the end of the input)
          if at some point this line was deleted, then each string will be empty */
        if (!isDeleted)
            sprintf(finalOutput,"%s%s%s%s\n",OLines,ILines,input,ALines);
        else
            sprintf(finalOutput,"%s\n",OLines);
        while(strstr(finalOutput,"\n\n")!=NULL) {
            strcpy(finalOutput,replace(finalOutput,"\n\n","\n"));
        }
        while(strstr(finalOutput,"\n \n")!=NULL) {
            strcpy(finalOutput,replace(finalOutput,"\n \n","\n"));
        }
        if(strcmp(finalOutput,"\n"))
            printf("%s",finalOutput);
        lineNumber++;
    }


    return 0;
}
