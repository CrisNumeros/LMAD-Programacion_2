#include <Windows.h>
#include <fstream>
#include "commctrl.h"
#include <CommDlg.h>
#include "resource.h"
#include <mmsystem.h>
#include <ctime>
#include <processenv.h>

#pragma comment(lib, "winmm.lib")

using namespace std;



struct CDireccion
{
	char Calle[40];
	char Num[40];
	char Colonia[40];
	char Municipio[40];
	char Estado[40];
};

struct Contactos
{
	Contactos *ant;
	char Nombre[40];
	CDireccion Direccion;
	bool Genero; // True = Hombre  --  False = Mujer
	char Correo[40];
	char Telefono[40];
	char Celular[40];
	char Foto1[MAX_PATH];
	char Foto2[MAX_PATH];
	char Categoria[40];
	char Orden[10];
	Contactos *sig;
}*Prin, *Aux, *Aux2, *Fin;

struct AFecha
{
	char Dia[4];
	char Mes[20];
	char Año[6];
	char Hora[4];
	char Minuto[4];
};

struct Alarmas
{
	Alarmas *ant;
	char Titulo[40];
	char NombreDelContacto[40];
	char Descripcion[MAX_PATH];
	AFecha Fecha;
	char Ubicacion[MAX_PATH];
	char Tipo[4];
	char Tono[MAX_PATH];
	Alarmas *sig;
}*APrin, *AAux, *AAux2, *ASonando, *ASonando2,*AFin;

HWND VP = 0;
HINSTANCE hCalendario = 0;
HINSTANCE hCalendario2 = 0;
HINSTANCE ADDCont = 0;
HINSTANCE hEventos = 0;
HINSTANCE hAlarma = 0;
HINSTANCE hTEventos = 0;
bool BoolAlarma = false;
bool EsArchivo = false;
bool ExpiroAlarma = false;
int ParametroGlobal = 0;
int SemanalDiaDomingo = 0;
int SemanalMes = 0;
int SemanalAño = 0;
int IntOrden = 0;
char EventoSeleccionado_Txt[40] = "";
int DiaGlobalSeleccionado = 0, MesGlobalSeleccionado = 0, AñoGlobalSeleccionado = 0, HoraGlobalSeleccionado = 0;
bool SeleccioneDesdeCalendario = false, SeleccioneDesdeSemanal = false;;
bool MostrarAlarmaSeleccionada = false;
char charOrden[10] = "";
bool NoDectetaMP3 = false;
bool ADDContactos_MostrarDatos = false;
bool ADDContactos_EliminarDato = false;
bool ADDContactos_ModificarDatos = false;
bool DiasCalendario_Eventos[6][7] = {
	{ false, false, false, false, false, false, false},
	{ false, false, false, false, false, false, false},
	{ false, false, false, false, false, false, false },
	{ false, false, false, false, false, false, false },
	{ false, false, false, false, false, false, false },
	{ false, false, false, false, false, false, false },
};
char TodasLasDire[MAX_PATH] = "";
char DireccionPrograma_Principio[MAX_PATH] = "";
char BMPCarpeta[MAX_PATH] = "";
char FILECarpeta[MAX_PATH] = "";
char DireccionPrograma_Alarmas[MAX_PATH] = "";
char DireccionPrograma_Despertador1[MAX_PATH] = "";
char DireccionPrograma_Alarma1[MAX_PATH] = "";
char DireccionPrograma_Alarma2[MAX_PATH] = "";
char DireccionPrograma_Alarma3[MAX_PATH] = "";
char DireccionPrograma_Contactos[MAX_PATH] = "";
char DireccionPrograma_Categorias[MAX_PATH] = "";
char bmp_Default[MAX_PATH] = "";

time_t rawtime;
struct tm timeinfo;

