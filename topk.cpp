#include "topk.h"
#include <math.h>
//https://stackoverflow.com/questions/31577866/c-convert-integer-to-binary-array
//SUNARTHSH METATROPHS INT SE ARRAY APO BITS
void int_to_bin_digit(unsigned int in, int count, int* out)
{
    /* assert: count <= sizeof(int)*CHAR_BIT */
    //std::cout << "phra na kanw to " << in << "\n";
    unsigned int mask = 1U << (count-1);
    int i;
    for (i = 0; i < count; i++) {
        out[i] = (in & mask) ? 1 : 0;
        in <<= 1;
    }
    /*for(unsigned int j=0; j<count; j++)
      std::cout << out[j];
    std::cout << "\n";*/
}

//ypsos heap me N nodes
int height(int N)
{
    return ceil(log2(N + 1)) - 1;
}

//sunarthsh swap periexomenou komvou
void swap_nodes_info(heapnode * node1, heapnode * node2){
  heapnode temp;
  temp.cat_name = node1->cat_name;
  temp.krousmata = node1->krousmata;
  temp.pososto = node1->pososto;
  node1->cat_name = node2->cat_name;
  node1->krousmata = node2->krousmata;
  node1->pososto = node2->pososto;
  node2->cat_name = temp.cat_name;
  node2->krousmata = temp.krousmata;
  node2->pososto = temp.pososto;
}

//apofasizei gia to swimdown
std::string make_decision(heapnode *node1){
  int leftkr = 0;
  int rightkr = 0;
  if(node1->left != NULL)
    leftkr = node1->left->krousmata;
  if(node1->right != NULL)
    rightkr = node1->right->krousmata;
  if(leftkr >= rightkr){ //to aristero einai megalutero
    if(node1->krousmata >= leftkr)
      return "tipota"; //einai megalutero kai ap ta 2 paidia tou. no prob
    else
      return "aristero"; //to aristero einai to megalytero ap ola, ekei prepei na kanei swap
  }
  else{ //deksi eiani megalutero
    if(node1->krousmata >= rightkr)
      return "tipota"; //einai megalutero kai ap ta 2 paidia tou. no prob
    else
      return "deksi"; //to deksi einai to megalytero ap ola, ekei prepei na kanei swap
  }
}//telos sunarthshs


//SUNARTHSEIS MAXHEAP
maxBinaryHeap::maxBinaryHeap(){
  root = NULL;
  latest = NULL;
  number_of_nodes = 0;
}

maxBinaryHeap::maxBinaryHeap(int total){
  root = NULL;
  latest = NULL;
  number_of_nodes = 0;
  maxsize = total;
}

//destructor. TO DELETE STH RIZA LEITOURGEI SE OLO TO HEAP LOGW C++
maxBinaryHeap::~maxBinaryHeap(){
  delete root;
}

