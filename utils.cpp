#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <fstream>
#include "utils.h"

//h aplh periptwsh me char *
//vasismeno se Marc J. Rochkind - Advanced UNIX Programming (2004, Addison-Wesley Professional) - selida 97
int send_string(int fd, char * str, int b){
  ssize_t nwritten = 0, n;
  int size = strlen(str) +1; //to mhkos poy prepei na steilei prwta
  int32_t conv = htonl(size); //to mhkos se UNIVERSALLY PASSABLE MORFH
  int bwrit = write(fd, &conv, sizeof(conv));
  //stelnoyme twra to string
  //prosexw na mh grapsw perissotero ap oso xreiazetai
  //grafw b bytes kathe fora mexri na parw to mhnyma me swstous elegxous

  int to_write=0;
  do {
    if(size-nwritten <= b)
      to_write = size-nwritten;
    else
      {to_write = b;}

    //error handling gia th write
    if ((n = write(fd, &((const char *)str)[nwritten], to_write)) == -1) {
      if (errno == EINTR) //an diakopei apo signal
        continue;
        else
        return -1;
      }
    nwritten += n;
  } while (nwritten < size); //an osa exw grapsei mexri twra ftasan to swsto megethos, stamata

  return nwritten;

}

//to stelnei apo string gia anti gia char array
//vasismeno se Marc J. Rochkind - Advanced UNIX Programming (2004, Addison-Wesley Professional) - selida 97
int send_string(int fd, std::string * str, int b){
  ssize_t nwritten = 0, n;
  int size = str->length() +1; //to mhkos poy prepei na steilei prwta
  int32_t conv = htonl(size); //to mhkos se UNIVERSALLY PASSABLE MORFH
  char a[size]; //gia na ginei low level I/O xreiazomai anagkastika char *
  strcpy(a, str->c_str());  //ara pernaw to std::string se ena proswrino char * kai stelnw auto
  //std::cout << *str;
  int bwrit = write(fd, &conv, sizeof(conv));
  //stelnoyme twra to string
  //prosexw na mh grapsw perissotero ap oso xreiazetai
  //grafw b bytes kathe fora mexri na parw to mhnyma me swstous elegxous

  int to_write=0;
  do {
    //apofasisw an me pairnei na grapsw b h to mhnyma teleiwnei se ligotera
    if(size-nwritten <= b)
      to_write = size-nwritten;
    else
      {to_write = b;}

    //error handling gia th write
    if ((n = write(fd, &((const char *)a)[nwritten], to_write)) == -1) {
      if (errno == EINTR) //an diakopei apo signal
        continue;
        else
        return -1;
      }
    nwritten += n;
  } while (nwritten < size); //an osa exw grapsei mexri twra ftasan to swsto megethos, stamata

  return nwritten;

}

//gia metafora akeraiou across the net
int send_integer(int fd, int *in){
  int32_t conv = htonl(*in); //sigoura 32bits
  write(fd, &conv, sizeof(conv));
  return 0;
}

//h aplh periptwsh me char *
//vasismeno se Marc J. Rochkind - Advanced UNIX Programming (2004, Addison-Wesley Professional) - selida 97
int receive_string(int fd, char * buf, int b){
  ssize_t nread = 0, n;
  strcpy(buf, "");
  int size =0;
  int32_t conv;
  //pare mhkos erxomenhs sumvoloseiras
  int brd = read(fd, &conv, sizeof(conv));
  //kanto twra se an8rwpinh morfh
  size = ntohl(conv);

  int to_read=0;
  do {
    //apofasisw an me pairnei na diabasw b h to mhnyma teleiwnei se ligotera
    if(size-nread <= b)
      to_read = size-nread;
    else
      to_read = b;

      //error handling ths read
    if ((n = read(fd, &((char *)buf)[nread], to_read)) == -1) {
      if (errno == EINTR) //an diakopei apo signal
        continue;
      else
        return -1;
    }
    if (n == 0)
      return nread;
    nread += n;
  } while (nread < size); //stamata otan diabaseis to akribes megethos mhnymatos se bytes

  return nread;

}

