/*
 * msg_ida.x: Ejemplo de envio de un mensaje con  codigo de retorno solamente
 */

/*
 * Los numeros después de las funciones tienen que ser diferente ya que se
 * utilizan en un switch del lado del servidor
 */
program MESSAGEPROG {
	version MESSAGEVERS {
		int PRINTMESSAGE(string) = 1;
	} = 1;
} = 0x20000000;
