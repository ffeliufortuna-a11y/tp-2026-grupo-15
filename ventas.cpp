#include <iostream>
#include <cstdio> 
#include <cstring>

using namespace std;

// Declaramos una variable global K fuera del main para usarla como corrimiento fijo en claves
const int K = 5;

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20]; // agregamos este campo para almacenar la clave encriptada, exigido por el TP para el login seguro
    float totalComision;
}; 

//  Creamos esta estructura para manejar el inventario y poder actualizar el stock
struct Producto {
    int codigo;         //  Código único del producto
    char descripcion[50]; // Descripción del producto.
    float precio;       // Precio unitario del product.
    int stockActual;    // Stock disponible que iremos descontando
};

struct ComandaHistorica {
    char fecha[11]; // "DD-MM-AAAA"
    char nombreMozo[50]; // el nombre completo, repetido en cada venta
    int codigoProducto;
    int cantidad;
    float comision;
};

// Creamos esta función para cifrar la contraseña inicial sumándole el valor K a cada carácter.
void encriptarContrasenia(const char origen[], char destino[]) {
    int i = 0; // Inicializamos el índice para recorrer el texto carácter por carácter.
    while (origen[i] != '\0') { //  Bucle que recorre la cadena hasta encontrar el carácter nulo de fin.
        destino[i] = origen[i] + K; //  Al carácter actual le sumamos K para encriptarlo
        i++; //  Avanzamos a la siguiente posición del texto.
    }
    destino[i] = '\0'; //  Colocamos el fin de cadena al final del texto ya encriptado.
}

//cargamos los mozos existentes de mozos.dat
void cargarMozos(Mozo mozos[], int &cantidadMozos) { 
    FILE *f = fopen("mozos.dat", "rb"); //abre el archivo
    cantidadMozos = 0; //contador de mozos que se va a actualizar a medida que se carguen los mozos

    if (f == NULL) { //si no hay mozos no se puede continuar asi que termina el programa
        cout << "No existe mozos.dat. Es necesario generar mozos.dat antes de cargar ventas." << endl;
        return;
    }
	
  Mozo aux; //aux para no perder los datos fuera del while
    while (fread(&aux, sizeof(Mozo), 1, f) == 1 && cantidadMozos < 50) { //lectura del .dat y cuenta la cantidad de mozos
        mozos[cantidadMozos++] = aux;
    }
    fclose(f); //cierra mozos.dat
}

// actualiza las comisiones de el/Los mozo/s
void guardarMozos(const Mozo mozos[], int cantidadMozos) {
    FILE *f = fopen("mozos.dat", "wb"); //abre el archivo
    if (f == NULL) { //se asegura que exista el archivo, sino termina el programa
        cout << "No se pudo actualizar mozos.dat." << endl;
        return;
    }

    fwrite(mozos, sizeof(Mozo), cantidadMozos, f); //actualiza mozos
    fclose(f); //cierra mozos.dat
}

//cargo todo el inventario para futuramente validar que exista 
void cargarInventario(Producto inventario[], int &cantidadProductos) {
    FILE *f = fopen("inventario.dat", "rb"); //abre el archivo inventario.dat
    cantidadProductos = 0; //contador de productos

    if (f == NULL) { //se asegura de que exista invetario.dat sino termina el programa
        cout << "No existe inventario.dat. Debe generarse antes de registrar ventas." << endl;
        return;
    }

    Producto aux; // auxiliar para extraer los productos del while (para inventario.dat)
    while (fread(&aux, sizeof(Producto), 1, f) == 1 && cantidadProductos < 100) {
        inventario[cantidadProductos++] = aux;
    }
    fclose(f);
}

//actualizo el stock 
void guardarInventario(const Producto inventario[], int cantidadProductos) {
    FILE *f = fopen("inventario.dat", "wb"); //abro el archivo inventario.dat
    if (f == NULL) { //me aseguro que exista el archivo, sino cierra programa
        cout << "No se pudo guardar el inventario." << endl;
        return;
    }

    fwrite(inventario, sizeof(Producto), cantidadProductos, f); //actualizo stock
    fclose(f);
}

