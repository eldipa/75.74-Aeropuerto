struct Operandos {
    int num_vuelo;
    int num_zona;
};

program TORREDECONTROLPROG {
    version TORREDECONTROLVERS {

    void ASIGNAR_VUELO(Operandos) = 0;

    void DESASIGNAR_VUELO(int num_vuelo) = 1;

    int GET_ZONA(int num_vuelo) = 2;

    } = 1;
} = 0x20000001;