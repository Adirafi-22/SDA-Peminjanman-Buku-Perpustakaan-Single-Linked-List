#include "header.h"

void createEmptyQueue(Queue *Q) {
    (*Q).HEAD = NULL;
    (*Q).TAIL = NULL;
}

addressQueue alokasiQueue(char nama[], int priority) {
    addressQueue P = (addressQueue)malloc(sizeof(ElmtQueue));
    if (P != NULL) {
        strcpy(P->nama, nama);
        P->priority = priority;
        P->next = NULL;
    }
    return P;
}

int isEmptyQueue(Queue Q) {
    return (Q.HEAD == NULL && Q.TAIL == NULL);
}

// Enqueue dengan prioritas
void enqueue(Queue *Q, addressQueue P) {
    if (isEmptyQueue(*Q)) {
        // Jika queue kosong, elemen menjadi HEAD dan TAIL
        (*Q).HEAD = P;
        (*Q).TAIL = P;
    } else if (P->priority > (*Q).HEAD->priority) {
        // Jika prioritas lebih tinggi dari HEAD, masukkan di depan
        P->next = (*Q).HEAD;
        (*Q).HEAD = P;
    } else {
        // Cari posisi yang tepat berdasarkan prioritas
        addressQueue curr = (*Q).HEAD;
        addressQueue prev = NULL;
        
        while (curr != NULL && P->priority <= curr->priority) {
            prev = curr;
            curr = curr->next;
        }
        
        // Sisipkan elemen
        if (curr == NULL) { // di akhir
            prev->next = P;
            (*Q).TAIL = P;
        } else { // di tengah
            P->next = curr;
            prev->next = P;
        }
    }
}

void dequeue(Queue *Q, addressQueue *P) {
    if (!isEmptyQueue(*Q)) {
        *P = (*Q).HEAD;
        
        if ((*Q).HEAD == (*Q).TAIL) {
            // Jika hanya ada satu elemen
            (*Q).HEAD = NULL;
            (*Q).TAIL = NULL;
        } else {
            // Jika ada lebih dari satu elemen
            (*Q).HEAD = (*Q).HEAD->next;
        }
        
        (*P)->next = NULL;
    } else {
        *P = NULL;
    }
}

void printInfoQueue(Queue Q) {
    addressQueue P = Q.HEAD;
    
    if (isEmptyQueue(Q)) {
        printf("Antrian kosong\n");
    } else {
        printf("\n=== ANTRIAN PEMINJAM ===\n");
        printf("%-20s | %s\n", "Nama", "Prioritas");
        printf("-----------------------------------\n");
        
        while (P != NULL) {
            char prioritasStr[20];
            switch (P->priority) {
                case DOSEN:
                    strcpy(prioritasStr, "Dosen");
                    break;
                case MAHASISWA:
                    strcpy(prioritasStr, "Mahasiswa");
                    break;
                case MASYARAKAT_UMUM:
                    strcpy(prioritasStr, "Masyarakat Umum");
                    break;
                default:
                    strcpy(prioritasStr, "Unknown");
            }
            
            printf("%-20s | %s\n", P->nama, prioritasStr);
            P = P->next;
        }
        printf("\n");
    }
}