//vasismeno se Marc J. Rochkind - Advanced UNIX Programming (2004, Addison-Wesley Professional) - selida 97
//to krataei se string anti gia gia char array
int receive_string(int fd, std::string * str, int b){
  ssize_t nread = 0, n;
  char tool[300];
  strcpy(tool, "");
  int size =0;
  int32_t conv;
  //pare mhkos erxomenhs sumvoloseiras
  int brd = read(fd, &conv, sizeof(conv));
  //kanto twra se an8rwpinh morfh
  size = ntohl(conv);

  int to_read=0;
  do {
    //apofasisw an me pairnei na diabasw b h to mhnyma teleiwnei se ligotera
    if(size-nread <= b)
      to_read = size-nread;
    else
      {to_read = b;}

      //error handling ths read
    if ((n = read(fd, &((char *)tool)[nread], to_read)) == -1) {
      if (errno == EINTR) //an diakopei apo signal
        continue;
      else
        return -1;
    }
    if (n == 0)
      return nread;
    nread += n;
    *str = std::string(tool);
  } while (nread < size); //stamata otan diabaseis to akribes megethos mhnymatos se bytes

  return nread;
}

//gia diavasma akeraiou from the net
int receive_integer(int fd, int *in){
  int32_t conv;
  read(fd, &conv, sizeof(conv));
  *in = ntohl(conv);
  return 0;
}

//gia na pairnw ta files poy exoun mesa ta countries-dirs
int extract_files(char * inpdir, int * fleft, std::string ** fls){
  DIR *dp;
  struct dirent *dirp;
  //anoigw kai tsekarw oti einai ok
  dp = opendir(inpdir);
  if(dp == NULL)
  {
      std::cout << "Error opening " << inpdir << "\n";
      return errno;
  }
  //metraw ta directories poy exei mesa
  while ((dirp = readdir(dp)) != NULL){
    if((strcmp(dirp->d_name, ".") == 0)||(strcmp(dirp->d_name, "..") == 0))
      continue;
    *(fleft) += 1;
  }
  closedir(dp);

  if(*fleft > 0){
    dp = opendir(inpdir);
    if(dp == NULL)
    {
        std::cout << "Error opening " << inpdir << "\n";
        return errno;
    }
    //metraw ta directories poy exei mesa

    *fls = new std::string[*fleft];
    int in =0;
    while ((dirp = readdir(dp)) != NULL){ //krata to dir name
      if((strcmp(dirp->d_name, ".") == 0)||(strcmp(dirp->d_name, "..") == 0))
        continue;
      (*fls)[in] = std::string(dirp->d_name);
      in++;
    }
    closedir(dp);
  }
  return 0;
}

//hash gia strings
//https://stackoverflow.com/questions/16075271/hashing-a-string-to-an-integer-in-c - hash function gia strings
//prosarmosmenh sta dika moy dedomena
unsigned hash_str(std::string str)
{
   uint32_t hash = 0x811c9dc5;
   uint32_t prime = 0x1000193;
   for(int i = 0; i < str.size(); ++i) {
        uint8_t value = str[i];
        hash = hash ^ value;
        hash *= prime;
    }
   return hash;
}


//h sunarthsh epistrefei to katallhlo apotelesma gia to an to date1 einai megalutero, iso h mikrotero tou date2
std::string dates_compare(std::string date1, std::string date2){

  if(is_date_ok(date1) == false)
    return "problem";
  if(is_date_ok(date2) == false)
    return "problem";
  if(date1 == "-") //de ginetai na mhn exei entry date
    return "problem";
  int params_count =0;
  std::string intermediate;
  std::stringstream check1(date1);
  int date1_parts[3]; //mia thesh gia mera, mia gia mhna mia gia xronia.
  while(getline(check1, intermediate, '-')) {
        date1_parts[params_count] = stoi(intermediate);
        params_count++;
  } //telos while eksagwghs gnwrismatwn apo date1
  //date2
  if(date2 == "-")
    return "smaller"; //den exei bgei akoma, eimaste ok me thn paula.

  int params_count2 =0;
  std::stringstream check2(date2);
  int date2_parts[3]; //mia thesh gia mera, mia gia mhna mia gia xronia.
  while(getline(check2, intermediate, '-')) {
        date2_parts[params_count2] = stoi(intermediate);
        params_count2++;
  }//telos while eksagwghs gnwrismatwn apo date2


  if(params_count2 != params_count)//problhmatiko input. de tha eprepe na dothei etsi sumfwna me ekfwnhsh
    return "kakws orismena dates. shouldn't happen kata ekfwnhsh";

  if(date1_parts[2] > date2_parts[2]) //megaluterh xronia
    return "bigger";
  if(date1_parts[2] == date2_parts[2]){ //ish xronia
    if(date1_parts[1] > date2_parts[1]) //megaluteros mhnas me ish xronia
      return "bigger";
    if(date1_parts[1] == date2_parts[1]){ //isos mhnas me ish xronia
      if(date1_parts[0] > date2_parts[0]) //megaluterh mera me iso mhna kai xronia
        return "bigger";
      if(date1_parts[0] == date2_parts[0]) //ola isa
        return "equal";
      if(date1_parts[0] < date2_parts[0]) //ish xronia isos mhnas mikroterh mera
        return "smaller";
    }
    if(date1_parts[1] < date2_parts[1]) //mikroteros mhnas me ish xronia
      return "smaller";
  }
  if(date1_parts[2] < date2_parts[2]) //mikroterh xronia
    return "smaller";
}


