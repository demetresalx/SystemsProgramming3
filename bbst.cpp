#include "bbst.h"

//GIA TO DENTRO
BBST::BBST(){
  date = NULL;
  list_of_records = NULL;
  left_child = NULL;
  right_child = NULL;
}

//constructor basei eggrafhs
BBST::BBST(record * rec){
  date = new std::string(rec->get_entryDate());
  list_of_records = new reclist(rec);

  left_child = NULL;
  right_child = NULL;
}

//SHMANTIKO. DIABASE TA SXOLIA MOY SE AUTON TON DESTRUCTOR
BBST::~BBST(){
  delete date; //apodesmeush tou xwrou gia to string poy eixame
  delete list_of_records; //svhnei th lista me idio tropo giati kathe komvos ths exei destructor
  delete left_child; //Ean gia ena antikeimeno exei oristei destructor, H DELETE THA TON KALESEI
  delete right_child; //auto shmainei oti kathe paidi tha kalei tautoxrona tous destructors twn paidiwn tou kai olo to dentro apodesmeuetai epituxws
  //h eggrafh tha apodesmeutei apo th domh moy (aplos HT) opoy  kai anhkei
}


//eisagei eggrafh sto dentro. den exoume data duplication afou milame gia deikth sthn arxikh eggrafh poy brisketai sth dikh moy domh aplou HT
int BBST::insert_record(record* rec){
  //std::cout << dates_compare(rec->get_entryDate() , *date);
  if(dates_compare(rec->get_entryDate() , *date) == "equal"){ //ean h hmeromhnia ths eggrafhs einai ish me tou komvou
    //std::cout << "brhka idio date";
    list_of_records->insert_record(rec); //valth sth lista tou komvou
    return 0;
  }//telos if gia hmeromhnia
  else if(dates_compare(rec->get_entryDate() , *date) == "bigger"){ //ean h hmeromhnia ths eggrafhs einai megaluterh ap tou komvou
    if(right_child == NULL){ //prepei na pame deksia kai den yparxei deksi paidi. to ftiaxnoume k paei ekei
      right_child = new BBST(rec);
      return 0;
    }
    else //yparxei deksi paidi, eksetazoume apo kei kai ustera
      right_child->insert_record(rec);

  }//telos else if gia hmeromhnia
  else{ //hmeromhnia eggrafhs mikroterh tou komvou
    if(left_child == NULL){ //prepei na pame aristera kai den yparxei deksi paidi. to ftiaxnoume k paei ekei
      left_child = new BBST(rec);
      return 0;
    }
    else //yparxei aristero paidi, eksetazoume apo kei kai ustera
      left_child->insert_record(rec);
  }//telos else gia hmeromhnia

}//telos sunarthshs

//arwgos sunarthsh
void BBST::collect_dated_reclists(BBST* root,std::string date2, search_containter * sc ){
  if(root == NULL)
    return; //xtyphsame akrh
  std::string resu = dates_compare(*(root->date), date2); //sugkrish hmeromhnias entrydate komvou me date2
  if(resu == "smaller"){ //ean h hmeromhnia twn eggrafwn einai mikroterh ths date2
    sc->insert_reclist(root->list_of_records);
    collect_dated_reclists(root->left_child, date2, sc);
    collect_dated_reclists(root->right_child, date2, sc);
  }
  else if(resu == "equal"){ //an einai iso, den exei nohma na paei deksia
    sc->insert_reclist(root->list_of_records);
    collect_dated_reclists(root->left_child, date2, sc);
  }
  else if(resu == "bigger"){
    collect_dated_reclists(root->left_child, date2, sc);
  }
  return;
}

//arwgos sunarthsh gia discharge
void BBST::collect_all_reclists(BBST* root, search_containter * sc ){
  if(root == NULL)
    return; //xtyphsame akrh
  sc->insert_reclist(root->list_of_records);
  collect_all_reclists(root->left_child, sc);
  collect_all_reclists(root->right_child, sc);
  return;
}



