#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <math.h>

#include "XrdClient/XrdClient.hh"

int main(int argc, char** argv) {

  if (argc != 3) {
    std::cout<<"Usage: ./xrd_test list_of_input_files iteration_length\n"<<std::endl;
    exit(1);
  }

  std::ifstream input_files(argv[1]);
  double iteration_length = atof(argv[2]);
  printf("Attempting to open files in %s at rate of one file per %.6lf seconds\n", 
      argv[1], iteration_length);

  std::string line;
  if ( input_files.is_open() ) {
    while ( getline ( input_files, line) ) {
      //std::cout<<"line : "<<line<<std::endl;
      XrdClient *cli = new XrdClient( line.c_str() );

      // file opening timestamp
      timeval tim1;
      timeval tim2;
      gettimeofday(&tim1, NULL);
      double t1=tim1.tv_sec+(tim1.tv_usec/1000000.0);
      int start_time = tim1.tv_sec;
    
      //cli->Open( 0, 0);
      //if (!cli->IsOpen() ) {
      bool fopen_success = true;
      if ( !cli->Open(0, 0) ||
             (cli->LastServerResp()->status != kXR_ok ) ) {
        // client failed to open file properly
        printf("failed to open file %s \n", line.c_str());
        fopen_success = false;
      }
      cli->Close();

      // file closing timestamp
      gettimeofday(&tim2, NULL);
      double t2=tim2.tv_sec+(tim2.tv_usec/1000000.0);
  
      std::string fsuccess_message;
      if (fopen_success) {
        fsuccess_message = "success";
      }      
      else {
        fsuccess_message = "failed";
      } 

      printf("RESULT: %s %s %i %.6lf \n", line.c_str(),
	fsuccess_message.c_str(), start_time, t2-t1);
      
      double sleep_length = iteration_length - (t2-t1);
      if (sleep_length <= 0) { sleep_length = 0.0; }

      timeval twait;
      double wait_seconds;
      twait.tv_usec = (modf(sleep_length, &wait_seconds))*1000000;
      twait.tv_sec = wait_seconds;
      
      //printf("sleep length this cycle: %.6lf \n", sleep_length);
      select(0,NULL,NULL,NULL,&twait);
    }
  }

  return 0;
}
