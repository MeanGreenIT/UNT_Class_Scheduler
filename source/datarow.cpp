#include "datarow.h"

DataRow::DataRow(std::string tID, std::string  tcatalog, std::string  tsection, std::string tcourse, std::string  tenrollCap, std::string  tenrollTotal, std::string  twaitTotal, std::string tdotw, std::string tstartTime, std::string tendTime, std::string tRoomID, std::string tProfessor)
{
	ID = stoi(tID);
    catalog = stoi(tcatalog);
    section = stoi(tsection);
    course = tcourse;
    enrollCap = stoi(tenrollCap);
    enrollTotal = stoi(tenrollTotal);
    waitTotal = stoi(twaitTotal);
    dotw = tdotw;
    startTime = tstartTime;
    endTime = tendTime;
    RoomID = tRoomID;
    Professor = tProfessor;
}