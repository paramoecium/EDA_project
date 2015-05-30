#ifndef _MY_PARSE_H_
#define _MY_PARSE_H_

#include <fstream>
#include <vector>
#include <string>

using namespace std;

bool inSet(char c, const vector<char>& set){
   for(unsigned i=0, n=set.size(); i<n; ++i){
      if(c == set[i]) return true;
   }
   return false;
}

void parseStr(ifstream& fin, const vector<char>& sep, const vector<char>& stop, vector<std::string>& list) {
   char c;
   bool comment = false;
   string str;
   list.clear();
   while(fin.get(c)){
      // line comment
      if(c == '/'){
         if(!comment) comment = true;
         else{
            while(fin.get(c)){
               if(c == '\n') break;
            }
            comment = false;
         }
         continue;
      }
      else if(c == '*'){
         if(!comment) str += c;
         else{
            bool end = false;
            while(fin.get(c)){
               if(c == '*') end = true;
               else if(c == '/' && end) break;
               else end = false;
            }
            comment = false;
         }
         continue;
      }
      else comment = false;

      if(inSet(c, sep)){
         if(str.size() > 0) list.push_back(str);
         str = "";
      }
      else if(inSet(c, stop)){
         if(str.size() > 0) list.push_back(str);
         break;
      }
      else str += c;
   }
}

#endif
