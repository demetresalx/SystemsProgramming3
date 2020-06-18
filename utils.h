#ifndef UTILS_H_
#define UTILS_H_

#define PERMS 0666
#define IO_PRM 128
#include <poll.h> //tha xreiastei opote to kanw include edw kai mesw autou tou header ta ginetai kai auto include
#include <string>
#include <dirent.h> //gia DIRECTORIES
#include <sstream>


//OI PARAKATW SUNARTHSEIS EINAI OLES LOW LEVEL I/O APO PIPIES. DES TO .CPP GIA DETAILS
//grapse string se pipe me akribh tropo
int send_string(int , char *, int );
int send_string(int , std::string *, int );
int send_integer(int, int*);

//diabase string se pipe me akribh tropo.
int receive_string(int, char *,int);
int receive_string(int, std::string *,int);
int receive_integer(int, int*);

//gia na pairnw ta files h ta directories poy yparxoyn se ena path
int extract_files(char * , int * , std::string **);
//gia na sortarw ta file names vasei mias quicksort
void sort_files(std::string * , int , int );

//h hash function moy gia hash tables
unsigned hash_str(std::string );

//gia sugkrish dates
std::string dates_compare(std::string , std::string );

//elegxei an ena date einai ok
bool is_date_ok(std::string );

//se poia kathgoria anhkei enas int
int get_age_category(int );

//boh8htikh domh gia to summary. anaferetai se dedomena enos arxeiou ana io
//sthn epanalhpsh sthn opoia xrhsimopoeitai einai gnwsth h xwra kai h hmeromhnia opote de thn krataw kai edw
class file_summary{
public:
  std::string diseasename; //onoma-hmeromhnia arxeiou
  int age_cats[4]; //arithmos krousmatwn ana hlikiakh kathgoria
  file_summary * next; //o epomenos ios

  file_summary();
  ~file_summary();
  int insert_data(std::string * ); //enhmerwnei tis metavlhtes poy prepei apo ta merh enos record
  void print_contents();
};
//edw tha krataw antikeimena apo panw class. telika tha ta stelnw gonio
class directory_summary{
public:
  std::string countryname; //h xwra poy antistoixei sto directory
  int nfiles; //posa arxeia exei to directory
  std::string * filenames; //pinakas me ta onomata twn arxeiwn - hmeromhnies
  int * nodes_per_file; //gia na kserw gia tis loupes
  file_summary ** tfile_sums; //enas pinakas apo deiktes se file sums. sthn i thesh einai to summary poy tha estelne to i arxeio

  directory_summary(){};
  directory_summary(int , std::string ); //me plh8os arxeiwn k xwra
  ~directory_summary();
};
//gia na stelnw ta summaries enos arxeiou sto gonio
void send_file_summary(int , int ,std::string , std::string, file_summary *, int );
//gia na ta diabazw k na ta parousiazw apo gonio
void receive_and_print_file_summary(int , int );
//gia na kanw reset ta poll events
void reset_poll_parameters(struct pollfd *, int);
//gia na tsekarw an teleiwsan ta pending connections se ena listening socket
int check_if_will_block(int );
//gia katharisma entolwn tou client kuriws
int sanitize_command(std::string, std::string *);
//gia apostolh queries apo client se server
int send_command(int , std::string *, int );
//gia na lamvanei apanthsh gia to erwthma numPatientAdmissions h discharges xwris xwra
void read_and_present_num_adms_disch(int , std::string * );

#endif
