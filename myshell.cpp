#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <fcntl.h>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
using namespace std;

int pipe_num = 0; 
int ifredirect = 0;
void vect2charr(vector<string>* cmd_vct, char ** &argv);
void close_pipes(int pipe_fd[][2]);
//void delete_process(char*** const & argv_arr, char*** const & redi_arr, vector<string>*& redirect_arr,vector<string>& cmd_vct);
void test_output(vector<string> *redirect_arr, char*** argv_arr, char*** redi_arr);
int fork_loop(int *pid, int pipe_fd[][2], char*** argv_arr, char*** argk_arr);
int redirect_process(char ** argk);
void redirect_parse(vector<string>* vec_arr, vector<string> * redirect, char**& redi_arr);
int redirect_judgement(char** argk, int cnt);


void delete_process(char*** const & argv_arr, char*** const & redi_arr, vector<string>*& redirect_arr,vector<string>& cmd_vct) {
  for(int i = 0; i <= pipe_num; i++){
    /* to determine the size of argv_arr[i] */
    int k = 0;
    //while(!strcmp(argv_arr[i][k], "NULL")){  k++; }
    while(argv_arr[i][k] != NULL){
      k++;
    }
    //cout << "k in delete is nums of argv_arr[i][x] = " << k << endl;
    /************Let's Delete It***********/
    for(int j = 0; j <= k; j++) {
      delete [] argv_arr[i][j];
    }
  }
  for(int i = 0; i <= pipe_num; i++){
    delete [] argv_arr[i];
  }
  //delete redi_arr
  if(ifredirect != 0) {
    for(int i = 0; i < pipe_num + 1; i++){
      /* to determine the size of redi_arr[i] */
      int k = 0;
      while(redi_arr[i][k] != NULL){
	k++;
      }
      //cout << "k in delete is nums of redi_arr[i][x] = " << k << endl;
      /************Let's Delete It***********/
      for(int j = 0; j <= k; j++) {
	delete [] redi_arr[i][j];
      }
    }
    for(int i = 0; i < pipe_num + 1; i++){
      delete [] redi_arr[i];
    }
  }  
  cmd_vct.clear();
}


void command_parse(string str, vector<string> * cmd_vct) {
  string temp;
  string::iterator it_end = str.end();
  string::iterator it = str.begin();
  //cout << "Input Command is: "<< str << endl;
  /* parsing process */
  while( it != it_end) {
    if( *it == ' '){
      ++it;
    }
    else{
      if( (*it == '\\') && (*(it+1) == ' ') ) {
	temp.push_back(*(it+1)); // push_back a space
	++(++it);
	if( (*it == ' ') || (*it == '\0')) {
	  cmd_vct->push_back(temp);
	  temp.clear();
	}
	continue;
      }
      else{
	temp.push_back(*it);
	++it;
	if( (*it == ' ') || (*it == '\0')) {
	  cmd_vct->push_back(temp);
	  temp.clear();
	} 
      }
    }
  }
  for(vector<string>::iterator it = cmd_vct->begin(); it != cmd_vct->end(); ++it){
    if(*it == "|"){
      pipe_num++;
      //cout << "I find a \"|\" " << endl;
    }
  }
} 

void adv_parse(vector<string>* cmd_vct, vector<string>* & redirect_arr, char *** argv_arr, char*** redi_arr) {
  //int pipe_num = 0;   
  //int ifredirect = 0; became a global variable
  vector<string>::iterator it_pos;  // an iterator used as container
  vector<vector<string>::iterator> it_vec;  //contains all '|' iterator
  for(vector<string>::iterator it = cmd_vct->begin(); it != cmd_vct->end(); ++it){
    if(*it == "|"){
      //pipe_num++;
      it_vec.push_back(it);
      //cout << "I find a \"|\" " << endl;
    }
    else if( (*it == "<") || (*it == ">") || (*it == "2>") ) {
      ifredirect++;
    }
  }
  /***************************VEC_ARR****************************/
  vector<string>* vec_arr = new vector<string>[pipe_num+1]();
  /***************************VEC_ARR****************************/
  for(int i = pipe_num; i > 0; i--){
    it_pos = it_vec.back();
    it_pos++;                  // *it_pos = '|'
    for(;it_pos != cmd_vct->end(); ++it_pos){
      vec_arr[i].push_back(*it_pos); 
    }
    //do some process with vec_arr to delete the '<'/'>' and related in/out file
    if(ifredirect != 0){
      redirect_parse(&vec_arr[i], &redirect_arr[i], redi_arr[i]);
    }
    vect2charr(&vec_arr[i], argv_arr[i]);

    it_pos = it_vec.back();  // restore the '|' pos and clean this part
    while( it_pos != cmd_vct->end() ){
      cmd_vct->pop_back();
    }  
    it_vec.pop_back();
  }
  /* For the last vec_arr[] */
  for(vector<string>::iterator it = cmd_vct->begin(); it != cmd_vct->end(); ++it){
    vec_arr[0].push_back(*it);
  }
  if(ifredirect != 0){
    redirect_parse(&vec_arr[0], &redirect_arr[0], redi_arr[0]);
  }
  vect2charr(&vec_arr[0], argv_arr[0]);

  /* Print Out every vec_arr[]*//*  
  for(int i = 0; i <= pipe_num; i++){
    while(vec_arr[i].empty() != true){
      cout << "vec_arr[" << i <<"] = " << vec_arr[i].back() << endl;
      vec_arr[i].pop_back();
    } 
    } */
  
  /********* vec_arr scope end*******/
  delete [] vec_arr;
}

