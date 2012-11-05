#ifndef API_TORRE_DE_CONTROL_H_
#define API_TORRE_DE_CONTROL_H_

class ApiTorreDeControl {
public:
    ApiTorreDeControl(char *path_torre_control) : path_torre_control(path_torre_control) {};
    virtual ~ApiTorreDeControl() {};
    void notificar_llegada_vuelo(int numero_vuelo);

private:
    char *path_torre_control;
};

#endif