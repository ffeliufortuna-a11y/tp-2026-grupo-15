#include <iostream>
#include <cstdio> 
#include <cstring>
using namespace std;

struct ComandaHistorica {
    char fecha[11]; // "DD-MM-AAAA"
    char nombreMozo[50]; // el nombre completo, repetido en cada venta
    int codigoProducto; int cantidad; float comision;
};

struct Mozo { 
    int idMozo; 
    char nombre[50];
    char password[20]; 
    float totalComision; 
};

struct Comanda { 
    int idMozo;       //  Guarda el identificador numÃ©rico del mozo
    int codigoProducto; //  Guarda el cÃ³digo del producto vendido
    int cantidad;     // Guarda la cantidad vendida del producto
    float comision;   // Guarda la comisiÃ³n correspondiente a la venta
};

struct Producto {
    int codigo;         //  CÃ³digo Ãºnico del producto
    char descripcion[50]; // DescripciÃ³n del producto.
    float precio;       // Precio unitario del product.
    int stockActual;    // Stock disponible que iremos descontando
};
// Declaramos una variable global k fuera del main para usarla como corrimiento fijo en claves
int k = 5;

// Creamos esta funcion para establecer la contrasenia inicial sumandole el valor k a cada caracter.
void encriptarPassword(char origen[], char destino[]) {
    int i = 0;                                       // Inicializamos el indice para recorrer el texto caracter por caracter.
    while (origen[i] != '\0') {                      //  Bucle que recorre la cadena hasta encontrar el caracter nulo de fin.
        destino[i] = origen[i] + k;                  //  Al caracter actual le sumamos k para encriptarlo
        i++;                                         //  Avanzamos a la siguiente posicion del texto.
    }
    destino[i] = '\0';                               //  Colocamos el fin de cadena al final del texto ya encriptado.
}

// Creamos esta funcion para actualizar el stock directamente en el archivo
void actualizarStock(FILE* inventario, int codProd, int cantVendida) {
    Producto p;
    
    fseek(inventario, 0, SEEK_SET); // SEEK_SET mueve el cursor al principio del archivo para buscar el producto desde el primer registro.
      
    // Leemos registro por registro
    while (fread(&p, sizeof(Producto), 1, inventario) == 1) {
        if (p.codigo == codProd) {
            p.stockActual -= cantVendida;  // Actualizamos la variable en memoria
           
           fseek(inventario, -sizeof(Producto), SEEK_CUR); // Retrocedemos el cursor el tamanio exacto de un Producto.            
           fwrite(&p, sizeof(Producto), 1, inventario); // Sobrescribimos el registro actualizado en esa misma posicion
           
            break; //Ya quedo actualizado
        }
    }
}

