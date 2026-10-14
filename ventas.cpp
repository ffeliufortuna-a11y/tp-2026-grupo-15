#include <iostream>
#include <cstdio>
#include <cstring>
#include <cerrno>

using namespace std;

// Declaramos una variable global K fuera del main para usarla como corrimiento fijo en claves
const int K = 5;

struct Mozo {
    int idMozo;             // Identificador unico del mozo.
    char nombre[50];        // Nombre completo del mozo.
    char password[20];      // Contrasenia almacenada con el corrimiento definido por K.
    float totalComision;    // Comision acumulada desde que comenzo a trabajar.
};

//  Creamos esta estructura para manejar el inventario y poder actualizar el stock
struct Producto {
    int codigo;         //  Código único del producto
    char descripcion[50]; // Descripción del producto.
    float precio;       // Precio unitario del product.
    int stockActual;    // Stock disponible que iremos descontando
};

// Registro que se guarda en la planilla diaria, usando el ID del mozo en lugar de su nombre.
struct Comanda {
    int idMozo;             // Mozo que realizo la venta.
    int codigoProducto;     // Codigo del producto vendido.
    int cantidad;           // Unidades vendidas.
    float comision;         // Comision generada por esta venta.
};

// Aplica un corrimiento fijo a cada caracter de la contrasenia.
void encriptarContrasenia(const char origen[], char destino[]) {
    int i = 0; // Inicializamos el índice para recorrer el texto carácter por carácter.
    while (origen[i] != '\0') { //  Bucle que recorre la cadena hasta encontrar el carácter nulo de fin.
        destino[i] = origen[i] + K; //  Al carácter actual le sumamos K para encriptarlo
        i++; //  Avanzamos a la siguiente posición del texto.
    }
    destino[i] = '\0'; //  Colocamos el fin de cadena al final del texto ya encriptado.
}

// Carga en memoria los mozos existentes en mozos.dat.
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

// Sobrescribe mozos.dat con los datos actualizados, incluida la comision acumulada.
bool guardarMozos(const Mozo mozos[], int cantidadMozos) {
    FILE *f = fopen("mozos.dat", "wb"); //abre el archivo
    if (f == NULL) { //se asegura que exista el archivo, sino termina el programa
        cout << "No se pudo actualizar mozos.dat." << endl;
        return false;
    }

    bool guardado = fwrite(mozos, sizeof(Mozo), cantidadMozos, f) == (size_t)cantidadMozos; //actualiza mozos
    if (fclose(f) != 0) { //cierra mozos.dat
        guardado = false;
    }
    if (!guardado) {
        cout << "Error al escribir mozos.dat." << endl;
    }
    return guardado;
}

// Carga en memoria los productos y el stock actual de inventario.dat.
void cargarInventario(Producto inventario[], int &cantidadProductos) {
    FILE *f = fopen("inventario.dat", "rb"); //abre el archivo inventario.dat
    cantidadProductos = 0; //contador de productos

    if (f == NULL) { //se asegura de que exista invetario.dat sino finaliza el programa
        cout << "No existe inventario.dat. Debe generarse antes de registrar ventas." << endl;
        return;
    }

    Producto aux; // auxiliar para extraer los productos del while (para inventario.dat)
    while (fread(&aux, sizeof(Producto), 1, f) == 1 && cantidadProductos < 100) {
        inventario[cantidadProductos++] = aux;
    }
    fclose(f);
}

// Sobrescribe inventario.dat con el stock actualizado.
bool guardarInventario(const Producto inventario[], int cantidadProductos) {
    FILE *f = fopen("inventario.dat", "wb"); //abro el archivo inventario.dat
    if (f == NULL) { //me aseguro que exista el archivo, sino finaliza programa
        cout << "No se pudo guardar el inventario." << endl;
        return false;
    }

    bool guardado = fwrite(inventario, sizeof(Producto), cantidadProductos, f) == (size_t)cantidadProductos; //actualizo stock
    if (fclose(f) != 0) {
        guardado = false;
    }
    if (!guardado) {
        cout << "Error al escribir inventario.dat." << endl;
    }
    return guardado;
}

// Encripta la clave ingresada y comprueba que coincida con la del mozo indicado.
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

