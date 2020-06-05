#ifndef _RECORD_HT_H_
#define _RECORD_HT_H_

#include "record.h"
#include "utils.h"
//H dikh moy domh gia thn apothhkeush twn eggrafwn. Me hash table exoume poly kalyterh xronikh polyplokothta ap oti me aplh lista.
//Na shmeiwthei pws autos o HT einai enas KLASSIKOS HT kai den akoloythei th morfologia twn disease kai country HT poy tha ftiaxtoun opws zhtaei h askhsh.

class record_HT_node{ //sunista ta buckets tou DIKOU MOU HT gia tis eggrafes poy tha einai basei recordID

  public:
    record_HT_node * next; //deikths ston epomeno komvo
    record * rec_ptr; //deikths sthn eggrafh
    record_HT_node(){}; //enas empty constructor
    record_HT_node(record * ); //constructor basei eggrafhs
    ~record_HT_node();
};


class record_HT{ // o HT

  public:
    int size; //megethos pinaka. tha einai iso me h1+h2
    record_HT_node ** table; //ena array poy apoteleitai apo listes eggrafwn (buckets).
    record_HT(){}; //enas empty contrusctor
    record_HT(int ); //enas constructor me orisma size
    ~record_HT();

    int insert_record(record * ); //vazei ena record mesa sto HT. HASHING BASEI ID

    void print_contents();
    record * recordPatientExit(std::string ,std::string , std::string * ); //gia to antistoixo erwthma
    record * searchPatientRecord(std::string ); //gia to sxetiko erwthma basei ID
};



#endif
