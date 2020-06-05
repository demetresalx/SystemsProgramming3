#ifndef _BBST_H_
#define _BBST_H_

#include "utils.h"
#include <string>
#include "record.h"
#include "topk.h"

//gia to duplicate date sto dentro : oi komvoi exoun lista apo eggrafes me idio date
class reclist{
public:
  record * recptr; //deikths se eggrafh
  reclist * next; //o epomenos.

  reclist();
  reclist(record *);
  ~reclist();
  int insert_record(record * );
  void print_contents();
};

//epikourikh klash gia erwthmata date1 date2
class search_containter{
public:
  int index; //gia tis eisagwges/eksagwges
  reclist ** arr; //ekei einai oi listes poy tha ginoun fetch

  search_containter();
  search_containter(int );
  ~search_containter();
  void insert_reclist(reclist* );
  int count_exit_limit(std::string ); //kanei disqualify oses den exoun exitdate > date1
  int count_exit_limit(std::string , std::string ); //gia diseaseFrequency me orisma country
  int count_entry_limit(std::string ); //kanei disqualify oses den exoune entrydate >= date1
  int count_entry_limit(std::string , std::string ); //to apo panw me orisma country
  int count_exit_limit2(std::string , std::string , std::string ); //gia to diasthma tou discharge
  void populate_simpleht(simple_cd_HT *, std::string ); //gia topk otan exoume date1 date2
  void populate_simpleht(simple_cd_HT *, std::string , std::string ); //gia topk age ranges
};

class BBST{
public:
  std::string * date; //deikths sthn hmeromhnia poy einai string
  reclist * list_of_records; //deikths sth lista eggrafwn. lista gt mporei na xoume panw apo ena me idia hmeromhnia
  BBST * left_child; //aristero paidi
  BBST * right_child; //deksi paidi

  BBST();
  BBST(record * ); //constructor basei eggrafhs
  ~BBST();
  int insert_record(record *);
  void print_contents();
  void collect_dated_reclists(BBST* ,std::string , search_containter*); //sunarthsh arwgos
  void collect_all_reclists(BBST* , search_containter*); //sunarthsh arwgos
  void populate_simpleht(simple_cd_HT *); //gemizei ton epikouriko ht
};



#endif
