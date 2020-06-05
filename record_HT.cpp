#include "record_HT.h"

//constructor gia node basei deikth se eggrafh
record_HT_node::record_HT_node(record * rec){
  next = NULL;
  rec_ptr = rec;
}

record_HT_node::~record_HT_node(){
  delete rec_ptr;//svhnei tn eggrafh
  delete next; //svhnei ton epomeno komvo. LOGW DELETE THA KLH8EI O DESTRCTOR TOU NEXT POY SBHNEI TON NEXT TOU NEXT KTL KTL..
}

//constructor gia HT basei size
record_HT::record_HT(int si){
  size = si;
  table = new record_HT_node *[size]; //ena dunamiko array apo deiktes se ht_nodes (buckets)
  for(unsigned int i=0; i<size; i++){
    table[i] = NULL; //arxika adeio olo
  }
}
record_HT::~record_HT(){
  for(unsigned int i=0; i<size; i++)
    if(table[i] != NULL)
      delete table[i];
  delete[] table; //katastrofh tou pinaka apo chain nodes. To kathe chain node exei ton diko tou destructor kai kanei toso delete this oso kai delete next.
  //h c++ frontizei kai ekshgw parakatw pws
}

//vazei record sto HT. HASHING BASEI ID
int record_HT::insert_record(record * rec){
  unsigned hval = hash_str(rec->get_recordID()); //hasharei to recordID
  hval = hval % size; //gia na pame sth swsth thesh pinaka

  if(table[hval] == NULL){ //ean den yparxei alusida ekei, th ftiaxnoyme
    if(rec->get_entryDate() == "-"){ // h eggrafh den yparxei kai den einai gia EXIT. aporriptetai
      //std::cout << "ERROR den yparxw kai eimai exit " << rec->get_recordID() << "\n";
      delete rec;
      return -1;
    }
    table[hval] = new record_HT_node(rec);
  }
  else{ //yparxei alusida, pame sto telos ENW KOITAZOUME gia IDIO ID
    record_HT_node * currptr = table[hval];
    while(currptr->next != NULL){ //paei sto teleutaio
      if(currptr->rec_ptr->get_recordID() == rec->get_recordID()) //brethhke idio id
      {
        if(rec->get_entryDate() != "-"){ // h eggrafh yparxei kai exei entry. aporriptetai
          //std::cout << "ERROR yparxw kai den ksanakanw entry " << rec->get_recordID() << "\n";
          delete rec;
          return -1;
        }
        else{ //yparxw kai thelw na eimai exit
          if(currptr->rec_ptr->get_exitDate() == "-"){ //yphrxa xwris exit date
            if((dates_compare(currptr->rec_ptr->get_entryDate(), rec->get_exitDate()) != "smaller") && (dates_compare(currptr->rec_ptr->get_entryDate(), rec->get_exitDate()) != "equal"))
            {/*std::cout << "ERROR do8en exit pro= yparxontos entry " << rec->get_recordID() << "\n";*/delete rec; return -1;} //to do8en exit date einai mikrotero tou yparxontos entry date
            currptr->rec_ptr->set_exitDate(rec->get_exitDate()); //enhmerwse exit date
            //currptr->rec_ptr->print();
            delete rec;
            return 3; //ola ok
          }
          else{ //yphrxa me exit date, ftanei. oxi deutero exit
            //std::cout << "ERROR eimai hdh etoimos " << rec->get_recordID() << "\n";
            delete rec;
            return -1;
          }
        }//telos else gia an eimai exit
      }
      currptr = currptr->next ;
    }//telos while alusida
    if(currptr->rec_ptr->get_recordID() == rec->get_recordID()) //brethhke idio id
    {
      if(rec->get_entryDate() != "-"){ // h eggrafh yparxei kai exei entry. aporriptetai
        //std::cout << "ERROR yparxw kai den ksanakanw entry " << rec->get_recordID() << "\n";
        delete rec;
        return -1;
      }
      else{ //yparxw kai thelw na eimai exit
        if(currptr->rec_ptr->get_exitDate() == "-"){ //yphrxa xwris exit date
          if((dates_compare(currptr->rec_ptr->get_entryDate(), rec->get_exitDate()) != "smaller") && (dates_compare(currptr->rec_ptr->get_entryDate(), rec->get_exitDate()) != "equal"))
          {/*std::cout << "ERROR do8en exit pro= yparxontos entry " << rec->get_recordID() << "\n";*/delete rec; return -1;} //to do8en exit date einai mikrotero tou yparxontos entry date
          currptr->rec_ptr->set_exitDate(rec->get_exitDate()); //enhmerwse exit date
          //currptr->rec_ptr->print();
          delete rec;
          return 3; //ola ok
        }
        else{ //yphrxa me exit date, ftanei. oxi deutero exit
          //std::cout << "ERROR eimai hdh etoimos " << rec->get_recordID() << "\n";
          delete rec;
          return -1;
        }
      }//telos else gia an eimai exit
    }
    if(rec->get_entryDate() == "-"){ // h eggrafh den yparxei kai einai gia EXIT. aporriptetai
      //std::cout << "ERROR den yparxw kai eimai exit " << rec->get_recordID() << "\n";
      delete rec;
      return -1;
    }
    currptr->next = new record_HT_node(rec); //valth
  }
  return 0; //egine ok, return 0

}

