#pragma once

//  ...............................................................................................
//  Trieda TWorkerV1
//  ...............................................................................................
class TWorkerV1 : public TWorker {
private:
    UINT64 FPociatocnyPocetKandidatov;
    UINT64 FAktualnyPocetKandidatov;
    PUINT64 FKandidati; 
    UINT64 FPociatocnyKoeficient;
    UINT64 FMaximalnyKoeficient;
    UINT64 FAktualnyKoeficient;
    INT32 FPocetVlakien;
private:
    UINT64 VypocetMOD(UINT64 iExponent, UINT64 iMod); // Vypocet mod
public:
    TWorkerV1(void); // Konstruktor
    virtual ~TWorkerV1(void); // Destruktor
protected:
    virtual void Uzatvorenie(void); // Uzatvorenie
    virtual BOOL Inicializacia(void); // Inicializacia objektu
    virtual void Spracovanie(void); // Spracovanie
public:
    virtual void ZobrazenieInfo(void); // Zobrazenie info
    virtual void ZobrazenieVysledkovSpracovania(void); // Zobrazenie vysledkov spracovania
public:
    virtual BOOL NacitanieZaciatocnehoStavu(void); // Nacitanie zaciatocneho stavu
    virtual BOOL NacitanieUlozenehoStavu(void); // Nacitanie ulozeneho stavu
    virtual BOOL JeUlozenyStav(void); // Ziskanie priznaku ulozeneho stavu
    virtual BOOL UlozenieStavu(BOOL iZobrazenieInfo = false); // Ulozenie stavu
};
//  ...............................................................................................
