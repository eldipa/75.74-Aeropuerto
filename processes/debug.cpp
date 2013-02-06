
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include "api_tractor.h"
#include "log.h"
#include "local_broker_comm.h"

int main()
try {
	LocalBrokerComm broker("TEST");


}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
