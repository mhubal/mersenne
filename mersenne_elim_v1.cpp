#include "mersenne_elim.h"
#include "mersenne_elim_v1.h"

#define MOD_MASKA 134217728

extern "C" UINT64 asm_mod(UINT64 iExponent, UINT64 iMod);

typedef struct {
    PUINT64 Kandidati;
    UINT64 Zaciatok;
    UINT64 CelkovyPocet;
    UINT64 AktualnyKoeficient;
    UINT64 AktualnyPocet;
    BOOL Spracovane;
} THR_DATA, *PTHR_DATA;

//  ...............................................................................................
//  Vypocet mod
//  ...............................................................................................
UINT64 VypocetMOD2(UINT64 iExponent, UINT64 iMod) {
    UINT128 S = 2; UINT64 M = MOD_MASKA; UINT128 Mod = iMod;
    while (M > 0) {
        if (M & iExponent) S = (S * S * 2) % Mod;
        else S = (S * S) % Mod;
        M >>= 1;
    }
    return (UINT64)S;
}
//  ...............................................................................................

//  ...............................................................................................
//  Konstruktor
//  ...............................................................................................
TWorkerV1::TWorkerV1(void) {

}
//  ...............................................................................................
//  ...............................................................................................
//  Destruktor
//  ...............................................................................................
TWorkerV1::~TWorkerV1(void) {
}
//  ...............................................................................................
//  ...............................................................................................
//  Uzatvorenie
//  ...............................................................................................
void TWorkerV1::Uzatvorenie(void) {
    TWorker::Uzatvorenie();

    if (FKandidati != NULL) delete FKandidati; // Uvolnenie pamate

    FPociatocnyPocetKandidatov = 0; // Inicializacia
    FAktualnyPocetKandidatov = 0;
    FKandidati = NULL;
}
//  ...............................................................................................
//  ...............................................................................................
//  Inicializacia objektu
//  ...............................................................................................
BOOL TWorkerV1::Inicializacia(void) {
    if (!TWorker::Inicializacia()) return false;
    
    FPociatocnyPocetKandidatov = 0; // Inicializacia
    FAktualnyPocetKandidatov = 0;
    FKandidati = NULL;
    FPocetVlakien = ParameterINT32("-vlakien", 1);

    return true; // OK
}
//  ...............................................................................................
//  ...............................................................................................
//  Nacitanie zaciatocneho stavu
//  ...............................................................................................
BOOL TWorkerV1::NacitanieZaciatocnehoStavu(void) {

    LogPrint("\nInicializacia dat...");

    FILE *F = fopen(FNazovZakladnehoSuboruKandidatov, "rt");
    if (F == NULL) {
        LogPrint("\nCHYBA! Nie je mozne otvorit zakladny subor kandidatov.");
        LogPrint("\n---------------------------------------------------------------------------------");
        return false;
    }

    UINT64 C = 0; UINT64 U;
    LogPrint("\n\tZiskava sa pocet zaznamov -> ");
    while(fscanf(F, "%llu", &U) == 1) C++;
    LogPrint("%llu", C);

    FPociatocnyPocetKandidatov = C;
    FKandidati = new UINT64[FPociatocnyPocetKandidatov]; // Alokacia pamate
    fseek(F, 0L, SEEK_SET); // Rewind suboru

    LogPrint("\n\tNacitavaju sa kandidati -> ");
    C = 0;
    while(fscanf(F, "%llu", FKandidati + C) == 1) C++;
    LogPrint("%llu", C);

    if (C != FPociatocnyPocetKandidatov) {
        fclose(F);
        LogPrint("\nCHYBA! Nie je mozne citat zo zakladneho suboru kandidatov.");
        LogPrint("\n---------------------------------------------------------------------------------");
        return false;
    }

    fclose(F);
    LogPrint(". OK.");

    FAktualnyPocetKandidatov = FPociatocnyPocetKandidatov;
    FPociatocnyKoeficient = 1;
    FMaximalnyKoeficient = 1; FMaximalnyKoeficient <<= 63; FMaximalnyKoeficient -= 2;
    FMaximalnyKoeficient /= 2; FMaximalnyKoeficient /= FKandidati[FPociatocnyPocetKandidatov - 1];
    FAktualnyKoeficient = FPociatocnyKoeficient;

    LogPrint("\nStartovaci pocet kandidatov: %llu", FPociatocnyPocetKandidatov);
    LogPrint("\nStartovaci koeficient: %llu", FPociatocnyKoeficient);
    LogPrint("\nMaximalny koeficient pre spracovanie: %llu", FMaximalnyKoeficient);

    LogPrint("\n---------------------------------------------------------------------------------");
    return true;
}
//  ...............................................................................................
//  ...............................................................................................
//  Nacitanie ulozeneho stavu
//  ...............................................................................................
BOOL TWorkerV1::NacitanieUlozenehoStavu(void) {
    LogPrint("\nObnovenie posledneho stavu...");

    FILE *F = fopen(FNazovSuboruAktualnychKandidatov, "rb");
    if (F == NULL) {
        LogPrint("\nCHYBA! Nie je mozne otvorit subor kandidatov.");
        LogPrint("\n---------------------------------------------------------------------------------");
        return false;
    }

    // Ziskanie poctu prvkov
    fseek(F, 0L, SEEK_END); FPociatocnyPocetKandidatov = (UINT64)ftell(F) / sizeof(UINT64); fseek(F, 0L, SEEK_SET);
    FKandidati = new UINT64[FPociatocnyPocetKandidatov]; // Alokacia pamate
    // Nacitanie kandidatov
    if (fread(FKandidati, sizeof(UINT64), FPociatocnyPocetKandidatov, F) != FPociatocnyPocetKandidatov) {
        fclose(F);
        LogPrint("\nCHYBA! Nie je mozne citat zo suboru kandidatov.");
        LogPrint("\n---------------------------------------------------------------------------------");
        return false;
    }
    fclose(F);

    F = fopen(FNazovSuboruAktualnehoStavu, "rt");
    if (F == NULL) {
        LogPrint("\nCHYBA! Nie je mozne otvorit subor posledneho stavu.");
        LogPrint("\n---------------------------------------------------------------------------------");
        return false;
    }
    if (fscanf(F, "%llu", &FPociatocnyKoeficient) != 1) FPociatocnyKoeficient = 1;
    fclose(F);

    UINT64 Max = FKandidati[0];
    for (UINT64 i = 1; i < FPociatocnyPocetKandidatov; i++) {
        if (FKandidati[i] > Max) Max = FKandidati[i];
    }

    FAktualnyPocetKandidatov = FPociatocnyPocetKandidatov;
    FMaximalnyKoeficient = 1; FMaximalnyKoeficient <<= 63; FMaximalnyKoeficient -= 2;
    FMaximalnyKoeficient /= 2; FMaximalnyKoeficient /= Max;
    FAktualnyKoeficient = FPociatocnyKoeficient;

    LogPrint("\nStartovaci pocet kandidatov: %llu", FPociatocnyPocetKandidatov);
    LogPrint("\nStartovaci koeficient: %llu", FPociatocnyKoeficient);
    LogPrint("\nMaximalny koeficient pre spracovanie: %llu", FMaximalnyKoeficient);

    LogPrint("\n---------------------------------------------------------------------------------");
    return true;
}
//  ...............................................................................................
//  ...............................................................................................
//  Ziskanie priznaku ulozeneho stavu
//  ...............................................................................................
BOOL TWorkerV1::JeUlozenyStav(void) {
    struct stat S;
    if (stat(FNazovSuboruAktualnehoStavu, &S) != 0) return false;
    if (stat(FNazovSuboruAktualnychKandidatov, &S) != 0) return false;
    return true;
}
//  ...............................................................................................
//  ...............................................................................................
//  Ulozenie stavu
//  ...............................................................................................
BOOL TWorkerV1::UlozenieStavu(BOOL iZobrazenieInfo) {
    if (!FUkladanieStavu) return true; // Preskocenie ukladania

    if (iZobrazenieInfo) LogPrint("\nUkladanie stavu -> ");
    
    CHAR FN1[260]; strcpy(FN1, FNazovSuboruAktualnychKandidatov); strcat(FN1, ".tmp"); // Pomocny subor
    FILE *F = fopen(FN1, "wb+");
    if (F == NULL) {
        if (iZobrazenieInfo) {
            LogPrint("CHYBA! Nie je mozne vytvorit aktualny subor kandidatov.");
        }
        return false;
    }
    if (fwrite(FKandidati, sizeof(UINT64), FAktualnyPocetKandidatov, F) != FAktualnyPocetKandidatov) {
        if (iZobrazenieInfo) {
            LogPrint("CHYBA! Nie je mozne zapisat do aktualneho suboru kandidatov.");
        }
        fclose(F);
        return false;
    }
    fclose(F);

    CHAR FN2[260]; strcpy(FN2, FNazovSuboruAktualnehoStavu); strcat(FN2, ".tmp"); // Pomocny subor
    F = fopen(FN2, "wt+");
    fprintf(F, "%llu\n", FAktualnyKoeficient);
    fclose(F);

    remove(FNazovSuboruAktualnychKandidatov); // Vymazanie suborov posledneho stavu
    remove(FNazovSuboruAktualnehoStavu);
    rename(FN1, FNazovSuboruAktualnychKandidatov); // Premenovanie suborov
    rename(FN2, FNazovSuboruAktualnehoStavu);

    if (iZobrazenieInfo) LogPrint("OK.");
    return true;
}
//  ...............................................................................................
//  ...............................................................................................
//  Zobrazenie info
//  ...............................................................................................
void TWorkerV1::ZobrazenieInfo(void) {
    LogPrint("\n---------------------------------------------------------------------------------");
    LogPrint("\nmersenne_elim verzia 1.05");
    LogPrint("\n---------------------------------------------------------------------------------");
    LogPrint("\nNastavenia:");
    LogPrint("\n\tPovolene nacitanie predosleho stavu: %s", ParameterExistuje("-inicializacia") ? "nie" : " ano");
    LogPrint("\n\tPovolene ukladanie stavu: %s", !FUkladanieStavu ? "nie" : " ano");
    LogPrint("\n\tZakladny subor kandidatov: [%s]", FNazovZakladnehoSuboruKandidatov);
    LogPrint("\n\tPovoleny log: [%s]", FLogFile != NULL ? "ano" : "nie");
    LogPrint("\n\tInterval ukladania stavu: [%.0lf sekund]", FIntervalUkladaniaStavu);
    LogPrint("\n\tPocet vlakien: [%d]", FPocetVlakien);
    LogPrint("\n---------------------------------------------------------------------------------");
}
//  ...............................................................................................
//  ...............................................................................................
//  Zobrazenie vysledkov spracovania
//  ...............................................................................................
void TWorkerV1::ZobrazenieVysledkovSpracovania(void) {
    LogPrint("\n---------------------------------------------------------------------------------");
    LogPrint("\nVysledky spracovania:");
    LogPrint("\n\tPociatocny pocet kandidatov: %llu", FPociatocnyPocetKandidatov);
    LogPrint("\n\tKoncovy pocet kandidatov: %llu", FAktualnyPocetKandidatov);
    LogPrint("\n\tPocet eliminovanych kandidatov: %llu", FPociatocnyPocetKandidatov - FAktualnyPocetKandidatov);
    LogPrint("\n\tPociatocny koeficient: %llu", FPociatocnyKoeficient);
    LogPrint("\n\tPosledne spracovany koeficient: %llu", FAktualnyKoeficient);
    LogPrint("\n---------------------------------------------------------------------------------");
}
//  ...............................................................................................

