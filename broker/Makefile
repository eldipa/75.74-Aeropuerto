
.DEFAULT_GOAL = all

	
local_broker_server_stub.c: ../rpc/local_broker.x
	rm -f $@
	cp $< local_broker.x
	rpcgen -C local_broker.x
	rm -f local_broker.x
	mv local_broker.h local_broker_rpc.h
	rm -f local_broker_clnt.c
	mv local_broker_svc.c local_broker_server_stub.c
	sed -i "s/#include \"local_broker.h\"/#include \"local_broker_rpc.h\"/" $@

local_broker_server_stub.o: local_broker_server_stub.c local_broker_rpc.h
	$(CC) $(CFLAGS) -c $< -o $@
	
libserverstub.a: local_broker_server_stub.o
	ar -ruc $@ $^

all: local_broker_server_stub.o