void AgregarEvento(Alarmas Datos);
void ModificarEvento(Alarmas Datos, Alarmas *Aux);
void ModificarContacto(char Nombre[], char DCalle[], char DNum[], char DColonia[], char DMunicipio[], char DEstado[], bool Genero, char Correo[], char Telefono[], char Celular[], char Foto1[], char Foto2[], char Categoria[]);
void BinarioCargar();
void BinarioGuardar();
void SeleccionarArchivo(HWND Ventana, int Filtro, char Archivo[]);
void DetectarAlarma(int Parte, UINT DT, HWND hWnd,bool *Alarma);
void CategoriaGuardar(HWND hWnd, UINT IDC, UINT ID_EDIT, char Categoria[], bool Modificar,int Cursel, bool Eliminar);
void ContactoOrdenarPor(int Parametro);
void MoverOrdenarPor(Contactos Temporal);
void EliminarContacto(Contactos *Usuario);
void AlarmaCargar();
void AlarmaGuardar();
void OrdenarAlarmas();
void MoverAlarmas();
void ReCargarTodosEventos(HWND hWnd);
void BotonesdelCalendario(HWND hWnd,UINT IDs, int Dias, int Semanas);
void VerificarAlarmasViejas(HWND hWnd);
void EliminarAlarma(Alarmas *Alarmita);
void ProximoEvento(HWND hWnd);
void PrincipioDireccion();
void EstablecerCalendario(HWND hWnd, int Parametro, int Mes, int UltimoDia, int Año, int TuDia, int TuMes, int TuAño);
void ReCargarContactos(HWND hWnd);
void ReCargarCategorias(HWND hWnd, UINT IDs, bool NuevaCate);
void AgregarContacto(char Nombre[], char DCalle[], char DNum[], char DColonia[], char DMunicipio[], char DEstado[], bool Genero, char Correo[], char Telefono[], char Celular[], char Foto1[], char Foto2[], char Categoria[], char Orden[]);
void EstablecerCalendario2(HWND hWnd, int Parametro, int Mes, int Dia, int Año);
void BotonesdelCalendario2(HWND hWnd, UINT IDs, int Hora, int Dia, int Mes, int Año);
BOOL CALLBACK VentanaPrincipal(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ADDContacto(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Calendario(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Calendario2(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Eventos(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK CallAlarma(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TodoslosEventos(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR cmdLine,
	int showCmd)
{
	Prin = Aux = Aux2 = Fin = NULL;
	APrin = AAux = AAux2 = AFin = ASonando = ASonando2 = NULL;
	PrincipioDireccion();
	AlarmaCargar();
	hTEventos = 0;
	hCalendario2 = hInstance;
	hCalendario = hInstance;
	ADDCont = hInstance;
	hEventos = hInstance;
	hAlarma = hInstance;
	VP = CreateDialog(
			hInstance, // Application instance.
			MAKEINTRESOURCE(IDD_PRINCIPAL), // Dialog resource ID.
			NULL, // Parent window--null for no parent.
			VentanaPrincipal // Dialog window procedure.
	); 
	ShowWindow(VP, showCmd);

	// configuración del centro de mensajes
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (VP == 0 || !IsDialogMessage(VP, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}

void PrincipioDireccion()
{
	int LimiteExe = 0;
	GetModuleFileName(NULL, TodasLasDire, sizeof(TodasLasDire));
	while (1)
	{
		if (TodasLasDire[LimiteExe] == '.' && TodasLasDire[LimiteExe + 1] == 'e' && TodasLasDire[LimiteExe + 2] == 'x' && TodasLasDire[LimiteExe + 3] == 'e')
		{
			break;
		}
		else
		{
			LimiteExe++;
		}
	}
	while (LimiteExe >= 0)
	{
		if (TodasLasDire[LimiteExe] == '\\' && TodasLasDire[LimiteExe+1]=='D'&& TodasLasDire[LimiteExe + 2] == 'e'&& TodasLasDire[LimiteExe + 3] == 'b'&& TodasLasDire[LimiteExe + 4] == 'u'&& TodasLasDire[LimiteExe + 5] == 'g')
		{
			break;
		}
		else
		{
			LimiteExe--;
		}
	}
	for (int i = 0; i <= LimiteExe; i++)
	{
		DireccionPrograma_Principio[i] = TodasLasDire[i];
	}
	char Carpeta[] = "Proyecto Final Progra2\\" ;
	sprintf_s(BMPCarpeta,"%s%sBMP Defaults\\", DireccionPrograma_Principio,Carpeta);
	sprintf_s(FILECarpeta,"%s%sFiles\\", DireccionPrograma_Principio,Carpeta);

	sprintf_s(DireccionPrograma_Alarmas, "%sEventandAlarms.cprg",  FILECarpeta);
	sprintf_s(DireccionPrograma_Despertador1, "%sDespertador1.bmp", FILECarpeta);
	sprintf_s(DireccionPrograma_Alarma1, "%sAlarma 1.mp3",  FILECarpeta);
	sprintf_s(DireccionPrograma_Alarma2, "%sAlarma 2.mp3",  FILECarpeta);
	sprintf_s(DireccionPrograma_Alarma3, "%sAlarma 3.mp3",  FILECarpeta);
	sprintf_s(DireccionPrograma_Contactos, "%sCont.cprg",  FILECarpeta);
	sprintf_s(DireccionPrograma_Categorias, "%sCates.txt",  FILECarpeta);
	sprintf_s(bmp_Default, "%sPerfil_Default.bmp", FILECarpeta);
}

void SeleccionarArchivo(HWND Ventana, int Filtro, char Archivo[])
{
	OPENFILENAME ofn;
	HANDLE hf;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = Ventana;
	ofn.lpstrFile = Archivo;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	if (Filtro==1)
	ofn.lpstrInitialDir = BMPCarpeta;
	else
		ofn.lpstrInitialDir = NULL;
	if (Filtro == 1)
		ofn.lpstrFilter = "Todos\0*.*\0Archivos BMP\0*.bmp\0";
	else if (Filtro == 2)
		ofn.lpstrFilter = "Todos\0*.*\0Musica MP3\0*.mp3\0";
	ofn.nFilterIndex = 2;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	
	if (GetOpenFileName(&ofn) != TRUE)
		MessageBox(Ventana, "No se eligió el archivo", "Aviso", MB_OK | MB_ICONEXCLAMATION);
	else
	{
		EsArchivo = false;
		int Posicion = 0;
		while (EsArchivo == false && Posicion < MAX_PATH)
		{
			if ((Filtro == 1 && Archivo[Posicion] == '.' && Archivo[Posicion + 1] == 'b' && Archivo[Posicion + 2] == 'm' && Archivo[Posicion + 3] == 'p') || (Filtro == 2 && Archivo[Posicion] == '.' && Archivo[Posicion + 1] == 'm' && Archivo[Posicion + 2] == 'p' && Archivo[Posicion + 3] == '3'))
				EsArchivo = true;
			else
				Posicion++;
		}
		if (EsArchivo == false)
		{
			MessageBox(Ventana, "El archivo no corresponde al formato", "Aviso", MB_OK | MB_ICONEXCLAMATION);
			Archivo = NULL;
		}
	}
	
}

void DetectarAlarma(int Parte, UINT DT, HWND hWnd, bool *Alarma)
{
	if (Parte == 1)
	{
		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);
		SetTimer(hWnd, DT, 10, (TIMERPROC)NULL);
	}
	else if (Parte == 2)
	{
		if (APrin != NULL)
		{
			AAux = APrin;
			int SegundoActual = timeinfo.tm_sec;
			int  MinutoActual = timeinfo.tm_min;
			int HoraActual = timeinfo.tm_hour;
			int AñoActual = timeinfo.tm_year + 1900;
			int MesActual = timeinfo.tm_mon;
			int DiaActual = timeinfo.tm_mday;
			while (AAux != NULL)
			{
				int Min = atoi(AAux->Fecha.Minuto);
				int Hor = atoi(AAux->Fecha.Hora);
				int Año = atoi(AAux->Fecha.Año);
				int Mes = atoi(AAux->Fecha.Mes);
				int Dia = atoi(AAux->Fecha.Dia);
				if (DiaActual == Dia  && MesActual == Mes-1 && AñoActual == Año && HoraActual ==  Hor && MinutoActual == Min && SegundoActual == 0 && *Alarma == false)
				{
					*Alarma = true;
					if (*Alarma == true)
					{
						char Comando[MAX_PATH] = "";
						ifstream Lectura;
						Lectura.open(AAux->Tono,ios::binary | ios::in);
						if (Lectura.is_open())
						{
							NoDectetaMP3 = false;
							sprintf_s(Comando, "open \"%s\" alias MP3", AAux->Tono);
							Lectura.close();
						}
						else
						{
							NoDectetaMP3 = true;
							sprintf_s(Comando, "open \"%s\" alias MP3", DireccionPrograma_Alarma1);
						}
						
						mciSendString(Comando, NULL, 0, 0);
						mciSendString("play MP3", NULL, 0, 0);
						ASonando = AAux;
						DialogBox(hAlarma, MAKEINTRESOURCE(IDD_ALARMA), hWnd, CallAlarma);
						mciSendString("stop MP3", NULL, 0, 0);
						mciSendString("close MP3", NULL, 0, 0);
						ProximoEvento(hWnd);
						ReCargarTodosEventos(hWnd);
						AAux = AAux2;
						*Alarma = false;
					}
				}
				else
				{
					AAux = AAux->sig;
				}
			}
		}
	}
}

void MoverOrdenarPor(Contactos Temporal)
{
	strcpy_s(Temporal.Nombre, Aux2->Nombre);
	Temporal.Genero = Aux2->Genero;
	strcpy_s(Temporal.Categoria, Aux2->Categoria);
	strcpy_s(Temporal.Direccion.Calle, Aux2->Direccion.Calle);
	strcpy_s(Temporal.Direccion.Num, Aux2->Direccion.Num);
	strcpy_s(Temporal.Direccion.Colonia, Aux2->Direccion.Colonia);
	strcpy_s(Temporal.Direccion.Municipio, Aux2->Direccion.Municipio);
	strcpy_s(Temporal.Direccion.Estado, Aux2->Direccion.Estado);
	strcpy_s(Temporal.Telefono, Aux2->Telefono);
	strcpy_s(Temporal.Celular, Aux2->Celular);
	strcpy_s(Temporal.Correo, Aux2->Correo);
	strcpy_s(Temporal.Foto1, Aux2->Foto1);
	strcpy_s(Temporal.Foto2, Aux2->Foto2);
	strcpy_s(Temporal.Orden, Aux2->Orden);

	strcpy_s(Aux2->Nombre, Aux->Nombre);
	Aux2->Genero = Aux->Genero;
	strcpy_s(Aux2->Categoria, Aux->Categoria);
	strcpy_s(Aux2->Direccion.Calle, Aux->Direccion.Calle);
	strcpy_s(Aux2->Direccion.Num, Aux->Direccion.Num);
	strcpy_s(Aux2->Direccion.Colonia, Aux->Direccion.Colonia);
	strcpy_s(Aux2->Direccion.Municipio, Aux->Direccion.Municipio);
	strcpy_s(Aux2->Direccion.Estado, Aux->Direccion.Estado);
	strcpy_s(Aux2->Telefono, Aux->Telefono);
	strcpy_s(Aux2->Celular, Aux->Celular);
	strcpy_s(Aux2->Correo, Aux->Correo);
	strcpy_s(Aux2->Foto1, Aux->Foto1);
	strcpy_s(Aux2->Foto2, Aux->Foto2);
	strcpy_s(Aux2->Orden, Aux->Orden);

	strcpy_s(Aux->Nombre, Temporal.Nombre);
	Aux->Genero = Temporal.Genero;
	strcpy_s(Aux->Categoria, Temporal.Categoria);
	strcpy_s(Aux->Direccion.Calle, Temporal.Direccion.Calle);
	strcpy_s(Aux->Direccion.Num, Temporal.Direccion.Num);
	strcpy_s(Aux->Direccion.Colonia, Temporal.Direccion.Colonia);
	strcpy_s(Aux->Direccion.Municipio, Temporal.Direccion.Municipio);
	strcpy_s(Aux->Direccion.Estado, Temporal.Direccion.Estado);
	strcpy_s(Aux->Telefono, Temporal.Telefono);
	strcpy_s(Aux->Celular, Temporal.Celular);
	strcpy_s(Aux->Correo, Temporal.Correo);
	strcpy_s(Aux->Foto1, Temporal.Foto1);
	strcpy_s(Aux->Foto2, Temporal.Foto2);
	strcpy_s(Aux->Orden, Temporal.Orden);
}

void ContactoOrdenarPor(int Parametro)
{
	if (Prin != NULL)
	{
		int Contador = 0;
		Contactos* Pos1 = NULL, * Pos2 = NULL;
		Aux = Prin;
		Aux = Aux->sig;
		Aux2 = Prin;

		Contactos Temporal;
		strcpy_s(Temporal.Nombre, "");
		Temporal.Genero = true;
		strcpy_s(Temporal.Categoria, "");
		strcpy_s(Temporal.Direccion.Calle, "");
		strcpy_s(Temporal.Direccion.Num, "");
		strcpy_s(Temporal.Direccion.Colonia, "");
		strcpy_s(Temporal.Direccion.Municipio, "");
		strcpy_s(Temporal.Direccion.Estado, "");
		strcpy_s(Temporal.Telefono, "");
		strcpy_s(Temporal.Celular, "");
		strcpy_s(Temporal.Correo, "");
		strcpy_s(Temporal.Foto1, "");
		strcpy_s(Temporal.Foto2, "");
		strcpy_s(Temporal.Orden, "");



		while (Aux != NULL)
		{
			Pos1 = Aux2;
			Pos2 = Aux;
			Contador++;
			for (int i = 0; i < Contador; i++)
			{
				if (Parametro == 0)
				{
					int VAux = atoi(Aux->Orden), VAux2 = atoi(Aux2->Orden);

					if (VAux < VAux2)
					{
						MoverOrdenarPor(Temporal);
					}
					Aux = Aux2;
					Aux2 = Aux->ant;
				}
				else if (Parametro == 1)
				{
					if (strcmp(Aux->Nombre, Aux2->Nombre) < 0)
					{
						MoverOrdenarPor(Temporal);
					}
					Aux = Aux2;
					Aux2 = Aux->ant;
				}
				else if (Parametro == 2)
				{
					if (strcmp(Aux->Categoria, Aux2->Categoria) < 0)
					{
						MoverOrdenarPor(Temporal);
					}
					Aux = Aux2;
					Aux2 = Aux->ant;
				}
			}
			Aux2 = Pos1;
			Aux = Pos2;
			Aux2 = Aux;
			Aux = Aux->sig;
		}
		Aux = Aux2 = NULL;
		Pos1 = Pos2 = NULL;

		if (Parametro == 2)
		{
			Contador = 0;
			Aux = Prin;
			Aux = Aux->sig;
			Aux2 = Prin;

			while (Aux != NULL)
			{
				Pos1 = Aux2;
				Pos2 = Aux;
				Contador++;
				for (int i = 0; i < Contador; i++)
				{
					if (strcmp(Aux->Nombre, Aux2->Nombre) < 0 && strcmp(Aux->Categoria, Aux2->Categoria) == 0)
					{
						MoverOrdenarPor(Temporal);
					}
					Aux = Aux2;
					Aux2 = Aux->ant;
				}
				Aux2 = Pos1;
				Aux = Pos2;
				Aux2 = Aux;
				Aux = Aux->sig;
			}
			Aux = Aux2 = NULL;
			Pos1 = Pos2 = NULL;
		}
	}
}

void BotonesdelCalendario2(HWND hWnd,UINT IDs, int Hora,int Dia, int Mes, int Año)
{
	char Textito[40] = "";
	SendDlgItemMessage(hWnd, IDs, LB_GETTEXT, Hora, (LPARAM)Textito);

	if (strcmp(Textito, "-") == 0)
	{
		if (MessageBox(hWnd, "No tiene eventos programados. \n¿Desea programar uno?", "Eventos", MB_YESNO) == IDYES)
		{
			DiaGlobalSeleccionado = Dia;
			MesGlobalSeleccionado = Mes;
			AñoGlobalSeleccionado = Año;
			HoraGlobalSeleccionado = Hora;
			SeleccioneDesdeCalendario = true;
			SeleccioneDesdeSemanal = true;
			DialogBox(hEventos, MAKEINTRESOURCE(IDD_EVENTOS), hWnd, Eventos);
		}
	}
	else if (strcmp(Textito, "x") == 0)
	{
		MessageBox(hWnd, "No puede programar un evento en esta hora", "Eventos", MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		DiaGlobalSeleccionado = Dia;
		MesGlobalSeleccionado = Mes;
		AñoGlobalSeleccionado = Año;
		HoraGlobalSeleccionado = Hora;
		SeleccioneDesdeCalendario = true;
		SeleccioneDesdeSemanal = true;
		DialogBox(hTEventos, MAKEINTRESOURCE(IDD_TEVENTOS), hWnd, TodoslosEventos);
	}
}

void BotonesdelCalendario(HWND hWnd, UINT IDs,int Dias, int Semanas)
{
	char Nombredelmes[12][30] = { "Enero","Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto","Septiembre","Octubre", "Noviembre", "Diciembre" };
	if (DiasCalendario_Eventos[Semanas][Dias] == false)
	{
		if (MessageBox(hWnd, "No tiene eventos programados.\n¿Desea programar uno?", "Eventos", MB_YESNO) == IDYES)
		{
			char V[40] = "";
			SendDlgItemMessage(hWnd, IDC_YEAR, WM_GETTEXT, sizeof(V), (LPARAM)V);
			AñoGlobalSeleccionado = atoi(V);
			SendDlgItemMessage(hWnd, IDC_MES, WM_GETTEXT, sizeof(V), (LPARAM)V);
			int Meees = 0;
			while (1)
			{
				if (strcmp(V, Nombredelmes[Meees]) == 0)
					break;
				else
					Meees++;
			}
			MesGlobalSeleccionado = Meees + 1;
			SendDlgItemMessage(hWnd, IDs, WM_GETTEXT, sizeof(V), (LPARAM)V);
			DiaGlobalSeleccionado = atoi(V);
			SeleccioneDesdeCalendario = true;
			SeleccioneDesdeSemanal = false;
			DialogBox(hEventos, MAKEINTRESOURCE(IDD_EVENTOS), hWnd, Eventos);
		}
	}
	else
	{
		SeleccioneDesdeCalendario = true;
		SeleccioneDesdeSemanal = false;
		char V[40] = "";
		SendDlgItemMessage(hWnd, IDC_YEAR, WM_GETTEXT, sizeof(V), (LPARAM)V);
		AñoGlobalSeleccionado = atoi(V);
		SendDlgItemMessage(hWnd, IDC_MES, WM_GETTEXT, sizeof(V), (LPARAM)V);
		int Meees = 0;
		while (1)
		{
			if (strcmp(V, Nombredelmes[Meees]) == 0)
				break;
			else
				Meees++;
		}
		MesGlobalSeleccionado = Meees + 1;
		strcpy_s(V, "");
		SendDlgItemMessage(hWnd, IDs, WM_GETTEXT, sizeof(2), (LPARAM)V);
		DiaGlobalSeleccionado = atoi(V);
		DialogBox(hTEventos, MAKEINTRESOURCE(IDD_TEVENTOS), hWnd, TodoslosEventos);
	}
}

void CategoriaGuardar(HWND hWnd, UINT IDC, UINT ID_EDIT, char Categoria[], bool Modificar, int Cursel, bool Eliminar)
{
	if (Modificar == false)
	{
		bool DuplicadoCategoria = false;
		int MiniIndex = SendMessage(GetDlgItem(hWnd, IDC), CB_GETCOUNT, 0, 0);
		for (int j = 0; j < MiniIndex; j++)
		{
			char CopiaTem[40] = "";
			SendMessage(GetDlgItem(hWnd, IDC), CB_GETLBTEXT, j, (LPARAM)CopiaTem);
			if (strcmp(Categoria, CopiaTem) == 0)
			{
				DuplicadoCategoria = true;
				break;
			}
		}

		if (DuplicadoCategoria == false)
		{
			SendMessage(GetDlgItem(hWnd, IDC), CB_ADDSTRING, 0, (LPARAM)Categoria);
			SendMessage(GetDlgItem(hWnd, ID_EDIT), WM_SETTEXT, 0, (LPARAM)"");
			MiniIndex = SendMessage(GetDlgItem(hWnd, IDC), CB_GETCOUNT, 0, 0);
			MiniIndex--;
			SendMessage(GetDlgItem(hWnd, IDC), CB_SETCURSEL, MiniIndex, 0);

			ofstream CateOut;
			CateOut.open(DireccionPrograma_Categorias, ios::app | ios::ate);

			if (CateOut.is_open())
			{
				CateOut << Categoria << endl;
				CateOut.close();
			}
		}
		else
		{
			char Detectar[40] = "";
			int NuevoIndex = 0;

			for (NuevoIndex; NuevoIndex < MiniIndex; NuevoIndex++)
			{
				SendDlgItemMessage(hWnd, IDC, CB_GETLBTEXT, NuevoIndex, (LPARAM)Detectar);

				if (strcmp(Detectar, Categoria) == 0)
				{
					break;
				}
			}

			SendMessage(GetDlgItem(hWnd, IDC), CB_SETCURSEL, NuevoIndex, 0);
			
			if (Cursel == 0)
			{
				MessageBox(hWnd, "La categoria ya existe en su lista", "Nota", MB_OK);
			}
		}
	}
	else
	{
		ofstream CateOut;
		CateOut.open(DireccionPrograma_Categorias, ios::out);

		if (CateOut.is_open())
		{
			int IndMax = SendDlgItemMessage(hWnd, IDC, CB_GETCOUNT, 0, 0);
			char Variables[40] = "";
			CateOut << "Nueva Categoria..." << endl;
			for (int i = 0; i<IndMax; i++)
			{
				SendDlgItemMessage(hWnd, IDC, CB_GETLBTEXT, i, (LPARAM)Variables);
				if (Cursel == i)
				{
					if (Eliminar == false)
						CateOut << Categoria << endl;
				}
				else
				{
					CateOut << Variables << endl;
				}
			}
			CateOut.close();
		}
		SendMessage(GetDlgItem(hWnd, ID_EDIT), WM_SETTEXT, 0, (LPARAM)"");
	}
	SendMessage(GetDlgItem(hWnd,ID_EDIT), WM_SETTEXT, 0, (LPARAM)"");
}

void AgregarContacto(char Nombre[], char DCalle[], char DNum[], char DColonia[], char DMunicipio[], char DEstado[], bool Genero, char Correo[], char Telefono[], char Celular[], char Foto1[], char Foto2[], char Categoria[],char Orden[])
{
	if (Prin == NULL)
	{
		Aux = new Contactos;
		Prin = Aux;
		Fin = Aux;
		Aux->ant = Aux->sig = NULL;
	}
	else
	{
		Aux = new Contactos;
		Fin->sig = Aux;
		Aux->ant = Fin;
		Aux->sig = NULL;
		Fin = Aux;
	}
	IntOrden++;
	_itoa_s(IntOrden, charOrden, 10);
	strcpy_s(Aux->Nombre, Nombre);
	strcpy_s(Aux->Direccion.Calle, DCalle);
	strcpy_s(Aux->Direccion.Num, DNum);
	strcpy_s(Aux->Direccion.Colonia, DColonia);
	strcpy_s(Aux->Direccion.Municipio, DMunicipio);
	strcpy_s(Aux->Direccion.Estado, DEstado);
	Aux->Genero = Genero;
	strcpy_s(Aux->Correo, Correo);
	strcpy_s(Aux->Telefono, Telefono);
	strcpy_s(Aux->Celular, Celular);
	strcpy_s(Aux->Foto1, Foto1);
	strcpy_s(Aux->Foto2, Foto2);
	strcpy_s(Aux->Categoria, Categoria);
	strcpy_s(Aux->Orden, Orden);
	strcpy_s(charOrden, "");
	Aux = NULL;
}

void ModificarContacto(char Nombre[], char DCalle[], char DNum[], char DColonia[], char DMunicipio[], char DEstado[], bool Genero, char Correo[], char Telefono[], char Celular[], char Foto1[], char Foto2[], char Categoria[])
{
	strcpy_s(Aux->Nombre, Nombre);
	strcpy_s(Aux->Direccion.Calle, DCalle);
	strcpy_s(Aux->Direccion.Num, DNum);
	strcpy_s(Aux->Direccion.Colonia, DColonia);
	strcpy_s(Aux->Direccion.Municipio, DMunicipio);
	strcpy_s(Aux->Direccion.Estado, DEstado);
	Aux->Genero = Genero;
	strcpy_s(Aux->Correo, Correo);
	strcpy_s(Aux->Telefono, Telefono);
	strcpy_s(Aux->Celular, Celular);
	strcpy_s(Aux->Foto1, Foto1);
	strcpy_s(Aux->Foto2, Foto2);
	strcpy_s(Aux->Categoria, Categoria);
	Aux = NULL;
}

void ModificarEvento(Alarmas Datos, Alarmas *AAux)
{
	strcpy_s(AAux->Titulo, Datos.Titulo);
	strcpy_s(AAux->NombreDelContacto, Datos.NombreDelContacto);
	strcpy_s(AAux->Descripcion, Datos.Descripcion);
	strcpy_s(AAux->Fecha.Dia, Datos.Fecha.Dia);
	strcpy_s(AAux->Fecha.Mes, Datos.Fecha.Mes);
	strcpy_s(AAux->Fecha.Año, Datos.Fecha.Año);
	strcpy_s(AAux->Fecha.Hora, Datos.Fecha.Hora);
	strcpy_s(AAux->Fecha.Minuto, Datos.Fecha.Minuto);
	strcpy_s(AAux->Ubicacion, Datos.Ubicacion);
	strcpy_s(AAux->Tipo, Datos.Tipo);
	strcpy_s(AAux->Tono, Datos.Tono);
	AAux = NULL;
}

void ProximoEvento(HWND hWnd)
{
	bool HayAlarma = false;
	if (APrin != NULL)
	{
		AAux = APrin;
		while (AAux != NULL && HayAlarma == false)
		{
			bool Continuar = true;
			time(&rawtime);
			localtime_s(&timeinfo, &rawtime);
			int Segundo[5]; //Año -- Mes -- Dia -- Hora -- Min
			int Primero[5]; //Año -- Mes -- Dia -- Hora -- Min

			Primero[0] = atoi(AAux->Fecha.Año);
			Segundo[0] = timeinfo.tm_year + 1900;

			Primero[1] = atoi(AAux->Fecha.Mes);
			Primero[1]--;
			Segundo[1] = timeinfo.tm_mon;

			Primero[2] = atoi(AAux->Fecha.Dia);
			Segundo[2] = timeinfo.tm_mday;

			Primero[3] = atoi(AAux->Fecha.Hora);
			Segundo[3] = timeinfo.tm_hour;

			Primero[4] = atoi(AAux->Fecha.Minuto);
			Segundo[4] = timeinfo.tm_min;

			if (Primero[0] > Segundo[0])
			{
				Continuar = true;
			}
			else if (Primero[0] < Segundo[0])
			{
				Continuar = false;
			}
			else
			{
				if (Primero[1] > Segundo[1])
				{
					Continuar = true;
				}
				else if (Primero[1] < Segundo[1])
				{
					Continuar = false;
				}
				else
				{
					if (Primero[2] > Segundo[2])
					{
						Continuar = true;
					}
					else if (Primero[2] < Segundo[2])
					{
						Continuar = false;
					}
					else
					{
						if (Primero[3] > Segundo[3])
						{
							Continuar = true;
						}
						else if (Primero[3] < Segundo[3])
						{
							Continuar = false;
						}
						else
						{
							if (Primero[4] > Segundo[4])
							{
								Continuar = true;
							}
							else if (Primero[4] < Segundo[4])
							{
								Continuar = false;
							}
							else if (Primero[4] == Segundo[4])
							{
								Continuar = false;
							}
						}
					}
				}
			}

			if (Continuar == true)
			{
				SendDlgItemMessage(hWnd, IDC_TITULODELEVENTO, WM_SETTEXT, 0, (LPARAM)AAux->Titulo);
				char TTT[MAX_PATH] = "";

				strcat_s(TTT, AAux->Fecha.Dia);
				strcat_s(TTT, "/");
				strcat_s(TTT, AAux->Fecha.Mes);
				strcat_s(TTT, "/");
				strcat_s(TTT, AAux->Fecha.Año);
				strcat_s(TTT, " ");
				strcat_s(TTT, AAux->Fecha.Hora);
				strcat_s(TTT, ":");

				if (AAux->Fecha.Minuto[0]=='0'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(TTT, "00");
				else if (AAux->Fecha.Minuto[0] == '1'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(TTT, "01");
				else if (AAux->Fecha.Minuto[0] == '2'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(TTT, "02");
				else if (AAux->Fecha.Minuto[0] == '3'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(TTT, "03");
				else if (AAux->Fecha.Minuto[0] == '4'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(TTT, "04");
				else if (AAux->Fecha.Minuto[0] == '5'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(TTT, "05");
				else if (AAux->Fecha.Minuto[0] == '6'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(TTT, "06");
				else if (AAux->Fecha.Minuto[0] == '7'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(TTT, "07");
				else if (AAux->Fecha.Minuto[0] == '8'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(TTT, "08");
				else if (AAux->Fecha.Minuto[0] == '9'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(TTT, "09");
				else
					strcat_s(TTT, AAux->Fecha.Minuto);

				SendDlgItemMessage(hWnd, IDC_FECHADELEVENTO, WM_SETTEXT, 0, (LPARAM)TTT);
				HayAlarma = true;
			}
			else
			{
				AAux = AAux->sig;
			}
		}
	}
	if (HayAlarma == true)
	{
		ShowWindow(GetDlgItem(hWnd, IDC_STATIC_TITULO), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FECHA), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_TITULODELEVENTO), SW_SHOW);
		ShowWindow(GetDlgItem(hWnd, IDC_FECHADELEVENTO), SW_SHOW);
	}
	else
	{
		ShowWindow(GetDlgItem(hWnd, IDC_STATIC_TITULO), SW_HIDE);
		ShowWindow(GetDlgItem(hWnd, IDC_STATIC_FECHA), SW_HIDE);
		ShowWindow(GetDlgItem(hWnd, IDC_TITULODELEVENTO), SW_HIDE);
		ShowWindow(GetDlgItem(hWnd, IDC_FECHADELEVENTO), SW_HIDE);
	}
}

void AgregarEvento(Alarmas Datos)
{
	AAux = NULL;
	if (APrin == NULL)
	{
		AAux = new Alarmas;
		APrin = AAux;
		AFin = AAux;
		AAux->ant = AAux->sig = NULL;
	}
	else
	{
		AAux = new Alarmas;
		AFin->sig = AAux;
		AAux->ant = AFin;
		AAux->sig = NULL;
		AFin = AAux;
	}
	strcpy_s(AAux->Titulo, Datos.Titulo);
	strcpy_s(AAux->NombreDelContacto, Datos.NombreDelContacto);
	strcpy_s(AAux->Descripcion, Datos.Descripcion);
	strcpy_s(AAux->Fecha.Dia, Datos.Fecha.Dia);
	strcpy_s(AAux->Fecha.Mes, Datos.Fecha.Mes);
	strcpy_s(AAux->Fecha.Año, Datos.Fecha.Año);
	strcpy_s(AAux->Fecha.Hora, Datos.Fecha.Hora);
	strcpy_s(AAux->Fecha.Minuto, Datos.Fecha.Minuto);
	strcpy_s(AAux->Ubicacion, Datos.Ubicacion);
	strcpy_s(AAux->Tipo, Datos.Tipo);
	strcpy_s(AAux->Tono, Datos.Tono);
	AAux = NULL;
}

void ReCargarTodosEventos(HWND hWnd)
{
	int uwu = SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA, LB_GETCOUNT, 0, 0);
	for (int o = 0; o < uwu; o++)
	{
		SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA, LB_DELETESTRING, 0, 0);
		SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA2, LB_DELETESTRING, 0, 0);
	}
	EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_MODIFICAR), false);
	EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_DELETE), false);
	if (SeleccioneDesdeCalendario == false)
	{
		if (APrin != NULL)
		{
			AAux = APrin;

			while (AAux != NULL)
			{
				SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA, LB_ADDSTRING, sizeof(AAux->Titulo), (LPARAM)AAux->Titulo);

				char Txt[MAX_PATH] = "";

				strcat_s(Txt, AAux->Fecha.Dia);
				strcat_s(Txt, "/");
				strcat_s(Txt, AAux->Fecha.Mes);
				strcat_s(Txt, "/");
				strcat_s(Txt, AAux->Fecha.Año);
				strcat_s(Txt, " ");
				strcat_s(Txt, AAux->Fecha.Hora);
				strcat_s(Txt, ":");
				if (AAux->Fecha.Minuto[0] == '0'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(Txt, "00");
				else if (AAux->Fecha.Minuto[0] == '1'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(Txt, "01");
				else if (AAux->Fecha.Minuto[0] == '2'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(Txt, "02");
				else if (AAux->Fecha.Minuto[0] == '3'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(Txt, "03");
				else if (AAux->Fecha.Minuto[0] == '4'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(Txt, "04");
				else if (AAux->Fecha.Minuto[0] == '5'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(Txt, "05");
				else if (AAux->Fecha.Minuto[0] == '6'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(Txt, "06");
				else if (AAux->Fecha.Minuto[0] == '7'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(Txt, "07");
				else if (AAux->Fecha.Minuto[0] == '8'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(Txt, "08");
				else if (AAux->Fecha.Minuto[0] == '9'&& AAux->Fecha.Minuto[1] == NULL)
					strcat_s(Txt, "09");
				else
					strcat_s(Txt, AAux->Fecha.Minuto);

				SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA2, LB_ADDSTRING, sizeof(Txt), (LPARAM)Txt);
				AAux = AAux->sig;
			}
		}
	}
	else
	{
		if (SeleccioneDesdeSemanal == false)
		{
			if (APrin != NULL)
			{
				AAux = APrin;

				while (AAux != NULL)
				{
					int TDia = atoi(AAux->Fecha.Dia);
					int TMes = atoi(AAux->Fecha.Mes);
					int TAño = atoi(AAux->Fecha.Año);
					if (TDia == DiaGlobalSeleccionado && TMes == MesGlobalSeleccionado && TAño == AñoGlobalSeleccionado)
					{
						SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA, LB_ADDSTRING, sizeof(AAux->Titulo), (LPARAM)AAux->Titulo);

						char Txt[MAX_PATH] = "";

						strcat_s(Txt, AAux->Fecha.Dia);
						strcat_s(Txt, "/");
						strcat_s(Txt, AAux->Fecha.Mes);
						strcat_s(Txt, "/");
						strcat_s(Txt, AAux->Fecha.Año);
						strcat_s(Txt, " ");
						strcat_s(Txt, AAux->Fecha.Hora);
						strcat_s(Txt, ":");
						strcat_s(Txt, AAux->Fecha.Minuto);

						SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA2, LB_ADDSTRING, sizeof(Txt), (LPARAM)Txt);
					}
					AAux = AAux->sig;
				}
			}
		}
		else
		{
			if (APrin != NULL)
			{
				AAux = APrin;

				while (AAux != NULL)
				{
					int TDia = atoi(AAux->Fecha.Dia);
					int TMes = atoi(AAux->Fecha.Mes);
					int TAño = atoi(AAux->Fecha.Año);
					int THora = atoi(AAux->Fecha.Hora);
					if (TDia == DiaGlobalSeleccionado && TMes == MesGlobalSeleccionado && TAño == AñoGlobalSeleccionado && THora == HoraGlobalSeleccionado)
					{
						SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA, LB_ADDSTRING, sizeof(AAux->Titulo), (LPARAM)AAux->Titulo);

						char Txt[MAX_PATH] = "";

						strcat_s(Txt, AAux->Fecha.Dia);
						strcat_s(Txt, "/");
						strcat_s(Txt, AAux->Fecha.Mes);
						strcat_s(Txt, "/");
						strcat_s(Txt, AAux->Fecha.Año);
						strcat_s(Txt, " ");
						strcat_s(Txt, AAux->Fecha.Hora);
						strcat_s(Txt, ":");
						strcat_s(Txt, AAux->Fecha.Minuto);

						SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA2, LB_ADDSTRING, sizeof(Txt), (LPARAM)Txt);
					}
					AAux = AAux->sig;
				}
			}
		}
	}
}

BOOL CALLBACK ADDContacto(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DetectarAlarma(1, IDC_DT_ADDCONTACTOS, hWnd, false);
	static char NombreTemporal[40] = "";
	static HWND EditControl[9] = {0,0,0,0,0,0,0,0,0}; //0: Nombre, 1: Calle, 2: Numero, 3: Colonia, 4: Municipio, 5: Estado, 6: Telefono, 7: Celular, 8: Correo
	static HWND StaticControl[11] = { 0,0,0,0,0,0,0,0,0,0,0 };//0: Nombre, 1: Genero, 2: Categoria, 3: Calle, 4: Numero, 5: Colonia, 6: Municipio, 7: Estado, 8: Telefono, 9: Celular, 10: Correo
	static HWND Categoria = 0;
	static HWND NuevaCategoria = 0;
	static HBITMAP bmp1, bmp2;
	static char Primer_bmp[MAX_PATH] = "";
	static char Segundo_bmp[MAX_PATH] = "";
	static bool BMP_Primero = true;

	Contactos DatosTemporales;
	strcpy_s(DatosTemporales.Categoria, "");
	strcpy_s(DatosTemporales.Celular,"");
	strcpy_s(DatosTemporales.Correo, "");
	strcpy_s(DatosTemporales.Direccion.Calle, "");
	strcpy_s(DatosTemporales.Direccion.Num, "");
	strcpy_s(DatosTemporales.Direccion.Colonia, "");
	strcpy_s(DatosTemporales.Direccion.Municipio, "");
	strcpy_s(DatosTemporales.Direccion.Estado, "");
	strcpy_s(DatosTemporales.Foto1, "");
	strcpy_s(DatosTemporales.Foto2, "");
	DatosTemporales.Genero = true;
	strcpy_s(DatosTemporales.Nombre, "");
	strcpy_s(DatosTemporales.Telefono, "");



	switch (msg)
	{
	case WM_TIMER:
	{
		if (BoolAlarma == false)
		{
			DetectarAlarma(2, IDC_DT_EVENTOS, hWnd, &BoolAlarma);

		}
	}
	return true;
	case WM_INITDIALOG:
	{
		ShowWindow(GetDlgItem(hWnd, IDC_ERRORFOTO), SW_HIDE);

		BMP_Primero = true;
		if (ADDContactos_MostrarDatos == false && ADDContactos_ModificarDatos == false)
		{
			strcpy_s(Primer_bmp, bmp_Default);
			strcpy_s(Segundo_bmp, bmp_Default);

			bmp1 = (HBITMAP)SendDlgItemMessage(hWnd, IDC_PIC, STM_GETIMAGE, IMAGE_BITMAP, 0);
			//Al objeto bmp2, se le asigna una imagen local:
			bmp2 = (HBITMAP)LoadImage(NULL, bmp_Default, IMAGE_BITMAP, 250, 250, LR_LOADFROMFILE);
			SendDlgItemMessage(
				hWnd,
				IDC_PIC,
				STM_SETIMAGE,
				IMAGE_BITMAP,
				(LPARAM)bmp2);
		}
		
		EditControl[0] = GetDlgItem(hWnd, IDC_NOMBRE);
		EditControl[1] = GetDlgItem(hWnd, IDC_DIR_CALLE);
		EditControl[2] = GetDlgItem(hWnd, IDC_DIR_NUM);
		EditControl[3] = GetDlgItem(hWnd, IDC_DIR_COLONIA);
		EditControl[4] = GetDlgItem(hWnd, IDC_DIR_MUNICIPIO);
		EditControl[5] = GetDlgItem(hWnd, IDC_DIR_ESTADO);
		EditControl[6] = GetDlgItem(hWnd, IDC_TEL);
		EditControl[7] = GetDlgItem(hWnd, IDC_CEL);
		EditControl[8] = GetDlgItem(hWnd, IDC_CORREO);

		StaticControl[0] = GetDlgItem(hWnd, IDC_STATICMOSTRAR);
		StaticControl[1] = GetDlgItem(hWnd, IDC_STATICMOSTRAR2);
		StaticControl[2] = GetDlgItem(hWnd, IDC_STATICMOSTRAR3);
		StaticControl[3] = GetDlgItem(hWnd, IDC_STATICMOSTRAR4);
		StaticControl[4] = GetDlgItem(hWnd, IDC_STATICMOSTRAR5);
		StaticControl[5] = GetDlgItem(hWnd, IDC_STATICMOSTRAR6);
		StaticControl[6] = GetDlgItem(hWnd, IDC_STATICMOSTRAR7);
		StaticControl[7] = GetDlgItem(hWnd, IDC_STATICMOSTRAR8);
		StaticControl[8] = GetDlgItem(hWnd, IDC_STATICMOSTRAR9);
		StaticControl[9] = GetDlgItem(hWnd, IDC_STATICMOSTRAR10);
		StaticControl[10] = GetDlgItem(hWnd, IDC_STATICMOSTRAR11);

		Categoria = GetDlgItem(hWnd, IDC_CATEGORIA);
		NuevaCategoria = GetDlgItem(hWnd, IDC_NUEVACATEGORIA);
		EnableWindow(GetDlgItem(hWnd, IDC_NUEVACATEGORIA), FALSE);
		ShowWindow(GetDlgItem(hWnd, IDC_NUEVACATEGORIA), SW_HIDE);
		ShowWindow(GetDlgItem(hWnd, IDC_STATIC_NUEVACATEGORIA), SW_HIDE);

		SendDlgItemMessage(hWnd, IDC_RADIOHOMBRE, BM_SETCHECK, BST_UNCHECKED, 0);
		SendDlgItemMessage(hWnd, IDC_RADIOMUJER, BM_SETCHECK, BST_UNCHECKED, 0);

		ifstream CateIn;
		CateIn.open(DireccionPrograma_Categorias, ios::in);

		if (CateIn.is_open())
		{
			char Categorias[40] = "";

			CateIn.getline(Categorias, 40, '\n');
			while(!CateIn.eof())
			{
				SendMessage(Categoria, CB_ADDSTRING, sizeof(Categorias), (LPARAM)Categorias);
				CateIn.getline(Categorias, 40, '\n');
			}
			CateIn.close();
		}
		SendMessage(Categoria, CB_SETCURSEL, 1, 0);

		if (ADDContactos_MostrarDatos == false)
		{
			for (int i = 0; i<11; i++)
				ShowWindow(StaticControl[i], SW_HIDE);

			ShowWindow(GetDlgItem(hWnd, IDC_ELIMINAR), SW_HIDE);


			if (ADDContactos_ModificarDatos == true)
			{
				strcpy_s(Primer_bmp, Aux->Foto1);
				strcpy_s(Segundo_bmp, Aux->Foto2);

				bmp2 = (HBITMAP)LoadImage(NULL, Primer_bmp, IMAGE_BITMAP, 250, 250, LR_LOADFROMFILE);
				SendDlgItemMessage(
					hWnd,
					IDC_PIC,
					STM_SETIMAGE,
					IMAGE_BITMAP,
					(LPARAM)bmp2);

				strcpy_s(NombreTemporal, Aux->Nombre);
				SendMessage(EditControl[0], WM_SETTEXT, 0, (LPARAM)Aux->Nombre);
				if (Aux->Genero == true)
					SendDlgItemMessage(hWnd, IDC_RADIOHOMBRE,BM_SETCHECK, BST_CHECKED, 0);
				else
					SendDlgItemMessage(hWnd, IDC_RADIOMUJER, BM_SETCHECK, BST_CHECKED, 0);
				
				char Bariable[40] = "";
				int Maximo = SendDlgItemMessage(hWnd, IDC_CATEGORIA, CB_GETCOUNT, 0, 0);
				for (int i = 1; i < Maximo; i++)
				{
					SendMessage(Categoria, CB_GETLBTEXT, i, (LPARAM)Bariable);
					if (strcmp(Bariable, Aux->Categoria) == 0)
					{
						SendMessage(Categoria, CB_SETCURSEL, i, 0);
						break;
					}
				}

				SendMessage(EditControl[1], WM_SETTEXT, 0, (LPARAM)Aux->Direccion.Calle);
				SendMessage(EditControl[2], WM_SETTEXT, 0, (LPARAM)Aux->Direccion.Num);
				SendMessage(EditControl[3], WM_SETTEXT, 0, (LPARAM)Aux->Direccion.Colonia);
				SendMessage(EditControl[4], WM_SETTEXT, 0, (LPARAM)Aux->Direccion.Municipio);
				SendMessage(EditControl[5], WM_SETTEXT, 0, (LPARAM)Aux->Direccion.Estado);
				SendMessage(EditControl[6], WM_SETTEXT, 0, (LPARAM)Aux->Telefono);
				SendMessage(EditControl[7], WM_SETTEXT, 0, (LPARAM)Aux->Celular);
				SendMessage(EditControl[8], WM_SETTEXT, 0, (LPARAM)Aux->Correo);
			}
		}
		else
		{
			for (int i = 0; i<9; i++)
				ShowWindow(EditControl[i], SW_HIDE);

			ShowWindow(GetDlgItem(hWnd, IDC_CARGARBMP), SW_HIDE);

			strcpy_s(Primer_bmp,Aux->Foto1);
			strcpy_s(Segundo_bmp, Aux->Foto2);

			bool SiAbrio1 = true, SiAbrio2 = true;
			ifstream abrir;
			abrir.open(Primer_bmp, ios::binary | ios::in);
			if (abrir.is_open())
			{
				abrir.close();
			}
			else
			{
				strcpy_s(Primer_bmp, bmp_Default);
				SiAbrio1 = false;
			}
			abrir.open(Segundo_bmp, ios::binary | ios::in);
			if (abrir.is_open())
			{
				abrir.close();
			}
			else
			{
				strcpy_s(Segundo_bmp, bmp_Default);
				SiAbrio2 = false;
			}

			if (SiAbrio1 == false && SiAbrio2 == false)
			{
				SendDlgItemMessage(hWnd, IDC_ERRORFOTO, WM_SETTEXT, 0, (LPARAM)"*Hubo un error al encontrar las 2 imagenes de perfil del contacto*");
				ShowWindow(GetDlgItem(hWnd, IDC_ERRORFOTO), SW_SHOW);
			}
			else if (SiAbrio1 == false)
			{

				SendDlgItemMessage(hWnd, IDC_ERRORFOTO, WM_SETTEXT, 0, (LPARAM)"*Hubo un error al encontrar la imagen numero 1 de perfil del contacto*");
				ShowWindow(GetDlgItem(hWnd, IDC_ERRORFOTO), SW_SHOW);
			}
			else if (SiAbrio2 == false)
			{
				SendDlgItemMessage(hWnd, IDC_ERRORFOTO, WM_SETTEXT, 0, (LPARAM)"*Hubo un error al encontrar la imagen numero 2 de perfil del contacto*");
				ShowWindow(GetDlgItem(hWnd, IDC_ERRORFOTO), SW_SHOW);
			}

			bmp2 = (HBITMAP)LoadImage(NULL, Primer_bmp, IMAGE_BITMAP, 250, 250, LR_LOADFROMFILE);
			SendDlgItemMessage(
				hWnd,
				IDC_PIC,
				STM_SETIMAGE,
				IMAGE_BITMAP,
				(LPARAM)bmp2);

			SendMessage(StaticControl[0], WM_SETTEXT, 0, (LPARAM)Aux->Nombre);
			if (Aux->Genero == true)
				SendMessage(StaticControl[1], WM_SETTEXT, 0, (LPARAM)"Hombre");
			else
				SendMessage(StaticControl[1], WM_SETTEXT, 0, (LPARAM)"Mujer");

			SendMessage(StaticControl[2], WM_SETTEXT, 0, (LPARAM)Aux->Categoria);
			SendMessage(StaticControl[3], WM_SETTEXT, 0, (LPARAM)Aux->Direccion.Calle);
			SendMessage(StaticControl[4], WM_SETTEXT, 0, (LPARAM)Aux->Direccion.Num);
			SendMessage(StaticControl[5], WM_SETTEXT, 0, (LPARAM)Aux->Direccion.Colonia);
			SendMessage(StaticControl[6], WM_SETTEXT, 0, (LPARAM)Aux->Direccion.Municipio);
			SendMessage(StaticControl[7], WM_SETTEXT, 0, (LPARAM)Aux->Direccion.Estado);
			SendMessage(StaticControl[8], WM_SETTEXT, 0, (LPARAM)Aux->Telefono);
			SendMessage(StaticControl[9], WM_SETTEXT, 0, (LPARAM)Aux->Celular);
			SendMessage(StaticControl[10], WM_SETTEXT, 0, (LPARAM)Aux->Correo);

			ShowWindow(GetDlgItem(hWnd, IDC_STATICCAMPOSOBLI), SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_CATEGORIA), SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_RADIOHOMBRE), SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_RADIOMUJER), SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_RESET), SW_HIDE);

			SendDlgItemMessage(hWnd, IDC_SAVE, WM_SETTEXT, 0, (LPARAM)"Editar");
			SendDlgItemMessage(hWnd, IDC_CANCEL, WM_SETTEXT, 0, (LPARAM)"Cerrar");
			SendDlgItemMessage(hWnd, IDC_STATICNOMBRE, WM_SETTEXT, 0, (LPARAM)"Nombre del contacto:");
			SendDlgItemMessage(hWnd, IDC_STATICGENERO, WM_SETTEXT, 0, (LPARAM)"Genero:");
			SendDlgItemMessage(hWnd, IDC_STATICCATEGORIA, WM_SETTEXT, 0, (LPARAM)"Categoria:");

		}



		return true;
	}
	case WM_COMMAND:
	{
		switch (HIWORD(wParam))
		{
		case CBN_SELENDOK:
		{
			int IndexCate = SendMessage(Categoria, CB_GETCURSEL, 0, 0);
			if (IndexCate == 0)
			{
				ShowWindow(GetDlgItem(hWnd, IDC_NUEVACATEGORIA), SW_SHOW);
				ShowWindow(GetDlgItem(hWnd, IDC_STATIC_NUEVACATEGORIA), SW_SHOW);
				EnableWindow(GetDlgItem(hWnd, IDC_NUEVACATEGORIA), TRUE);
				//MessageBox(hWnd, "Es Otros...", "Comprobacion", MB_OK);
			}
			else
			{
				ShowWindow(GetDlgItem(hWnd, IDC_NUEVACATEGORIA), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_STATIC_NUEVACATEGORIA), SW_HIDE);
				SendMessage(NuevaCategoria, WM_SETTEXT, 0, (LPARAM)"");
				EnableWindow(GetDlgItem(hWnd, IDC_NUEVACATEGORIA), FALSE);
			}

			

		}//Fin case CBN_SELENDOK
			return true;
		}

		switch (LOWORD(wParam))
		{
		case IDC_BMP_ANT:
		{
			SendMessage(GetDlgItem(hWnd, IDC_BMP_STATIC), WM_SETTEXT, 0, (LPARAM)"1/2");
			BMP_Primero = true;
			bmp2 = (HBITMAP)LoadImage(NULL, Primer_bmp, IMAGE_BITMAP, 250, 250, LR_LOADFROMFILE);
			SendDlgItemMessage(
				hWnd,
				IDC_PIC,
				STM_SETIMAGE,
				IMAGE_BITMAP,
				(LPARAM)bmp2);
		}
			return true;
		case IDC_BMP_SIG:
		{
			SendMessage(GetDlgItem(hWnd, IDC_BMP_STATIC), WM_SETTEXT, 0, (LPARAM)"2/2");
			BMP_Primero = false;
			bmp2 = (HBITMAP)LoadImage(NULL, Segundo_bmp, IMAGE_BITMAP, 250, 250, LR_LOADFROMFILE);
			SendDlgItemMessage(
				hWnd,
				IDC_PIC,
				STM_SETIMAGE,
				IMAGE_BITMAP,
				(LPARAM)bmp2);
		}
			return true;
		case IDC_CARGARBMP:
			{
			char UsandoBMP[MAX_PATH] = "";

			SeleccionarArchivo(hWnd, 1, UsandoBMP);
			if (strcmp(UsandoBMP, "") != 0 && EsArchivo == true)
			{
				bmp1 = (HBITMAP)SendDlgItemMessage(hWnd, IDC_PIC, STM_GETIMAGE, IMAGE_BITMAP, 0);
				//Al objeto bmp2, se le asigna una imagen local:
				bmp2 = (HBITMAP)LoadImage(NULL, UsandoBMP, IMAGE_BITMAP, 250, 250, LR_LOADFROMFILE);
				SendDlgItemMessage(
					hWnd,
					IDC_PIC,
					STM_SETIMAGE,
					IMAGE_BITMAP,
					(LPARAM)bmp2);
				if (BMP_Primero == true)
					strcpy_s(Primer_bmp, UsandoBMP);
				else
					strcpy_s(Segundo_bmp,UsandoBMP );
			}
			}
			return true;
		case IDC_CANCEL:
			EndDialog(hWnd, 0);
			return true;
		case IDC_SAVE:
		{
			if (ADDContactos_MostrarDatos == false)
			{
				char Temporal[40] = "";
				char NCategoria[40] = "";
				int IndiceGlobal = 0;
				bool DobleNombre = false;
				bool Continuar = true;
				bool CategoriaExtra = false;
				while (IndiceGlobal < 9)
				{
					if (IndiceGlobal == 0) //Para detectar los datos obligatorios (Nombre, Genero, Categoria, Alarma)
					{
						//Detectar el Combo Box
						int Index = SendMessage(Categoria, CB_GETCURSEL, 0, 0);

						if (Index == 0)
						{
							SendMessage(NuevaCategoria, WM_GETTEXT, sizeof(Temporal), (LPARAM)Temporal);
							int ind = 0;
							do {
								if (Temporal[ind] == NULL || ind == 41)//Verificar si el caracter es Nulo ó Si el indice supera a la cantidad de caracteres del arreglo char
								{
									Continuar = false;
									IndiceGlobal = 10;
									ind = 42;
								}
								else if (Temporal[ind] == 32) //Verificar si el caracter es un "espacio"
								{
									ind++;
								}
								else //Hay caracteres legibles para insertarlos en otra variable  (En caso de que hay espacios antes del caracter)
								{
									for (int i = 0; i < 40; i++)
									{
										if (ind < 40)
										{
											DatosTemporales.Categoria[i] = Temporal[ind];
											ind++;
										}
										else
											DatosTemporales.Categoria[i] = NULL;
									}
									ind = 42;

									CategoriaExtra = true;
									strcpy_s(NCategoria, DatosTemporales.Categoria);
								}
							} while (ind >= 0 && ind <= 40);
						}
						else
						{
							SendMessage(Categoria, CB_GETLBTEXT, (WPARAM)Index, (LPARAM)DatosTemporales.Categoria);
						}
						strcpy_s(Temporal, "");

						//Detectar el Radio Button
						if (Continuar == true)
						{
							if (SendDlgItemMessage(hWnd, IDC_RADIOHOMBRE, BM_GETCHECK, 0, 0) == BST_CHECKED)
							{
								DatosTemporales.Genero = true;
							}
							else if (SendDlgItemMessage(hWnd, IDC_RADIOMUJER, BM_GETCHECK, 0, 0) == BST_CHECKED)
							{
								DatosTemporales.Genero = false;
							}
							else
							{
								IndiceGlobal = 10;
								Continuar = false;
							}
						}

						//Detectar el Edit Box
						if (Continuar == true)
						{
							SendMessage(EditControl[IndiceGlobal], WM_GETTEXT, sizeof(Temporal), (LPARAM)Temporal);
							int ind = 0;
							do {
								if (Temporal[ind] == NULL || ind == 41)//Verificar si el caracter es Nulo ó Si el indice supera a la cantidad de caracteres del arreglo char
								{
									if (IndiceGlobal == 0)
									{
										Continuar = false;
										IndiceGlobal = 10;
										ind = 42;
									}
									else
									{
										ind = 42;
									}
								}
								else if (Temporal[ind] == 32) //Verificar si el caracter es un "espacio"
								{
									ind++;
								}
								else //Hay caracteres legibles para insertarlos en otra variable  (En caso de que hay espacios antes del caracter)
								{
									for (int i = 0; i < 40; i++)
									{
										if (ind < 40)
										{
											DatosTemporales.Nombre[i] = Temporal[ind];
											ind++;
										}
										else
											DatosTemporales.Nombre[i] = NULL;
									}
									ind = 42;

									for (int j = 0; j < 40; j++)
									{
										for (int i = 65; i <= 90; i++)
										{
											if (DatosTemporales.Nombre[j] == i)
											{
												i += 32;
												DatosTemporales.Nombre[j] = i;
												break;
											}
										}
									}
									for (int j = 0; j < 40; j++)
									{
										for (int i = 97; i <= 122; i++)
										{
											if ((DatosTemporales.Nombre[j] == i && j==0) || (DatosTemporales.Nombre[j] == i && DatosTemporales.Nombre[j-1] == 32))
											{
												i -= 32;
												DatosTemporales.Nombre[j] = i;
												break;
											}
										}
									}
								}
							} while (ind >= 0 && ind <= 40);

							if (Continuar == true && Prin != NULL)
							{
								
								Aux2 = Prin;

								while (Aux2 != NULL)
								{
									if (strcmp(Aux2->Nombre, DatosTemporales.Nombre) == 0)
									{
										if (strcmp(Aux2->Nombre, NombreTemporal) != 0)
										{
											DobleNombre = true;
											Continuar = false;
											IndiceGlobal = 10;
											break;
										}
										else
											Aux2 = Aux2->sig;
									}
									else
									Aux2 = Aux2->sig;
								}

								Aux2 = NULL;
							}
						}
						strcpy_s(Temporal, "");
					}	//Fin del If para comprobar los campos obligatorios		
					else
					{
						strcpy_s(Temporal, "");
						SendMessage(EditControl[IndiceGlobal], WM_GETTEXT, sizeof(Temporal), (LPARAM)Temporal);
						int ind = 0;
						do {
							if (Temporal[ind] == NULL || ind == 41)//Verificar si el caracter es Nulo ó Si el indice supera a la cantidad de caracteres del arreglo char
							{
								ind = 42;
								if (IndiceGlobal == 1)
									strcpy_s(DatosTemporales.Direccion.Calle, "NULL");
								if (IndiceGlobal == 2)
									strcpy_s(DatosTemporales.Direccion.Num, "NULL");
								if (IndiceGlobal == 3)
									strcpy_s(DatosTemporales.Direccion.Colonia, "NULL");
								if (IndiceGlobal == 4)
									strcpy_s(DatosTemporales.Direccion.Municipio, "NULL");
								if (IndiceGlobal == 5)
									strcpy_s(DatosTemporales.Direccion.Estado, "NULL");
								if (IndiceGlobal == 6)
									strcpy_s(DatosTemporales.Telefono, "NULL");
								if (IndiceGlobal == 7)
									strcpy_s(DatosTemporales.Celular, "NULL");
								if (IndiceGlobal == 8)
									strcpy_s(DatosTemporales.Correo, "NULL");
							}
							else if (Temporal[ind] == 32) //Verificar si el caracter es un "espacio"
							{
								ind++;
							}
							else //Hay caracteres legibles para insertarlos en otra variable  (En caso de que hay espacios antes del caracter)
							{
								char Temporal2[40] = "";
								for (int i = 0; i < 40; i++)
								{
									if (ind < 40)
									{
										Temporal2[i] = Temporal[ind];
										ind++;
									}
									else
										Temporal2[i] = NULL;
								}
								ind = 42;

								if (IndiceGlobal == 1)
									strcpy_s(DatosTemporales.Direccion.Calle, Temporal2);
								if (IndiceGlobal == 2)
									strcpy_s(DatosTemporales.Direccion.Num, Temporal2);
								if (IndiceGlobal == 3)
									strcpy_s(DatosTemporales.Direccion.Colonia, Temporal2);
								if (IndiceGlobal == 4)
									strcpy_s(DatosTemporales.Direccion.Municipio, Temporal2);
								if (IndiceGlobal == 5)
									strcpy_s(DatosTemporales.Direccion.Estado, Temporal2);
								if (IndiceGlobal == 6)
									strcpy_s(DatosTemporales.Telefono, Temporal2);
								if (IndiceGlobal == 7)
									strcpy_s(DatosTemporales.Celular, Temporal2);
								if (IndiceGlobal == 8)
									strcpy_s(DatosTemporales.Correo, Temporal2);
							}
						} while (ind >= 0 && ind <= 40);

					}
					IndiceGlobal++;
				} //Fin del While
				if (Continuar == true)
				{
					strcpy_s(DatosTemporales.Foto1, Primer_bmp);
					strcpy_s(DatosTemporales.Foto2, Segundo_bmp);
					
					if (ADDContactos_ModificarDatos == false)
						AgregarContacto(DatosTemporales.Nombre, DatosTemporales.Direccion.Calle, DatosTemporales.Direccion.Num, DatosTemporales.Direccion.Colonia, DatosTemporales.Direccion.Municipio, DatosTemporales.Direccion.Estado, DatosTemporales.Genero, DatosTemporales.Correo, DatosTemporales.Telefono, DatosTemporales.Celular, DatosTemporales.Foto1, DatosTemporales.Foto2, DatosTemporales.Categoria,charOrden);
					else
						ModificarContacto(DatosTemporales.Nombre, DatosTemporales.Direccion.Calle, DatosTemporales.Direccion.Num, DatosTemporales.Direccion.Colonia, DatosTemporales.Direccion.Municipio, DatosTemporales.Direccion.Estado, DatosTemporales.Genero, DatosTemporales.Correo, DatosTemporales.Telefono, DatosTemporales.Celular, DatosTemporales.Foto1, DatosTemporales.Foto2, DatosTemporales.Categoria);
					int MiniIndex = 0;
					if (CategoriaExtra == true)
					{
						CategoriaGuardar(hWnd, IDC_CATEGORIA, IDC_NUEVACATEGORIA, NCategoria, false,1,false);

						ShowWindow(GetDlgItem(hWnd, IDC_NUEVACATEGORIA), SW_HIDE);
						ShowWindow(GetDlgItem(hWnd, IDC_STATIC_NUEVACATEGORIA), SW_HIDE);
						EnableWindow(GetDlgItem(hWnd, IDC_NUEVACATEGORIA), FALSE);

					}
					BinarioGuardar();
					if (ADDContactos_ModificarDatos == false)
					{
						if (MessageBox(hWnd, "Se ha guardado correctamente el contacto \n¿Desea agregar otro?", "Aviso", MB_YESNO | MB_ICONINFORMATION) == IDNO)
							EndDialog(hWnd, 0);
					}
					else
					{
						MessageBox(hWnd, "Se ha guardado correctamente el contacto", "Aviso", MB_OK | MB_ICONINFORMATION);
						EndDialog(hWnd, 0);
					}
				}
				else
				{
					if (DobleNombre == false)
						MessageBox(hWnd, "Por favor, completa los campos obligatorios", "Aviso", MB_OK | MB_ICONINFORMATION);
					else
						MessageBox(hWnd, "El nombre del contacto ya existe. Por favor ingrese otro nombre", "Aviso", MB_OK | MB_ICONINFORMATION);
				}
			}
			else
			{
				ADDContactos_ModificarDatos = true;
				EndDialog(hWnd, 0);
			}
		}
		return true;
		case IDC_RESET:
			for (int i = 0; i < 9; i++)
				SendMessage(EditControl[i], WM_SETTEXT, 0, (LPARAM)"");
			SendDlgItemMessage(hWnd, IDC_RADIOHOMBRE, BM_SETCHECK, BST_UNCHECKED, 0);
			SendDlgItemMessage(hWnd, IDC_RADIOMUJER, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(Categoria, CB_SETCURSEL, 1, 0);
			strcpy_s(Primer_bmp, bmp_Default); 
			strcpy_s(Segundo_bmp ,bmp_Default);
			BMP_Primero = true;
			SendDlgItemMessage(hWnd, IDC_BMP_STATIC, WM_SETTEXT, 0, (LPARAM)"1/2");
			bmp2 = (HBITMAP)LoadImage(NULL, bmp_Default, IMAGE_BITMAP, 250, 250, LR_LOADFROMFILE);
			SendDlgItemMessage(
				hWnd,
				IDC_PIC,
				STM_SETIMAGE,
				IMAGE_BITMAP,
				(LPARAM)bmp2);

			return true;
		case IDC_ELIMINAR:
		{
			if (MessageBox(hWnd, "¿Estas seguro que desea eliminar este contacto?", "Advertencia", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				EliminarContacto(Aux);
				ADDContactos_EliminarDato = true;
				EndDialog(hWnd, 0);
			}
		}
			return true;
		return true;
		}
	}
	return true;

	case WM_CLOSE:
		if ((MessageBox(hWnd, "Esta seguro que desea salir", "Advertencia", MB_YESNO | MB_ICONINFORMATION)) == IDYES)
			EndDialog(hWnd, 0);
		return true;
	case WM_DESTROY:
		return true;
	}
	return false;
}

BOOL CALLBACK VentanaPrincipal(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	DetectarAlarma(1, IDC_DT_VP, hWnd, false);
	static int CambieCatego = 0;
	static bool OpenCate = false;
	static int CateIndice = 0;
	static int I = 0;
	static bool YaAgrege[2] = { false,false };
	char VContactos[6][40] = {"Nombre","Genero","Categoria","Telefono","Celular","Correo"};
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		for (int i = 0; i < 6; i++)
		{
			SendDlgItemMessage(hWnd, IDC_BUSCARPOR, CB_ADDSTRING, sizeof(VContactos[i]), (LPARAM)VContactos[i]);
		}
		SendDlgItemMessage(hWnd, IDC_BUSCARPOR, CB_SETCURSEL, 0,0);
		CambieCatego = 0;


		ShowWindow(GetDlgItem(hWnd, IDC_GENEROS), SW_HIDE);
		ShowWindow(GetDlgItem(hWnd, IDC_BUSCARNUM), SW_HIDE);
		ShowWindow(GetDlgItem(hWnd, IDC_CATE_EDIT), SW_HIDE);
		ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_EDIT), SW_HIDE);
		ShowWindow(GetDlgItem(hWnd, IDC_CATE_MOVER), SW_HIDE);
		ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_MOVER), SW_HIDE);
		ShowWindow(GetDlgItem(hWnd, IDC_CATE_CONF), SW_HIDE);

		ReCargarCategorias(hWnd, IDC_CATE_CATE,false);
		ReCargarCategorias(hWnd, IDC_GENEROS,false);
		BinarioCargar();
		ReCargarContactos(hWnd);
		VerificarAlarmasViejas(hWnd);
		AlarmaGuardar();
		ProximoEvento(hWnd);
	
	}
		return true;
	case WM_TIMER:
	{
		if (BoolAlarma == false)
		{
			DetectarAlarma(2, IDC_DT_EVENTOS, hWnd, &BoolAlarma);

		}
	}
	return true;
	case WM_COMMAND:
	{
		switch (HIWORD(wParam))
		{
		case LBN_SELCHANGE:
		{
			int Index = SendDlgItemMessage(hWnd,IDC_LIST_NOMBRE,LB_GETCURSEL, 0,0);
			SendDlgItemMessage(hWnd, IDC_LIST_CATEGORIA, LB_SETCURSEL, Index, 0);
			SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_SETCURSEL, Index, 0);
			SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_SETCURSEL, Index, 0);
		}
			return true;
		case LBN_DBLCLK:
		{
			Aux = Prin;
			char VBuscar[40] = "";
			int Index = SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_GETTEXT, Index, (LPARAM)VBuscar);

			while (Aux != NULL)
			{
				if (strcmp(Aux->Nombre, VBuscar) == 0)
				{
					break;
				}
				else
				{
					Aux = Aux->sig;
				}
			}

			ADDContactos_MostrarDatos = true;
			DialogBox(ADDCont, MAKEINTRESOURCE(IDD_ADDCONTACTOS), VP, ADDContacto);
			ADDContactos_MostrarDatos = false;
			if (ADDContactos_ModificarDatos == true)
			{
				DialogBox(ADDCont, MAKEINTRESOURCE(IDD_ADDCONTACTOS), VP, ADDContacto);
				ReCargarContactos(hWnd);
				BinarioGuardar();
			}
			if (ADDContactos_EliminarDato == true)
			{
				ReCargarContactos(hWnd);
				BinarioGuardar();
				MessageBox(hWnd, "El contacto ha sido eliminado con exito", "Aviso", MB_OK);
			}
			ADDContactos_ModificarDatos = false;
			ADDContactos_EliminarDato = false;
		}
			return true;
		case CBN_SELENDOK:
		{
			int VerificarCate = SendDlgItemMessage(hWnd, IDC_CATE_CATE,CB_GETCURSEL,0,0);
			int VerificarEliminar = SendDlgItemMessage(hWnd, IDC_CATE_MOVER, CB_GETCURSEL, 0, 0);
			if (VerificarCate != CambieCatego)
			{
				ShowWindow(GetDlgItem(hWnd, IDC_CATE_MOVER), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_MOVER), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_CATE_CONF), SW_HIDE);
				CateIndice = 0;
				CambieCatego = VerificarCate;
				SendDlgItemMessage(hWnd, IDC_CATE_EDIT, WM_SETTEXT, 0, (LPARAM)"");
				ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_EDIT), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_CATE_EDIT), SW_HIDE);
				SendDlgItemMessage(hWnd, IDC_CATE_MOVER, CB_SETCURSEL, 1, 0);
				VerificarEliminar = SendDlgItemMessage(hWnd, IDC_CATE_MOVER, CB_GETCURSEL, 0, 0);
			}
				if (VerificarEliminar == 0)
				{

					ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_EDIT), SW_SHOW);
					ShowWindow(GetDlgItem(hWnd, IDC_CATE_EDIT), SW_SHOW);
					SendDlgItemMessage(hWnd, IDC_CATE_STAT_EDIT, WM_SETTEXT, 0, (LPARAM)"Ingresa nueva Categoria:");

					OpenCate = true;
				}
				else
				{
					ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_EDIT), SW_HIDE);
					ShowWindow(GetDlgItem(hWnd, IDC_CATE_EDIT), SW_HIDE);

					OpenCate = false;
				}
			if (SendDlgItemMessage(hWnd, IDC_BUSCARPOR, CB_GETCURSEL, 0, 0) == 1 && YaAgrege[0] == false)
			{

				SendDlgItemMessage(hWnd, IDC_GENEROS, CB_RESETCONTENT, 0, 0);
				SendDlgItemMessage(hWnd, IDC_GENEROS, CB_ADDSTRING, 0, (LPARAM)"Hombre");
				SendDlgItemMessage(hWnd, IDC_GENEROS, CB_ADDSTRING, 0, (LPARAM)"Mujer");
				SendDlgItemMessage(hWnd, IDC_GENEROS, CB_SETCURSEL, 0, 0);

				ShowWindow(GetDlgItem(hWnd, IDC_BUSCAR), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_GENEROS), SW_SHOW);
				ShowWindow(GetDlgItem(hWnd, IDC_BUSCARNUM), SW_HIDE);
				SendDlgItemMessage(hWnd, IDC_BUSCARNUM, WM_SETTEXT, 0, (LPARAM)"");
				SendDlgItemMessage(hWnd, IDC_BUSCAR, WM_SETTEXT, 0, (LPARAM)"");
				YaAgrege[0] = true;
				YaAgrege[1] = false;
			}
			if (SendDlgItemMessage(hWnd, IDC_BUSCARPOR, CB_GETCURSEL, 0, 0) == 2 && YaAgrege[1] == false)
			{
				SendDlgItemMessage(hWnd, IDC_GENEROS, CB_RESETCONTENT, 0, 0);
				ifstream CateIn;
				CateIn.open(DireccionPrograma_Categorias, ios::in);

				if (CateIn.is_open())
				{
					char Categorias[40] = "";

					CateIn.getline(Categorias, 40, '\n');
					while (!CateIn.eof())
					{
						SendDlgItemMessage(hWnd, IDC_GENEROS, CB_ADDSTRING, sizeof(Categorias), (LPARAM)Categorias);

						CateIn.getline(Categorias, 40, '\n');
					}
					CateIn.close();
				}
				SendDlgItemMessage(hWnd, IDC_GENEROS, CB_DELETESTRING, 0, 0);
				SendDlgItemMessage(hWnd, IDC_GENEROS, CB_SETCURSEL, 0, 0);

				ShowWindow(GetDlgItem(hWnd, IDC_BUSCAR), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_GENEROS), SW_SHOW);
				ShowWindow(GetDlgItem(hWnd, IDC_BUSCARNUM), SW_HIDE);
				SendDlgItemMessage(hWnd, IDC_BUSCARNUM, WM_SETTEXT, 0, (LPARAM)"");
				SendDlgItemMessage(hWnd, IDC_BUSCAR, WM_SETTEXT, 0, (LPARAM)"");
				YaAgrege[0] = false;
				YaAgrege[1] = true;
			}
			if (SendDlgItemMessage(hWnd, IDC_BUSCARPOR, CB_GETCURSEL, 0, 0) == 3 || SendDlgItemMessage(hWnd, IDC_BUSCARPOR, CB_GETCURSEL, 0, 0) == 4)
			{
				YaAgrege[0] = false;
				YaAgrege[1] = false;
				ShowWindow(GetDlgItem(hWnd, IDC_BUSCAR), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_BUSCARNUM), SW_SHOW);
				ShowWindow(GetDlgItem(hWnd, IDC_GENEROS), SW_HIDE);
				SendDlgItemMessage(hWnd, IDC_BUSCAR, WM_SETTEXT, 0, (LPARAM)"");
				SendDlgItemMessage(hWnd, IDC_BUSCAR, ES_NUMBER, true, true);
				SetWindowLong(GetDlgItem(hWnd, IDC_BUSCAR), ES_NUMBER, true);
			}
			if (SendDlgItemMessage(hWnd, IDC_BUSCARPOR, CB_GETCURSEL, 0, 0) == 0 || SendDlgItemMessage(hWnd, IDC_BUSCARPOR, CB_GETCURSEL, 0, 0) == 5)
			{
				YaAgrege[0] = false;
				YaAgrege[1] = false;
				ShowWindow(GetDlgItem(hWnd, IDC_BUSCAR), SW_SHOW);
				ShowWindow(GetDlgItem(hWnd, IDC_BUSCARNUM), SW_HIDE);
				SendDlgItemMessage(hWnd, IDC_BUSCARNUM, WM_SETTEXT, 0, (LPARAM)"");
				ShowWindow(GetDlgItem(hWnd, IDC_GENEROS), SW_HIDE);
				SetWindowLong(GetDlgItem(hWnd, IDC_BUSCAR), ES_NUMBER, false);
			}
		}
			return true;

		}

		switch (LOWORD(wParam))
		{
		case IDC_CATE_NUEVO:
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_MOVER), SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_MOVER), SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_CONF), SW_SHOW);
			CateIndice = 1;
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_EDIT), SW_SHOW);
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_EDIT), SW_SHOW);
			SendDlgItemMessage(hWnd, IDC_CATE_STAT_EDIT, WM_SETTEXT, 0, (LPARAM)"Ingresa nueva Categoria:");

			return true;
		case IDC_CATE_MODI:
		{
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_CONF), SW_SHOW);
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_MOVER), SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_MOVER), SW_HIDE);
			CateIndice = 2;
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_EDIT), SW_SHOW);
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_EDIT), SW_SHOW);
			char Text1[70] = "";
			char Text2[40] = "";
			char Text3[5] = "";
			I = SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETLBTEXT, I, (LPARAM)Text2);
			strcpy_s(Text1, "Modifica la categoria \"");
			strcpy_s(Text3, "\" :");
			strcat_s(Text1, Text2);
			strcat_s(Text1, Text3);
			SendDlgItemMessage(hWnd, IDC_CATE_STAT_EDIT, WM_SETTEXT, 0, (LPARAM)Text1);
		}
			return true;
		case IDC_CATE_ELIM:
		{
			CateIndice = 3;
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_CONF), SW_SHOW);
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_MOVER), SW_SHOW);
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_EDIT), SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_EDIT), SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_MOVER), SW_SHOW);

			ReCargarCategorias(hWnd, IDC_CATE_MOVER, true);
			I = SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETCURSEL, 0, 0);

			SendDlgItemMessage(hWnd, IDC_CATE_MOVER, CB_DELETESTRING, I + 1, 0);


			char Text1[200] = "";
			char Text2[40] = "";
			char Text3[40] = "";
			I = SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETLBTEXT, I, (LPARAM)Text2);
			strcpy_s(Text1, "Los contactos con \"");
			strcpy_s(Text3, "\" se moveran a la categoria :");
			strcat_s(Text1, Text2);
			strcat_s(Text1, Text3);
			SendDlgItemMessage(hWnd, IDC_CATE_STAT_MOVER, WM_SETTEXT, 0, (LPARAM)Text1);

			SendDlgItemMessage(hWnd, IDC_CATE_MOVER, CB_SETCURSEL, 1, 0);

		}
			return true;
		case IDC_CATE_TRAS:
		{
			CateIndice = 4;
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_CONF), SW_SHOW);
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_MOVER), SW_SHOW);
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_EDIT), SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_EDIT), SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_CATE_MOVER), SW_SHOW);

			ReCargarCategorias(hWnd, IDC_CATE_MOVER, true);
			I = SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETCURSEL, 0, 0);

			SendDlgItemMessage(hWnd, IDC_CATE_MOVER, CB_DELETESTRING, I + 1, 0);


			char Text1[200] = "";
			char Text2[40] = "";
			char Text3[40] = "";
			I = SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETLBTEXT, I, (LPARAM)Text2);
			strcpy_s(Text1, "Los contactos con \"");
			strcpy_s(Text3, "\" se moveran a la categoria :");
			strcat_s(Text1, Text2);
			strcat_s(Text1, Text3);
			SendDlgItemMessage(hWnd, IDC_CATE_STAT_MOVER, WM_SETTEXT, 0, (LPARAM)Text1);

			SendDlgItemMessage(hWnd, IDC_CATE_MOVER, CB_SETCURSEL, 1, 0);
		}
		return true;
		case IDC_CATE_CONF:
		{
			char Temporal[40] = "";
			char NuevaCate[40] = "";
			SendMessage(GetDlgItem(hWnd, IDC_CATE_EDIT), WM_GETTEXT, sizeof(Temporal), (LPARAM)Temporal);
			bool NoGuardar = false;
			int ind = 0;

			do {
				if (Temporal[ind] == NULL || ind == 41)//Verificar si el caracter es Nulo ó Si el indice supera a la cantidad de caracteres del arreglo char
				{
					ind = 42;
					NoGuardar = true;
				}
				else if (Temporal[ind] == 32) //Verificar si el caracter es un "espacio"
				{
					ind++;
				}
				else //Hay caracteres legibles para insertarlos en otra variable  (En caso de que hay espacios antes del caracter)
				{
					for (int i = 0; i < 40; i++)
					{
						if (ind < 40)
						{
							NuevaCate[i] = Temporal[ind];
							ind++;
						}
						else
							NuevaCate[i] = NULL;
					}
					ind = 42;
				}
			} while (ind >= 0 && ind <= 40);

			if (NoGuardar == false || CateIndice == 3 || CateIndice == 4)
			{
				if (CateIndice == 1)
				{
					CategoriaGuardar(hWnd, IDC_CATE_CATE, IDC_CATE_EDIT, NuevaCate, false, 0,false);
					CateIndice = 0;
					ShowWindow(GetDlgItem(hWnd, IDC_CATE_EDIT), SW_HIDE);
					ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_EDIT), SW_HIDE);
					ShowWindow(GetDlgItem(hWnd, IDC_CATE_CONF), SW_HIDE);
				}
				else if (CateIndice == 2)
				{
					bool Seguir = true;
					int Ult = SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETCOUNT, 0, 0);
					for (int i = 0; i < Ult; i++)
					{
						char ViejaCate[40] = "";
						SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETLBTEXT, i, (LPARAM)ViejaCate);
						if (strcmp(ViejaCate, NuevaCate) == 0)
						{
							Seguir = false;
							break;
						}

					}

					if (Seguir == true)
					{
						if (MessageBox(hWnd, "Los contactos que tengan esa categoria\nseran reemplazadas por la nueva categoria.\n\n        ¿Deseas continuar?", "Advertencia", MB_YESNO) == IDYES)
						{
							
							char CategoriaVieja[40] = "";
							SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETLBTEXT, I, (LPARAM)CategoriaVieja);
							CateIndice = 0;
							CategoriaGuardar(hWnd, IDC_CATE_CATE, IDC_CATE_EDIT, NuevaCate, true, I,false);
							ReCargarCategorias(hWnd, IDC_CATE_CATE,false);
							ReCargarCategorias(hWnd, IDC_GENEROS,false);
							SendMessage(GetDlgItem(hWnd, IDC_CATE_CATE), CB_SETCURSEL, I, 0);
							ShowWindow(GetDlgItem(hWnd, IDC_CATE_EDIT), SW_HIDE);
							ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_EDIT), SW_HIDE);
							ShowWindow(GetDlgItem(hWnd, IDC_CATE_CONF), SW_HIDE);

							if (Prin != NULL)
							{
								Aux = Prin;
								while (Aux != NULL)
								{
									if (strcmp(Aux->Categoria, CategoriaVieja) == 0)
									{
										strcpy_s(Aux->Categoria, NuevaCate);
									}
									Aux = Aux->sig;
								}
								Aux = NULL;
							}
							ReCargarContactos(hWnd);
							BinarioGuardar();
						}
					}
					else
					{
						MessageBox(hWnd, "Ya existe una categoria igual. Por favor, ingrese otro nombre", "Aviso", MB_OK);
					}
				}
				else if (CateIndice == 3)
				{
					int n = MessageBox(hWnd, "Los contactos que tengan esa categoria\npueden ser eliminados o moverlos a otra categoria.\n\nSi deseas eliminar los contactos con esa categoria, presiona \"Si\"\nSi deseas mover los contactos a la categoria seleccionada, presiona \"No\"\nSi deseas cancelar la eliminacion de la categoria, presiona \"Cancelar\"", "Advertencia", MB_YESNOCANCEL);					
					
					if (n== IDYES)
					{
						char CateElim[40] = "";
						SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETLBTEXT, I, (LPARAM)CateElim);
						if (Prin != NULL)
						{
							Aux = Prin;
							Contactos *AuxSig;
							AuxSig = Aux->sig;
							while (Aux != NULL)
							{
								if (strcmp(Aux->Categoria, CateElim) == 0)
								{
									EliminarContacto(Aux);
								}

								
								Aux = AuxSig;
								if (AuxSig !=NULL)
									AuxSig = AuxSig->sig;
							}
							Aux = AuxSig = NULL;
						}
						

						CategoriaGuardar(hWnd, IDC_CATE_CATE, IDC_CATE_EDIT, CateElim, true, I, true);
						ReCargarCategorias(hWnd, IDC_CATE_CATE, false);
						ReCargarCategorias(hWnd, IDC_GENEROS, false);
						ReCargarContactos(hWnd);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_MOVER), SW_HIDE);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_MOVER), SW_HIDE);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_CONF), SW_HIDE);
						BinarioGuardar();
						CateIndice = 0;

						MessageBox(hWnd, "Los contactos y la categoria ha sido eliminados con exito", "Aviso", MB_OK);
					}
					else if (n == IDNO)
					{
						char MoverCate[40] = "";
						if (NoGuardar == true)
						{
							int u= SendDlgItemMessage(hWnd, IDC_CATE_MOVER, CB_GETCURSEL, 0,0);
							SendDlgItemMessage(hWnd, IDC_CATE_MOVER, CB_GETLBTEXT, u, (LPARAM)MoverCate);
						}
						else
						{
							strcpy_s(MoverCate, NuevaCate);

						}

						char CateElim[40] = "";
						SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETLBTEXT, I, (LPARAM)CateElim);
						if (Prin != NULL)
						{
							Aux = Prin;
							while (Aux != NULL)
							{
								if (strcmp(Aux->Categoria, CateElim) == 0)
								{
									strcpy_s(Aux->Categoria, MoverCate);
								}

								Aux = Aux->sig;
							}
						}
						Aux = NULL;
						if (NoGuardar == false)
						{
							CategoriaGuardar(hWnd, IDC_CATE_CATE, IDC_CATE_EDIT, MoverCate, false, 1, false);
						}

						CategoriaGuardar(hWnd, IDC_CATE_CATE, IDC_CATE_EDIT, CateElim, true, I, true);
						ReCargarCategorias(hWnd, IDC_CATE_CATE, false);
						ReCargarCategorias(hWnd, IDC_GENEROS, false);
						ReCargarContactos(hWnd);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_MOVER), SW_HIDE);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_MOVER), SW_HIDE);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_CONF), SW_HIDE);
						BinarioGuardar();
						CateIndice = 0;
						
						MessageBox(hWnd, "La categoria se ha eliminado correctamente", "Aviso", MB_OK);
					}
				}
				else if (CateIndice == 4)
				{
					if (MessageBox(hWnd, "Los contactos que tengan esa categoria\nseran movidos a la nueva categoria.\n\n        ¿Deseas continuar?", "Advertencia", MB_YESNO) == IDYES)
					{
						char MoverCate[40] = "";
						if (NoGuardar == true)
						{
							int u = SendDlgItemMessage(hWnd, IDC_CATE_MOVER, CB_GETCURSEL, 0, 0);
							SendDlgItemMessage(hWnd, IDC_CATE_MOVER, CB_GETLBTEXT, u, (LPARAM)MoverCate);
						}
						else
						{
							strcpy_s(MoverCate, NuevaCate);

						}

						char CateElim[40] = "";
						SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETLBTEXT, I, (LPARAM)CateElim);
						if (Prin != NULL)
						{
							Aux = Prin;
							while (Aux != NULL)
							{
								if (strcmp(Aux->Categoria, CateElim) == 0)
								{
									strcpy_s(Aux->Categoria, MoverCate);
								}

								Aux = Aux->sig;
							}
						}
						Aux = NULL;
						if (NoGuardar == false)
						{
							CategoriaGuardar(hWnd, IDC_CATE_CATE, IDC_CATE_EDIT, MoverCate, false, 1, false);
						}

						ReCargarCategorias(hWnd, IDC_CATE_CATE, false);
						ReCargarCategorias(hWnd, IDC_GENEROS, false);
						ReCargarContactos(hWnd);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_MOVER), SW_HIDE);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_STAT_MOVER), SW_HIDE);
						ShowWindow(GetDlgItem(hWnd, IDC_CATE_CONF), SW_HIDE);
						BinarioGuardar();
						CateIndice = 0;

						MessageBox(hWnd, "Los contactos se han movido correctamente", "Aviso", MB_OK);
					}
				}
			}
			else
			{
				MessageBox(hWnd, "Por favor, ingrese un nombre a la categoria", "Advertencia", MB_OK);
			}
			CambieCatego = SendDlgItemMessage(hWnd, IDC_CATE_CATE, CB_GETCURSEL, 0, 0);

		}
			return true;
		case IDC_BOTONREINICIAR:
			ReCargarContactos(hWnd);
			SendDlgItemMessage(hWnd, IDC_BUSCAR, WM_SETTEXT, 0, (LPARAM)"");
			return true;
		case IDC_BOTONBUSCAR:
		{
			if (Prin != NULL)
			{
				Aux = Prin;
				char VBuscar[40] = "";
				int Contador = 0;
				int Indice = SendDlgItemMessage(hWnd, IDC_BUSCARPOR, CB_GETCURSEL, 0, 0);
				if (Indice == 3 || Indice ==4)
					SendDlgItemMessage(hWnd, IDC_BUSCARNUM, WM_GETTEXT, sizeof(VBuscar), (LPARAM)VBuscar);
				else
					SendDlgItemMessage(hWnd, IDC_BUSCAR, WM_GETTEXT, sizeof(VBuscar), (LPARAM)VBuscar);

				int Aproximado = 0;
				while (Aproximado != 40)
				{
					if (VBuscar[Aproximado] == NULL)
					{
						break;
					}
					else
					{
						Aproximado++;
					}

				}

				SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_RESETCONTENT, 0, 0);
				SendDlgItemMessage(hWnd, IDC_LIST_CATEGORIA, LB_RESETCONTENT, 0, 0);
				SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_RESETCONTENT, 0, 0);

				while (Aux != NULL)
				{
					if (Indice == 0)
					{
						for (int j = 0; j < 40; j++)
						{

							if (Aux->Nombre[j] == VBuscar[0] || Aux->Nombre[j] == VBuscar[0]+32 || Aux->Nombre[j] == VBuscar[0]-32)
							{
								bool EsAprox = true;
								int p = 0;
								for (int i = j; i < j + Aproximado; i++)
								{

									if (Aux->Nombre[i] != VBuscar[p] && Aux->Nombre[i] != VBuscar[p]+32 && Aux->Nombre[i] != VBuscar[p]-32)
									{
										EsAprox = false;
									}
									else
									{
										p++;
									}
								}
								if (EsAprox == true)
								{
									SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_ADDSTRING, 0, (LPARAM)Aux->Nombre);
									if (Aux->Genero == true)
										SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, 0, (LPARAM)"Hombre");
									else
										SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, 0, (LPARAM)"Mujer");
									SendDlgItemMessage(hWnd, IDC_LIST_CATEGORIA, LB_ADDSTRING, 0, (LPARAM)Aux->Categoria);
									Contador++;
									break;
								}
							}
						}
					}
					else if (Indice == 1)
					{
						if (SendDlgItemMessage(hWnd, IDC_GENEROS, CB_GETCURSEL, 0, 0) == 0)
						{
							if (Aux->Genero == true)
							{
								SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_ADDSTRING, 0, (LPARAM)Aux->Nombre);
								if (Aux->Genero == true)
									SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, 0, (LPARAM)"Hombre");
								else
									SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, 0, (LPARAM)"Mujer");
								SendDlgItemMessage(hWnd, IDC_LIST_CATEGORIA, LB_ADDSTRING, 0, (LPARAM)Aux->Categoria);
								Contador++;
							}
						}
						else
						{
							if (Aux->Genero == false)
							{
								SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_ADDSTRING, 0, (LPARAM)Aux->Nombre);
								if (Aux->Genero == true)
									SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, 0, (LPARAM)"Hombre");
								else
									SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, 0, (LPARAM)"Mujer");
								SendDlgItemMessage(hWnd, IDC_LIST_CATEGORIA, LB_ADDSTRING, 0, (LPARAM)Aux->Categoria);
								Contador++;
							}
						}
					}
					else if (Indice == 2)
					{
						int indice = SendDlgItemMessage(hWnd, IDC_GENEROS, CB_GETCURSEL, 0, 0);
						char PruebaCategoria[40] = "";
						SendDlgItemMessage(hWnd, IDC_GENEROS, CB_GETLBTEXT, indice, (LPARAM)PruebaCategoria);
						if (strcmp(PruebaCategoria, Aux->Categoria) == 0)
						{
							SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_ADDSTRING, 0, (LPARAM)Aux->Nombre);
							if (Aux->Genero == true)
								SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, 0, (LPARAM)"Hombre");
							else
								SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, 0, (LPARAM)"Mujer");
							SendDlgItemMessage(hWnd, IDC_LIST_CATEGORIA, LB_ADDSTRING, 0, (LPARAM)Aux->Categoria);
							Contador++;
						}
					}
					else if (Indice == 3)
					{
						for (int j = 0; j < 40; j++)
						{
							if (Aux->Telefono[j] == VBuscar[0])
							{
								bool EsAprox = true;
								int p = 0;
								for (int i = j; i < j + Aproximado; i++)
								{

									if (Aux->Telefono[i] != VBuscar[p])
									{
										EsAprox = false;
									}
									else
									{
										p++;
									}
								}
								if (EsAprox == true)
								{
									SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_ADDSTRING, 0, (LPARAM)Aux->Nombre);
									if (Aux->Genero == true)
										SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, 0, (LPARAM)"Hombre");
									else
										SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, 0, (LPARAM)"Mujer");
									SendDlgItemMessage(hWnd, IDC_LIST_CATEGORIA, LB_ADDSTRING, 0, (LPARAM)Aux->Categoria);
									Contador++;
									break;
								}
							}
						}
					}
					else if (Indice == 4)
					{
						for (int j = 0; j < 40; j++)
						{
							if (Aux->Celular[j] == VBuscar[0])
							{
								bool EsAprox = true;
								int p = 0;
								for (int i = j; i < j + Aproximado; i++)
								{

									if (Aux->Celular[i] != VBuscar[p])
									{
										EsAprox = false;
									}
									else
									{
										p++;
									}
								}
								if (EsAprox == true)
								{
									SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_ADDSTRING, 0, (LPARAM)Aux->Nombre);
									if (Aux->Genero == true)
										SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, 0, (LPARAM)"Hombre");
									else
										SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, 0, (LPARAM)"Mujer");
									SendDlgItemMessage(hWnd, IDC_LIST_CATEGORIA, LB_ADDSTRING, 0, (LPARAM)Aux->Categoria);
									Contador++;
									break;
								}
							}
						}
					}
					else
					{
						for (int j = 0; j < 40; j++)
						{

							if (Aux->Correo[j] == VBuscar[0] || Aux->Correo[j] == VBuscar[0] + 32 || Aux->Correo[j] == VBuscar[0] - 32)
							{
								bool EsAprox = true;
								int p = 0;
								for (int i = j; i < j + Aproximado; i++)
								{

									if (Aux->Correo[i] != VBuscar[p] && Aux->Correo[i] != VBuscar[p] + 32 && Aux->Correo[i] != VBuscar[p] - 32)
									{
										EsAprox = false;
									}
									else
									{
										p++;
									}
								}
								if (EsAprox == true)
								{
									SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_ADDSTRING, 0, (LPARAM)Aux->Nombre);
									if (Aux->Genero == true)
										SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, 0, (LPARAM)"Hombre");
									else
										SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, 0, (LPARAM)"Mujer");
									SendDlgItemMessage(hWnd, IDC_LIST_CATEGORIA, LB_ADDSTRING, 0, (LPARAM)Aux->Categoria);
									Contador++;
									break;
								}
							}
						}
					}

					Aux = Aux->sig;
				}
				if (Contador == 0)
					MessageBox(hWnd, "No existe el dato a buscar", "Advertencia", MB_OK | MB_ICONINFORMATION);
			}
		}

			return true;
		case IDC_TEVENTOS:
			SeleccioneDesdeCalendario = false;
			DialogBox(hTEventos, MAKEINTRESOURCE(IDD_TEVENTOS), VP, TodoslosEventos);
			ProximoEvento(hWnd);
			return true;
		case IDC_SALIR:
			if (MessageBox(hWnd, "\t¿Estas seguro que deseas Salir?\n\nLas alarmas no sonaran si el programa no está en funcionamiento", "Advertencia", MB_YESNO | MB_ICONINFORMATION) == IDYES)
				DestroyWindow(hWnd);
			return true;
		//Menus:
		case ID_EVENTOS_CALENDARIO:
			DialogBox(hCalendario, MAKEINTRESOURCE(IDD_CALENDARIO), VP, Calendario);
			ProximoEvento(hWnd);
			return true;
		case ID_EVENTOS_CALENDARIO2:
			DialogBox(hCalendario2, MAKEINTRESOURCE(IDD_CALENDARIO2), VP, Calendario2);
			ProximoEvento(hWnd);
			return true;
		case ID_EVENTOS_EVENTOS:
			MostrarAlarmaSeleccionada = false;
			SeleccioneDesdeCalendario = false;
			DialogBox(hEventos, MAKEINTRESOURCE(IDD_EVENTOS), VP, Eventos);
			ProximoEvento(hWnd);
			return true;
		case ID_EVENTOS_MOSTRAREVENTOS:
			SeleccioneDesdeCalendario = false;
			DialogBox(hTEventos, MAKEINTRESOURCE(IDD_TEVENTOS), VP, TodoslosEventos);
			ProximoEvento(hWnd);
			return true;
		case ID_CONTACTOS_AGREGARUNCONTACTO:
			ADDContactos_MostrarDatos = false;
			DialogBox(ADDCont, MAKEINTRESOURCE(IDD_ADDCONTACTOS), VP, ADDContacto);
			ReCargarContactos(hWnd);
			ReCargarCategorias(hWnd,IDC_CATE_CATE,false);
			ReCargarCategorias(hWnd, IDC_GENEROS,false);
			return true;
		case ID_ARCHIVO_GUARDARARCHIVO:
			BinarioGuardar();
			return true;
		case ID_ARCHIVO_CARGARARCHIVO:
			BinarioCargar();
			ReCargarContactos(hWnd);
			return true;
		case ID_CONTACTOS_MODIFICARUNCONTACTO:
		{
			int Numerito = SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_GETCURSEL, 0, 0);
			if (Numerito > -1)
			{
				char VBuscar[40] = "";
				SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_GETTEXT, Numerito, (LPARAM)VBuscar);
				
				Aux = Prin;

				while (Aux != NULL)
				{
					if (strcmp(Aux->Nombre, VBuscar) == 0)
					{
						break;
					}
					else
					{
						Aux = Aux->sig;
					}
				}

				ADDContactos_MostrarDatos = false;
				ADDContactos_ModificarDatos = true;
				DialogBox(ADDCont, MAKEINTRESOURCE(IDD_ADDCONTACTOS), VP, ADDContacto);
				ADDContactos_MostrarDatos = false;
				ADDContactos_ModificarDatos = false;
			}
			else
			{
				MessageBox(hWnd, "Por favor, seleccione uno de la lista \"Nombre\"", "Aviso", MB_OK);
			}
		}
		return true;
		case ID_CONTACTOS_ELIMINARUNCONTACTO:
		{
			int Numerito = SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_GETCURSEL, 0, 0);
			if (Numerito > -1)
			{
				char VBuscar[40] = "";
				SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_GETTEXT, Numerito, (LPARAM)VBuscar);

				Aux = Prin;

				while (Aux != NULL)
				{
					if (strcmp(Aux->Nombre, VBuscar) == 0)
					{
						break;
					}
					else
					{
						Aux = Aux->sig;
					}
				}
				if (MessageBox(hWnd, "¿Estas seguro que desea eliminar este contacto?", "Advertencia", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				{
					EliminarContacto(Aux);
					ReCargarContactos(hWnd);
					BinarioGuardar();
					MessageBox(hWnd, "El contacto ha sido eliminado con exito", "Aviso", MB_OK);
				}
			}
			else
			{
				MessageBox(hWnd, "Por favor, seleccione uno de la lista \"Nombre\"", "Aviso", MB_OK);
			}
		}
			return true;
		case ID_ARCHIVO_SALIR:
			if (MessageBox(hWnd, "\t¿Estas seguro que deseas Salir?\n\nLas alarmas no sonaran si el programa no está en funcionamiento","Advertencia",MB_YESNO | MB_ICONINFORMATION) == IDYES)
				DestroyWindow(hWnd);
			return true;
		case ID_ORDENARPOR_SINORDENAR:
			ContactoOrdenarPor(0);
			ReCargarContactos(hWnd);
			return true;
		case ID_ORDENARPOR_PORNOMBRE:
			ContactoOrdenarPor(1);
			ReCargarContactos(hWnd);
			return true;
		case ID_ORDENARPOR_PORCATEGORIA:
			ContactoOrdenarPor(2);
			ReCargarContactos(hWnd);
			return true;
		case ID_ACERCADE_INFORMACION:

			MessageBox(hWnd,
				"-----------------------------------------------------------------------\n"
				" Lista de Contactos\n"
				" Version 2.0\n"
				" Creado por Cristopher Navarro Acosta. \n"
				" Realizado para el Proyecto de Programacion II\n"
				" Materia iniciada el 17 de Enero del 2017\n\n"
				" El proyecto fue empezado el 16 de Marzo del 2017.\n"
				" Ultima Modificacion el 10 de Diciembre del 2019. \n"
				"-----------------------------------------------------------------------\n"
				,
				"Acerca de...",
				0
			);


			return true;
		}
	}
	return true;

	case WM_CLOSE:
		if (MessageBox(hWnd, "\t¿Estas seguro que deseas Salir?\n\nLas alarmas no sonaran si el programa no está en funcionamiento", "Advertencia", MB_YESNO | MB_ICONINFORMATION) == IDYES)
			DestroyWindow(hWnd);
		return true;
	case WM_DESTROY:
		PostQuitMessage(0);
		return true;
	}
	return false;

}