//validar que sea correcta la contraseña
bool loginMozo(Mozo mozos[], int cantidadMozos, int idIngresado, const char passIngresada[], Mozo &mozoLogueado) {
    char passEncriptada[20];
    encriptarContrasenia(passIngresada, passEncriptada);

    for (int i = 0; i < cantidadMozos; i++) {
        if (mozos[i].idMozo == idIngresado && strcmp(mozos[i].password, passEncriptada) == 0) {
            mozoLogueado = mozos[i];
            return true;
        }
    }

    return false;
}

Producto *buscarProducto(Producto inventario[], int cantidadProductos, int codigo) {
    for (int i = 0; i < cantidadProductos; i++) {
        if (inventario[i].codigo == codigo) {
            return &inventario[i];
        }
    }
    return NULL;
}

//me aseguro que la fecha ingresada sea valida
bool esFechaValida(int dia, int mes, int anio) {
   if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || anio < 1) { //si no pasa los criterios minimos para ser una fecha valida se retorna
        return false;
    }

    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; //dias que tiene "cada mes" sin contar años biciestos
    bool anioBisiesto = anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0); //verifico si hay año biciesto

    if (anioBisiesto == true && mes == 2) { //si es año biciesto aumento el contador de febrero en 1
        diasPorMes[1] = diasPorMes[1] + 1;
    }

    if (dia > diasPorMes[mes - 1]) { //me aseguro que ningun mes supere los dias respectivos a la lista
        return false; //si no verifica retorna
    }
	
    return true; //verifica que la fecha es valida y retorna
}

