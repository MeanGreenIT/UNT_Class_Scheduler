#ifndef DATAROW_H
#define DATAROW_H
#include <string>
class DataRow
{
public:
    int ID;
    int catalog;
    int section;
    std::string course;
    int enrollCap;
    int enrollTotal;
    int waitTotal;
    std::string dotw;
    std::string startTime;
    std::string endTime;
    std::string RoomID;
    std::string Professor;
    DataRow(std::string tID, std::string  tcatalog, std::string  tsection, std::string tcourse, std::string  tenrollCap, std::string  tenrollTotal, std::string  twaitTotal, std::string tdotw, std::string tstartTime, std::string tendTime, std::string tRoomID, std::string tProfessor);
    ~DataRow();
};
#endif