BOOL CALLBACK Calendario(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DetectarAlarma(1, IDC_DT_CALENDARIO, hWnd, false);

	static int Parametro; //1: Lunes - 2: Martes - 3: Miercoles - 4: Jueves - 5: Viernes - 6: Sabado -  7: Domingo
	static int Ano = 0;
	static int Mes = 0;
	static int Dia = 0;
	static int TodosMeses[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	static int UltimoDia;

	switch (msg)
	{
	case WM_INITDIALOG:
	{
		Ano = timeinfo.tm_year + 1900;
		Mes = timeinfo.tm_mon;
		Dia = timeinfo.tm_mday;
		UltimoDia = TodosMeses[Mes];

		if (timeinfo.tm_wday == 0)
		{
			Parametro = 7;
		}
		else
		{
			Parametro = timeinfo.tm_wday;
		}
		for (int DiasMenos = timeinfo.tm_mday; DiasMenos > 1; DiasMenos--)
		{
			Parametro--;
			if (Parametro == 0)
				Parametro = 7;
		}


		if (Ano % 4 == 0)
		{
			TodosMeses[1] = 29;
		}
		else
			TodosMeses[1] = 28;
		EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano,Dia,timeinfo.tm_mon+1,timeinfo.tm_year+1900);
	}
	return true;
	case WM_TIMER:
	{
		if (BoolAlarma == false)
		{
			DetectarAlarma(2, IDC_DT_EVENTOS, hWnd, &BoolAlarma);

		}
	}
	return true;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_OCALENDARIO:
		{
			EndDialog(hWnd, 0);
			DialogBox(hCalendario2, MAKEINTRESOURCE(IDD_CALENDARIO2), VP, Calendario2);
		}
		return true;
		case ID_OEVENTO:
			MostrarAlarmaSeleccionada = false;
			SeleccioneDesdeCalendario = false;
			DialogBox(hEventos, MAKEINTRESOURCE(IDD_EVENTOS), hWnd, Eventos);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case ID_OSALIR:
			EndDialog(hWnd, 0);
			return true;
		case IDC_NEXTMONTH:
		{
			Parametro = ParametroGlobal;
			if (Parametro == 8)
				Parametro = 1;
			Mes++;
			if (Mes == 12)
			{
				Mes = 0;
				Ano++;
				if (Ano % 4 == 0)
				{
					TodosMeses[1] = 29;
				}
				else
					TodosMeses[1] = 28;
			}
			UltimoDia = TodosMeses[Mes];
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia,Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
		}
			return true;
		case IDC_BACKMONTH:
		{

			Mes--;
			if (Mes == -1)
			{
				Mes = 11;
				Ano--;
				if (Ano % 4 == 0)
					TodosMeses[1] = 29;
				else
					TodosMeses[1] = 28;
			}
			UltimoDia = TodosMeses[Mes];
			for (int i = UltimoDia-1; i >= 0; i--)
			{
				Parametro--;
				if (Parametro == 0)
					Parametro = 7;
			}

			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
		}
		return true;
		//------------------- Calendario (Izquierda a Derecha, Arriba a Abajo) -----------------
		case IDC_BLUNES_1:
			BotonesdelCalendario(hWnd, IDC_BLUNES_1,0,0);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BLUNES_2:
			BotonesdelCalendario(hWnd, IDC_BLUNES_2,0,1);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BLUNES_3:
			BotonesdelCalendario(hWnd, IDC_BLUNES_3,0,2);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BLUNES_4:
			BotonesdelCalendario(hWnd, IDC_BLUNES_4,0,3);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BLUNES_5:
			BotonesdelCalendario(hWnd, IDC_BLUNES_5,0,4);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BLUNES_6:
			BotonesdelCalendario(hWnd, IDC_BLUNES_6,0,5);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BMARTES_1:
			BotonesdelCalendario(hWnd, IDC_BMARTES_1,1,0);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BMARTES_2:
			BotonesdelCalendario(hWnd, IDC_BMARTES_2,1,1);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BMARTES_3:
			BotonesdelCalendario(hWnd, IDC_BMARTES_3,1,2);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BMARTES_4:
			BotonesdelCalendario(hWnd, IDC_BMARTES_4,1,3);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BMARTES_5:
			BotonesdelCalendario(hWnd, IDC_BMARTES_5,1,4);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BMARTES_6:
			BotonesdelCalendario(hWnd, IDC_BMARTES_6,1,5);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BMIERCOLES_1:
			BotonesdelCalendario(hWnd, IDC_BMIERCOLES_1,2,0);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BMIERCOLES_2:
			BotonesdelCalendario(hWnd, IDC_BMIERCOLES_2,2,1);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BMIERCOLES_3:
			BotonesdelCalendario(hWnd, IDC_BMIERCOLES_3,2,2);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BMIERCOLES_4:
			BotonesdelCalendario(hWnd, IDC_BMIERCOLES_4,2,3);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BMIERCOLES_5:
			BotonesdelCalendario(hWnd, IDC_BMIERCOLES_5,2,4);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BMIERCOLES_6:
			BotonesdelCalendario(hWnd, IDC_BMIERCOLES_6,2,5);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BJUEVES_1:
			BotonesdelCalendario(hWnd, IDC_BJUEVES_1,3,0);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BJUEVES_2:
			BotonesdelCalendario(hWnd, IDC_BJUEVES_2,3,1);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BJUEVES_3:
			BotonesdelCalendario(hWnd, IDC_BJUEVES_3,3,2);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BJUEVES_4:
			BotonesdelCalendario(hWnd, IDC_BJUEVES_4,3,3);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BJUEVES_5:
			BotonesdelCalendario(hWnd, IDC_BJUEVES_5,3,4);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BJUEVES_6:
			BotonesdelCalendario(hWnd, IDC_BJUEVES_6,3,5);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BVIERNES_1:
			BotonesdelCalendario(hWnd, IDC_BVIERNES_1,4,0);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BVIERNES_2:
			BotonesdelCalendario(hWnd, IDC_BVIERNES_2,4,1);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BVIERNES_3:
			BotonesdelCalendario(hWnd, IDC_BVIERNES_3,4,2);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BVIERNES_4:
			BotonesdelCalendario(hWnd, IDC_BVIERNES_4,4,3);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BVIERNES_5:
			BotonesdelCalendario(hWnd, IDC_BVIERNES_5,4,4);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BVIERNES_6:
			BotonesdelCalendario(hWnd, IDC_BVIERNES_6,4,5);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BSABADO_1:
			BotonesdelCalendario(hWnd, IDC_BSABADO_1,5,0);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BSABADO_2:
			BotonesdelCalendario(hWnd, IDC_BSABADO_2,5,1);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BSABADO_3:
			BotonesdelCalendario(hWnd, IDC_BSABADO_3,5,2);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BSABADO_4:
			BotonesdelCalendario(hWnd, IDC_BSABADO_4,5,3);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BSABADO_5:
			BotonesdelCalendario(hWnd, IDC_BSABADO_5,5,4);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BSABADO_6:
			BotonesdelCalendario(hWnd, IDC_BSABADO_6,5,5);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BDOMINGO_1:
			BotonesdelCalendario(hWnd, IDC_BDOMINGO_1,6,0);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BDOMINGO_2:
			BotonesdelCalendario(hWnd, IDC_BDOMINGO_2,6,1);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BDOMINGO_3:
			BotonesdelCalendario(hWnd, IDC_BDOMINGO_3,6,2);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BDOMINGO_4:
			BotonesdelCalendario(hWnd, IDC_BDOMINGO_4,6,3);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BDOMINGO_5:
			BotonesdelCalendario(hWnd, IDC_BDOMINGO_5,6,4);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		case IDC_BDOMINGO_6:
			BotonesdelCalendario(hWnd, IDC_BDOMINGO_6,6,5);
			EstablecerCalendario(hWnd, Parametro, Mes, UltimoDia, Ano, Dia, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
			return true;
		}
	}

	return true;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return true;
	case WM_DESTROY:
		return true;
	}
	return false;
}

