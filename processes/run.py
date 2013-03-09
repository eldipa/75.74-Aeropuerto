#!/usr/bin/python

import cmd
import subprocess
import os
import time
import optparse
import signal
BASE = "/tmp/"
processes = []

class Aeropuerto(cmd.Cmd):
    """Simple command processor example."""

    def do_local_broker(self, line):
        """ Parameters: id_broker port """
        try:
            id_broker, port = str.split(line)

            proc_name = "localbroker"+id_broker
            wkdir = os.path.join( BASE, "wd"+proc_name )
            p = self.run_process("./../local_broker/local_broker_launcher", [proc_name, wkdir, port], "./../local_broker")

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.do_local_broker.__doc__;
        except Exception as error:
            print str(error)

        pass

    def do_message_broker(self, line):
        """ Parameters: config_file """
        try:
            config_file_name = str.split(line)
            
            proc_name = "message_broker"
            config_file = os.path.join(os.getcwd(), config_file_name)

            p = self.run_process("./message_broker_launcher", [proc_name, config_file])

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.do_message_broker.__doc__;
        except Exception as error:
            print str(error)

        pass


    def do_intermediate_broker(self, line):
        """ Parameters: config_file """
        try:
            config_file_name = str.split(line)
            
            proc_name = "intermediate_broker"
            config_file = os.path.join(os.getcwd(), config_file_name)
            p = self.run_process("./intermediate_broker_launcher", [proc_name, config_file])

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.do_intermediate_broker.__doc__;
        except Exception as error:
            print str(error)

        pass


    def do_torre_de_control(self, line):
        """ Parameters: config_file """
        try:
            config_file_name = str.split(line)
            
            proc_name = "torre_de_control"
            config_file = os.path.join(os.getcwd(), config_file_name)
            p = self.run_process("./torre_de_control_launcher", [proc_name, config_file])

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.torre_de_control_launcher.__doc__;
        except Exception as error:
            print str(error)

        pass


    def do_puesto_checkin(self, line):
        """ Parameters: id_checkin config_file """
        try:
            id_checkin, config_file_name = str.split(line)

            proc_name = "puesto_checkin"+id_checkin
            wkdir = os.path.join( BASE, "wd"+proc_name )            
            config_file = os.path.join(os.getcwd(), config_file_name)
            p = self.run_process("./puesto_checkin", [proc_name, wkdir, config_file, id_checkin, "1"])

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.puesto_checkin.__doc__;
        except Exception as error:
            print str(error)

        pass

    def do_robot_checkin(self, line):
        """ Parameters: id_robot_checkin config_file id_aeropuerto=1 id_cinta_checkin=1 """

        try:
            id_robot, config_file_name = str.split(line)
            
            proc_name = "robot_checkin"+id_robot
            wkdir = os.path.join( BASE, "wd"+proc_name )            
            config_file = os.path.join(os.getcwd(), config_file_name)
            p = self.run_process("./robot_checkin", [proc_name, wkdir, config_file, id_robot, "1", "1"])

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.robot_checkin.__doc__;
        except Exception as error:
            print str(error)

        pass


    def do_robot_scanner(self, line):
        """ Parameters: id_robot_scanner config_file """

        try:
            id_robot_scanner, config_file_name = str.split(line)
            
            proc_name = "robot_scanner"+id_robot_scanner
            wkdir = os.path.join( BASE, "wd"+proc_name )            
            config_file = os.path.join(os.getcwd(), config_file_name)
            p = self.run_process("./robot_scanner", [proc_name, wkdir, config_file, id_robot_scanner, "1", "1"])

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.robot_scanner.__doc__;
        except Exception as error:
            print str(error)

        pass

    def do_robot_despacho(self, line):
        """ Parameters: id_robot_despacho config_file zona_desde zona_hasta"""

        try:
            id_robot_despacho, config_file_name, zona_desde, zona_hasta = str.split(line)
            
            proc_name = "robot_despacho"+id_robot_despacho
            wkdir = os.path.join( BASE, "wd"+proc_name )            
            config_file = os.path.join(os.getcwd(), config_file_name)
            p = self.run_process("./robot_despacho", [proc_name, wkdir, config_file, id_robot_despacho, zona_desde, zona_hasta])

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.robot_despacho.__doc__;
        except Exception as error:
            print str(error)

        pass

    def do_robot_carga(self, line):
        """ Parameters: id_robot_carga config_file"""

        try:
            id_robot_carga, config_file_name = str.split(line)
            
            proc_name = "robot_carga"+id_robot_carga
            wkdir = os.path.join( BASE, "wd"+proc_name )            
            config_file = os.path.join(os.getcwd(), config_file_name)
            p = self.run_process("./robot_carga", [proc_name, wkdir, config_file, id_robot_carga, id_robot_carga, id_robot_carga ])

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.robot_carga.__doc__;
        except Exception as error:
            print str(error)

        pass

    def do_robot_equipaje_sospechoso(self, line):
        """ Parameters: config_file pos_cinta_central=3"""

        try:
            config_file_name = str.split(line)
            
            proc_name = "robot_control_equipaje_sospechoso"
            wkdir = os.path.join( BASE, "wd"+proc_name )            
            config_file = os.path.join(os.getcwd(), config_file_name)
            p = self.run_process("./robot_carga", [proc_name, wkdir, config_file, "3" ])

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.do_robot_equipaje_sospechoso.__doc__;
        except Exception as error:
            print str(error)

        pass


    def do_robot_intercargo(self, line):
        """ Parameters: config_file"""
        try:
            config_file_name = str.split(line)
            
            proc_name = "robot_intercargo"
            wkdir = os.path.join( BASE, "wd"+proc_name )            
            config_file = os.path.join(os.getcwd(), config_file_name)
            p = self.run_process("./robot_intercargo", [proc_name, wkdir, config_file])

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.robot_intercargo.__doc__;
        except Exception as error:
            print str(error)

        pass

    def do_generador_trasbordo(self, line):
        """ Parameters: config_file path_vuelos_entrantes path_vuelos_interconexion"""
        try:
            config_file_name, path_entrantes, path_interconexion = str.split(line)
            
            proc_name = "generador_vuelos_trasbordo"
            wkdir = os.path.join( BASE, "wd"+proc_name )            
            config_file = os.path.join(os.getcwd(), config_file_name)
            file_vuelos_entrantes = os.path.join(os.getcwd(), path_entrantes)
            file_vuelos_interconexion = os.path.join(os.getcwd(), path_interconexion)

            p = self.run_process("./robot_intercargo", [proc_name, wkdir, config_file, file_vuelos_entrantes, file_vuelos_interconexion])

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.do_generador_trasbordo.__doc__;
        except Exception as error:
            print str(error)

        pass


    def do_scheduler_vuelos(self, line):
        """ Parameters: config_file"""
        try:
            config_file_name = str.split(line)
            
            proc_name = "scheduler_vuelos"
            wkdir = os.path.join( BASE, "wd"+proc_name )            
            config_file = os.path.join(os.getcwd(), config_file_name)
            p = self.run_process("./scheduler_vuelos", [proc_name, wkdir, config_file])

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.do_scheduler_vuelos.__doc__;
        except Exception as error:
            print str(error)

        pass

    def do_scheduler_aviones(self, line):
        """ Parameters: config_file"""
        try:
            config_file_name = str.split(line)
            
            proc_name = "scheduler_aviones"
            wkdir = os.path.join( BASE, "wd"+proc_name )            
            config_file = os.path.join(os.getcwd(), config_file_name)
            p = self.run_process("./scheduler_aviones", [proc_name, wkdir, config_file])

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.do_scheduler_aviones.__doc__;
        except Exception as error:
            print str(error)

        pass


    def do_tractor (self, line):
        """ Parameters: id_tractor config_file"""
        try:
            id_tractor, config_file_name = str.split(line)
            
            proc_name = "tractor"+id_tractor
            wkdir = os.path.join( BASE, "wd"+proc_name )            
            config_file = os.path.join(os.getcwd(), config_file_name)
            p = self.run_process("./tractor", [proc_name, wkdir, config_file, id_tractor])

            if(p):
                processes.append(p)

        except ValueError:
            print "Bad Argument:"+self.do_tractor.__doc__;
        except Exception as error:
            print str(error)

        pass


    def do_list(self, line):
        print "processes ("+str(len(processes))+")"
        for p in processes:
            print "\t"+str(p[0])+" pid="+str(p[1])
        return False
           
    def do_exit(self, line):
        print "Finalizando procesos.Enviando SIGTERM..."
        #        for p in processes:
        #            print "kill "+str(p[1])
        #            os.kill(p[1], signal.SIGTERM)


        print "Finalizando procesos.Enviando SIGKILL..."
        time.sleep(2)
        for p in processes:
            print "kill "+str(p[1])
            os.kill(p[1], signal.SIGKILL)


        return True
           
    def do_shell(self, line):
        "Run a shell command"
        print "running shell command:", line
        output = os.popen(line).read()
        print output
        self.last_output = output

    def postloop(self):
        print "EXIT"


    def run_process(self, proc_file, args, chdir="./"):
        pid = os.fork()

        if pid == 0:
            os.chdir(chdir);
            os.execv(proc_file, args)
            raise Exception("The processes image cannot be loaded")
        elif pid == -1:
            raise Exception("The processes children cannot be created")

        return [args[0], pid]

if __name__ == '__main__':
    try:
        Aeropuerto().cmdloop()
    except Exception as error:
        print str(error)

        print "Finalizando procesos.Enviando SIGTERM..."
        time.sleep(5)
        for p in processes:
            p.terminate()

        print "Finalizando procesos.Enviando SIGKILL..."
        time.sleep(2)
        for p in processes:
            p.kill()
