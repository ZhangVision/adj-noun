#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

using namespace std;

char noun_file[100]; 
char adj_file[100];
char out_file[100]={"annotation_result.txt"};
int mode=0;  
vector<string> noun_words,adj_words;
int noun_size=0,adj_size=0;
int combination_max_size=0;
struct comb2{
  int i,ii;
};
struct comb3{
  int i,j,jj;
};
int que=0,ans=0,que_ans=0;
struct comb3* que_ans_comb;
int que_ans_comb_maxsize=100;

void help(){
  printf("Word Relative Adjectives DataSet Annotation Toolkit\n");
  printf("Usage:\n");
  printf("    -noun [noun words file]\n");
  printf("    -output [results file]\n");
  printf("    -adj [adjective words file]\n");
  printf("    -mode [naa(default) or ann]\n");
  printf("    -num [num of annotation (100 default)]\n");
}

int parse_argv(int argc,char **argv){      
  for(int i=1;i<argc-1;i++){
    if(!strcmp("-noun",argv[i])){
      strcpy(noun_file,argv[i+1]);
    }else if(!strcmp("-adj",argv[i])){
      strcpy(adj_file,argv[i+1]);
    }else if(!strcmp("-mode",argv[i])){
      if(!strcmp("ann",argv[i+1]))mode=1;
    }else if(!strcmp("-num",argv[i])){
      que_ans_comb_maxsize=atoi(argv[i+1]);
    }else if(!strcmp("-output",argv[i])){
      strcpy(out_file,argv[i+1]);
    }       
  }

  if(strlen(noun_file)<=0){
    printf("Missing noun words file! use: -noun\n");
    exit(0); 
  }
  if(strlen(adj_file)<=0){
    printf("Missing adjective words file! use: -adj\n");
    exit(0); 
  }
}

void readwords(){
  FILE* nfin=fopen(noun_file,"r");
  FILE* afin=fopen(adj_file,"r");
  if(nfin==NULL||afin==NULL){
    printf("Error opening noun and adj files\n");
    exit(1);
  }
  char word[50];
  while(fscanf(nfin,"%s",word)!=EOF){
    string temp(word);
    noun_words.push_back(temp);
  }
  while(fscanf(afin,"%s",word)!=EOF){
    string temp(word);
    adj_words.push_back(temp);
  }
  fclose(nfin);fclose(afin);
}

void preprocess_comb(){
  printf("debug");
  que=mode?adj_size:noun_size;
  ans=mode?noun_size:adj_size;
  que_ans=que*ans*(ans-1)/2;
  que_ans_comb_maxsize=que_ans_comb_maxsize>que_ans?que_ans:que_ans_comb_maxsize;
  struct comb2* ans_comb=(struct comb2*)malloc(ans*(ans-1)/2*sizeof(struct comb2));
  que_ans_comb=(struct comb3*)malloc(que_ans*sizeof(struct comb3));
  int c=0;
  for(int i=0;i<ans;i++)
    for(int ii=i+1;ii<ans;ii++){
      ans_comb[c].i=i;
      ans_comb[c++].ii=ii;
    }
  c=0;
  for(int i=0;i<que;i++)
    for(int j=0;j<ans*(ans-1)/2;j++){
      que_ans_comb[c].i=i; 
      que_ans_comb[c].j=ans_comb[j].i;
      que_ans_comb[c++].jj=ans_comb[j].ii;
    }
  free(ans_comb);
}

int rand_gen(int i){
  return rand()%i;
}
void swap(int i,int j){
  struct comb3 temp;
  temp=que_ans_comb[j];
  que_ans_comb[j]=que_ans_comb[i];
  que_ans_comb[i]=temp;  
}
void shuffle(){
  srand((unsigned)time(NULL));
  for(int i=que_ans-1;i>0;i--){
    swap(i,rand_gen(i));
  }
}
void print_to_annotator(int i,FILE*fo){
  char ques[20],anss1[20],anss2[20];
  int qi=que_ans_comb[i].i;
  int aj=que_ans_comb[i].j;
  int ajj=que_ans_comb[i].jj;
  if(!mode){
    strcpy(ques,noun_words[qi].c_str());
    strcpy(anss1,adj_words[aj].c_str());
    strcpy(anss2,adj_words[ajj].c_str());
  }else{
    strcpy(ques,adj_words[qi].c_str());
    strcpy(anss1,noun_words[aj].c_str());
    strcpy(anss2,noun_words[ajj].c_str());
  }  
  printf("\n(%d) which word do you think is more related with \'%s\':\n",i,ques);
  printf("1. %s\n",anss1);
  printf("2. %s\n",anss2);
  printf("3. both 1 and 2\n");
  printf("4. none of them\n\n");
  char answer[20]={"5"},buffer=0;
 // if((buffer=getchar())!=0&&buffer!='\n')
   // while((buffer=getchar())!='\n'&&buffer!=EOF){};
  do{
    printf("  your answer would be:");scanf("%s",answer);printf("\n");
  }while(strcmp(answer,"1")&&strcmp(answer,"2")&&strcmp(answer,"3")&&strcmp(answer,"4"));
  fprintf(fo,"%s %s %s %s\n",ques,anss1,anss2,answer);
}
int main(int argc,char **argv){
  if(argc==1){
    help();
    return 0;
  }
  parse_argv(argc,argv);
  readwords();

  noun_size=noun_words.size();
  adj_size=adj_words.size();
  
  printf("There are %d noun words:\n",noun_size);
  for(int i=0;i<noun_words.size();i++){
    printf("%s ",noun_words[i].c_str());
  }
  printf("\nThere are %d adjective words:\n",adj_size);
  for(int i=0;i<adj_words.size();i++){
    printf("%s ",adj_words[i].c_str());
  }  
  printf("\n");
  
  if(mode==0){
    combination_max_size=noun_size*adj_size*(adj_size-1)/2;
    printf("There could be %d (noun,adj1,adj2) combinations\n",combination_max_size); 
  }else{
    combination_max_size=adj_size*noun_size*(noun_size-1)/2;
    printf("There could be %d (adj,noun1,noun2) combinations\n",combination_max_size);
  }
  
  if(combination_max_size<=0)exit(1);
  preprocess_comb();
  shuffle();
  printf("************************************ANOTATION STARTS*************************\n");
  FILE* fo=fopen(out_file,"w");
  for(int i=0;i<que_ans_comb_maxsize;i++){
    print_to_annotator(i,fo);
  }
  free(que_ans_comb);
  fclose(fo);
}
