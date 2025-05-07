#include "header.h"
// Fungsi untuk membersihkan layar konsol
void clearScreen() {
    #ifdef _WIN32
        system("cls");  // Untuk Windows
    #else
        system("clear"); // Untuk Linux/Unix/macOS
    #endif
}

// Implementasi fungsi untuk operasi perpustakaan
void tambahBuku(List *L, char judul[], int stok) {
    addressList P = findElm(*L, judul);
    
    if (P == NULL) {
        // Jika buku belum ada, tambahkan buku baru
        P = alokasi(judul, stok);
        insertFirst(L, P);
        printf("Buku '%s' berhasil ditambahkan dengan stok %d.\n", judul, stok);
    } else {
        // Jika buku sudah ada, tambahkan stok
        P->stok += stok;
        printf("Stok buku '%s' ditambahkan. Stok sekarang: %d\n", judul, P->stok);
    }
}

void tambahStok(List *L, char judul[], int tambahan) {
    addressList P = findElm(*L, judul);
    
    if (P != NULL) {
        P->stok += tambahan;
        printf("Stok buku '%s' ditambahkan. Stok sekarang: %d\n", judul, P->stok);
    } else {
        printf("Buku '%s' tidak ditemukan.\n", judul);
    }
}

void pinjamBuku(List *L, Stack *S, char judul[], char nama[], int priority) {
    addressList P = findElm(*L, judul);
    
    if (P != NULL) {
        addressQueue Peminjam = alokasiQueue(nama, priority);
        
        // Masukkan ke antrian peminjam
        enqueue(&(P->antrianPeminjam), Peminjam);
        printf("'%s' telah ditambahkan ke dalam antrian peminjam buku '%s'.\n", nama, judul);
        
        // Tambahkan ke stack aktivitas
        Aktivitas aktivitas;
        strcpy(aktivitas.namaPeminjam, nama);
        strcpy(aktivitas.judulBuku, judul);
        aktivitas.tipePriority = priority;
        aktivitas.tipeAktivitas = 1; // 1 = Pinjam
        
        addressStack As = alokasiStack(aktivitas);
        push(S, As);
        
        // Jika stok buku > 0, langsung proses peminjaman
        if (P->stok > 0) {
            printf("Stok buku '%s' tersedia. Proses peminjaman dapat dilakukan.\n", judul);
        } else {
            printf("Stok buku '%s' kosong. Masuk dalam antrian peminjaman.\n", judul);
        }
    } else {
        printf("Buku '%s' tidak ditemukan.\n", judul);
    }
}

void prosesPeminjaman(List *L, Stack *S, char judul[]) {
    addressList P = findElm(*L, judul);
    
    if (P != NULL) {
        if (P->stok > 0 && !isEmptyQueue(P->antrianPeminjam)) {
            addressQueue Peminjam;
            dequeue(&(P->antrianPeminjam), &Peminjam);
            
            P->stok--;
            
            printf("Peminjaman buku '%s' oleh '%s' berhasil diproses.\n", 
                   judul, Peminjam->nama);
            
            // Tambahkan ke stack aktivitas
            Aktivitas aktivitas;
            strcpy(aktivitas.namaPeminjam, Peminjam->nama);
            strcpy(aktivitas.judulBuku, judul);
            aktivitas.tipePriority = Peminjam->priority;
            aktivitas.tipeAktivitas = 1; // 1 = Pinjam
            
            addressStack As = alokasiStack(aktivitas);
            push(S, As);
            
            free(Peminjam);
        } else if (P->stok <= 0) {
            printf("Stok buku '%s' kosong. Tidak dapat memproses peminjaman.\n", judul);
        } else {
            printf("Tidak ada antrian peminjam untuk buku '%s'.\n", judul);
        }
    } else {
        printf("Buku '%s' tidak ditemukan.\n", judul);
    }
}

void kembalikanBuku(List *L, Stack *S, char judul[], char nama[]) {
    addressList P = findElm(*L, judul);
    
    if (P != NULL) {
        P->stok++;
        printf("Buku '%s' berhasil dikembalikan oleh '%s'. Stok sekarang: %d\n", 
               judul, nama, P->stok);
        
        // Tambahkan ke stack aktivitas
        Aktivitas aktivitas;
        strcpy(aktivitas.namaPeminjam, nama);
        strcpy(aktivitas.judulBuku, judul);
        aktivitas.tipePriority = 0; // tidak perlu prioritas untuk pengembalian
        aktivitas.tipeAktivitas = 2; // 2 = Kembalikan
        
        addressStack As = alokasiStack(aktivitas);
        push(S, As);
        
        // Cek apakah ada antrian peminjam
        if (!isEmptyQueue(P->antrianPeminjam)) {
            printf("Ada antrian peminjam untuk buku '%s'. Siap untuk diproses.\n", judul);
            printInfoQueue(P->antrianPeminjam);
        }
    } else {
        printf("Buku '%s' tidak ditemukan.\n", judul);
    }
}