BOOL CALLBACK Calendario2(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int LBNControl[8]; //0: Las horas  - 1 al 7: Los dias
	static int LBNUtilizado[8];
	DetectarAlarma(1, IDC_DT_CALENDARIO2, hWnd, false);
	char Horas[24][10] = { "00:00","01:00","02:00","03:00","04:00","05:00","06:00","07:00","08:00","09:00","10:00","11:00",
		"12:00","13:00","14:00","15:00","16:00","17:00","18:00","19:00","20:00","21:00","22:00","23:00" };
	static int TuDia = timeinfo.tm_mday;
	static int TuMes = timeinfo.tm_mon;
	static int TuAño = timeinfo.tm_year + 1900;
	static int Parametro; //1: Lunes - 2: Martes - 3: Miercoles - 4: Jueves - 5: Viernes - 6: Sabado -  7: Domingo
	static int TodosMeses[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };


	switch (msg)
	{
	case WM_INITDIALOG:
		for (int i = 0; i < 8; i++)
			LBNUtilizado[i] = LBNControl[i] = -1;
		if (timeinfo.tm_wday == 0)
		{
			Parametro = 7;
		}
		else
		{
			Parametro = timeinfo.tm_wday;
		}
		for (int i = 0; i < 24; i++)
			SendDlgItemMessage(hWnd, IDC_HORAS, LB_ADDSTRING, sizeof(Horas[i]), (LPARAM)Horas[i]);
		EstablecerCalendario2(hWnd,Parametro,TuMes,TuDia,TuAño);

		return true;
	case WM_TIMER:
	{
		if (BoolAlarma == false)
		{
			DetectarAlarma(2, IDC_DT_EVENTOS, hWnd, &BoolAlarma);

		}
	}
	return true;
	case WM_COMMAND:
	{
		switch (HIWORD(wParam))
		{
		case LBN_DBLCLK:
		{
			int MiniHora, MiniDia, MiniMes, MiniAño;
			int LBNMini[7] = { -1,-1,-1,-1,-1,-1,-1 };
			int IDList[7] = {IDC_LISTLUNES,IDC_LISTMARTES, IDC_LISTMIERCOLES, IDC_LISTJUEVES, IDC_LISTVIERNES, IDC_LISTSABADO, IDC_LISTDOMINGO };
			for (int q = 0; q < 7; q++)
				LBNMini[q] = SendDlgItemMessage(hWnd, IDList[q], LB_GETCURSEL, 0, 0);

			for (int n = 0; n < 7; n++)
			{
				if (LBNMini[n] >= 0)
				{
					int IDDias[7] = { IDC_DSLUNES,  IDC_DSMARTES ,IDC_DSMIERCOLES,IDC_DSJUEVES,IDC_DSVIERNES,  IDC_DSSABADO,IDC_DSDOMINGO };
					int IDMes[7] = { IDC_MSLUNES,  IDC_MSMARTES ,IDC_MSMIERCOLES,IDC_MSJUEVES,IDC_MSVIERNES,  IDC_MSSABADO,IDC_MSDOMINGO };
					int IDAño[7] = { IDC_ASLUNES,  IDC_ASMARTES ,IDC_ASMIERCOLES,IDC_ASJUEVES,IDC_ASVIERNES,  IDC_ASSABADO,IDC_ASDOMINGO };

					MiniHora = LBNMini[n];
					char v[30]="";
					SendDlgItemMessage(hWnd, IDDias[n], WM_GETTEXT,sizeof(v),(LPARAM)v);
					MiniDia = atoi(v);
					SendDlgItemMessage(hWnd, IDMes[n], WM_GETTEXT, sizeof(v), (LPARAM)v);
					char charminimes[12][30] = { "Enero","Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto","Septiembre","Octubre", "Noviembre", "Diciembre" };
					for (int r = 0; r < 12; r++)
					{
						if (strcmp(v, charminimes[r]) == 0)
						{
							MiniMes = r + 1;
							break;
						}
					}
					SendDlgItemMessage(hWnd, IDAño[n], WM_GETTEXT, sizeof(v), (LPARAM)v);
					MiniAño = atoi(v);

					BotonesdelCalendario2(hWnd, IDList[n],MiniHora, MiniDia, MiniMes, MiniAño);

					
					EstablecerCalendario2(hWnd, n+1, MiniMes-1, MiniDia, MiniAño);
					break;
				}

			}
		}
			return true;
		case LBN_SELCHANGE:
		{
			bool Seguir = true;
			int IDList[8] = {IDC_HORAS,IDC_LISTLUNES,IDC_LISTMARTES, IDC_LISTMIERCOLES, IDC_LISTJUEVES, IDC_LISTVIERNES, IDC_LISTSABADO, IDC_LISTDOMINGO};
			for (int q = 0; q < 8; q++)
				LBNControl[q] = SendDlgItemMessage(hWnd, IDList[q], LB_GETCURSEL, 0, 0);

			if (LBNControl[0] != LBNUtilizado[0])
			{
				for (int n = 1; n < 8; n++)
					SendDlgItemMessage(hWnd, IDList[n], LB_SETCURSEL, LBNControl[0], 0);

				for ( int j=0; j<8; j++)
					LBNUtilizado[j] = LBNControl[0];

				SendDlgItemMessage(hWnd, IDC_HORAS2, LB_SETCURSEL, LBNControl[0], 0);
				Seguir = false;
			}
			
			if (Seguir == true)
			{
				for (int u = 1; u < 8; u++)
				{
					if (LBNControl[u] != LBNUtilizado[u])
					{
						for (int r = 0; r < 8; r++)
						{
							if (r != u)
							{
								SendDlgItemMessage(hWnd, IDList[r], LB_SETCURSEL, -1, 0);
								LBNUtilizado[r] = LBNControl[r] = -1;
							}
							else
							{
								LBNUtilizado[r] = LBNControl[r];
								SendDlgItemMessage(hWnd, IDC_HORAS2, LB_SETCURSEL, LBNControl[r], 0);
							}
						}

						break;
					}

				}
			}

		}
			return true;

		}
		switch (LOWORD(wParam))
		{
		case IDC_ANTERIOR:
			for (int i = 0; i < 8; i++)
				LBNUtilizado[i] = LBNControl[i] = -1;
			for (int i = 0; i < 8; i++)
			{
				SemanalDiaDomingo--;
				if (SemanalDiaDomingo == 0)
				{
					SemanalMes--;
					if (SemanalMes == -1)
					{
						SemanalMes = 11;
						SemanalAño--;
					}
					SemanalDiaDomingo = TodosMeses[SemanalMes];
				}
			}
			EstablecerCalendario2(hWnd, 7, SemanalMes, SemanalDiaDomingo, SemanalAño);
			return true;
		case IDC_SIGUIENTE:
			for (int i = 0; i < 8; i++)
				LBNUtilizado[i] = LBNControl[i] = -1;
			EstablecerCalendario2(hWnd, 1, SemanalMes, SemanalDiaDomingo, SemanalAño);

			return true;
		case ID_OCALENDARIO:
		{
			EndDialog(hWnd, 0);
			DialogBox(hCalendario, MAKEINTRESOURCE(IDD_CALENDARIO), VP, Calendario);
		}
		return true;
		case ID_OEVENTO:
			MostrarAlarmaSeleccionada = false;
			SeleccioneDesdeCalendario = false;
			DialogBox(hEventos, MAKEINTRESOURCE(IDD_EVENTOS), hWnd, Eventos);
			return true;
		case ID_OSALIR:
			EndDialog(hWnd, 0);
			return true;
		}
	}
	return true;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return true;
	case WM_DESTROY:
		return true;
	}

	return false;
}