void redirect_parse(vector<string>* vec_arr, vector<string> * redirect, char**& redi_arr) {
  for(vector<string>::iterator it = vec_arr->begin(); it < vec_arr->end(); ){
  //while(vector<string>::iterator)
  //redirect analyze
    //cout << "*it = " << *it << endl;
    //cout << "redirect.size = " << redirect->size() << endl;
    //cout << redirect[0] << " " << redirect[1] << " " << redirect[2] << endl;
    if( (*it == "<") || (*it == ">") || (*it == "2>") ) {
      //cout << "*it = " << *it << endl;
      string a = *it;
      string b = *(it+1);
      redirect->push_back(a);
      redirect->push_back(b);
      it = vec_arr->erase(it);
      it = vec_arr->erase(it);
      /*
	for(vector<string>::iterator it = vec_arr->begin(); it != vec_arr->end(); ++it){
	cout << *it << endl;
	}*/
      //vec_arr->shrink_to_fit();
    }
    else{
      ++it;
    }
  }
  vect2charr(redirect, redi_arr);
  /* cat -p < in -f > out -l, could have two < > in one command line
  if( redirect->size() > 2) {
    cout << "redirect.size = " << redirect->size() << endl;
    cout << "Error: input 2 redirect symbol sequentially" << endl;
    }*/
  /*
    for(unsigned i = 0; i < redirect->size(); i++){
    cout << "redirect[" << i << "] = " << (*redirect)[i] << endl;
    }*/
}

/* Transfer vector into char* argv[] */
void vect2charr(vector<string>* cmd_vct, char ** &argv) {
  //cout << "this is char* output" << endl;
  argv = new char*[cmd_vct->size()+1]; // last one for NULL
  for(unsigned i = 0; i < cmd_vct->size(); i++) {    
    argv[i] = new char[ (*cmd_vct)[i].size() + 1 ];
    strcpy(argv[i], (*cmd_vct)[i].c_str());    
    //cout << "argv["<<i <<"] = " << argv[i] << endl;
  }
  // the last one is NULL
  argv[cmd_vct->size()] = NULL; 
}

int builtin_command(char* cwd_buf, vector<string>* direct, char ** argv) {
  if(!strcmp(argv[0], "cd")) {
    //cout << "argv[1] = " << argv[1] << endl;
    chdir(argv[1]);
    return 1;
  }
  if(!strcmp(argv[0], "pushd")) {
    direct->push_back(cwd_buf);
    chdir(argv[1]);
    return 1;
  }
  if(!strcmp(argv[0], "popd")) {
    if(direct->size() == 0) {
      cout << "Vector direct is empty" << endl;
      return 1;
    }
    string temp = direct->back();
    direct->pop_back();
    chdir(temp.c_str());
    return 1;
  }  
  if(!strcmp(argv[0], "dirstack")) {
    for(vector<string>::iterator it = direct->begin(); it != direct->end(); ++it) {
      //cout << "size = " << direct->size() << endl;
      cout << *it << endl;
    }
    return 1;
  }
  return 0;
}

int exit_command(vector<string> cmd_vct, char ** argv) {
  if(argv[0] == NULL) {
    //cout << "Read an Enter" << endl;
    return 2;
  }
  if(!strcmp(argv[0], "exit")) {
    //cout << "Read an Exit" << endl;
    return 0;
  }
  return 1;
}


