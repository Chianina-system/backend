#ifndef COMP_GRAMMAR_H
#define COMP_GRAMMAR_H
#include "../common/CommonLibs.hpp"

class Grammar {
	private:
		char rawLabel[256][GRAMMAR_STR_LEN];	// rawLabel[0]~rawLabel[255] -> (char)-128 ~ (char)127
		int numRawLabels;

		char erules[256];	// e-rule
		int numErules;		// number of e-rule
		char rules[65536];	// s-rule && d-rule
							// rules[0]~rules[65535] -> (short)-32768 ~ (short)32767

	public:
		Grammar();
		// getters and setters
		int addRawLabel(char *label);
		char getLabelValue(const char *str);
		inline char *getRawLabel(char value){return rawLabel[value+128];}

		inline int getNumErules() {return numErules;}
		inline char getErule(int index) {return erules[index];}

		inline char checkRules(char edgeVal) {return rules[changeShort((char)127,edgeVal)+32768];} // find s-rule edges
		inline char checkRules(char srcVal,char dstVal) {return rules[changeShort(srcVal,dstVal)+32768];} // find d-rule edges;
			
		void myTrim(char *src);
		bool loadGrammar(const char *filename);	// load grammar from file
		inline short changeShort(char a,char b) {return (short)a << 8 | ((short)b & 0xFF);}
		void test();

		// TODO: modify following methods.
		inline bool isAssign(char label) {char* raw = this->getRawLabel(label); return strcmp(raw, "a");}

		inline bool isAssign_bidirect(char label){char* raw = this->getRawLabel(label); return strcmp(raw, "a") || strcmp(raw, "-a");}

		inline bool isMemoryAlias(char label) {char* raw = this->getRawLabel(label); return strcmp(raw, "M");}

		inline bool isDereference(char label) {char* raw = this->getRawLabel(label); return strcmp(raw, "d");}

		inline bool isDereference_reverse(char label) {char* raw = this->getRawLabel(label); return strcmp(raw, "-d");}

		inline bool isDereference_bidirect(char label){char* raw = this->getRawLabel(label); return strcmp(raw, "d") || strcmp(raw, "-d");}

		inline bool isEruleLabel(char label){
			for(int i = 0; i < getNumErules(); i++){
				if(label == getErule(i)){
					return true;
				}
			}
			return false;
		}
};

#endif
