#include <signal.h> //sigaction
#include <iostream>
#include <cstring>
#include <errno.h> //perror
#include <sys/types.h> //open
#include <sys/stat.h> //open
#include <fcntl.h> //open k flags
#include <unistd.h> //read, write
#include <fstream>
#include <sys/socket.h> //socket programming
#include <netinet/in.h> //socket programming
#include <arpa/inet.h> //to idio
#include "worker.h"
#include "utils.h"
#include "record.h"
#include "record_HT.h"
#include "cdHashTable.h"

int summary_entries =0;
int quitflag1 =0; //gia na kserw an tha grapsw log kai kleinw

void quit_hdl1(int signo){
  quitflag1=1; //gia na kserei sth megalh while ti tha kanei to paidi
}


//diabazei arxeio kai kanei populate tis domes (apo 1h ergasia oi perissoteres)
void parse_records_from_file(std::string filename, std::string date, std::string folder, record_HT * rht, diseaseHashTable * dht, countryHashTable *cht,file_summary* summ){
  std::ifstream infile(filename.c_str()); //diabasma apo tis grammes tou arxeiou
  std::string line; //EPITREPETAI H STRING EIPAN STO PIAZZA
  if(is_date_ok(date) == false) //an to date onoma arxeiou den einai hmeromhnia, asto
    return;

    //std::cout << " my file is " << filename << "\n";
  while (std::getline(infile, line)){
    //https://stackoverflow.com/questions/49201654/splitting-a-string-with-multiple-delimiters-in-c
    std::string const delims{ " \t\r\n" }; //delimiters einai ta: space,tab kai carriage return. TELOS.
    size_t beg, pos = 0;
    int params_count =0;
    std::string true_record_parts[8];
    true_record_parts[5] = "-"; //gia entry d
    true_record_parts[6] = "-"; //gia exit d
    std::string record_parts[6]; //mia thesi gia kathe melos tou record opws prpei na einai sto arxeio
    while ((beg = line.find_first_not_of(delims, pos)) != std::string::npos){
        pos = line.find_first_of(delims, beg + 1);
        record_parts[params_count] = line.substr(beg, pos - beg);
        params_count++;
    }//telos while eksagwghs gnwrismatwn apo grammh
    if(params_count != 6) //kati leipei/pleonazei, akurh h eggrafh!
      {std::cerr << "ERROR\n";continue;}
    //fernw thn eggrafh sth morfh ths ergasias 1 gia na einai apodotika kai eukolotera ta queries
    true_record_parts[0] = record_parts[0]; //id
    true_record_parts[1] = record_parts[2]; //first name
    true_record_parts[2] = record_parts[3]; //last name
    true_record_parts[3] = record_parts[4]; //disease
    true_record_parts[4] = folder; //country
    if(record_parts[1] == "ENTER")
      true_record_parts[5] = date; //entrydate to onoma tou arxeiou
    else if(record_parts[1] == "EXIT")
      true_record_parts[6] = date; //exitdate to onoma tou arxeiou
    else //kakh eggrafh, aporripsh k sunexeia
      {std::cerr << "ERROR\n";continue;}
    true_record_parts[7] = record_parts[5]; //age
    if(stoi(true_record_parts[7]) < 0)//arnhtiko age, proxwrame
      {std::cerr << "ERROR\n";continue;}
    record * new_rec_ptr = new record(true_record_parts); //dhmiourgia eggrafhs
    //std::cout << new_rec_ptr->get_recordID() << " " << new_rec_ptr->get_patientFirstName() << " " << new_rec_ptr->get_age() << " " << new_rec_ptr->get_entryDate()<< " " << new_rec_ptr->get_country() << "\n";
    //TO PERNAW STIS DOMES ME ELEGXO GIA EXIT AN YPARXEI KTL!!
    int parsed = rht->insert_record(new_rec_ptr);
    if(parsed < 0)
      {std::cerr << "ERROR\n";continue;} //den egine insert gt exei problhma, pame epomenh
    if(parsed != 3){ //an htan pragmati entelws nea eggrafh, kane insert. Diaforetika, epeidh ola ta insert ginontai me pointers, h enhmerwsh ths hmeromhnias sto recordHT arkei kai gia auta (giati deixnoun sthn eggrafh auth)
      dht->insert_record(new_rec_ptr);
      cht->insert_record(new_rec_ptr);
    }
    //PAME na perasoume thn plhroforia poy phrame sth domh summary
    if(summ->insert_data(true_record_parts) == 1)
      summary_entries += 1; //mphke kainourgia astheneia

  }
  //std::cout << "i was " << filename << " " << summ->diseasename <<" "<< summ->age_cats[0] << "\n";
}