//  ...............................................................................................
//  ...............................................................................................
PVOID VlaknoSpracovanie(PVOID iData) {
    PTHR_DATA Data = (PTHR_DATA)iData;

    UINT64 k = 0; Data->Spracovane = false;
    while(k < Data->AktualnyPocet) {
        
        if (Data->AktualnyKoeficient >= Data->Kandidati[k + Data->Zaciatok]) {
            k++;
            continue;
        }

        Data->Spracovane = true;
        UINT64 Kand = 2 * Data->AktualnyKoeficient * Data->Kandidati[k + Data->Zaciatok] + 1;
        
        if ((Kand & 0x7) == 3) { k++; continue; }
        if ((Kand & 0x7) == 5) { k++; continue; }
        if ((Kand % 3) == 0) { k++; continue; }
        if ((Kand % 5) == 0) { k++; continue; }
        if ((Kand % 7) == 0) { k++; continue; }
        if ((Kand % 11) == 0) { k++; continue; }
        if ((Kand % 13) == 0) { k++; continue; }
        if ((Kand % 17) == 0) { k++; continue; }
        if ((Kand % 19) == 0) { k++; continue; }

        //if (VypocetMOD2(Data->Kandidati[k + Data->Zaciatok], Kand) == 1) {
        if (asm_mod(Data->Kandidati[k + Data->Zaciatok], Kand) == 1) {
            if (k < Data->AktualnyPocet - 1) Data->Kandidati[k + Data->Zaciatok] = Data->Kandidati[Data->AktualnyPocet + Data->Zaciatok - 1];
            Data->AktualnyPocet--;
        } else k++;
    }

    pthread_exit(0);
    return NULL;
}
//  ...............................................................................................