//vazei sto heap ena zeugos xwra/astheneia - arithmo krousmatwn
//DE GINETAI NA EMFANISTEI H IDIA ASTHENEIA/XWRA DYO FORES GIATI INSERT APO HASH TABLE ME KLEIDI AUTO
void maxBinaryHeap::insert(std::string cntdis, int numofkrousmata, float posost){
  if(root == NULL){ //prwto stoixeio sto heap
    root = new heapnode();
    root->cat_name = cntdis;
    root->krousmata = numofkrousmata;
    root->pososto = posost;
    number_of_nodes++; //anebainei o arithmos komvwn tou heap
    latest = root;
    return;
  }
  else{ //yparxei riza
    //ftiaxnw bitpath. Kanw ton arithmo ths theshs poy tha mpei binary kai 1 shmainei deksia, 0 aristera
    int * path = new int[maxsize]; //megistos arithmos bits gia to path to last
    int_to_bin_digit(number_of_nodes+1, maxsize, path); //twra to outta exei to bitpath poy prepei na akoloythhsw gia ton last kovmo
    int path_index = 0; //Tha broume thesh most significant bit.prepei na agnohsoume ton prwto asso giati einai h riza
    for(unsigned int i=0; i<maxsize; i++){
      if(path[i] == 1){
        path_index = i+1;
        break;
      }
    }//telos for most significant
    bool insert_done = false;
    heapnode * currnode = root;
    while(true){ //arxizei while mexri na ginei insert
      if(path[path_index] == 1){ //1 shmainei pame deksia
        if(currnode->right == NULL){ //ekei tha einai o teleutaios komvos
          currnode->right = new heapnode();
          currnode->right->cat_name = cntdis;
          currnode->right->krousmata = numofkrousmata;
          currnode->right->pososto = posost;
          currnode->right->parent = currnode;
          latest = currnode->right;
          number_of_nodes++;
          break;
        }//telos if null deksi
        else{
          currnode = currnode->right;
          path_index++;
        }//telos else deksi not null
      }//telos if deksia
      else{ //0 shmainei pame aristera
        if(currnode->left == NULL){ //ekei tha ienai o teleutaios komvos
          currnode->left = new heapnode();
          currnode->left->cat_name = cntdis;
          currnode->left->krousmata = numofkrousmata;
          currnode->left->pososto = posost;
          currnode->left->parent = currnode;
          latest = currnode->left;
          number_of_nodes++;
          break;
        }//telos if null aristero
        else{
          currnode = currnode->left;
          path_index++;
        }//telos else aristero not null
      }//telos else aristera
    }//telos while gia insert
    delete[] path;
////////MAINTAIN HEAP PROPERTY///////////////////////////////////////////////
    //ARXIZEI TO MAINTAIN == SWIMMING UP
    currnode = latest; //arxizei twra swimming apo katw pros panw
    while(true){
      if(currnode->parent == NULL)
        break; //ftasame riza
      if(currnode->krousmata <= currnode->parent->krousmata) //einai ok ekei poy einai. stop
        break;
      else{ //krousmata neou komvou perissotera apo goniou
        swap_nodes_info(currnode, currnode->parent); //swap metaksu tous
        currnode = currnode->parent; //sunexizw pros ta panw to swimming
      }
    }//telos while gia swimming up, TELOS MAINTAIN HEAP PROPERTY
  }//telos else yparxei riza
}//telos sunarthshs

heapnode maxBinaryHeap::extract(){
  heapnode to_be_returned(*root); //pairnei ta data tou root
  if(number_of_nodes == 1){ //mono h riza
    number_of_nodes--;
    delete root;
    root = NULL;
    latest = NULL;
    return to_be_returned;
  }
  swap_nodes_info(root, latest); //riza ginetai to teleutaio
  if(latest->parent != NULL){ //den htan riza, thelw na ftiaksw ta pointers swsta
    if(latest->parent->left == latest) //latest einai aristero paidi
      latest->parent->left = NULL;
    else if(latest->parent->right == latest) //latest einai deksi paidi
      latest->parent->right = NULL;
    else
      std::cout << "should never happen :))\n";
  }
  delete latest; //katastrefetai o teleutaios komvos (prwhn riza)
  number_of_nodes--;
  //paw na enhmerwsw ton deikth latest
  int * path = new int[maxsize]; //megistos arithmos bits gia to path to last
  int_to_bin_digit(number_of_nodes, maxsize, path); //twra to outta exei to bitpath poy prepei na akoloythhsw gia ton last kovmo
  int path_index = 0; //Tha broume thesh most significant bit.prepei na agnohsoume ton prwto asso giati einai h riza
  for(unsigned int i=0; i<maxsize; i++){
    if(path[i] == 1){
      path_index = i+1;
      break;
    }
  }//telos for most significant
  latest = root;
  //std::cout << path_index << " " << number_of_nodes << "\n";
  //EDW EINAI gia na kanei to swsto arithmo step
  int stepsnum = height(number_of_nodes);
  for(unsigned int i= path_index; i<path_index+stepsnum; i++){
    if(latest == NULL)
      std::cout << "should neverDADA happen :))\n";
    if(path[i] == 1) //pame deksia
      latest = latest->right;
    else //pame aristera
      latest = latest->left;
  }//telos for gia fix tou latest
  delete[] path;
  //ARXIZEI SWIMMING DOWN NEAS RIZAS GIA MAINTAIN HEAP PROPERTY
  heapnode * currnode = root;
  std::string whattofollow = "tipota";
  while(true){
    if((currnode->left == NULL) && (currnode->right == NULL))
      break; //einai solo riza h fyllo, den exei paidia
    if(make_decision(currnode) == "tipota") //ola kala, katse st auga sou
      break;
    if(make_decision(currnode) == "aristero"){
      swap_nodes_info(currnode, currnode->left);
      currnode = currnode->left;
    }
    if(make_decision(currnode) == "deksi"){
      swap_nodes_info(currnode, currnode->right);
      currnode = currnode->right;
    }
  }//telos while gia swimming down, TELOS MAINTAIN HEAP PROPERTY
  return to_be_returned;
}//telos sunarthshs