//sunarthsh ektypwshs
void record_HT::print_contents(){
  for(unsigned int i=0; i<size; i++){
    if(table[i] == NULL) //kenh alusida
      continue;
    else{
      record_HT_node * currptr = table[i];
      while(currptr!= NULL){ //paei sto teleutaio. ENDEIKTIKH EKTYPWSH. MONO MERIKA PEDIA ALLA MPORW KAI OLA
        std::cout << currptr->rec_ptr->get_recordID() << " " << currptr->rec_ptr->get_diseaseID() << " " << currptr->rec_ptr->get_patientFirstName() << " " << currptr->rec_ptr->get_entryDate() << " " <<currptr->rec_ptr->get_exitDate() <<"\n";
        currptr = currptr->next ;
      }//telos while gia orizontia lista
    }//telos else
  }//telos for gia kathe alusida
}//telos sunarthshs

//gia antistoixo erwthma
record * record_HT::recordPatientExit(std::string recid, std::string exitd , std::string * whattodowithcurr){
  unsigned hval = hash_str(recid); //hasharei to recordID
  hval = hval % size; //gia na pame sth swsth thesh pinaka

  if(table[hval] == NULL){ //ean den yparxei alusida ekei, den yparxei h eggrafh
    return NULL;
  }
  else{ //yparxei alusida, pame na broume to id an yparxei
    record_HT_node * currptr = table[hval];
    while(currptr->next != NULL){ //paei sto teleutaio
      if(currptr->rec_ptr->get_recordID() == recid){ //to brhkame
        //std::cout << currptr->rec_ptr->get_entryDate() << " " << exitd;
        if((dates_compare(currptr->rec_ptr->get_entryDate(), exitd) != "smaller")&&(dates_compare(currptr->rec_ptr->get_entryDate(), exitd) != "equal")){ //prepei to yparxon entry date na einai mikrotero h iso tou exitdate poy dinoume twra
          return NULL;
        }
        //if cases gia th sxesh tou exitd me ton current metrhth stis alles domes
        if(exitd == "-"){ //an mas edwse - (den apantane sto piatsa pws to xeirizomaste ara to lamvanw kai auto ypopsin)
          if(currptr->rec_ptr->get_exitDate() == "-")
            *whattodowithcurr = "tipota"; //htan mesa sto nosokomeio kai paramenei mesa.
          else
            *whattodowithcurr = "increase"; //ksanampainei sto nosokomeio, o curr metrhths prepei na anebei
        }
        else{ //de mas edwse -
            if(currptr->rec_ptr->get_exitDate() == "-")
              *whattodowithcurr = "decrease"; //o asthenhs htan mesa sto nosokomeio kai twra phre date ara bgainei kai prepei na meiwthei o curr metrhths
            else
              *whattodowithcurr = "tipota"; //htan ektos nosokomeiou kai paramenei ektos nosokomeiou
        }
        currptr->rec_ptr->set_exitDate(exitd); //to thesame
        return currptr->rec_ptr;
      }//telos if brhkame thn eggrafh
      currptr = currptr->next ;
    }//telos while
    if(currptr->rec_ptr->get_recordID() == recid){ //to brhkame
      if((dates_compare(currptr->rec_ptr->get_entryDate(), exitd) != "smaller")&&(dates_compare(currptr->rec_ptr->get_entryDate(), exitd) != "equal")){ //prepei to yparxon entry date na einai mikrotero h iso tou exitdate poy dinoume twra
        return NULL;
      }
      //if cases gia th sxesh tou exitd me ton current metrhth stis alles domes
      if(exitd == "-"){ //an mas edwse - (den apantane sto piatsa pws to xeirizomaste ara to lamvanw kai auto ypopsin)
        if(currptr->rec_ptr->get_exitDate() == "-")
          *whattodowithcurr = "tipota"; //htan mesa sto nosokomeio kai paramenei mesa.
        else
          *whattodowithcurr = "increase"; //ksanampainei sto nosokomeio, o curr metrhths prepei na anebei
      }
      else{ //de mas edwse -
          if(currptr->rec_ptr->get_exitDate() == "-")
            *whattodowithcurr = "decrease"; //o asthenhs htan mesa sto nosokomeio kai twra phre date ara bgainei kai prepei na meiwthei o curr metrhths
          else
            *whattodowithcurr = "tipota"; //htan ektos nosokomeiou kai paramenei ektos nosokomeiou
      }
      currptr->rec_ptr->set_exitDate(exitd); //to thesame
      return currptr->rec_ptr;
    }//telos if brhkame thn eggrafh
  }//telos else yparxei alysida
  return NULL; //egine ok, return 0

}//telos sunarthshs

//sunarthsh gia to omwnymo erwthma
record * record_HT::searchPatientRecord(std::string recid){
  unsigned hval = hash_str(recid); //hasharei to recordID
  hval = hval % size; //gia na pame sth swsth thesh pinaka

  if(table[hval] == NULL){ //ean den yparxei alusida ekei, den yparxei h eggrafh
    return NULL;
  }
  else{ //yparxei alusida, pame na broume to id an yparxei
    record_HT_node * currptr = table[hval];
    while(currptr->next != NULL){ //paei sto teleutaio

      if(currptr->rec_ptr->get_recordID() == recid){ //to brhkame
        return currptr->rec_ptr;
      }//telos if brhkame thn eggrafh
      currptr = currptr->next ;
    }//telos while
    if(currptr->rec_ptr->get_recordID() == recid){ //to brhkame
      return currptr->rec_ptr;
    }//telos if brhkame thn eggrafh
  }//telos else yparxei alysida
  return NULL; //egine ok, return 0
}
