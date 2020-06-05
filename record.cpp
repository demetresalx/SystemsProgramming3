#include "record.h"


record::record(std::string *params){ //constructor basei twn gnwrismatwn poy ekshxthhsan apo grammh input
  //std::cout << "i am construcotrrrrr";

  recordID = params[0];
  patientFirstName = params[1];
  patientLastName = params[2];
  diseaseID = params[3];
  country = params[4];
  entryDate = params[5];
  exitDate = params[6];
  age = std::stoi(params[7]);
}

//destructor, sxoliasmeno eswteriko gt to eixa dokimasei kai me pointers alla de xreiasthke
record::~record(){


}

//setters
void record::set_recordID(std::string newid){
  recordID = newid;
}

void record::set_patientFirstName(std::string newfn){
  patientFirstName = newfn;
}

void record::set_patientLastName(std::string newln){
  patientLastName = newln;
}

void record::set_diseaseID(std::string newid){
  diseaseID = newid;
}

void record::set_country(std::string newc){
  country = newc;
}

void record::set_entryDate(std::string newed){
  entryDate = newed;
}

void record::set_exitDate(std::string newed){
  exitDate = newed;
}

void record::set_age(int newage){
  age = newage;
}

//getters
std::string record::get_recordID(){
  return recordID;
}

std::string record::get_patientFirstName(){
  return patientFirstName;
}

std::string record::get_patientLastName(){
  return patientLastName;
}

std::string record::get_diseaseID(){
  return diseaseID;
}

std::string record::get_country(){
  return country;
}

std::string record::get_entryDate(){
  return entryDate;
}

std::string record::get_exitDate(){
  return exitDate;
}

int record::get_age(){
  return age;
}
