#include <iostream>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include "grammar.h"
using std::cout;
using std::endl;

Grammar::Grammar() {
    numRawLabels = 0;
    numErules = 0;
    for(int i = 0;i < 65536;++i)
        rules[i] = (char)127;
}

bool Grammar::loadGrammar(char *filename) {
    std::ifstream fin;
    fin.open(filename);
    if(!fin) {
        cout << "can't load grammar file: " << filename << endl;
        return false;
    }

    char str[512];
    char *token;
    char arg[3][GRAMMAR_STR_LEN]; int index[3];

    while(fin.getline(str,sizeof(str))) {
        int num = 0;
        token = strtok(str,"\t");
        while(token) {
            strcpy(arg[num++],token);
            token = strtok(NULL,"\t");
        }
        for(int i = 0;i < num;++i) {
            myTrim(arg[i]);
            index[i] = addRawLabel(arg[i]);
        }

        switch(num)
        {
            case 1:    // add e-rule
                erules[numErules++] = (char)(index[0]-128);
                break;
            case 2:    // add s-rule
            {
                short tmp = changeShort((char)127,(char)(index[1]-128));
                rules[tmp+32768] = (char)(index[0]-128);
                break;
            }
            case 3:    // add d-rule
            {
                short tmp = changeShort((char)(index[1]-128),(char)(index[2]-128));
                rules[tmp+32768] = (char)(index[0]-128);
                break;
            }
            default:
                ;
        }
    }
    test();
    fin.close();
    return true;
}

void Grammar::myTrim(char *src) {
    char *fp = src;
    while(*src) {
        if(*src != ' ')
            *(fp++) = *src;
        ++src;
    }
    *fp = '\0';
}

int Grammar::addRawLabel(char *label) {
    for(int i = 0;i < numRawLabels;++i) {
        if(!strcmp(rawLabel[i],label))
            return i;
    }
    strcpy(rawLabel[numRawLabels++],label);
    return numRawLabels-1;
}

void Grammar::test() {
    cout << "==========GRAMMER TEST START==========" << endl;
    cout << "rawLabels: " << endl;
    for(int i = 0;i < numRawLabels;++i)
        cout << "(" << rawLabel[i]  << "," << (i-128) << ") ";
    cout << endl;

    cout << "eRules :" << endl;
    for(int i = 0;i < numErules;++i)
        cout << (int)erules[i] << ",";
    cout << endl;

    cout << "s-rules and d-rules: " << endl;
    for(int i = 0;i < 65536;++i) {
        if(rules[i] != (char)127)
        {
            short s = i-32768;
            char a = *((char*)&s);
            char b = *(((char*)&s)+1);
            if(b == (char)127)
                cout << "s-rule: " << (int)rules[i] << ":= " << (int)a << endl;
            else
                cout << "d-rule: " << (int)rules[i] << ":= " << (int)b << "," << (int)a << endl;
        }
    }
    cout << "==========GRAMMAR TEST END============" << endl;
}

char Grammar::getLabelValue(char *str) {
    for(int i = 0;i < numRawLabels;++i) {
        if(!strcmp(rawLabel[i],str))
            return (char)(i-128);
    }
    return (char)127;
}

