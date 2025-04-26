#include "systemerror.h"



SystemError::SystemError(){

}


void SystemError::errorReport(Exception ex){
    emit reported(ex);
}

SystemError *SystemError::instance(){
    static SystemError _ins;
    return &_ins;
}