BOOL CALLBACK Eventos(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static char Tono[MAX_PATH] = "";
	static char TuTono[MAX_PATH] = "";
	static char comando[MAX_PATH] = "";
	static HWND Requisitos[11] = { 0,0,0,0,0,0,0,0,0,0,0 };//0=Titulo, 1=Contactos, 2=Descripcion, 3=Dia, 4=Mes, 5=Año, 6=Hora, 7=Minutos, 8=Ubicacion, 9=Tipo, 10=Tono
	DetectarAlarma(1, IDC_DT_EVENTOS, hWnd, false);
	static int intDias[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	static int intSeleccionarDia = timeinfo.tm_mon;
	static char charMeses[12][20] = { "Enero","Febrero","Marzo","Abril","Mayo","Junio","Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre" };
	static int intSeleccionarMes = 0;
	static int intAño = 0;
	static int ComboBox_Alarma = 0;
	static int ComboBox_Fecha_Mes = 0;
	static int ComboBox_Fecha_Año = 0;
	static int ComboBox_TuAlarma = 0;
	static int ComboBox_TuFecha_Mes = 0;
	static int ComboBox_TuFecha_Año = 0;


	switch (msg)
	{
	case WM_INITDIALOG:
	{
		if (MostrarAlarmaSeleccionada == true && APrin != NULL)
		{
			AAux = APrin;
			while (AAux != NULL)
			{
				if (strcmp(AAux->Titulo, EventoSeleccionado_Txt) == 0)
					break;
				else
					AAux = AAux->sig;
			}
		}


		intAño = timeinfo.tm_year + 1900;
		if (intAño % 4 == 0)
			intDias[1] = 29;
		else
			intDias[1] = 28;

		Requisitos[0] = GetDlgItem(hWnd, IDC_EVENTOS_TITULO);
		Requisitos[1] = GetDlgItem(hWnd, IDC_EVENTOS_CONTACTOS_OUT);
		Requisitos[2] = GetDlgItem(hWnd, IDC_EVENTOS_DESCRIPCION);
		Requisitos[3] = GetDlgItem(hWnd, IDC_EVENTOS_FECHA_DIA);
		Requisitos[4] = GetDlgItem(hWnd, IDC_EVENTOS_FECHA_MES);
		Requisitos[5] = GetDlgItem(hWnd, IDC_EVENTOS_FECHA_YEAR);
		Requisitos[6] = GetDlgItem(hWnd, IDC_EVENTOS_HORA_HORA);
		Requisitos[7] = GetDlgItem(hWnd, IDC_EVENTOS_HORA_MINUTOS);
		Requisitos[8] = GetDlgItem(hWnd, IDC_EVENTOS_UBICACION);
		Requisitos[9] = GetDlgItem(hWnd, IDC_EVENTOS_REPETICION);
		Requisitos[10] = GetDlgItem(hWnd, IDC_EVENTOS_ALARMAS_TONO);

		if (Prin != NULL)
		{
			Aux = Prin;

			while (Aux != NULL)
			{
				SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_IN, LB_ADDSTRING, sizeof(Aux->Nombre), (LPARAM)Aux->Nombre);
				Aux = Aux->sig;
			}
			Aux = NULL;
		}

		char Canciones[4][30] = { "Seleccionar alarma...","Alarma 1","Alarma 2","Alarma 3" };
		for (int i = 0; i < 4; i++)
			SendMessage(GetDlgItem(hWnd, IDC_EVENTOS_ALARMAS_TONO), CB_ADDSTRING, sizeof(Canciones[0]), (LPARAM)Canciones[i]);

		char TipoAlarm[4][20] = { "Una sola vez","Cada dia","Cada semana","Cada año" };
		for (int i = 0; i < 4; i++)
			SendMessage(Requisitos[9], CB_ADDSTRING, 0, (LPARAM)TipoAlarm[i]);

		if (MostrarAlarmaSeleccionada == false)
		{
			EnableWindow(GetDlgItem(hWnd, IDC_EVENTOS_AUTOUBICACION), false);

			SendMessage(GetDlgItem(hWnd, IDC_EVENTOS_ALARMAS_TONO), CB_SETCURSEL, 1, 0);
			ComboBox_TuAlarma = ComboBox_Alarma = 1;
			strcpy_s(TuTono, DireccionPrograma_Alarma1);

			SendMessage(Requisitos[9], CB_SETCURSEL, 0, 0);

			SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_OUT, LB_ADDSTRING, 0, (LPARAM)"Ninguno");

			for (int i = 1; i <= intDias[intSeleccionarDia]; i++)
			{
				char V[10] = "";
				_itoa_s(i, V, 10);
				if (!(V[1] >= 48 && V[1] <= 57))
				{
					V[1] = V[0];
					V[0] = '0';

					for (int i = 2; i < 10; i++)
					{
						V[i] = NULL;
					}
				}
				SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_ADDSTRING, 0, (LPARAM)V);
			}

			for (int i = 0; i < 12; i++)
			{
				SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_MES, CB_ADDSTRING, 0, (LPARAM)charMeses[i]);
			}
			for (int i = timeinfo.tm_year + 1900; i < timeinfo.tm_year + 50 + 1900; i++)
			{
				char V[10] = "";
				_itoa_s(i, V, 10);
				SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_YEAR, CB_ADDSTRING, 0, (LPARAM)V);
			}

			for (int i = 0; i < 60; i++)
			{
				char V[10] = "";
				_itoa_s(i, V, 10);
				if (!(V[1] >= 48 && V[1] <= 57))
				{
					V[1] = V[0];
					V[0] = '0';

					for (int i = 2; i < 10; i++)
					{
						V[i] = NULL;
					}
				}

				SendDlgItemMessage(hWnd, IDC_EVENTOS_HORA_MINUTOS, CB_ADDSTRING, 0, (LPARAM)V);
				if (i < 24)
				{
					SendDlgItemMessage(hWnd, IDC_EVENTOS_HORA_HORA, CB_ADDSTRING, 0, (LPARAM)V);
				}
			}

			if (SeleccioneDesdeCalendario == false)
			{
				SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_SETCURSEL, timeinfo.tm_mday - 1, 0);
				SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_MES, CB_SETCURSEL, timeinfo.tm_mon, 0);
				SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_YEAR, CB_SETCURSEL, 0, 0);
				SendDlgItemMessage(hWnd, IDC_EVENTOS_HORA_MINUTOS, CB_SETCURSEL, timeinfo.tm_min, 0);
				SendDlgItemMessage(hWnd, IDC_EVENTOS_HORA_HORA, CB_SETCURSEL, timeinfo.tm_hour, 0);
				ComboBox_Fecha_Mes = ComboBox_TuFecha_Mes = 0;
				ComboBox_Fecha_Año = ComboBox_TuFecha_Año = 0;
			}
			else
			{
					SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_SETCURSEL, DiaGlobalSeleccionado - 1, 0);
					SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_MES, CB_SETCURSEL, MesGlobalSeleccionado - 1, 0);
					ComboBox_Fecha_Mes = ComboBox_TuFecha_Mes = MesGlobalSeleccionado - 1;
					int Pos = 0;
					while (Pos < 50)
					{
						char V[40] = "";
						SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_YEAR, CB_GETLBTEXT, Pos, (LPARAM)V);
						int u = atoi(V);
						if (AñoGlobalSeleccionado == u)
							break;
						else
							Pos++;
					}
					if (Pos == 50)
						Pos--;
					SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_YEAR, CB_SETCURSEL, Pos, 0);
					ComboBox_Fecha_Año = ComboBox_TuFecha_Año = Pos;
					if (SeleccioneDesdeSemanal == false)
					{
						SendDlgItemMessage(hWnd, IDC_EVENTOS_HORA_MINUTOS, CB_SETCURSEL, 0, 0);
						SendDlgItemMessage(hWnd, IDC_EVENTOS_HORA_HORA, CB_SETCURSEL, 12, 0);
					}
					else
					{
						SendDlgItemMessage(hWnd, IDC_EVENTOS_HORA_MINUTOS, CB_SETCURSEL, 0, 0);
						SendDlgItemMessage(hWnd, IDC_EVENTOS_HORA_HORA, CB_SETCURSEL, HoraGlobalSeleccionado, 0);
					}

			}
			/*
			SliderControl = GetDlgItem(hWnd, IDC_ALARM_CONTROL);
			SendMessage(SliderControl, TBM_SETRANGE, (WPARAM)0, (LPARAM)MAKELONG(0, 100));
			SendMessage(SliderControl, TBM_SETPOS, (WPARAM)1, 46);
	*/
		}
		else
		{
			SendDlgItemMessage(hWnd, IDC_EVENTOS_CANCELAR, WM_SETTEXT, 0, (LPARAM)"Cerrar");
			SendDlgItemMessage(hWnd, IDC_EVENTOS_CREAR, WM_SETTEXT, 0, (LPARAM)"Guardar");
			SendDlgItemMessage(hWnd, IDC_EVENTOS_TITULO, WM_SETTEXT, sizeof(AAux->Titulo), (LPARAM)AAux->Titulo);
			SendDlgItemMessage(hWnd, IDC_EVENTOS_DESCRIPCION, WM_SETTEXT, sizeof(AAux->Descripcion), (LPARAM)AAux->Descripcion);
			SendDlgItemMessage(hWnd, IDC_EVENTOS_UBICACION, WM_SETTEXT, sizeof(AAux->Ubicacion), (LPARAM)AAux->Ubicacion);

			if (strcmp(AAux->Ubicacion, "Ninguno") == 0)
			{
				EnableWindow(GetDlgItem(hWnd, IDC_EVENTOS_AUTOUBICACION), false);
			}
			else
			{
				EnableWindow(GetDlgItem(hWnd, IDC_EVENTOS_AUTOUBICACION), true);
			}

			if (strcmp(AAux->Tono, DireccionPrograma_Alarma1) == 0)
			{
				SendMessage(GetDlgItem(hWnd, IDC_EVENTOS_ALARMAS_TONO), CB_SETCURSEL, 1, 0);
				ComboBox_TuAlarma = ComboBox_Alarma = 1;
			}
			else if (strcmp(AAux->Tono, DireccionPrograma_Alarma2) == 0)
			{
				SendMessage(GetDlgItem(hWnd, IDC_EVENTOS_ALARMAS_TONO), CB_SETCURSEL, 2, 0);
				ComboBox_TuAlarma = ComboBox_Alarma = 2;
			}
			else if (strcmp(AAux->Tono, DireccionPrograma_Alarma3) == 0)
			{
				SendMessage(GetDlgItem(hWnd, IDC_EVENTOS_ALARMAS_TONO), CB_SETCURSEL, 3, 0);
				ComboBox_TuAlarma = ComboBox_Alarma = 3;
			}
			else
			{
				char CCancion[MAX_PATH] = "";

				SendMessage(GetDlgItem(hWnd, IDC_EVENTOS_ALARMAS_TONO), CB_SETCURSEL, 4, 0);

				ComboBox_TuAlarma = ComboBox_Alarma = 4;
			}
			strcpy_s(TuTono, AAux->Tono);

			if (AAux->Tipo[0] =='1')
				SendMessage(Requisitos[9], CB_SETCURSEL, 0, 0);
			else if(AAux->Tipo[0] == '2')
				SendMessage(Requisitos[9], CB_SETCURSEL, 1, 0);
			else if (AAux->Tipo[0] == '3')
				SendMessage(Requisitos[9], CB_SETCURSEL, 2, 0);
			else if (AAux->Tipo[0] == '4')
				SendMessage(Requisitos[9], CB_SETCURSEL, 3, 0);

			SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_OUT, LB_ADDSTRING, 0, (LPARAM)AAux->NombreDelContacto);

			int intauxdia = atoi(AAux->Fecha.Dia);
			int intauxmes = atoi(AAux->Fecha.Mes);
			int intauxaño = atoi(AAux->Fecha.Año);
			int intauxhora = atoi(AAux->Fecha.Hora);
			int intauxmin = atoi(AAux->Fecha.Minuto);

			for (int i = 1; i <= intDias[intauxmes-1]; i++)
			{
				char V[10] = "";
				_itoa_s(i, V, 10);
				if (!(V[1] >= 48 && V[1] <= 57))
				{
					V[1] = V[0];
					V[0] = '0';

					for (int i = 2; i < 10; i++)
					{
						V[i] = NULL;
					}
				}
				SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_ADDSTRING, 0, (LPARAM)V);
			}

			for (int i = 0; i < 12; i++)
			{
				SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_MES, CB_ADDSTRING, 0, (LPARAM)charMeses[i]);
			}

			for (int i = timeinfo.tm_year + 1900; i < timeinfo.tm_year + 50 + 1900; i++)
			{
				char V[10] = "";
				_itoa_s(i, V, 10);
				SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_YEAR, CB_ADDSTRING, 0, (LPARAM)V);
			}

			for (int i = 0; i < 60; i++)
			{
				char V[10] = "";
				_itoa_s(i, V, 10);
				if (!(V[1] >= 48 && V[1] <= 57))
				{
					V[1] = V[0];
					V[0] = '0';

					for (int i = 2; i < 10; i++)
					{
						V[i] = NULL;
					}
				}

				SendDlgItemMessage(hWnd, IDC_EVENTOS_HORA_MINUTOS, CB_ADDSTRING, 0, (LPARAM)V);
				if (i < 24)
				{
					SendDlgItemMessage(hWnd, IDC_EVENTOS_HORA_HORA, CB_ADDSTRING, 0, (LPARAM)V);
				}
			}

			SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_SETCURSEL, intauxdia - 1, 0);
			SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_MES, CB_SETCURSEL, intauxmes -1, 0);
			
			int Añopos = intauxaño - (timeinfo.tm_year + 1900);
			if (Añopos < 0)
			{
				SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_YEAR, CB_RESETCONTENT, 0, 0);
				char V[10] = "";
				_itoa_s(Añopos + (timeinfo.tm_year + 1900), V, 10);
				SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_YEAR, CB_ADDSTRING, 0, (LPARAM)V);
				for (int i = timeinfo.tm_year + 1900; i < timeinfo.tm_year + 50 + 1900; i++)
				{
					_itoa_s(i, V, 10);
					SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_YEAR, CB_ADDSTRING, 0, (LPARAM)V);
				}
				Añopos = 0;
			}

			SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_YEAR, CB_SETCURSEL, Añopos, 0);
			SendDlgItemMessage(hWnd, IDC_EVENTOS_HORA_MINUTOS, CB_SETCURSEL, intauxmin, 0);
			SendDlgItemMessage(hWnd, IDC_EVENTOS_HORA_HORA, CB_SETCURSEL, intauxhora, 0);
			ComboBox_Fecha_Mes = ComboBox_TuFecha_Mes = intauxmes-1;
			ComboBox_Fecha_Año = ComboBox_TuFecha_Año = Añopos;

		}
	}
	return true;
	case WM_TIMER:
	{
		if (BoolAlarma == false)
		{
			DetectarAlarma(2, IDC_DT_EVENTOS, hWnd, &BoolAlarma);

		}
	}
	return true;
	case WM_COMMAND:
	{
		switch (HIWORD(wParam))
		{
		case CBN_SELCHANGE:
		{
			ComboBox_TuAlarma = SendDlgItemMessage(hWnd, IDC_EVENTOS_ALARMAS_TONO, CB_GETCURSEL, 0, 0);
			ComboBox_TuFecha_Año = SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_YEAR, CB_GETCURSEL, 0, 0);
			ComboBox_TuFecha_Mes = SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_MES, CB_GETCURSEL, 0, 0);

			if (ComboBox_TuFecha_Mes != ComboBox_Fecha_Mes)
			{
				int TintDia = SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_GETCURSEL, 0, 0);

				SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_RESETCONTENT, 0, 0);
				for (int i = 1; i <= intDias[ComboBox_TuFecha_Mes]; i++)
				{
					char V[4] = "";
					_itoa_s(i, V, 10);
					SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_ADDSTRING, 0, (LPARAM)V);
				}

				if (ComboBox_TuFecha_Mes == 1 && TintDia > intDias[1] - 1)
				{
					SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_SETCURSEL, intDias[1]-1, 0);
				}
				else
				{
					if (TintDia == 30 && (ComboBox_TuFecha_Mes == 3 || ComboBox_TuFecha_Mes == 5 || ComboBox_TuFecha_Mes == 8 || ComboBox_TuFecha_Mes == 10))
						SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_SETCURSEL, TintDia - 1, 0);
					else
						SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_SETCURSEL, TintDia, 0);
				}

				ComboBox_Fecha_Mes = ComboBox_TuFecha_Mes;
			}
			if (ComboBox_TuFecha_Año != ComboBox_Fecha_Año)
			{
				char TcharAño[6] = "";
				int TintAño = 0;
				SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_YEAR, CB_GETLBTEXT, ComboBox_TuFecha_Año, (LPARAM)TcharAño);
				TintAño = atoi(TcharAño);
				if (TintAño % 4 == 0)
					intDias[1] = 29;
				else
					intDias[1] = 28;

				int TintDia = SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_GETCURSEL, 0, 0);

				SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_RESETCONTENT, 0, 0);
				for (int i = 1; i <= intDias[ComboBox_TuFecha_Mes]; i++)
				{
					char V[4] = "";
					_itoa_s(i, V, 10);
					SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_ADDSTRING, 0, (LPARAM)V);
				}

				if (ComboBox_TuFecha_Mes == 1 && TintDia > intDias[1]-1)
				{
					SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_SETCURSEL, intDias[1] - 1, 0);
				}
				else
				{
					if (TintDia == 30 && (ComboBox_TuFecha_Mes == 3 || ComboBox_TuFecha_Mes == 5 || ComboBox_TuFecha_Mes == 8 || ComboBox_TuFecha_Mes == 10))
						SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_SETCURSEL, TintDia - 1, 0);
					else
						SendDlgItemMessage(hWnd, IDC_EVENTOS_FECHA_DIA, CB_SETCURSEL, TintDia, 0);
				}

				ComboBox_Fecha_Año = ComboBox_TuFecha_Año;
			}


			if (ComboBox_TuAlarma != ComboBox_Alarma)
			{
				mciSendString("stop MP3", NULL, 0, 0);
				mciSendString("close MP3", NULL, 0, 0);
				int IndexAlarma = SendDlgItemMessage(hWnd, IDC_EVENTOS_ALARMAS_TONO, CB_GETCURSEL, 0, 0);
				ComboBox_TuAlarma = ComboBox_Alarma = IndexAlarma;
				if (IndexAlarma == 0)
				{
					mciSendString("stop MP3", NULL, 0, 0);
					mciSendString("close MP3", NULL, 0, 0);
					SeleccionarArchivo(hWnd, 2, Tono);
					if (strcmp(Tono, "") != 0)
					{
						char Cancion[MAX_PATH] = "";

						int Derecha = 0;
						int Izquierda = 0;
						while (1)
						{
							if (Tono[Derecha] == '.' && Tono[Derecha + 1] == 'm' && Tono[Derecha + 2] == 'p' && Tono[Derecha + 3] == '3')
								break;
							else
								Derecha++;
						}
						Izquierda = Derecha;
						while (1)
						{
							if (Tono[Izquierda] == '\\')
								break;
							else
								Izquierda--;
						}
						Izquierda++;
						int inicio = 0;
						for (Izquierda; Izquierda < Derecha; Izquierda++)
						{
							Cancion[inicio] = Tono[Izquierda];
							inicio++;
						}
						SendDlgItemMessage(hWnd, IDC_EVENTOS_ALARMAS_TONO, CB_DELETESTRING, 4, 0);
						SendMessage(GetDlgItem(hWnd, IDC_EVENTOS_ALARMAS_TONO), CB_ADDSTRING, sizeof(Cancion), (LPARAM)Cancion);
						SendDlgItemMessage(hWnd, IDC_EVENTOS_ALARMAS_TONO, CB_SETCURSEL, 4, 0);
						ComboBox_TuAlarma = ComboBox_Alarma = 4;
						mciSendString("stop MP3", NULL, 0, 0);
						mciSendString("close MP3", NULL, 0, 0);
						strcpy_s(TuTono, Tono);
					}
					else
					{
						SendDlgItemMessage(hWnd, IDC_EVENTOS_ALARMAS_TONO, CB_SETCURSEL, 1, 0);
						ComboBox_TuAlarma = ComboBox_Alarma = 1;
					}
				}
				else if (IndexAlarma == 1)
					strcpy_s(TuTono, DireccionPrograma_Alarma1);
				else if (IndexAlarma == 2)
					strcpy_s(TuTono, DireccionPrograma_Alarma2);
				else if (IndexAlarma == 3)
					strcpy_s(TuTono, DireccionPrograma_Alarma3);
				else if (IndexAlarma == 4)
					strcpy_s(TuTono, Tono);
			}
		}
			return true;
		}
		switch (LOWORD(wParam))
		{
		case IDC_EVENTOS_BUSCAR_RESET:
		{
			SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_IN, LB_RESETCONTENT, 0, 0);

			if (Prin != NULL)
			{
				Aux = Prin;
				while (Aux != NULL)
				{

					SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_IN, LB_ADDSTRING, sizeof(Aux->Nombre), (LPARAM)Aux->Nombre);
			
					Aux = Aux->sig;
				}
			}
			SendDlgItemMessage(hWnd, IDC_EVENTOS_BUSCAR, WM_SETTEXT, 0, (LPARAM)"");
		}
			return true;
		case IDC_EVENTOS_BUSCAR_BUSCAR:
		{
			if (Prin != NULL)
			{
				Aux = Prin;
				char VBuscar[40] = "";
				int Contador = 0;
				SendDlgItemMessage(hWnd, IDC_EVENTOS_BUSCAR, WM_GETTEXT, sizeof(VBuscar), (LPARAM)VBuscar);

				int Aproximado = 0;
				while (Aproximado != 40)
				{
					if (VBuscar[Aproximado] == NULL)
					{
						break;
					}
					else
					{
						Aproximado++;
					}

				}
				SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_IN, LB_RESETCONTENT, 0, 0);
				while (Aux != NULL)
				{
					for (int j = 0; j < 40; j++)
					{

						if (Aux->Nombre[j] == VBuscar[0] || Aux->Nombre[j] == VBuscar[0] + 32 || Aux->Nombre[j] == VBuscar[0] - 32)
						{
							bool EsAprox = true;
							int p = 0;
							for (int i = j; i < j + Aproximado; i++)
							{

								if (Aux->Nombre[i] != VBuscar[p] && Aux->Nombre[i] != VBuscar[p] + 32 && Aux->Nombre[i] != VBuscar[p] - 32)
								{
									EsAprox = false;
								}
								else
								{
									p++;
								}
							}
							if (EsAprox == true)
							{
								SendDlgItemMessage(hWnd,IDC_EVENTOS_CONTACTOS_IN, LB_ADDSTRING, 0, (LPARAM)Aux->Nombre);
			
								Contador++;
								break;
							}
						}
					}
					Aux = Aux->sig;
				}
				if (Contador == 0)
					MessageBox(hWnd, "No existe el dato a buscar", "Advertencia", MB_OK | MB_ICONINFORMATION);
			}
		}
			return true;
		case IDC_EVENTOS_ALARMAS_PLAY:
		{
			if (strcmp(TuTono, "") != 0)
			{
				sprintf_s(comando, "open \"%s\" alias MP3", TuTono);
				mciSendString(comando, NULL, 0, 0);
				mciSendString("play MP3", NULL, 0, 0);
			}
		}
		return true;
		case IDC_EVENTOS_ALARMAS_PAUSE:
			mciSendString("pause MP3", NULL, 0, 0);
			return true;
		case IDC_EVENTOS_ALARMAS_STOP:
			mciSendString("stop MP3", NULL, 0, 0);
			mciSendString("close MP3", NULL, 0, 0);
			return true;
		case IDC_EVENTOS_CANCELAR:
			mciSendString("stop MP3", NULL, 0, 0);
			mciSendString("close MP3", NULL, 0, 0);
			EndDialog(hWnd, 0);
			return true;
		case IDC_EVENTOS_ADD:
		{
			int Indice = SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_IN,LB_GETCURSEL,0,0);
			if (Indice >= 0)
			{
				char NContacto[40] = "";
				SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_OUT, LB_GETTEXT, 0, (LPARAM)NContacto);

				if (strcmp(NContacto, "Ninguno") == 0)
				{
					SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_OUT, LB_DELETESTRING, 0, 0);
				}
				else
				{
					SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_IN, LB_ADDSTRING, sizeof(NContacto), (LPARAM)NContacto);
					SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_OUT, LB_DELETESTRING, 0, 0);
				}
				char Contacto[40] = "";
				Indice = SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_IN, LB_GETCURSEL, 0, 0);
				SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_IN, LB_GETTEXT, Indice, (LPARAM)Contacto);
				SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_IN, LB_DELETESTRING, Indice, 0);
				SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_OUT, LB_ADDSTRING, sizeof(Contacto), (LPARAM)Contacto);
				EnableWindow(GetDlgItem(hWnd, IDC_EVENTOS_AUTOUBICACION), true);
				SendDlgItemMessage(hWnd, IDC_EVENTOS_UBICACION, WM_SETTEXT, 0, (LPARAM)"");
			}
		}
			return true;
		case IDC_EVENTOS_AUTOUBICACION:
		{
			char Contacto[40] = "";
			SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_OUT, LB_GETTEXT, 0, (LPARAM)Contacto);

			Aux = Prin;
			while (Aux != NULL)
			{
				if (strcmp(Contacto,Aux->Nombre)==0)
				{
					char T[MAX_PATH] = "";
					strcat_s(T, Aux->Direccion.Calle);
					strcat_s(T, ", ");
					strcat_s(T, Aux->Direccion.Num);
					strcat_s(T, ", ");
					strcat_s(T, Aux->Direccion.Colonia);
					strcat_s(T, ", ");
					strcat_s(T, Aux->Direccion.Municipio);
					strcat_s(T, ", ");
					strcat_s(T, Aux->Direccion.Estado);

					SendDlgItemMessage(hWnd, IDC_EVENTOS_UBICACION, WM_SETTEXT, sizeof(T), (LPARAM)T);
					break;
				}
				else
					Aux = Aux->sig;
			}


		}
			return true;
		case IDC_EVENTOS_DELETE:
		{

				char Contacto[40] = "";
				SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_OUT, LB_GETTEXT, 0, (LPARAM)Contacto);

				if (strcmp(Contacto, "Ninguno") != 0)
				{
					EnableWindow(GetDlgItem(hWnd, IDC_EVENTOS_AUTOUBICACION), false);
					SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_OUT, LB_DELETESTRING, 0, 0);
					SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_IN, LB_ADDSTRING, sizeof(Contacto), (LPARAM)Contacto);
					SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_OUT, LB_ADDSTRING, 0, (LPARAM)"Ninguno");
					SendDlgItemMessage(hWnd, IDC_EVENTOS_UBICACION, WM_SETTEXT, 0, (LPARAM)"");
				}
		}
			return true;
		case IDC_EVENTOS_CREAR:
		{
			Alarmas	DatosTemporales;
			char Temporal[40];
			char TemporalMaximo[MAX_PATH];
			bool Continuar = true;
			bool TituloDoble = false;


			for (int Empezar = 0; Empezar < 11; Empezar++)
			{
				if (Empezar == 0 || Empezar == 2 || Empezar == 8) //Verificar si son Edit Control
				{
					int ind = 0;
					int Limite_ind;
					if (Empezar == 2 || Empezar == 8)
					{
						Limite_ind = MAX_PATH;
					}
					else
					{
						Limite_ind = 40;
					}
					if (Empezar == 2 || Empezar==8)
						SendMessage(Requisitos[Empezar], WM_GETTEXT, sizeof(TemporalMaximo), (LPARAM)TemporalMaximo);
					else
						SendMessage(Requisitos[Empezar], WM_GETTEXT, sizeof(Temporal), (LPARAM)Temporal);

					do {
						if (Empezar == 2 || Empezar == 8)
						{
							if (TemporalMaximo[ind] == NULL || ind == Limite_ind + 1)//Verificar si el caracter es Nulo ó Si el indice supera a la cantidad de caracteres del arreglo char
							{
								
								ind = Limite_ind + 2;
							if (Empezar == 2)
							{
								strcpy_s(DatosTemporales.Descripcion, "Sin descripcion");
							}
							else
							{
								strcpy_s(DatosTemporales.Ubicacion, "Sin Ubicacion");
							}
							}
							else if (TemporalMaximo[ind] == 32) //Verificar si el caracter es un "espacio"
							{
								ind++;
							}
							else //Hay caracteres legibles para insertarlos en otra variable  (En caso de que hay espacios antes del caracter)
							{
								for (int i = 0; i < Limite_ind; i++)
								{
									if (Empezar == 2)
									{
										if (ind < Limite_ind)
										{
											DatosTemporales.Descripcion[i] = TemporalMaximo[ind];
											ind++;
										}
										else
										{
											DatosTemporales.Descripcion[i] = NULL;
										}
									}
									else
									{
										if (ind < Limite_ind)
										{
											DatosTemporales.Ubicacion[i] = TemporalMaximo[ind];
											ind++;
										}
										else
										{
											DatosTemporales.Ubicacion[i] = NULL;
										}
									}
								}
								ind = Limite_ind + 2;
							}
						}
						else
						{
							if (Temporal[ind] == NULL || ind == Limite_ind + 1)//Verificar si el caracter es Nulo ó Si el indice supera a la cantidad de caracteres del arreglo char
							{
								ind = Limite_ind + 2;
								if (Empezar == 0)
								{
									int NumEvento = 0;
									char Convertir[40] = "";
									bool ReSeguir = true;
									char Compilartxt[40] = "";

									_itoa_s(NumEvento, Convertir, 10);
									strcpy_s(Compilartxt, "");
									strcat_s(Compilartxt, "Evento ");
									strcat_s(Compilartxt, Convertir);

									do {
										ReSeguir = false;
										if (APrin != NULL)
										{
											Alarmas *Util;
											Util = APrin;
											while (Util != NULL)
											{
												if (strcmp(Compilartxt, Util->Titulo) == 0)
												{
													ReSeguir = true;
													NumEvento++;
													_itoa_s(NumEvento, Convertir, 10);
													strcpy_s(Compilartxt, "");
													strcat_s(Compilartxt, "Evento ");
													strcat_s(Compilartxt, Convertir);
													break;
												}
												else
												{
													Util = Util->sig;
												}
											}
										}
									} while (ReSeguir == true);
									strcpy_s(DatosTemporales.Titulo, Compilartxt);
								}
							}
							else if (Temporal[ind] == 32) //Verificar si el caracter es un "espacio"
							{
								ind++;
							}
							else //Hay caracteres legibles para insertarlos en otra variable  (En caso de que hay espacios antes del caracter)
							{
								for (int i = 0; i < Limite_ind; i++)
								{
									if (ind < Limite_ind)
									{
										DatosTemporales.Titulo[i] = Temporal[ind];
										ind++;
									}
									else
									{
										DatosTemporales.Titulo[i] = NULL;
									}
								}
								char NombreAnterior[40] = "";
								if (MostrarAlarmaSeleccionada == true)
									strcpy_s(NombreAnterior,EventoSeleccionado_Txt);


								if (APrin != NULL)
								{
									AAux = APrin;
									while (AAux != NULL)
									{
										if (strcmp(AAux->Titulo, DatosTemporales.Titulo) == 0)
										{
											if (!(MostrarAlarmaSeleccionada == true && strcmp(DatosTemporales.Titulo, NombreAnterior) == 0))
											{
												TituloDoble = true;
												break;
											}
											else
												AAux = AAux->sig;
										}
										{
											AAux = AAux->sig;
										}
									}
								}
								ind = Limite_ind + 2;
							}
						}
					} while (ind >= 0 && ind <= Limite_ind);
				}
				else if ((Empezar >= 3 && Empezar <= 7) || Empezar == 9 || Empezar == 10) //ComboBox
				{
					int Pos = SendMessage(Requisitos[Empezar], CB_GETCURSEL, 0, 0);
					if (Empezar == 3)
					{
						Pos++;
						_itoa_s(Pos, DatosTemporales.Fecha.Dia, 10);
					}
					else if (Empezar == 4)
					{
						Pos++;
						_itoa_s(Pos, DatosTemporales.Fecha.Mes, 10);
					}
					else if (Empezar == 5)
						SendMessage(Requisitos[Empezar], CB_GETLBTEXT, Pos, (LPARAM)DatosTemporales.Fecha.Año);
					else if (Empezar == 6)
						_itoa_s(Pos, DatosTemporales.Fecha.Hora, 10);
					else if (Empezar == 7)
						_itoa_s(Pos, DatosTemporales.Fecha.Minuto, 10);
					else if (Empezar == 9)
					{
						if (Pos == 0)
							strcpy_s(DatosTemporales.Tipo, "1");
						else if (Pos == 1)
							strcpy_s(DatosTemporales.Tipo, "2");
						else if (Pos == 2)
							strcpy_s(DatosTemporales.Tipo, "3");
						else
							strcpy_s(DatosTemporales.Tipo, "4");
					}
					else
					{
						strcpy_s(DatosTemporales.Tono, TuTono);
					}
				}
				else //ListBox
				{
					char CCont[40] = "";
					SendDlgItemMessage(hWnd, IDC_EVENTOS_CONTACTOS_OUT, LB_GETTEXT, 0, (LPARAM)CCont);
					strcpy_s(DatosTemporales.NombreDelContacto, CCont);

				}
			}//Fin del For
			if (Continuar == true)
			{
				if (TituloDoble == false)
				{
					time(&rawtime);
					localtime_s(&timeinfo, &rawtime);
					int Primero[5]; //Año -- Mes -- Dia -- Hora -- Min
					int Segundo[5]; //Año -- Mes -- Dia -- Hora -- Min
					Continuar = false;

					Primero[0] = atoi(DatosTemporales.Fecha.Año);
					Segundo[0] = timeinfo.tm_year + 1900;

					Primero[1] = atoi(DatosTemporales.Fecha.Mes);
					Primero[1]--;
					Segundo[1] = timeinfo.tm_mon;

					Primero[2] = atoi(DatosTemporales.Fecha.Dia);
					Segundo[2] = timeinfo.tm_mday;

					Primero[3] = atoi(DatosTemporales.Fecha.Hora);
					Segundo[3] = timeinfo.tm_hour;

					Primero[4] = atoi(DatosTemporales.Fecha.Minuto);
					Segundo[4] = timeinfo.tm_min;

					{
						if (Primero[0] > Segundo[0])
						{
							Continuar = true;
						}
						else if (Primero[0] < Segundo[0])
						{
							Continuar = false;
						}
						else
						{
							if (Primero[1] > Segundo[1])
							{
								Continuar = true;
							}
							else if (Primero[1] < Segundo[1])
							{
								Continuar = false;
							}
							else
							{
								if (Primero[2] > Segundo[2])
								{
									Continuar = true;
								}
								else if (Primero[2] < Segundo[2])
								{
									Continuar = false;
								}
								else
								{
									if (Primero[3] > Segundo[3])
									{
										Continuar = true;
									}
									else if (Primero[3] < Segundo[3])
									{
										Continuar = false;
									}
									else
									{
										if (Primero[4] > Segundo[4])
										{
											Continuar = true;
										}
										else if (Primero[4] < Segundo[4])
										{
											Continuar = false;
										}
									}
								}
							}
						}
					}
				}
				if (Continuar == true)
				{
					if (TituloDoble == false)
					{
						mciSendString("stop MP3", NULL, 0, 0);
						mciSendString("close MP3", NULL, 0, 0);
						if (MostrarAlarmaSeleccionada == true)
						{
							if (APrin != NULL)
							{
								bool nel = true;
								AAux = APrin;
								while (AAux != NULL && nel == true)
								{
									if (strcmp(AAux->Titulo, EventoSeleccionado_Txt) == 0)
										nel = false;
									else
										AAux = AAux->sig;
								}
							}

							ModificarEvento(DatosTemporales, AAux);
						}
						else
							AgregarEvento(DatosTemporales);
						OrdenarAlarmas();
						AlarmaGuardar();
						MessageBox(hWnd, "Se ha guardado el evento correctamente", "Aviso", MB_OK | MB_ICONINFORMATION);
						EndDialog(hWnd, 0);
					}
					else
					{
						MessageBox(hWnd, "El titulo del evento ya existe. \nPor favor, ingrese otro titulo", "Aviso", MB_OK | MB_ICONINFORMATION);
					}
				}
				else
				{
					MessageBox(hWnd, "La fecha que intenta programar ya expiro. \nPor favor, modifique la fecha", "Aviso", MB_OK | MB_ICONINFORMATION);
				}
			}
			else
			{
				mciSendString("stop MP3", NULL, 0, 0);
				mciSendString("close MP3", NULL, 0, 0);
				MessageBox(hWnd, "Por favor, complete los campos obligatorios", "Aviso", MB_OK | MB_ICONINFORMATION);
			}
		}
		return true;
		}

	}
	return true;
	case WM_CLOSE:
		mciSendString("stop MP3", NULL, 0, 0);
		mciSendString("close MP3", NULL, 0, 0);
		EndDialog(hWnd, 0);
		return true;
	case WM_DESTROY:
		return true;
	}


	return false;
}

