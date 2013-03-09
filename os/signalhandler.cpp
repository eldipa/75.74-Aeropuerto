#include "signalhandler.h"
#include <cerrno>

SignalHandler * SignalHandler::instance = NULL;
EventHandler * SignalHandler::signal_handlers[NSIG];

SignalHandler::SignalHandler() {
}

SignalHandler* SignalHandler::getInstance() {

	if (instance == NULL)
		instance = new SignalHandler();

	return instance;
}

void SignalHandler::destruir() {
	if (instance != NULL) {
		delete (instance);
		instance = NULL;
	}
}

EventHandler* SignalHandler::registrarHandler(int signum, EventHandler* eh) {
	int error;
	EventHandler * old_eh = SignalHandler::signal_handlers[signum];
	SignalHandler::signal_handlers[signum] = eh;

	struct sigaction sa;
	sa.sa_handler = SignalHandler::dispatcher;
	// inicializa la mascara de seniales a bloquear durante la ejecucion del handler como vacio

	error = sigemptyset(&sa.sa_mask);
	if (error != 0) {
		perror("No se puede vaiar la mascara\n");
		//throw Error
	}
	sa.sa_flags = 0;
	error = sigaction(signum, &sa, 0); // cambiar accion de la senial
	if (error != 0) {
		perror("No se pudo registrar el manejador de señal\n");
		//throw Error
	}
	return old_eh;
}

void SignalHandler::dispatcher(int signum) {
	if (SignalHandler::signal_handlers[signum] != 0)
		SignalHandler::signal_handlers[signum]->handleSignal(signum);
}

int SignalHandler::removerHandler(int signum) {

	SignalHandler::signal_handlers[signum] = NULL;
	return 0;
}