//  ...............................................................................................
//  Spracovanie
//  ...............................................................................................
void TWorkerV1::Spracovanie(void) {

    if (FPocetVlakien == 1) { // Jednovlaknove spracovanie

        UINT64 ZacUkladaniaStavu = SpustenieMerania();
        while(FAktualnyKoeficient < FMaximalnyKoeficient) {
            if (FPoziadavkaNaUkoncenie) break;
            if ((FAktualnyKoeficient & 0x3) == 2) {  // Koeficient bez efektu?
                FAktualnyKoeficient++;
                continue;
            }
            
            UINT64 ZacMer = SpustenieMerania();
            UINT64 k = 0; UINT64 PovodnyPocet = FAktualnyPocetKandidatov;
            while(k < FAktualnyPocetKandidatov) {
                
                UINT64 Kand = 2 * FAktualnyKoeficient * FKandidati[k] + 1;
                
                if ((Kand & 0x7) == 3) { k++; continue; }
                if ((Kand & 0x7) == 5) { k++; continue; }
                if ((Kand % 3) == 0) { k++; continue; }
                if ((Kand % 5) == 0) { k++; continue; }
                if ((Kand % 7) == 0) { k++; continue; }
                if ((Kand % 11) == 0) { k++; continue; }
                if ((Kand % 13) == 0) { k++; continue; }
                if ((Kand % 17) == 0) { k++; continue; }
                if ((Kand % 19) == 0) { k++; continue; }

                //if (VypocetMOD(FKandidati[k], Kand) == 1) {
                if (asm_mod(FKandidati[k], Kand) == 1) {
                    if (k < FAktualnyPocetKandidatov - 1) FKandidati[k] = FKandidati[FAktualnyPocetKandidatov - 1];
                    FAktualnyPocetKandidatov--;
                } else k++;
            }
            if (PovodnyPocet != FAktualnyPocetKandidatov) {
                LogPrint("\nK = %llu, %llu -> ", FAktualnyKoeficient, PovodnyPocet);
                LogPrint("%llu (%.3lf sekund)", FAktualnyPocetKandidatov, ZastavenieMerania(ZacMer));
            }

            if (FUkladanieStavu) {
                if (ZastavenieMerania(ZacUkladaniaStavu) > FIntervalUkladaniaStavu) {
                    UlozenieStavu(false);
                    ZacUkladaniaStavu = SpustenieMerania();
                    LogPrint(" -> Ulozeny stav.");
                }
            }

            FAktualnyKoeficient++;
        }
        return;
    }

    THR_DATA Data[16]; pthread_t Vlakna[16];

    UINT64 ZacUkladaniaStavu = SpustenieMerania();
    UINT64 PoslednaZmenaMer = SpustenieMerania();
    while(FAktualnyKoeficient < FMaximalnyKoeficient) {
        if (FPoziadavkaNaUkoncenie) break;
        if ((FAktualnyKoeficient & 0x3) == 2) {  // Koeficient bez efektu?
            FAktualnyKoeficient++;
            continue;
        }
    
        UINT64 PovodnyPocet = FAktualnyPocetKandidatov;
        UINT64 ZacMer = SpustenieMerania();

        // Vyplnenie struktur
        Data[0].Kandidati = FKandidati; 
        Data[0].Zaciatok = 0; 
        Data[0].AktualnyKoeficient = FAktualnyKoeficient;
        Data[0].CelkovyPocet = Data[0].AktualnyPocet = FAktualnyPocetKandidatov / FPocetVlakien;

        for (INT32 i = 1; i < FPocetVlakien; i++) {
            Data[i].Kandidati = FKandidati;
            Data[i].Zaciatok = Data[i - 1].Zaciatok + Data[i - 1].CelkovyPocet;
            Data[i].CelkovyPocet = Data[i].AktualnyPocet = Data[i - 1].CelkovyPocet;
            Data[i].AktualnyKoeficient = FAktualnyKoeficient;
        }

        Data[FPocetVlakien - 1].Kandidati = FKandidati; 
        Data[FPocetVlakien - 1].Zaciatok = Data[FPocetVlakien - 2].Zaciatok + Data[FPocetVlakien - 2].CelkovyPocet; 
        Data[FPocetVlakien - 1].AktualnyKoeficient = FAktualnyKoeficient;
        Data[FPocetVlakien - 1].AktualnyPocet = Data[FPocetVlakien - 1].CelkovyPocet = FAktualnyPocetKandidatov - Data[FPocetVlakien - 1].Zaciatok;

        // Vytvorenie vlakien
        for (INT32 i = 0; i < FPocetVlakien; i++) {
            if (pthread_create(Vlakna + i, NULL, VlaknoSpracovanie, Data + i)) {
                LogPrint("\nChyba vytvorenia vlakna!!!");
                _exit(0);
            }
        }

        // Vykonanie vlakien
        for (INT32 i = 0; i < FPocetVlakien; i++) {
            if (pthread_join(Vlakna[i], NULL)) {
                LogPrint("\nChyba vlakna - join!!!");
                _exit(0);
            }
            //printf("\n%d\n", i);
            //if (pthread_detach(Vlakna[i])) {
                //LogPrint("\nChyba vlakna - detach!!!");
                //_exit(0);
            //}
        }

        FAktualnyPocetKandidatov -= Data[FPocetVlakien - 1].CelkovyPocet - Data[FPocetVlakien - 1].AktualnyPocet; // Znizim pocet podla posledneho
        for (INT32 i = FPocetVlakien - 2; i >= 0; i--) {
            UINT64 Medzera = Data[i].CelkovyPocet - Data[i].AktualnyPocet;
            memmove(Data[i].Kandidati + Data[i].Zaciatok + Data[i].AktualnyPocet, FKandidati + FAktualnyPocetKandidatov - Medzera, Medzera * sizeof(UINT64));
            FAktualnyPocetKandidatov -= Medzera;
        }

        // Test na koniec spracovania
        BOOL Spr = Data[0].Spracovane;
        for (INT32 i = 1; i < FPocetVlakien; i++) {
            Spr |= Data[i].Spracovane;
        }
        if (!Spr) break;

        if (PovodnyPocet != FAktualnyPocetKandidatov) {
            LogPrint("\nK = %llu, %llu -> ", FAktualnyKoeficient, PovodnyPocet);
            LogPrint("%llu (%.3lf sekund, %.3lf sekund od posl. zmeny)", FAktualnyPocetKandidatov, ZastavenieMerania(ZacMer), ZastavenieMerania(PoslednaZmenaMer));
            PoslednaZmenaMer = SpustenieMerania();
        }

        if (FUkladanieStavu) {
            if (ZastavenieMerania(ZacUkladaniaStavu) > FIntervalUkladaniaStavu) {
                UlozenieStavu(false);
                ZacUkladaniaStavu = SpustenieMerania();
                LogPrint(" -> Ulozeny stav.");
            }
        }

        FAktualnyKoeficient++;
    }

}
//  ...............................................................................................
//  ...............................................................................................
//  Vypocet mod
//  ...............................................................................................
UINT64 TWorkerV1::VypocetMOD(UINT64 iExponent, UINT64 iMod) {
    UINT128 S = 2; UINT64 M = MOD_MASKA; UINT128 Mod = iMod;
    while (M > 0) {
        if (M & iExponent) S = (S * S * 2) % Mod;
        else S = (S * S) % Mod;
        M >>= 1;
    }
    return (UINT64)S;
}
//  ...............................................................................................
