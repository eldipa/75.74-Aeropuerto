#!/usr/bin/python

import sys, os, csv
import sqlite3

SEP = ":"
FILE_VUELOS = "vuelos.csv"
FILE_PASAJEROS = "pasajeros.csv"
FILE_EQUIPAJES = "equipajes.csv"

def usage():
    print "usage: ./generar_csv.py [v:p:e]..."
    print "v: numero de vuelo"
    print "p: cantidad de pasajeros del vuelo"
    print "e: cantidad de equipajes por cada pasajero"
    print ""
    print "id del pasajero=numero_de_vuelo+numero_de_pasajero"
    print "id del equipaje=numero_de_pasajero+numero_de_equipaje"
    print ""
    print "Ejemplo: ./generar_csv 1:2:2 3:3:5"
    print "\t Genera un vuelo con id=1, y dos pasajeros con 2 equipajes cada uno."
    print "\t Otro vuelo con id=3, 3 pasajeros y 5 equipajes por cada uno"

if __name__ == '__main__':
    
    if ((len(sys.argv)==1) or (sys.argv[1] =="--help")):
        usage()
        exit(0);
        
    
    file_vuelos = open(FILE_VUELOS,"w")
    file_pasajeros = open(FILE_PASAJEROS, "w")
    file_equipajes = open(FILE_EQUIPAJES, "w")
    
    csv_vuelos = csv.writer(file_vuelos, delimiter=SEP)
    csv_pasajeros = csv.writer(file_pasajeros, delimiter=SEP)
    csv_equipajes = csv.writer(file_equipajes, delimiter=SEP)

    for i in sys.argv[1:]:
        try:
            num_vuelo, cant_pasajeros, cant_equipajes = i.split(":")

            print "vuelo="+num_vuelo
            csv_vuelos.writerow((num_vuelo, "compania", "origen", "destino", "", "", "", 0))

            for num_pasajero in range(0,int(cant_pasajeros)):
                id_pasajero = num_vuelo+str(num_pasajero)

                print "\t"+id_pasajero
                csv_pasajeros.writerow((id_pasajero, id_pasajero*4, id_pasajero, num_vuelo))

                for numero_equipaje in range(0, int(cant_equipajes)):
                    id_equipaje = id_pasajero+str(numero_equipaje)

                    print "\t\t"+id_equipaje
                    csv_equipajes.writerow((id_equipaje, id_pasajero, num_vuelo))
        except ValueError:
            pass
                
    file_vuelos.close()
    file_pasajeros.close()
    file_equipajes.close()