int main (){
    FILE* f = fopen("comandas_historicas.dat", "rb");
    if (f == NULL) {
        cout << "No se pudo abrir el archivo." << endl;
        return 0;
    }

    // Declaramos un vec estÃ¡tico para cargar todo el archivo histÃ³rico 
    ComandaHistorica v[500];
    // Variable para llevar la cuenta exacta de cuÃ¡ntas comandas leÃ­mos del archivo.
    int cantComandas = 0;
    ComandaHistorica c;

    // Leemos el archivo histÃ³rico completo y lo guardamos en el arreglo v para ordenarlo despuÃ©s
    while (fread(&c, sizeof(ComandaHistorica), 1, f) == 1 && cantComandas < 500) {
        v[cantComandas] = c;                             //  Almacenamos cada comanda leÃ­da en la posiciÃ³n del vec.
        cantComandas++;                                  // Incrementamos el contador de comandas almacenadas
    }
    fclose(f);

    // Validamos si el arreglo quedÃ³ vacÃ­o para evitar errores de ejecuciÃ³n en el ordenamiento.
    if (cantComandas == 0) return 0;

    //Aplicamos ordenamiento burbuja para ordenar el arreglo primero por fecha y luego por nombre de mozo
    for (int i = 0; i < cantComandas - 1; i++) {
        for (int j = 0; j < cantComandas - i - 1; j++) {
            int comp = strcmp(v[j].fecha, v[j+1].fecha); //  Comparamos las fechas de dos comandas contiguas.
            if (comp > 0 || (comp == 0 && strcmp(v[j].nombreMozo, v[j+1].nombreMozo) > 0)) { 
                ComandaHistorica aux = v[j];             // Pasamos el elemento siguiente al lugar actual.
                v[j+1] = aux;                            // Asignamos el auxiliar a la siguiente posiciÃ³n.
            }
        }
    }


    Mozo lista[50];
    int cantidadMozos = 0; 

    // Modificamos el bucle para recorrer el arreglo ordenado `v` en lugar de leer directo del archivo viejo.
    for (int i = 0; i < cantComandas; i++) {
        int encontrado = 0;

        for (int j = 0; j < cantidadMozos; j++) { 
            if (strcmp(lista[j].nombre, v[i].nombreMozo) == 0) {
                encontrado = 1;
                lista[j].totalComision += v[i].comision;
            }
        }

        if (encontrado == 0) {
            lista[cantidadMozos].idMozo = cantidadMozos + 1; 
            strcpy(lista[cantidadMozos].nombre, v[i].nombreMozo); 
            lista[cantidadMozos].totalComision = v[i].comision; 
            
            // Creamos un texto temporal para guardar el ID numÃ©rico del mozo
            char passTemp[20];
            sprintf(passTemp, "%d", lista[cantidadMozos].idMozo); // Convertimos el ID entero a formato de texto para usarlo como clave inicial
            encriptarPassword(passTemp, lista[cantidadMozos].password); // Encriptamos esa contraseÃ±a inicial y la guardamos en el mozo.

            cantidadMozos++; 
        }
    }
    
    FILE* fmozos = fopen("mozos.dat", "wb");
    if (fmozos == NULL) {
        cout << "No se pudo crear el archivo" << endl;
        return 0;
    }
    
    fwrite(lista, sizeof(Mozo), cantidadMozos, fmozos);
    fclose(fmozos);
    
    cout << "Archivo mozos.dat generado exitosamente con " << cantidadMozos << " mozos." << endl;
    
    // Abrimos el inventario en modo de lectura y escritura
    FILE* inventario = fopen("inventario.dat", "rb+");
    if (inventario == NULL) {
        cout << "No se pudo abrir inventario.dat para actualizacion" << endl;
        return 0;
    }

    // Declaramos una variable para almacenar la fecha actual que se estÃ¡ procesando en el corte de control
    char fechaActual[11] = "";
    
    FILE* fDia = NULL;

    // Recorremos nuevamente el vec ordenado para hacer el corte de control por fecha
    for (int i = 0; i < cantComandas; i++) {
        // Comprobamos si cambiÃ³ la fecha respecto a la iteraciÃ³n anterior.
        if (strcmp(v[i].fecha, fechaActual) != 0) {
            if (fDia != NULL) fclose(fDia);               //  Si ya habÃ­a un archivo diario abierto, lo cerramos antes de cambiar de dÃ­a
            strcpy(fechaActual, v[i].fecha);              //  Actualizamos la fecha actual con la nueva fecha encontrada
            
            char nombreArch[30];                          // Variable temporal para construir el nombre del archivo diario
            sprintf(nombreArch, "comandas_%s.dat", fechaActual); //  Armamos el nombre  con el formato comandas_dd-mm-aaaa.dat
            fDia = fopen(nombreArch, "wb");               //  Abrimos el nuevo archivo diario 
        }

        //  Buscamos el ID numÃ©rico que le corresponde al nombre del mozo actual
        int idMozoEncontrado = 0;
        for (int j = 0; j < cantidadMozos; j++) {
            if (strcmp(lista[j].nombre, v[i].nombreMozo) == 0) {
                idMozoEncontrado = lista[j].idMozo;       // Guardamos el ID numÃ©rico hallado
                break;
            }
        }

        // Creamos una comanda con la estructura reducida 
        Comanda comandaDiaria;
        comandaDiaria.idMozo = idMozoEncontrado;         //  Asignamos el ID del mozo.
        comandaDiaria.codigoProducto = v[i].codigoProducto; //  Copiamos el codigo del producto.
        comandaDiaria.cantidad = v[i].cantidad;         // : Copiamos la cantidad vendida
        comandaDiaria.comision = v[i].comision;         //  Copiamos la comision de la venta

        if (fDia != NULL) {
            fwrite(&comandaDiaria, sizeof(Comanda), 1, fDia); //  Escribimos la comanda reducida en el archivo del dia correspondiente
        }
        
        //  Llamamos a la funcion para descontar la cantidad vendida en el inventario
          actualizarStock(inventario, comandaDiaria.codigoProducto, comandaDiaria.cantidad);    }

    //  Cerramos el ultimo archivo diario que haya quedado abierto al finalizar el bucle
    if (fDia != NULL) fclose(fDia);
    // Cerramos el archivo que mantuvimos abierto durante todo el proceso
    if (inventario != NULL) {
    	fclose(inventario);
    }

    return 0;
}