//gemizei ton simple epikouriko ht gia to topk
void BBST::populate_simpleht(simple_cd_HT * htptr){
  if(list_of_records == NULL)
    return; //de tha exoun krousmata oute ta paidia an den exei o gonios
  else{ //exoume krousmata
    reclist * currptr = list_of_records;
    htptr->insert_krousma(list_of_records->recptr);
    currptr = list_of_records->next;
    while(currptr!= NULL){ //gia na piasei ola ta krousmata
      htptr->insert_krousma(currptr->recptr);
      currptr = currptr->next;
    }//telos while gia reclist
  }//telos else an exoume krousmata
  if(left_child != NULL)//yparxei aristeropaidi
    left_child->populate_simpleht(htptr);
  if(right_child != NULL) //yparxei deksi paidi
    right_child->populate_simpleht(htptr);

  return;
}//telos sunarthshs

//dikh moy bohthhtikh ektypwsh
void BBST::print_contents(){
  std::cout << "eimai komvos dentrou, exw date: " << *date << "\n";
  list_of_records->print_contents();
  if(left_child != NULL)
    left_child->print_contents();
  if(right_child != NULL)
    right_child->print_contents();
}




//komvos listas gia diplotypes hmeromhnies
reclist::reclist(){
  recptr = NULL;
  next = NULL;
}

reclist::reclist(record * rec){
  //std::cout << "yparxww kai bazww " << rec->get_patientFirstName();
  recptr = rec;
  next = NULL;
}

//destructor. H DELETE STHN C++ KALEI TOUS DESTRUCTORS ARA ME THN KLHSH THS DELETE GIA NEXT PAEI SE OLH TH LISTA
reclist::~reclist(){
  delete next;
}

//bazei eggrafh sth lista
int reclist::insert_record(record * rec){
  reclist * currptr = this;
  while(currptr->next != NULL) //paei sto teleutaio
    currptr = currptr->next ;
  currptr->next = new reclist(rec);

  return 0;
}

void reclist::print_contents(){
  std::cout << "\t eimai h lista ston komvo t dentrou kai exw ta: ";
  reclist * currptr = this;
  while(currptr != NULL){ //paei mexri to telos
    std::cout << currptr->recptr->get_patientFirstName() << currptr->recptr->get_exitDate() << "# ";
    currptr = currptr->next ;
  }//telos while gia next
  std::cout << "\n";
}



//GIA THN ARWGO KLASH DATE1 DATE2 ERWTHMATWN
search_containter::search_containter(){
  index =0;
  arr = NULL;
}

search_containter::search_containter(int maxsize){
  index =0;
  arr = new reclist *[maxsize]{NULL}; //arxika oloi oi deiktes null
}

search_containter::~search_containter(){
  delete[] arr; //mono to desmeumeno xwro. To periexomeno twn deiktwn prepei na meinei athikto
}

void search_containter::insert_reclist(reclist * rl){
  arr[index] = rl;
  index++;
}

//O,TI EINAI MESA STON CONTAINER EINAI ENTRYDATE <= DATE2. KOITAZEI GIA EXITDATE > DATE1
int search_containter::count_exit_limit(std::string date1){
  int num_approved =0;
  for(unsigned int i=0; i<index; i++){
    if(arr[i] == NULL) //oi eisagwges ginontai h mia meta thn allh ksekinwntas apo to 0. to prwto null shmainei den exei alles
      return num_approved;
    reclist * currptr = arr[i];
    while(currptr != NULL){
      if(currptr->recptr->get_exitDate() == "-") //einai akoma mesa. ton theloyme
        num_approved++;
      else if(dates_compare(currptr->recptr->get_exitDate(), date1) == "bigger" ){
        num_approved++; //exei exitdate megalutero tou date1, to theloyme
      }
      else if(dates_compare(currptr->recptr->get_exitDate(), date1) == "equal" ){
        if(dates_compare(currptr->recptr->get_entryDate(), date1) == "equal" )
          num_approved++; //vazei kai autous poy mphkan kai bghkan thn idia mera an to exitdate einai iso me date1
      }
      else
        num_approved += 0;
      currptr = currptr->next;
    }//telos while gia lista eggrafwn
  }//telos for gia thn i lista
  return num_approved;
}//telos sunarthshs