void batalkanAktivitas(List *L, Stack *S) {
    if (!isEmptyStack(*S)) {
        addressStack P;
        pop(S, &P);
        
        if (P->info.tipeAktivitas == 1) { // Pembatalan peminjaman
            addressList bookP = findElm(*L, P->info.judulBuku);
            
            if (bookP != NULL) {
                // Cari peminjam di antrian dan hapus
                addressQueue current = bookP->antrianPeminjam.HEAD;
                addressQueue prev = NULL;
                int found = 0;
                
                while (current != NULL && !found) {
                    if (strcmp(current->nama, P->info.namaPeminjam) == 0) {
                        found = 1;
                        
                        // Hapus dari antrian
                        if (prev == NULL) { // head
                            bookP->antrianPeminjam.HEAD = current->next;
                            if (bookP->antrianPeminjam.HEAD == NULL) {
                                bookP->antrianPeminjam.TAIL = NULL;
                            }
                        } else if (current->next == NULL) { // tail
                            prev->next = NULL;
                            bookP->antrianPeminjam.TAIL = prev;
                        } else { // middle
                            prev->next = current->next;
                        }
                        
                        free(current);
                    } else {
                        prev = current;
                        current = current->next;
                    }
                }
                
                printf("Peminjaman buku '%s' oleh '%s' dibatalkan.\n", 
                       P->info.judulBuku, P->info.namaPeminjam);
            }
        } else if (P->info.tipeAktivitas == 2) { // Pembatalan pengembalian
            addressList bookP = findElm(*L, P->info.judulBuku);
            
            if (bookP != NULL) {
                bookP->stok--;
                printf("Pengembalian buku '%s' oleh '%s' dibatalkan. Stok sekarang: %d\n", 
                       P->info.judulBuku, P->info.namaPeminjam, bookP->stok);
            }
        }
        
        // Tambahkan ke stack aktivitas pembatalan
        Aktivitas aktivitas;
        strcpy(aktivitas.namaPeminjam, P->info.namaPeminjam);
        strcpy(aktivitas.judulBuku, P->info.judulBuku);
        aktivitas.tipePriority = P->info.tipePriority;
        aktivitas.tipeAktivitas = 3; // 3 = Batalkan
        
        addressStack As = alokasiStack(aktivitas);
        push(S, As);
        
        free(P);
    } else {
        printf("Tidak ada aktivitas yang dapat dibatalkan.\n");
    }
}

int main() {
    List daftarBuku;
    Stack historyAktivitas;
    int pilihan, stok, priority;
    char judul[50], nama[50];
    
    createEmpty(&daftarBuku);
    createEmptyStack(&historyAktivitas);
    
    do {
        printf("\n=============================================\n");
        printf("    SISTEM PEMINJAMAN BUKU PERPUSTAKAAN\n");
        printf("=============================================\n");
        printf("1. Tambah Buku\n");
        printf("2. Tambah Stok\n");
        printf("3. Pinjam Buku\n");
        printf("4. Proses Peminjaman\n");
        printf("5. Kembalikan Buku\n");
        printf("6. Batalkan Aktivitas Terakhir\n");
        printf("7. Lihat Data Buku\n");
        printf("8. Lihat History Aktivitas\n");
        printf("0. Keluar\n");
        printf("Pilihan Anda: ");
        scanf("%d", &pilihan);
        getchar(); // clear buffer
        
        switch (pilihan) {
            case 1:
                clearScreen();
                printf("\n--- TAMBAH BUKU ---\n");
                printf("Judul buku: ");
                fgets(judul, 50, stdin);
                judul[strcspn(judul, "\n")] = 0; // hapus newline
                
                printf("Stok: ");
                scanf("%d", &stok);
                getchar(); // clear buffer
                
                tambahBuku(&daftarBuku, judul, stok);
                break;
                
            case 2:
                clearScreen();
                printf("\n--- TAMBAH STOK ---\n");
                printf("Judul buku: ");
                fgets(judul, 50, stdin);
                judul[strcspn(judul, "\n")] = 0; // hapus newline
                
                printf("Tambahan stok: ");
                scanf("%d", &stok);
                getchar(); // clear buffer
                
                tambahStok(&daftarBuku, judul, stok);
                break;
                
            case 3:
                clearScreen();
                printf("\n--- PINJAM BUKU ---\n");
                printf("Judul buku: ");
                fgets(judul, 50, stdin);
                judul[strcspn(judul, "\n")] = 0; // hapus newline
                
                printf("Nama peminjam: ");
                fgets(nama, 50, stdin);
                nama[strcspn(nama, "\n")] = 0; // hapus newline
                
                printf("Prioritas (1: Masyarakat Umum, 2: Mahasiswa, 3: Dosen): ");
                scanf("%d", &priority);
                getchar(); // clear buffer
                
                pinjamBuku(&daftarBuku, &historyAktivitas, judul, nama, priority);
                break;
                
            case 4:
                clearScreen();
                printf("\n--- PROSES PEMINJAMAN ---\n");
                printf("Judul buku yang akan diproses: ");
                fgets(judul, 50, stdin);
                judul[strcspn(judul, "\n")] = 0; // hapus newline
                
                prosesPeminjaman(&daftarBuku, &historyAktivitas, judul);
                break;
                
            case 5:
                clearScreen();
                printf("\n--- KEMBALIKAN BUKU ---\n");
                printf("Judul buku: ");
                fgets(judul, 50, stdin);
                judul[strcspn(judul, "\n")] = 0; // hapus newline
                
                printf("Nama peminjam: ");
                fgets(nama, 50, stdin);
                nama[strcspn(nama, "\n")] = 0; // hapus newline
                
                kembalikanBuku(&daftarBuku, &historyAktivitas, judul, nama);
                break;
                
            case 6:
                clearScreen();
                printf("\n--- BATALKAN AKTIVITAS TERAKHIR ---\n");
                batalkanAktivitas(&daftarBuku, &historyAktivitas);
                break;
                
            case 7:
                clearScreen();
                printInfo(daftarBuku);
                break;
                
            case 8:
                clearScreen();
                printInfoStack(historyAktivitas);
                break;
                
            case 0:
                clearScreen();
                printf("Terima kasih telah menggunakan program ini.\n");
                break;
                
            default:
                printf("Pilihan tidak valid.\n");
        }
    } while (pilihan != 0);
    
    return 0;
}
