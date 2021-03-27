    operands[0] = readWord(line, lInd);
    if (operands[0] == NULL)
        return NULL;
    if (strcmp(operands[0], "\0") == 0){
        printf("error [line %d]: After the end of a label must come a directive or an instruction\n", lineCnt);
        freeArr(operands, opAmount);
        return error;
    }
    if ((operationInd = getOperationIndex(operands[0], lineCnt)) == -1){
        freeArr(operands, opAmount);
        return error;
    }
    jumpSpaces(line, lInd);
    for (; opAmount <= MAX_WORDS; opAmount++){
        if ((operands[opAmount] = readWord(line, lInd)) == NULL || strcmp(operands[opAmount], "\0") == 0){
            freeArr(operands, opAmount);
            if (operands[opAmount] == NULL){
                free(error);
                return NULL;
            }
            return error;
        }
        if (!isThereComma(line, lInd, lineCnt)){
            correctCommas = FALSE;
            break;
        }
        if (opAmount == MAX_WORDS){
            if (!isBlank(line, *lInd)){
                if (isThereComma(line, lInd, lineCnt))
                    /*validOpsAmnt = FALSE;
                    break;*/;
                else
                    printf("error [line %d]: Extranous text after end of command\n", lineCnt);
                freeArr(operands, opAmount);
                return error;
            }
            break;
        }
    }
    if (!isValidOpsAmount(operationInd, opAmount - 1, lineCnt)/* || !validOpsAmnt*/){
        /*if(!validOpsAmnt)
            isValidOpsAmount(operationInd, -1, lineCnt);*/
        freeArr(operands, opAmount);
        return error;
    }