// Busca un mozo por ID y devuelve su direccion, o NULL si no existe.
Mozo *buscarMozo(Mozo mozos[], int cantidadMozos, int idMozo) {
    for (int i = 0; i < cantidadMozos; i++) {
        if (mozos[i].idMozo == idMozo) {
            return &mozos[i];
        }
    }
    return NULL;
}

// Comprueba que la fecha exista en el calendario, incluidos los anios bisiestos.
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

// Ordena la planilla diaria en forma ascendente por ID de mozo.
void ordenarComandasPorMozo(Comanda planilla[], int cantidad) {
    for (int i = 0; i < cantidad - 1; i++) {
        for (int j = 0; j < cantidad - i - 1; j++) {
            if (planilla[j].idMozo > planilla[j + 1].idMozo) {
                Comanda aux = planilla[j];
                planilla[j] = planilla[j + 1];
                planilla[j + 1] = aux;
            }
        }
    }
}

// Lee la planilla del dia. Si aun no existe, la considera vacia para la primera venta.
bool cargarPlanillaDiaria(const char fecha[], Comanda planilla[], int &cantidad) {
    char nombreArchivo[40];
    snprintf(nombreArchivo, sizeof(nombreArchivo), "comandas_%s.dat", fecha);

    FILE *f = fopen(nombreArchivo, "rb");
    cantidad = 0;
    if (f == NULL) {
        if (errno == ENOENT) {
            return true;
        }
        cout << "No se pudo abrir la planilla diaria para leerla." << endl;
        return false;
    }

    Comanda aux;
    while (cantidad < 500 && fread(&aux, sizeof(Comanda), 1, f) == 1) {
        planilla[cantidad++] = aux;
    }
    if (ferror(f)) {
        cout << "Error al leer la planilla diaria." << endl;
        fclose(f);
        return false;
    }
    if (cantidad == 500 && fread(&aux, sizeof(Comanda), 1, f) == 1) {
        cout << "La planilla diaria supera el limite de registros." << endl;
        fclose(f);
        return false;
    }
    fclose(f);
    ordenarComandasPorMozo(planilla, cantidad);
    return true;
}

// Guarda la planilla diaria completa; informa si no pudo escribir todos los registros.
bool guardarPlanillaDiaria(const char fecha[], const Comanda planilla[], int cantidad) {
    // Se abre en modo escritura porque la planilla ya contiene las ventas anteriores y la nueva.
    char nombreArchivo[40];
    snprintf(nombreArchivo, sizeof(nombreArchivo), "comandas_%s.dat", fecha);

    FILE *f = fopen(nombreArchivo, "wb");
    if (f == NULL) {
        cout << "No se pudo guardar la planilla del dia." << endl;
        return false;
    }

    bool guardado = fwrite(planilla, sizeof(Comanda), cantidad, f) == (size_t)cantidad;
    if (fclose(f) != 0) {
        guardado = false;
    }
    if (!guardado) {
        cout << "Error al escribir la planilla del dia." << endl;
    }
    return guardado;
}

