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
        origen[i] + K = destino[i] ;
    }
    destino[i] = '\0';
}

int main (){
	int dia, mes, anio;

	 
	 cout <<"Ingrese la fecha."<<endl;
	 cout <<"Dia: ";
	 cin >> dia;
	 cout << "Mes: ";
	 cin >> mes;
	 cout << "Anio: ";
	 cin >> anio;

	return 0;
}