//elegxw an ena date einai ok
bool is_date_ok(std::string dato){
  if(dato == "-")
    return true;
  int params_count =0;
  std::string intermediate;
  std::stringstream check1(dato);
  int date1_parts[3]; //mia thesh gia mera, mia gia mhna mia gia xronia.
  while(getline(check1, intermediate, '-')) {
        //std::cout << intermediate;
        date1_parts[params_count] = stoi(intermediate);
        params_count++;
  } //telos while eksagwghs gnwrismatwn apo dato
  if(params_count != 3) //den to zorizoume. einai lathos
    return false;
  if((date1_parts[1] == 1) || (date1_parts[1] == 3) || (date1_parts[1] == 5) || (date1_parts[1] == 7) || (date1_parts[1] == 8) || (date1_parts[1] == 10) || (date1_parts[1] == 12)){
    if((date1_parts[0] <= 31)&&(date1_parts[0] >= 1))
      return true;
  }
  else if((date1_parts[1] == 4) || (date1_parts[1] == 6) || (date1_parts[1] == 9) || (date1_parts[1] == 11) ){
    if((date1_parts[0] <= 30)&&(date1_parts[0] >= 1))
      return true;
  }
  else if(date1_parts[1] == 2){
    if((date1_parts[0] <= 29)&&(date1_parts[0] >= 1))
      return true;
  }
  return false;
}


//sortarisma arxeiwn me quicksort, prosarmosmenh sthn askhsh (strings)
//https://www.geeksforgeeks.org/quick-sort/
int partition (std::string * filen, int low, int high)
{
    std::string pivot = filen[high]; // pivot
    int i = (low - 1); // Index of smaller element

    for (int j = low; j <= high - 1; j++)
    {
        // If current element is smaller than the pivot
        if(dates_compare(filen[j], pivot) == "smaller")
        {
            i++; // increment index of smaller element, swap
            std::string temp = filen[i];
            filen[i] = filen[j];
            filen[j] = temp;
        }
    }
    std::string temp = filen[i+1];
    filen[i+1] = filen[high];
    filen[high] = temp;
    return (i + 1);
}

void sort_files(std::string * filesn , int low, int high){
  if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
        at right place */
        int pi = partition(filesn, low, high);

        // Separately sort elements before
        // partition and after partition
        sort_files(filesn, low, pi - 1);
        sort_files(filesn, pi + 1, high);
    }
}


//moy leei an einai sthn 1h, 2h, 3h h 4h orismenh kathgoria hlikias
int get_age_category(int age){
  //error, should never happen
  if(age <0)
    return -1;

  if((age >= 0)&&(age <= 20))
    {return 0;} //ekei anhkei
  else if((age >= 21)&&(age <= 40))
    {return 1;} //ekei anhkei
  else if((age >= 41)&&(age <= 60))
    {return 2;} //ekei anhkei
  else //61+
    {return 3;} //ekei anhkei

}

//steile se kapoion (gonio) ta periexomena tou file summary
void send_file_summary(int wfd, int summ_entries, std::string filename, std::string country, file_summary * fsm, int bsize){
  write(wfd, &summ_entries, sizeof(int));
  if(summ_entries == 0)
    return; //mh grapseis tipota allo

  //paw na grapsw
  send_string(wfd, &filename, bsize);
  send_string(wfd, &country, bsize);
  file_summary * currptr = fsm;
  for(int i=0; i<summ_entries; i++){
    //steile onoma iou
    send_string(wfd, &(currptr->diseasename), bsize);
    //grapse arithmo krousmatwn kathe kathgorias hlikiakhs
    for(int j=0; j<4; j++)
      write(wfd, &(currptr->age_cats[j]), sizeof(int));

    currptr = currptr->next; //h parametros summ entries einai tetoia poy de tha prokalesei problhma
  }//telos for periexomena tou summary
}//telos sunarthshs

