#include <iostream>
#include <cstdio> 
#include <cstring>

using namespace std;

const int K = 5;

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct Producto {
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual;
};

struct ComandaHistorica {
    char fecha[11];
    char nombreMozo[50];
    int codigoProducto;
    int cantidad;
    float comision;
};

void encriptarContrasenia(const char origen[], char destino[]) {
    int i = 0;
    while (origen[i] != '\0') {
        destino[i] = origen[i] + K;
        i++;
    }
    destino[i] = '\0';
}

void cargarMozos(Mozo mozos[], int &cantidadMozos) {
    FILE *f = fopen("mozos.dat", "rb");
    cantidadMozos = 0;

    if (f == NULL) {
        cout << "No existe mozos.dat. Es necesario generar mozos.dat antes de cargar ventas." << endl;
        return;
    }
  Mozo aux;
    while (fread(&aux, sizeof(Mozo), 1, f) == 1 && cantidadMozos < 50) {
        mozos[cantidadMozos++] = aux;
    }
    fclose(f);
}

void guardarMozos(const Mozo mozos[], int cantidadMozos) {
    FILE *f = fopen("mozos.dat", "wb");
    if (f == NULL) {
        cout << "No se pudo actualizar mozos.dat." << endl;
        return;
    }

    fwrite(mozos, sizeof(Mozo), cantidadMozos, f);
    fclose(f);
}

void cargarInventario(Producto inventario[], int &cantidadProductos) {
    FILE *f = fopen("inventario.dat", "rb");
    cantidadProductos = 0;

    if (f == NULL) {
        cout << "No existe inventario.dat. Debe generarse antes de registrar ventas." << endl;
        return;
    }

    Producto aux;
    while (fread(&aux, sizeof(Producto), 1, f) == 1 && cantidadProductos < 100) {
        inventario[cantidadProductos++] = aux;
    }
    fclose(f);
}

void guardarInventario(const Producto inventario[], int cantidadProductos) {
    FILE *f = fopen("inventario.dat", "wb");
    if (f == NULL) {
        cout << "No se pudo guardar el inventario." << endl;
        return;
    }

    fwrite(inventario, sizeof(Producto), cantidadProductos, f);
    fclose(f);
}

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

bool esFechaValida(int dia, int mes, int anio) {
   if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || anio < 1) {
        return false;
    }

    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool anioBisiesto = anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0);

    if (anioBisiesto == true && mes == 2) {
        diasPorMes[1] = 29;
    }

    if (dia > diasPorMes[mes - 1]) {
        return false;
    }
	
    return true;
}

int main (){
	int dia, mes, anio;
    char fecha[11];
	 
	 cout <<"Ingrese la fecha."<<endl;
	 cout <<"Dia: ";
	 cin >> dia;
	 cout << "Mes: ";
	 cin >> mes;
	 cout << "Anio: ";
	 cin >> anio;
	
     if (esFechaValida(dia, mes, anio) == false) {
        cout << "Fecha invalida. Verifique que el dia exista para ese mes y anio." << endl;
        return 0;
    }

    sprintf(fecha, "%02d-%02d-%04d", dia, mes, anio);

	Mozo mozos[50];
    int cantidadMozos = 0;
	cargarMozos(mozos, cantidadMozos);

	if (cantidadMozos == 0) {
        cout << "No hay mozos cargados. Primero debe generarse mozos.dat." << endl;
        return 0;
    }

	Producto inventario[100];
    int cantidadProductos = 0;
    cargarInventario(inventario, cantidadProductos);

	 if (cantidadProductos == 0) {
        cout << "No hay productos cargados. Primero debe generarse inventario.dat." << endl;
        return 0;
    }

	int idMozo;
    char pass[20];

    cout << "Ingrese el ID del mozo: ";
    cin >> idMozo;
    cout << "Ingrese la password: ";
    cin >> pass;

    Mozo mozoActual;
    if (loginMozo(mozos, cantidadMozos, idMozo, pass, mozoActual) == false) {
        cout << "Credenciales invalidas." << endl;
        return 0;
    }

cout << "Bienvenido/a: " << mozoActual.nombre << endl;

    int continuar = 1;
    float totalVentaDelMozo = 0.0f;

    while (continuar) {
        int codigoProducto;
        int cantidadVendida;

        cout << "\nIngrese codigo del producto (0 para finalizar): ";
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

	
	return 0;
}
