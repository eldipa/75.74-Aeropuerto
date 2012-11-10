#include "contenedor.h"
#include <stdexcept>

#include "log.h"

Contenedor::Contenedor() :
		peso(0), cant_equipajes(0) {
}
Contenedor::~Contenedor() {
}

bool Contenedor::hay_lugar(const Equipaje& e) {
	if(this->cant_equipajes == MAX_VALIJAS_POR_CONTENEDOR){
		return false;
	}
	return (e.peso() + peso <= MAX_PESO_CONTENEDOR);
}

void Contenedor::agregar_equipaje(const Equipaje& e) {
	if (!hay_lugar(e))
		throw std::runtime_error("Contenedor: no hay lugar en el contenedor.exceso de peso");

	if (cant_equipajes > MAX_VALIJAS_POR_CONTENEDOR)
		throw std::runtime_error(
				"Contenedor.no hay lugar en el contenedor.sobrepasa la cantidad de equipajes");

	equipajes[cant_equipajes] = e;
	cant_equipajes++;
	peso += e.peso();

	Log::info("Equipaje (%d) agregado al contenedor.tengo %d equipajes, peso total %d/%d",
			e.getRfid().rfid, cant_equipajes, peso, MAX_PESO_CONTENEDOR);

}

std::vector<int> Contenedor::get_rfids() {
	std::vector<int> rfids;
	for (int i = 0; i < cant_equipajes; i++)
		rfids.push_back(equipajes[i]);
	return rfids;
}
