
.DEFAULT_GOAL = all

local_broker_rpc.h: local_broker.x
	rm -f local_broker_rpc.h
	rpcgen -C $< -h -o local_broker_rpc.h
	
local_broker_client_stub.c: local_broker.x
	rm -f local_broker_client_stub.c
	rpcgen -C $< -l -o local_broker_client_stub.c
	sed -i "s/#include \"local_broker.h\"/#include \"local_broker_rpc.h\"/" $@

local_broker_client_stub.o: local_broker_client_stub.c local_broker_rpc.h
	$(CC) $(CFLAGS) -c $< -o $@
	
libclientstub.a: local_broker_client_stub.o
	ar -ruc $@ $^

all: libclientstub.a local_broker_client_stub.o