BOOL CALLBACK CallAlarma(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DetectarAlarma(1, IDC_DT_ALARMA, hWnd, false);
	static HBITMAP bmp1, bmp2;
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		bmp1 = (HBITMAP)SendDlgItemMessage(hWnd, IDC_PIC_ALARMA, STM_GETIMAGE, IMAGE_BITMAP, 0);
		//Al objeto bmp2, se le asigna una imagen local:
		bmp2 = (HBITMAP)LoadImage(NULL, DireccionPrograma_Despertador1, IMAGE_BITMAP, 125, 125, LR_LOADFROMFILE);
		SendDlgItemMessage(
			hWnd,
			IDC_PIC_ALARMA,
			STM_SETIMAGE,
			IMAGE_BITMAP,
			(LPARAM)bmp2);

		if (NoDectetaMP3 == false)
		{
			ShowWindow(GetDlgItem(hWnd, IDC_ALARMA_ERROR), SW_HIDE);
		}
		if (ASonando->Tipo[0] != '1')
		{
			ShowWindow(GetDlgItem(hWnd, IDC_ALARMA_ELIMINAR), SW_HIDE);
		}

		if (ExpiroAlarma == false)
		{
			ShowWindow(GetDlgItem(hWnd, IDC_ALARMA_VIEJO), SW_HIDE);
		}

		char Temp[MAX_PATH] = "";
		char Fecha[10] = "";
		int intFecha = 0;
		char charMeses[12][20] = { "Enero","Febrero","Marzo","Abril","Mayo","Junio","Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre" };

		strcpy_s(Temp, "Alarma de: ");
		strcpy_s(Fecha, AAux->Fecha.Dia);
		strcat_s(Temp, Fecha);
		strcat_s(Temp, " de ");
		strcpy_s(Fecha, AAux->Fecha.Mes);
		intFecha = atoi(Fecha);
		strcat_s(Temp, charMeses[intFecha-1]);
		strcat_s(Temp, " del ");
		strcpy_s(Fecha, AAux->Fecha.Año);
		strcat_s(Temp, Fecha);
		strcat_s(Temp, " || ");
		strcpy_s(Fecha, AAux->Fecha.Hora);
		strcat_s(Temp, Fecha);
		strcat_s(Temp, ":");
		strcpy_s(Fecha, AAux->Fecha.Minuto);
		strcat_s(Temp, Fecha);
		SendDlgItemMessage(hWnd, IDC_ALARMA_TIEMPO, WM_SETTEXT, sizeof(Temp), (LPARAM)Temp);

		strcpy_s(Temp, "");
		strcpy_s(Temp, AAux->Titulo);
		SendDlgItemMessage(hWnd, IDC_ALARMA_TITULO, WM_SETTEXT, sizeof(Temp), (LPARAM)Temp);

		strcpy_s(Temp, "");
		strcpy_s(Temp, AAux->Descripcion);
		SendDlgItemMessage(hWnd, IDC_ALARMA_DESCRIPCION, WM_SETTEXT, sizeof(Temp), (LPARAM)Temp);

		strcpy_s(Temp, "");
		strcpy_s(Temp, AAux->Ubicacion);
		SendDlgItemMessage(hWnd, IDC_ALARMA_UBICACION, WM_SETTEXT, sizeof(Temp), (LPARAM)Temp);

		strcpy_s(Temp, "");
		strcpy_s(Temp, AAux->NombreDelContacto);
		SendDlgItemMessage(hWnd, IDC_ALARMA_CONTACTOS, WM_SETTEXT, sizeof(Temp), (LPARAM)Temp);

		strcpy_s(Temp, "");
		strcpy_s(Temp, AAux->Tipo);


		if (Temp[0] == '1')
			SendDlgItemMessage(hWnd, IDC_ALARMA_REPETIR, WM_SETTEXT, 0, (LPARAM)"Una sola vez");
		else if (Temp[0] == '2')
			SendDlgItemMessage(hWnd, IDC_ALARMA_REPETIR, WM_SETTEXT, 0, (LPARAM)"Cada dia");
		else if (Temp[0] == '3')
			SendDlgItemMessage(hWnd, IDC_ALARMA_REPETIR, WM_SETTEXT, 0, (LPARAM)"Cada semana");
		else if (Temp[0] == '4')
			SendDlgItemMessage(hWnd, IDC_ALARMA_REPETIR, WM_SETTEXT, 0, (LPARAM)"Cada año");
	}
		return true;
	case WM_TIMER:
	{
		if (BoolAlarma == false)
		{
			DetectarAlarma(2, IDC_DT_EVENTOS, hWnd, &BoolAlarma);
		}
	}
	return true;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_ALARMA_ELIMINAR:
			ASonando2 = ASonando->sig;
			EliminarAlarma(ASonando);
			AAux2 = ASonando2;
			ASonando = ASonando2 = NULL;
			AlarmaGuardar();
			EndDialog(hWnd, 0);
			return true;
		case IDC_ALARMA_CERRAR:
			if (ASonando->Tipo[0] == '1')
			{
				if (MessageBox(hWnd, "La alarma no se eliminara. \nPodras eliminarlo en \"Mostrar todos los eventos\"\n¿Deseas continuar?", "Aviso", MB_OKCANCEL | MB_ICONINFORMATION) == IDOK)
				{
					AAux = ASonando;
					AAux2 = ASonando->sig;
					ASonando = NULL;
					EndDialog(hWnd, 0);
				}
			}
			else
			{
				if (ASonando->Tipo[0] == '2')
				{
					int intDias[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
					int Dia = atoi(ASonando->Fecha.Dia);
					int Mes = atoi(ASonando->Fecha.Mes);
					int Año = atoi(ASonando->Fecha.Año);
					if (Año % 4 == 0)
					{
						intDias[1] = 29;
					}
					else
						intDias[1] = 28;
					Dia++;
					Mes--;
					if (Dia > intDias[Mes])
					{
						Dia = Dia - intDias[Mes];
						Mes++;
						if (Mes == 12)
						{
							Mes = 0;
							Año++;
						}
					}
					Mes++;
					_itoa_s(Año, ASonando->Fecha.Año, 10);
					_itoa_s(Mes, ASonando->Fecha.Mes, 10);
					_itoa_s(Dia, ASonando->Fecha.Dia, 10);
				}
				else if (ASonando->Tipo[0] == '3')
				{
					int intDias[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
					int Dia = atoi(ASonando->Fecha.Dia);
					int Mes = atoi(ASonando->Fecha.Mes);
					int Año = atoi(ASonando->Fecha.Año);
					if (Año % 4 == 0)
					{
						intDias[1] = 29;
					}
					else
						intDias[1] = 28;
					Dia += 7;
					Mes--;
					if (Dia > intDias[Mes])
					{
						Dia = Dia - intDias[Mes];
						Mes++;
						if (Mes == 12)
						{
							Mes = 0;
							Año++;
						}
					}
					Mes++;
					_itoa_s(Año, ASonando->Fecha.Año, 10);
					_itoa_s(Mes, ASonando->Fecha.Mes, 10);
					_itoa_s(Dia, ASonando->Fecha.Dia, 10);
				}
				else if (ASonando->Tipo[0] == '4')
				{
					int Año = atoi(ASonando->Fecha.Año);
					Año++;
					_itoa_s(Año, ASonando->Fecha.Año, 10);
				}
				Alarmas Mover;
				Mover = *ASonando;
				Mover.sig = Mover.ant = NULL;
				EliminarAlarma(ASonando);
				AgregarEvento(Mover);
				OrdenarAlarmas();

				AlarmaGuardar();
				AAux = ASonando;
				AAux2 = ASonando->sig;
				ASonando = NULL;
				EndDialog(hWnd, 0);
			}

			return true;
		}
		return true;
	}
	/*
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return true;
		*/
	case WM_DESTROY:
		return true;
	}


	return false;
}