int main (){
	int dia, mes, anio;
    char fecha[11]; //lista de los 12 meses

	//ingreso la fecha en orden dia-mes-anio
	 cout <<"Ingrese la fecha."<<endl;
	 cout <<"Dia: ";
	 cin >> dia;
	 cout << "Mes: ";
	 cin >> mes;
	 cout << "Anio: ";
	 cin >> anio;

	//llamo a la funcion esFechaValida para validar que exista la fecha
     if (esFechaValida(dia, mes, anio) == false) {
        cout << "Fecha invalida. Verifique que el dia exista para ese mes y anio." << endl;
        return 0; //si no es valida la fecha termina el programa, sino continua
    }

    sprintf(fecha, "%02d-%02d-%04d", dia, mes, anio); //formato de la fecha

	Mozo mozos[50]; //establezco la cantidad de mozos limite
    int cantidadMozos = 0; //contador de mozos existentes
	cargarMozos(mozos, cantidadMozos); //llamo a la funcion cargarMozos para cargar los mozos existentes

	if (cantidadMozos == 0) { //verifico si hay mozos
        cout << "No hay mozos cargados. Primero debe generarse mozos.dat." << endl;
        return 0; //si no hay mozos termino el programa, sino continua
    }

	Producto inventario[100]; //establezco limite de productos 
    int cantidadProductos = 0; //contador de productos existentes
    cargarInventario(inventario, cantidadProductos); //llamo a la funcion cargarInventario para cargar el inventario existente

	 if (cantidadProductos == 0) { //verifico si hay productos
        cout << "No hay productos cargados. Primero debe generarse inventario.dat." << endl;
        return 0; //si no hay productos termino el programa, sino continua
    }

	int idMozo;
    char pass[20]; //limite del pass

	//Ingreso los datos del mozo
    cout << "Ingrese el ID del mozo: ";
    cin >> idMozo;
    cout << "Ingrese la password: ";
    cin >> pass;

    Mozo mozoActual;
    if (loginMozo(mozos, cantidadMozos, idMozo, pass, mozoActual) == false) { //valido al mozo
        cout << "Credenciales invalidas." << endl;
        return 0; //si no se verifica los datos del mozo, finaliza el programa
    }

cout << "Bienvenido/a: " << mozoActual.nombre << endl; //si se verifico los datos da la bienvenida

    int continuar = true;
    float totalVentaDelMozo = 0.0f; //contador de ventas

    while (continuar) {
        int opcion;
        cout << "\n--- Menu de ventas ---" << endl;
        cout << "1. Registrar venta" << endl;
        cout << "2. Cambiar de mozo" << endl;
        cout << "0. Finalizar" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                while (true) {
                    int codigoProducto;
                    int cantidadVendida;

                    cout << "\nIngrese codigo del producto (0 para volver al menu): ";
                    cin >> codigoProducto;

                    if (codigoProducto == 0) {
                        break;
                    }

                    Producto *producto = buscarProducto(inventario, cantidadProductos, codigoProducto);
                    if (producto == NULL) {
                        cout << "Producto inexistente." << endl;
                        continue;
                    }

                    cout << "Descripcion: " << producto->descripcion << endl;
                    cout << "Precio unitario: $" << producto->precio << endl;
                    cout << "Stock actual: " << producto->stockActual << endl;
                    cout << "Cantidad a vender: ";
                    cin >> cantidadVendida;

                    if (cantidadVendida <= 0) {
                        cout << "La cantidad debe ser mayor a cero." << endl;
                        continue;
                    }

                    if (cantidadVendida > producto->stockActual) {
                        cout << "No hay stock suficiente para esa venta." << endl;
                        continue;
                    }

                    float subtotal = producto->precio * cantidadVendida;
                    float comision = subtotal * 0.10f;

                    ComandaHistorica registro;
                    strcpy(registro.fecha, fecha);
                    strcpy(registro.nombreMozo, mozoActual.nombre);
                    registro.codigoProducto = codigoProducto;
                    registro.cantidad = cantidadVendida;
                    registro.comision = comision;

                    FILE *fHistorico = fopen("comandas_historicas.dat", "ab");
                    if (fHistorico == NULL) {
                        cout << "No se pudo abrir comandas_historicas.dat." << endl;
                        return 0;
                    }

                    fwrite(&registro, sizeof(ComandaHistorica), 1, fHistorico);
                    fclose(fHistorico);

                    producto->stockActual -= cantidadVendida;
                    totalVentaDelMozo += subtotal;

                    for (int i = 0; i < cantidadMozos; i++) {
                        if (mozos[i].idMozo == mozoActual.idMozo) {
                            mozos[i].totalComision += comision;
                            mozoActual.totalComision = mozos[i].totalComision;
                            break;
                        }
                    }

                    guardarInventario(inventario, cantidadProductos);
                    guardarMozos(mozos, cantidadMozos);

                    cout << "Venta registrada correctamente." << endl;
                    cout << "Subtotal: $" << subtotal << endl;
                    cout << "Comision del mozo: $" << comision << endl;
                    cout << "Total acumulado del mozo en esta sesion: $" << totalVentaDelMozo << endl;
                }
                break;
            }

            case 2: {
                int nuevoId;
                char nuevaPass[20];
                Mozo nuevoMozo;

                cout << "Ingrese el ID del nuevo mozo: ";
                cin >> nuevoId;
                cout << "Ingrese la password: ";
                cin >> nuevaPass;

                if (loginMozo(mozos, cantidadMozos, nuevoId, nuevaPass, nuevoMozo)) {
                    mozoActual = nuevoMozo;
                    totalVentaDelMozo = 0.0f;
                    cout << "Ahora opera: " << mozoActual.nombre << endl;
                } else {
                    cout << "Credenciales invalidas. Se mantiene el mozo actual." << endl;
                }
                break;
            }

            case 0:
                continuar = false;
                break;

            default:
                cout << "Opcion invalida." << endl;
                break;
        }
    }

	cout << "\nSe cerró la carga de ventas del dia " << fecha << "." << endl;
    cout << "Comision total del mozo en la sesion: $" << mozoActual.totalComision << endl;
	
	return 0;
}
