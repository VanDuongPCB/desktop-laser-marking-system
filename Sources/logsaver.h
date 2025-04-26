#ifndef LOGSAVER_H
#define LOGSAVER_H
#include "lot.h"
#include "model.h"
#include "design.h"

class LogSaver{
public:
    static void save(std::shared_ptr<Lot> lot,std::shared_ptr<Model> model, std::shared_ptr<Design> design);
};

#endif // LOGSAVER_H
