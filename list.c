#include "header.h"

void createEmpty(List *L) {
    (*L).FIRST = NULL;
}

addressList alokasi(char judul[], int stok) {
    addressList P = (addressList)malloc(sizeof(ElmtList));
    if (P != NULL) {
        strcpy(P->judul, judul);
        P->stok = stok;
        createEmptyQueue(&(P->antrianPeminjam));
        P->next = NULL;
    }
    return P;
}

void insertFirst(List *L, addressList P) {
    P->next = (*L).FIRST;
    (*L).FIRST = P;
}

void deleteFirst(List *L, addressList *P) {
    if ((*L).FIRST != NULL) {
        *P = (*L).FIRST;
        (*L).FIRST = (*P)->next;
        (*P)->next = NULL;
    } else {
        *P = NULL;
    }
}

addressList findElm(List L, char judul[]) {
    addressList P = L.FIRST;
    while (P != NULL) {
        if (strcmp(P->judul, judul) == 0) {
            return P;
        }
        P = P->next;
    }
    return NULL;
}

void printInfo(List L) {
    addressList P = L.FIRST;
    if (P == NULL) {
        printf("List kosong\n");
    } else {
        printf("\n=== DAFTAR BUKU ===\n");
        printf("%-30s | %s\n", "Judul Buku", "Stok");
        printf("-----------------------------------\n");
        while (P != NULL) {
            printf("%-30s | %d\n", P->judul, P->stok);
            P = P->next;
        }
        printf("\n");
    }
}
