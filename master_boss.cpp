#include <signal.h> //sigaction
#include <iostream>
#include <cstring>
#include <errno.h> //perror
#include <sys/types.h> //open
#include <sys/stat.h> //open
#include <sys/wait.h> //waitpid
#include <fcntl.h> //open k flags
#include <unistd.h> //read, write
#include "master_boss.h"
#include "worker.h"
#include "utils.h"

int glob_counter=0; //gia na perimenei oti ola ta paidia einai ok

int quitflag =0; //gia na kserw an tha grapsw log kai kleinw

void quit_hdl(int signo){
  quitflag=1; //gia na kserei sth megalh while ti tha kanei to paidi
}

int child_death =0; //gia xeirismo SIGCHLD
int deadpid = -1;

void chld_hdl(int signo){
  signal(SIGCHLD, chld_hdl);//ksanakane handle
  child_death=1; //flag gia na kserei ti na kanei meta
  pid_t p;
  int status;
    //eipw8hke piazza oti de tha pethanoyn 2 tautoxrona
    while(1){
      p=waitpid(-1, &status, WNOHANG);
      if(p <= 0)
        break;
      deadpid = p; //trackdown poio pethane
    }
}//telos sigchld handler


//sunarthsh poy kanei ROUND-ROBIN share ta directories sta paidia-workers
int share_dirs(int *dpw, int ndirs, int ws){
  int share=ndirs/ws; //posa tha parei toulaxiston kathe paidi
  for(int i=0; i<ws; i++)
    dpw[i] = share;
  share = ndirs % ws; //posa meinane
  for(int i=0; i<share; i++)
    dpw[i] += 1; //round robin ena o kathenas twra
  return 0;
}