//SUNARTHSEIS HEAPNODE
//empty constr
heapnode::heapnode(){
  left = NULL;
  right = NULL;
  parent = NULL;
  krousmata = 0;
  //pososto = 0.0;
}

//copy constructor gia klhsh isws mazi me thn extract
heapnode::heapnode(const heapnode &par){
  left = NULL;
  right = NULL;
  parent = NULL;
  cat_name = par.cat_name;
  krousmata = par.krousmata;
  pososto = par.pososto;
}

//destructor. DOULEUEI ANADROMIKA LOGW DELETE POY STH C++ KALEI TOUS DESTRUCTORS
heapnode::~heapnode(){
  delete left;
  delete right;
}



//SUNARTHSEIS SIMPLEHT
simple_cd_HT::simple_cd_HT(int sz){
  size = sz;
  table = new simple_cd_HT_node *[size]; //ena dunamiko array apo deiktes se ht_nodes (buckets)
  for(unsigned int i=0; i<size; i++){
    table[i] = NULL; //arxika adeio olo
  }
  sunolika =0;
}

//destructor
simple_cd_HT::~simple_cd_HT(){
  for(unsigned int i=0; i<size; i++)
    if(table[i] != NULL)
      delete table[i];
  delete[] table; //katastrofh tou pinaka apo chain nodes. To kathe chain node exei ton diko tou destructor kai kanei toso delete this oso kai delete next.
}

//constructor simple node basei record
int simple_cd_HT::insert_krousma(record * rec){

    unsigned hval = get_age_category(rec->get_age()); //pame sth 8esh pinaka basei hlikiakhs kathgorias
    hval = hval % size; //gia na pame sth swsth thesh pinaka

    if(table[hval] == NULL){ //ean den yparxei alusida ekei, th ftiaxnoyme
      table[hval] = new simple_cd_HT_node();
      table[hval]->krousmata += 1;
      sunolika++;
      table[hval]->cd_name = std::to_string(get_age_category(rec->get_age())); //gia na mhn ginoun terasties allages sth domh ths 1hs askhshs, apla kanw ton arithmo-anagnwristiko ths kathgorias hlikias string
      return 0;
    }
    else{ //yparxei alusida, pame sto telos ENW KOITAZOUME OTI DEN YPARXEI IDIO ID
      simple_cd_HT_node * currptr = table[hval];
      while(currptr->next != NULL){ //paei sto teleutaio
        if(currptr->cd_name == std::to_string(get_age_category(rec->get_age())) ){ //brethhke h idia astheneia. aplws aukshsh krousmatwn k telos
          currptr->krousmata += 1;
          sunolika++;
          return 0;
        }
        currptr = currptr->next ;
      }//telos while buckets
      if(currptr->cd_name == std::to_string(get_age_category(rec->get_age())) ){ //brethhke h idia astheneia. aplws aukshsh krousmatwn k telos
        currptr->krousmata += 1;
        sunolika++;
        return 0;
      }
      currptr->next = new simple_cd_HT_node();
      currptr->krousmata += 1;
      sunolika++;
      currptr->cd_name = std::to_string(get_age_category(rec->get_age())); //gia na mhn ginoun terasties allages sth domh ths 1hs askhshs, apla kanw ton arithmo-anagnwristiko ths kathgorias hlikias string;
      return 0; //ola ok
    }//telos else alusidas

  return 0;
}

//gia gemisma heap
void simple_cd_HT::populate_heap(maxBinaryHeap * heaptr){
  for(unsigned int i=0; i<size; i++){
    if(table[i] == NULL)
      continue; //adeio bucket
    else{
      simple_cd_HT_node * currptr = table[i];
      while(currptr!= NULL){ //paei sto teleutaio. ENDEIKTIKH EKTYPWSH. MONO MERIKA PEDIA ALLA MPORW KAI OLA
        currptr->pososto = (float)currptr->krousmata / (float)sunolika; //ypologismos posostou apo teliko apotelesma
        heaptr->insert(currptr->cd_name, currptr->krousmata, currptr->pososto);
        currptr = currptr->next ;
      }//telos while gia orizontia lista
    }//telos else
  }//telos for
}//telos sunarthshs

//h mageia ths c++ diagrafei olo to bucketlist gt h delete kalei destructors
simple_cd_HT_node::~simple_cd_HT_node(){
  delete next;
}