int main() {
  pid_t ppid, wpid;
  //extern char **environ;
  string str;
  vector<string> direct;
  vector<string> cmd_vct;
  //vector<string> vec_arr[pipe_num+1];
  int status;
  char buf[100];
  int flag = 1;
  while(flag != 0){
    //cout << "flag = "<< flag << endl;
    pipe_num = 0;
    ifredirect = 0;
    getcwd(buf, sizeof(buf));
    cout.flush();
    cout << "myShell :";
    cout << buf <<" $ " ;
    /* Get the input Comnmand Line */
    getline(cin, str);
    if(cin.eof() == true) {
      //cout << "EOF!!!" << endl;
      return EXIT_SUCCESS;
    }
    /* Parse, Split and Stored into vector<string> cmd_vct */
    command_parse(str, &cmd_vct);   
    /* Transfer string into Char*[] */
    //cout << "pipe_num = " << pipe_num << endl;
    pid_t pid[pipe_num+1];
    int pipe_fd[pipe_num][2];
    /* Create all Pipes*/
    for(int i = 0; i < pipe_num; i++){
      if(pipe(pipe_fd[i]) < 0) {
	perror("Create pipe failed");
      }
    }
    char **argv_arr[pipe_num+1];
    char **redi_arr[pipe_num+1];
    vector<string> * redirect_arr = new vector<string>[pipe_num+1]();
    adv_parse(&cmd_vct, redirect_arr, argv_arr, redi_arr);
    /* Test of output */
    //test_output(redirect_arr, argv_arr, redi_arr);
    delete [] redirect_arr;
    /* Judge Exit & Build-in Command */
    int flag = exit_command(cmd_vct, argv_arr[0]);
    if(flag == 0) { 
      delete_process(argv_arr, redi_arr, redirect_arr, cmd_vct);
      return EXIT_SUCCESS;
    }
    if(flag == 2) { // enter  
      delete_process(argv_arr, redi_arr, redirect_arr, cmd_vct);
      continue; 
    }  
    int ifbuiltin = builtin_command(buf, &direct, argv_arr[0]);
    if(ifbuiltin == 1){
      delete_process(argv_arr, redi_arr, redirect_arr, cmd_vct);
      continue;
    }
    /***********************************************************************/
    /* Child & Parent Process */
    /* Fork Root */
    ppid = fork();
    if(ppid < 0) {
      perror("error occured on fork");
      return EXIT_FAILURE;
    }
    /* In Root Child Process */
    else if(ppid == 0) {
      /* Execute the Child Process */
      int ret = fork_loop(pid, pipe_fd, argv_arr, redi_arr);
      if(ret == -1 ) {
	//cout << "Error occured in fork loop" << endl;
	delete_process(argv_arr, redi_arr, redirect_arr, cmd_vct);
	return -1;
      }
      if(pipe_num != 0){
	close_pipes(pipe_fd);
      }
      //after the fork loop, because no exec function, you have to delete it 
      for(int i = 0; i < pipe_num+1; i++){
	wpid = waitpid(pid[i], &status, 0);
      }
      if(wpid == -1) {
	perror("error in waitpid");
	exit(EXIT_FAILURE);
      }      
      if(WIFEXITED(status)) {
	printf("Program exited with status %d\n", WEXITSTATUS(status));
      }
      else if(WIFSIGNALED(status)) {
	printf("Program was killed by signal %d\n", WTERMSIG(status));
      }
      else if (WIFSTOPPED(status)) {
	printf("stopped by signal %d\n", WSTOPSIG(status));
      } 
      else if (WIFCONTINUED(status)) {
	printf("continued\n");
      }
      close_pipes(pipe_fd);
      //after the fork loop, because no exec function, you have to delete it 
      //for(int i = 0; i < pipe_num+1; i++){
	//std::cout << "Waiting for " << pid[i] << "\n";
	//waitpid(pid[i], NULL, 0);
      //}
      delete_process(argv_arr, redi_arr, redirect_arr, cmd_vct);    
      //waitpid(pid[pipe_num], NULL, 0);
      //sleep(1);
      return 0;
    }
    /* In Parent Process */
    do{
      close_pipes(pipe_fd);
      wpid = waitpid(ppid, &status, 0);
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    //printf("WEXITSTATUS: %d\n", WEXITSTATUS(status));
    /* REMEMBER TO DELETE ARGV[]*/
    delete_process(argv_arr, redi_arr, redirect_arr, cmd_vct);
  } 
}

int fork_loop(int *pid, int pipe_fd[][2], char*** argv_arr, char*** argk_arr) {
  //int res_stdin = dup(STDIN_FILENO);
  //int res_stdout = dup(STDOUT_FILENO);
  for(int i = 0; i < pipe_num + 1; i++) {
    if((pid[i] = fork()) < 0) { perror("fork failed"); }
    /**********Child Process************/
    if(pid[i] == 0) {
      //std::cout << "pid " << getpid() << " will run " << argv_arr[i][0]<< "\n";
      //for the first command part
      if(i == 0){
	//cout << "i = 0" << endl;
	if(ifredirect != 0){
	  int ret = redirect_process(argk_arr[i]);
	  if(ret == -1) return -1;
	}
	if(pipe_num != 0){  // pipes exist
	  dup2(pipe_fd[0][1], STDOUT_FILENO);
	}
      }
      //for the last command part
      else if(i == pipe_num){
	//cout << "i = pipe_num" << endl;
	if(ifredirect != 0){
	  int ret = redirect_process(argk_arr[i]);
	  if(ret == -1) return -1;
	}
	if(pipe_num != 0){  // pipes exist
	  dup2(pipe_fd[i-1][0], STDIN_FILENO);
	}
      }
      //for intermediate command part
      else{
	//cout << "i = others" << endl;
	/* the commands in the middle, should not have redirect*/
	if(pipe_num != 0){  // pipes exist
	  dup2(pipe_fd[i-1][0], STDIN_FILENO);
	  dup2(pipe_fd[i][1], STDOUT_FILENO);
	}
      }
      //cout<<"Fork " << i <<" child process: ppid = "<< getppid() <<" pid =" <<getpid() << endl;
      if(pipe_num != 0){
	close_pipes(pipe_fd);
      }
      int ret = execvp(argv_arr[i][0], argv_arr[i]);
      if(ret == -1 ) {
	cout << "Command " << argv_arr[i][0] << " not found" << endl;
	return -1;
      }
    }
    /**********Parent Process************/
    cout.flush();
    //waitpid(pid[i], NULL, 0);
  }
  return 0;
}


void close_pipes(int pipe_fd[][2]){
  for(int i = 0; i < pipe_num; i++){
    for(int j = 0; j <= 1 ;j++){
      close(pipe_fd[i][j]);
    }
  }
}

int redirect_process(char ** argk){
  int if_have_two_redirect = 0;
  int res_stdin = dup(STDIN_FILENO);
  int res_stdout = dup(STDOUT_FILENO);
  if(argk[0] == NULL){ 
    dup2(res_stdout, STDOUT_FILENO);
    dup2(res_stdin, STDIN_FILENO);
    return 0; 
  }
  if(argk[0] != NULL) {
    int ret = redirect_judgement(argk, 0);
    if(ret == -1){ return -1; }
  }
  //if this command has two redirect
  if(argk[2] != NULL) {
    if_have_two_redirect = 1;
    int ret = redirect_judgement(argk, 2);
    if(ret == -1){ return -1; }
  }
  //the third redirect(include 2>)
  if(if_have_two_redirect == 1){
    if(argk[4] != NULL) {
      int ret = redirect_judgement(argk, 4);
      if(ret == -1){ return -1; }
    }
  } 
  return 0;
}
  
int redirect_judgement(char** argk, int cnt) {
  int fd;
  if(!strcmp(argk[cnt], "<")) {
    fd = open(argk[cnt+1], O_RDONLY);
    if(fd == -1) {
      perror("Error: fail to open the file");
      return -1;
    }
    if(dup2(fd, STDIN_FILENO) == -1){
      perror("Error: fail to redirect stdin");
      return -1;
    }
    if(close(fd) == -1){
      perror("Error: fail to close file\n");
      return -1;
    }
  }
  if(!strcmp(argk[cnt], ">")) {
    fd = open(argk[cnt+1], O_CREAT | O_RDWR, 0666);
    if(fd == -1){
      perror("Error: fail to open file\n");
      return -1;
    }
    if(dup2(fd, STDOUT_FILENO) == -1){
      perror("Error: fail to redirect stdout\n");
      return -1;
    }
    if(close(fd) == -1){
      perror("Error: fail to close file\n");
      return -1;
    }
  }
  if(!strcmp(argk[cnt], "2>")) {
    fd = open(argk[cnt+1], O_CREAT | O_RDWR, 0666);
    if(fd == -1){
      perror("Error: fail to open file\n");
      return -1;
    }
    if(dup2(fd, STDERR_FILENO) == -1){
      perror("Error: fail to redirect stdout\n");
      return -1;
    }
    if(close(fd) == -1){
      perror("Error: fail to close file\n");
      return -1;
    }
  }
  return 0;
}

void test_output(vector<string> *redirect_arr, char*** argv_arr, char*** redi_arr) {
  if(redirect_arr->empty() == false) {
    /* print redirect_arr */
    for(int i = 0; i < pipe_num+1; i++){
      cout << "redirect_arr[" << i << "] = " << endl;
      for(vector<string>::iterator it = redirect_arr[i].begin(); it != redirect_arr[i].end(); ++it){
	cout << *it << endl;
      }
    }
    /* print redi_arr */
    for(int i = 0; i < pipe_num+1; i++){
      cout<< "redi_arr[" << i << "] = " << endl;
      for(int j = 0; redi_arr[i][j] != NULL; j++){
	cout << redi_arr[i][j] << endl;
      }
    } 
  }
  /* print argv_arr */
  for(int i = 0; i < pipe_num+1; i++){
    cout<< "argv_arr[" << i << "] = " << endl;
    for(int j = 0; argv_arr[i][j] != NULL; j++){
      cout << argv_arr[i][j] << endl;
    }
  }
}