//O,TI EINAI MESA STON CONTAINER EXEI ENTRYDATE <= DATE2. KOITAZEI GIA ENTRYDATE >= DATE1 TWRA
int search_containter::count_entry_limit(std::string date1){
  int num_approved =0;
  for(unsigned int i=0; i<index; i++){
    if(arr[i] == NULL) //oi eisagwges ginontai h mia meta thn allh ksekinwntas apo to 0. to prwto null shmainei den exei alles
      return num_approved;
    reclist * currptr = arr[i];
    while(currptr != NULL){

      if(dates_compare(currptr->recptr->get_entryDate(), date1) == "bigger" ){
        num_approved++; //exei entry megalutero tou date1, to theloyme
      }
      else if(dates_compare(currptr->recptr->get_entryDate(), date1) == "equal" ){
        num_approved++; //vazei kai autous an to entrydate einai iso me date1
      }
      else
        num_approved += 0;
      currptr = currptr->next;
    }//telos while gia lista eggrafwn
  }//telos for gia thn i lista
  return num_approved;
}//telos sunarthshs

//idia me apo panw aplws elegxei k country
int search_containter::count_entry_limit(std::string date1, std::string country){
  int num_approved =0;
  for(unsigned int i=0; i<index; i++){
    if(arr[i] == NULL) //oi eisagwges ginontai h mia meta thn allh ksekinwntas apo to 0. to prwto null shmainei den exei alles
      return num_approved;
    reclist * currptr = arr[i];
    while(currptr != NULL){

      if(dates_compare(currptr->recptr->get_entryDate(), date1) == "bigger" ){
        if(currptr->recptr->get_country() == country) //tsekarw k country
          num_approved++; //exei entry megalutero tou date1, to theloyme
      }
      else if(dates_compare(currptr->recptr->get_entryDate(), date1) == "equal" ){
        if(currptr->recptr->get_country() == country) //tsekarw k country
          num_approved++; //vazei kai autous an to entrydate einai iso me date1
      }
      else
        num_approved += 0;
      currptr = currptr->next;
    }//telos while gia lista eggrafwn
  }//telos for gia thn i lista
  return num_approved;
}//telos sunarthshs

//O,TI EINAI MESA STON CONTAINER EINAI ENTRYDATE <= DATE2. KOITAZEI GIA EXITDATE > DATE1 KAI EPIPLEON COUNTRY GIA THN DISEASEFREQUENCY
int search_containter::count_exit_limit(std::string date1, std::string country){
  int num_approved =0;
  for(unsigned int i=0; i<index; i++){
    if(arr[i] == NULL) //oi eisagwges ginontai h mia meta thn allh ksekinwntas apo to 0. to prwto null shmainei den exei alles
      return num_approved;
    reclist * currptr = arr[i];
    while(currptr != NULL){
      if((currptr->recptr->get_exitDate() == "-") && (currptr->recptr->get_country() == country)) //einai akoma mesa kai exei th xwra mas. ton theloyme
        num_approved++;
      else if((dates_compare(currptr->recptr->get_exitDate(), date1) == "bigger" )&& (currptr->recptr->get_country() == country)){ //koitame kai xwra pleon
        num_approved++; //exei exitdate megalutero tou date1, to theloyme
      }
      else if((dates_compare(currptr->recptr->get_exitDate(), date1) == "equal" ) && (currptr->recptr->get_country() == country)){
        if(dates_compare(currptr->recptr->get_entryDate(), date1) == "equal" )
          num_approved++; //vazei kai autous poy mphkan kai bghkan thn idia mera an to exitdate einai iso me date1
      }
      else
        num_approved += 0;
      currptr = currptr->next;
    }//telos while gia lista eggrafwn
  }//telos for gia thn i lista
  return num_approved;
}//telos sunarthshs