BOOL CALLBACK TodoslosEventos(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int EventoSeleccionado_Num = 0;

	DetectarAlarma(1, IDC_DT_TEVENTOS, hWnd, false);
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		ReCargarTodosEventos(hWnd);
		if (SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA, LB_GETCOUNT, 0, 0) == 0)
			EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_DELETEALL), false);
		else
			EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_DELETEALL), true);
	}
		return true;
	case WM_TIMER:
	{
		if (BoolAlarma == false)
		{
			DetectarAlarma(2, IDC_DT_EVENTOS, hWnd, &BoolAlarma);
		}
	}
	return true;
	case WM_COMMAND:
	{
		switch (HIWORD(wParam))
		{
		case LBN_DBLCLK:
		{
			int EventoSeleccionado_Num = SendMessage(GetDlgItem(hWnd, IDC_TEVENTOS_LISTA), LB_GETCURSEL, 0, 0);
			if (EventoSeleccionado_Num >= 0)
			{
				SendMessage(GetDlgItem(hWnd, IDC_TEVENTOS_LISTA), LB_GETTEXT, EventoSeleccionado_Num, (LPARAM)EventoSeleccionado_Txt);
				AAux = APrin;
				while (AAux != NULL)
				{
					if (strcmp(AAux->Titulo, EventoSeleccionado_Txt) == 0)
					{
						char tex[MAX_PATH] = "";
						char Mensaje[900] = "";

						strcat_s(tex, "Titulo:\n     ");
						strcat_s(tex, AAux->Titulo);
						strcat_s(tex, "\n\n");
						strcat_s(Mensaje, tex);
						strcpy_s(tex, "");

						strcat_s(tex, "Descripcion:\n     ");
						strcat_s(tex, AAux->Descripcion);
						strcat_s(tex, "\n\n");
						strcat_s(Mensaje, tex);
						strcpy_s(tex, "");

						strcat_s(tex, "Al nombre de:\n     ");
						strcat_s(tex, AAux->NombreDelContacto);
						strcat_s(tex, "\n\n");
						strcat_s(Mensaje, tex);
						strcpy_s(tex, "");

						strcat_s(tex, "Ubicacion:\n     ");
						strcat_s(tex, AAux->Ubicacion);
						strcat_s(tex, "\n\n");
						strcat_s(Mensaje, tex);
						strcpy_s(tex, "");

						strcat_s(tex, "Fecha:\n     ");
						strcat_s(tex, AAux->Fecha.Dia);
						strcat_s(tex, "/");
						strcat_s(tex, AAux->Fecha.Mes);
						strcat_s(tex, "/");
						strcat_s(tex, AAux->Fecha.Año);
						strcat_s(tex, "\n\n");
						strcat_s(Mensaje, tex);
						strcpy_s(tex, "");
						strcat_s(tex, "Hora:\n     ");
						strcat_s(tex, AAux->Fecha.Hora);
						strcat_s(tex, ":");
						if (AAux->Fecha.Minuto[0] == '0'&& AAux->Fecha.Minuto[1] == NULL)
							strcat_s(tex, "00");
						else if (AAux->Fecha.Minuto[0] == '1'&& AAux->Fecha.Minuto[1] == NULL)
							strcat_s(tex, "01");
						else if (AAux->Fecha.Minuto[0] == '2'&& AAux->Fecha.Minuto[1] == NULL)
							strcat_s(tex, "02");
						else if (AAux->Fecha.Minuto[0] == '3'&& AAux->Fecha.Minuto[1] == NULL)
							strcat_s(tex, "03");
						else if (AAux->Fecha.Minuto[0] == '4'&& AAux->Fecha.Minuto[1] == NULL)
							strcat_s(tex, "04");
						else if (AAux->Fecha.Minuto[0] == '5'&& AAux->Fecha.Minuto[1] == NULL)
							strcat_s(tex, "05");
						else if (AAux->Fecha.Minuto[0] == '6'&& AAux->Fecha.Minuto[1] == NULL)
							strcat_s(tex, "06");
						else if (AAux->Fecha.Minuto[0] == '7'&& AAux->Fecha.Minuto[1] == NULL)
							strcat_s(tex, "07");
						else if (AAux->Fecha.Minuto[0] == '8'&& AAux->Fecha.Minuto[1] == NULL)
							strcat_s(tex, "08");
						else if (AAux->Fecha.Minuto[0] == '9'&& AAux->Fecha.Minuto[1] == NULL)
							strcat_s(tex, "09");
						else
							strcat_s(tex, AAux->Fecha.Minuto);

						strcat_s(tex, "\n\n");
						strcat_s(Mensaje, tex);
						strcpy_s(tex, "");

						strcat_s(tex, "Tono:\n     ");
						strcat_s(Mensaje, tex);
						strcpy_s(tex, "");
						strcat_s(tex, AAux->Tono);

						char Cancion[MAX_PATH] = "";

						int Derecha = 0;
						int Izquierda = 0;
						while (1)
						{
							if (tex[Derecha] == '.' && tex[Derecha + 1] == 'm' && tex[Derecha + 2] == 'p' && tex[Derecha + 3] == '3')
								break;
							else
								Derecha++;
						}
						Izquierda = Derecha;
						while (1)
						{
							if (tex[Izquierda] == '\\')
								break;
							else
								Izquierda--;
						}
						Izquierda++;
						int inicio = 0;
						for (Izquierda; Izquierda < Derecha; Izquierda++)
						{
							Cancion[inicio] = tex[Izquierda];
							inicio++;
						}

						strcat_s(Mensaje, Cancion);
						strcpy_s(tex, "");
						strcat_s(Mensaje, "\n\nRepeticion:\n     ");
						strcpy_s(tex, AAux->Tipo);
						if (tex[0] == '1')
							strcat_s(Mensaje, "Solo una vez");
						else if (tex[0] == '2')
							strcat_s(Mensaje, "Cada dia");
						else if (tex[0] == '3')
							strcat_s(Mensaje,"Cada semana");
						else if (tex[0] == '4')
							strcat_s(Mensaje, "Cada año");

						MessageBox(hWnd, Mensaje, "Descripcion del evento", MB_OK);


						break;
					}
					else
					{
						AAux = AAux->sig;
					}
				}
			}
			
			

		}
			return true;
		case LBN_SELCHANGE:
		{
			int EventoSeleccionado_Num = SendMessage(GetDlgItem(hWnd, IDC_TEVENTOS_LISTA), LB_GETCURSEL, 0, 0);
			if (EventoSeleccionado_Num >= 0)
			{
				SendMessage(GetDlgItem(hWnd, IDC_TEVENTOS_LISTA), LB_GETTEXT, EventoSeleccionado_Num, (LPARAM)EventoSeleccionado_Txt);
				AAux = APrin;
				while (AAux != NULL)
				{
					if (strcmp(AAux->Titulo, EventoSeleccionado_Txt) == 0)
					{
						bool Continuar = true;
						time(&rawtime);
						localtime_s(&timeinfo, &rawtime);
						int Segundo[5]; //Año -- Mes -- Dia -- Hora -- Min
						int Primero[5]; //Año -- Mes -- Dia -- Hora -- Min

						Primero[0] = atoi(AAux->Fecha.Año);
						Segundo[0] = timeinfo.tm_year + 1900;

						Primero[1] = atoi(AAux->Fecha.Mes);
						Primero[1]--;
						Segundo[1] = timeinfo.tm_mon;

						Primero[2] = atoi(AAux->Fecha.Dia);
						Segundo[2] = timeinfo.tm_mday;

						Primero[3] = atoi(AAux->Fecha.Hora);
						Segundo[3] = timeinfo.tm_hour;

						Primero[4] = atoi(AAux->Fecha.Minuto);
						Segundo[4] = timeinfo.tm_min;

						if (Primero[0] > Segundo[0])
						{
							Continuar = true;
						}
						else if (Primero[0] < Segundo[0])
						{
							Continuar = false;
						}
						else
						{
							if (Primero[1] > Segundo[1])
							{
								Continuar = true;
							}
							else if (Primero[1] < Segundo[1])
							{
								Continuar = false;
							}
							else
							{
								if (Primero[2] > Segundo[2])
								{
									Continuar = true;
								}
								else if (Primero[2] < Segundo[2])
								{
									Continuar = false;
								}
								else
								{
									if (Primero[3] > Segundo[3])
									{
										Continuar = true;
									}
									else if (Primero[3] < Segundo[3])
									{
										Continuar = false;
									}
									else
									{
										if (Primero[4] > Segundo[4])
										{
											Continuar = true;
										}
										else if (Primero[4] < Segundo[4])
										{
											Continuar = false;
										}
										else if (Primero[4] == Segundo[4])
										{
											Continuar = false;
										}
									}
								}
							}
						}
						if (Continuar == false)						
							EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_MODIFICAR), false);
						else	
							EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_MODIFICAR), true);
							
						
						SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA2, LB_SETCURSEL, EventoSeleccionado_Num, 0);
						EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_DELETE), true);
						break;
					}
					else
						AAux = AAux->sig;
				}
			}
			else
			{
				EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_MODIFICAR), false);
				EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_DELETE), false);
			}
		}
		return true;
		}

		switch (LOWORD(wParam))
		{
		case IDC_TEVENTOS_CLOSE:
			EndDialog(hWnd, 0);
			return true;
		case IDC_TEVENTOS_NEW:
		{
			MostrarAlarmaSeleccionada = false;
			DialogBox(hEventos, MAKEINTRESOURCE(IDD_EVENTOS), hWnd, Eventos);
			ReCargarTodosEventos(hWnd);
			if (SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA, LB_GETCOUNT, 0, 0) == 0)
				EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_DELETEALL), false);
			else
				EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_DELETEALL), true);
		}
			return true;
		case IDC_TEVENTOS_MODIFICAR:
			if (APrin != NULL)
			{
				AAux = APrin;
				while (AAux != NULL)
				{
					if (strcmp(AAux->Titulo, EventoSeleccionado_Txt) == 0)
						break;
					else
						AAux = AAux->sig;
				}
				MostrarAlarmaSeleccionada = true;
				DialogBox(hEventos, MAKEINTRESOURCE(IDD_EVENTOS), hWnd, Eventos);
			}
			ReCargarTodosEventos(hWnd);
			return true;
		case IDC_TEVENTOS_DELETE:
		{
			if (MessageBox(hWnd, "¿Estas seguro que deseas eliminar este evento?", "Advertencia", MB_YESNO | MB_ICONINFORMATION) == IDYES)
			{
				char TTitulo[40] = "";
				int ind = SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA, LB_GETCURSEL, 0, 0);
				SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA, LB_GETTEXT, ind, (LPARAM)TTitulo);
				AAux = APrin;
				while (AAux != NULL)
				{
					if (strcmp(TTitulo, AAux->Titulo) == 0)
					{
						EliminarAlarma(AAux);
						MessageBox(hWnd, "El evento ha sido eliminado con exito", "Aviso", MB_OK | MB_ICONINFORMATION);
						AlarmaGuardar();
						ReCargarTodosEventos(hWnd);
						if (SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA, LB_GETCOUNT, 0, 0) == 0)
							EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_DELETEALL), false);
						else
							EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_DELETEALL), true);
					}
					else
						AAux = AAux->sig;

				}
			}
		}
			return true;
		case IDC_TEVENTOS_DELETEALL:
			if (MessageBox(hWnd, "¿Estas seguro que deseas eliminar TODOS los eventos?", "Advertencia", MB_YESNO | MB_ICONINFORMATION) == IDYES)
			{
				Alarmas *Alarmasig;
				AAux = APrin;

				while (AAux != NULL)
				{
					Alarmasig = AAux->sig;
					EliminarAlarma(AAux);
					AAux = Alarmasig;
				}
				AlarmaGuardar();
				ReCargarTodosEventos(hWnd);
				MessageBox(hWnd, "Se han eliminado todas las alarmas correctamente", "Aviso", MB_OK);



				if (SendDlgItemMessage(hWnd, IDC_TEVENTOS_LISTA, LB_GETCOUNT, 0, 0) == 0)
					EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_DELETEALL), false);
				else
					EnableWindow(GetDlgItem(hWnd, IDC_TEVENTOS_DELETEALL), true);
			}
			return true;
		}
	}
	return true;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return true;
	case WM_DESTROY:
		return true;
	}

	return false;
}

void BinarioGuardar()
{

	ofstream Sal;
	Sal.open(DireccionPrograma_Contactos, ios::binary | ios::out);
	if (Sal.is_open())
	{
		if (Prin != NULL)
		{
			Aux = Prin;
			while (Aux != NULL)
			{
				Sal << Aux->Nombre << endl;
				Sal << Aux->Categoria << endl;
				if (Aux->Genero == true)
					Sal << "Hombre" << endl;
				else
					Sal << "Mujer" << endl;

				Sal << Aux->Direccion.Calle << endl;

				Sal << Aux->Direccion.Num << endl;

				Sal << Aux->Direccion.Colonia << endl;

				Sal << Aux->Direccion.Municipio << endl;

				Sal << Aux->Direccion.Estado << endl;

				Sal << Aux->Telefono << endl;

				Sal << Aux->Celular << endl;

				Sal << Aux->Correo << endl;

				Sal << Aux->Foto1 << endl;

				Sal << Aux->Foto2 << endl;

				Sal << Aux->Orden << endl;

				Aux = Aux->sig;
			}

			Sal.close();
		}
	}

}

void BinarioCargar()
{
	if (Prin != NULL)
	{
		Aux = Prin;
		while (Aux->sig != NULL)
		{
			Aux2 = Aux;
			Aux = Aux->sig;
			delete Aux2;
		}
		Aux2 = Aux;
		delete Aux2;

		Aux = Prin = Fin = NULL;
	}

	ifstream Ent;
	Ent.open(DireccionPrograma_Contactos, ios::binary | ios::in);
	if (Ent.is_open())
	{
		Contactos Temp;
		char TTemp[40] = "";

		Ent.getline(Temp.Nombre, 40, '\n');
		while (!Ent.eof())
		{
			Ent.getline(Temp.Categoria, 40, '\n');
			Ent.getline(TTemp, 40, '\n');
			if (TTemp[0] == 'H')
				Temp.Genero = true;
			else
				Temp.Genero = false;
			Ent.getline(Temp.Direccion.Calle, 40, '\n');
			Ent.getline(Temp.Direccion.Num, 40, '\n' );
			Ent.getline(Temp.Direccion.Colonia, 40, '\n');
			Ent.getline(Temp.Direccion.Municipio, 40, '\n');
			Ent.getline(Temp.Direccion.Estado, 40, '\n');
			Ent.getline(Temp.Telefono, 40, '\n');
			Ent.getline(Temp.Celular, 40, '\n');
			Ent.getline(Temp.Correo, 40, '\n');
			Ent.getline(Temp.Foto1, MAX_PATH,'\n');
			Ent.getline(Temp.Foto2, MAX_PATH, '\n');
			Ent.getline(Temp.Orden, 10, '\n');

			AgregarContacto(Temp.Nombre, Temp.Direccion.Calle, Temp.Direccion.Num, Temp.Direccion.Colonia, Temp.Direccion.Municipio, Temp.Direccion.Estado, Temp.Genero, Temp.Correo, Temp.Telefono, Temp.Celular, Temp.Foto1, Temp.Foto2, Temp.Categoria, Temp.Orden);
			Ent.getline(Temp.Nombre, 40, '\n');
		}

		Ent.close();

		Aux = Prin;
		while (Aux != NULL)
		{
			int VcharOrden = atoi(charOrden), VAux = atoi(Aux->Orden);
			if (VcharOrden < VAux)
			{
				strcpy_s(charOrden, Aux->Orden);
			}
			IntOrden = atoi(charOrden);
			Aux = Aux->sig;
		}

	}

}

void AlarmaGuardar()
{
	ofstream Out;
	Out.open(DireccionPrograma_Alarmas, ios::binary | ios::out);
	if (Out.is_open())
	{
		if (APrin != NULL)
		{
			AAux = APrin;
			while (AAux != NULL)
			{
				Out << AAux->Titulo << endl;
				Out << AAux->Descripcion << endl;
				Out << AAux->Fecha.Dia << endl;
				Out << AAux->Fecha.Mes << endl;
				Out << AAux->Fecha.Año << endl;
				Out << AAux->Fecha.Hora << endl;
				Out << AAux->Fecha.Minuto << endl;
				Out << AAux->NombreDelContacto << endl;
				Out << AAux->Tono << endl;
				Out << AAux->Ubicacion << endl;
				Out << AAux->Tipo << endl;
				AAux = AAux->sig;
			}
			Out.close();
		}
	}
}

void AlarmaCargar()
{
	if (APrin != NULL)
	{
		AAux = APrin;
		while (AAux->sig != NULL)
		{
			AAux2 = AAux;
			AAux = AAux->sig;
			delete AAux2;
		}
		AAux2 = AAux;
		delete AAux2;

		AAux = APrin = AFin = NULL;
	}

	ifstream In;
	In.open(DireccionPrograma_Alarmas, ios::binary | ios::in);
	if (In.is_open())
	{
		Alarmas Temp;

		In.getline(Temp.Titulo, 40, '\n');
		while (!In.eof())
		{
			In.getline(Temp.Descripcion,MAX_PATH,'\n');
			In.getline(Temp.Fecha.Dia, 4, '\n');
			In.getline(Temp.Fecha.Mes, 20, '\n');
			In.getline(Temp.Fecha.Año, 6, '\n');
			In.getline(Temp.Fecha.Hora, 4, '\n');
			In.getline(Temp.Fecha.Minuto, 4, '\n');
			In.getline(Temp.NombreDelContacto, 40, '\n');
			In.getline(Temp.Tono, MAX_PATH, '\n');
			In.getline(Temp.Ubicacion, MAX_PATH, '\n');
			In.getline(Temp.Tipo, 4, '\n');
			AgregarEvento(Temp);
			In.getline(Temp.Titulo, 40, '\n');

		}
		In.close();
	}
}