int main() {
    int dia, mes, anio;
    char fecha[11]; 

	// Se solicita una sola fecha para todas las ventas de esta tanda.
	 cout <<"Ingrese la fecha."<<endl;
	 cout <<"Dia: ";
	 cin >> dia;
	 cout << "Mes: ";
	 cin >> mes;
	 cout << "Anio: ";
	 cin >> anio;

	// Se valida la fecha antes de usarla para nombrar la planilla.
     if (esFechaValida(dia, mes, anio) == false) {
        cout << "Fecha invalida. Verifique que el dia exista para ese mes y anio." << endl;
        return 0; //si no es valida la fecha termina el programa, sino continua
    }

    sprintf(fecha, "%02d-%02d-%04d", dia, mes, anio); // Arma la fecha con dia y mes de dos digitos.

	Mozo mozos[50]; // Arreglo con el maximo de mozos que se cargan en memoria.
    int cantidadMozos = 0; // Cantidad de registros leidos desde mozos.dat.
	cargarMozos(mozos, cantidadMozos); // Carga los mozos para validar las ventas.

	if (cantidadMozos == 0) { //verifico si hay mozos
        cout << "No hay mozos cargados. Primero debe generarse mozos.dat." << endl;
        return 0; //si no hay mozos termino el programa, sino continua
    }

    Producto inventario[100]; // Arreglo con el maximo de productos que se cargan en memoria.
    int cantidadProductos = 0; // Cantidad de registros leidos desde inventario.dat.
    cargarInventario(inventario, cantidadProductos); // Carga productos y stock para validar cada venta.

	 if (cantidadProductos == 0) { //verifico si hay productos
        cout << "No hay productos cargados. Primero debe generarse inventario.dat." << endl;
        return 0; //si no hay productos termino el programa, sino continua
    }

    bool continuar = true;

    while (continuar) {
        int opcion;
        cout << "\n--- Menu de ventas ---" << endl;
        cout << "1. Registrar venta" << endl;
        cout << "0. Finalizar" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                // Cada venta requiere identificar y autenticar al mozo que la registra.
                int idMozo;
                char pass[20];
                int codigoProducto;
                int cantidadVendida;

                cout << "Ingrese el ID del mozo: ";
                cin >> idMozo;
                if (buscarMozo(mozos, cantidadMozos, idMozo) == NULL) {
                    cout << "No existe un mozo con ese ID." << endl;
                    break;
                }

                cout << "Ingrese la password: ";
                cin >> pass;
                Mozo mozoActual;
                if (!loginMozo(mozos, cantidadMozos, idMozo, pass, mozoActual)) {
                    cout << "Credenciales invalidas." << endl;
                    break;
                }

                cout << "Bienvenido/a: " << mozoActual.nombre << endl;
                cout << "Ingrese codigo del producto: ";
                cin >> codigoProducto;

                Producto *producto = buscarProducto(inventario, cantidadProductos, codigoProducto);
                if (producto == NULL) {
                    cout << "Producto inexistente." << endl;
                    break;
                }

                cout << "Descripcion: " << producto->descripcion << endl;
                cout << "Precio unitario: $" << producto->precio << endl;
                cout << "Stock actual: " << producto->stockActual << endl;
                cout << "Cantidad a vender: ";
                cin >> cantidadVendida;

                if (cantidadVendida <= 0) {
                    cout << "La cantidad debe ser mayor a cero." << endl;
                    break;
                }
                if (cantidadVendida > producto->stockActual) {
                    cout << "No hay stock suficiente para esa venta." << endl;
                    break;
                }

                // La comision se calcula como el 10% del subtotal de la venta.
                float subtotal = producto->precio * cantidadVendida;
                float comision = subtotal * 0.10f;

                // Se prepara el registro diario con el ID numerico del mozo.
                Comanda comandaDiaria;
                comandaDiaria.idMozo = mozoActual.idMozo;
                comandaDiaria.codigoProducto = codigoProducto;
                comandaDiaria.cantidad = cantidadVendida;
                comandaDiaria.comision = comision;

                Comanda planillaDiaria[500];
                int cantidadEnPlanilla = 0;
                // Se conserva la planilla existente, se agrega la venta y se guarda ordenada.
                if (!cargarPlanillaDiaria(fecha, planillaDiaria, cantidadEnPlanilla)) {
                    break;
                }
                if (cantidadEnPlanilla >= 500) {
                    cout << "La planilla diaria alcanzo el limite de registros." << endl;
                    break;
                }
                planillaDiaria[cantidadEnPlanilla++] = comandaDiaria;
                ordenarComandasPorMozo(planillaDiaria, cantidadEnPlanilla);
                if (!guardarPlanillaDiaria(fecha, planillaDiaria, cantidadEnPlanilla)) {
                    break;
                }

                // Solo despues de guardar la comanda se actualizan stock y comision acumulada.
                producto->stockActual -= cantidadVendida;
                for (int i = 0; i < cantidadMozos; i++) {
                    if (mozos[i].idMozo == mozoActual.idMozo) {
                        mozos[i].totalComision += comision;
                        mozoActual.totalComision = mozos[i].totalComision;
                        break;
                    }
                }

                bool inventarioGuardado = guardarInventario(inventario, cantidadProductos);
                bool mozosGuardados = guardarMozos(mozos, cantidadMozos);
                if (!inventarioGuardado || !mozosGuardados) {
                    cout << "La comanda diaria se guardo, pero no se pudieron guardar todos los archivos auxiliares. Se finaliza la carga para evitar inconsistencias." << endl;
                    continuar = false;
                    break;
                }

                cout << "Venta registrada correctamente." << endl;
                cout << "Subtotal: $" << subtotal << endl;
                cout << "Comision del mozo: $" << comision << endl;
                cout << "Comision total acumulada del mozo: $" << mozoActual.totalComision << endl;
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

    return 0;
}
