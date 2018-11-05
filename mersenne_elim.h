#pragma once

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h>
#include <stdarg.h>
#include <pthread.h>

#define INT8 char
#define INT16 short
#define INT32 int
#define INT64 long long int
#define UINT8 unsigned char
#define UINT16 unsigned short
#define UINT32 unsigned int
#define UINT64 unsigned long long int
#define CHAR char
#define BYTE unsigned char
#define FLOAT float
#define DOUBLE double
#define BOOL bool
#define UINT128 unsigned __int128

#define PINT8 char*
#define PINT16 short*
#define PINT32 int*
#define PINT64 long long int*
#define PUINT8 unsigned char*
#define PUINT16 unsigned short*
#define PUINT32 unsigned int*
#define PUINT64 unsigned long long int*
#define PCHAR char*
#define PBYTE unsigned char*
#define PFLOAT float*
#define PDOUBLE double*
#define PBOOL bool*
#define PVOID void*

#define CONST_PINT8 const char*
#define CONST_PINT16 const short*
#define CONST_PINT32 const int*
#define CONST_PINT64 const long long int*
#define CONST_PUINT8 const unsigned char*
#define CONST_PUINT16 const unsigned short*
#define CONST_PUINT32 const unsigned int*
#define CONST_PUINT64 const unsigned long long int*
#define CONST_PCHAR const char*
#define CONST_PBYTE const unsigned char*
#define CONST_PFLOAT const float*
#define CONST_PDOUBLE const double*
#define CONST_PBOOL const bool*
#define CONST_PVOID const void*

//  ...............................................................................................
//  Trieda TWorker
//  ...............................................................................................
class TWorker {
private:
    INT32 FPocetArgumentov;
    PCHAR *FArgumenty;
protected:
    BOOL FPoziadavkaNaUkoncenie; // Priznak poziadavky na ukoncenie spracovania
    BOOL FUkladanieStavu; // Priznak ukladania stavu
    CHAR FNazovZakladnehoSuboruKandidatov[260]; // Nazov zakladneho suboru kandidatov
    CHAR FNazovSuboruAktualnychKandidatov[260]; // Nazov aktualneho suboru kandidatov
    CHAR FNazovSuboruAktualnehoStavu[260]; // Nazov suboru pre ulozenie stavu
    DOUBLE FIntervalUkladaniaStavu; // Interval ukladania stavu
    FILE *FLogFile;
public:
    TWorker(void); // Konstruktor
    virtual ~TWorker(void); // Destruktor
protected:
    virtual void Uzatvorenie(void); // Uzatvorenie
    virtual BOOL Inicializacia(void); // Inicializacia objektu
    virtual void Spracovanie(void); // Spracovanie
public:
    BOOL Inicializacia(INT32 iPocetArgumentov, PCHAR *iArgumenty); // Inicializacia
    virtual void ZobrazenieInfo(void); // Zobrazenie info
    virtual void ZobrazenieVysledkovSpracovania(void); // Zobrazenie vysledkov spracovania
    void LogPrint(CONST_PCHAR iFormatovanaSprava, ...); // Print aj log spolu
public:
    virtual BOOL NacitanieZaciatocnehoStavu(void); // Nacitanie zaciatocneho stavu
    virtual BOOL NacitanieUlozenehoStavu(void); // Nacitanie ulozeneho stavu
    virtual BOOL JeUlozenyStav(void); // Ziskanie priznaku ulozeneho stavu
    virtual BOOL UlozenieStavu(BOOL iZobrazenieInfo = false); // Ulozenie stavu
public:
    void SpustenieSpracovania(void); // Spustenie spracovania
    void ZastavenieSpracovania(void); // Zastavenie spracovania
public:
    BOOL ParameterExistuje(CONST_PCHAR iNazov); // Test existencie parametra
    INT32 ParameterINT32(CONST_PCHAR iNazov, INT32 iPrednastavenaHodnota = 0); // Ziskanie parametra
public:
    static UINT64 SpustenieMerania(void); // Spustenie merania
    static DOUBLE ZastavenieMerania(UINT64 iZaciatok); // Zastavenie merania
};
//  ...............................................................................................