void VerificarAlarmasViejas(HWND hWnd)
{
	if (APrin != NULL)
	{
		AAux = APrin;

		while (AAux != NULL)
		{
			AAux2 = AAux->sig;
			char Comando[MAX_PATH] = "";
			bool Continuar = true;
			time(&rawtime);
			localtime_s(&timeinfo, &rawtime);
			int Segundo[5]; //Año -- Mes -- Dia -- Hora -- Min
			int Primero[5]; //Año -- Mes -- Dia -- Hora -- Min

			Primero[0] = atoi(AAux->Fecha.Año);
			Segundo[0] = timeinfo.tm_year + 1900;

			Primero[1] = atoi(AAux->Fecha.Mes);
			Primero[1]--;
			Segundo[1] = timeinfo.tm_mon;

			Primero[2] = atoi(AAux->Fecha.Dia);
			Segundo[2] = timeinfo.tm_mday;

			Primero[3] = atoi(AAux->Fecha.Hora);
			Segundo[3] = timeinfo.tm_hour;

			Primero[4] = atoi(AAux->Fecha.Minuto);
			Segundo[4] = timeinfo.tm_min;

			if (Primero[0] > Segundo[0])
			{
				Continuar = true;
			}
			else if (Primero[0] < Segundo[0])
			{
				Continuar = false;
			}
			else
			{
				if (Primero[1] > Segundo[1])
				{
					Continuar = true;
				}
				else if (Primero[1] < Segundo[1])
				{
					Continuar = false;
				}
				else
				{
					if (Primero[2] > Segundo[2])
					{
						Continuar = true;
					}
					else if (Primero[2] < Segundo[2])
					{
						Continuar = false;
					}
					else
					{
						if (Primero[3] > Segundo[3])
						{
							Continuar = true;
						}
						else if (Primero[3] < Segundo[3])
						{
							Continuar = false;
						}
						else
						{
							if (Primero[4] > Segundo[4])
							{
								Continuar = true;
							}
							else if (Primero[4] < Segundo[4])
							{
								Continuar = false;
							}
							else if (Primero[4] == Segundo[4])
							{
								Continuar = false;
							}
						}
					}
				}
			}
			
			if (Continuar == false)
			{
				char Comando[MAX_PATH] = "";
				ifstream Lectura;
				Lectura.open(AAux->Tono, ios::binary | ios::in);
				if (Lectura.is_open())
				{
					NoDectetaMP3 = false;
					sprintf_s(Comando, "open \"%s\" alias MP3", AAux->Tono);
					Lectura.close();
				}
				else
				{
					NoDectetaMP3 = true;
					sprintf_s(Comando, "open \"%s\" alias MP3", DireccionPrograma_Alarma1);
				}
				ExpiroAlarma = true;

				mciSendString(Comando, NULL, 0, 0);
				mciSendString("play MP3", NULL, 0, 0);
				ASonando = AAux;
				DialogBox(hAlarma, MAKEINTRESOURCE(IDD_ALARMA), hWnd, CallAlarma);
				mciSendString("stop MP3", NULL, 0, 0);
				mciSendString("close MP3", NULL, 0, 0);
			}
			AAux = AAux2;
		}
	}
}

void EliminarAlarma(Alarmas *Alarmita)
{
	if (Alarmita->sig == NULL && Alarmita->ant == NULL)
	{
		delete Alarmita;
		APrin = AFin = AAux = AAux2 = NULL;
	}
	else if (Alarmita == APrin)
	{
		AAux2 = Alarmita->sig;
		APrin = AAux2;
		AAux2->ant = NULL;
		delete Alarmita;
	}
	else if (Alarmita == AFin)
	{
		AAux2 = Alarmita->ant;
		AFin = AAux2;
		AAux2->sig = NULL;
		delete Alarmita;
	}
	else
	{
		Alarmas *AAux3 = NULL;
		AAux2 = Alarmita->sig;
		AAux3 = Alarmita->ant;
		AAux2->ant = AAux3;
		AAux3->sig = AAux2;
		delete Alarmita;
	}
	AAux2 = NULL;
}

void ReCargarContactos(HWND hWnd)
{
	SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_LIST_CATEGORIA, LB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_RESETCONTENT, 0, 0);
	if (Prin != NULL)
	{
		char Sexo[2][20] = { "Hombre", "Mujer" };
		Aux = Prin;
		while (Aux != NULL)
		{
			
			SendDlgItemMessage(hWnd, IDC_LIST_NOMBRE, LB_ADDSTRING, sizeof(Aux->Nombre), (LPARAM)Aux->Nombre);
			SendDlgItemMessage(hWnd, IDC_LIST_CATEGORIA, LB_ADDSTRING, sizeof(Aux->Categoria), (LPARAM)Aux->Categoria);
			if (Aux->Genero == true)
				SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, sizeof(Sexo[0]), (LPARAM)Sexo[0]);
			else
				SendDlgItemMessage(hWnd, IDC_LIST_GENERO, LB_ADDSTRING, sizeof(Sexo[1]), (LPARAM)Sexo[1]);
			Aux = Aux->sig;
		}
	}
}

void ReCargarCategorias(HWND hWnd, UINT IDs, bool NuevaCate)
{
	SendDlgItemMessage(hWnd, IDs, CB_RESETCONTENT, 0, 0);
	ifstream CateIn;
	CateIn.open(DireccionPrograma_Categorias, ios::in);

	if (CateIn.is_open())
	{
		char Categorias[40] = "";

		CateIn.getline(Categorias, 40, '\n');
		while (!CateIn.eof())
		{
			SendDlgItemMessage(hWnd, IDs, CB_ADDSTRING, sizeof(Categorias), (LPARAM)Categorias);

			CateIn.getline(Categorias, 40, '\n');
		}
		CateIn.close();
	}
	if (NuevaCate == false)
	{
		SendDlgItemMessage(hWnd, IDs, CB_DELETESTRING, 0, 0);
		
		SendDlgItemMessage(hWnd, IDs, CB_SETCURSEL, 0, 0);
	}
}

void EliminarContacto(Contactos *Usuario)
{
	if (Usuario->sig == NULL && Usuario->ant == NULL)
	{
		delete Usuario;
		Prin = Fin = Aux = Aux2 = NULL;
	}
	else if (Usuario == Prin)
	{
		Aux2 = Usuario->sig;
		Prin = Aux2;
		Aux2->ant = NULL;
		delete Usuario;
	}
	else if (Usuario == Fin)
	{
		Aux2 = Usuario->ant;
		Fin = Aux2;
		Aux2->sig = NULL;
		delete Usuario;
	}
	else
	{
		Contactos *Aux3 = NULL;
		Aux2 = Usuario->sig;
		Aux3 = Usuario->ant;
		Aux2->ant = Aux3;
		Aux3->sig = Aux2;
		delete Usuario;
	}
	Aux2 = NULL;
}

void MoverAlarmas()
{
	Alarmas Temporal;
	strcpy_s(Temporal.Titulo, AAux2->Titulo);
	strcpy_s(Temporal.NombreDelContacto, AAux2->NombreDelContacto);
	strcpy_s(Temporal.Fecha.Año, AAux2->Fecha.Año);
	strcpy_s(Temporal.Fecha.Mes, AAux2->Fecha.Mes);
	strcpy_s(Temporal.Fecha.Dia, AAux2->Fecha.Dia);
	strcpy_s(Temporal.Fecha.Hora, AAux2->Fecha.Hora);
	strcpy_s(Temporal.Fecha.Minuto, AAux2->Fecha.Minuto);
	strcpy_s(Temporal.Descripcion, AAux2->Descripcion);
	strcpy_s(Temporal.Ubicacion, AAux2->Ubicacion);
	strcpy_s(Temporal.Tipo, AAux2->Tipo);
	strcpy_s(Temporal.Tono, AAux2->Tono);

	strcpy_s(AAux2->Titulo, AAux->Titulo);
	strcpy_s(AAux2->NombreDelContacto, AAux->NombreDelContacto);
	strcpy_s(AAux2->Fecha.Año, AAux->Fecha.Año);
	strcpy_s(AAux2->Fecha.Mes, AAux->Fecha.Mes);
	strcpy_s(AAux2->Fecha.Dia, AAux->Fecha.Dia);
	strcpy_s(AAux2->Fecha.Hora, AAux->Fecha.Hora);
	strcpy_s(AAux2->Fecha.Minuto, AAux->Fecha.Minuto);
	strcpy_s(AAux2->Descripcion, AAux->Descripcion);
	strcpy_s(AAux2->Ubicacion, AAux->Ubicacion);
	strcpy_s(AAux2->Tipo, AAux->Tipo);
	strcpy_s(AAux2->Tono, AAux->Tono);

	strcpy_s(AAux->Titulo, Temporal.Titulo);
	strcpy_s(AAux->NombreDelContacto, Temporal.NombreDelContacto);
	strcpy_s(AAux->Fecha.Año, Temporal.Fecha.Año);
	strcpy_s(AAux->Fecha.Mes, Temporal.Fecha.Mes);
	strcpy_s(AAux->Fecha.Dia, Temporal.Fecha.Dia);
	strcpy_s(AAux->Fecha.Hora, Temporal.Fecha.Hora);
	strcpy_s(AAux->Fecha.Minuto, Temporal.Fecha.Minuto);
	strcpy_s(AAux->Descripcion, Temporal.Descripcion);
	strcpy_s(AAux->Ubicacion, Temporal.Ubicacion);
	strcpy_s(AAux->Tipo, Temporal.Tipo);
	strcpy_s(AAux->Tono, Temporal.Tono);
}

void OrdenarAlarmas()
{
	if (APrin != NULL && APrin->sig !=NULL)
	{
		AAux = AFin;
		AAux2 = AAux;
		AAux = AAux->ant;


		while (AAux != NULL)
		{
			bool Cambiar = false;
			int Primero[5]; //AAux: Año -- Mes -- Dia -- Hora -- Min
			int Segundo[5]; //AAux2: Año -- Mes -- Dia -- Hora -- Min

			Primero[0] = atoi(AAux->Fecha.Año);
			Segundo[0] = atoi(AAux2->Fecha.Año);

			Primero[1] = atoi(AAux->Fecha.Mes);
			Primero[1]--;
			Segundo[1] = atoi(AAux2->Fecha.Mes);
			Segundo[1]--;

			Primero[2] = atoi(AAux->Fecha.Dia);
			Segundo[2] = atoi(AAux2->Fecha.Dia);

			Primero[3] = atoi(AAux->Fecha.Hora);
			Segundo[3] = atoi(AAux2->Fecha.Hora);

			Primero[4] = atoi(AAux->Fecha.Minuto);
			Segundo[4] = atoi(AAux2->Fecha.Minuto);



			if (Primero[0] > Segundo[0])
			{
				Cambiar = true;
			}
			else if (Primero[0] < Segundo[0])
			{
				Cambiar = false;
			}
			else
			{
				if (Primero[1] > Segundo[1])
				{
					Cambiar = true;
				}
				else if (Primero[1] < Segundo[1])
				{
					Cambiar = false;
				}
				else
				{
					if (Primero[2] > Segundo[2])
					{
						Cambiar = true;
					}
					else if (Primero[2] < Segundo[2])
					{
						Cambiar = false;
					}
					else
					{
						if (Primero[3] > Segundo[3])
						{
							Cambiar = true;
						}
						else if (Primero[3] < Segundo[3])
						{
							Cambiar = false;
						}
						else
						{
							if (Primero[4] > Segundo[4])
							{
								Cambiar = true;
							}
							else if (Primero[4] < Segundo[4])
							{
								Cambiar = false;
							}
						}
					}
				}
			}


			if (Cambiar == true)
			{
				MoverAlarmas();
			}


			AAux2 = AAux;
			AAux = AAux->ant;
		}
	}
}

void EstablecerCalendario(HWND hWnd, int Parametro, int Mes, int UltimoDia, int Año, int TuDia, int TuMes, int TuAño)
{
	char Texto[MAX_PATH] = "";
	char charAno[20] = "";
	bool Seguir = true;
	bool DetecteParametro = false;
	int intDia = 1;
	char charmesselec[4] = "";
	_itoa_s(Mes + 1, charmesselec, 10);
	char charmes[12][30] = { "Enero","Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto","Septiembre","Octubre", "Noviembre", "Diciembre" };
	SendDlgItemMessage(hWnd, IDC_MES, WM_SETTEXT, sizeof(charmes[Mes]), (LPARAM)charmes[Mes]);

	int IDs[6][7] = {
		{IDC_BLUNES_1,IDC_BMARTES_1,IDC_BMIERCOLES_1,IDC_BJUEVES_1,IDC_BVIERNES_1,IDC_BSABADO_1,IDC_BDOMINGO_1},
		{ IDC_BLUNES_2,IDC_BMARTES_2,IDC_BMIERCOLES_2,IDC_BJUEVES_2,IDC_BVIERNES_2,IDC_BSABADO_2,IDC_BDOMINGO_2 },
		{ IDC_BLUNES_3,IDC_BMARTES_3,IDC_BMIERCOLES_3,IDC_BJUEVES_3,IDC_BVIERNES_3,IDC_BSABADO_3,IDC_BDOMINGO_3 },
		{ IDC_BLUNES_4,IDC_BMARTES_4,IDC_BMIERCOLES_4,IDC_BJUEVES_4,IDC_BVIERNES_4,IDC_BSABADO_4,IDC_BDOMINGO_4 },
		{ IDC_BLUNES_5,IDC_BMARTES_5,IDC_BMIERCOLES_5,IDC_BJUEVES_5,IDC_BVIERNES_5,IDC_BSABADO_5,IDC_BDOMINGO_5 },
		{ IDC_BLUNES_6,IDC_BMARTES_6,IDC_BMIERCOLES_6,IDC_BJUEVES_6,IDC_BVIERNES_6,IDC_BSABADO_6,IDC_BDOMINGO_6 },
	};


	_itoa_s(Año, charAno, 10);
	SendDlgItemMessage(hWnd, IDC_YEAR, WM_SETTEXT, sizeof(charAno), (LPARAM)charAno);

	char charDia[3] = "";
	
	
	for (int Semana = 0; Semana < 6; Semana++)
	{
		for (int Dias = 0; Dias < 7; Dias++)
		{
			if ((Parametro == Dias + 1 || intDia > 1) && Seguir == true)
			{
				if (intDia == UltimoDia + 1)
				{
					ShowWindow(GetDlgItem(hWnd, IDs[Semana][Dias]), SW_HIDE);
					if (DetecteParametro == false)
						ParametroGlobal = Dias + 1;
					Seguir = false;
					DetecteParametro = true;
				}
				else //Aqui es para las alarmas :3
				{
					ShowWindow(GetDlgItem(hWnd, IDs[Semana][Dias]), SW_SHOW);
					_itoa_s(intDia, charDia, 10);
					int Contador = 0; 
					if (APrin != NULL)
					{
						AAux = APrin;
						
						char charContador[4] = "";
						while (AAux != NULL)
						{
							if (strcmp(AAux->Fecha.Año, charAno) == 0 && strcmp(AAux->Fecha.Mes, charmesselec) == 0 && strcmp(AAux->Fecha.Dia, charDia) == 0)
							{
								Contador++;
								strcpy_s(Texto, "");
								_itoa_s(Contador, charContador, 10);
								strcat_s(Texto, charDia);
								strcat_s(Texto, "\n\n*");
								strcat_s(Texto, charContador);
								if (Contador == 1)
									strcat_s(Texto, " evento programado");
								else
									strcat_s(Texto, " eventos programados");
								DiasCalendario_Eventos[Semana][Dias] = true;
								SendDlgItemMessage(hWnd, IDs[Semana][Dias], WM_SETTEXT, sizeof(Texto), (LPARAM)Texto);
							}
							AAux = AAux->sig;
						}
						AAux = NULL;
					}
					if (Contador == 0)
					{
						SendDlgItemMessage(hWnd, IDs[Semana][Dias], WM_SETTEXT, sizeof(charDia), (LPARAM)charDia);
						DiasCalendario_Eventos[Semana][Dias] = false;
					}
						strcpy_s(charDia, "");
					bool Bloqueardia = false;
					if(Año < TuAño)
					{
						Bloqueardia = true;
					}
					else if (Año > TuAño)					
					{
						Bloqueardia = false;
					}
					else
					{
						if (Mes+1 < TuMes)
						{
							Bloqueardia = true;
						}
						else if (Mes+1 > TuMes)
						{
							Bloqueardia = false;
						}
						else
						{
							if (intDia < TuDia)
							{
								Bloqueardia = true;
							}
							else if (intDia > TuDia)
							{
								Bloqueardia = false;
							}
						}
					
					}


					if (Bloqueardia == true)
					{
						EnableWindow(GetDlgItem(hWnd, IDs[Semana][Dias]), false);
					}
					else
					{
						EnableWindow(GetDlgItem(hWnd, IDs[Semana][Dias]), true);
					}
				}
				intDia++;
			}
			else
			{
				ShowWindow(GetDlgItem(hWnd, IDs[Semana][Dias]), SW_HIDE);
			}

		}
	}

}

void EstablecerCalendario2(HWND hWnd, int Parametro, int Mes, int Dia, int Año)
{
	int TuDia = timeinfo.tm_mday, TuMes = timeinfo.tm_mon, TuAño = timeinfo.tm_year + 1900;
	int TodosMeses[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	int IntDias = Dia;
	int ElMes = Mes;
	int IntAño = Año;
	char CharDias[3] = "";
	char CharAño[6] = "";
	char charmes[12][30] = { "Enero","Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto","Septiembre","Octubre", "Noviembre", "Diciembre" };

	if (Año % 4 == 0)
		TodosMeses[1] = 29;
	else
		TodosMeses[1] = 28;

	SendDlgItemMessage(hWnd, IDC_HORAS2, LB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_LISTLUNES, LB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_LISTMARTES, LB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_LISTMIERCOLES, LB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_LISTJUEVES, LB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_LISTVIERNES, LB_RESETCONTENT, 0,0);
	SendDlgItemMessage(hWnd, IDC_LISTSABADO, LB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_LISTDOMINGO, LB_RESETCONTENT, 0, 0);
	
	while (Parametro != 1)
	{
		Parametro--;
		IntDias--;

		if (IntDias == 0)
		{
			ElMes--;
			if (ElMes == -1)
			{
				ElMes = 11;
				IntAño--;
			}
			IntDias = TodosMeses[ElMes];
		}
	}

	int IDDias[7] = { IDC_DSLUNES,  IDC_DSMARTES ,IDC_DSMIERCOLES,IDC_DSJUEVES,IDC_DSVIERNES,  IDC_DSSABADO,IDC_DSDOMINGO };
	int IDMes[7] = { IDC_MSLUNES,  IDC_MSMARTES ,IDC_MSMIERCOLES,IDC_MSJUEVES,IDC_MSVIERNES,  IDC_MSSABADO,IDC_MSDOMINGO };
	int IDAño[7] = { IDC_ASLUNES,  IDC_ASMARTES ,IDC_ASMIERCOLES,IDC_ASJUEVES,IDC_ASVIERNES,  IDC_ASSABADO,IDC_ASDOMINGO };

	for (int i = 0; i < 7; i++)
	{
		_itoa_s(IntDias, CharDias, 10);
		_itoa_s(IntAño, CharAño, 10);
		SendDlgItemMessage(hWnd, IDDias[i], WM_SETTEXT, 0, (LPARAM)CharDias);
		SendDlgItemMessage(hWnd, IDMes[i], WM_SETTEXT, 0, (LPARAM)charmes[ElMes]);
		SendDlgItemMessage(hWnd, IDAño[i], WM_SETTEXT, 0, (LPARAM)CharAño);
		IntDias++;
		if (IntDias == TodosMeses[ElMes] + 1)
		{
			IntDias = 1;
			ElMes++;
			if (ElMes == 12)
			{
				ElMes = 0;
				IntAño++;
			}
		}
	}
	SemanalMes = ElMes;
	SemanalDiaDomingo = IntDias;
	SemanalAño = IntAño;
	int IDListas[7] = { IDC_LISTLUNES,IDC_LISTMARTES,IDC_LISTMIERCOLES,IDC_LISTJUEVES,IDC_LISTVIERNES,IDC_LISTSABADO,IDC_LISTDOMINGO };
	int HayEventos[7] = { 0,0,0,0,0,0,0 }; //0 Lunes - 1 Martes - 2 Miercoles - 3 Jueves - 4 Viernes - 5 Sabado - 6 Domingo

	for (int nu = 0; nu < 24; nu++)
	{
		SendDlgItemMessage(hWnd, IDC_HORAS2, LB_ADDSTRING, 0, (LPARAM)"-");
	}

	if (APrin != NULL)
	{
		for (int i = 0; i < 24; i++)
		{
				char charhorita[4] = "";
					_itoa_s(i, charhorita, 10);
			for (int n = 0; n<7; n++)
				HayEventos[n] = 0;

			AAux = APrin;

			while (AAux != NULL)
			{
				for (int j = 0; j < 7; j++)
				{
					char Añito[6] = "";
					char UsandoDia[4] = "";
					char UsandoMes[30] = "";
					char EsElMes[20] = "";
					SendDlgItemMessage(hWnd, IDDias[j], WM_GETTEXT, sizeof(UsandoDia), (LPARAM)UsandoDia);
					SendDlgItemMessage(hWnd, IDMes[j], WM_GETTEXT, sizeof(UsandoMes), (LPARAM)UsandoMes);
					SendDlgItemMessage(hWnd, IDAño[j], WM_GETTEXT, sizeof(Añito), (LPARAM)Añito);

					for (int p = 0; p < 12; p++)
					{
						if (strcmp(UsandoMes, charmes[p]) == 0)
						{
							_itoa_s(p + 1, EsElMes, 10);
							break;
						}
					}

					if (strcmp(AAux->Fecha.Dia, UsandoDia) == 0 && strcmp(AAux->Fecha.Mes, EsElMes) == 0 && strcmp(AAux->Fecha.Hora, charhorita) == 0 && strcmp(AAux->Fecha.Año,Añito)==0)
					{
						HayEventos[j]++;
					}
				}
				AAux = AAux->sig;
			}
			for (int q = 0; q < 7; q++)
			{
				if (HayEventos[q] > 0)
				{
					char Txt[50] = "";
					char numerito[5] = "";
					_itoa_s(HayEventos[q], numerito, 10);
					strcat_s(Txt, numerito);
					if (HayEventos[q] > 1)
						strcat_s(Txt, " eventos");
					else
						strcat_s(Txt, " evento");
					SendDlgItemMessage(hWnd, IDListas[q], LB_ADDSTRING, 0, (LPARAM)Txt);
				}
				else
				{
					bool Bloqueardia = false;
					char temaño[7] = "";
					SendDlgItemMessage(hWnd, IDAño[q], WM_GETTEXT, sizeof(temaño), (LPARAM)temaño);
					int Añoahora = atoi(temaño);
					if (Añoahora < TuAño)
					{
						Bloqueardia = true;
					}
					else if (Añoahora > TuAño)
					{
						Bloqueardia = false;
					}
					else
					{
						char temmes[30] = "";
						int Mesahora;
						SendDlgItemMessage(hWnd, IDMes[q], WM_GETTEXT, sizeof(temmes), (LPARAM)temmes);
						for (int re = 0; re < 12; re++)
						{
							if (strcmp(temmes, charmes[re]) == 0)
							{
								Mesahora = re;
								break;
							}
						}
						if (Mesahora < TuMes)
						{
							Bloqueardia = true;
						}
						else if (Mesahora > TuMes)
						{
							Bloqueardia = false;
						}
						else
						{
							char tem[4] = "";
							SendDlgItemMessage(hWnd, IDDias[q], WM_GETTEXT, sizeof(tem), (LPARAM)tem);
							int Diaahora = atoi(tem);
							if (Diaahora < TuDia)
							{
								Bloqueardia = true;
							}
							else if (Diaahora > TuDia)
							{
								Bloqueardia = false;
							}
							else
							{
								if (i < timeinfo.tm_hour)
								{
									Bloqueardia = true;
								}
								else
								{
									Bloqueardia = false;
								}

							}
						}

					}


					if (Bloqueardia == true)
					{
						SendDlgItemMessage(hWnd, IDListas[q], LB_ADDSTRING, 0, (LPARAM)"x");
					}
					else
					{
						SendDlgItemMessage(hWnd, IDListas[q], LB_ADDSTRING, 0, (LPARAM)"-");
					}
				}
			}
		}

	}
	else
	{
		for (int i = 0; i < 7; i++)
		{
			for (int q = 0; q < 24; q++)
			{
				bool Bloqueardia = false;
				char temaño[7] = "";
				SendDlgItemMessage(hWnd, IDAño[q], WM_GETTEXT, sizeof(temaño), (LPARAM)temaño);
				int Añoahora = atoi(temaño);
				if (Añoahora < TuAño)
				{
					Bloqueardia = true;
				}
				else if (Añoahora > TuAño)
				{
					Bloqueardia = false;
				}
				else
				{
					char temmes[30] = "";
					int Mesahora;
					SendDlgItemMessage(hWnd, IDMes[q], WM_GETTEXT, sizeof(temmes), (LPARAM)temmes);
					for (int re = 0; re < 12; re++)
					{
						if (strcmp(temmes, charmes[re]) == 0)
						{
							Mesahora = re;
							break;
						}
					}
					if (Mesahora < TuMes)
					{
						Bloqueardia = true;
					}
					else if (Mesahora > TuMes)
					{
						Bloqueardia = false;
					}
					else
					{
						char tem[4] = "";
						SendDlgItemMessage(hWnd, IDDias[q], WM_GETTEXT, sizeof(tem), (LPARAM)tem);
						int Diaahora = atoi(tem);
						if (Diaahora < TuDia)
						{
							Bloqueardia = true;
						}
						else if (Diaahora > TuDia)
						{
							Bloqueardia = false;
						}
						else
						{
							if (i < timeinfo.tm_hour)
							{
								Bloqueardia = true;
							}
							else
							{
								Bloqueardia = false;
							}

						}
					}

				}


				if (Bloqueardia == true)
				{
					SendDlgItemMessage(hWnd, IDListas[q], LB_ADDSTRING, 0, (LPARAM)"x");
				}
				else
				{
					SendDlgItemMessage(hWnd, IDListas[q], LB_ADDSTRING, 0, (LPARAM)"-");
				}
			}
		}
	}
}