int administrate(char * in_dir, int wnum, int bsize, std::string * pipe_names, int * pids){
  //SIGNAL HANDLERS MOY gia SIGINT/SIGQUIT
  struct sigaction actquit;
  sigfillset(&(actquit.sa_mask)); //otan ekteleitai o handler thelw na blockarw ta panta
  actquit.sa_handler = quit_hdl;
  sigaction(SIGINT, &actquit, NULL); //to orisame!
  sigaction(SIGQUIT, &actquit, NULL); //to orisame!

  char abuf[300]; //ergaleio gia reading apo pipes ktl
  std::string * subdirs = NULL; //tha mpoun ta subdir names
  int * dirs_per_wrk = new int[wnum](); //gia na dw an teleiwse me tous katalogous gia to i paidi, initialized to 0
  int dirs_n=0;
  extract_files(in_dir, &dirs_n, &subdirs); //euresh dirs
  share_dirs(dirs_per_wrk, dirs_n, wnum); //katanomh dirs
  std::string dirsofeach[wnum][dirs_n]; //xwres kathe paidiou gia log
  struct pollfd pipe_rfds[wnum]; //ta read fds twn pipes poy tha mpoyn kai sthn poll
  struct pollfd pipe_wfds[wnum]; //ta write fds antistoixa, auta mallon de tha xreiastoun poll giati ta paidia diabazoun mono apo enan
  //anoigw ta pipes kai krataw tous fds tous
  for(int i=0; i< wnum; i++){
    pipe_wfds[i].fd = open(pipe_names[2*i +1].c_str(), O_WRONLY ); //anoigma kathe pipe pros ta paidia gia grapsimo
    pipe_rfds[i].fd = open(pipe_names[2*i].c_str(), O_RDONLY );
  }


  if(dirs_n == 0) //lathos
    {std::cout << "empty dir!!\n"; delete[] subdirs;return 1;}
  //ROUND-ROBIN KATANOMH YPO-KATALOGWN-XWRWN
  int dirs_writ = 0;
  for(int i=0; i<wnum; i++){
    write(pipe_wfds[i].fd, &(dirs_per_wrk[i]), sizeof(int)); //tou eipame oti diabazei teleutaia fora
    //std::cout << dirs_per_wrk[i];
    for(int j=0; j< dirs_per_wrk[i]; j++){
      sprintf(abuf, "%s/%s", in_dir,(subdirs[dirs_writ]).c_str() ); //pairnw to dir_name kai to bazw mazi me to inputdir (ftiaxnw path)
      //std::cout << "ok";
      send_string(pipe_wfds[i].fd, &(subdirs[dirs_writ]), bsize); //steile onoma xwras sketo
      send_string(pipe_wfds[i].fd, abuf, bsize); //steile to path
      dirsofeach[i][j] = subdirs[dirs_writ]; //to krataw gia log
      dirs_writ++;
    }

  }//telos for gia moirasma directories

  std::string tool;
  int kids_read =0;

  //KANW POLL GIA SUMMARIES!! Etsi mporw na diabazw prwta ta summaries twn paidiwn poy exoun teleiwsei
  //meta to kathe summary sigourepsou mesw named pipe oti to i paidi teleiwse
  int already_read[wnum]; //mh diabaseis ksana to idio paidi
  int already_ok[wnum]; //twra gia na parei kai to mhyma oti to paidi teleiwse
  memset(already_read, 0, sizeof(already_read)); // arxika ola adiabasta
  memset(already_ok, 0, sizeof(already_read)); // arxika ola adiabasta

  while(kids_read < wnum){
    //arxikopoihsh se kathe loupa gia thn poll
      reset_poll_parameters(pipe_rfds, wnum);

    int rc = poll(pipe_rfds, wnum, 2000); //kanw poll
    if(rc == 0)
      {;;/*std::cout << "timeout\n";*/}
    else{//tsekarw poioi einai etoimoi
      for(int i=0; i<wnum; i++){
        //an einai etoimo kai den to exw ksanadiabasei
        if(pipe_rfds[i].revents == POLLIN){
          if(already_read[i] == 0){ //pame gia ta summaries
            for(int j=0; j< dirs_per_wrk[i]; j++){
              int nfls =0;
              read(pipe_rfds[i].fd, &nfls, sizeof(int));
              for(int k=0; k<nfls; k++)
                receive_and_print_file_summary(pipe_rfds[i].fd, bsize); //ektupwse to summary
            }
            already_read[i] = 1;
          }
          //tsekarw kai to oti to paidi teleiwse genika
          if(already_ok[i] == 0){
            receive_string(pipe_rfds[i].fd, &tool, bsize);
            if(tool == "ok") //teleiwse to parsing to paidi
              {kids_read++; already_ok[i] = 1;}
          }

        } //telos elegxou diathesimothtas fd
      }//telos for gia paidia
    } //telos else gia timeout ths poll
  }//telos while


  //GIA XEIRISMO SIGCHLD
  signal(SIGCHLD, chld_hdl);

  if(kids_read ==wnum)
    std::cout << "Children completed parsing!\n";

  //o master paramenei gia na prosexei aplws an pethane kapoio paidi
  while(1){

    if(deadpid >0){ //fagame SIGCHLD, ftiaxnw neo child me ta dirs tou nekrou
      //std::cout << "dwstouuuu\n";
      int deadindex = -1;
      for(int i=0; i< wnum; i++)
        if(pids[i] == deadpid)
          {deadindex = i; break;} //vrhka ti prepei na steilw sto neo paidi na kserei
      int newpid = fork(); //ftiakse neo paidi!!
      if(newpid >0){ //gonios
        pids[deadindex] = newpid; //pairnei th 8esh otu paliou
        //tou metaferei ta dirs tou paliou, DE THA MAS DWSEI KSANA TO IDIO SUMMARY
        pipe_wfds[deadindex].fd = open(pipe_names[2*deadindex +1].c_str(), O_WRONLY);
        write(pipe_wfds[deadindex].fd, &(dirs_per_wrk[deadindex]), sizeof(int)); //tou eipame oti diabazei teleutaia fora
        for(int i=0; i< dirs_per_wrk[deadindex]; i++){
          sprintf(abuf, "%s/%s", in_dir,(dirsofeach[deadindex][i]).c_str() ); //pairnw to dir_name kai to bazw mazi me to inputdir (ftiaxnw path)
          send_string(pipe_wfds[deadindex].fd, &(dirsofeach[deadindex][i]), bsize); //steile onoma xwras sketo
          send_string(pipe_wfds[deadindex].fd, abuf, bsize); //steile to path
        } //telos for poy paradidei ta directories toy paliou paidiou sto neo paidi
        std::string isnewguyok ="";
        //perimenw ok mesw blocking pipe apo to neo paidi oti parsare
        pipe_rfds[deadindex].fd = open(pipe_names[2*deadindex].c_str(), O_RDONLY);
        receive_string(pipe_rfds[deadindex].fd, &isnewguyok, bsize);
        if(isnewguyok == "ok")
          std::cout << "New worker replaced dead one. Resume orders.\n"; //ola kala!
      }//telos if gonios sto sigchld
      else if(newpid == 0){ //neo paidi
        char rpp[100];
        char wpp[100];
        strcpy(rpp, pipe_names[2*deadindex +1].c_str()); strcpy(wpp, pipe_names[2*deadindex ].c_str());
        work(rpp, wpp, bsize, 1);
        //return 0;
      }
      else
        perror("fork fail: ");
      deadpid =-1;
    } //telos if xeirismou SIGCHLD
    //eipw8hke apo ton ko. Ntoula oti o master termatizei me ctrl-C (SIGINT)
    if(quitflag > 0) //fagame SIGINT/QUIT
      break;

  }//telos while poy diabazei entoles

  delete[] dirs_per_wrk;
  delete[] subdirs; //apodesmeush axreiastou pleon pinaka

  //kleise ta pipes
  for(int i=0; i<wnum; i++){
    close(pipe_rfds[i].fd);
    close(pipe_wfds[i].fd);
  }

//telos
  return 0;
}

//pare kai parousiase ta apotelesmata topk apo ena pipe paidiou
//GIA THN EKTYPWSH POSOSTWN EVALA 0 DEKADIKA PSHFIA GIATI ETSI EINAI STHN EKFWNHSH
void read_and_present_topk(int rfd){
  int fetched=0;
  read(rfd, &fetched, sizeof(int));
  if(fetched ==0) //to paidi auto den exei tpt. mh sunexiseis
    return;

  int age_cat;
  float pososto;
  std::string onoma_kat = "";
  //diabazw ta topk tou paidiou (mono ena paidi tha einai)
  for(int i=0; i< fetched; i++){
    read(rfd, &age_cat, sizeof(int)); //pare omada hlikias
    read(rfd, &pososto, sizeof(float)); //pare pososto
    if(age_cat == 0)
      printf("0-20: %.0f%\n",pososto*100);
    else if(age_cat == 1)
      printf("21-40: %.0f%\n",pososto*100);
    else if(age_cat == 2)
      printf("41-60: %.0f%\n",pososto*100);
    else
      printf("60+: %.0f%\n",pososto*100);
  }
}

//pare kai parousiase ta apotelesmata topk apo ena pipe paidiou
void read_and_present_num_adms_disch(int rfd, int bsize){
  int nc =0;
  int adms=0;
  read(rfd, &nc, sizeof(int));
  std::string cname;
  for(int i=0; i< nc; i++){
    //pare onoma xwras
    receive_string(rfd, &cname, bsize);
    //pare timh
    read(rfd, &adms, sizeof(int));
    std::cout << cname << " " << adms << "\n";
  }

}
