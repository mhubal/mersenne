#include "mersenne_elim.h"

//  ...............................................................................................
//  Konstruktor
//  ...............................................................................................
TWorker::TWorker(void) {

}
//  ...............................................................................................
//  ...............................................................................................
//  Destruktor
//  ...............................................................................................
TWorker::~TWorker(void) {
    Uzatvorenie(); // Uzatvorenie
}
//  ...............................................................................................
//  ...............................................................................................
//  Uzatvorenie
//  ...............................................................................................
void TWorker::Uzatvorenie(void) {
    delete FArgumenty; // Uvolnenie pamate
    if (FLogFile != NULL) fclose(FLogFile);
}
//  ...............................................................................................
//  ...............................................................................................
//  Inicializacia objektu
//  ...............................................................................................
BOOL TWorker::Inicializacia(void) {
    setvbuf(stdout, NULL, _IONBF, 0); // Autoflush pre printf

    strcpy(FNazovZakladnehoSuboruKandidatov, "/home/mhubal/Vyskum/Prvocisla/Mersenne/data/kandidati.txt");
    strcpy(FNazovSuboruAktualnychKandidatov, "./kandidati.progress");
    strcpy(FNazovSuboruAktualnehoStavu, "./stav.progress");
    FIntervalUkladaniaStavu = 600.0; // Sekundy

    FUkladanieStavu = !ParameterExistuje("-neukladat_stav"); // Priznak ukladania stavu

    FLogFile = NULL;
    if (!ParameterExistuje("-bez_logu")) FLogFile = fopen("mersenne_elim.log", "at+");

    return true;
}
//  ...............................................................................................
//  ...............................................................................................
//  Inicializacia
//  ...............................................................................................
BOOL TWorker::Inicializacia(INT32 iPocetArgumentov, PCHAR *iArgumenty) {
    
    FPocetArgumentov = iPocetArgumentov - 1; // Ulozenie argumentov spustenia
    FArgumenty = new PCHAR[FPocetArgumentov];
    for (INT32 i = 0; i < FPocetArgumentov; i++) FArgumenty[i] = iArgumenty[i + 1];

    return Inicializacia();
}
//  ...............................................................................................
//  ...............................................................................................
//  Nacitanie zaciatocneho stavu
//  ...............................................................................................
BOOL TWorker::NacitanieZaciatocnehoStavu(void) {
    return false;
}
//  ...............................................................................................
//  ...............................................................................................
//  Nacitanie ulozeneho stavu
//  ...............................................................................................
BOOL TWorker::NacitanieUlozenehoStavu(void) {
    return false;
}
//  ...............................................................................................
//  ...............................................................................................
//  Ziskanie priznaku ulozeneho stavu
//  ...............................................................................................
BOOL TWorker::JeUlozenyStav(void) {
    return false;
}
//  ...............................................................................................
//  ...............................................................................................
//  Ulozenie stavu
//  ...............................................................................................
BOOL TWorker::UlozenieStavu(BOOL iZobrazenieInfo) {
    return false;
}
//  ...............................................................................................
//  ...............................................................................................
//  Spustenie spracovania
//  ...............................................................................................
void TWorker::SpustenieSpracovania(void) {
    FPoziadavkaNaUkoncenie = false;
    Spracovanie(); // Volanie spracovania
}
//  ...............................................................................................
//  ...............................................................................................
//  Zastavenie spracovania
//  ...............................................................................................
void TWorker::ZastavenieSpracovania(void) {
    FPoziadavkaNaUkoncenie = true;
    usleep(1000);
}
//  ...............................................................................................
//  ...............................................................................................
//  Zobrazenie info
//  ...............................................................................................
void TWorker::ZobrazenieInfo(void) {

}
//  ...............................................................................................
//  ...............................................................................................
//  Test existencie parametra
//  ...............................................................................................
BOOL TWorker::ParameterExistuje(CONST_PCHAR iNazov) {
    size_t L  = strlen(iNazov);
    for (INT32 i = 0; i < FPocetArgumentov; i++) {
        if (strncasecmp(FArgumenty[i], iNazov, L) == 0) return true;
    }
    return false;
}
//  ...............................................................................................
//  ...............................................................................................
//  Zobrazenie vysledkov spracovania
//  ...............................................................................................
void TWorker::ZobrazenieVysledkovSpracovania(void) {

}
//  ...............................................................................................
//  ...............................................................................................
//  Spracovanie
//  ...............................................................................................
void TWorker::Spracovanie(void) {

}
//  ...............................................................................................
//  ...............................................................................................
//  Spustenie merania
//  ...............................................................................................
UINT64 TWorker::SpustenieMerania(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000000 + t.tv_usec;
}
//  ...............................................................................................
//  ...............................................................................................
//  Zastavenie merania
//  ...............................................................................................
DOUBLE TWorker::ZastavenieMerania(UINT64 iZaciatok) {
    struct timeval t;
    gettimeofday(&t, NULL);
    UINT64 E = t.tv_sec * 1000000 + t.tv_usec;
    return (DOUBLE)(E - iZaciatok) / 1000000.0;
}
//  ...............................................................................................
//  ...............................................................................................
//  Print aj log spolu
//  ...............................................................................................
void TWorker::LogPrint(CONST_PCHAR iFormatovanaSprava, ...) {
    va_list l;
    va_start(l, iFormatovanaSprava);
    vprintf(iFormatovanaSprava, l);
    va_end(l);
    if (FLogFile != NULL) {
        va_start(l, iFormatovanaSprava);
        vfprintf(FLogFile, iFormatovanaSprava, l);
        fflush(FLogFile);
        va_end(l);
    }
}
//  ...............................................................................................
//  ...............................................................................................
//  Ziskanie parametra
//  ...............................................................................................
INT32 TWorker::ParameterINT32(CONST_PCHAR iNazov, INT32 iPrednastavenaHodnota) {
    CHAR BUF[1024]; sprintf(BUF, "%s=", iNazov);
    size_t L  = strlen(BUF);
    for (INT32 i = 0; i < FPocetArgumentov; i++) {
        if (strncasecmp(FArgumenty[i], BUF, L) == 0) {
            INT32 I;
            return sscanf(FArgumenty[i] + L, "%d", &I) == 1 ? I : iPrednastavenaHodnota;
        }
    }
    return iPrednastavenaHodnota;
}
//  ...............................................................................................
