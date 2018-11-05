
#include "mersenne_elim.h"
#include "mersenne_elim_v1.h"

#define MIN_MERSENNE_PRVOCISLO 332192831
#define MAX_MERSENNE_PRVOCISLO 536870909

#define MOD_MASKA 268435456

TWorker *Worker; // Objekt workera

//  ...............................................................................................
//  ...............................................................................................
void SignalHandler(INT32 iSignal) {
    Worker->ZastavenieSpracovania();
}
//  ...............................................................................................

//  ...............................................................................................
//  ...............................................................................................
INT32 main(INT32 iPocetArgumentov, PCHAR *iArgumenty) {

    Worker = new TWorkerV1();

    // Inicializacia objektu a prostredia
    if (!Worker->Inicializacia(iPocetArgumentov, iArgumenty)) {
        printf("\nKRITICKA CHYBA!!! Nie je mozne inicializovat aplikaciu.");
        printf("\n");
        delete Worker;
        return -1;
    }

    Worker->ZobrazenieInfo(); // Zobrazenie info

    if (!Worker->ParameterExistuje("-inicializacia") && (Worker->JeUlozenyStav())) { // Je ulozeny stav?
        if (!Worker->NacitanieUlozenehoStavu()) { // Nacitanie ulozeneho stavu
            printf("\n");
            delete Worker;
            return -2;
        }
    } else {
        if (!Worker->NacitanieZaciatocnehoStavu()) { // Inicializacia stavu
            printf("\n");
            delete Worker;
            return -3;
        }
    }

    signal(SIGINT, SignalHandler); // Nastavenie handlera

    Worker->SpustenieSpracovania(); // Spustenie spracovania
    
    Worker->UlozenieStavu(true); // Ulozenie stavu
    Worker->ZobrazenieVysledkovSpracovania(); // Zobrazenie vysledok spracovania

    delete Worker; // Zrusenie workera
    printf("\n");
    return 0;
}
//  ...............................................................................................
