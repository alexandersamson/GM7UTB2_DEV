#include "Prompt.h"


bool Prompt::sendDataPackageToInterfaceAndClosePrompt(Interface *interface, Globals::CommonInternalDataPackage dataPackageToSend){
  if(sendDataPackageToInterface(interface, dataPackageToSend) == false){
    return false;
  }
  requestToClose();
  return true;
}


bool Prompt::sendDataPackageToInterface(Interface *interface, Globals::CommonInternalDataPackage dataPackageToSend){
  dataPackageToSend.iteration = dataPacakgeIterator;
  if(interface->inputCommonDataPackage(dataPackageToSend) == false){
    return false;
  }
  dataPacakgeIterator++;
  return true;
}

void Prompt::requestToClose(){
  requestingToClose = true;
}

bool Prompt::isRequestingToClose(){
    return requestingToClose;
}

void Prompt::close(){
  requestingToClose = false;
  integerValue = 0;
  stringValue = "\0";
  captureBackspace = false;
}