int work(char * read_pipe, char * write_pipe, int bsize, int dosumms){
  //SIGNAL HANDLERS MOY gia SIGINT/SIGQUIT
  struct sigaction actquit;
  sigfillset(&(actquit.sa_mask)); //otan ekteleitai o handler thelw na blockarw ta panta
  actquit.sa_handler = quit_hdl1;
  sigaction(SIGINT, &actquit, NULL); //to orisame!
  sigaction(SIGQUIT, &actquit, NULL); //to orisame!

  int read_fd, write_fd;
  char sbuf[500];
  char jbuf[500];
  int n_dirs=0;
  int n_files=0;
  int tot_files=0; //gia to summary
  //oi domes moy. Enas aplos HT gia eggrafes kai oi HTs apo thn ergasia 1
  record_HT records_htable(50); //o DIKOS MOU HT gia tis eggrafes basei recordID megethous h1+h2. KALUTEROS APO APLH LISTA
  diseaseHashTable diseases_htable(25, 64); //O erg1 HT GIA DISEASE
  countryHashTable countries_htable(25, 64); //O erg1 HT GIA COUNTRY
  //anoigw ta pipes kai krataw fds
  read_fd = open(read_pipe, O_RDONLY );
  write_fd = open(write_pipe, O_WRONLY);


    //DIABAZW DIRECTORIES POY MOY EDWSE O GONIOS
    read(read_fd, &n_dirs, sizeof(int));
    directory_summary * dsums[n_dirs]; //gia ta summaries
    std::string * countries = new std::string[n_dirs]; //onomata xwrwn poy exw
    std::string * date_files = NULL; //tha mpoun ta filesnames-hmeromhnies
    for(int i=0; i<n_dirs; i++){
      n_files=0;
      receive_string(read_fd, &(countries[i]), bsize ); //pairnw prwta xwra
      receive_string(read_fd, sbuf, bsize ); //pairnw olo to path
      extract_files(sbuf, &n_files, &date_files); //pairnw plhrofories
      sort_files(date_files,0 ,n_files-1); //sort by date gia pio swsto parsing
      dsums[i] = new directory_summary(n_files, countries[i]);
      for(int j=0; j<n_files; j++){
        summary_entries=0; //gia na kserw ti tha pw sto gonio
        file_summary * mysum = new file_summary; //boh8htikh domh gia to summary poy tha stelnei meta apo kathe arxeio sto gonio
        strcpy(jbuf, "");
        sprintf(jbuf, "%s/%s",sbuf, (date_files[j]).c_str());
        parse_records_from_file(std::string(jbuf), date_files[j] ,countries[i], &records_htable, &diseases_htable, &countries_htable ,mysum);
        dsums[i]->filenames[j] = date_files[j];
        dsums[i]->nodes_per_file[j] = summary_entries;
        dsums[i]->tfile_sums[j] = mysum;
      }

      //std::cout << getpid() << " diabasa dir ap par " << sbuf << "\n";
      delete[] date_files; //sbhse to new poy egine
    }
    int serverPort =0;
    char serverIP[256];
    receive_string(read_fd, serverIP,bsize); //pare serverIP
    read(read_fd, &serverPort ,sizeof(int)); //pare serverPort
    //std::cout << "Iam child and i got" << serverIP << " " << serverPort << "\n";
    //records_htable.print_contents();
    //diseases_htable.print_contents();
    //countries_htable.print_contents();

    //Paw na ftiaksw socket gia server
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, serverIP, &(serv_addr.sin_addr)); //pare vale th dieu9unsh tou server
    serv_addr.sin_port = htons(serverPort); //Vazw to port tou orismatos Serverport
    int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(serv_sock < 0)
      {printf("socket error\n");return -1;}

    //thelw na parw ton arithmo portas poy tha steilw ston server
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
      {printf("socket error\n");return -1;}
    struct sockaddr_in m_addr;
    bzero((char *) &m_addr, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_ANY;
    m_addr.sin_port = 0; //VAZW TO PORT 0 GIA NA PAREI TUXAIO DIA8ESIMO
    bind(sock, (struct sockaddr *) &m_addr, sizeof(m_addr));
    //TWRA PAW NA VRW POIO PORT NUMBER EINAI AKRIBWS
    socklen_t len = sizeof(m_addr);
    getsockname(sock, (struct sockaddr *)&m_addr, &len);
    printf("port number %d kai omorfa : %d\n", m_addr.sin_port, htons(m_addr.sin_port));
    //printf("port number %d kai omorfa : %d\n", serv_addr.sin_port, htons(serv_addr.sin_port));

    int port_to_send = htons(m_addr.sin_port);
    //STELNW STO SERVER TA SUMMARY STATISTICS
    if(connect(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\nConnection Failed. Server may not be up \n");
        return -1;
    }
    send_string(serv_sock, "melitzana!" ,bsize); //dokimh

    /*
  if(dosumms==0) //an eisai paidi poy eftiakse o gonios apo sigchld mhn kaneis ksanasumms
    for(int i=0; i<n_dirs; i++){
      send_integer(serv_sock, &(dsums[i]->nfiles));
      //send_string(write_fd,  ,bsize)
      for(int j=0; j<dsums[i]->nfiles; j++){
        send_file_summary(write_fd, dsums[i]->nodes_per_file[j], dsums[i]->filenames[j], dsums[i]->countryname, dsums[i]->tfile_sums[j], bsize);
      }
      //std::cout << "\n";
    }
    */


    for(int i=0; i<n_dirs; i++)
      delete dsums[i]; //ME DESTRUCTORS THS C++ OLH H DESMEUMENH KATHARIZEII, des ~directory_summary
    //enhmerwnw gonio oti teleiwsa to parsing
    send_string(write_fd, "ok", bsize);

  strcpy(sbuf, "");
  char sbuf2[200];
  strcpy(sbuf2, "");
  std::string tool;

  //arxizw na pairnw entols xrhsth
  int successful = 0; //epituxh erwthmata
  int failed = 0; //apotuxhmena erwthmata
  while(1){

    if(quitflag1 > 0) //fagame SIGINT/QUIT
      {/*std::cout << "ciao\n";*/break;}

    int rdb = receive_string(read_fd, &tool, bsize);
    while(tool == ""){

      rdb = receive_string(read_fd, &tool, bsize);
    }

    if(quitflag1 > 0) //fagame SIGINT/QUIT
      {/*std::cout << "ciao\n";*/break;}

    if(tool == "/exit"){
          //isws cleanup??
      break;
    }
    else if(tool == "bad"){
      failed++;//apotyxhmeno erwthma
    }
    else if(tool == "/diseaseFrequency1"){ //xwris orisma country
      std::string dis_name;
      rdb = receive_string(read_fd, &dis_name, bsize); //diabase astheneia
      std::string date1;
      rdb = receive_string(read_fd, &date1, bsize); //diabase date1
      std::string date2;
      rdb = receive_string(read_fd, &date2, bsize); //diabase date2
      int number_to_present = diseases_htable.total_recs_for_cat(dis_name, date1, date2);
      //std::cout << dis_name << " ^ " << number_to_present << "\n";
      write(write_fd, &number_to_present, sizeof(int)); //tou stelnw to zhtoumeno noumero
      successful++;//epituxia
    }//telos if diseaseFrequency1
    else if(tool == "/diseaseFrequency2"){ //ME orisma country
      std::string dis_name;
      rdb = receive_string(read_fd, &dis_name, bsize); //diabase astheneia
      std::string date1;
      rdb = receive_string(read_fd, &date1, bsize); //diabase date1
      std::string date2;
      rdb = receive_string(read_fd, &date2, bsize); //diabase date2
      std::string country;
      rdb = receive_string(read_fd, &country, bsize); //diabase date2
      int number_to_present = diseases_htable.total_recs_for_cat(dis_name, date1, date2, country);
      //std::cout << dis_name << " ^ " << number_to_present << "\n";
      write(write_fd, &number_to_present, sizeof(int)); //tou stelnw to zhtoumeno noumero
      successful++; //epituxia
    }//telos if diseaseFrequency2
    else if(tool == "/listCountries"){
      write(write_fd, &n_dirs, sizeof(int)); //stelnw sto gonio poses xwres tha exw
      for(int i=0; i<n_dirs; i++){
        std::string countryandme = countries[i] + " " + std::to_string(getpid());
        send_string(write_fd, &countryandme, bsize);
      }
      successful++;//epituxia
    }//telos if listCountries
    else if(tool == "/searchPatientRecord"){
      std::string id_to_look_for = "";
      record * recptr = NULL; //gia ton entopismo eggrafhs
      rdb = receive_string(read_fd, &id_to_look_for, bsize); //diabase to zhtoumeno id
      recptr = records_htable.searchPatientRecord(id_to_look_for); //psaksto
      if(recptr != NULL){ //an to brhke
        std::string requested_rec = "";
        if(recptr->get_exitDate() == "-") //den exoume bgei akoma, h ekfwnhs den kserw giati thelei 2 paules anti gia 1 alla ok
          requested_rec = recptr->get_recordID() + " " + recptr->get_patientFirstName() + " " + recptr->get_patientLastName() + " " + recptr->get_diseaseID() + " " + std::to_string(recptr->get_age()) + " " + recptr->get_entryDate() + " --" ;
        else //exei kanoniko exitdate
          requested_rec = recptr->get_recordID() + " " + recptr->get_patientFirstName() + " " + recptr->get_patientLastName() + " " + recptr->get_diseaseID() + " " + std::to_string(recptr->get_age()) + " " + recptr->get_entryDate() + " " + recptr->get_exitDate() ;
        send_string(write_fd, &requested_rec, bsize); //grapsto
      }
      else //den to brhke
        send_string(write_fd, "nope", bsize); //grapse oti de brhkes tpt
      successful++;//epituxia
    }//telos if searchPatientRecord
    else if(tool == "/topk-AgeRanges"){
      int kapa = 0;
      read(read_fd, &kapa, sizeof(int)); //pare timh k
      std::string country;
      rdb = receive_string(read_fd, &country, bsize); //pare country
      std::string disease;
      rdb = receive_string(read_fd, &disease, bsize); //pare disease
      std::string date1;
      rdb = receive_string(read_fd, &date1, bsize); //diabase date1
      std::string date2;
      rdb = receive_string(read_fd, &date2, bsize); //diabase date2
      int fetched=0;
      int * resul_arr = new int[kapa]; //me boh8aei na perasw ston patera ta apotelesmata
      float * fresul_arr = new float[kapa]; //gia ta pososta
      countries_htable.topk_age_ranges(kapa, country, disease, date1, date2, &fetched, resul_arr, fresul_arr);
      deliver_topk(write_fd, fetched, resul_arr, fresul_arr); //steile apotelesmata ston patera
      delete[] resul_arr;
      delete[] fresul_arr;
    }//telos topk
    else if(tool == "/numPatientAdmissions1"){ //xwris country
      std::string dis_name;
      rdb = receive_string(read_fd, &dis_name, bsize); //diabase astheneia
      std::string date1;
      rdb = receive_string(read_fd, &date1, bsize); //diabase date1
      std::string date2;
      rdb = receive_string(read_fd, &date2, bsize); //diabase date2
      int * country_admissions = new int[n_dirs];
      for(int i=0; i<n_dirs; i++)//bres gia auth th xwra
        country_admissions[i] = diseases_htable.admissions(dis_name, date1, date2, countries[i]);
      //ta stelnw ektos loop gia na mhn ka8usteroun oi workers kai na douleuoun aneksarthta
      deliver_num_adms_disch1(write_fd, n_dirs, countries, country_admissions , bsize);
      delete[] country_admissions;
      successful++;//epituxia
    }//telos numPatientAdmissions1
    else if(tool == "/numPatientAdmissions2"){
      std::string dis_name;
      rdb = receive_string(read_fd, &dis_name, bsize); //diabase astheneia
      std::string date1;
      rdb = receive_string(read_fd, &date1, bsize); //diabase date1
      std::string date2;
      rdb = receive_string(read_fd, &date2, bsize); //diabase date2
      std::string country;
      rdb = receive_string(read_fd, &country, bsize); //diabase date2
      int number_to_present = diseases_htable.admissions(dis_name, date1, date2, country);
      //std::cout << dis_name << " ^ " << number_to_present << "\n";
      write(write_fd, &number_to_present, sizeof(int)); //tou stelnw to zhtoumeno noumero
      successful++; //epituxia
    }//telos numPatientAdmissions2
    else if(tool == "/numPatientDischarges1"){ //xwris country
      std::string dis_name;
      rdb = receive_string(read_fd, &dis_name, bsize); //diabase astheneia
      std::string date1;
      rdb = receive_string(read_fd, &date1, bsize); //diabase date1
      std::string date2;
      rdb = receive_string(read_fd, &date2, bsize); //diabase date2
      int * country_disch = new int[n_dirs];
      for(int i=0; i<n_dirs; i++)//bres gia auth th xwra
        country_disch[i] = diseases_htable.discharges(dis_name, date1, date2, countries[i]);
      //ta stelnw ektos loop gia na mhn ka8usteroun oi workers kai na douleuoun aneksarthta
      deliver_num_adms_disch1(write_fd, n_dirs, countries, country_disch , bsize);
      delete[] country_disch;
      successful++;//epituxia
    }//telos numPatientDischarges1
    else if(tool == "/numPatientDischarges2"){
      std::string dis_name;
      rdb = receive_string(read_fd, &dis_name, bsize); //diabase astheneia
      std::string date1;
      rdb = receive_string(read_fd, &date1, bsize); //diabase date1
      std::string date2;
      rdb = receive_string(read_fd, &date2, bsize); //diabase date2
      std::string country;
      rdb = receive_string(read_fd, &country, bsize); //diabase date2
      int number_to_present = diseases_htable.discharges(dis_name, date1, date2, country);
      //std::cout << dis_name << " ^ " << number_to_present << "\n";
      write(write_fd, &number_to_present, sizeof(int)); //tou stelnw to zhtoumeno noumero
      successful++; //epituxia
    }//telos numPatientDischarges2
    else{
      ;;//std::cout << "diabas apo gonio "<< tool << getpid() <<"\n";
    }

    if(quitflag1 > 0) //fagame SIGINT/QUIT
      {/*std::cout << "ciao\n";*/break;}

  }

  delete[] countries; //svhse to new poy egine
  close(read_fd);
  close(write_fd);


  exit(0);


}

//stelnei ston patera apotelesmata topk
void deliver_topk(int wfd, int fetchd, int * res_arr, float * fres_arr){
  write(wfd, &fetchd, sizeof(int)); //enhmerwse ton patera na kserei ti na perimenei na diabasei
  if(fetchd == 0) //tipota
    return;

  for(int i=0; i< fetchd; i++){
    write(wfd, &res_arr[i], sizeof(int)); //hlikiakh kathgoria
    write(wfd, &fres_arr[i], sizeof(float)); //pososto krousmatwn ths

  }
}

//stelnw apotelesmata sthn 1h periptwsh ths numadmissions
void deliver_num_adms_disch1(int wfd, int ncountries ,void * stptr, int * admis , int bsize){
  write(wfd, &ncountries, sizeof(int));
  for(int i=0; i<ncountries; i++){
    //stelnw xwra
    send_string(wfd, &(((std::string *)stptr)[i]), bsize );
    //stelnw timh
    write(wfd, &admis[i], sizeof(int));
  }
}