//diabase apo kapoion (paidi) ta periexomena tou summary
void receive_and_print_file_summary(int rfd, int bsize){
  int summ_entries =0;
  read(rfd, &summ_entries, sizeof(int));
  if(summ_entries == 0)
    return; //mhn kaneis tpt allo

  //paw na diabasw
  std::string filename;
  std::string country;
  std::string dis_name;
  receive_string(rfd, &filename, bsize);
  receive_string(rfd, &country, bsize);
  //AKOLOYTHW FORMAT EKTYPWSHS EKFWNHSHS
  std::cout << filename << "\n";
  std::cout << country << "\n";

  for(int i=0; i<summ_entries; i++){
    //diabase k printare onoma iou
    receive_string(rfd, &dis_name, bsize);
    std::cout << dis_name << "\n";
    //diabase k deikse arithmo krousmatwn kathe kathgorias hlikiakhs
    int krousm=0;
    read(rfd, &krousm, sizeof(int));
    std::cout << "Age range 0-20 years: " << krousm << " cases\n";
    read(rfd, &krousm, sizeof(int));
    std::cout << "Age range 21-40 years: " << krousm << " cases\n";
    read(rfd, &krousm, sizeof(int));
    std::cout << "Age range 41-60 years: " << krousm << " cases\n";
    read(rfd, &krousm, sizeof(int));
    std::cout << "Age range 60+ years: " << krousm << " cases\n";

    //afhne kenh seira metaksu iwn opws fainetai na kanei h ekfwnhsh
    std::cout << "\n";
  }//telos for entries enos summary
}//telos sunarthshs


//gia th boh8htikh klash gia ta summaries ana io arxeiou
file_summary::file_summary(){
  diseasename = "";
  for(int i=0; i<4; i++)
    age_cats[i] = 0; //arxikopoiei se 0
  next = NULL;
}
//me tous kala orismenous destructors ths c++ arkei! h katastrofh ginetai anadromika se olous!!
file_summary::~file_summary(){
  delete next;
}

int file_summary::insert_data(std::string * record_parts){
  if(record_parts[5] != "-"){ //koitame MONO tis enter gia ta summaries
    if(diseasename == ""){ //mono thn prwth fora
      diseasename = record_parts[3]; //pare thn astheneia
      if((std::stoi(record_parts[7]) >= 0)&&(std::stoi(record_parts[7]) <= 20))
        {age_cats[0] += 1; return 1;} //ekei anhkei
      else if((std::stoi(record_parts[7]) >= 21)&&(std::stoi(record_parts[7]) <= 40))
        {age_cats[1] += 1; return 1;} //ekei anhkei
      else if((std::stoi(record_parts[7]) >= 41)&&(std::stoi(record_parts[7]) <= 60))
        {age_cats[2] += 1; return 1;} //ekei anhkei
      else
        {age_cats[3] += 1; return 1;} //ekei anhkei
    }
    if(diseasename == record_parts[3]){ //bre8hke h as8eneia, kanoyme enhmerwsh
      if((std::stoi(record_parts[7]) >= 0)&&(std::stoi(record_parts[7]) <= 20))
        {age_cats[0] += 1; return 0;} //ekei anhkei
      else if((std::stoi(record_parts[7]) >= 21)&&(std::stoi(record_parts[7]) <= 40))
        {age_cats[1] += 1; return 0;} //ekei anhkei
      else if((std::stoi(record_parts[7]) >= 41)&&(std::stoi(record_parts[7]) <= 60))
        {age_cats[2] += 1; return 0;} //ekei anhkei
      else
        {age_cats[3] += 1; return 0;} //ekei anhkei
    }//telos if bre8hke astehneia
    if(next == NULL){ //to vazoume ston epomeno adeio
      next = new file_summary;
      return next->insert_data(record_parts);
    }
    else //an o epomenos den einai adeios, tha krinei autos
      return next->insert_data(record_parts);

  }//telos if einai entry eggrafh
  return -1;
}

//h 2h boh8htikh klash gia summaries
directory_summary::directory_summary(int filesn, std::string cnt){
  nfiles = filesn;
  countryname = cnt;
  filenames = new std::string[nfiles];
  nodes_per_file = new int[nfiles];
  tfile_sums = new file_summary*[nfiles];
}

directory_summary::~directory_summary(){
  for(int i=0; i<nfiles; i++)
    delete tfile_sums[i];
  delete[] tfile_sums;
  delete[] nodes_per_file;
  delete[] filenames;
}

//gia poll
void reset_poll_parameters(struct pollfd * pollfds, int length){
  for(int i=0; i<length; i++)
    pollfds[i].events = POLLIN;
}

//gia kykliko buffer (pool diafaneiwn)
pool::pool(int size){
  fds = new int[size]; //oso to bufferSize tha einai auto
  memset(fds, 0, sizeof(fds)); //gemise to me 0 arxika
  start = 0;
  end = -1;
  count = 0;

}

//destructor
pool::~pool(){
  delete fds;
}
