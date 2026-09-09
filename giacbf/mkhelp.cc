// Example:
// ./mkhelp 0 > static_helpfr.h
// ./mkhelp 1 > static_helpen.h

#include <iostream>
#include <fstream>
#include <string.h>

const int HELP_LANGUAGES=5;

struct static_help_t {
  const char * cmd_name;
  const char * cmd_howto[HELP_LANGUAGES];
  const char * cmd_syntax;
  const char * cmd_related;
  const char * cmd_examples;
};

const static_help_t static_help[]={
#include "static_help.h"
};

const int static_help_size=sizeof(static_help)/sizeof(static_help_t);
using namespace std;

void output(ostream & of,const char * s){
  if (!s) return;
  int l=strlen(s);
  for (int i=0;i<l;++i){
    unsigned char c=(unsigned char)s[i];
    if (c=='"'){
      of << "\\\"";
      continue;
    }
    if (c>=0x80){
      // 非 ASCII 字节转义为独立字符串段。内容层输出 ""\xNN"",
      // 调用方的包裹引号使其成为 "ascii" "\xNN" "ascii" 相邻字面量
      // (C 自动拼接),既保持纯 ASCII 源,又避免 \x 转义吞掉后续
      // 十六进制字符(如 "\xE9a" 会把 'a' 也当作转义)。
      of << "\"\"\\x";
      of << hex << int(c);
      of << "\"\"";
      continue;
    }
    of << s[i];
  }
}

int main(int argc,char **argv){
  ofstream fr("static_helpfr.h");
  ofstream en("static_helpen.h");
  ofstream es("static_helpes.h");
  ofstream el("static_helpel.h");
  ofstream de("static_helpde.h");
  ofstream * ptrtab[]={&fr,&en,&es,&el,&de};
  for (int l=0;l<sizeof(ptrtab)/sizeof(ofstream *);++l){
    ofstream * ptr=ptrtab[l];
    for (int i=0;i<static_help_size;++i){
      const static_help_t & h=static_help[i];
      (*ptr) << "{\"" ;
      output(*ptr,h.cmd_name);
      (*ptr) << "\",\"";
      output(*ptr,h.cmd_howto[l]);
      (*ptr) << "\",\"";
      output(*ptr,h.cmd_syntax);
      (*ptr) << "\",\"";
      output(*ptr,h.cmd_related);
      (*ptr) << "\",\"";
      output(*ptr,h.cmd_examples);
      (*ptr) << "\"},\n";
    }
  }
}
