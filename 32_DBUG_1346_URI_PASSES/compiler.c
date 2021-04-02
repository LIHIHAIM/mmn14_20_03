/* [mmn 14: compiler.c]:
This file contiains the main function in the compilation process of the assembly file.
The compile() function will compile the wnated assmebly file into three optput files: 
fileName.ob, fileName.ent, fileName.ext (if possible)

This file also conatins the sub functions nedded for the main 'compile()' fucntion.

author: Uri K.H,   Lihi Haim    Date: 2.4.2021 
ID: 215105321,     313544165    Tutor: Danny Calfon
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "files.h"
#include "line.h"
#include "label.h"
#include "util.h"
#include "instructionHandeling.h"
#include "dataHandeling.h"
#include "symbolTable.h"
#include "statmentType.h"

#define FOREVER 1

void clean(boolean, FILE *);

/* compile(): The function compiles an Assembly file and make one to three files in addition:
 file.ob - a binary file, which includes the memory....
 file.ent - an "entry" file, which includes the names of all the variable which other files can see and
            their address in the RAM.
 file.ext - an "extern" file, which includes the names of all the external variables (from other files) 
            and their calling address in the RAM. */
boolean compile(char *fileName){
     /* file variables */
     char *tempFileName = malloc(strlen(fileName) + MAX_SUFIX_LEN + 1);
     FILE *fd = openf(addSufix(tempFileName, ".as", fileName), "r");
     register int lineCnt = 0;
     /* line variables */
     char line[LINE_LEN + 1] = {'\0'};/* array for handling a line of assembly code */
     int lInd; /* the Line-Index */
     char *tempLine = NULL, *optLabel = NULL;
     /* label and compilation variables and falgs */
     statType type;
     boolean compSuc = TRUE, labelExist, pushed;

     if (!isAlloc(tempFileName)){
          fclose(fd);
          return ERROR;
     }
     if (!fd){
          free(tempFileName);
          return FALSE;
     }
     free(tempFileName);
     clean(FALSE, NULL);

     while (FOREVER){ /* first pass (reading file first time) loop */
          labelExist = FALSE;
          lineCnt++;
          lInd = 0;
          tempLine = readLine(fd, LINE_LEN + 1, lineCnt); /* reading a line from the file */
          if (tempLine[0] == '\0'){ /* end of file reached */
               free(tempLine);
               break;
          }
          strcpy(line, tempLine);
          free(tempLine);

          jumpSpaces(line, &lInd);
          if(line[lInd] == '\0')
               continue;
          if (!(optLabel = readWord(line, &lInd))){
               clean(TRUE, fd);
               return ERROR;
          }
          else if(isLabel(optLabel)){ /* optional label validation and first line checking */
               labelExist = TRUE;
               if(!isValidLabel(optLabel, lineCnt, TRUE)) /* could not get a label */
                    compSuc = FALSE;
               if(isBlank(line, lInd)){
                    printf("error [line %d]: statment cannot include only label defenition\n", lineCnt);
                    printf("warning [line %d]: the label \"%s\" in a blank line is meaningless (ignored)\n", lineCnt, optLabel);
                    jumpSpaces(line, &lInd);
                    free(optLabel);
                    compSuc = FALSE;
                    continue;
               }
          }
          else if(!labelExist){
               free(optLabel);
               lInd = 0;
          }
          switch ((type = getStatType(line))){ /* finding the statment type (instruction/ directive/ balnk line/ comment) */
          case blank: /* would not enter... */
          case comment:
               break;
          case directive:
          {    
               int commandType;
               char *tempCommand;
               jumpSpaces(line, &lInd);
               if((tempCommand = readWord(line, &lInd)) == NULL){ /* can not be blank (checking in line ) */
                    free(tempCommand);
                    free(optLabel);
                    clean(TRUE, fd);
                    return ERROR;
               }
               else if(isBlank(line, lInd)){
                    printf("error [line %d]: missing argument after the directive \"%s\"\n", lineCnt, tempCommand);
                    jumpSpaces(line, &lInd);
                    compSuc = FALSE;
                    free(tempCommand);
                    if(labelExist)
                         free(optLabel);
                    continue;
               }
               switch ((commandType = getDirCom(tempCommand, lineCnt, TRUE))){
               case data:
               case string:
                    free(tempCommand);
                    if (labelExist){
                         if (!(pushed = addToSymTab(optLabel, DAT, lineCnt)))
                              compSuc = FALSE;
                         else if(pushed == ERROR){
                              free(optLabel);
                              clean(TRUE, fd);
                              return ERROR;
                         }
                         free(optLabel);
                    }
                    if(commandType == data){
                         if(!(pushed = pushData(line, &lInd, "data", lineCnt, compSuc)))
                              compSuc = FALSE;
                    }
                    else if(commandType == string){
                         if(!(pushed = pushData(line, &lInd, "string", lineCnt, compSuc)))
                              compSuc = FALSE;
                    }
                    if(pushed == ERROR){
                         clean(TRUE, fd);
                         return ERROR;
                    }
                    break;
               case external:
                    free(tempCommand);
                    if (labelExist){
                         if(isIlegalName(optLabel, lineCnt) || wasDefined(optLabel, lineCnt, TRUE))
                              compSuc = FALSE;
                         printf("warning [line %d]: the label \"%s\" before the directive \".extern\" is meaningless (ignored)\n", lineCnt, optLabel);
                         free(optLabel);
                    }
                    if (!(pushed = pushExtern(line, &lInd, lineCnt)))
                         compSuc = FALSE;
                    else if (pushed == ERROR){
                         clean(TRUE, fd);
                         return ERROR;
                    }
                    break;
               case entry:
               {    
                    boolean check;
                    free(tempCommand);
                    if (labelExist){
                         if(isIlegalName(optLabel, lineCnt) || wasDefined(optLabel, lineCnt, TRUE))
                              compSuc = FALSE;
                         printf("warning [line %d]: the label \"%s\" label before the directive \".entry\" is meaningless (ignored)\n", lineCnt, optLabel);
                         free(optLabel);
                    }
                    if(!(check = checkEntry(line, &lInd, lineCnt))) /* if the entry line is (semi) valid */
                         compSuc = FALSE;
                    else if(check == ERROR){
                         free(optLabel);
                         clean(TRUE, fd);
                         return ERROR;
                    }
                    break;
               }
               default: /* could not find a valid directive command */
                    if(labelExist)
                         free(optLabel);
                    free(tempCommand);
                    compSuc = FALSE;
               }
               break;
          }
          case instruction:
               if (labelExist){
                    if (!(pushed = addToSymTab(optLabel, COD, lineCnt)))
                         compSuc = FALSE;
                    else if(pushed == ERROR){
                         free(optLabel);
                         clean(TRUE, fd);
                         return ERROR;
                    }
                    free(optLabel);
               }
               if(!(pushed = pushInstFirst(line, &lInd, lineCnt, compSuc)))
                    compSuc = FALSE;
               else if(pushed == ERROR){
                    clean(TRUE, fd);
                    return ERROR;
               }
               break;
          }
     }
     if (compSuc == FALSE){
          clean(TRUE, fd);
          return FALSE;
     }

     encPlusIC();
     rewind(fd);
     lineCnt = 0;

     
     while (FOREVER){ /* second pass (reading file in the second time) loop */
          lInd = 0;
          lineCnt++;
          labelExist = FALSE;
          tempLine = readLine(fd, LINE_LEN + 1, lineCnt); /* reading a line from the file */
          if (tempLine[0] == '\0'){ /* end of file reached */
               free(tempLine);
               break;
          }
          strcpy(line, tempLine);
          free(tempLine);
          
          jumpSpaces(line, &lInd);
          if(line[lInd] == '\0')
               continue;
          if (!(optLabel = readWord(line, &lInd))){
               free(tempFileName);
               clean(TRUE, fd);
               return ERROR;
          }
          if(isLabel(optLabel)) /* jumping label if exist (validation in first pass) */
               labelExist = TRUE;
          if(!labelExist)
               lInd = 0;
          free(optLabel);

          switch ((type = getStatType(line))){ /* finding the statment type (instruction/ directive/ balnk line/ comment) */
          case blank:
          case comment:
               break;
          default:
          {
               boolean added;
               if(!(added = pushEntry(line, &lInd, lineCnt))){ /* */
                    compSuc = FALSE;
                    break;
               }
               else if(added == ERROR){
                    clean(TRUE, fd);
                    return ERROR;
               }
               else if(lInd == -1)
                    break;
               jumpSpaces(line, &lInd);
               while(line[lInd] != '\0'){
                    if(!(added = pushInstSecond(line, &lInd, lineCnt))){
                         compSuc = FALSE;
                         break;
                    }
                    else if(added == ERROR){
                         clean(TRUE, fd);
                         return ERROR;
                    }
                    isThereComma(line, &lInd, lineCnt);
               }
               break;
          }
          }
     }
     if(compSuc == TRUE){
          if(!buildOutputFiles(fileName)){
               clean(TRUE, fd);
               return ERROR;
          }
     }
     clean(TRUE, fd);
     return compSuc;
}

/* clean(): the function get as a parameter the boolean flag that indicates
 whether to free all DATA and Instruction that are stored and if it's TRUE releases all DATA
 and sets DC and IC counters to thier original values*/
void clean(boolean terminate, FILE *fd){
     cleanDC();
     cleanIC();
     cleanSymAndExtTab();
     if(terminate){
          fclose(fd);
          printf("CLOSED\n");
     }
     return;
}