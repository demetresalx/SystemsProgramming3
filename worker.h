#ifndef WORKER_H_
#define WORKER_H_

//sunarthsh poy kanei th douleia tou worker
int work(char *, char *, int , int );

//stelnei ta apotelesmata ths topk ston patera
void deliver_topk(int , int, int * , float *);

//stelnei ston patera to apotelesma tus numPatientAdmissions xwris orisma country
void deliver_num_adms_disch1(int, int ,void *, int * , int );



#endif
