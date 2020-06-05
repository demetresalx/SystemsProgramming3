#include "record.h"
#include "utils.h"
//https://stackoverflow.com/questions/31577866/c-convert-integer-to-binary-array
//SUNARTHSH METATROPHS INT SE ARRAY APO BITS
void int_to_bin_digit(unsigned int , int , int* );

//mia klash gia komvo dentrou
class heapnode{
public:
  heapnode * left; //aristero paidi
  heapnode * right; //deksi paidi
  heapnode * parent; //goneas, isws de xreiazetai

  std::string cat_name; //onoma xwras h astheneias
  int krousmata; //arithmos krousmatwn
  float pososto; //gia topk


  heapnode();
  heapnode( const heapnode&); //copy con
  ~heapnode(); //destructor
};

//h klash moy heap gia ta erwthmata topk
class maxBinaryHeap{
public:
  int number_of_nodes; //arithmos komvwn poy exei mesa o heap
  heapnode * root;
  int maxsize; //iso me to total krousmata ths xwras/astheneias. De ginetai parapanw
  heapnode *latest; //gia th diathrhsh kuriws

  maxBinaryHeap();
  maxBinaryHeap(int); //total gia maxsize
  ~maxBinaryHeap();
  void insert(std::string , int , float); //arrwstia/xwra kai arithmos krousmatwn kai pososto
  heapnode extract(); //bgazei th root kai kanei maintain
};

//epikourikh klassh
class simple_cd_HT_node{
public:
  simple_cd_HT_node * next; //deikths ston epomeno komvo
  std::string cd_name; //kwdikos hlikiakhs kathgorias
  int krousmata; //arithmos krousmatwn
  float pososto; //gia topk

  simple_cd_HT_node(){ next = NULL; krousmata =0;}; //enas empty constructor
  //simple_cd_HT_node(record * ); //constructor basei eggrafhs
  ~simple_cd_HT_node();

};

//epikourikh klassh, gia to topk
class simple_cd_HT{
public:
  int size; //megethos pinaka. tha einai iso me h1+h2
  simple_cd_HT_node ** table; //ena array poy apoteleitai apo listes eggrafwn (buckets).
  int sunolika; //gia na bgei pososto sto topk

  simple_cd_HT(){}; //enas empty contrusctor
  simple_cd_HT(int); //enas constructor me orisma size
  ~simple_cd_HT();

  int insert_krousma(record * ); //vazei ena record mesa sto HT. HASHING BASEI ID
  void populate_heap(maxBinaryHeap *); //gemizei to swro poy zhteitai gia to erwthma
};

//swap periexomenou metaksu 2 heapnodes
void swap_nodes_info(heapnode *, heapnode *);
//ti na kanw sto swim down
std::string make_decision(heapnode *);
//ypsos heap
int height(int );
