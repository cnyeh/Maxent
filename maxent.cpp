/*
 * Copyright (C) 1998-2015 ALPS Collaboration. See COPYRIGHT.TXT
 * All rights reserved. Use is subject to license terms. See LICENSE.TXT
 * For use in publications, see ACKNOWLEDGE.TXT
 */

#include "maxent.hpp"
//#include <alps/mc/mcoptions.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/exception/diagnostic_information.hpp> 
#include "maxent_parms_default.hpp"

inline void checkInput(alps::params &p){
    if(!p.exists("BETA")){
	std::cout<<"Please supply BETA"<<std::endl;
	p["help"] = true;
    }
    if(!p.exists("NDAT")){
	std::cout<<"Please supply NDAT"<<std::endl;
	p["help"] = true;
    }
}

int main(int argc,char** argv)
{
  //gtest requires char** while alps params requires const char**
  if(argc==2 && std::string(argv[1])==std::string("--test")){
    ::testing::InitGoogleTest(&argc, argv);
    exit(RUN_ALL_TESTS());
  }
  set_defaults(parms);
  checkInput(parms);
    if (parms.help_requested(std::cout)) {
        return 0;
    }
  std::string basename;
  //if basename="" then make a better one
  if(parms.defaulted("BASENAME")){
    std::string input_file = argv[1];
    if (input_file.find(".in.h5") != std::string::npos)
	parms["BASENAME"] = input_file.substr(0,input_file.find_last_of(".in.h5")-5)+ ".out.h5";
    else if (input_file.find(".out.h5") != std::string::npos)
	 parms["BASENAME"] = input_file;
    else
         parms["BASENAME" ]= input_file.substr(0,input_file.find_last_of('.'))+ ".out";
    basename=parms["BASENAME"].as<std::string>();
  alps::params parms(argc,const_cast<const char**>(argv)); 
  }
  else
    basename=boost::lexical_cast<std::string>(parms["BASENAME"]);
      // If requested, we print the help message, which is constructed from the
    // information we gave when defining the parameters.
  try{
        //allow for multiple default model runs
        //set MODEL_RUNS = #runs
        //then place:
        //RUN_0 = "Model1"
        //RUN_1 = "Model2"
        //..etc
        if(parms.exists("MODEL_RUNS")){
            int nruns=parms["MODEL_RUNS"];
            std::cout<<"Performing " << nruns <<" runs" <<std::endl;
	    //ALPSCore requires all params are defined
 	    for(int i=0;i<nruns;i++)
		parms.define<std::string>("RUN_" + boost::lexical_cast<std::string>(i),"Run");
            for(int i=0;i<nruns;i++){
                std::string currModel = parms["RUN_" + boost::lexical_cast<std::string>(i)].as<std::string>();
                parms["DEFAULT_MODEL"]= currModel;
                
                //run a simulation with the new default model.
                //Change the basename to match
                parms["BASENAME"] = basename+'.'+currModel;
                MaxEntSimulation my_sim(parms);
                my_sim.run();
                my_sim.evaluate();
            }
        }
      else{
          MaxEntSimulation my_sim(parms);
          my_sim.run();
          my_sim.evaluate();
        }
  }
    catch(const std::exception &e){
        std::cerr << "Caught Exception " << boost::diagnostic_information(e);
    }
    catch(...){
        std::cerr << "Caught Exception" << boost::current_exception_diagnostic_information();
    }
}

