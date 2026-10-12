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
	
    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || anio < 1900) {
        cout << "Fecha invalida." << endl;
        return 0;
    }

    sprintf(fecha, "%02d-%02d-%04d", dia, mes, anio);

	Mozo mozos[50];
    int cantidadMozos = 0;
   
	
	return 0;
}