int search_containter::count_exit_limit2(std::string date1, std::string date2, std::string country){
  int num_approved =0;
  for(unsigned int i=0; i<index; i++){
    if(arr[i] == NULL) //oi eisagwges ginontai h mia meta thn allh ksekinwntas apo to 0. to prwto null shmainei den exei alles
      return num_approved;
    reclist * currptr = arr[i];
    while(currptr != NULL){

      if((dates_compare(currptr->recptr->get_exitDate(), date1) == "bigger" )&& (currptr->recptr->get_country() == country)){ //koitame kai xwra pleon
        if((dates_compare(currptr->recptr->get_exitDate(), date2) == "smaller" )||(dates_compare(currptr->recptr->get_exitDate(), date2) == "equal" ))
          num_approved++; //exei exitdate megalutero tou date1, to theloyme
      }
      else if((dates_compare(currptr->recptr->get_exitDate(), date1) == "equal" ) && (currptr->recptr->get_country() == country)){
        if((dates_compare(currptr->recptr->get_exitDate(), date2) == "smaller" )||(dates_compare(currptr->recptr->get_exitDate(), date2) == "equal" ))
          num_approved++; //vazei kai autous poy mphkan kai bghkan thn idia mera an to exitdate einai iso me date1
      }
      else
        num_approved += 0;
      currptr = currptr->next;
    }//telos while gia lista eggrafwn
  }//telos for gia thn i lista
  return num_approved;
}//telos sunarthshs

//disqualify tis exitdate <= date1 kai eisodos se simpleht gia topk
void search_containter::populate_simpleht(simple_cd_HT *htptr, std::string date1){
  int num_approved =0;
  for(unsigned int i=0; i<index; i++){
    if(arr[i] == NULL) //oi eisagwges ginontai h mia meta thn allh ksekinwntas apo to 0. to prwto null shmainei den exei alles
      return;
    reclist * currptr = arr[i];
    while(currptr != NULL){
      if(currptr->recptr->get_exitDate() == "-") //einai akoma mesa. ton theloyme
        htptr->insert_krousma(currptr->recptr);
      else if(dates_compare(currptr->recptr->get_exitDate(), date1) == "bigger" ){
        htptr->insert_krousma(currptr->recptr); //exei exitdate megalutero tou date1, to theloyme
      }
      else if(dates_compare(currptr->recptr->get_exitDate(), date1) == "equal" ){
        if(dates_compare(currptr->recptr->get_entryDate(), date1) == "equal" )
          htptr->insert_krousma(currptr->recptr); //vazei kai autous poy mphkan kai bghkan thn idia mera an to exitdate einai iso me date1
      }
      else
        num_approved += 0;
      currptr = currptr->next;
    }//telos while gia lista eggrafwn
  }//telos for gia thn i lista
  return;
}

//disqualify tis exitdate <= date1 kai eisodos se simpleht gia topk MAZI ME ELEGXO DISEASE
void search_containter::populate_simpleht(simple_cd_HT *htptr, std::string date1, std::string disease_name){
  int num_approved =0;
  for(unsigned int i=0; i<index; i++){
    if(arr[i] == NULL) //oi eisagwges ginontai h mia meta thn allh ksekinwntas apo to 0. to prwto null shmainei den exei alles
      return;
    reclist * currptr = arr[i];
    while(currptr != NULL){
      if(currptr->recptr->get_exitDate() == "-"){ //einai akoma mesa. ton theloyme
        if(currptr->recptr->get_diseaseID() == disease_name) //MONO EAN EXEI TO DISEASE POY THELW THA ASXOLH8W
          htptr->insert_krousma(currptr->recptr);
      }
      else if(dates_compare(currptr->recptr->get_exitDate(), date1) == "bigger" ){
        if(currptr->recptr->get_diseaseID() == disease_name) //MONO EAN EXEI TO DISEASE POY THELW THA ASXOLH8W
          htptr->insert_krousma(currptr->recptr); //exei exitdate megalutero tou date1, to theloyme
      }
      else if(dates_compare(currptr->recptr->get_exitDate(), date1) == "equal" ){
        if(dates_compare(currptr->recptr->get_entryDate(), date1) == "equal" )
          if(currptr->recptr->get_diseaseID() == disease_name) //MONO EAN EXEI TO DISEASE POY THELW THA ASXOLH8W
            htptr->insert_krousma(currptr->recptr); //vazei kai autous poy mphkan kai bghkan thn idia mera an to exitdate einai iso me date1
      }
      else
        num_approved += 0;
      currptr = currptr->next;
    }//telos while gia lista eggrafwn
  }//telos for gia thn i lista
  return;
}
